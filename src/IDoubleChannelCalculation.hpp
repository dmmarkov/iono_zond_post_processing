/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   IDoubleChannelCalculation.hpp
 * Author: markov
 */

#ifndef IDOUBLECHANNELCALCULATION_HPP
#define IDOUBLECHANNELCALCULATION_HPP

#include "GnssLogTime.hpp"
#include "ICalculation.hpp"

class IDoubleChannelCalculation : public ICalculation {
public:
    virtual ~IDoubleChannelCalculation() {

    }

    virtual void pushPsrPhaseLockTime1(double pseudorange, double carrierPhase, double lockTime, double carrierToNoise, unsigned short week, unsigned int milliseconds) = 0;
    virtual void pushPsrPhaseLockTime2(double pseudorange, double carrierPhase, double lockTime, double carrierToNoise, unsigned short week, unsigned int milliseconds) = 0;
    virtual void pushElevationAngle(double elevationAngle, unsigned short week, unsigned int milliseconds) = 0;
};

#endif /* IDOUBLECHANNELCALCULATION_HPP */

