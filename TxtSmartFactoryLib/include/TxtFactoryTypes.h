/*
 * TxtFactoryTypes.h
 *
 *  Created on: 15.03.2019
 *      Author: steiger-a
 */

#ifndef TxtFactoryTypes_H_
#define TxtFactoryTypes_H_

#include <string>
#include <vector>
#include <pthread.h>
#include <assert.h>
#include <map>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"


namespace ft {


typedef enum
{
	LEDS_OFF = 0,
	LEDS_READY = 1,
	LEDS_BUSY = 2,
	LEDS_ERROR = 4,
	LEDS_CALIB = 7,
	LEDS_WAIT_ERROR =  6,
	LEDS_WAIT_READY = 3
} TxtLEDSCode_t;

class EncPos3 {
public:
	EncPos3(uint16_t x=0, uint16_t y=0, uint16_t z=0) : x(x), y(y), z(z) {}

	uint16_t x, y, z;
};

class EncPos2{
public:
	EncPos2(uint16_t x=0, uint16_t y=0) : x(x), y(y) {}

	uint16_t x, y;
};

typedef enum
{
	WP_TYPE_NONE,
	WP_TYPE_WHITE,
	WP_TYPE_RED,
	WP_TYPE_BLUE
} TxtWPType_t;

inline const char * toString(TxtWPType_t v)
{
	switch(v) {
	case WP_TYPE_NONE: return "NONE";
	case WP_TYPE_WHITE: return "WHITE";
	case WP_TYPE_RED: return "RED";
	case WP_TYPE_BLUE: return "BLUE";
	default: return "";
	}
}

typedef enum
{
	//WP_STATE_NONE,
	WP_STATE_RAW,
	WP_STATE_PROCESSED,
	WP_STATE_REJECTED
} TxtWPState_t;

inline const char * toString(TxtWPState_t v)
{
	switch(v) {
	//case WP_STATE_NONE: return "NONE";
	case WP_STATE_RAW: return "RAW";
	case WP_STATE_PROCESSED: return "PROCESSED";
	case WP_STATE_REJECTED: return "REJECTED";
	default: return "";
	}
}

class TxtWorkpiece {
public:
	TxtWorkpiece()
		: tag_uid(""), type(WP_TYPE_NONE), state(WP_STATE_RAW) {}
	TxtWorkpiece(const TxtWorkpiece& wp)
		: tag_uid(wp.tag_uid), type(wp.type), state(wp.state) {};
	TxtWorkpiece(std::string tag_uid, TxtWPType_t type, TxtWPState_t state)
		: tag_uid(tag_uid), type(type), state(state) {}
	virtual ~TxtWorkpiece() {}

	void printDebug() {
		SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "WP uid:{} type:{} state:{}",tag_uid,toString(type), toString(state));
	}

	std::string tag_uid;
	TxtWPType_t type;
	TxtWPState_t state;
};

class TxtJoysticksData {
public:
	TxtJoysticksData(int aX1,int aY1,bool b1,int aX2,int aY2,bool b2)
		: aX1(aX1), aY1(aY1), b1(b1), aX2(aX2), aY2(aY2), b2(b2) {};
	TxtJoysticksData()
		: aX1(0), aY1(0), b1(false), aX2(0), aY2(0), b2(false) {};

	//copy constructor
	TxtJoysticksData(const TxtJoysticksData& jd) :
		aX1(jd.aX1),aY1(jd.aY1),b1(jd.b1), aX2(jd.aX2),aY2(jd.aY2),b2(jd.b2) {};

	int aX1;
	int aY1;
	bool b1;
	int aX2;
	int aY2;
	bool b2;
};

typedef std::map<std::string,TxtWorkpiece*> Stock_map_t;

typedef enum
{
	WAITING_FOR_ORDER,
	ORDERED,
	IN_PROCESS,
	SHIPPED
} TxtOrderState_t;

inline const char * toString(TxtOrderState_t v)
{
	switch(v) {
	case WAITING_FOR_ORDER: return "WAITING_FOR_ORDER";
	case ORDERED: return "ORDERED";
	case IN_PROCESS: return "IN_PROCESS";
	case SHIPPED: return "SHIPPED";
	default: return "[Unknown TxtOrderState_t]";
	}
}

struct TxtOrderState {
	TxtWPType_t type;
	TxtOrderState_t state;
};

typedef enum
{
	INVALID_INDEX = -1,
	DELIVERY_RAW_INDEX = 0,
	INSPECTION_INDEX = 1,
	WAREHOUSING_INDEX = 2,
	OUTSOURCING_INDEX = 3,
	PROCESSING_OVEN_INDEX = 4,
	PROCESSING_MILLING_INDEX = 5,
	SORTING_INDEX = 6,
	SHIPPING_INDEX = 7,
	NUM_INDEX_MAX
} TxtHistoryIndex_t;

typedef enum
{
	INVALID = 0,
	DELIVERY_RAW = 100,       // = "Anlieferung Rohware",
	INSPECTION = 200,         // = "Qualitaetskontrolle",
	WAREHOUSING = 300,        // = "Einlagerung",
	OUTSOURCING = 400,        // = "Auslagerung",
	PROCESSING_OVEN = 500,    // = "Bearbeitung Brennofen",
	PROCESSING_MILLING = 600, // = "Bearbeitung Fraese",
	SORTING = 700,            // = "Sortierung",
	SHIPPING = 800            // = "Versand Ware"
} TxtHistoryCode_t;

inline const TxtHistoryCode_t toCode(TxtHistoryIndex_t v)
{
	switch(v)
	{
	case DELIVERY_RAW_INDEX: return DELIVERY_RAW;
	case INSPECTION_INDEX: return INSPECTION;
	case WAREHOUSING_INDEX: return WAREHOUSING;
	case OUTSOURCING_INDEX: return OUTSOURCING;
	case PROCESSING_OVEN_INDEX: return PROCESSING_OVEN;
	case PROCESSING_MILLING_INDEX: return PROCESSING_MILLING;
	case SORTING_INDEX: return SORTING;
	case SHIPPING_INDEX: return SHIPPING;
	default: return INVALID;
	}
}

inline const char * toString(TxtHistoryCode_t v)
{
	switch(v)
	{
	case DELIVERY_RAW: return "DELIVERY_RAW";
	case INSPECTION: return "INSPECTION";
	case WAREHOUSING: return "WAREHOUSING";
	case OUTSOURCING: return "OUTSOURCING";
	case PROCESSING_OVEN: return "PROCESSING_OVEN";
	case PROCESSING_MILLING: return "PROCESSING_MILLING";
	case SORTING: return "SORTING";
	case SHIPPING: return "SHIPPING";
	default: return "[Unknown TxtHistoryCode_t]";
	}
}

typedef std::map<TxtHistoryCode_t,int64_t> History_map_t;


} /* namespace ft */


#endif /* TxtFactoryTypes_H_ */
