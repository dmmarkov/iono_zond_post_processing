/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   GnssLogTime.hpp
 * Author: markov
 */

#ifndef GNSSLOGTIME_HPP
#define GNSSLOGTIME_HPP

#include <iostream>

#include <boost/thread/mutex.hpp>

#include "GnssCommonTypes.hpp"

class GnssLogTime {
public:
    GnssLogTime();
    GnssLogTime(unsigned short week, unsigned int milliseconds);
    GnssLogTime(const GnssLogTime& gnssLogTime);
    virtual ~GnssLogTime();
    bool operator==(const GnssLogTime& b) const;
    bool operator!=(const GnssLogTime& b) const;
    bool operator<(const GnssLogTime& b) const;
    bool operator>(const GnssLogTime& b) const;
    bool operator<=(const GnssLogTime& b) const;
    bool operator>=(const GnssLogTime& b) const;
    GnssLogTime& operator=(const GnssLogTime& b);
    PTime getUtcTime();
    std::string getStringUtcTime();
    std::string getStringIsoTime();
    unsigned short getWeek();
    unsigned int getMilliseconds();
private:
    unsigned short week_;
    unsigned int milliseconds_;

    std::string stringUtcTime_;
    std::string stringIsoTime_;

    PTime utcTime_;
};

#endif /* GNSSLOGTIME_HPP */

