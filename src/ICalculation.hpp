/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   ICalculation.hpp
 * Author: dmitri
 *
 * Created on 26 февраля 2017 г., 8:52
 */

#ifndef ICALCULATION_HPP
#define ICALCULATION_HPP

#include "GnssLogTime.hpp"

class ICalculation {
public:
    virtual ~ICalculation() {

    }

    virtual void mergeData() = 0;
    virtual void calculateData() = 0;

    virtual void stopThreads() = 0;

    virtual void closePlotData() = 0;
    virtual void forceCloseOutDataFiles(GnssLogTime& time) = 0;
};

#endif /* ICALCULATION_HPP */

