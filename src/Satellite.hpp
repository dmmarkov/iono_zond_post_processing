/*
 * File:   Satellite.hpp
 * Author: markov
 */

#ifndef SATELLITE_HPP
#define	SATELLITE_HPP

#include "project_defines.hpp"

#include <vector>
#include <deque>
#include <string>
#include <atomic>

#include <mutex>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "GnssCommonTypes.hpp"
#include "ISatelliteChannel.hpp"
#include "ISatellite.hpp"
#include "IDoubleChannelCalculation.hpp"

class Satellite: public ISatellite {
public:
    Satellite();
    virtual ~Satellite();
    virtual int getSatelliteId() override;

    virtual std::string getSatelliteName() override;

    virtual void pushDetrendedPower(SatelliteChannelType channelType, double detrendedPower, unsigned short week, unsigned int milliseconds) override;
    virtual void pushCarrierToNoiseAndLockTime(SatelliteChannelType channelType, double carrierToNoise, double lockTime, unsigned short week, unsigned int milliseconds) override;
    virtual void pushDetrendedPhase(SatelliteChannelType channelType, double detrendedPhase, unsigned short week, unsigned int milliseconds) override;

    virtual void pushNovatelS4(SatelliteChannelType channelType, double novatelS4, unsigned short week, unsigned int milliseconds) override;
    virtual void pushNovatelSigmaPhi60(SatelliteChannelType channelType, double novatelSigmaPhi60, unsigned short week, unsigned int milliseconds) override;
    virtual void pushNovatelCodeCarrierDivergence(SatelliteChannelType channelType, double novatelCodeCarrierDivergence, unsigned short week, unsigned int milliseconds) override;

    virtual void pushSatVisParams(double elevationAngle, double azimuthAngle, unsigned short week, unsigned int milliseconds) override;

    virtual void pushSatelliteXyz(double x, double y, double z, unsigned short week, unsigned int milliseconds) override;

    virtual void pushPsrPhaseLockTime(SatelliteChannelType channelType, double pseudorange, double carrierPhase, double lockTime, double CarrierToNoise, unsigned short week, unsigned int milliseconds) override;
    virtual void forceCloseOutDataFiles(GnssLogTime& time) override;
protected:
    int satelliteId_;
    std::string satelliteName_;
    std::atomic<ISatelliteChannel*> firstChannel_;
    std::atomic<ISatelliteChannel*> secondChannel_;
    std::atomic<IDoubleChannelCalculation*> doubleChannelCalculation_;

    boost::mutex modifyChannels_;

    std::atomic<bool> isEnabledProcessing_;

    std::atomic<unsigned short> firstWeek_;
    std::atomic<unsigned int> firstMilliseconds_;

    virtual bool isChannelsExists() = 0;
    virtual void createSatelliteChannels(double elevationAngle, unsigned short week, unsigned int milliseconds);
    virtual void destroySatelliteChannels(double elevationAngle, unsigned short week, unsigned int milliseconds);

    void pushCalculatedElevationAngle(double elevationAngle, unsigned short week, unsigned int milliseconds);
private:
    std::atomic<int> lockPrintPlots_;
};

#endif	/* SATELLITE_HPP */

