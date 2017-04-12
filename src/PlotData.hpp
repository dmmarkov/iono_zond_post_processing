/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   PlotData.hpp
 * Author: dmitri
 *
 * Created on 8 Май 2016 г., 19:10
 */

#ifndef PLOTDATA_HPP
#define PLOTDATA_HPP

#include <string>
#include <fstream>
#include <atomic>

#include <mutex>
#include "boost/date_time/posix_time/posix_time.hpp"

#include "GnssLogTime.hpp"

class PlotData {
public:
    explicit PlotData(std::string satelliteName, std::string channelName, std::string outFileName);
    explicit PlotData(std::string satelliteName, std::string outFileName);
    virtual ~PlotData();

    void pushData(unsigned short week, unsigned int milliseconds, double value);
    void closeFile();

    void disableOutput();

    void forceCloseOutDataFiles(GnssLogTime& time);
private:
    std::string timeName_;
    std::string satelliteName_;
    std::string channelName_;
    std::string outFileName_;

    std::string outFullFileName_;
    std::string outRelativeFileName_;

    std::ofstream outPlotData_;

    boost::mutex lockOutPlotData_;

    std::atomic<bool> isEnabledOutput_;

    boost::posix_time::ptime fakeTime_;
    boost::posix_time::time_period timePeriod_;

    void initNewOutPlot(GnssLogTime& time);
    void internalCloseFile();
    void createOutPutFile();
    void pushData(GnssLogTime& time, double value);

    PlotData(const PlotData& orig) = delete;
    PlotData& operator=(const PlotData& ) = delete;
};

#endif /* PLOTDATA_HPP */

