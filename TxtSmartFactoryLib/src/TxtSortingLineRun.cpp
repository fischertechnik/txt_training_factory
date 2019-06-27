/*
 * TxtSortingLineRun.cpp
 *
 *  Created on: 03.04.2019
 *      Author: steiger-a
 */

#ifndef __DOCFSM__
#include "TxtSortingLine.h"

#include "TxtMqttFactoryClient.h"
#include "Utils.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
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


void TxtSortingLine::fsmStep()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "fsmStep",0);

	// Entry activities ===============================================
	if( newState != currentState )
	{
		switch( newState )
		{
		//-------------------------------------------------------------
		case IDLE:
		{
			printEntryState(IDLE);
			setActStatus(false, SM_READY);
			break;
		}
		//-------------------------------------------------------------
		default: break;
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
		setStatus(SM_ERROR);
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
		FSM_TRANSITION( INIT );
#endif
		printState(INIT);
		FSM_TRANSITION( IDLE, color=blue, label='initialized' );
		break;
	}
	//-----------------------------------------------------------------
	case IDLE:
	{
		printState(IDLE);
		/*TODO wait req MPO
		if (reqMPOproduced)
		{
			auto start = std::chrono::system_clock::now();
			while (!isColorSensorTriggered())
			{
				auto end = std::chrono::system_clock::now();
				auto dur = end-start;
				auto diff_s = std::chrono::duration_cast< std::chrono::duration<float> >(dur).count();
				double diff_max = 5.0;
				if (diff_s > diff_max) {
					FSM_TRANSITION( FAULT, color=red, label='timeout\n5 sec' );
					break;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}

			assert(mqttclient);
			mqttclient->publishSLD_Ack(SLD_STARTED, ft::WP_TYPE_NONE, 0, TIMEOUT_MS_PUBLISH);

			FSM_TRANSITION( START, color=blue, label='req\nMPO' );
			reqMPOproduced = false;
		}
		if (reqVGRstart)
		{
			FSM_TRANSITION( START, color=blue, label='req\nVGR' );
			reqVGRstart = false;
		}*/
		if (isColorSensorTriggered())
		{
			FSM_TRANSITION( START, color=blue, label='start' );
		}


#ifdef __DOCFSM__
		FSM_TRANSITION( IDLE, color=green, label='wait' );
#endif
		break;
	}
	//-----------------------------------------------------------------
	case START:
	{
		printState(START);
		setActStatus(true, SM_BUSY);
		convBelt.moveRight();
		detectedColorValue = 3000;
		FSM_TRANSITION( COLOR_DETECTION, color=blue, label='conv\non' );
		break;
	}
	//-----------------------------------------------------------------
	case COLOR_DETECTION:
	{
		printState(COLOR_DETECTION);
		if (readColorValue() < detectedColorValue)
		{
			detectedColorValue = lastColorValue;
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "color value [min]: {} [{}]", lastColorValue, detectedColorValue);
		}
		if (isEjectionTriggered())
		{
			//min color is final color
			std::cout << "color final value: " << detectedColorValue << std::endl;
			FSM_TRANSITION( START_COUNT, color=blue, label='start\ncounter' );
		}
#ifdef __DOCFSM__
		FSM_TRANSITION( COLOR_DETECTION, color=blue, label='find min\nvalue' );
#endif
		break;
	}
	//-----------------------------------------------------------------
	case START_COUNT:
	{
		printState(START_COUNT);
		setCompressor(true);
		// Reset counter
		u16Counter = 0;
		FSM_TRANSITION( CHECK_COUNT, color=blue, label='check\ncounter' );
		break;
	}
	//-----------------------------------------------------------------
	case CHECK_COUNT:
	{
		printState(CHECK_COUNT);
		switch (getDetectedColor())
		{
		case WP_TYPE_WHITE:
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "WHITE",0);
			if (u16Counter >= COUNT_WHITE)
			{
				FSM_TRANSITION( EJECTION_WHITE, color=blue, label='counter\nw' );
			}
			break;
		case WP_TYPE_RED:
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "RED",0);
			if (u16Counter >= COUNT_RED)
			{
				FSM_TRANSITION( EJECTION_RED, color=blue, label='counter\nr' );
			}
			break;
		case WP_TYPE_BLUE:
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "BLUE",0);
			if (u16Counter >= COUNT_BLUE)
			{
				FSM_TRANSITION( EJECTION_BLUE, color=blue, label='counter\nb' );
			}
			break;
		default:
			if (u16Counter >= COUNT_WRONG)
			{
				FSM_TRANSITION( FAULT, color=red, label='counter\nwrong' );
			}
			break;
		}
		SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "counter: {}",u16Counter);
#ifdef __DOCFSM__
		FSM_TRANSITION( CHECK_COUNT, color=blue, label='check\ncounter' );
#endif
		break;
	}
	//-----------------------------------------------------------------
	case EJECTION_WHITE:
	{
		printState(EJECTION_WHITE);
		ejectWhite();
		FSM_TRANSITION( SORTED, color=blue, label='eject\nw' );
		break;
	}
	//-----------------------------------------------------------------
	case EJECTION_RED:
	{
		printState(EJECTION_RED);
		ejectRed();
		FSM_TRANSITION( SORTED, color=blue, label='eject\nr' );
		break;
	}
	//-----------------------------------------------------------------
	case EJECTION_BLUE:
	{
		printState(EJECTION_BLUE);
		ejectBlue();
		FSM_TRANSITION( SORTED, color=blue, label='eject\nb' );
		break;
	}
	//-----------------------------------------------------------------
	case SORTED:
	{
		printState(SORTED);
		convBelt.stop();
		setActStatus(false, SM_BUSY);

		auto start = std::chrono::system_clock::now();
		while (!isWhite() && !isRed() && !isBlue())
		{
			auto end = std::chrono::system_clock::now();
			auto dur = end-start;
			auto diff_s = std::chrono::duration_cast< std::chrono::duration<float> >(dur).count();
			double diff_max = 10.0;
			if (diff_s > diff_max) {
				FSM_TRANSITION( FAULT, color=red, label='timeout\n5 sec' );
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		if (isWhite())
		{
			assert(mqttclient);
			mqttclient->publishSLD_Ack(SLD_SORTED, ft::WP_TYPE_WHITE, lastColorValue, TIMEOUT_MS_PUBLISH);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
		else if (isRed())
		{
			assert(mqttclient);
			mqttclient->publishSLD_Ack(SLD_SORTED, ft::WP_TYPE_RED, lastColorValue, TIMEOUT_MS_PUBLISH);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
		else if (isBlue())
		{
			assert(mqttclient);
			mqttclient->publishSLD_Ack(SLD_SORTED, ft::WP_TYPE_BLUE, lastColorValue, TIMEOUT_MS_PUBLISH);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}

		FSM_TRANSITION( IDLE, color=green, label='next' );
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


void TxtSortingLine::run()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "run",0);
	assert(mqttclient);
	obs_sld = new TxtSortingLineObserver(this, mqttclient);

	FSM_INIT_FSM( INIT, color=black, label='init' );
	while (!m_stoprequested)
	{
		fsmStep();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}


} /* namespace ft */
