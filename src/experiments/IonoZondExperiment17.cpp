/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   IonoZondExperiment17.cpp
 * Author: dmitri
 *
 * Created on 18 февраля 2017 г., 17:26
 */

#include <vector>

#include "IonoZondExperiment17.hpp"
#include "gnss_functions.hpp"
#include "ThreadManager.hpp"
#include "itoa_sse2.hpp"
#include "dtoa_milo.h"

static const size_t sizeOfMlVector = 3000;
static const size_t deviderCoefficient = 3000;

IonoZondExperiment17::IonoZondExperiment17(const int satelliteSystem, const int satelliteId, const double frequency) :
    isStopThreads_(false),
    countOfPushed_(0)
{
    satelliteSystem_ = satelliteSystem;
    satelliteId_ = satelliteId;
    frequency_ = frequency;

    lastMilliseconsTime_ = 0;

    workers.reserve(sizeOfMlVector / deviderCoefficient);

    for(size_t i = 0; i < (sizeOfMlVector / deviderCoefficient); ++i) {
        workers.push_back(new Experiment17Worker(satelliteSystem_, satelliteId_, frequency_));
    }

    Logger::getInstance().info("Create IonoZondExperiment17 " + intToString(satelliteId_));
}

IonoZondExperiment17::~IonoZondExperiment17() {
    for(auto it = workers.begin(); it != workers.end(); ++it) {
        delete (*it);
    }

    Logger::getInstance().info("Delete IonoZondExperiment17 " + intToString(satelliteId_));
}

void
IonoZondExperiment17::stopThreads() {
    isStopThreads_ = true;

    for (auto it = workers.begin(); it != workers.end(); ++it) {
        (*it)->stopThreads();
    }
}

void
IonoZondExperiment17::pushExperimentData(GnssLogTime *logTime, const double s4) {
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
IonoZondExperiment17::mergeData() {
// dummy method
}

void
IonoZondExperiment17::calculateData() {
    //dummy method
}

void
IonoZondExperiment17::closePlotData() {
    //dummy
}

void
IonoZondExperiment17::forceCloseOutDataFiles(GnssLogTime& time) {
    //dummy
}

//worker methods

Experiment17Worker::Experiment17Worker(const int satelliteSystem, const int satelliteId, const double frequency) :
    isStopThreads_(false),
    s4Data_(20000, true)
{
    satelliteSystem_ = satelliteSystem;
    satelliteId_ = satelliteId;
    frequency_ = frequency;

    sizeOfStringBuffer_ = 131072;
    stringBuffer_ = new char[sizeOfStringBuffer_];

    ThreadManager::getInstance().addNewCalculation(this);

    Logger::getInstance().info("Create Experiment17Worker " + intToString(satelliteId_));
}

Experiment17Worker::~Experiment17Worker() {
//    ThreadManager::getInstance().releaseCalculation(this);

    delete[] stringBuffer_;

    Logger::getInstance().info("Delete Experiment17Worker " + intToString(satelliteId_));
}

void
Experiment17Worker::pushExperimentData(const unsigned short week, unsigned int milliseconds, const double s4) {
    if (!isStopThreads_) {
        Experiment17Worker::Data el = {
            .week_ = week,
            .milliseconds_ = milliseconds,
            .s4 = s4
        };

        s4Data_.push_back(el);
    }
}

void
Experiment17Worker::stopThreads() {
    isStopThreads_ = true;

    ThreadManager::getInstance().releaseCalculation(this);
}

void
Experiment17Worker::mergeData() {
    //dummy
}

void
Experiment17Worker::calculateData() {
    boost::unique_lock<boost::mutex> lockMethod(calcLock_, boost::try_to_lock);

    if (lockMethod && !isStopThreads_) {
        s4Data_.lock();
    #ifdef DEBUG
        size_t countLines = 0;
    #endif

        size_t countPrecision = 0;
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

                countPrecision = 0;
                dtoa_milo((*it).s4, strPos);
                while(*strPos && countPrecision < 7) {
                    ++strPos;
                    ++countPrecision;
                }

                *strPos = '\0';
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
                printf("%1.5f | ", (*it).s4);
#else
                printf("%1.5f;", (*it).s4);
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
            Logger::getInstance().info("Experiment17Worker::calculateData " + intToString(satelliteSystem_) + " " + intToString(satelliteId_) + " " + doubleToString(frequency_, 0) + " count = " + intToString(countLines));
        }
    #endif
        s4Data_.unlock();
    }
}

void
Experiment17Worker::closePlotData() {
    //dummy
}

void
Experiment17Worker::forceCloseOutDataFiles(GnssLogTime& time) {
    //dummy
}
