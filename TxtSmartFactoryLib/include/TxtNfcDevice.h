/*
 * TxtNfcDevice.h
 *
 *  Created on: 09.02.2019
 *      Author: steiger-a
 */

#ifndef TXTNFCDEVICE_H_
#define TXTNFCDEVICE_H_

#include <err.h>
#include <stdlib.h>
#include <nfc/nfc.h>
#include <freefare.h>

#include <map>
#include <string>
#include <iostream>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "Observer.h"
#include "TxtMqttFactoryClient.h"
#include "TxtFactoryTypes.h"

#include "nfc-utils.h"


#define MAX_DEVICE_COUNT 16
#define MAX_TARGET_COUNT 16


namespace ft {


typedef union ts_u
{
    uint8_t u8[8];
    int64_t s64;
} uTS;

#define N_MAX_TS 8

class TxtNfcData {
public:
	TxtNfcData() : wp(), uts(), mask_ts(0) {}
	virtual ~TxtNfcData() {}

	TxtWorkpiece wp;
	uTS uts[N_MAX_TS];
	uint8_t mask_ts;
};


class TxtNfcDevice : public SubjectObserver {
public:
	TxtNfcDevice();
	virtual ~TxtNfcDevice();

	bool open();
	void close();

	std::string readTagsGetUID();

	bool eraseTags();
	std::string readTags();
	bool writeTags(TxtWorkpiece wp, std::vector<uTS> vuts, uint8_t mask_ts);

    TxtNfcData* getNfcData() { return nfcData; }
    void printNfcData();

	void publish() { Notify(); }

protected:
	void printRawData(uint8_t* buffer);

	bool opened;
	nfc_device *pnd;
	nfc_context *context;
	TxtNfcData* nfcData;
};


class TxtNfcDeviceObserver : public ft::Observer {
public:
	TxtNfcDeviceObserver(ft::TxtNfcDevice* s, ft::TxtMqttFactoryClient* mqttclient)
		: _subject(s), _mqttclient(mqttclient)
	{
		SPDLOG_LOGGER_TRACE(spdlog::get("console"), "TxtNfcDeviceObserver",0);
		_subject->Attach(this);
	}
	virtual ~TxtNfcDeviceObserver() {
		SPDLOG_LOGGER_TRACE(spdlog::get("console"), "~TxtNfcDeviceObserver",0);
		_subject->Detach(this);
	}
	void Update(ft::SubjectObserver* theChangedSubject) {
		if(theChangedSubject == _subject) {
			//SPDLOG_LOGGER_TRACE(spdlog::get("console"), "Update 1",0);
			assert(_mqttclient);
			TxtNfcData* nfcData = _subject->getNfcData();
			if (nfcData)
			{
				TxtWorkpiece wp = nfcData->wp;
				History_map_t map_hist;
				for(unsigned int i = 0; i<NUM_INDEX_MAX; i++)
				{
					if (((nfcData->mask_ts >> i) & 0x1) == 1)
					{
						int64_t hts = nfcData->uts[i].s64;
						TxtHistoryCode_t code = toCode((TxtHistoryIndex_t)i);
						map_hist[code] = hts;
					}
				}
				_mqttclient->publishNfcDS(wp, map_hist, TIMEOUT_MS_PUBLISH);
			}
			//SPDLOG_LOGGER_TRACE(spdlog::get("console"), "Update 2",0);
		}
	}
private:
	ft::TxtNfcDevice *_subject;
	ft::TxtMqttFactoryClient* _mqttclient;
};


} /* namespace ft */


#endif /* TXTNFCDEVICE_H_ */
