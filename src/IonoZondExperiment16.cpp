/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   IonoZondExperiment16.cpp
 * Author: dmitri
 *
 * Created on 18 февраля 2017 г., 17:26
 */

#include <vector>

#include "IonoZondExperiment16.hpp"
#include "gnss_functions.hpp"
#include "ThreadManager.hpp"
#include "itoa_sse2.hpp"
#include "dtoa_milo.h"

static const size_t sizeOfMlVector = 3000;
static const size_t deviderCoefficient = 3000;

IonoZondExperiment16::IonoZondExperiment16(const int satelliteSystem, const int satelliteId, const double frequency) :
    isStopThreads_(false),
    countOfPushed_(0)
{
    satelliteSystem_ = satelliteSystem;
    satelliteId_ = satelliteId;
    frequency_ = frequency;

    lastMilliseconsTime_ = 0;
    workers.reserve(sizeOfMlVector / deviderCoefficient);

    for(size_t i = 0; i < (sizeOfMlVector / deviderCoefficient); ++i) {
        workers.push_back(new Experiment16Worker(satelliteSystem_, satelliteId_, frequency_));
    }

    Logger::getInstance().info("Create IonoZondExperiment16 " + intToString(satelliteId_));
}

IonoZondExperiment16::~IonoZondExperiment16() {
    for(auto it = workers.begin(); it != workers.end(); ++it) {
        delete (*it);
    }

    Logger::getInstance().info("Delete IonoZondExperiment16 " + intToString(satelliteId_));
}

void
IonoZondExperiment16::stopThreads() {
    isStopThreads_ = true;

    for (auto it = workers.begin(); it != workers.end(); ++it) {
        (*it)->stopThreads();
    }
}

void
IonoZondExperiment16::pushExperimentData(GnssLogTime *logTime, const double s4) {
    if (!isStopThreads_) {
        if (countOfPushed_ < (sizeOfMlVector / deviderCoefficient)) {
            countOfPushed_++;
        }

        size_t i = 0;
        for (auto it = workers.begin(); it != workers.end() && i < countOfPushed_; ++it, ++i) {
            (*it)->pushExperimentData(logTime->getWeek(), logTime->getMilliseconds(), s4);
        }
    }
}

void
IonoZondExperiment16::mergeData() {
    //dummy method
}

void
IonoZondExperiment16::calculateData() {
    //dummy method
}

void
IonoZondExperiment16::closePlotData() {
    //dummy
}

void
IonoZondExperiment16::forceCloseOutDataFiles(GnssLogTime& time) {
    //dummy
}

//worker methods

Experiment16Worker::Experiment16Worker(const int satelliteSystem, const int satelliteId, const double frequency) :
    isStopThreads_(false),
    s4Data_(20000, true)
{
    satelliteSystem_ = satelliteSystem;
    satelliteId_ = satelliteId;
    frequency_ = frequency;

    sizeOfStringBuffer_ = 131072;
    stringBuffer_ = new char[sizeOfStringBuffer_];

    ThreadManager::getInstance().addNewCalculation(this);

    Logger::getInstance().info("Create Experiment16Worker " + intToString(satelliteId_));
}

Experiment16Worker::~Experiment16Worker() {
//    ThreadManager::getInstance().releaseCalculation(this);

    delete[] stringBuffer_;

    Logger::getInstance().info("Delete Experiment16Worker " + intToString(satelliteId_));
}

void
Experiment16Worker::pushExperimentData(const unsigned short week, unsigned int milliseconds, const double s4) {
    if (!isStopThreads_) {
        Experiment16Worker::Data el = {
            .week_ = week,
            .milliseconds_ = milliseconds,
            .s4 = s4
        };

        s4Data_.push_back(el);
    }
}

void
Experiment16Worker::stopThreads() {
    isStopThreads_ = true;
    ThreadManager::getInstance().releaseCalculation(this);
}

void
Experiment16Worker::mergeData() {
    //dummy
}

void
Experiment16Worker::calculateData() {
    boost::unique_lock<boost::mutex> lockMethod(calcLock_, boost::try_to_lock);

    if (lockMethod && !isStopThreads_) {
        s4Data_.lock();
    #ifdef DEBUG
        size_t countLines = 0;
    #endif
        while (s4Data_.size() >= sizeOfMlVector) {
            auto strPos = stringBuffer_;
//            auto strEnd = stringBuffer_ + sizeOfStringBuffer_; //used for protection, but not used now

#if 1
            std::strcpy(strPos, "satellite_system_id:");
            strPos += 20;
#endif
            u32toa_sse2(satelliteSystem_, strPos);
            while(*strPos) {
                ++strPos;
            }

#if 1
            std::strcpy(strPos, " frequency:");
            strPos += 11;
#else
            *strPos = ';';
            ++strPos;
#endif
            u32toa_sse2((size_t)frequency_, strPos);
            while(*strPos) {
                ++strPos;
            }

            size_t counter = 1;
            auto it = s4Data_.begin();
            auto end = s4Data_.end();
            auto erased = s4Data_.begin();
            for(; it != end; ++it) {
#if 1
                std::strcpy(strPos, " s4_");
                strPos += 4;

                u32toa_sse2(counter, strPos);
                while(*strPos) {
                    ++strPos;
                }

                *strPos = ':';
                strPos += 1;
#else
                *strPos = ';';
                ++strPos;
#endif

                dtoa_milo((*it).s4, strPos);
                while(*strPos) {
                    ++strPos;
                }

                counter++;

                if (counter <= sizeOfMlVector / deviderCoefficient) {
                    ++erased;
                }

                if (counter == sizeOfMlVector) {
                    break;
                }
            }

            ++it;
            ThreadManager::getInstance().someOneUsedCout();

            {
                boost::unique_lock<boost::mutex> lock(mutexLockCout_);
                //выводим последний элемент и приплюсовываем всю остальную строку
#if 1
                printf("%1.10f | ", (*it).s4);
#else
                printf("%1.10f;", (*it).s4);
#endif
                fputs(stringBuffer_, stdout);
                fputc('\n', stdout);
//                    fflush(stdout);
//                    quick_exit(1);
#ifdef DEBUG
                countLines++;
#endif
            }

            s4Data_.erase(s4Data_.begin(), erased);
        }
    #ifdef DEBUG
        if (countLines > 0) {
            Logger::getInstance().info("Experiment16Worker::calculateData " + intToString(satelliteSystem_) + " " + intToString(satelliteId_) + " countLines = " + intToString(countLines));
        }
    #endif
        s4Data_.unlock();
    }
}

void
Experiment16Worker::closePlotData() {
    //dummy
}

void
Experiment16Worker::forceCloseOutDataFiles(GnssLogTime& time) {
    //dummy
}
