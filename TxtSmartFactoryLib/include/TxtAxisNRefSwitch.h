/*
 * TxtAxisNRefSwitch.h
 *
 *  Created on: 08.11.2018
 *      Author: steiger-a
 */

#ifndef TxtAxisNRefSwitch_H_
#define TxtAxisNRefSwitch_H_


#include <stdio.h>          // for printf()
#include <unistd.h>         // for sleep()
#include <iostream>
#include <thread>
#include <vector>

#include "KeLibTxtDl.h"     // TXT Lib
#include "FtShmem.h"        // TXT Transfer Area

#include "TxtAxis.h"


namespace ft {


class TxtAxisNRefSwitch : public TxtAxis {
	friend class TxtVacuumGripperRobot;
	friend class TxtHighBayWarehouse;
public:
	TxtAxisNRefSwitch(std::string name, FISH_X1_TRANSFER* pTArea, uint8_t chM, uint8_t chS1, uint8_t chS2);
	TxtAxisNRefSwitch(std::string name, FISH_X1_TRANSFER* pTArea, uint8_t chM, uint8_t chS1, uint8_t chS2, uint8_t chS3);
	virtual ~TxtAxisNRefSwitch();

	void moveS2X(int idx);
	void moveS2() { moveS2X(0); }
	std::thread moveS2Thread() {
		return std::thread(&TxtAxisNRefSwitch::moveS2, this);
	}
	void moveS3() { moveS2X(1); }
	std::thread moveS3Thread() {
		return std::thread(&TxtAxisNRefSwitch::moveS3, this);
	}

	bool isS2XValid(int idx) { return idx >= 0 && (size_t)idx < chS2X.size(); }

protected:
	virtual void setMotorRight();

	virtual void moveRight(uint16_t steps, uint16_t* pPos);

	std::vector<uint8_t> chS2X;
};


} /* namespace ft */


#endif /* TxtAxisNRefSwitch_H_ */
