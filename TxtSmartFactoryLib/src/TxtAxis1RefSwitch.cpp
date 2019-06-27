/*
 * Axis1RefSwitch.cpp
 *
 *  Created on: 08.11.2018
 *      Author: steiger-a
 */

#include "TxtAxis1RefSwitch.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"


namespace ft {


TxtAxis1RefSwitch::TxtAxis1RefSwitch(std::string name, FISH_X1_TRANSFER* pTArea, uint8_t chM, uint8_t chS1, uint16_t posEnd)
	: TxtAxis(name, pTArea, chM, chS1), posEnd(posEnd)
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console_axes"), "{} TxtAxis1RefSwitch chM:{} chS1:{} posEnd:{}",name,chM,chS1,posEnd);
}

TxtAxis1RefSwitch::~TxtAxis1RefSwitch()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console_axes"), "{} ~TxtAxis1RefSwitch",name);
}

bool TxtAxis1RefSwitch::moveAbs(uint16_t p)
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console_axes"), "{} movePos p:{}",name,p);
	if (p > posEnd)
	{
		spdlog::get("console_axes")->error("{} Warning: p:{} > posEnd:{}",name,p,posEnd);
	}
	return TxtAxis::moveAbs(p);
}

void TxtAxis1RefSwitch::setMotorRight()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console_axes"), "{}({}) setMotorRight",name,status);
	assert(pTArea);
	if (pos >= posEnd)
	{
		setMotorOff();
	}
	else
	{
		if (chM < 8)
		{
			assert(pTArea);
			pTArea->ftX1out.duty[chM*2] = 0;
			pTArea->ftX1out.duty[chM*2+1] = speed;
		}
		else
		{
			assert(pTArea+1);
			(pTArea+1)->ftX1out.duty[(chM-8)*2] = 0;
			(pTArea+1)->ftX1out.duty[(chM-8)*2+1] = speed;
		}
	}
}

void TxtAxis1RefSwitch::moveRight(uint16_t steps, uint16_t* p) {
	assert(p!=NULL);
	SPDLOG_LOGGER_TRACE(spdlog::get("console_axes"), "{} moveRight steps:{} p:{}",name,steps,*p);
	if (steps <= 0) {
		spdlog::get("console_axes")->warn("{} Warning: steps<=0. Exit function.",name);
		return;
	}
	if (status == AXIS_NOREF) {
		spdlog::get("console_axes")->error("{} Error: status == AXIS_NOREF. Execute moveRef() first! Exit function.",name);
		return;
	}
	if (status != AXIS_READY) {
		spdlog::get("console_axes")->error("{} Error: status != AXIS_READY. Exit function.",name);
		return;
	}
	int16_t posa = *p;
	SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "posa: {}", posa);

	if (chM < 8)
	{
		assert(pTArea);
		pTArea->ftX1out.distance[chM] = steps; // Distance to go back
		pTArea->ftX1out.motor_ex_cmd_id[chM]++; // Set new Distance Value for Motor 1 [0]
	}
	else
	{
		assert(pTArea+1);
		(pTArea+1)->ftX1out.distance[chM-8] = steps; // Distance to go back
		(pTArea+1)->ftX1out.motor_ex_cmd_id[chM-8]++; // Set new Distance Value for Motor 1 [0]
	}
	SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} setup distance:{}",name,steps);
	setMotorRight();
	SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} setup duty right",name);

	setStatus(AXIS_MOVING_RIGHT);

	//resetCounter();

	auto start = std::chrono::system_clock::now();
	while ((chM<8?pTArea->ftX1in.motor_ex_cmd_id[chM]:(pTArea+1)->ftX1in.motor_ex_cmd_id[chM-8])
		< (chM<8?pTArea->ftX1out.motor_ex_cmd_id[chM]:(pTArea+1)->ftX1out.motor_ex_cmd_id[chM-8]))
	{
		//check end pos
		if (((posa+steps)) >= posEnd) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} STOPPING posEnd:{}",name,posEnd);
			setMotorOff();
			break;
		}
		//check stop req
		if (stopReq) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} stopAllReq",name);
			setMotorOff();
			stopReq = false;
			break;
		}
		//check timeout
		auto end = std::chrono::system_clock::now();
		auto dur = end-start;
		auto diff_s = std::chrono::duration_cast< std::chrono::duration<float> >(dur).count();
		double diff_max = (double)TIMEOUT_S_MOVERIGHT;
		//SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "diff_s:{} diff_max:{}",diff_s,diff_max);
		if (diff_s > diff_max) {
			setStatus(AXIS_TIMEOUT_MOVERIGHT);
			spdlog::get("console_axes")->warn("{} diff_s > diff_max: diff_s:{} diff_max:{}",name,diff_s,diff_max);
			break;
		}
		SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} OutExCmd:{} InExCmd:{} Counter:{} reach:{}",
			name,
			chM<8?pTArea->ftX1out.motor_ex_cmd_id[chM] :(pTArea+1)->ftX1out.motor_ex_cmd_id[chM-8],
			chM<8?pTArea->ftX1in.motor_ex_cmd_id[chM]  :(pTArea+1)->ftX1in.motor_ex_cmd_id[chM-8],
			chM<8?pTArea->ftX1in.counter[chM]          :(pTArea+1)->ftX1in.counter[chM-8],
			chM<8?pTArea->ftX1in.motor_ex_reached[chM] :(pTArea+1)->ftX1in.motor_ex_reached[chM-8]);
		std::this_thread::sleep_for(std::chrono::milliseconds(CYCLE_MS_AXIS));
	}
	if (status == AXIS_MOVING_RIGHT)
	{
		//set new pos
		INT16 chMcounter = chM<8?pTArea->ftX1in.counter[chM]:(pTArea+1)->ftX1in.counter[chM-8];
		if ((posa + chMcounter) >= posEnd) {
			setStatus(AXIS_ERROR);
			SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} AXIS_ERROR: *p:{} posa:{} pTArea->ftX1in.counter[ch]:{}",name,*p,posa,chMcounter);
			std::cout << "exit moveRight" << std::endl;
			exit(1);
		} else {
			*p = posa + chMcounter;
			setStatus(AXIS_READY);
		}
	} else {
		std::string sst = toString(status);
		SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} setStatus:{}",name,sst);
		std::cout << "exit moveRight 2" << std::endl;
		exit(1);
	}
}


} /* namespace ft */
