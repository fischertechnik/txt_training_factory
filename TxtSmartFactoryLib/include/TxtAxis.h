/*
 * TxtAxis.h
 *
 *  Created on: 08.11.2018
 *      Author: steiger-a
 */

#ifndef TxtAxis_H_
#define TxtAxis_H_


#include <stdio.h>          // for printf()
#include <unistd.h>         // for sleep()
#include <iostream>
#include <thread>
#include <mutex>

#include "KeLibTxtDl.h"     // TXT Lib
#include "FtShmem.h"        // TXT Transfer Area

#include "Observer.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"


namespace ft {


typedef enum
{
	AXIS_ERROR = -1,
	AXIS_NOREF = 0,
	AXIS_READY,
	AXIS_MOVING_LEFT,
	AXIS_MOVING_RIGHT,
	AXIS_MOVING_REF,
	AXIS_MOVING_S2X,
	AXIS_TIMEOUT_MOVEREF,
	AXIS_TIMEOUT_MOVELEFT,
	AXIS_TIMEOUT_MOVERIGHT
} TxtAxis_status_t;

inline const char * toString(TxtAxis_status_t st)
{
	switch(st)
	{
	case AXIS_ERROR: return "AXIS_ERROR";
	case AXIS_NOREF: return "AXIS_NOREF";
	case AXIS_READY: return "AXIS_READY";
	case AXIS_MOVING_LEFT: return "AXIS_MOVING_LEFT";
	case AXIS_MOVING_RIGHT: return "AXIS_MOVING_RIGHT";
	case AXIS_MOVING_REF: return "AXIS_MOVING_REF";
	case AXIS_MOVING_S2X: return "AXIS_MOVING_S2X";
	case AXIS_TIMEOUT_MOVEREF: return "AXIS_TIMEOUT_MOVEREF";
	case AXIS_TIMEOUT_MOVELEFT: return "AXIS_TIMEOUT_MOVELEFT";
	case AXIS_TIMEOUT_MOVERIGHT: return "AXIS_TIMEOUT_MOVERIGHT";
	default: return "UNKNOWN";
	}
}

//TODO
#define TIMEOUT_S_MOVEREF 30.0
#define TIMEOUT_S_MOVELEFT 30.0
#define TIMEOUT_S_MOVERIGHT 30.0

#define CYCLE_MS_AXIS 1

class TxtVacuumGripperRobot;
class TxtHighBayWarehouse;
class TxtAxis : public SubjectObserver {
	friend class TxtVacuumGripperRobot;
	friend class TxtHighBayWarehouse;
public:
	TxtAxis(std::string name, FISH_X1_TRANSFER* pTArea, uint8_t chM, uint8_t chS1);
	virtual ~TxtAxis();

	bool init();

	TxtAxis_status_t getStatus() { return status; }

	void setSpeed(int16_t s);
	int16_t getSpeed() { return speed; }

	uint16_t getPosAbs() { return pos; }

	/* move commands */
	void stop();

	virtual bool moveAbs(uint16_t p);
	std::thread moveAbsThread(uint16_t p) {
		return std::thread(&TxtAxis::moveAbs, this, p);
	}

	bool moveRel(int rp) {
		if (status == AXIS_READY)
		{
			if (((rp>0)&&((int)pos+rp<65535)) || //negative
				((rp<0)&&(pos>=-rp))) //positive
			{
				return moveAbs(pos+rp);
			}
		}
		return false;
	}

	void moveRef();
	std::thread moveRefThread() {
		return std::thread(&TxtAxis::moveRef, this);
	}

protected:
	void setStatus(TxtAxis_status_t status);

	void reset();
	void resetCounter();

	virtual void setMotorOff();
	virtual void setMotorLeft();

	virtual void moveLeft(uint16_t steps, uint16_t* pPos);
	virtual void moveRight(uint16_t steps, uint16_t* pPos) = 0;

	std::string name;
	FISH_X1_TRANSFER* pTArea;
	TxtAxis_status_t status;
	uint16_t pos;
	int16_t speed;
	bool stopReq;

	uint8_t chM;
	uint8_t chS1;
};


} /* namespace ft */


#endif /* TxtAxis_H_ */
