/*
 * TxtConveyorBelt.cpp
 *
 *  Created on: 18.02.2019
 *      Author: steiger-a
 */

#include "TxtConveyorBelt.h"


namespace ft {


TxtConveyorBelt::TxtConveyorBelt(FISH_X1_TRANSFER* pTArea, uint8_t chM)
	: pTArea(pTArea), chM(chM), speed(512)
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "TxtConveyorBelt",0);
}

TxtConveyorBelt::~TxtConveyorBelt()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "~TxtConveyorBelt",0);
}

void TxtConveyorBelt::setSpeed(int16_t s) {
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "setSpeed:{}", s);
	speed=s;
}

void TxtConveyorBelt::moveLeft()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "moveLeft",0);
	assert(pTArea);
	pTArea->ftX1out.duty[chM*2] = speed;
	pTArea->ftX1out.duty[chM*2+1] = 0;
}

void TxtConveyorBelt::moveRight()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "moveRight",0);
	assert(pTArea);
	pTArea->ftX1out.duty[chM*2] = 0;
	pTArea->ftX1out.duty[chM*2+1] = speed;
}

void TxtConveyorBelt::stop() {
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "stop",0);
	assert(pTArea);
	pTArea->ftX1out.duty[chM*2] = 0;
	pTArea->ftX1out.duty[chM*2+1] = 0;
}


TxtConveyorBeltLightBarriers::TxtConveyorBeltLightBarriers(FISH_X1_TRANSFER* pTArea, uint8_t chM, int chL1, int chL2)
	: TxtConveyorBelt(pTArea, chM), chL1(chL1), chL2(chL2)
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "TxtConveyorBeltLightBarriers",0);
}

TxtConveyorBeltLightBarriers::~TxtConveyorBeltLightBarriers()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "~TxtConveyorBeltLightBarriers",0);
}


void TxtConveyorBeltLightBarriers::moveIn() {
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "moveIn",0);
	moveLeft();
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	stop();
}

void TxtConveyorBeltLightBarriers::moveOut() {
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "moveOut",0);
	moveRight();
	std::this_thread::sleep_for(std::chrono::milliseconds(1200));
	stop();
}


} /* namespace ft */
