/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   IonoZondExperiment20.cpp
 * Author: dmitri
 *
 * Created on 22 апреля 2017 г., 16:31
 */

#include <sstream>

#include "IonoZondExperiment20.hpp"
#include "Logger.hpp"
#include "common_functions.hpp"

IonoZondExperiment20::IonoZondExperiment20(ISatellite* satellite) {
    satellite_ = satellite;

    minTecValue_ = -200.0;
    maxTecValue_ = +200.0;
    intervalCount_ = 4000;
    stepOfCounter_ = (maxTecValue_ - minTecValue_) / (double) intervalCount_;

    counters_.resize(intervalCount_);

    std::stringstream filename;

    switch (satellite_->getSatelliteSystem()) {
        case SatelliteSystem::GPS:
            filename << "GPS.";
            break;
        case SatelliteSystem::GLONASS:
            filename << "GLONASS.";
            break;
        default:
            Logger::getInstance().error("Unknown satellite system");
            break;
    }
    filename << satellite_->getSatelliteId() << ".csv";

    outData_.open(filename.str(), std::ios::out);
}

IonoZondExperiment20::~IonoZondExperiment20() {

    outData_.close();

    std::stringstream str;

    switch (satellite_->getSatelliteSystem()) {
        case SatelliteSystem::GPS:
            str << "GPS ";
            break;
        case SatelliteSystem::GLONASS:
            str << "GLONASS ";
            break;
        default:
            Logger::getInstance().error("Unknown satellite system");
            break;
    }

    str << satellite_->getSatelliteId();

    size_t counter = 0;
    for (auto &t : counters_) {
        str << " " << t;
        counter += t;
    }

    if (counter > 180000) {
        printCout(str.rdbuf());
    }
}

void
IonoZondExperiment20::pushTec(const double tec) {
#warning Implement me please
//    size_t i = (int)((tec - minTecValue_) / stepOfCounter_);
//    доделать вывод значений в файл и посмотреть что с ними не так
////    std::stringstream str;
////    str << doubleToString(tec, 10) << " " << i;
////    printCout(str.rdbuf());
////
////    if (((int)i - 1) < 0) {
////        std::stringstream str;
////        str << doubleToString(tec, 10) << " " << doubleToString(stepOfCounter_, 10) << " " << i;
//////        perror(str.str().c_str());
////        printCout(str.rdbuf());
////        exit(EXIT_FAILURE);
////    }
//
//    if (i >= counters_.size()) {
//        std::stringstream str;
//        str << doubleToString(tec, 10) << " " << doubleToString(stepOfCounter_, 10) << " " << i;
////        perror(str.str().c_str());
//        printCout(str.rdbuf());
//        exit(EXIT_FAILURE);
//    }
//
//    counters_.at(i)++;
}