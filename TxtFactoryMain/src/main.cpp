#include "KeLibTxtDl.h"     // TXT Lib
#include "FtShmem.h"        // TXT Transfer Area

#include "Utils.h"
#include "Observer.h"

#include "TxtAlert.h"
#include "TxtBME680.h"
#include "TxtCamera.h"
#include "TxtMotionDetection.h"
#include "TxtPanTiltUnit.h"
#include "TxtFactoryTypes.h"
#include "TxtJoystickXYBController.h"
#include "TxtMqttFactoryClient.h"
#include "TxtSound.h"

#include <stdio.h>          // for printf()
#include <string.h>         // for memset()
#include <unistd.h>         // for sleep()
#include <cmath>			// for pow()

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/async.h"

bool first_message_arrived = false;
bool first_message_subscribe = false;

int vgr_lastCode = -1;
int hbw_lastCode = -1;
int mpo_lastCode = -1;
int sld_lastCode = -1;
int dsi_lastCode = -1;
int dso_lastCode = -1;

std::string sts_mpo;
std::string sts_hbw;
std::string sts_vgr;
std::string sts_sld;

// Version info
#define VERSION_HEX ((0<<16)|(8<<8)|(1<<0))
char TxtAppVer[32];

unsigned int DebugFlags;
FILE *DebugFile;

FISH_X1_TRANSFER* pTArea = NULL;

ft::TxtJoystickXYBController* pJoy = NULL;

ft::TxtPanTiltUnitController* pPtuControl = NULL;
float panpos_last = -100.f; //invalid
float tiltpos_last = -100.f;
float panpos_published_last = -200.f;
float tiltpos_published_last = -200.f;

ft::TxtMqttFactoryClient* pcli = NULL;
ft::TxtBME680* pBme680 = NULL; // extern in TxtBME680.cpp
ft::TxtCamera* pCam = NULL;

//uint16_t u16CountState1000ms = 0;
int16_t ldr_last = 0;
int mleds = 0;

double delta = 0.1;

double force_max_rate = -1.0;
double period_ldr = 60.0; //Default: 1 Min
double period_bme680 = 60.0; //Default: 1 Min
int64_t timestamp_bme680 = 0; //ns
double timestamp_ldr = 0.; //s

std::chrono::system_clock::time_point tsLastDetectedTemp;
std::chrono::system_clock::time_point tsLastDetectedHum;
#ifdef DEBUG
std::chrono::system_clock::time_point startLast;
#endif

#define TIMEOUT_CONNECTION_MS 60000 //60 s
#define DEGREE2STEPS 10.f

void setLED(int o, int v) {
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "setLED o:{} v:{}", o, v);
	if (pTArea) {
		pTArea->ftX1out.duty[o] = v;
	}
}


class TxtJoystickButtonsObserver : public ft::Observer {
public:
	TxtJoystickButtonsObserver(ft::TxtJoystickXYBController* s)
	{
		SPDLOG_LOGGER_TRACE(spdlog::get("console"), "TxtJoystickButtonsObserver",0);
		_subject = s;
		_subject->Attach(this);
	}
	virtual ~TxtJoystickButtonsObserver() {
		SPDLOG_LOGGER_TRACE(spdlog::get("console"), "~TxtJoystickButtonsObserver",0);
		_subject->Detach(this);
	}
	void Update(ft::SubjectObserver* theChangedSubject) {
		if(theChangedSubject == _subject) {
			//SPDLOG_LOGGER_TRACE(spdlog::get("console"), "Update 1",0);
			ft::TxtJoysticksData jd = _subject->getData();
			long timeout_ms = TIMEOUT_CONNECTION_MS;
			assert(pcli);
			pcli->publishSSC_Joy(jd, timeout_ms);
			//SPDLOG_LOGGER_TRACE(spdlog::get("console"), "Update 2",0);
		}
	}
private:
	ft::TxtJoystickXYBController *_subject;
};


class TxtPTUPosObserver : public ft::Observer {
public:
	TxtPTUPosObserver(ft::TxtPanTiltUnit* s)
	{
		SPDLOG_LOGGER_TRACE(spdlog::get("console"), "TxtPTUPosObserver",0);
		_subject = s;
		_subject->Attach(this);
	}
	virtual ~TxtPTUPosObserver() {
		SPDLOG_LOGGER_TRACE(spdlog::get("console"), "~TxtPTUPosObserver",0);
		_subject->Detach(this);
	}
	void Update(ft::SubjectObserver* theChangedSubject) {
		if(theChangedSubject == _subject) {
			//SPDLOG_LOGGER_TRACE(spdlog::get("console"), "Update 1",0);
			panpos_last = (float)_subject->getPosPanRel();
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "panpos_last = {}", panpos_last);
			tiltpos_last = (float)_subject->getPosTiltRel();
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "tiltpos_last = {}", tiltpos_last);
			//SPDLOG_LOGGER_TRACE(spdlog::get("console"), "Update 2",0);
		}
	}
private:
	ft::TxtPanTiltUnit *_subject;
};


class TxtCameraObserver : public ft::Observer {
public:
	TxtCameraObserver(ft::TxtCamera* s)
	{
		SPDLOG_LOGGER_TRACE(spdlog::get("console"), "TxtCameraObserver",0);
		_subject = s;
		_subject->Attach(this);
	}
	virtual ~TxtCameraObserver() {
		SPDLOG_LOGGER_TRACE(spdlog::get("console"), "~TxtCameraObserver",0);
		_subject->Detach(this);
	}
	void Update(ft::SubjectObserver* theChangedSubject) {
		if(theChangedSubject == _subject) {
			//SPDLOG_LOGGER_TRACE(spdlog::get("console"), "TxtCameraObserver Update 1",0);
#ifdef DEBUG
			auto start = std::chrono::system_clock::now();
#endif

			std::string sdata = _subject->getDataString();
			if (!sdata.empty()) {
#ifdef CAM_TEST
					spdlog::get("console")->info("CAM 3: --- publish");
#endif
				assert(pcli);
				long timeout_ms = TIMEOUT_CONNECTION_MS;//TODO _subject->getPeriod(); //67 max 15fps
				if (mleds==1) {
					setLED(4, 512);
				}
				pcli->publishCam(sdata, timeout_ms);
			}

#ifdef DEBUG
			auto dur = start-startLast;
			auto secs = std::chrono::duration_cast< std::chrono::duration<float> >(dur);
			double period_ms = _subject->getPeriod();
			double wait_ms = period_ms - secs.count()/1000.0;
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "fps:{} diff_s:{} period_ms:{} wait_ms:{}",
					1./secs.count(), secs.count(), period_ms, wait_ms);
			startLast = start;
#endif
			if (mleds==1) {
				setLED(4, 0);
			}
			//SPDLOG_LOGGER_TRACE(spdlog::get("console"), "TxtCameraObserver Update 2",0);
		}
	}
private:
	ft::TxtCamera *_subject;
};

class TxtMotionDetectionObserver : public ft::Observer {
public:
	TxtMotionDetectionObserver(ft::TxtMotionDetection* s)
	{
		SPDLOG_LOGGER_TRACE(spdlog::get("console"), "TxtMotionDetectionObserver",0);
		_subject = s;
		_subject->Attach(this);
	}
	virtual ~TxtMotionDetectionObserver() {
		SPDLOG_LOGGER_TRACE(spdlog::get("console"), "~TxtMotionDetectionObserver",0);
		_subject->Detach(this);
	}
	void Update(ft::SubjectObserver* theChangedSubject) {
		if(theChangedSubject == _subject) {
			//SPDLOG_LOGGER_TRACE(spdlog::get("console"), "TxtMotionDetectionObserver Update 1",0);
			std::string sdata = _subject->getDataString();
			if (!sdata.empty()) {
				if (pPtuControl && !pPtuControl->isBusy()) { //Alert if PTU offline
					assert(pcli);
					long timeout_ms = TIMEOUT_CONNECTION_MS;//TODO 1000
					pcli->publishAlert(true, "cam", sdata, 100, timeout_ms);
					std::cout << "Alert: cam" << std::endl;
				}
			}
			//SPDLOG_LOGGER_TRACE(spdlog::get("console"), "TxtMotionDetectionObserver Update 2",0);
		}
	}
private:
	ft::TxtMotionDetection *_subject;
};


class TxtBme680Observer : public ft::Observer {
public:
	TxtBme680Observer(ft::TxtBME680* s)
	{
		SPDLOG_LOGGER_TRACE(spdlog::get("console"), "TxtBme680Observer",0);
		_subject = s;
		_subject->Attach(this);
	}
	virtual ~TxtBme680Observer() {
		SPDLOG_LOGGER_TRACE(spdlog::get("console"), "~TxtBme680Observer",0);
		_subject->Detach(this);
	}
	void Update(ft::SubjectObserver* theChangedSubject) {
		if(theChangedSubject == _subject) {
			//SPDLOG_LOGGER_TRACE(spdlog::get("console"), "TxtBme680Observer Update 1",0);
			//BME680
			double diff = ((_subject->_timestamp - timestamp_bme680) / 1000000000.) + delta;
			spdlog::get("console")->info("BME680 diff:{}", diff);
			if ((timestamp_bme680 == 0) || (diff >= period_bme680)) {
				SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "timestamp_bme680:{} period_bme680:{}", timestamp_bme680/1000000000., period_bme680);
				timestamp_bme680 = _subject->_timestamp;
				long timeout_ms = TIMEOUT_CONNECTION_MS;//TODO1000L*period_bme680;
				assert(pcli);
				pcli->publishBme680(
					_subject->_timestamp,
					_subject->_iaq,
					_subject->_iaq_accuracy,
					_subject->_temperature,
					_subject->_humidity,
					_subject->_pressure,
					_subject->_raw_temperature,
					_subject->_raw_humidity,
					_subject->_gas,
					timeout_ms);
			}
			//SPDLOG_LOGGER_TRACE(spdlog::get("console"), "TxtBme680Observer Update 2",0);
		}
	}
private:
	ft::TxtBME680 *_subject;
};

class TxtAlertBme680Observer : public ft::Observer {
public:
	TxtAlertBme680Observer(ft::TxtBME680* s)
	{
		SPDLOG_LOGGER_TRACE(spdlog::get("console"), "TxtBme680Observer",0);
		_subject = s;
		_subject->Attach(this);
	}
	virtual ~TxtAlertBme680Observer() {
		SPDLOG_LOGGER_TRACE(spdlog::get("console"), "~TxtBme680Observer",0);
		_subject->Detach(this);
	}
	void Update(ft::SubjectObserver* theChangedSubject) {
		if(theChangedSubject == _subject) {
			SPDLOG_LOGGER_TRACE(spdlog::get("console"), "TxtAlertBme680Observer Update",0);
			//_subject->_timestamp
			long int timeout_ms = TIMEOUT_CONNECTION_MS;//TODO 1000;
			if (_subject->_temperature < 4.0) {
				auto tsDetected = std::chrono::system_clock::now();
				auto dur = tsDetected-tsLastDetectedTemp;
				auto secs = std::chrono::duration_cast< std::chrono::duration<float> >(dur);
				SPDLOG_LOGGER_TRACE(spdlog::get("console"), "elapsed_seconds {}", secs.count());
				if (secs.count() > TIMEWAIT_S_MAX) {
					tsLastDetectedTemp = tsDetected;
					assert(pcli);
					pcli->publishAlert(false, "bme680/t", ft::ftos(_subject->_temperature,1), 200, timeout_ms);
					std::cout << "Alert: bme680/t" << std::endl;
				}
			}
			if (_subject->_humidity > 80.) {
				auto tsDetected = std::chrono::system_clock::now();
				auto dur = tsDetected-tsLastDetectedHum;
				auto secs = std::chrono::duration_cast< std::chrono::duration<float> >(dur);
				SPDLOG_LOGGER_TRACE(spdlog::get("console"), "elapsed_seconds {}", secs.count());
				if (secs.count() > TIMEWAIT_S_MAX) {
					tsLastDetectedHum = tsDetected;
					assert(pcli);
					pcli->publishAlert(false, "bme680/h", ft::ftos(_subject->_humidity,1), 300, timeout_ms);
					std::cout << "Alert: bme680/h" << std::endl;
				}
			}
			SPDLOG_LOGGER_TRACE(spdlog::get("console"), "TxtAlertBme680Observer Update",0);
		}
	}
private:
	ft::TxtBME680 *_subject;
};

void setLEDs(ft::TxtLEDSCode_t code)
{
	setLED(7, ((int)code & 0x1) ? 512 : 0);    // green
	setLED(6, ((int)code & 0x2)>>1 ? 512 : 0); // yellow
	setLED(5, ((int)code & 0x4)>>2 ? 512 : 0); // red
}

class callback : public virtual mqtt::callback
{
	ft::TxtMqttFactoryClient& cli_;

	void connected(const std::string& cause) override {
		SPDLOG_LOGGER_TRACE(spdlog::get("console"), "connected: {}", cause);
		long timeout_ms = TIMEOUT_CONNECTION_MS;
		std::cout << "Subscribe MQTTClient" << std::endl;
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

		if (msg->get_topic() == TOPIC_CONFIG_LINK) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED link:{}", msg->get_topic());
			std::stringstream ssin(msg->to_string());
			Json::Value root;
			try {
				ssin >> root;
				std::string smessage = root["message"].asString();
				SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  message:{}", smessage);
				int code = -1;
				if (root.isMember("code")) {
					code = root["code"].asInt();
				}
				SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  code:{}", code);
			} catch (const Json::RuntimeError& exc) {
				std::cout << "Error: " << exc.what() << std::endl;
			}
		} else if (msg->get_topic() == TOPIC_LOCAL_BROADCAST) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED local broadcast:{}", msg->get_topic());
			std::stringstream ssin(msg->to_string());
			Json::Value root;
			try {
				ssin >> root;
				std::string sts = root["ts"].asString();
				std::string station = root["station"].asString();
				std::string hardwareId = root["hardwareId"].asString();
				std::string softwareName = root["softwareName"].asString();
				std::string softwareVersion = root["softwareVersion"].asString();
				std::string message = root["message"].asString();
				SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  station:{} ts:{}", station, sts);
				if (station=="MPO") {
					sts_mpo = sts;
					std::cout << "ts_mpo: " << sts_mpo << std::endl;
				} else if (station=="HBW") {
					sts_hbw = sts;
					std::cout << "ts_hbw: " << sts_hbw << std::endl;
				} else if (station=="VGR") {
					sts_vgr = sts;
					std::cout << "ts_vgr: " << sts_vgr << std::endl;
				} else if (station=="SLD") {
					sts_sld = sts;
					std::cout << "ts_sld: " << sts_sld << std::endl;
				} else {
					std::cout << "Unknown station: " << station << std::endl;
					spdlog::get("file_logger")->error("Unknown station: {}",station);
					exit(1);
				}
				//check time sync
				if (!ft::trycheckTimestampTTL(sts))
				{
					std::cout << "Please sync time!" << station << ", " << sts << std::endl;
					spdlog::get("file_logger")->error("Please sync time! {} ({})",station,sts);
					ft::TxtSound::play(pTArea,2);
					exit(1);
				}
				//check SW version
				if (TxtAppVer != softwareVersion)
				{
					std::cout << "Wrong SW Version!" << station << " " << TxtAppVer << "!=" << softwareVersion << std::endl;
					spdlog::get("file_logger")->error("Wrong SW Version! {} {}!={}",station,TxtAppVer,softwareVersion);
					ft::TxtSound::play(pTArea,3);
					exit(1);
				}
			} catch (const Json::RuntimeError& exc) {
				std::cout << "Error: " << exc.what() << std::endl;
			}
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "OK.", 0);
		} else if (msg->get_topic() == TOPIC_CONFIG_BME680) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED bme680 config: {}", msg->get_topic());
			if (force_max_rate) {
				std::cout << "force_max_rate=true: ignoring bme680 config" << std::endl;
			} else {
				std::stringstream ssin(msg->to_string());
				Json::Value root;
				try {
					ssin >> root;
					double period = -1.0;
					if (root.isMember("period")) {
						period = root["period"].asDouble();
						SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  period: {}", period);
					}
					if (period >= 3.0) {
						period_bme680 = period;
						SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "Setting period_bme680={}s",period_bme680);
					} else if (period >= 1.0) {
						period_bme680 = 3.0;
						spdlog::get("console")->warn("WRONG CONFIG: period should be >= 3.0. Setting period_bme680=3.0s",0);
					}
				} catch (const Json::RuntimeError& exc) {
					std::cout << "Error: " << exc.what() << std::endl;
				}
			}
		} else if (msg->get_topic() == TOPIC_CONFIG_LDR) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED ldr config: {}", msg->get_topic());
			if (force_max_rate) {
				std::cout << "force_max_rate=true: ignoring ldr config" << std::endl;
			} else {
				std::stringstream ssin(msg->to_string());
				Json::Value root;
				try {
					ssin >> root;
					double period = -1.0;
					if (root.isMember("period")) {
						period = root["period"].asDouble();
						SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  period: {}", period);
					}
					if (period >= 1.0) {
						period_ldr = period;
						SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "Setting period_ldr={}s",period_ldr);
					} else {
						spdlog::get("console")->warn("WRONG CONFIG: period >= 1.0. Setting period_ldr=1.0s",0);
					}
				} catch (const Json::RuntimeError& exc) {
					std::cout << "Error: " << exc.what() << std::endl;
				}
			}
		} else if (msg->get_topic() == TOPIC_CONFIG_CAM) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED cam config: {}", msg->get_topic());
			if (force_max_rate) {
				std::cout << "force_max_rate=true: ignoring cam config" << std::endl;
			} else {
				std::stringstream ssin(msg->to_string());
				Json::Value root;
				try {
					ssin >> root;
					bool bon = root["on"].asBool();
					SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  on: {}", bon);
					double fps = -1.0;
					if (root.isMember("fps")) {
						fps = root["fps"].asDouble();
						SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  fps: {}", fps);
					}
					if (fps > 0.0) {
						//assert(pCam);
						if (pCam) pCam->setFps(fps);
						SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "CONFIG: fps:{}",fps);
					}
					if (bon) {
						//assert(pCam);
						if (pCam) pCam->start();
						SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "CONFIG: start camera",0);
					} else {
						//assert(pCam);
						if (pCam) pCam->stop();
						SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "CONFIG: stop camera",0);
					}
				} catch (const Json::RuntimeError& exc) {
					std::cout << "Error: " << exc.what() << std::endl;
				}
			}
		} else if (msg->get_topic() == TOPIC_OUTPUT_PTU) {
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED PTU control: {}", msg->get_topic());
			if (pPtuControl) {
				std::stringstream ssin(msg->to_string());
				Json::Value root;
				try {
					ssin >> root;
					std::string scmd = root["cmd"].asString();
					SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  cmd:{}", scmd);
					float degree = 0.f;
					if (root.isMember("degree")) {
						degree = root["degree"].asFloat();
						SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  degree:{}", degree);
					}
					assert(pPtuControl);
					int steps = DEGREE2STEPS*degree;
					if (pPtuControl->executeCmd(scmd, steps)) {
						SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "executeCmd (TRUE): scmd:{} degree:{} steps:{}", scmd, degree, steps);
					} else {
						SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "executeCmd (FALSE)",0);
					}
				} catch (const Json::RuntimeError& exc) {
					std::cout << "Error: " << exc.what() << std::endl;
				}
			} else {
				std::cout << "PTU not available!" << std::endl;
			}
		}
		else if (msg->get_topic() == TOPIC_INPUT_STATE_HBW)
		{
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED input state HBW: {}", msg->get_topic());
			updateLEDs(msg, "hbw");
		}
		else if (msg->get_topic() == TOPIC_INPUT_STATE_VGR)
		{
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED input state VGR: {}", msg->get_topic());
			updateLEDs(msg, "vgr");
		}
		else if (msg->get_topic() == TOPIC_INPUT_STATE_MPO)
		{
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED input state MPO: {}", msg->get_topic());
			updateLEDs(msg, "mpo");
		}
		else if (msg->get_topic() == TOPIC_INPUT_STATE_SLD)
		{
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED input state SLD: {}", msg->get_topic());
			updateLEDs(msg, "sld");
		}
		else if (msg->get_topic() == TOPIC_INPUT_STATE_DSI)
		{
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED input state DSI: {}", msg->get_topic());
			updateLEDs(msg, "dsi");
		}
		else if (msg->get_topic() == TOPIC_INPUT_STATE_DSO)
		{
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "DETECTED input state DSO: {}", msg->get_topic());
			updateLEDs(msg, "dso");
		} else {
			std::cout << "Unknown topic: " << msg->get_topic() << std::endl;
			spdlog::get("file_logger")->error("Unknown topic: {}",msg->get_topic());
			exit(1);
		}
	}

	void updateLEDs(mqtt::const_message_ptr msg, const std::string station)
	{
		SPDLOG_LOGGER_TRACE(spdlog::get("console"), "updateLEDs",0);
		std::stringstream ssin(msg->to_string());
		Json::Value root;
		try {
			ssin >> root;
			int code = root["code"].asInt();
			SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "  code: {}", code);

			//update lastCode
			if (station=="hbw") {
				hbw_lastCode=code;
			} else if (station=="vgr") {
				vgr_lastCode=code;
			} else if (station=="mpo") {
				mpo_lastCode=code;
			} else if (station=="sld") {
				sld_lastCode=code;
			} else if (station=="dsi") {
				dsi_lastCode=code;
			} else if (station=="dso") {
				dso_lastCode=code;
			}


			//aggregate state LEDs
			if ((hbw_lastCode==ft::LEDS_ERROR)||
				(vgr_lastCode==ft::LEDS_ERROR)||
				(mpo_lastCode==ft::LEDS_ERROR)||
				(sld_lastCode==ft::LEDS_ERROR))
			{
				setLEDs(ft::LEDS_ERROR);
			} else if ((hbw_lastCode==ft::LEDS_CALIB)||
				(vgr_lastCode==ft::LEDS_CALIB)||
				(mpo_lastCode==ft::LEDS_CALIB)||
				(sld_lastCode==ft::LEDS_CALIB))
			{
				setLEDs(ft::LEDS_CALIB);
			} else if ((hbw_lastCode==ft::LEDS_WAIT_ERROR)||
				(vgr_lastCode==ft::LEDS_WAIT_ERROR)||
				(mpo_lastCode==ft::LEDS_WAIT_ERROR)||
				(sld_lastCode==ft::LEDS_WAIT_ERROR))
			{
				setLEDs(ft::LEDS_WAIT_ERROR);
			} else if ((hbw_lastCode==ft::LEDS_WAIT_READY)||
				(vgr_lastCode==ft::LEDS_WAIT_READY)||
				(mpo_lastCode==ft::LEDS_WAIT_READY)||
				(sld_lastCode==ft::LEDS_WAIT_READY))
			{
				setLEDs(ft::LEDS_WAIT_READY);
			} else if ((hbw_lastCode==ft::LEDS_BUSY)||
				(vgr_lastCode==ft::LEDS_BUSY)||
				(mpo_lastCode==ft::LEDS_BUSY)||
				(sld_lastCode==ft::LEDS_BUSY))
			{
				setLEDs(ft::LEDS_BUSY);
			} else if ((hbw_lastCode==ft::LEDS_READY)||
				(vgr_lastCode==ft::LEDS_READY)||
				(mpo_lastCode==ft::LEDS_READY)||
				(sld_lastCode==ft::LEDS_READY))
			{
				setLEDs(ft::LEDS_READY);
			} else {
				setLEDs(ft::LEDS_OFF);
			}

		} catch (const Json::RuntimeError& exc) {
			std::cout << "Error: " << exc.what() << std::endl;
		}
	}

	void delivery_complete(mqtt::delivery_token_ptr token) override {
		if (token) {
			SPDLOG_LOGGER_TRACE(spdlog::get("console"), "delivery_complete: {}: {}", token->get_message_id(), token->get_message()->get_topic());
		} else {
			SPDLOG_LOGGER_TRACE(spdlog::get("console"), "delivery token is NULL",0);
		}
	}

public:
	callback(ft::TxtMqttFactoryClient& cli) : cli_(cli) {}
};


int main(int argc, char* argv[])
{
	sprintf(TxtAppVer, "%d.%d.%d", (VERSION_HEX >> 16) & 0xff,
	            (VERSION_HEX >> 8) & 0xff, (VERSION_HEX >> 0) & 0xff);
	fprintf( stdout, "TxtFactoryMain V%d.%d.%d\n", (VERSION_HEX >> 16) & 0xff,
	            (VERSION_HEX >> 8) & 0xff, (VERSION_HEX >> 0) & 0xff);
	try
	{
		//can be set globaly or per logger(logger->set_error_handler(..))
		spdlog::set_error_handler([](const std::string& msg)
	    {
			std::cout << "err handler spdlog:" << msg << std::endl;
			spdlog::get("file_logger")->error("err handler spdlog: {}",msg);
			exit(1);
	    });

		auto file_logger = spdlog::basic_logger_mt<spdlog::async_factory>("file_logger", "Data/TxtFactoryMain.log", true);
		spdlog::get("file_logger")->set_level(spdlog::level::trace);
		spdlog::get("file_logger")->info("TxtFactoryMain {}", TxtAppVer);

		// Console logger with color
		auto console = spdlog::stdout_color_mt("console");
		auto console_axes = spdlog::stdout_color_mt("console_axes");
		//spdlog::set_formatter();
		spdlog::set_pattern("[%t][%Y-%m-%d %T.%e][%L] %v");
		spdlog::set_level(spdlog::level::trace);
		console_axes->set_level(spdlog::level::off);//trace);
	}
	catch (const spdlog::spdlog_ex& ex)
	{
		std::cout << "Log initialization failed: " << ex.what() << std::endl;
	}

	//load config
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;
    std::ifstream test("/opt/knobloch/.TxtFactoryMain.json", std::ifstream::binary);
    if (test.is_open()) {
        std::cout << "load file /opt/knobloch/.TxtFactoryMain.json" << std::endl;
        bool ok = Json::parseFromStream(builder, test, &root, &errs);
        if ( !ok )
        {
            std::cout  << errs << "\n";
        }
    }
    bool sound_enable = root.get("sound", true ).asBool();
    std::string host = root.get("host", "localhost" ).asString();
    int port = root.get("port", 1883 ).asInt();
    std::string mqtt_user = root.get("mqtt_user", "txt" ).asString();
    mqtt::binary_ref mqtt_pass = root.get("mqtt_pass", "xtx" ).asString();
    int w = root.get("cam_w", 320. ).asDouble();
    int h = root.get("cam_h", 240. ).asDouble();
    force_max_rate = root.get("force_max_rate", false).asBool();
    double max_limit_Area_moveDetect = root.get("max_limit_Area_moveDetect", 10000.0).asDouble();
    double broadcast_retry_delay = root.get("broadcast_retry_delay", 5.0).asDouble();
    int mcontrol = root.get("controlmode", 10).asInt();
    mleds = root.get("ledsmode", 1).asInt();
    std::cout << "sound:" << sound_enable
    	<< " host:" << host
		<< " port:" << port
		<< " mqtt_user:" << mqtt_user
		<< " mqtt_pass:" << mqtt_pass << std::endl
		<< " cam w,h:" << w << "," << h << std::endl
		<< " force_max_rate:" << force_max_rate
		<< " control mode:" << mcontrol
		<< " leds mode:" << mleds
		<< " max_limit_Area_moveDetect:" << max_limit_Area_moveDetect
		<< " broadcast_retry_delay:" << broadcast_retry_delay
		<< std::endl;

    if (StartTxtDownloadProg() == KELIB_ERROR_NONE)
    {
        pTArea = GetKeLibTransferAreaMainAddress();

        if (pTArea)
        {
            //SetTransferAreaCompleteCallback(JoysticksTransferAreaCallbackFunction);
		    try {
				std::cout << "Init MQTTClient" << std::endl;
				std::stringstream sout_port;
				sout_port << port;
				//TODO
				ft::TxtMqttFactoryClient mqttclient("TxtFactoryMain", host, sout_port.str(), mqtt_user, mqtt_pass);
				pcli = &mqttclient;
				callback cb(mqttclient);
				mqttclient.set_callback(cb);

				std::cout << "Init TxtPTU" << std::endl;
				ft::TxtTransfer T(pTArea);
				ft::TxtPanTiltUnit ptu(&T);
				ft::TxtPanTiltUnitController ptucontrol(&ptu, mcontrol);
				pPtuControl = &ptucontrol;

				//PTU
				if (mcontrol > 0) {
					ptu.moveHome();
					if (ptu.getStatus() != ft::PTU_TIMEOUT_MOVEHOME) {
						if (ptu.getStatus() == ft::PTU_READY) {
							//ptu.moveCenter();
							ptu.moveHBW();
						}
					}
				}

				std::cout << "Init TxtBME680" << std::endl;
				ft::TxtBME680 bme680;
				pBme680 = &bme680;

				std::cout << "Init TxtCamera" << std::endl;
				ft::TxtCamera cam(w, h);
				pCam = &cam;
				std::cout << "Init TxtMotionDetection" << std::endl;
				ft::TxtMotionDetection mdcam(&cam, max_limit_Area_moveDetect); //default 500
				std::cout << "Start TxtCamera Thread" << std::endl;
				bool rcam = cam.startThread();
				if (!rcam) {
					std::cerr << "Error: init TxtCamera" << std::endl;
				} else {
					if (force_max_rate) {
						std::cout << "cam force_max_rate" << std::endl;
						cam.setFps(15.0);
						cam.start();
					}

					std::cout << "Start TxtMotionDetection Thread" << std::endl;
					bool retcam = mdcam.startThread();
					if (!retcam) {
						std::cerr << "Error: init TxtMotionDetection" << std::endl;
						return retcam;
					}
				}

				std::cout << "Connect MQTTClient" << std::endl;
				assert(pcli);
		    	bool ret = pcli->connect(TIMEOUT_CONNECTION_MS);
		    	if (!ret) {
			        std::cerr << "Error: timeout connecting to MQTT broker: " << TIMEOUT_CONNECTION_MS << "s" << std::endl;
			        return 1;
		    	}

				std::cout << "Init BME680" << std::endl;
	        	int ret2 = bme680.init();
	        	if (ret2 != 0) {
	        		std::cerr << "Error: init bme680" << std::endl;
	        	}
				if (force_max_rate) {
					std::cout << "bme680 force_max_rate" << std::endl;
					period_bme680 = 3.0;
				}

	            //Control
				std::cout << "Init PTU Control" << std::endl;
	            ptucontrol.startThread();

				//Joystick
				std::cout << "Init TxtJoystickXYBController" << std::endl;
				ft::TxtJoystickXYBController joy(&T, 4, 5, 10, 6, 7, 11);
				pJoy = &joy;

#ifndef LOCAL
				std::cout << "Waiting first_message_arrived ... ";
				while(!first_message_arrived) {
					std::cout << "Broadcast MQTTClient" << std::endl;
					//Broadcast: to trigger config messages
					long timeout_ms = TIMEOUT_CONNECTION_MS;
					double timestamp_s = ft::getnowtimestamp_s();
					pcli->publishBroadcast(timestamp_s, "TxtFactoryMain", TxtAppVer, "init", timeout_ms);
					sleep(broadcast_retry_delay);
				}
				std::cout << "OK" << std::endl;

				std::cout << "Waiting is_connected ... ";
				while(!pcli->is_connected());
				std::cout << "OK" << std::endl;

				std::cout << "Waiting first_message_subscribe ... ";
				while(!first_message_subscribe);
				std::cout << "OK" << std::endl;

				setLEDs(ft::LEDS_WAIT_READY);
#else
				setLEDs(ft::LEDS_WAIT_ERROR);
#endif

				//start observers
				std::cout << "Init TxtPTUPosObserver" << std::endl;
				TxtPTUPosObserver ptu_obs(&ptu);
				std::cout << "Init TxtBme680Observer" << std::endl;
				TxtBme680Observer obs_bme680(&bme680);
				std::cout << "Init TxtCameraObserver" << std::endl;
				TxtCameraObserver obs_cam(&cam);
				std::cout << "Init TxtMotionDetectionObserver" << std::endl;
				TxtMotionDetectionObserver obs_cammd(&mdcam);
				std::cout << "Init TxtAlertBme680Observer" << std::endl;
				TxtAlertBme680Observer obs_alertBme680(&bme680);
				std::cout << "Init TxtJoystickButtonsObserver" << std::endl;
				TxtJoystickButtonsObserver obs_joy(&joy);

				pJoy->startThread();

				if (force_max_rate) {
					std::cout << "ldr force_max_rate" << std::endl;
					period_ldr = 1.0;
				}

				bool firstvalue = true;

				//LDR setup
				pTArea->ftX1config.uni[2].mode = MODE_R;
				pTArea->ftX1config.uni[2].digital = 0;
				pTArea->ftX1state.config_id ++; // Save the new Setup

				while(true) {
					if (pTArea) {

						//LDR
						double timestamp_s = ft::getnowtimestamp_s();
						double diff = timestamp_s - timestamp_ldr;
						//SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "LDR diff:{}s", diff);
						if ((timestamp_ldr == 0) || (diff >= period_ldr)) {
							SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "timestamp_ldr:{} period_ldr:{}", timestamp_ldr, period_ldr);
							timestamp_ldr = timestamp_s;

							//LDR I3
							int16_t ldr = pTArea->ftX1in.uni[2];
							firstvalue = false;
							SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "ldr: {}", ldr);
							if (!firstvalue) {//Den ersten Wert verwerfen, da immer ungültig.
								long timeout_ms = TIMEOUT_CONNECTION_MS;//TODO period_ldr*1000;
								mqttclient.publishLDR(timestamp_s, ldr, timeout_ms);
							}
						}

						//PTU Pos
						//SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "panpos_last:{} tiltpos_last:{}",
						//		panpos_last, tiltpos_last);
						if ((panpos_last!=panpos_published_last) ||
								(tiltpos_last!=tiltpos_published_last))
						{
							panpos_published_last = panpos_last;
							tiltpos_published_last = tiltpos_last;
							long timeout_ms = TIMEOUT_CONNECTION_MS;
							if (panpos_published_last>-10.f && tiltpos_published_last>-10.f) {
								if ((panpos_last>=-1.f && panpos_last<=1.f)&&
									(panpos_last>=-1.f && panpos_last<=1.f))
								{
									mqttclient.publishPtuPos(panpos_last, tiltpos_last, timeout_ms);
								} else {
									mqttclient.publishPtuPos(-100.f, -100.f, timeout_ms);
								}
							}
							SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "panpos_published_last:{} tiltpos_published_last:{}",
									panpos_published_last, tiltpos_published_last);
						}

						std::this_thread::sleep_for(std::chrono::milliseconds(50));
					}
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
			} catch (const cv::Exception& exc) {
				std::cerr << "OpenCV Error: " << exc.what() << std::endl;
				spdlog::get("file_logger")->error("OpenCV Error: {}", exc.what());
				ft::TxtSound::play(pTArea,1);
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
