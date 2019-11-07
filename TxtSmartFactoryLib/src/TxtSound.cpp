/*
 * TxtSound.cpp
 *
 *  Created on: 10.10.2019
 *      Author: steiger-a
 */

#include "TxtSound.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"


namespace ft {


void TxtSound::play(FISH_X1_TRANSFER* pTArea, int r,  int num)
{
	assert(pTArea);
	for (int i = 0; i < r; i++)
	{
		pTArea->sTxtOutputs.u16SoundCmdId = i;
		pTArea->sTxtOutputs.u16SoundIndex = num;
		pTArea->sTxtOutputs.u16SoundRepeat = 0;
		pTArea->sTxtOutputs.u16SoundCmdId++;
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		while(pTArea->sTxtInputs.u16SoundCmdId == i)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}


TxtSound::TxtSound(TxtTransfer* pT) : pT(pT), mute(false)
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "TxtSound",0);
}

TxtSound::~TxtSound()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "~TxtSound",0);
}

void TxtSound::play(int num, int repeat)
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "play num:{} repeat:{}",num,repeat);
	if (!mute)
	{
		assert(pT);
		assert(pT->pTArea);
		pT->pTArea->sTxtOutputs.u16SoundIndex = num;
		pT->pTArea->sTxtOutputs.u16SoundRepeat = repeat;
		pT->pTArea->sTxtOutputs.u16SoundCmdId++;
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	} else {
		std::cout << "sound is disabled." << std::endl;
	}
}


} /* namespace ft */
