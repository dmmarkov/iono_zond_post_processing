/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   GnssLogTime.cpp
 * Author: markov
 */
#include <boost/date_time/posix_time/posix_time.hpp>

#include "gnss_functions.hpp"

#include "GnssLogTime.hpp"
#include "common_functions.hpp"

GnssLogTime::GnssLogTime() {
    week_ = 0;
    milliseconds_ = 0;
}

GnssLogTime::GnssLogTime(unsigned short week, unsigned int milliseconds) {
    week_ = week;
    milliseconds_ = milliseconds;

    utcTime_ = getLogTime(week_, milliseconds_);
}

GnssLogTime::GnssLogTime(const GnssLogTime& gnssLogTime) : stringUtcTime_(gnssLogTime.stringUtcTime_),
                                                           stringIsoTime_(gnssLogTime.stringIsoTime_)
{
    week_ = gnssLogTime.week_;
    milliseconds_ = gnssLogTime.milliseconds_;

    utcTime_ = gnssLogTime.utcTime_;
}

GnssLogTime::~GnssLogTime() {

}

GnssLogTime& GnssLogTime::operator=(const GnssLogTime& b) {
    week_ = b.week_;
    milliseconds_ = b.milliseconds_;
    utcTime_ = b.utcTime_;
    stringUtcTime_ = b.stringUtcTime_;
    stringIsoTime_ = b.stringIsoTime_;

    return *this;
}

bool GnssLogTime::operator==(const GnssLogTime& b) const {
    return ((week_ == b.week_) && (milliseconds_ == b.milliseconds_));
}

bool GnssLogTime::operator!=(const GnssLogTime& b) const {
    return ((week_ != b.week_) || (milliseconds_ != b.milliseconds_));
}

bool GnssLogTime::operator<(const GnssLogTime& b) const {
    return ((week_ < b.week_) || (milliseconds_ < b.milliseconds_));
}

bool GnssLogTime::operator>(const GnssLogTime& b) const {
    return ((week_ >= b.week_) && (milliseconds_ > b.milliseconds_));
}

bool GnssLogTime::operator<=(const GnssLogTime& b) const {
    return ((*this < b) || (*this == b));
}

bool GnssLogTime::operator>=(const GnssLogTime& b) const {
    return ((*this > b) || (*this == b));
}

PTime
GnssLogTime::getUtcTime() {
    //return UTC time in ptime variable
    return utcTime_;
}

std::string
GnssLogTime::getStringUtcTime() {
    //return UTC time in string format
    if (stringUtcTime_.empty()) {
        stringUtcTime_ = pTimeToSimpleString(utcTime_);
    }

    return stringUtcTime_;
}

std::string GnssLogTime::getStringIsoTime() {
    //return UTS time in ISO extended string
    if (stringIsoTime_.empty()) {
        stringIsoTime_ = pTimeToIsoString(utcTime_);
    }

    return stringIsoTime_;
}

unsigned short
GnssLogTime::getWeek() {
    return week_;
}

unsigned int
GnssLogTime::getMilliseconds() {
    return milliseconds_;
}

