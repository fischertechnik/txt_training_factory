/*
 * TxtAxis1RefSwitch.h
 *
 *  Created on: 08.11.2018
 *      Author: steiger-a
 */

#ifndef TXTAXIS1REFSWITCH_H_
#define TXTAXIS1REFSWITCH_H_


#include <stdio.h>          // for printf()
#include <unistd.h>         // for sleep()
#include <iostream>
#include <thread>

#include "KeLibTxtDl.h"     // TXT Lib
#include "FtShmem.h"        // TXT Transfer Area

#include "TxtAxis.h"


namespace ft {


class TxtAxis1RefSwitch : public TxtAxis {
	friend class TxtVacuumGripperRobot;
	friend class TxtHighBayWarehouse;
public:
	TxtAxis1RefSwitch(std::string name, FISH_X1_TRANSFER* pTArea, uint8_t chM, uint8_t chS1, uint16_t posEnd);
	virtual ~TxtAxis1RefSwitch();

	bool moveAbs(uint16_t);

	uint16_t getPosEnd() { return posEnd; }

protected:
	virtual void setMotorRight();

	virtual void moveRight(uint16_t steps, uint16_t* pPos);

	uint16_t posEnd;
};


} /* namespace ft */


#endif /* TXTAXIS1REFSWITCH_H_ */
