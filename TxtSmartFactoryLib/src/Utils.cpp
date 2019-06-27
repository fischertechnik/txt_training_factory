/*
 * Utils.cpp
 *
 *  Created on: 22.01.2018
 *      Author: steiger-a
 */

#include "Utils.h"

#include <algorithm>

#ifndef NO_MQTT
#include "opencv2/opencv.hpp"
#endif


namespace ft {


void gettimestampstr(int64_t timestamp, char* sts) {
	//SPDLOG_LOGGER_TRACE(spdlog::get("console"), "gettimestampstr");
	time_t epch = timestamp/1000000000.;
	double temp_modf_integer_part;
	int ms = modf(timestamp/1000000000., &temp_modf_integer_part) * 1000.;

	struct tm * timeinfo;
	timeinfo = gmtime ( &epch ); //UTC
	sprintf(sts, "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ",
			timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
			timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, ms);
}

void gettimestr(time_t rawtime, int ms, char* sts) {
	//SPDLOG_LOGGER_TRACE(spdlog::get("console"), "gettimestr");
	struct tm * timeinfo;
	//time ( &rawtime );
	timeinfo = gmtime ( &rawtime ); //UTC
	sprintf(sts, "%04d-%02d-%02dT%02d:%02d:%02d.%03dZ",
			timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
			timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, ms);
}

void getnowstr(char* sts) {
	//SPDLOG_LOGGER_TRACE(spdlog::get("console"), "getnowstr");
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	struct timespec {
		time_t   tv_sec;        /* seconds */
		long     tv_nsec;       /* nanoseconds */
	};
	time_t epch = ts.tv_sec;
	gettimestr(epch, (int)(ts.tv_nsec/1000000), sts);
}

double getnowtimestamp_s() {
	//SPDLOG_LOGGER_TRACE(spdlog::get("console"), "getnowtimestamp");
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	struct timespec {
		time_t   tv_sec;        /* seconds */
		long     tv_nsec;       /* nanoseconds */
	};
	time_t epch = ts.tv_sec;
	double ms = ts.tv_nsec/1000000000.;
	//SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "epch:{} ms:{}", epch, ms);
	return (double)epch+ms;
}

std::chrono::system_clock::time_point trygettimepoint(const std::string& str)
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "gettimepoint {}",str);
	std::string _str = str;
	replace(_str.begin(), _str.end(), 'T', ' ');
	_str = _str.substr(0,_str.length()-1); // ...Z
	SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "replace {}",_str);
    std::istringstream iss{_str};
    std::tm tm{};
    if (!(iss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S")))
    {
        throw std::invalid_argument("get_time");
    }
    std::chrono::system_clock::time_point timePoint{std::chrono::seconds(std::mktime(&tm))};
    if (iss.eof())
    {
        return timePoint;
    }
    double zz;
    if (iss.peek() != '.' || !(iss >> zz))
    {
        throw std::invalid_argument("decimal");
    }
    using hr_clock = std::chrono::high_resolution_clock;
    std::size_t seconds = zz * hr_clock::period::den / hr_clock::period::num;
    return timePoint += hr_clock::duration(seconds);
}

#ifndef NO_MQTT
std::string ftos(float f, int nd) {
	//SPDLOG_LOGGER_TRACE(spdlog::get("console"), "");
    std::ostringstream ostr;
    int tens = std::stoi("1" + std::string(nd, '0'));
    ostr << round(f*tens)/tens;
    return ostr.str();
}
#endif


} /* namespace ft */
