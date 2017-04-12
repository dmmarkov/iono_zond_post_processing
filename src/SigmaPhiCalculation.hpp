/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   SigmaPhiCalculation.hpp
 * Author: markov
 */

#ifndef SIGMAPHICALCULATION_HPP
#define SIGMAPHICALCULATION_HPP

#include <atomic>

#include <boost/thread/thread.hpp>

#include "GnssCommonTypes.hpp"
#include "GnssDataMeasurements2.hpp"
#include "ISatelliteChannel.hpp"
#include "ISigmaPhiCalculation.hpp"
#include "PlotData.hpp"

class SigmaPhiCalculation : public ISigmaPhiCalculation {
public:
    explicit SigmaPhiCalculation(ISatelliteChannel* satelliteChannel);
    virtual ~SigmaPhiCalculation();

    virtual void pushDetrendedAdr(double detrendedAdr, unsigned short week, unsigned int milliseconds) override;
    virtual void pushElevationAngle(double elevationAngle, unsigned short week, unsigned int milliseconds) override;
    virtual void pushLockTime(double lockTime, unsigned short week, unsigned int milliseconds) override;

    virtual void stopThreads() override;
    virtual void closePlotData() override;
    virtual void forceCloseOutDataFiles(GnssLogTime& time) override;

    virtual void mergeData() override;
    virtual void calculateData() override;
private:

    struct DataForSigmaPhiCalculation {
        unsigned short week;
        unsigned int milliseconds;
        double DetrendedAdr;
        double LockTime;
        double ElevationAngle;
        bool isProcessed;
    };

    struct DetrendedAdr {
        unsigned short week;
        unsigned int milliseconds;
        double DetrendedAdr;
    };

    struct ElevationAngle {
        unsigned short week;
        unsigned int milliseconds;
        double ElevationAngle;
    };

    struct LockTime {
        unsigned short week;
        unsigned int milliseconds;
        double LockTime;
    };

    std::atomic<bool> isStopThreads_;

    GnssDataMeasurements2<DetrendedAdr> detrendedAdr_;
    GnssDataMeasurements2<ElevationAngle> elevationAngle_;
    GnssDataMeasurements2<LockTime> lockTime_;
    GnssDataMeasurements2<DataForSigmaPhiCalculation> dataForSigmaPhiCalculation_;

    PlotData* sigmaPhiPlotData_;

    double sumOfDetrendedAdr_;
    double sumOfDetrendedAdr2_;

    boost::mutex calcLock_;
    boost::mutex mergeLock_;

    ISatelliteChannel* satelliteChannel_;
};

#endif /* SIGMAPHICALCULATION_HPP */

