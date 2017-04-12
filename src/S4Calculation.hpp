/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   S4Calculation.hpp
 * Author: markov
 */

#ifndef S4CALCULATION_HPP
#define S4CALCULATION_HPP

#include <list>
#include <atomic>

#include <boost/thread/thread.hpp>

#include "GnssLogTime.hpp"
#include "GnssDataMeasurements2.hpp"
#include "struct_headers.hpp"
#include "GnssCommonTypes.hpp"
#include "ISatelliteChannel.hpp"
#include "IS4Calculation.hpp"
#include "PlotData.hpp"
#include "S4IrregulationsSearch.hpp"

class S4Calculation : public IS4Calculation {
public:
    explicit S4Calculation(ISatelliteChannel* satelliteChannel);
    virtual ~S4Calculation();

    virtual void pushDetrendedPower(double detrendedPower, unsigned short week, unsigned int milliseconds) override;
    virtual void pushElevationAngle(double elevationAngle, unsigned short week, unsigned int milliseconds) override;
    virtual void pushCarrierToNoiseAndLockTime(double carrierToNoise,
                                       double lockTime,
                                       unsigned short week,
                                       unsigned int milliseconds) override;

    virtual void stopThreads() override;
    virtual void closePlotData() override;
    virtual void forceCloseOutDataFiles(GnssLogTime& time) override;

    virtual void mergeData() override;
    virtual void calculateData() override;
private:

    struct DataForS4Calculation {
        unsigned short week;
        unsigned int milliseconds;
        double DetrendedPower;
        double CarrierToNoise;
        double LockTime;
        double ElevationAngle;
        bool isProcessed;
    };

    struct DetrendedPower {
        unsigned short week;
        unsigned int milliseconds;
        double DetrendedPower;
    };

    struct ElevationAngle {
        unsigned short week;
        unsigned int milliseconds;
        double ElevationAngle;
    };

    struct CarrierToNoiseAndLockTime {
        unsigned short week;
        unsigned int milliseconds;
        double CarrierToNoise;
        double LockTime;
    };

    std::atomic<bool> isStopThreads_;

    GnssDataMeasurements2<DetrendedPower> detrendedPower_;
    GnssDataMeasurements2<ElevationAngle> elevationAngle_;
    GnssDataMeasurements2<CarrierToNoiseAndLockTime> carrierToNoiseAndLockTime_;
    GnssDataMeasurements2<DataForS4Calculation> dataForS4Calculation_;

    PlotData* s4PlotData_;

    double sumOfDetrendedPower_;
    double sumOfDetrendedPower2_;

    boost::mutex calcLock_;
    boost::mutex mergeLock_;

    ISatelliteChannel* satelliteChannel_;
    S4IrregulationsSearch* irregulationsSearch_;
};

#endif /* S4CALCULATION_HPP */

