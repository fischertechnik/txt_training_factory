/*
 * TxtAxis.cpp
 *
 *  Created on: 08.11.2018
 *      Author: steiger-a
 */

#include "TxtAxis.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"


namespace ft {


TxtAxis::TxtAxis(std::string name, FISH_X1_TRANSFER* pTArea, uint8_t chM, uint8_t chS1)
	: name(name), pTArea(pTArea), status(AXIS_NOREF), pos(0), speed(512), stopReq(false), chM(chM), chS1(chS1)
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console_axes"), "{} TxtAxis chM:{} chS1:{}",name,chM,chS1);
	init();
}

TxtAxis::~TxtAxis() {
	SPDLOG_LOGGER_TRACE(spdlog::get("console_axes"), "{} ~TxtAxis",name);
	if (pTArea) setMotorOff();
}

bool TxtAxis::init() {
	SPDLOG_LOGGER_TRACE(spdlog::get("console_axes"), "{} init",name);
	setStatus(AXIS_NOREF);
	return true;
}

void TxtAxis::setSpeed(int16_t s) {
	SPDLOG_LOGGER_TRACE(spdlog::get("console_axes"), "{} setSpeed:{}",name,s);
	speed=s;
}

void TxtAxis::stop() {
	SPDLOG_LOGGER_TRACE(spdlog::get("console_axes"), "{} stop",name);
	stopReq = true;
}

void TxtAxis::setStatus(TxtAxis_status_t st) {
	SPDLOG_LOGGER_TRACE(spdlog::get("console_axes"), "{} setStatus:{}",name,st);
	status=st;
	std::string sst = toString(status);
	SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} setStatus:{}",name,sst);
}

void TxtAxis::reset() {
	SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} reset",name);
	if (chM < 8)
	{
		assert(pTArea);
		pTArea->ftX1out.cnt_reset_cmd_id[chM] = 0;
		pTArea->ftX1out.distance[chM] = 0;
		pTArea->ftX1out.motor_ex_cmd_id[chM]++;
	}
	else
	{
		assert(pTArea+1);
		(pTArea+1)->ftX1out.cnt_reset_cmd_id[chM-8] = 0;
		(pTArea+1)->ftX1out.distance[chM-8] = 0;
		(pTArea+1)->ftX1out.motor_ex_cmd_id[chM-8]++;
	}
}

void TxtAxis::resetCounter() {
	SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} cnt {}",
			name,chM<8?pTArea->ftX1in.counter[chM]:(pTArea+1)->ftX1in.counter[chM-8]);
	chM<8?pTArea->ftX1out.cnt_reset_cmd_id[chM]++:(pTArea+1)->ftX1out.cnt_reset_cmd_id[chM-8]++;
	//while(chM<8?pTArea->ftX1in.cnt_resetted[chM]:(pTArea+1)->ftX1in.cnt_resetted[chM-8] != 1);
	SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} cnt_reset {}",
			name,chM<8?pTArea->ftX1in.counter[chM]:(pTArea+1)->ftX1in.counter[chM-8]);
}

void TxtAxis::setMotorOff()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console_axes"), "{}({}) setMotorOff",name,status);
	if (chM < 8)
	{
		assert(pTArea);
		pTArea->ftX1out.duty[chM*2] = 0;
		pTArea->ftX1out.duty[chM*2+1] = 0;
	}
	else
	{
		assert(pTArea+1);
		(pTArea+1)->ftX1out.duty[(chM-8)*2] = 0;
		(pTArea+1)->ftX1out.duty[(chM-8)*2+1] = 0;
	}
}

void TxtAxis::setMotorLeft()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console_axes"), "{}({}) setMotorLeft",name,status);
	assert(chS1<8?pTArea:pTArea+1);
	if (chS1<8?pTArea->ftX1in.uni[chS1]:(pTArea+1)->ftX1in.uni[chS1-8] == 1)
	{
		setMotorOff();
	}
	else
	{
		if (chM < 8)
		{
			assert(pTArea);
			pTArea->ftX1out.duty[chM*2] = speed;
			pTArea->ftX1out.duty[chM*2+1] = 0;
		}
		else
		{
			assert(pTArea+1);
			(pTArea+1)->ftX1out.duty[(chM-8)*2] = speed;
			(pTArea+1)->ftX1out.duty[(chM-8)*2+1] = 0;
		}
	}
}

bool TxtAxis::moveAbs(uint16_t p) {
	SPDLOG_LOGGER_TRACE(spdlog::get("console_axes"), "{} movePos p:{}",name,p);
	if (p == pos) {
		SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} p:{} == pos:{}",name,p,pos);
		return true;
	} else if (p > pos) {
		SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} p:{} > pos:{}",name,p,pos);
		int steps = p - pos;
		uint16_t pret = pos;
		moveRight(steps, &pret);
		if (status == AXIS_READY) {
			pos= pret;
			SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} pos:{}",name,pos);
			Notify();
			return true;
		} else {
			std::string sst = toString(status);
			SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} setStatus:{}",name,sst);
			std::cout << "exit p > pos" << std::endl;
		}
	} else if (p < pos) {
		SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} p:{} < pos:{}",name,p,pos);
		int steps = pos - p;
		uint16_t pret = pos;
		moveLeft(steps, &pret);
		if (status == AXIS_READY) {
			pos= pret;
			SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} pos:{}",name,pos);
			Notify();
			return true;
		} else {
			std::string sst = toString(status);
			SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} setStatus:{}",name,sst);
			std::cout << "exit p < pos" << std::endl;
		}
	}
	return false;
}

void TxtAxis::moveRef()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console_axes"), "{} moveRef",name);

	setMotorOff();
	resetCounter();

	if (chS1 < 8)
	{
		assert(pTArea);
		pTArea->ftX1config.uni[chS1].mode = MODE_R; // Digital Switch with PullUp resistor
		pTArea->ftX1config.uni[chS1].digital = 1;
		pTArea->ftX1state.config_id ++; // Save the new Setup
	}
	else
	{
		assert(pTArea+1);
		(pTArea+1)->ftX1config.uni[chS1-8].mode = MODE_R; // Digital Switch with PullUp resistor
		(pTArea+1)->ftX1config.uni[chS1-8].digital = 1;
		(pTArea+1)->ftX1state.config_id ++; // Save the new Setup
	}
	SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} setup input chS1[{}]",name,chS1);

	//check switch ref
	if (chS1<8?pTArea->ftX1in.uni[chS1]:(pTArea+1)->ftX1in.uni[chS1-8] == 1) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		return;
	}

	if (chM < 8)
	{
		assert(pTArea);
		pTArea->ftX1out.distance[chM] = 0;
		pTArea->ftX1out.motor_ex_cmd_id[chM]++;
	}
	else
	{
		assert(pTArea+1);
		(pTArea+1)->ftX1out.distance[chM-8] = 0;
		(pTArea+1)->ftX1out.motor_ex_cmd_id[chM-8]++;
	}

	if ((status != AXIS_READY)&&(status != AXIS_NOREF)) {
		spdlog::get("console_axes")->error("{} Error: status != AXIS_READY. Exit function.",name);
		std::cout << "exit moveRef" << std::endl;
		exit(1);
	}
	setMotorLeft();
	SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} set motor chM[{}] {} {}",name,chM,
			chM<8?pTArea->ftX1out.duty[chM*2]:(pTArea+1)->ftX1out.duty[(chM-8)*2],
			chM<8?pTArea->ftX1out.duty[chM*2+1]:(pTArea+1)->ftX1out.duty[(chM-8)*2+1]);

	setStatus(AXIS_MOVING_REF);

	auto start = std::chrono::system_clock::now();
	while (true)
	{
		//check stop req
		if (stopReq) {
			setMotorOff();
			setStatus(AXIS_NOREF);
			stopReq = false;
			SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} stopReq",name);
			break;
		}
		//check switch ref
		if (chS1<8?pTArea->ftX1in.uni[chS1]:(pTArea+1)->ftX1in.uni[chS1-8] == 1)
		{
			setMotorOff();
			SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} pTArea->ftX1in.uni[chS1] == 1",name);
			break;
		}
		//check timeout
		auto end = std::chrono::system_clock::now();
		auto dur = end-start;
		auto diff_s = std::chrono::duration_cast< std::chrono::duration<float> >(dur).count();
		//SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "diff_s:{} diff_max:{}",diff_s,TIMEOUT_S_MOVEREF);
		if (diff_s > TIMEOUT_S_MOVEREF) {
			setStatus(AXIS_TIMEOUT_MOVEREF);
			spdlog::get("console_axes")->warn("{} diff_s > AXIS_TIMEOUT_MOVEREF, diff_s:{} diff_max:{}",name,diff_s,TIMEOUT_S_MOVEREF);
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(CYCLE_MS_AXIS));
	}
	if (status == AXIS_MOVING_REF)
	{
		resetCounter();
		pos = 0;
		setStatus(AXIS_READY);
	} else {
		std::string sst = toString(status);
		SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} setStatus:{}",name,sst);
		std::cout << "exit moveRef 2" << std::endl;
		exit(1);
	}
	Notify();
}

void TxtAxis::moveLeft(uint16_t steps, uint16_t* p) {
	assert(p!=NULL);
	SPDLOG_LOGGER_TRACE(spdlog::get("console_axes"), "{} moveLeft chM:{} chS1:{} steps:{} p:{}",name,chM,chS1,steps,*p);
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
	SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} posa: {}",name,posa);

	if (chS1 < 8)
	{
		assert(pTArea);
		pTArea->ftX1config.uni[chS1].mode = MODE_R; // Digital Switch with PullUp resistor
		pTArea->ftX1config.uni[chS1].digital = 1;
		pTArea->ftX1state.config_id ++; // Save the new Setup
	}
	else
	{
		assert(pTArea+1);
		(pTArea+1)->ftX1config.uni[chS1-8].mode = MODE_R; // Digital Switch with PullUp resistor
		(pTArea+1)->ftX1config.uni[chS1-8].digital = 1;
		(pTArea+1)->ftX1state.config_id ++; // Save the new Setup
	}

	SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} setup distance:{}",name,steps);
	if (chM < 8)
	{
		pTArea->ftX1out.distance[chM] = steps; // Distance to drive Motor 1 [0]
		pTArea->ftX1out.motor_ex_cmd_id[chM]++; // Set new Distance Value for Motor 1 [0]
	}
	else
	{
		(pTArea+1)->ftX1out.distance[chM-8] = steps; // Distance to drive Motor 1 [0]
		(pTArea+1)->ftX1out.motor_ex_cmd_id[chM-8]++; // Set new Distance Value for Motor 1 [0]
	}
	setMotorLeft();

	setStatus(AXIS_MOVING_LEFT);
	SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} setup duty left",name);

	//resetCounter();

	auto start = std::chrono::system_clock::now();
	while ((chM<8?pTArea->ftX1in.motor_ex_cmd_id[chM]:(pTArea+1)->ftX1in.motor_ex_cmd_id[chM-8])
		< (chM<8?pTArea->ftX1out.motor_ex_cmd_id[chM]:(pTArea+1)->ftX1out.motor_ex_cmd_id[chM-8]))
	{
		//check home switch
		if (chS1<8?pTArea->ftX1in.uni[chS1]:(pTArea+1)->ftX1in.uni[chS1-8] == 1)
		{
			SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} pTArea->ftX1in.uni[chS1] == 1",name);
			setMotorOff();
			resetCounter();
			pos = 0;
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
		double diff_max = (double)TIMEOUT_S_MOVELEFT;
		//SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "diff_s:{} diff_max:{}",diff_s,diff_max);
		if (diff_s > diff_max) {
			setStatus(AXIS_TIMEOUT_MOVELEFT);
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
	if (status == AXIS_MOVING_LEFT)
	{
		//set new pos
		SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} posa >= pTArea->ftX1in.counter[ch:{}]",name,chM);
		INT16 chMcounter = chM<8?pTArea->ftX1in.counter[chM]:(pTArea+1)->ftX1in.counter[chM-8];
		if (posa >= chMcounter) {
			*p = posa - chMcounter;
		} else {
			*p = 0;
		}
		SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} *p:{}",name,*p);
		setStatus(AXIS_READY);
	} else {
		std::string sst = toString(status);
		SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} setStatus:{}",name,sst);
		exit(1);
	}
}


} /* namespace ft */
