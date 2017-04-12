/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   ISigmaPhiCalculation.hpp
 * Author: markov
 */

#ifndef ISIGMAPHICALCULATION_HPP
#define ISIGMAPHICALCULATION_HPP

#include "GnssLogTime.hpp"
#include "ICalculation.hpp"

class ISigmaPhiCalculation : public ICalculation {
public:
    virtual ~ISigmaPhiCalculation() {

    }

    virtual void pushDetrendedAdr(double detrendedAdr, unsigned short week, unsigned int milliseconds) = 0;
    virtual void pushElevationAngle(double elevationAngle, unsigned short week, unsigned int milliseconds) = 0;
    virtual void pushLockTime(double lockTime, unsigned short week, unsigned int milliseconds) = 0;
};

#endif /* ISIGMAPHICALCULATION_HPP */

