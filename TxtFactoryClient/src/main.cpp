#include "TxtHighBayWarehouse.h"
#include "TxtVacuumGripperRobot.h"
#include "TxtMultiProcessingStation.h"
#include "TxtSortingLine.h"
#include "TxtMqttFactoryClient.h"
#include "TxtJoystickXYBController.h"
#include "Utils.h"
#include "TxtAxis.h"
#include "TxtSound.h"

#include "KeLibTxtDl.h"     // TXT Lib
#include "FtShmem.h"        // TXT Transfer Area

#include <stdio.h>          // for printf()
#include <string.h>         // for memset()
#include <unistd.h>         // for sleep()
#include <cmath>			// for pow()
#include <fstream>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/async.h"

// Version info
#define VERSION_HEX ((0<<16)|(8<<8)|(1<<0))
char TxtAppVer[32];

unsigned int DebugFlags;
FILE *DebugFile;

FISH_X1_TRANSFER* pTArea = NULL;
ft::TxtMqttFactoryClient* pcli = NULL;

#ifdef CLIENT_MPO
	ft::TxtMultiProcessingStation* pMPO = NULL;
#elif CLIENT_HBW
	ft::TxtHighBayWarehouse* pHBW = NULL;
#elif CLIENT_VGR
	ft::TxtVacuumGripperRobot* pVGR = NULL;
#elif CLIENT_SLD
	ft::TxtSortingLine* pSLD = NULL;
#else
	#error Set CLIENT_XXX define first!
#endif

#define TIMEOUT_CONNECTION_MS 60000 //60 s

bool first_message_arrived = false;
bool first_message_subscribe = false;

class callback : public virtual mqtt::callback
{
	ft::TxtMqttFactoryClient& cli_;
#ifdef CLIENT_MPO
	ft::TxtMultiProcessingStation& mpo_;
#elif CLIENT_HBW
	ft::TxtHighBayWarehouse& hbw_;
#elif CLIENT_VGR
	ft::TxtVacuumGripperRobot& vgr_;
#elif CLIENT_SLD
	ft::TxtSortingLine& sld_;
#else
	#error Set CLIENT_XXX define first!
#endif

	void connected(const std::string& cause) override {
		SPDLOG_LOGGER_TRACE(spdlog::get("console"), "connected: {}", cause);
		long timeout_ms = TIMEOUT_CONNECTION_MS;
		std::cout << "Subscribe MQTTClient" << std::endl;
		assert(pcli);
		pcli->start_consume(timeout_ms);
		first_message_subscribe = true;
	}

	// Callback for when the connection is lost.
	// This will initiate the attempt to manually reconnect.
	void connection_lost(const std::string& cause) override {
		SPDLOG_LOGGER_TRACE(spdlog::get("console"), "connection_lost: {}", cause);
	}

	// Callback for when a message arrives.
	void message_arrived(mqtt::const_message_ptr msg) override {
		assert(msg);
		SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "message_arrived  message:{} payload:{}", msg->get_topic(), msg->to_string());
		//BUGFIX: msg->get_topic() is empty
		//FIX paho.mqtt.cpp: https://github.com/eclipse/paho.mqtt.c/issues/440#issuecomment-380161713

		first_message_arrived = true;

#ifdef CLIENT_MPO
		if (msg->get_topic() == TOPIC_OUTPUT_STATE_ACK) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED state ack:{}", msg->get_topic());
			std::stringstream ssin(msg->to_string());
			Json::Value root;
			try {
				ssin >> root;
				std::string sts = root["ts"].asString();
				SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  ts:{}", sts);

				if (ft::trycheckTimestampTTL(sts))
				{
					mpo_.requestQuit();
				}
			} catch (const Json::RuntimeError& exc) {
				std::cout << "Error: " << exc.what() << std::endl;
			}
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "OK.", 0);
		} else if (msg->get_topic() == TOPIC_LOCAL_VGR_DO) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED vgr do:{}", msg->get_topic());
			std::stringstream ssin(msg->to_string());
			Json::Value root;
			try {
				ssin >> root;
				std::string sts = root["ts"].asString();
				ft::TxtVgrDoCode_t code = (ft::TxtVgrDoCode_t)root["code"].asInt();
				SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  ts:{} code:{}", sts, (int)code);

				if (ft::trycheckTimestampTTL(sts))
				{
					ft::TxtWorkpiece* wp = NULL;
					if (root["workpiece"] != Json::Value::null) {
						wp = new ft::TxtWorkpiece();
						wp->tag_uid = root["workpiece"]["id"].asString();
						std::string stype = root["workpiece"]["type"].asString();
						if (stype == "WHITE") {
							wp->type = ft::WP_TYPE_WHITE;
						} else if(stype == "RED") {
							wp->type = ft::WP_TYPE_RED;
						} else if (stype == "BLUE") {
							wp->type = ft::WP_TYPE_BLUE;
						} else {
							wp->type = ft::WP_TYPE_NONE;
						}
						std::string sstate = root["workpiece"]["state"].asString();
						if (sstate == "RAW") {
							wp->state = ft::WP_STATE_RAW;
						} else if(sstate == "PROCESSED") {
							wp->state = ft::WP_STATE_PROCESSED;
						} else if (sstate == "REJECTED") {
							wp->state = ft::WP_STATE_REJECTED;
						}
					}

					switch(code)
					{
					case ft::VGR_EXIT:
						mpo_.requestExit("VGR");
						break;
					case ft::VGR_MPO_PRODUCE:
						mpo_.requestVGRproduce(wp);
						break;
					default:
						break;
					}
				}
			} catch (const Json::RuntimeError& exc) {
				std::cout << "Error: " << exc.what() << std::endl;
			}
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "OK.", 0);
		} else if (msg->get_topic() == TOPIC_LOCAL_SLD_ACK) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED SLD ack:{}", msg->get_topic());
			std::stringstream ssin(msg->to_string());
			Json::Value root;
			try {
				ssin >> root;
				std::string sts = root["ts"].asString();
				ft::TxtSldAckCode_t code = (ft::TxtSldAckCode_t)root["code"].asInt();
				SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  ts:{} code:{}", sts, (int)code);

				if (ft::trycheckTimestampTTL(sts))
				{
					switch (code)
					{
					case ft::SLD_EXIT:
						mpo_.requestExit("SLD");
						break;
					case ft::SLD_STARTED:
						mpo_.requestSLDstarted();
						break;
					default:
						break;
					}
				}
			} catch (const Json::RuntimeError& exc) {
				std::cout << "Error: " << exc.what() << std::endl;
			}
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "OK.", 0);
		} else {
			std::cout << "Unknown topic: " << msg->get_topic() << std::endl;
			spdlog::get("file_logger")->error("Unknown topic: {}",msg->get_topic());
			exit(1);
		}
#elif CLIENT_HBW
		if (msg->get_topic() == TOPIC_OUTPUT_STATE_ACK) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED state ack:{}", msg->get_topic());
			std::stringstream ssin(msg->to_string());
			Json::Value root;
			try {
				ssin >> root;
				std::string sts = root["ts"].asString();
				SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  ts:{}", sts);
				if (ft::trycheckTimestampTTL(sts))
				{
					hbw_.requestQuit();
				}
			} catch (const Json::RuntimeError& exc) {
				std::cout << "Error: " << exc.what() << std::endl;
			}
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "OK.", 0);
		} else if (msg->get_topic() == TOPIC_LOCAL_SSC_JOY) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED joy:{}", msg->get_topic());
			std::stringstream ssin(msg->to_string());
			Json::Value root;
			try {
				ssin >> root;
				std::string sts = root["ts"].asString();
				SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  ts:{}", sts);
				if (ft::trycheckTimestampTTL(sts))
				{
					ft::TxtJoysticksData jd;
					jd.aX1 = root["aX1"].asInt();
					jd.aY1 = root["aY1"].asInt();
					jd.b1 = root["b1"].asBool();
					jd.aX2 = root["aX2"].asInt();
					jd.aY2 = root["aY2"].asInt();
					jd.b2 = root["b2"].asBool();
					hbw_.requestJoyBut(jd);
					SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  1:{} {} {} 2:{} {} {}", jd.aX1, jd.aY1, jd.b1, jd.aX2, jd.aY2, jd.b2);
				}
			} catch (const Json::RuntimeError& exc) {
				std::cout << "Error: " << exc.what() << std::endl;
			}
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "OK.", 0);
		} else if (msg->get_topic() == TOPIC_LOCAL_VGR_DO) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED vgr do:{}", msg->get_topic());
			std::stringstream ssin(msg->to_string());
			Json::Value root;
			try {
				ssin >> root;
				std::string sts = root["ts"].asString();
				ft::TxtVgrDoCode_t code = (ft::TxtVgrDoCode_t)root["code"].asInt();
				SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  ts:{} code:{}", sts, (int)code);

				if (ft::trycheckTimestampTTL(sts))
				{
					ft::TxtWorkpiece* wp = NULL;
					if (root["workpiece"] != Json::Value::null) {
						wp = new ft::TxtWorkpiece();
						wp->tag_uid = root["workpiece"]["id"].asString();
						std::string stype = root["workpiece"]["type"].asString();
						if (stype == "WHITE") {
							wp->type = ft::WP_TYPE_WHITE;
						} else if(stype == "RED") {
							wp->type = ft::WP_TYPE_RED;
						} else if (stype == "BLUE") {
							wp->type = ft::WP_TYPE_BLUE;
						} else {
							wp->type = ft::WP_TYPE_NONE;
						}
						std::string sstate = root["workpiece"]["state"].asString();
						if (sstate == "RAW") {
							wp->state = ft::WP_STATE_RAW;
						} else if(sstate == "PROCESSED") {
							wp->state = ft::WP_STATE_PROCESSED;
						} else if (sstate == "REJECTED") {
							wp->state = ft::WP_STATE_REJECTED;
						}
					}

					switch(code)
					{
					case ft::VGR_EXIT:
						hbw_.requestExit("VGR");
						break;
					case ft::VGR_HBW_FETCHCONTAINER:
						hbw_.requestVGRfetchContainer(wp);
						break;
					case ft::VGR_HBW_STORE_WP:
						hbw_.requestVGRstore(wp);
						break;
					case ft::VGR_HBW_FETCH_WP:
						hbw_.requestVGRfetch(wp);
						break;
					case ft::VGR_HBW_STORECONTAINER:
						hbw_.requestVGRstoreContainer(wp);
						break;
					case ft::VGR_HBW_CALIB:
						hbw_.requestVGRcalib();
						break;
					case ft::VGR_HBW_RESETSTORAGE:
						hbw_.requestVGRresetStorage();
						break;
					default:
						break;
					}
				}
			} catch (const Json::RuntimeError& exc) {
				std::cout << "Error: " << exc.what() << std::endl;
			}
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "OK.", 0);
		} else {
			std::cout << "Unknown topic: " << msg->get_topic() << std::endl;
			spdlog::get("file_logger")->error("Unknown topic: {}",msg->get_topic());
			exit(1);
		}
#elif CLIENT_VGR
		if (msg->get_topic() == TOPIC_OUTPUT_STATE_ACK) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED state ack:{}", msg->get_topic());
			std::stringstream ssin(msg->to_string());
			Json::Value root;
			try {
				ssin >> root;
				std::string sts = root["ts"].asString();
				SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  ts:{}", sts);
				if (ft::trycheckTimestampTTL(sts))
				{
					vgr_.requestQuit();
				}
			} catch (const Json::RuntimeError& exc) {
				std::cout << "Error: " << exc.what() << std::endl;
			}
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "OK.", 0);
		} else if (msg->get_topic() == TOPIC_OUTPUT_ORDER) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED order:{}", msg->get_topic());
			std::stringstream ssin(msg->to_string());
			Json::Value root;
			try {
				ssin >> root;
				std::string sts = root["ts"].asString();
				SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  ts:{}", sts);
				if (ft::trycheckTimestampTTL(sts))
				{
					std::string stype = root["type"].asString();
					SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  type:{}", stype);
					if (stype == "WHITE")
					{
						vgr_.requestOrder(ft::WP_TYPE_WHITE);
					} else if(stype == "RED")
					{
						vgr_.requestOrder(ft::WP_TYPE_RED);
					} else if (stype == "BLUE")
					{
						vgr_.requestOrder(ft::WP_TYPE_BLUE);
					}
				}
			} catch (const Json::RuntimeError& exc) {
				std::cout << "Error: " << exc.what() << std::endl;
			}
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "OK.", 0);
		} else if (msg->get_topic() == TOPIC_OUTPUT_NFC_DS) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED nfc ds:{}", msg->get_topic());
			std::stringstream ssin(msg->to_string());
			Json::Value root;
			try {
				ssin >> root;
				std::string sts = root["ts"].asString();
				SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  ts:{}", sts);
				if (ft::trycheckTimestampTTL(sts))
				{
					std::string scmd = root["cmd"].asString();
					SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  cmd:{}", scmd);
					if (scmd == "read")
					{
						vgr_.requestNfcRead();
					} else if (scmd == "delete") {
						vgr_.requestNfcDelete();
					}
				}
			} catch (const Json::RuntimeError& exc) {
				std::cout << "Error: " << exc.what() << std::endl;
			}
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "OK.", 0);
		} else if (msg->get_topic() == TOPIC_LOCAL_SSC_JOY) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED joy but:{}", msg->get_topic());
			std::stringstream ssin(msg->to_string());
			Json::Value root;
			try {
				ssin >> root;
				std::string sts = root["ts"].asString();
				SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  ts:{}", sts);
				if (ft::trycheckTimestampTTL(sts))
				{
					ft::TxtJoysticksData jd;
					jd.aX1 = root["aX1"].asInt();
					jd.aY1 = root["aY1"].asInt();
					jd.b1 = root["b1"].asBool();
					jd.aX2 = root["aX2"].asInt();
					jd.aY2 = root["aY2"].asInt();
					jd.b2 = root["b2"].asBool();
					vgr_.requestJoyBut(jd);
					SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  1:{} {} {} 2:{} {} {}", jd.aX1, jd.aY1, jd.b1, jd.aX2, jd.aY2, jd.b2);
				}
			} catch (const Json::RuntimeError& exc) {
				std::cout << "Error: " << exc.what() << std::endl;
			}
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "OK.", 0);
		} else if (msg->get_topic() == TOPIC_LOCAL_MPO_ACK) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED MPO ack:{}", msg->get_topic());
			std::stringstream ssin(msg->to_string());
			Json::Value root;
			try {
				ssin >> root;
				std::string sts = root["ts"].asString();
				ft::TxtMpoAckCode_t code = (ft::TxtMpoAckCode_t)root["code"].asInt();
				SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  ts:{} code:{}", sts, (int)code);

				if (ft::trycheckTimestampTTL(sts))
				{
					ft::TxtWorkpiece* wp = NULL;
					if (root["workpiece"] != Json::Value::null) {
						wp = new ft::TxtWorkpiece();
						wp->tag_uid = root["workpiece"]["id"].asString();
						std::string stype = root["workpiece"]["type"].asString();
						if (stype == "WHITE") {
							wp->type = ft::WP_TYPE_WHITE;
						} else if(stype == "RED") {
							wp->type = ft::WP_TYPE_RED;
						} else if (stype == "BLUE") {
							wp->type = ft::WP_TYPE_BLUE;
						} else {
							wp->type = ft::WP_TYPE_NONE;
						}
						std::string sstate = root["workpiece"]["state"].asString();
						if (sstate == "RAW") {
							wp->state = ft::WP_STATE_RAW;
						} else if(sstate == "PROCESSED") {
							wp->state = ft::WP_STATE_PROCESSED;
						} else if (sstate == "REJECTED") {
							wp->state = ft::WP_STATE_REJECTED;
						}
					}

					switch (code)
					{
					case ft::MPO_EXIT:
						vgr_.requestExit("MPO");
						break;
					case ft::MPO_STARTED:
						vgr_.requestMPOstarted(wp);
						break;
					default:
						break;
					}
				}
			} catch (const Json::RuntimeError& exc) {
				std::cout << "Error: " << exc.what() << std::endl;
			}
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "OK.", 0);
		} else if (msg->get_topic() == TOPIC_LOCAL_HBW_ACK) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED HBW ack:{}", msg->get_topic());
			std::stringstream ssin(msg->to_string());
			Json::Value root;
			try {
				ssin >> root;
				std::string sts = root["ts"].asString();
				if (ft::trycheckTimestampTTL(sts))
				{
					ft::TxtHbwAckCode_t code = (ft::TxtHbwAckCode_t)root["code"].asInt();
					SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  ts:{} code:{}", sts, (int)code);

					ft::TxtWorkpiece* wp = NULL;
					if (root["workpiece"] != Json::Value::null) {
						wp = new ft::TxtWorkpiece();
						wp->tag_uid = root["workpiece"]["id"].asString();
						std::string stype = root["workpiece"]["type"].asString();
						if (stype == "WHITE") {
							wp->type = ft::WP_TYPE_WHITE;
						} else if(stype == "RED") {
							wp->type = ft::WP_TYPE_RED;
						} else if (stype == "BLUE") {
							wp->type = ft::WP_TYPE_BLUE;
						} else {
							wp->type = ft::WP_TYPE_NONE;
						}
						std::string sstate = root["workpiece"]["state"].asString();
						if (sstate == "RAW") {
							wp->state = ft::WP_STATE_RAW;
						} else if(sstate == "PROCESSED") {
							wp->state = ft::WP_STATE_PROCESSED;
						} else if (sstate == "REJECTED") {
							wp->state = ft::WP_STATE_REJECTED;
						}
					}
					switch (code)
					{
					case ft::HBW_EXIT:
						vgr_.requestExit("HBW");
						break;
					case ft::HBW_STORED:
						vgr_.requestHBWstored(wp);
						break;
					case ft::HBW_FETCHED:
						vgr_.requestHBWfetched(wp);
						break;
					case ft::HBW_CALIB_NAV:
						vgr_.requestHBWcalib_nav();
						break;
					case ft::HBW_CALIB_END:
						vgr_.requestHBWcalib_end();
						break;
					default:
						break;
					}
				}
			} catch (const Json::RuntimeError& exc) {
				std::cout << "Error: " << exc.what() << std::endl;
			}
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "OK.", 0);
		} else if (msg->get_topic() == TOPIC_LOCAL_SLD_ACK) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED SLD ack:{}", msg->get_topic());
			std::stringstream ssin(msg->to_string());
			Json::Value root;
			try {
				ssin >> root;
				std::string sts = root["ts"].asString();
				if (ft::trycheckTimestampTTL(sts))
				{
					ft::TxtSldAckCode_t code = (ft::TxtSldAckCode_t)root["code"].asInt();
					SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  ts:{} code:{}", sts, (int)code);

					std::string stype = root["type"].asString();
					SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  type:{}", stype);
					ft::TxtWPType_t type = ft::WP_TYPE_NONE;
					if (stype == "WHITE") {
						type = ft::WP_TYPE_WHITE;
					} else if(stype == "RED") {
						type = ft::WP_TYPE_RED;
					} else if (stype == "BLUE") {
						type = ft::WP_TYPE_BLUE;
					}
					switch (code)
					{
					case ft::SLD_EXIT:
						vgr_.requestExit("SLD");
						break;
					case ft::SLD_SORTED:
						vgr_.requestSLDsorted(type);
						break;
					case ft::SLD_CALIB_END:
						vgr_.requestSLDcalib_end();
						break;
					default:
						break;
					}
				}
			} catch (const Json::RuntimeError& exc) {
				std::cout << "Error: " << exc.what() << std::endl;
			}
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "OK.", 0);
		} else {
			std::cout << "Unknown topic: " << msg->get_topic() << std::endl;
			spdlog::get("file_logger")->error("Unknown topic: {}",msg->get_topic());
			exit(1);
		}
#elif CLIENT_SLD
		if (msg->get_topic() == TOPIC_OUTPUT_STATE_ACK) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED state ack:{}", msg->get_topic());
			std::stringstream ssin(msg->to_string());
			Json::Value root;
			try {
				ssin >> root;
				std::string sts = root["ts"].asString();
				SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  ts:{}", sts);
				if (ft::trycheckTimestampTTL(sts))
				{
					sld_.requestQuit();
				}
			} catch (const Json::RuntimeError& exc) {
				std::cout << "Error: " << exc.what() << std::endl;
			}
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "OK.", 0);
		} else if (msg->get_topic() == TOPIC_LOCAL_SSC_JOY) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED joy:{}", msg->get_topic());
			std::stringstream ssin(msg->to_string());
			Json::Value root;
			try {
				ssin >> root;
				std::string sts = root["ts"].asString();
				SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  ts:{}", sts);
				if (ft::trycheckTimestampTTL(sts))
				{
					ft::TxtJoysticksData jd;
					jd.aX1 = root["aX1"].asInt();
					jd.aY1 = root["aY1"].asInt();
					jd.b1 = root["b1"].asBool();
					jd.aX2 = root["aX2"].asInt();
					jd.aY2 = root["aY2"].asInt();
					jd.b2 = root["b2"].asBool();
					sld_.requestJoyBut(jd);
					SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  1:{} {} {} 2:{} {} {}", jd.aX1, jd.aY1, jd.b1, jd.aX2, jd.aY2, jd.b2);
				}
			} catch (const Json::RuntimeError& exc) {
				std::cout << "Error: " << exc.what() << std::endl;
			}
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "OK.", 0);
		} else if (msg->get_topic() == TOPIC_LOCAL_MPO_ACK) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED mpo ack:{}", msg->get_topic());
			std::stringstream ssin(msg->to_string());
			Json::Value root;
			try {
				ssin >> root;
				std::string sts = root["ts"].asString();
				if (ft::trycheckTimestampTTL(sts))
				{
					ft::TxtMpoAckCode_t code = (ft::TxtMpoAckCode_t)root["code"].asInt();
					SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  ts:{} code:{}", sts, (int)code);
					switch (code)
					{
					case ft::MPO_PRODUCED:
						sld_.requestMPOproduced();
						break;
					default:
						break;
					}
				}
			} catch (const Json::RuntimeError& exc) {
				std::cout << "Error: " << exc.what() << std::endl;
			}
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "OK.", 0);
		} else if (msg->get_topic() == TOPIC_LOCAL_VGR_DO) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED vgr do:{}", msg->get_topic());
			std::stringstream ssin(msg->to_string());
			Json::Value root;
			try {
				ssin >> root;
				std::string sts = root["ts"].asString();
				ft::TxtVgrDoCode_t code = (ft::TxtVgrDoCode_t)root["code"].asInt();
				SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  ts:{} code:{}", sts, (int)code);
				if (ft::trycheckTimestampTTL(sts))
				{
					switch(code)
					{
					case ft::VGR_EXIT:
						sld_.requestExit("VGR");
						break;
					case ft::VGR_SLD_START:
						sld_.requestVGRstart();
						break;
					case ft::VGR_SLD_CALIB:
						sld_.requestVGRcalib();
						break;
					default:
						break;
					}
				}
			} catch (const Json::RuntimeError& exc) {
				std::cout << "Error: " << exc.what() << std::endl;
			}
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "OK.", 0);
		} else {
			std::cout << "Unknown topic: " << msg->get_topic() << std::endl;
			spdlog::get("file_logger")->error("Unknown topic: {}",msg->get_topic());
			exit(1);
		}
#else
	#error Set CLIENT_XXX define first!
#endif
	}

	void delivery_complete(mqtt::delivery_token_ptr token) override {
		if (token) {
			SPDLOG_LOGGER_TRACE(spdlog::get("console"), "delivery_complete: {}: {}", token->get_message_id(), token->get_message()->get_topic());
		} else {
			SPDLOG_LOGGER_TRACE(spdlog::get("console"), "delivery token is NULL",0);
		}
	}

public:
#ifdef CLIENT_MPO
	callback(ft::TxtMqttFactoryClient& cli, ft::TxtMultiProcessingStation& mpo) : cli_(cli), mpo_(mpo) {}
#elif CLIENT_HBW
	callback(ft::TxtMqttFactoryClient& cli, ft::TxtHighBayWarehouse& hbw) : cli_(cli), hbw_(hbw) {}
#elif CLIENT_VGR
	callback(ft::TxtMqttFactoryClient& cli, ft::TxtVacuumGripperRobot& vgr) : cli_(cli), vgr_(vgr) {}
#elif CLIENT_SLD
	callback(ft::TxtMqttFactoryClient& cli, ft::TxtSortingLine& sld) : cli_(cli), sld_(sld) {}
#else
	#error Set CLIENT_XXX define first!
#endif
};

int main(int argc, char* argv[])
{
	std::string clientName;
#ifdef CLIENT_MPO
	clientName = "TxtFactoryMPO";
#elif CLIENT_HBW
	clientName = "TxtFactoryHBW";
#elif CLIENT_VGR
	clientName = "TxtFactoryVGR";
#elif CLIENT_SLD
	clientName = "TxtFactorySLD";
#else
	#error Set CLIENT_XXX define first!
#endif

	sprintf(TxtAppVer,"%d.%d.%d",
			(VERSION_HEX>>16)&0xff,(VERSION_HEX>>8)&0xff,(VERSION_HEX>>0)&0xff);
	fprintf(stdout,"%s V%d.%d.%d\n",clientName.c_str(),
			(VERSION_HEX>>16)&0xff,(VERSION_HEX>>8)&0xff,(VERSION_HEX>>0)&0xff);

	try
	{
		//can be set globaly or per logger(logger->set_error_handler(..))
		spdlog::set_error_handler([](const std::string& msg)
	    {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "err handler spdlog:{}", msg);
	    });

		auto file_logger = spdlog::basic_logger_mt<spdlog::async_factory>("file_logger", "Data/"+clientName+".log", true);
		spdlog::get("file_logger")->set_level(spdlog::level::trace);
		spdlog::get("file_logger")->info("{} {}", clientName.c_str(), TxtAppVer);

		// Console logger with color
		auto console = spdlog::stdout_color_mt("console");
		auto console_axes = spdlog::stdout_color_mt("console_axes");
		//spdlog::set_formatter();
		spdlog::set_pattern("[%t][%Y-%m-%d %T.%e][%L] %v");
		spdlog::set_level(spdlog::level::trace);
		console_axes->set_level(spdlog::level::trace);//trace);
	}
	catch (const spdlog::spdlog_ex& ex)
	{
		std::cout << "Log initialization failed: " << ex.what() << std::endl;
	}

	//load config
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;
    const std::string filenameConfig = "Data/Config.Client.json";
    std::ifstream test(filenameConfig, std::ifstream::binary);
    if (test.is_open()) {
        std::cout << "load file " << filenameConfig << std::endl;
        bool ok = Json::parseFromStream(builder, test, &root, &errs);
        if ( !ok )
        {
            std::cout  << errs << "\n";
        }
    }
    bool sound_enable = root.get("sound", true ).asBool();
    std::string host = root.get("host", "192.168.0.10" ).asString();
    int port = root.get("port", 1883 ).asInt();
    std::string mqtt_user = root.get("mqtt_user", "txt" ).asString();
    mqtt::binary_ref mqtt_pass = root.get("mqtt_pass", "xtx" ).asString();
    std::cout << "sound:" << sound_enable
    	<< " host:" << host
		<< " port:" << port
		<< " mqtt_user:" << mqtt_user
		<< " mqtt_pass:" << mqtt_pass << std::endl
		<< std::endl;

    if (StartTxtDownloadProg() == KELIB_ERROR_NONE)
    {
        pTArea = GetKeLibTransferAreaMainAddress();

        if (pTArea)
        {
		    try {

				std::cout << "Init MQTTClient" << std::endl;
				std::stringstream sout_port;
				sout_port << port;
				ft::TxtMqttFactoryClient mqttclient(clientName, host, sout_port.str(), mqtt_user, mqtt_pass);
				pcli = &mqttclient;

				ft::TxtTransfer T(pTArea);
#ifdef CLIENT_MPO
		    	ft::TxtMultiProcessingStation mpo(&T, pcli);
		    	mpo.sound.enable(sound_enable);
		    	pMPO = &mpo;
				callback cb(mqttclient, mpo);
				mqttclient.set_callback(cb); //set callback first!
#elif CLIENT_HBW
		    	ft::TxtHighBayWarehouse hbw(&T, pcli);
		    	hbw.sound.enable(sound_enable);
		    	pHBW = &hbw;
				callback cb(mqttclient, hbw);
				mqttclient.set_callback(cb); //set callback first!
#elif CLIENT_VGR
		    	ft::TxtVacuumGripperRobot vgr(&T, pcli);
		    	vgr.sound.enable(sound_enable);
		    	pVGR = &vgr;
				callback cb(mqttclient, vgr);
				mqttclient.set_callback(cb); //set callback first!
#elif CLIENT_SLD
		    	ft::TxtSortingLine sld(&T, pcli);
		    	sld.sound.enable(sound_enable);
		    	pSLD = &sld;
				callback cb(mqttclient, sld);
				mqttclient.set_callback(cb); //set callback first!
#else
				#error Set CLIENT_XXX define first!
#endif

				std::cout << "Connect MQTTClient" << std::endl;
				assert(pcli);
		    	bool ret = pcli->connect(TIMEOUT_CONNECTION_MS);
		    	if (!ret) {
			        std::cerr << "Error: timeout connecting to MQTT broker: " << TIMEOUT_CONNECTION_MS << "s" << std::endl;
					spdlog::get("file_logger")->error("Error: timeout connecting to MQTT broker: {}s", TIMEOUT_CONNECTION_MS);
			        return 1;
		    	}

				std::cout << "Waiting is_connected ... ";
				while(!pcli->is_connected());
				std::cout << "OK" << std::endl;

				std::cout << "Waiting first_message_subscribe ... ";
				while(!first_message_subscribe);
				std::cout << "OK" << std::endl;

				std::cout << "Broadcast MQTTClient" << std::endl;
				//Broadcast: to trigger config messages
				long timeout_ms = TIMEOUT_CONNECTION_MS;
				double timestamp_s = ft::getnowtimestamp_s();
#ifdef CLIENT_MPO
				pcli->publishStationBroadcast("MPO",
#elif CLIENT_HBW
				pcli->publishStationBroadcast("HBW",
#elif CLIENT_VGR
				pcli->publishStationBroadcast("VGR",
#elif CLIENT_SLD
				pcli->publishStationBroadcast("SLD",
#else
				#error Set CLIENT_XXX define first!
#endif
						timestamp_s, clientName, TxtAppVer, "init", timeout_ms);

				//std::cout << "Waiting first_message_arrived ... ";
				//while(!first_message_arrived);
				//std::cout << "OK" << std::endl;

				std::cout << "Starting factory station thread ... ";
#ifdef CLIENT_MPO
		    	mpo.startThread();
#elif CLIENT_HBW
		    	hbw.startThread();
#elif CLIENT_VGR
		    	vgr.startThread();
#elif CLIENT_SLD
		    	sld.startThread();
#else
				#error Set CLIENT_XXX define first!
#endif
				std::cout << "OK" << std::endl;

		    	while(true) {
		    		//idle
					std::this_thread::sleep_for(std::chrono::milliseconds(10000));
					//retain flag: do not use because of performance issue!
#ifdef CLIENT_MPO
					mpo.Notify();
#elif CLIENT_HBW
					hbw.Notify();
					hbw.publishStorage();
#elif CLIENT_VGR
					vgr.Notify();
#elif CLIENT_SLD
					sld.Notify();
#else
					#error Set CLIENT_XXX define first!
#endif
				}

			} catch (const std::invalid_argument& exc) {
				std::cerr << "invalid_argument Error: " << exc.what() << std::endl;
				spdlog::get("file_logger")->error("invalid_argument Error: {}", exc.what());
				ft::TxtSound::play(pTArea,1);
				return 1;
			} catch (const Json::RuntimeError& exc) {
				std::cerr << "Json Error: " << exc.what() << std::endl;
				spdlog::get("file_logger")->error("Json Error: {}", exc.what());
				ft::TxtSound::play(pTArea,1);
		        return 1;
		    } catch (const mqtt::exception& exc) {
		        std::cerr << "MQTT Error: " << exc.what() << " [" << exc.get_reason_code() << "]" << std::endl;
				spdlog::get("file_logger")->error("MQTT Error: {} [{}]", exc.what(), exc.get_reason_code());
				ft::TxtSound::play(pTArea,2);
		        return 1;
			} catch (const spdlog::spdlog_ex& exc) {
			    std::cerr << "Spdlog Error: " << exc.what() << std::endl;
				spdlog::get("file_logger")->error("Spdlog Error: {}", exc.what());
				ft::TxtSound::play(pTArea,1);
		        return 1;
			}
        }
        StopTxtDownloadProg();
    }
	return 0;
}
