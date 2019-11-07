/*
 * TxtVacuumGripperRobotRun.cpp
 *
 *  Created on: 08.11.2018
 *      Author: steiger-a
 */

#ifndef __DOCFSM__
#include "TxtVacuumGripperRobot.h"

#include "Utils.h"
#endif

#ifdef FSM_INIT_FSM
 #undef FSM_INIT_FSM
#endif
#define FSM_INIT_FSM( startState, attr... )                                \
		currentState = startState;                                         \
		newState = startState;

#ifdef FSM_TRANSITION
 #undef FSM_TRANSITION
#endif
#ifdef _DEBUG
 #define FSM_TRANSITION( _newState, attr... )                              \
		do                                                                 \
		{                                                                  \
			std::cerr << state2str( currentState ) << " -> "               \
			<< state2str( _newState ) << std::endl;                        \
			newState = _newState;                                          \
		}                                                                  \
		while( false )
#else
 #define FSM_TRANSITION( _newState, attr... )  newState = _newState
#endif


namespace ft {


void TxtVacuumGripperRobot::fsmStep()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "fsmStep",0);

	// Entry activities ===================================================
	if( newState != currentState )
	{
		//update order view (only if state changed)
		if (reqOrder)
		{
			ord_state.type = reqWP_order.type;
			ord_state.state = ORDERED;
			assert(mqttclient);
			mqttclient->publishStateOrder(ord_state, TIMEOUT_MS_PUBLISH);
		}

		switch( newState )
		{
		//-----------------------------------------------------------------
		case FAULT:
		{
			printEntryState(FAULT);
			setStatus(SM_ERROR);
			sound.error();
			release();
			break;
		}
		//-----------------------------------------------------------------
		case IDLE:
		{
			printEntryState(IDLE);
			dps.Notify();
			release();
			setSpeed(512);
			moveRef();
			setActStatus(false, SM_READY);
			dps.setActiveDSI(false);
			dps.setActiveDSI(false);
			break;
		}
		//-----------------------------------------------------------------
		case CALIB_VGR:
		{
			printEntryState(CALIB_VGR);
			sound.info2();
			moveRef();
			break;
		}
		//-----------------------------------------------------------------
		case START_DELIVERY:
		{
			printEntryState(START_DELIVERY);
			dps.setErrorDSI(false);
			dps.setActiveDSI(true);
			break;
		}
		//-----------------------------------------------------------------
		default:
			break;
		}
		currentState = newState;
	}

	// Do activities ==================================================
	switch( currentState )
	{
	//-----------------------------------------------------------------
	case FAULT:
	{
		printState(FAULT);
		if (reqQuit)
		{
			setStatus(SM_READY);
			FSM_TRANSITION( IDLE, color=green, label='req\nquit' );
			reqQuit = false;
		}
#ifdef __DOCFSM__
		FSM_TRANSITION( FAULT, color=red, label='wait' );
#endif
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		break;
	}
	//-----------------------------------------------------------------
	case INIT:
	{
#ifdef __DOCFSM__ //TODO remove, needed for graph
		FSM_TRANSITION( INIT, color=blue, label='wait' );
#endif
		printState(INIT);
		moveRef();
		FSM_TRANSITION( IDLE, color=green, label='initialized' );
		break;
	}
	//-----------------------------------------------------------------
	case IDLE:
	{
		//printState(IDLE);

		//NFC requests
		if (reqNfcDelete)
		{
			if (dps.nfcDelete())
			{
				dps.publishNfc();
				reqNfcDelete = false;
			}
		}
		if (reqNfcRead)
		{
			if (dps.nfcRead().empty())
			{
				dps.publishNfc();
				reqNfcRead = false;
			}
		}

		if (reqSLDsorted)
		{
			setTarget("dso");
			reqWP_SLD.printDebug();
			if (reqWP_SLD.type == WP_TYPE_WHITE)
			{
				moveSSD1();
			}
			else if (reqWP_SLD.type == WP_TYPE_RED)
			{
				moveSSD2();
			}
			else if (reqWP_SLD.type == WP_TYPE_BLUE)
			{
				moveSSD3();
			} else {
				FSM_TRANSITION( FAULT, color=red, label='nfc error' );
				break;
			}
			if (reqWP_SLD.type != WP_TYPE_NONE)
			{
				FSM_TRANSITION( NFC_PRODUCED, color=blue, label='req sorted' );
			}
			reqSLDsorted = false;
		}
		else if (reqOrder)
		{
			ord_state.type = reqWP_order.type;
			ord_state.state = ORDERED;
			assert(mqttclient);
			mqttclient->publishStateOrder(ord_state, TIMEOUT_MS_PUBLISH);

			FSM_TRANSITION( FETCH_WP_VGR, color=blue, label='req order' );
			reqOrder = false;
		}
		else if (!dps.is_DIN())
		{
			FSM_TRANSITION( START_DELIVERY, color=blue, label='dsi' );
		}
		/*else if (joyData.aY2 < -500)
		{
			//TODO local demo mode
		}*/
		else if (joyData.aX2 > 500)
		{
			if (ord_state.state == WAITING_FOR_ORDER)
			{
				reqWP_order.type = WP_TYPE_WHITE;
				reqOrder = true;
			}
		}
		else if (joyData.aY2 > 500)
		{
			if (ord_state.state == WAITING_FOR_ORDER)
			{
				reqWP_order.type = WP_TYPE_RED;
				reqOrder = true;
			}
		}
		else if (joyData.aX2 < -500)
		{
			if (ord_state.state == WAITING_FOR_ORDER)
			{
				reqWP_order.type = WP_TYPE_BLUE;
				reqOrder = true;
			}
		}
		else
		{
			std::string uid = dps.nfcReadUID();
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "uid: {}",uid);
			if (uid.length()==8) //command uid has length=8
			{
				SPDLOG_LOGGER_TRACE(spdlog::get("console"), "tag_uid: {}", uid);
				//set action uids if calibData is empty
				if (joyData.aX1 > 500 || dps.getUIDResetHBW().empty())
				{
					dps.saveUIDResetHBW(uid);
				}
				else if (((joyData.aX1 < -500) || dps.getUIDResetHBW()!=uid) && (dps.getUIDCalibMode().empty()))
				{
					dps.saveUIDCalibMode(uid);
				}
				//check uids
				if (uid == dps.getUIDResetHBW())
				{
					assert(mqttclient);
					mqttclient->publishVGR_Do(VGR_HBW_RESETSTORAGE, 0, TIMEOUT_MS_PUBLISH);
					mqttclient->publishStateVGR(ft::LEDS_WAIT_READY, "", TIMEOUT_MS_PUBLISH, 0, "");
					std::this_thread::sleep_for(std::chrono::milliseconds(2000));
					mqttclient->publishStateVGR(ft::LEDS_READY, "", TIMEOUT_MS_PUBLISH, 0, "");
				}
				else if (uid == dps.getUIDCalibMode())
				{
					FSM_TRANSITION( CALIB_VGR, color=orange, label='cmd calib' );
				}
				/*else if (uid == dps.getUIDOrderWHITE())*/
				/*else if (uid == dps.getUIDOrderRED())*/
				/*else if (uid == dps.getUIDOrderBLUE())*/
			}
		}
#ifdef __DOCFSM__
		FSM_TRANSITION( IDLE, color=green, label='wait' );
#endif
		break;
	}
	//-----------------------------------------------------------------
	case FETCH_WP_VGR:
	{
		printState(FETCH_WP_VGR);

		assert(mqttclient);
		reqWP_order.printDebug();
		mqttclient->publishVGR_Do(VGR_HBW_FETCH_WP, &reqWP_order, TIMEOUT_MS_PUBLISH);

		setTarget("hbw");
		moveFromHBW1();

		FSM_TRANSITION( VGR_WAIT_FETCHED, color=green, label='fetched' );
		break;
	}
	//-----------------------------------------------------------------
	case VGR_WAIT_FETCHED:
	{
		printState(VGR_WAIT_FETCHED);

		if (reqHBWfetched)
		{
			moveFromHBW2();

			reqWP_MPO = reqWP_HBW;

			assert(mqttclient);
			mqttclient->publishVGR_Do(VGR_HBW_STORECONTAINER, reqWP_MPO, TIMEOUT_MS_PUBLISH);

			assert(reqWP_MPO);
			proStorage.setTimestampNow(reqWP_MPO->tag_uid, OUTSOURCING_INDEX);

			reqHBWfetched = false;
			FSM_TRANSITION( MOVE_VGR2MPO, color=green, label='transport to MPO' );
		}
#ifdef __DOCFSM__
		FSM_TRANSITION( VGR_WAIT_FETCHED, color=blue, label='wait' );
#endif
		break;
	}
	//-----------------------------------------------------------------
	case MOVE_VGR2MPO:
	{
		printState(MOVE_VGR2MPO);
		setTarget("mpo");
		moveMPO();

		assert(reqWP_MPO);
		reqWP_MPO->printDebug();

		ord_state.type = reqWP_MPO->type;
		ord_state.state = IN_PROCESS;
		assert(mqttclient);
		mqttclient->publishStateOrder(ord_state, TIMEOUT_MS_PUBLISH);

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		assert(mqttclient);
		mqttclient->publishVGR_Do(VGR_MPO_PRODUCE, reqWP_MPO, TIMEOUT_MS_PUBLISH);
		proStorage.setTimestampNow(reqWP_MPO->tag_uid, PROCESSING_OVEN_INDEX);

		moveRef();

		FSM_TRANSITION( START_PRODUCE, color=blue, label='produce' );
		break;
	}
	//-----------------------------------------------------------------
	case START_PRODUCE:
	{
		printState(START_PRODUCE);
		if (reqMPOstarted)
		{
			//TODO type is null,  repeat publish state
			//ord_state.type = reqWP_MPO->type;
			//ord_state.state = IN_PROCESS;
			//assert(mqttclient);
			//mqttclient->publishStateOrder(ord_state, TIMEOUT_MS_PUBLISH);

			FSM_TRANSITION( IDLE, color=green, label='transport to MPO' );
			reqMPOstarted = false;
		}
#ifdef __DOCFSM__
		FSM_TRANSITION( START_PRODUCE, color=blue, label='wait' );
#endif
		break;
	}
	//-----------------------------------------------------------------
	case START_DELIVERY:
	{
		printState(START_DELIVERY);

		setTarget("hbw");
		moveDeliveryInAndGrip();
		moveNFC();
		std::string uid = dps.nfcReadUID();
		if (uid.empty())
		{
			FSM_TRANSITION( WRONG_COLOR, color=red, label='empty tag' );
			break;
		} else {
			proStorage.resetTagUidMaskTs(uid);
			proStorage.setTimestampNow(uid, DELIVERY_RAW_INDEX);
			if (reqWP_HBW != 0) {
				reqWP_HBW->printDebug();
				delete reqWP_HBW;
			}
			reqWP_HBW = new TxtWorkpiece(uid,WP_TYPE_NONE,WP_STATE_RAW);
			reqWP_HBW->printDebug();
			FSM_TRANSITION( COLOR_DETECTION, color=blue, label='nfc tag ok' );
		}
#ifdef __DOCFSM__
		FSM_TRANSITION( START_DELIVERY, color=blue, label='wait' );
#endif
		break;
	}
	//-----------------------------------------------------------------
	case COLOR_DETECTION:
	{
		printState(COLOR_DETECTION);
		moveColorSensor();
		dps.readColorValue();
		assert(reqWP_HBW);
		reqWP_HBW->printDebug();
		proStorage.setTimestampNow(reqWP_HBW->tag_uid, INSPECTION_INDEX);
		if ((dps.getLastColor() != WP_TYPE_NONE) /*&&
			(hbw->canColorBeStored(dps.getLastColor()))*/)
		{
			reqWP_HBW->type = dps.getLastColor();
			reqWP_HBW->printDebug();
			FSM_TRANSITION( NFC_RAW, color=blue, label='color ok' );
		}
		else
		{
			FSM_TRANSITION( NFC_REJECTED, color=blue, label='color wrong' );
			break;
		}
#ifdef __DOCFSM__
		FSM_TRANSITION( COLOR_DETECTION, color=blue, label='wait' );
#endif
		break;
	}
	//-----------------------------------------------------------------
	case NFC_RAW:
	{
		printState(NFC_RAW);
		moveNFC();
		std::string uid = dps.nfcReadUID();
		if (uid.empty())
		{
			FSM_TRANSITION( WRONG_COLOR, color=red, label='nfc error' );
			break;
		}
		std::vector<int64_t> vts = proStorage.getTagUidVts(uid);
		uint8_t mask_ts = proStorage.getTagUidMaskTs(uid);
		std::string tag_uid = dps.nfcDeviceDeleteWriteRawRead(reqWP_HBW->type, vts, mask_ts);
		if (tag_uid.empty())
		{
			FSM_TRANSITION( FAULT, color=red, label='nfc error' );
			break;
		}

		FSM_TRANSITION( STORE_WP_VGR, color=blue, label='nfc write ok' );
		break;
	}
	//-----------------------------------------------------------------
	case NFC_REJECTED:
	{
		printState(NFC_REJECTED);
		moveRefYNFC();
		std::string uid = dps.nfcReadUID();
		if (!uid.empty())
		{
			proStorage.resetTagUidMaskTs(uid);
			std::vector<int64_t> vts = proStorage.getTagUidVts(uid);
			uint8_t mask_ts = proStorage.getTagUidMaskTs(uid);
			std::string tag_uid = dps.nfcDeviceWriteRejectedRead(ft::WP_TYPE_NONE, vts, mask_ts);
			if (tag_uid.empty())
			{
				FSM_TRANSITION( FAULT, color=red, label='nfc error' );
				break;
			}
		}
		FSM_TRANSITION( WRONG_COLOR, color=blue, label='wrong color');
		break;
	}
	//-----------------------------------------------------------------
	case WRONG_COLOR:
	{
		printState(WRONG_COLOR);
		dps.setErrorDSI(true);
		moveWrongRelease();
		sound.warn();
		FSM_TRANSITION( IDLE, color=green, label='next' );
		break;
	}
	//-----------------------------------------------------------------
	case NFC_PRODUCED:
	{
		printState(NFC_PRODUCED);
		proStorage.setTimestampNow(reqWP_SLD.tag_uid, SORTING_INDEX);
		grip();
		dps.setActiveDSO(true);
		moveRefYNFC();
		std::string uid = dps.nfcReadUID();
		if (uid.empty())
		{
			FSM_TRANSITION( FAULT, color=red, label='nfc error' );
			break;
		}
		reqWP_SLD.tag_uid = uid;
		reqWP_SLD.printDebug();
		proStorage.setTimestampNow(reqWP_SLD.tag_uid, SHIPPING_INDEX);
		std::vector<int64_t> vts = proStorage.getTagUidVts(reqWP_SLD.tag_uid);
		uint8_t mask_ts = proStorage.getTagUidMaskTs(reqWP_SLD.tag_uid);
		std::string tag_uid = dps.nfcDeviceWriteProducedRead(reqWP_SLD.type, vts,mask_ts);
		if (tag_uid.empty())
		{
			FSM_TRANSITION( FAULT, color=red, label='nfc error' );
			break;
		}
		FSM_TRANSITION( MOVE_PICKUP_WAIT, color=blue, label='nfc ok' );
		break;
	}
	//-----------------------------------------------------------------
	case MOVE_PICKUP_WAIT:
	{
		printState(MOVE_PICKUP_WAIT);
		if (dps.is_DOUT()) //dout empty
		{
			dps.setErrorDSO(false);

			moveDeliveryOutAndRelease();

			ord_state.type = reqWP_order.type;
			ord_state.state = SHIPPED;
			assert(mqttclient);
			mqttclient->publishStateOrder(ord_state, TIMEOUT_MS_PUBLISH);
			std::this_thread::sleep_for(std::chrono::milliseconds(2000));

			FSM_TRANSITION( MOVE_PICKUP, color=blue, label='delivered' );
		}
		else //dout not empty
		{
			dps.setErrorDSO(true);
		}
#ifdef __DOCFSM__
		FSM_TRANSITION( MOVE_PICKUP_WAIT, color=blue, label='wait' );
#endif
		break;
	}
	//-----------------------------------------------------------------
	case MOVE_PICKUP:
	{
		printState(MOVE_PICKUP);
		dps.setActiveDSO(false);
		ord_state.type = reqWP_order.type;
		ord_state.state = WAITING_FOR_ORDER;
		assert(mqttclient);
		mqttclient->publishStateOrder(ord_state, TIMEOUT_MS_PUBLISH);
		mqttclient->publishStateOrder(ord_state, TIMEOUT_MS_PUBLISH); //2x workaround if message is lost
		FSM_TRANSITION( IDLE, color=blue, label='next' );
		break;
	}
	//-----------------------------------------------------------------
	case STORE_WP_VGR:
	{
		printState(STORE_WP_VGR);

		assert(mqttclient);
		mqttclient->publishVGR_Do(VGR_HBW_FETCHCONTAINER, reqWP_HBW, TIMEOUT_MS_PUBLISH);

		dps.setActiveDSI(false);
		if (dps.getLastColor() == WP_TYPE_NONE)
		{
			moveWrongRelease();
			FSM_TRANSITION( FAULT, color=red, label='wrong color' );
			break;
		}

		assert(reqWP_HBW);
		reqWP_HBW->printDebug();
		reqWP_HBW->type = dps.getLastColor();

		moveToHBW();
		FSM_TRANSITION( STORE_WP, color=blue, label='transport to HBW' );
		break;
	}
	//-----------------------------------------------------------------
	case STORE_WP:
	{
		printState(STORE_WP);

		if (reqHBWfetched)
		{
			release();
			assert(reqWP_HBW);
			reqWP_HBW->printDebug();
			proStorage.setTimestampNow(reqWP_HBW->tag_uid, WAREHOUSING_INDEX);

			assert(mqttclient);
			mqttclient->publishVGR_Do(VGR_HBW_STORE_WP, reqWP_HBW, TIMEOUT_MS_PUBLISH);

			moveRef();
			FSM_TRANSITION( IDLE, color=green, label='fetched' );
			reqHBWfetched = false;
		}
#ifdef __DOCFSM__
		FSM_TRANSITION( STORE_WP, color=blue, label='wait' );
#endif
		break;
	}
	//-----------------------------------------------------------------
	case CALIB_VGR:
	{
		printState(CALIB_VGR);
		setStatus(SM_CALIB);
		while(true)
		{
			if (joyData.aX2 > 500) {
				assert(mqttclient);
				mqttclient->publishVGR_Do(VGR_HBW_CALIB, 0, TIMEOUT_MS_PUBLISH);
				mqttclient->publishStateVGR(ft::LEDS_READY, "", TIMEOUT_MS_PUBLISH, 0, "");
				FSM_TRANSITION( CALIB_HBW, color=orange, label='init' );
				break;
			} else if (joyData.aX2 < -500) {
				calibPos = (TxtVgrCalibPos_t)-1;
				FSM_TRANSITION( CALIB_VGR_NAV, color=orange, label='init' );
				break;
			} else if (joyData.aY2 < -500) {
				FSM_TRANSITION( CALIB_DPS, color=orange, label='init' );
				break;
			} else if (joyData.aY2 > 500) {
				assert(mqttclient);
				mqttclient->publishVGR_Do(VGR_SLD_CALIB, 0, TIMEOUT_MS_PUBLISH);
				mqttclient->publishStateVGR(ft::LEDS_READY, "", TIMEOUT_MS_PUBLISH, 0, "");
				FSM_TRANSITION( CALIB_SLD, color=orange, label='init' );
				break;
			} else if (joyData.b1) {
				FSM_TRANSITION( IDLE, color=green, label='cancel' );
				break;
			} else if (joyData.b2) {
				FSM_TRANSITION( IDLE, color=green, label='cancel' );
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
#ifdef __DOCFSM__
		FSM_TRANSITION( CALIB_VGR, color=orange, label='wait' );
#endif
		break;
	}
	//-----------------------------------------------------------------
	case CALIB_HBW:
	{
		printState(CALIB_HBW);
		if (reqHBWcalib_end)
		{
			FSM_TRANSITION( IDLE, color=green, label='HBW calibrated' );
			reqHBWcalib_end = true;
			break;
		}
#ifdef __DOCFSM__
		FSM_TRANSITION( CALIB_HBW, color=orange, label='wait' );
#endif
		break;
	}
	//-----------------------------------------------------------------
	case CALIB_SLD:
	{
		printState(CALIB_SLD);
		if (reqSLDcalib_end)
		{
			FSM_TRANSITION( IDLE, color=green, label='SLD calibrated' );
			reqSLDcalib_end = true;
			break;
		}
#ifdef __DOCFSM__
		FSM_TRANSITION( CALIB_SLD, color=orange, label='wait' );
#endif
		break;
	}
	//-----------------------------------------------------------------
	case CALIB_DPS:
	{
		printState(CALIB_DPS);
		setStatus(SM_CALIB);
		moveRef();
		moveColorSensor(true);
		calibColorValues[0] = -1;
		calibColorValues[1] = -1;
		calibColorValues[2] = -1;
		calibColor=ft::TxtWPType_t::WP_TYPE_WHITE;
		FSM_TRANSITION( CALIB_DPS_NEXT, color=orange, label='next' );
		break;
	}
	//-----------------------------------------------------------------
	case CALIB_DPS_NEXT:
	{
		printState(CALIB_DPS_NEXT);
		bool exit_next = false;
		while(!exit_next)
		{
			setStatus(SM_CALIB);
			if (joyData.b1) {
				sound.info1();
				switch(calibColor)
				{
				case ft::TxtWPType_t::WP_TYPE_WHITE:
					calibColorValues[0] = dps.readColorValue();
					SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "value white: {}",calibColorValues[0]);

					std::this_thread::sleep_for(std::chrono::milliseconds(2000));
					calibColor=ft::TxtWPType_t::WP_TYPE_RED;

					break;
				case ft::TxtWPType_t::WP_TYPE_RED:
					calibColorValues[1] = dps.readColorValue();
					SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "value red: {}",calibColorValues[1]);

					std::this_thread::sleep_for(std::chrono::milliseconds(2000));
					calibColor=ft::TxtWPType_t::WP_TYPE_BLUE;

					break;
				case ft::TxtWPType_t::WP_TYPE_BLUE:
					calibColorValues[2] = dps.readColorValue();
					SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "value blue: {}",calibColorValues[2]);

					SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "w:{} r:{} b:{}",calibColorValues[0],calibColorValues[1],calibColorValues[2]);
					if ((calibColorValues[0] > 0)&&
						(calibColorValues[1] > 0)&&
						(calibColorValues[2] > 0))
					{
						dps.calibData.color_th[0] = (calibColorValues[0] + calibColorValues[1]) / 2;
						dps.calibData.color_th[1] = (calibColorValues[1] + calibColorValues[2]) / 2;
						SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "th1:{} th2:{}",dps.calibData.color_th[0],dps.calibData.color_th[1]);

						//check
						if ((calibColorValues[0] < calibColorValues[1])&&
							(calibColorValues[1] < calibColorValues[2])&&
							(calibColorValues[0] < calibColorValues[2])&&
							(dps.calibData.color_th[0] >= 200)&&
							(dps.calibData.color_th[1] < 2000))
						{
							dps.calibData.save();
						} else {
							sound.error();
						}
					} else {
						sound.error();
					}
					FSM_TRANSITION( IDLE, color=green, label='DPS calibrated' );
					exit_next = true;

					break;
				default:
					break;
				}
			} else if (joyData.b2) {
				sound.warn();
				FSM_TRANSITION( IDLE, color=green, label='cancel' );
				exit_next = true;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
#ifdef __DOCFSM__
		FSM_TRANSITION( CALIB_DPS_NEXT, color=orange, label='next color' );
#endif
		break;
	}
	//-----------------------------------------------------------------
	case CALIB_VGR_NAV:
	{
		printState(CALIB_VGR_NAV);
		bool reqmove = true;
		while(true)
		{
			if (joyData.b1) {
				break;
			} else if (joyData.b2) {
				FSM_TRANSITION( IDLE, color=green, label='cancel' );
				break;
			} else if (joyData.aX2 > 500) {
				calibPos=(TxtVgrCalibPos_t)(calibPos-1);
				std::cout << ft::toString(calibPos) << std::endl;
				reqmove = true;
			} else if (joyData.aX2 < -500) {
				calibPos=(TxtVgrCalibPos_t)(calibPos+1);
				std::cout << ft::toString(calibPos) << std::endl;
				reqmove = true;
			}
			//check pos valid
			if (calibPos < 0)
			{
				calibPos = (TxtVgrCalibPos_t)(VGRCALIB_END-1);
			} else if (calibPos >= VGRCALIB_END)
			{
				calibPos = (TxtVgrCalibPos_t)0;
			}
			//move pos
			if (reqmove)
			{
				moveCalibPos();
				setStatus(SM_CALIB);
				FSM_TRANSITION( CALIB_VGR_MOVE, color=orange, label='calib pos' );
				reqmove = false;
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
#ifdef __DOCFSM__
		FSM_TRANSITION( CALIB_VGR_NAV, color=orange, label='select pos' );
#endif
		break;
	}
	//-----------------------------------------------------------------
	case CALIB_VGR_MOVE:
	{
		printState(CALIB_VGR_MOVE);
		while(true)
		{
			if (joyData.b2) {
				break; //-> NAV
			} else if (joyData.b1) {
				EncPos3 p3 = getPos3();
				switch(calibPos)
				{
				case VGRCALIB_DSI:
					calibData.setPos3("DIN", p3);
					calibData.copyPos3X("DIN","DIN0");
					calibData.copyPos3Z("DIN","DIN0");
					calibData.save();
					break;
				case VGRCALIB_DCS:
					calibData.setPos3("DCS", p3);
					calibData.copyPos3X("DCS","DCS0");
					calibData.copyPos3Z("DCS","DCS0");
					calibData.save();
					break;
				case VGRCALIB_NFC:
					calibData.setPos3("DNFC", p3);
					calibData.copyPos3X("DNFC","DNFC0");
					calibData.copyPos3Z("DNFC","DNFC0");
					calibData.save();
					break;
				case VGRCALIB_WDC:
					calibData.setPos3("WDC", p3);
					calibData.copyPos3X("WDC","WDC0");
					calibData.save();
					break;
				case VGRCALIB_DSO:
					calibData.setPos3("DOUT", p3);
					calibData.copyPos3X("DOUT","DOUT0");
					calibData.save();
					break;
				case VGRCALIB_HBW:
					calibData.setPos3("HBW1", p3);
					calibData.copyPos3X("HBW1","HBW");
					calibData.copyPos3Z("HBW1","HBW");
					calibData.copyPos3X("HBW1","HBW0");
					calibData.save();
					break;
				case VGRCALIB_MPO:
					calibData.setPos3("MPO", p3);
					calibData.copyPos3X("MPO","MPO0");
					calibData.copyPos3Z("MPO","MPO0");
					calibData.save();
					break;
				case VGRCALIB_SL1:
					calibData.setPos3("SSD1", p3);
					calibData.copyPos3X("SSD1","SSD10");
					calibData.save();
					break;
				case VGRCALIB_SL2:
					calibData.setPos3("SSD2", p3);
					calibData.copyPos3X("SSD2","SSD20");
					calibData.save();
					break;
				case VGRCALIB_SL3:
					calibData.setPos3("SSD3", p3);
					calibData.copyPos3X("SSD3","SSD30");
					calibData.save();
					break;
				default:
					break;
				}
				//same pos again
				break; //-> NAV
			} else if ((joyData.aX2 > 500)||(joyData.aX2 < -500)) {
				break; //-> NAV
			}
			moveJoystick();
#ifdef __DOCFSM__
			FSM_TRANSITION( CALIB_VGR_MOVE, color=orange, label='move' );
#endif
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		FSM_TRANSITION( CALIB_VGR_NAV, color=orange, label='ok' );
		break;
	}
	//-----------------------------------------------------------------
	default: assert( 0 ); break;
	}

	if( newState == currentState )
		return;

	// Exit activities ================================================
	switch( currentState )
	{
	//-----------------------------------------------------------------
	//-----------------------------------------------------------------
	default: break;
	}
}

void TxtVacuumGripperRobot::moveCalibPos()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "moveCalibPos",0);
	switch(calibPos)
	{
	case VGRCALIB_DSI:
		moveRef();
		move("DIN");
		break;
	case VGRCALIB_DCS:
		moveRef();
		move("DCS");
		break;
	case VGRCALIB_NFC:
		moveRef();
		move("DNFC");
		break;
	case VGRCALIB_WDC:
		moveRef();
		move("WDC");
		break;
	case VGRCALIB_DSO:
		moveRef();
		move("DOUT");
		break;
	case VGRCALIB_HBW:
		moveRef();
		move("HBW0");
		move("HBW");
		move("HBW1");
		break;
	case VGRCALIB_MPO:
		moveRef();
		move("HBW");
		move("HBW0");
		move("MPO0");
		move("MPO");
		break;
	case VGRCALIB_SL1:
		moveYRef();
		moveRef();
		move("SSD10");
		move("SSD1");
		break;
	case VGRCALIB_SL2:
		moveYRef();
		moveRef();
		move("SSD20");
		move("SSD2");
		break;
	case VGRCALIB_SL3:
		moveYRef();
		moveRef();
		move("SSD30");
		move("SSD3");
		break;
	default:
		assert(0);
		break;
	}
}

void TxtVacuumGripperRobot::initDashboard()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "initDashboard", 0);
	assert(mqttclient);
	mqttclient->publishStateHBW(ft::LEDS_OFF, "", TIMEOUT_MS_PUBLISH, 0, "");
	mqttclient->publishStateMPO(ft::LEDS_OFF, "", TIMEOUT_MS_PUBLISH, 0, "");
	mqttclient->publishStateSLD(ft::LEDS_OFF, "", TIMEOUT_MS_PUBLISH, 0, "");
	mqttclient->publishStateVGR(ft::LEDS_OFF, "", TIMEOUT_MS_PUBLISH, 0, "hbw");
	mqttclient->publishStateVGR(ft::LEDS_OFF, "", TIMEOUT_MS_PUBLISH, 0, "mpo");
	mqttclient->publishStateVGR(ft::LEDS_OFF, "", TIMEOUT_MS_PUBLISH, 0, "dso");
	mqttclient->publishStateDSI(ft::LEDS_OFF, "", TIMEOUT_MS_PUBLISH, 0, "");
	mqttclient->publishStateDSO(ft::LEDS_OFF, "", TIMEOUT_MS_PUBLISH, 0, "");
	mqttclient->publishStateOrder(ord_state, TIMEOUT_MS_PUBLISH);
	TxtWorkpiece wp_empty("",WP_TYPE_NONE,WP_STATE_RAW);
	History_map_t map_hist;
	mqttclient->publishNfcDS(wp_empty, map_hist, TIMEOUT_MS_PUBLISH);
}

void TxtVacuumGripperRobot::run()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "run",0);

	TxtNfcDevice* nfc = dps.getNfc();
	assert(nfc);
	obs_vgr = new TxtVacuumGripperRobotObserver(this, mqttclient);
	obs_nfc = new TxtNfcDeviceObserver(nfc, mqttclient);
	obs_dps = new TxtDeliveryPickupStationObserver(&dps, mqttclient);

	//nfc
	std::cout << "open nfc device ... ";
	bool suc = nfc->open();
	std::cout << (suc?"OK":"FAILED") << std::endl;
	if (!suc) {
		std::cout << "exit NFC failed" << std::endl;
		spdlog::get("file_logger")->error("exit NFC failed",0);
		exit(1);
	}

	//dps update thread
	dps.startThread();

	initDashboard();

	//move seq if program started first time
	moveYRef();
	moveZRef();
	moveXRef();

	FSM_INIT_FSM( INIT, color=black, label='init' );
	while (!m_stoprequested)
	{
		fsmStep();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	assert(mqttclient);
	mqttclient->publishVGR_Do(VGR_EXIT, 0, TIMEOUT_MS_PUBLISH);
	initDashboard();
}


} /* namespace ft */
