/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PlotData.cpp
 * Author: dmitri
 * 
 * Created on 8 Р СљР В°Р в„– 2016 Р С–., 19:10
 */

#include <thread>
#include <chrono>

#include "PlotData.hpp"
#include "common_functions.hpp"
#include "Logger.hpp"

PlotData::PlotData(std::string satelliteName, std::string channelName, std::string outFileName) : timePeriod_(fakeTime_, boost::posix_time::time_duration(0, 0, 0, 0)) {
    isEnabledOutput_ = true;
    satelliteName_ = satelliteName;
    channelName_ = channelName;
    outFileName_ = outFileName;

    outFullFileName_ = "";
    outRelativeFileName_ = "";
}

PlotData::PlotData(std::string satelliteName, std::string outFileName) : timePeriod_(fakeTime_, boost::posix_time::time_duration(0, 0, 0, 0)) {    
    isEnabledOutput_ = true;
    satelliteName_ = satelliteName;
    channelName_ = "";
    outFileName_ = outFileName;

    outFullFileName_ = "";
    outRelativeFileName_ = "";
}

PlotData::~PlotData() {
//    isEnabledOutput_ = false;
//    
//    {
//        internalCloseFile();
//    }
}

void 
PlotData::createOutPutFile() {    
//    std::string tempStr1 = "", tempStr2 = "";
//    if (channelName_ == "") {
//	PrintPlots::createDirectoriesForPlots(timeName_, satelliteName_, &tempStr1, &tempStr2);
//    } else {
//	PrintPlots::createDirectoriesForPlots(timeName_, satelliteName_, channelName_, &tempStr1, &tempStr2);
//    }
//    
//    outFullFileName_ = tempStr1 + "/" + outFileName_ + ".csv";
//    outRelativeFileName_ = tempStr2 + "/" + outFileName_ + ".csv\n";
//
//    outPlotData_.exceptions(std::fstream::failbit | std::fstream::badbit);
//    try {
//        outPlotData_.open(outFullFileName_, std::ios::out);
//    } catch (std::exception& e) {
//        Logger::getInstance().error("File " + std::string(e.what()) + " open error");        
//    } 
//    
//    if (outPlotData_.is_open()) {
//        outPlotData_ << "time;value" << std::endl;
//    } else {
//        Logger::getInstance().error("File " + outFullFileName_ + " open error");
//    }      
}

void
PlotData::initNewOutPlot(GnssLogTime& time) {
//    unsigned int lastMilliseconds = time.getMilliseconds() % 1000;
//    int lastMinutes = time.getUtcTime().time_of_day().minutes();
//    int lastSeconds = time.getUtcTime().time_of_day().seconds();
//
//    //if the minutes great than 30, so we just need to reduce only on value until 30, not much
//    if (lastMinutes >= 30) {
//        lastMinutes -= 30;
//    }
//
//    unsigned int firstMilliseconds = time.getMilliseconds() - ((lastMinutes * 60 + lastSeconds) * 1000 + lastMilliseconds);
//    
//    GnssLogTime* firstTime = new GnssLogTime(time.getWeek(), firstMilliseconds);
//    
//    PTime lastTime = firstTime->getUtcTime() + boost::posix_time::time_duration(0, 30, 0, 0);
//    timePeriod_ = boost::posix_time::time_period(firstTime->getUtcTime(), lastTime);
//    timeName_ = firstTime->getStringUtcTime() + " till " + pTimeToSimpleString(lastTime);    
//    
//#ifdef _WIN32_WINNT
//    timeName_.replace(14, 1, ".");
//    timeName_.replace(17, 1, ".");
//    timeName_.replace(40, 1, ".");
//    timeName_.replace(43, 1, ".");
//#endif    
//    
//    delete firstTime;
//    
//    createOutPutFile();  
}

void PlotData::closeFile() {
//    boost::unique_lock<boost::mutex> lock(lockOutPlotData_); 
//    
//    internalCloseFile();
}

void PlotData::internalCloseFile(){    
//    if (outPlotData_.is_open()) {
//        outPlotData_.flush();
//        outPlotData_.close();
//    }
}

void
PlotData::pushData(GnssLogTime& time, double value) {
//    if (isEnabledOutput_) {
//        if (outPlotData_.is_open()) {
//            if (!timePeriod_.contains(time.getUtcTime())) {                
//                internalCloseFile();                
//                initNewOutPlot(time);
//            }
//            
//            if (outPlotData_.is_open()) {
//                outPlotData_ << time.getStringIsoTime() << ";" << doubleToString(value, 10) << '\n';
//            }
//        } else {            
//            initNewOutPlot(time);
//            if (outPlotData_.is_open()) {
//                outPlotData_ << time.getStringIsoTime() << ";" << doubleToString(value, 10) << '\n';
//            }
//        }
//    }    
}

void
PlotData::pushData(unsigned short week, unsigned int milliseconds, double value) {
//    boost::unique_lock<boost::mutex> lock(lockOutPlotData_);  
//    
//    if (isEnabledOutput_) {
//        GnssLogTime time(week, milliseconds);
//        pushData(time, value);
//    }
}

void
PlotData::disableOutput() {
//    boost::unique_lock<boost::mutex> lock(lockOutPlotData_);
//    
//    isEnabledOutput_ = false;
}

void 
PlotData::forceCloseOutDataFiles(GnssLogTime& time) {
//    //it is the moment when we lost satellite but it's upper then 15 degrees
//    //so the timeperiod will not be updated and later it will be the bug which 
//    //will be look like the delay of plot drawing 
//    boost::unique_lock<boost::mutex> lock(lockOutPlotData_);
//        if (isEnabledOutput_) {
//        if (!timePeriod_.contains(time.getUtcTime())) {
//            isEnabledOutput_ = false;
//            internalCloseFile();
//        }
//    }
}