/*
 * TxtVacuumGripper.h
 *
 *  Created on: 07.02.2019
 *      Author: steiger-a
 */

#ifndef TXTVACUUMGRIPPER_H_
#define TXTVACUUMGRIPPER_H_

#include "KeLibTxtDl.h"     // TXT Lib
#include "FtShmem.h"        // TXT Transfer Area

#include "Observer.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"


namespace ft {


class TxtVacuumGripper {
public:
	/*!
	 * Contructor vacuum gripper.
	 * @param pTArea Pointer to Transfer Area.
	 * @param chComp Channel for compressor. master: 0-7 extension: 8-15
	 * @param chValve Channel for valve. master: 0-7 extension: 8-15
	 */
	TxtVacuumGripper(FISH_X1_TRANSFER* pTArea, uint8_t chComp, uint8_t chValve);
	virtual ~TxtVacuumGripper();

	void grip();
	void release();

protected:
	void setCompressor(bool on);

	FISH_X1_TRANSFER* pTArea;

	/* ports */
	uint8_t chComp;
	uint8_t chValve;
};


} /* namespace ft */


#endif /* TXTVACUUMGRIPPER_H_ */
