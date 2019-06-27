/*
 * TxtMultiProcessingStation.cpp
 *
 *  Created on: 03.04.2019
 *      Author: steiger-a
 */

#include "TxtMultiProcessingStation.h"

#include "TxtMqttFactoryClient.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"


namespace ft {


TxtMultiProcessingStation::TxtMultiProcessingStation(FISH_X1_TRANSFER* pTArea, ft::TxtMqttFactoryClient* mqttclient)
	: TxtSimulationModel(pTArea, mqttclient),
	  currentState(__NO_STATE), newState(__NO_STATE),
	  chMsaw(1),
	  vgripper(pTArea,7,8+4),
	  axisGripper("gripper",pTArea,8+1,4,8+2),
	  axisOvenInOut("ovenInOut",pTArea,8+0,8+1,8+0),
	  axisRotTable("rotTable",pTArea,0,0,1,2),
	  convBelt(pTArea,2),
	  reqQuit(false), reqVGRwp(0), reqVGRproduce(false), reqSLDstarted(false)
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "TxtMultiProcessingStation",0);
    configInputs();
}

TxtMultiProcessingStation::~TxtMultiProcessingStation()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "~TxtMultiProcessingStation",0);
}

bool TxtMultiProcessingStation::isEndConveyorBeltTriggered()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "isEndConveyorBeltTriggered", 0);
	assert(pTArea);
	return (pTArea->ftX1in.uni[3] != 1);
}

void TxtMultiProcessingStation::setSawOff() {
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "setSawLeft",0);
	assert(pTArea);
	pTArea->ftX1out.duty[chMsaw*2] = 0;
	pTArea->ftX1out.duty[chMsaw*2+1] = 0;
	setActStatus(false, SM_READY);
}

void TxtMultiProcessingStation::setSawLeft() {
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "setSawLeft",0);
	setActStatus(true, SM_BUSY);
	assert(pTArea);
	pTArea->ftX1out.duty[chMsaw*2] = 512;
	pTArea->ftX1out.duty[chMsaw*2+1] = 0;
}

void TxtMultiProcessingStation::setSawRight()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "setSawRight",0);
	setActStatus(true, SM_BUSY);
	assert(pTArea);
	pTArea->ftX1out.duty[chMsaw*2] = 0;
	pTArea->ftX1out.duty[chMsaw*2+1] = 512;
}

void TxtMultiProcessingStation::setValveEjection(bool on)
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "setValveEjection {}", on);
	setActStatus(on, on?SM_BUSY:SM_READY);
	assert(pTArea);
	pTArea->ftX1out.duty[6] = on ? 512 : 0; // Switch on with PWM Value 512 (= max speed)
}

void TxtMultiProcessingStation::setCompressor(bool on)
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "setCompressor {}", on);
	setActStatus(on, on?SM_BUSY:SM_READY);
	assert(pTArea);
	pTArea->ftX1out.duty[7] = on ? 512 : 0; // Switch on with PWM Value 512 (= max speed)
}

bool TxtMultiProcessingStation::isOvenTriggered()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "isOvenTriggered", 0);
	assert(pTArea+1);
	return ((pTArea+1)->ftX1in.uni[4] != 1);
}

void TxtMultiProcessingStation::setValveVacuum(bool on)
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "setValveVacuum {}", on);
	setActStatus(on, on?SM_BUSY:SM_READY);
	assert(pTArea+1);
	(pTArea+1)->ftX1out.duty[4] = on ? 512 : 0; // Switch on with PWM Value 512 (= max speed)
}

void TxtMultiProcessingStation::setValveLowering(bool on)
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "setValveLowering {}", on);
	setActStatus(on, on?SM_BUSY:SM_READY);
	assert(pTArea+1);
	(pTArea+1)->ftX1out.duty[5] = on ? 512 : 0; // Switch on with PWM Value 512 (= max speed)
}

void TxtMultiProcessingStation::setValveOvenDoor(bool on)
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "setValveOvenDoor {}", on);
	setActStatus(on, on?SM_BUSY:SM_READY);
	assert(pTArea+1);
	(pTArea+1)->ftX1out.duty[6] = on ? 512 : 0; // Switch on with PWM Value 512 (= max speed)
}

void TxtMultiProcessingStation::setLightOven(bool on)
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "setLightOven {}", on);
	assert(pTArea+1);
	(pTArea+1)->ftX1out.duty[7] = on ? 512 : 0;
}

void TxtMultiProcessingStation::configInputs()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "configInputs", 0);

	//master
	assert(pTArea);
	//End Conveyor Belt
	pTArea->ftX1config.uni[3].mode = MODE_R; // Digital Switch with PullUp resistor
	pTArea->ftX1config.uni[3].digital = 1;
	//save
	pTArea->ftX1state.config_id ++; // Save the new Setup

	//extension
	assert(pTArea+1);
	//Oven Phototransistor
	(pTArea+1)->ftX1config.uni[4].mode = MODE_R; // Digital Switch with PullUp resistor
	(pTArea+1)->ftX1config.uni[4].digital = 1;
	//save
	(pTArea+1)->ftX1state.config_id ++; // Save the new Setup
}


} /* namespace ft */
