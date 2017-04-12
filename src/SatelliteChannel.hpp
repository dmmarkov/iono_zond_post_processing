/*
 * File:   SatelliteChannel.hpp
 * Author: dmitri
 */

#ifndef SATELLITECHANNEL_HPP
#define	SATELLITECHANNEL_HPP

#include "project_defines.hpp"

#include <vector>
#include <deque>
#include <atomic>

#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "gnss_functions.hpp"
#include "struct_headers.hpp"
#include "ISatellite.hpp"
#include "GnssCommonTypes.hpp"
#include "IS4Calculation.hpp"
#include "ISigmaPhiCalculation.hpp"
#include "ISatelliteChannel.hpp"
#include "PlotData.hpp"

class SatelliteChannel : public ISatelliteChannel {
public:
    SatelliteChannel();
    virtual ~SatelliteChannel();

    virtual std::string getSatelliteName() override;
    virtual std::string getChannelName() override;
    virtual std::string getFullChannelName() override;
    virtual ISatellite* getSatellite() override;

    virtual void pushDetrendedPower(double detrendedPower, unsigned short week, unsigned int milliseconds) override;
    virtual void pushElevationAngle(double elevationAngle, unsigned short week, unsigned int milliseconds) override;
    virtual void pushCarrierToNoiseAndLockTime(double carrierToNoise, double lockTime, unsigned short week, unsigned int milliseconds) override;
    virtual void pushDetrendedPhase(double detrendedPhase, unsigned short week, unsigned int milliseconds) override;
    virtual void pushNovatelS4(double novatelS4, unsigned short week, unsigned int milliseconds) override;
    virtual void pushNovatelSigmaPhi60(double novatelSigmaPhi60, unsigned short week, unsigned int milliseconds) override;
    virtual void pushNovatelCodeCarrierDivergence(double novatelCodeCarrierDivergence, unsigned short week, unsigned int milliseconds) override;

    virtual double getFrequency() override;
    virtual double getWaveLength() override;

    virtual void stopThreads() override;

    virtual void closePlotData() override;

    virtual void forceCloseOutDataFiles(GnssLogTime& time) override;
protected:
    double frequency_;
    double waveLength_;
    SatelliteChannelType channelType_;
    ISatellite* parentSatellite_;
    std::string channelName_;
    std::string fullChannelName_;

    std::atomic<IS4Calculation*> s4Calculation_;
    std::atomic<ISigmaPhiCalculation*> sigmaPhiCalculation_;

    PlotData* elevationAnglePlotData_;
    PlotData* carrierToNoisePlotData_;
    PlotData* lockTimePlotData_;
    PlotData* novatelS4PlotData_;
    PlotData* novatelSigmaPhi60PlotData_;
    PlotData* novatelCodeCarrierDivergencePlotData_;

    void createPlotData();
};

#endif	/* SATELLITECHANNEL_HPP */

