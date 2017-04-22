/*
 * File:   SatelliteGroup.h
 * Author: dmitri
 */

#ifndef SATELLITEGROUP_H
#define	SATELLITEGROUP_H

#include "project_defines.hpp"

#include <list>
#include <atomic>
#include <mutex>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "struct_headers.hpp"
#include "Satellite.hpp"
#include "gnss_functions.hpp"
#include "GpsSatellite.hpp"
#include "GlonassSatellite.hpp"

class SatelliteGroup {
public:
    explicit SatelliteGroup();
    virtual ~SatelliteGroup();

    void writeISMRAWOBS(LogRawHeader* CurrentHeader, LogRawBodyISMRAWOBS* CurrentLog);
    void writeISMRAWTEC(LogRawHeader* CurrentHeader, LogRawBodyISMRAWTEC* CurrentLog);
    void writeSATXYZ2(LogRawHeader* CurrentHeader, LogRawBodySATXYZ2* CurrentLog);
    void writePSRPOS(LogRawHeader* CurrentHeader, LogRawBodyPSRPOS* CurrentLog);
    void writePSRXYZ(LogRawHeader* CurrentHeader, LogRawBodyPSRXYZ* CurrentLog);
    void writeRANGE(LogRawHeader* CurrentHeader, LogRawBodyRANGE* CurrentLog);
    void writeSATVIS2(LogRawHeader* CurrentHeader, LogBodySATVIS2* CurrentLog);
    void writeISMDETOBS(LogRawHeader* CurrentHeader, LogBodyISMDETOBS* CurrentLog);
    void writeISMREDOBS(LogRawHeader* CurrentHeader, LogRawBodyISMREDOBS* CurrentLog);

    void forceCloseOutDataFiles(GnssLogTime& time);
private:
    std::map<int, ISatellite*> satellites_;

    std::atomic<double> receiverLatitude_;
    std::atomic<double> receiverLongitude_;
    std::atomic<double> receiverHeightAboveSea_;

    boost::mutex mutexAddSatellite_;

    std::atomic<bool> isPsrPosSet_;

    ISatellite* addSatellite(unsigned char satelliteId, unsigned char satelliteSystem, char glonassFrequency, bool isRangeSource);
};

#endif	/* SATELLITEGROUP_H */

