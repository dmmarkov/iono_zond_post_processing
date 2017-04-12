/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   IS4Calculation.hpp
 * Author: markov
 */

#ifndef IS4CALCULATION_HPP
#define IS4CALCULATION_HPP

#include "GnssLogTime.hpp"
#include "ICalculation.hpp"

class IS4Calculation : public ICalculation {
public:
    virtual ~IS4Calculation() {

    }

    virtual void pushDetrendedPower(double detrendedPower, unsigned short week, unsigned int milliseconds) = 0;
    virtual void pushElevationAngle(double elevationAngle, unsigned short week, unsigned int milliseconds) = 0;
    virtual void pushCarrierToNoiseAndLockTime(double carrierToNoise,
                                       double lockTime,
                                       unsigned short week,
                                       unsigned int milliseconds) = 0;
};

#endif /* IS4CALCULATION_HPP */

