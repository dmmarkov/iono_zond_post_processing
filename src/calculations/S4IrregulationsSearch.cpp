/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   S4IrregulationsSearch.cpp
 * Author: dmitri
 * 
 * Created on 6 Июнь 2016 г., 22:34
 */

#include <thread>
#include <chrono>

#include "S4IrregulationsSearch.hpp"
#include "GlobalConfiguration.hpp"
#include "common_functions.hpp"
#include "Logger.hpp"
#include "gnss_const.hpp"

S4IrregulationsSearch::S4IrregulationsSearch(ISatelliteChannel* satelliteChannel) {
//    isStopThreads_ = false;
//    
//    satelliteChannel_ = satelliteChannel;
//    
//    calculateData_ = boost::thread(boost::bind(&S4IrregulationsSearch::calculateData, this));
//
//    Logger::getInstance().info("Create S4IrregulationsSearch for channel " + satelliteChannel_->getFullChannelName());
}

S4IrregulationsSearch::~S4IrregulationsSearch() {
//    calculateData_.join();
//    
//    {
//        boost::unique_lock<boost::mutex> lock(mutexLockPush_);
//
//        for(auto it = listWriteOfS4Data_.begin(); it != listWriteOfS4Data_.end(); ++it) {
//            delete *it;
//        }
//
//        for(auto it = listReadOfS4Data_.begin(); it != listReadOfS4Data_.end(); ++it) {
//            delete *it;
//        }
//    }
//    
//    outFile_.close();
}

void 
S4IrregulationsSearch::stopThreads() {
    isStopThreads_ = true;
}

void 
S4IrregulationsSearch::pushS4Value(unsigned short week, unsigned int milliseconds, double s4value) {
//    boost::unique_lock<boost::mutex> lock(mutexLockPush_);
//    
//    S4Data* newS4 = new S4Data;
//    newS4->week = week;
//    newS4->milliseconds = milliseconds;
//    newS4->s4value = s4value;
//    
//    listWriteOfS4Data_.push_back(newS4);
//    
//    if (!outFile_.is_open()) {
//        std::string fileName = GlobalConfiguration::getInstance().getPathForIrregulationsOutput() + "/" + satelliteChannel_->getSatelliteName() + "_" + satelliteChannel_->getChannelName() + "_" + intToString(week) + "_" + intToString(milliseconds);
//        outFile_.open(fileName, std::ios::out);
//    }
}

void S4IrregulationsSearch::moveDataToReadList(){
//    boost::unique_lock<boost::mutex> lock(mutexLockPush_);
//    
//    for(auto it = listWriteOfS4Data_.begin(); it != listWriteOfS4Data_.end(); ++it) {
//        listReadOfS4Data_.push_back(*it);
//    }
//    
//    listWriteOfS4Data_.erase(listWriteOfS4Data_.begin(), listWriteOfS4Data_.end());
}

void
S4IrregulationsSearch::calculateData() {
//    std::this_thread::sleep_for(std::chrono::milliseconds(100));
//    
//    while (!isStopThreads_) {
//        moveDataToReadList();
//        
//        size_t counter = 0;
//        double sumOfElements = 0;
//        auto firstEl = listReadOfS4Data_.begin();
//        for(auto it = listReadOfS4Data_.begin(); it != listReadOfS4Data_.end(); ++it) {
//            if (counter < 30000) {
//                sumOfElements += (*it)->s4value;
//                counter++;
//            } else {
//                double newExpectedValue = sumOfElements / counter;
//                
//                if (newExpectedValue > S4_IRREGULATION_VALUE) {
//                    outFile_ << (*firstEl)->week << ";" << (*firstEl)->milliseconds << ";" << (*firstEl)->s4value << ";" << (*it)->week << ";" << (*it)->milliseconds << ";" << (*it)->s4value << "\n";
//                }
//                
//                sumOfElements -= (*firstEl)->s4value;
//                sumOfElements += (*it)->s4value;
//                delete *firstEl;
//                firstEl++;
//            }
//        }
//        
//        if (listReadOfS4Data_.begin() != firstEl) {
//            listReadOfS4Data_.erase(listReadOfS4Data_.begin(), firstEl);
//        }
//        
//        std::this_thread::sleep_for(std::chrono::milliseconds(100));
//    }
}

