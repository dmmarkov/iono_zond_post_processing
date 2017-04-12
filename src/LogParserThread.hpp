/*
 * File:   LogParserThread.hpp
 * Author: dmitri
 */

#ifndef LOGPARSERTHREAD_HPP
#define	LOGPARSERTHREAD_HPP

#include "project_defines.hpp"

#include <deque>
#include <vector>
#include <atomic>

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

#include "SatelliteGroup.hpp"
#include "GnssDataMeasurements2.hpp"

class LogParserThread {
public:
    LogParserThread();
    virtual ~LogParserThread();

    void pushRawData(unsigned char* data, size_t size_of_data);
private:
    SatelliteGroup* satelliteGroup_;

    GnssDataMeasurements2<BinaryRawData> rawDataNetwork_;

    uint64_t logsCount_;
    std::atomic<int> lockReleaseMemory_;
    std::atomic<int> lockPrintPlots_;
    std::atomic<bool> isThreadStop_;

    GnssLogTime lastLogTime_;

    LogParserThread(const LogParserThread& orig) = delete;
    LogParserThread& operator=(const LogParserThread& ) = delete;

    void parseRawNetworkData();

    void parseRawRANGE(unsigned char* data);
    void parseRawPSRPOS(unsigned char* data);
    void parseRawPSRXYZ(unsigned char* data);
    void parseRawSATVIS2(unsigned char* data);
    void parseRawSATXYZ2(unsigned char* data);
    void parseRawISMDETOBS(unsigned char* data);
    void parseRawISMREDOBS(unsigned char* data);

    void releaseUnusedMemory(unsigned short week, unsigned int milliseconds);
    void printPlotDataCoutMessages(unsigned short week, unsigned int milliseconds);
};

#endif	/* LOGPARSERTHREAD_HPP */

