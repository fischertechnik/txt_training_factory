/*
 * TxtAxisNRefSwitch.cpp
 *
 *  Created on: 08.11.2018
 *      Author: steiger-a
 */

#include "TxtAxisNRefSwitch.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"


namespace ft {


TxtAxisNRefSwitch::TxtAxisNRefSwitch(std::string name, FISH_X1_TRANSFER* pTArea, uint8_t chM, uint8_t chS1, uint8_t chS2)
	: TxtAxis(name, pTArea, chM, chS1), chS2X()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console_axes"), "{} TxtAxisNRefSwitch chM:{} chS1:{} chS2:{}",name,chM,chS1,chS2);
	chS2X.push_back(chS2);
}

TxtAxisNRefSwitch::TxtAxisNRefSwitch(std::string name, FISH_X1_TRANSFER* pTArea, uint8_t chM, uint8_t chS1, uint8_t chS2, uint8_t chS3)
	: TxtAxis(name, pTArea, chM, chS1), chS2X()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console_axes"), "{} TxtAxisNRefSwitch chM:{} chS1:{} chS2:{} chS3:{}",name,chM,chS1,chS2,chS3);
	chS2X.push_back(chS2);
	chS2X.push_back(chS3);
}

TxtAxisNRefSwitch::~TxtAxisNRefSwitch() {
	SPDLOG_LOGGER_TRACE(spdlog::get("console_axes"), "{} ~TxtAxisNRefSwitch",name);
}

void TxtAxisNRefSwitch::moveS2X(int idx)
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console_axes"), "{} moveS2X[{}]",name,idx);
	if (!isS2XValid(idx))
	{
		spdlog::get("console_axes")->error("{} Error: index {} for S2X is out of bounds!",name,idx);
		std::cout << "exit moveS2X" << std::endl;
		exit(1);
	}
	uint8_t chS2 = chS2X[idx];

	if (chS2 < 8)
	{
		assert(pTArea);
		pTArea->ftX1config.uni[chS2].mode = MODE_R; // Digital Switch with PullUp resistor
		pTArea->ftX1config.uni[chS2].digital = 1;
		pTArea->ftX1state.config_id ++; // Save the new Setup
	}
	else
	{
		assert(pTArea+1);
		(pTArea+1)->ftX1config.uni[chS2-8].mode = MODE_R; // Digital Switch with PullUp resistor
		(pTArea+1)->ftX1config.uni[chS2-8].digital = 1;
		(pTArea+1)->ftX1state.config_id ++; // Save the new Setup
	}
	SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} setup input chS2[{}]",name,chS2);

	//check switch ref
	if (chS2<8?pTArea->ftX1in.uni[chS2]:(pTArea+1)->ftX1in.uni[chS2-8] == 1) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		return;
	}

	SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} chS2[{}]",name,chS2);

	assert(pTArea);
	if ((status != AXIS_READY)&&(status != AXIS_NOREF)) {
		spdlog::get("console_axes")->error("{} Error: status != AXIS_READY. Exit function.",name);
		std::cout << "exit moveS2X 2" << std::endl;
		exit(1);
	}
	setMotorRight();
	SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} set motor chM[{}] {} {}",name,chM,
			chM<8?pTArea->ftX1out.duty[chM*2]:(pTArea+1)->ftX1out.duty[(chM-8)*2],
			chM<8?pTArea->ftX1out.duty[chM*2+1]:(pTArea+1)->ftX1out.duty[(chM-8)*2+1]);

	setStatus(AXIS_MOVING_S2X);

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
		if (chS2<8?pTArea->ftX1in.uni[chS2]:(pTArea+1)->ftX1in.uni[chS2-8] == 1)
		{
			setMotorOff();
			SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} pTArea->ftX1in.uni[chS2] == 1",name);
			break;
		}
		//check timeout
		auto end = std::chrono::system_clock::now();
		auto dur = end-start;
		auto diff_s = std::chrono::duration_cast< std::chrono::duration<float> >(dur).count();
		//SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "diff_s:{} diff_max:{}",diff_s,TIMEOUT_S_MOVEREF);
		if (diff_s > TIMEOUT_S_MOVERIGHT) {
			setStatus(AXIS_TIMEOUT_MOVERIGHT);
			spdlog::get("console_axes")->warn("{} diff_s > TIMEOUT_S_MOVERIGHT, diff_s:{} diff_max:{}",name,diff_s,TIMEOUT_S_MOVERIGHT);
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(CYCLE_MS_AXIS));
	}
	if (status == AXIS_MOVING_S2X)
	{
		resetCounter();
		pos = 0;
		setStatus(AXIS_READY);
	} else {
		std::string sst = toString(status);
		SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} setStatus:{}",name,sst);
		std::cout << "exit moveS2X 3" << std::endl;
		exit(1);
	}
	Notify();
}

void TxtAxisNRefSwitch::setMotorRight()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console_axes"), "{}({}) setMotorRight",name,status);
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

void TxtAxisNRefSwitch::moveRight(uint16_t steps, uint16_t* p) {
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
	SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} posa: {}",name,posa);

	//config last home switch
	uint16_t chS2 = chS2X[chS2X.size()-1];
	if (chS2 < 8)
	{
		assert(pTArea);
		pTArea->ftX1config.uni[chS2].mode = MODE_R; // Digital Switch with PullUp resistor
		pTArea->ftX1config.uni[chS2].digital = 1;
		pTArea->ftX1state.config_id ++; // Save the new Setup
	}
	else
	{
		assert(pTArea+1);
		(pTArea+1)->ftX1config.uni[chS2-8].mode = MODE_R; // Digital Switch with PullUp resistor
		(pTArea+1)->ftX1config.uni[chS2-8].digital = 1;
		(pTArea+1)->ftX1state.config_id ++; // Save the new Setup
	}

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

	setStatus(AXIS_MOVING_RIGHT);
	SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} setup duty right",name);

	//resetCounter();

	auto start = std::chrono::system_clock::now();
	while ((chM<8?pTArea->ftX1in.motor_ex_cmd_id[chM]:(pTArea+1)->ftX1in.motor_ex_cmd_id[chM-8])
		< (chM<8?pTArea->ftX1out.motor_ex_cmd_id[chM]:(pTArea+1)->ftX1out.motor_ex_cmd_id[chM-8]))
	{
		if (chS2<8?pTArea->ftX1in.uni[chS2]:(pTArea+1)->ftX1in.uni[chS2-8] == 1)
		{   // Input IX on Master Interface is "1"
			SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} end switch:{} duty 0 0",name,chM);
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
		*p = posa + chMcounter;
		setStatus(AXIS_READY);
	} else {
		std::string sst = toString(status);
		SPDLOG_LOGGER_DEBUG(spdlog::get("console_axes"), "{} setStatus:{}",name,sst);
		std::cout << "exit moveRight 2X" << std::endl;
		exit(1);
	}
}


} /* namespace ft */
