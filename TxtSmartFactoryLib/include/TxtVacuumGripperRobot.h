/*
 * TxtVacuumGripperRobot.h
 *
 *  Created on: 08.11.2018
 *      Author: steiger-a
 */

#ifndef TXTVACUUMGRIPPERROBOT_H_
#define TXTVACUUMGRIPPERROBOT_H_

#ifndef __DOCFSM__
#include "TxtSimulationModel.h"
#include "TxtCalibData.h"
#include "TxtAxis1RefSwitch.h"
#include "TxtVacuumGripperRobot.h"
#include "TxtDeliveryPickupStation.h"
#include "TxtVacuumGripper.h"
#include "TxtMqttFactoryClient.h"
#include "TxtFactoryProcessStorage.h"

#include <map>
#include <thread>
#endif


#ifdef FSM_DECLARE_STATE_XE
 #undef FSM_DECLARE_STATE_XE
#endif
#define FSM_DECLARE_STATE_XE( stateName, attr... ) stateName


namespace ft {


class TxtVacuumGripperRobot;
class TxtVacuumGripperRobotObserver;


typedef enum
{
	VGRMOV_PTP, // 3 threads
	VGRMOV_XYZ, // seq3 ...
	VGRMOV_XZY,
	VGRMOV_YXZ,
	VGRMOV_YZX,
	VGRMOV_ZXY,
	VGRMOV_ZYX,
	VGRMOV_X_PTP, // seq2 ...
	VGRMOV_Y_PTP,
	VGRMOV_Z_PTP
} TxtVgrPosOrder_t;


typedef enum
{
	VGRCALIB_DSI = 0,
	VGRCALIB_DCS,
	VGRCALIB_NFC,
	VGRCALIB_WDC,
	VGRCALIB_DSO,
	VGRCALIB_HBW,
	VGRCALIB_MPO,
	VGRCALIB_SL1,
	VGRCALIB_SL2,
	VGRCALIB_SL3,
	VGRCALIB_END
} TxtVgrCalibPos_t;

inline const char * toString(TxtVgrCalibPos_t v)
{
	switch(v) {
	case VGRCALIB_DSI: return "DSI";
	case VGRCALIB_DCS: return "DCS";
	case VGRCALIB_NFC: return "NFC";
	case VGRCALIB_WDC: return "WDC";
	case VGRCALIB_DSO: return "DSO";
	case VGRCALIB_HBW: return "HBW";
	case VGRCALIB_MPO: return "MPO";
	case VGRCALIB_SL1: return "SL1";
	case VGRCALIB_SL2: return "SL2";
	case VGRCALIB_SL3: return "SL3";
	default: return "";
	}
}


class TxtVacuumGripperRobotCalibData : public ft::TxtCalibData {
public:
	TxtVacuumGripperRobotCalibData()
		: TxtCalibData("Data/Calib.VGR.json") {};
	virtual ~TxtVacuumGripperRobotCalibData() {}

	bool load();
	bool saveDefault();
	bool save();

	void checkKeyExit(const std::string key) {
		auto search = map_pos3.find(key);
		if (search == map_pos3.end()) {
			std::cout << "Error: key unknown " << key << std::endl;
			spdlog::get("file_logger")->error("Error: key unknown {}",key);
			exit(1);
		}
	}
	void setPos3(const std::string key, EncPos3 pos3) {
		map_pos3[key] = pos3;
	}
	void copyPos3X(const std::string key_src, const std::string key_dst) {
		checkKeyExit(key_src);
		checkKeyExit(key_dst);
		map_pos3[key_dst].x = map_pos3[key_src].x;
	}
	void copyPos3Y(const std::string key_src, const std::string key_dst) {
		checkKeyExit(key_src);
		checkKeyExit(key_dst);
		map_pos3[key_dst].y = map_pos3[key_src].y;
	}
	void copyPos3Z(const std::string key_src, const std::string key_dst) {
		checkKeyExit(key_src);
		checkKeyExit(key_dst);
		map_pos3[key_dst].z = map_pos3[key_src].z;
	}

	std::map<std::string, EncPos3> map_pos3;
};

class TxtJoystickXYBController;
class TxtVacuumGripperRobot : public ft::TxtSimulationModel {
	friend class TxtJoystickXYBController;
public:
	const int ydelta = 50;

	enum State_t
	{
		__NO_STATE,
		FSM_DECLARE_STATE_XE( FAULT, color=red ),
		FSM_DECLARE_STATE_XE( INIT, color=blue ),
		FSM_DECLARE_STATE_XE( IDLE, color=green ),
		FSM_DECLARE_STATE_XE( FETCH_WP_VGR, color=blue ),
		FSM_DECLARE_STATE_XE( VGR_WAIT_FETCHED, color=blue ),
		FSM_DECLARE_STATE_XE( MOVE_VGR2MPO, color=blue ),
		FSM_DECLARE_STATE_XE( START_PRODUCE, color=blue ),
		FSM_DECLARE_STATE_XE( MOVE_PICKUP_WAIT, color=blue ),
		FSM_DECLARE_STATE_XE( MOVE_PICKUP, color=blue ),
		FSM_DECLARE_STATE_XE( START_DELIVERY, color=blue ),
		FSM_DECLARE_STATE_XE( COLOR_DETECTION, color=blue ),
		FSM_DECLARE_STATE_XE( WRONG_COLOR, color=blue ),
		FSM_DECLARE_STATE_XE( NFC_RAW, color=blue ),
		FSM_DECLARE_STATE_XE( NFC_PRODUCED, color=blue ),
		FSM_DECLARE_STATE_XE( NFC_REJECTED, color=blue ),
		FSM_DECLARE_STATE_XE( STORE_WP_VGR, color=blue ),
		FSM_DECLARE_STATE_XE( STORE_WP, color=blue ),
		FSM_DECLARE_STATE_XE( CALIB_HBW, color=orange ),
		FSM_DECLARE_STATE_XE( CALIB_SLD, color=orange ),
		FSM_DECLARE_STATE_XE( CALIB_DPS, color=orange ),
		FSM_DECLARE_STATE_XE( CALIB_DPS_NEXT, color=orange ),
		FSM_DECLARE_STATE_XE( CALIB_VGR, color=orange ),
		FSM_DECLARE_STATE_XE( CALIB_VGR_NAV, color=orange ),
		FSM_DECLARE_STATE_XE( CALIB_VGR_MOVE, color=orange ),
	};

	inline const char * toString(State_t state)
	{
		#define _CASE_ITEM( _state ) case _state: return #_state;
		switch( state )
		{
		   _CASE_ITEM( FAULT )
		   _CASE_ITEM( INIT )
		   _CASE_ITEM( IDLE )
		   _CASE_ITEM( FETCH_WP_VGR )
		   _CASE_ITEM( VGR_WAIT_FETCHED )
		   _CASE_ITEM( MOVE_VGR2MPO )
		   _CASE_ITEM( START_PRODUCE )
		   _CASE_ITEM( MOVE_PICKUP_WAIT )
		   _CASE_ITEM( MOVE_PICKUP )
		   _CASE_ITEM( START_DELIVERY )
		   _CASE_ITEM( COLOR_DETECTION )
		   _CASE_ITEM( WRONG_COLOR )
		   _CASE_ITEM( NFC_RAW )
		   _CASE_ITEM( NFC_PRODUCED )
		   _CASE_ITEM( NFC_REJECTED )
		   _CASE_ITEM( STORE_WP_VGR )
		   _CASE_ITEM( STORE_WP )
		   _CASE_ITEM( CALIB_HBW )
		   _CASE_ITEM( CALIB_SLD )
		   _CASE_ITEM( CALIB_DPS )
		   _CASE_ITEM( CALIB_DPS_NEXT )
		   _CASE_ITEM( CALIB_VGR )
		   _CASE_ITEM( CALIB_VGR_NAV )
		   _CASE_ITEM( CALIB_VGR_MOVE )
		   default: break;
		}
		return "[TxtVacuumGripperRobot::State_t] Unknown State";
		#undef _CASE_ITEM
	}

	inline void printState(State_t state)
	{
		std::cout << toString(state) << std::endl;
	}
	inline void printEntryState(State_t state)
	{
		std::cout << "entry " << toString(state) << std::endl;
	}

	inline void printExitState(State_t state)
	{
		std::cout << "exit " << toString(state) << std::endl;
	}

	TxtVacuumGripperRobot(TxtTransfer* pT, ft::TxtMqttFactoryClient* mqttclient = 0);
	virtual ~TxtVacuumGripperRobot();

	/* remote */
	void requestQuit() {
		SPDLOG_LOGGER_TRACE(spdlog::get("console"),"requestQuit",0);
		reqQuit= true;
	}
	void requestOrder(TxtWPType_t type) {
		SPDLOG_LOGGER_TRACE(spdlog::get("console"),"requestOrder {}",(int)type);
		reqWP_order = ft::TxtWorkpiece("", type, WP_STATE_RAW);
		reqOrder= true;
	}
	void requestNfcRead() {
		SPDLOG_LOGGER_TRACE(spdlog::get("console"),"requestNfcRead",0);
		reqNfcRead= true;
	}
	void requestNfcDelete() {
		SPDLOG_LOGGER_TRACE(spdlog::get("console"),"requestNfcDelete",0);
		reqNfcDelete= true;
	}
	/* local */
	void requestExit(const std::string name) {
		std::cout << "program terminated by " << name << std::endl;
		spdlog::get("file_logger")->error("program terminated by {}",name);
		exit(1);
	}
	void requestJoyBut(TxtJoysticksData jd) {
		SPDLOG_LOGGER_TRACE(spdlog::get("console"),"requestJoyBut",0);
		joyData = jd;
		reqJoyData = true;
	}
	void requestMPOstarted(TxtWorkpiece* wp) {
		SPDLOG_LOGGER_TRACE(spdlog::get("console"),"requestMPOstarted",0);
		reqWP_MPO = wp;
		reqMPOstarted = true;
	}
	void requestHBWcalib_nav() {
		SPDLOG_LOGGER_TRACE(spdlog::get("console"),"requestHBWcalib_nav",0);
		reqHBWcalib_nav = true;
	}
	void requestHBWcalib_end() {
		SPDLOG_LOGGER_TRACE(spdlog::get("console"),"requestHBWcalib_end",0);
		reqHBWcalib_end = true;
	}
	void requestSLDcalib_end() {
		SPDLOG_LOGGER_TRACE(spdlog::get("console"),"requestSLDcalib_end",0);
		reqSLDcalib_end = true;
	}
	void requestHBWstored(TxtWorkpiece* wp) {
		SPDLOG_LOGGER_TRACE(spdlog::get("console"),"requestHBWstored",0);
		reqWP_HBW = wp;
		reqHBWstored = true;
	}
	void requestHBWfetched(TxtWorkpiece* wp) {
		SPDLOG_LOGGER_TRACE(spdlog::get("console"),"requestHBWfetched",0);
		reqWP_HBW = wp;
		reqHBWfetched = true;
	}
	void requestSLDsorted(TxtWPType_t type) {
		SPDLOG_LOGGER_TRACE(spdlog::get("console"),"requestSLDsorted {}",(int)type);
		reqWP_SLD.type = type;
		reqWP_SLD.state = ft::WP_STATE_PROCESSED;
		reqSLDsorted = true;
	}

	void stop();
	void moveRef();
	EncPos3 getPos3() {
		EncPos3 p3;
		p3.x = axisX.getPosAbs();
		p3.y = axisY.getPosAbs();
		p3.z = axisZ.getPosAbs();
		return p3;
	}

	void moveJoystick();

	void moveXRef();
	void moveYRef();
	void moveZRef();

	void moveXEnd();
	void moveYEnd();
	void moveZEnd();

	void grip() { vgripper.grip(); }
	void release() { vgripper.release(); }

	void moveDeliveryInAndGrip();
	void moveDeliveryOutAndRelease();
	void moveColorSensor(bool half = false);
	void moveRefYNFC();
	void moveNFC();
	std::string nfcDeviceDeleteWriteRawRead(std::vector<int64_t> vts, uint8_t mask_ts);
	std::string nfcDeviceWriteProducedRead(std::vector<int64_t> vts, uint8_t mask_ts);
	std::string nfcDeviceWriteRejectedRead(std::vector<int64_t> vts, uint8_t mask_ts);
	void moveWrongRelease();
	void moveToHBW();
	void moveFromHBW1();
	void moveFromHBW2();
	void moveMPO();
	void moveSSD1();
	void moveSSD2();
	void moveSSD3();

	void setSpeed(int16_t s);

	void setTarget(std::string t) { target = t; Notify(); }
	std::string getTarget() { return target; }

	TxtAxis1RefSwitch axisX;
	TxtAxis1RefSwitch axisY;
	TxtAxis1RefSwitch axisZ;

protected:
	State_t currentState;
	State_t newState;
	TxtVgrCalibPos_t calibPos;
	TxtWPType_t calibColor;
	int calibColorValues[3];
	EncPos3 lastPos3;

	void configInputs();
	void initDashboard();
	void move(const std::string pos3name, TxtVgrPosOrder_t order = VGRMOV_PTP);
	void move(EncPos3 p3, TxtVgrPosOrder_t order = VGRMOV_PTP) { move(p3.x,p3.y,p3.z, order); }
	void move(uint16_t x, uint16_t y, uint16_t z, TxtVgrPosOrder_t order = VGRMOV_PTP);

	void moveCalibPos();

	/*!
     * @dotfile TxtVacuumGripperRobotRun.gv
     */
	void run();

	TxtVacuumGripper vgripper;
	TxtVacuumGripperRobotCalibData calibData;

	std::string target;

	TxtDeliveryPickupStation dps;

	/* remote */
	bool reqQuit;
	bool reqOrder;
	TxtWorkpiece reqWP_order;
	bool reqNfcRead;
	bool reqNfcDelete;
	TxtOrderState ord_state;
	/* local */
	TxtJoysticksData joyData;
	bool reqJoyData;
	bool reqMPOstarted;
	TxtWorkpiece* reqWP_MPO;
	bool reqHBWstored;
	bool reqHBWfetched;
	bool reqHBWcalib_nav;
	bool reqHBWcalib_end;
	bool reqSLDcalib_end;
	TxtWorkpiece* reqWP_HBW;
	bool reqSLDsorted;
	TxtWorkpiece reqWP_SLD;

	TxtFactoryProcessStorage proStorage;

	TxtVacuumGripperRobotObserver* obs_vgr;
	TxtNfcDeviceObserver* obs_nfc;
	TxtDeliveryPickupStationObserver* obs_dps;

private:
   void fsmStep();
};


class TxtVacuumGripperRobotObserver : public ft::Observer {
public:
	TxtVacuumGripperRobotObserver(ft::TxtVacuumGripperRobot* s, ft::TxtMqttFactoryClient* mqttclient)
		: _subject(s), _mqttclient(mqttclient)
	{
		SPDLOG_LOGGER_TRACE(spdlog::get("console"), "TxtVacuumGripperRobotObserver",0);
		_subject->Attach(this);
	}
	virtual ~TxtVacuumGripperRobotObserver() {
		SPDLOG_LOGGER_TRACE(spdlog::get("console"), "~TxtVacuumGripperRobotObserver",0);
		_subject->Detach(this);
	}
	void Update(ft::SubjectObserver* theChangedSubject) {
		if(theChangedSubject == _subject) {
			//SPDLOG_LOGGER_TRACE(spdlog::get("console"), "Update 1",0);

			ft::TxtSimulationModel_status_t st = _subject->getStatus();
			int code = 0;
			if (st == SM_READY) {
				code = 1;
			} else if (st == SM_BUSY) {
				code = 2;
			} else if (st == SM_ERROR) {
				code = 4;
			} else if (st == SM_CALIB) {
				code = 7;
			}
			assert(_mqttclient);
			_mqttclient->publishStateVGR((ft::TxtLEDSCode_t)code,"",TIMEOUT_MS_PUBLISH,_subject->isActive()?1:0,_subject->getTarget());

			//SPDLOG_LOGGER_TRACE(spdlog::get("console"), "Update 2",0);
		}
	}
private:
	ft::TxtVacuumGripperRobot *_subject;
	ft::TxtMqttFactoryClient* _mqttclient;
};


} /* namespace ft */


#endif /* TXTVACUUMGRIPPERROBOT_H_ */
