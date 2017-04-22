/*
 * File:   LogParserThread.cpp
 * Author: dmitri
 */

#include "boost/date_time/posix_time/posix_time.hpp"

#include "LogParserThread.hpp"
#include "crc32.hpp"
#include "common_functions.hpp"
#include "Logger.hpp"
#include "gnss_const.hpp"
#include "server_consts.hpp"
#include "SvmPredictionData.hpp"
#include "ThreadManager.hpp"

LogParserThread::LogParserThread() : rawDataNetwork_(5) {
    satelliteGroup_ = new SatelliteGroup();

    lockReleaseMemory_ = LOCK_RELEASE_MEMORY;
    lockPrintPlots_ = 0; //if the program will be started less then some minutes, we may get the problem when not last data will be drawn

    isThreadStop_ = false;

    logsCount_ = 0;
}

LogParserThread::~LogParserThread() {
    Logger::getInstance().info("LogParserThread::~LogParserThread() begin");

    isThreadStop_ = true;

    delete satelliteGroup_;

    Logger::getInstance().info("LogParserThread::~LogParserThread() end");
}

void
LogParserThread::pushRawData(unsigned char* data, size_t size_of_data) {
    BinaryRawData* el = new BinaryRawData(data, size_of_data);
    rawDataNetwork_.push_back(*el);
    delete el;

    parseRawNetworkData();
}

void
LogParserThread::parseRawNetworkData() {
    rawDataNetwork_.lock();
    try {
        if (rawDataNetwork_.size() > 0) {
            //get size of data
            size_t sizeOfData = 0;
            size_t allocatedMemorySize = 0;

            for (auto it = rawDataNetwork_.begin(); it != rawDataNetwork_.end(); ++it) {
                sizeOfData += (*it).size;
            }

            if (sizeOfData > 28) {
                allocatedMemorySize = sizeOfData;
                unsigned char* sourceData = (unsigned char*) allocateHugePages(&allocatedMemorySize);
                {
                    size_t sourceOffset = 0;
                    for (auto it = rawDataNetwork_.begin(); it != rawDataNetwork_.end(); ++it) {
                        memcpy(&sourceData[sourceOffset], (*it).data, (*it).size);
                        sourceOffset += (*it).size;
                    }
                }

                LogRawHeader* currentHeader = reinterpret_cast<LogRawHeader*> (sourceData);
                size_t countOfProcessed = 0;
                while (countOfProcessed < (sizeOfData - 28)) {
                    if (
                            (currentHeader->firstByte == 0xAA) &&
                            (currentHeader->secondByte == 0x44) &&
                            (currentHeader->thirdByte == 0x12) &&
                            (currentHeader->headerLength == 28)
                            ) {
                        //copy the memory and push it to the que of full parse
                        size_t fullLogLength = currentHeader->messageLength + currentHeader->headerLength + 4;
                        if ((countOfProcessed + fullLogLength) > sizeOfData) {
                            break;
                        }

                        if (currentHeader->messageID == 43) {
                            GnssLogTime currentLogTime(currentHeader->week, currentHeader->milliseconds);

                            if (lastLogTime_.getWeek() != 0) {
                                boost::posix_time::time_duration timeDiff = currentLogTime.getUtcTime() - lastLogTime_.getUtcTime();

                                if ( timeDiff.total_milliseconds() != 20 && logsCount_ != 0 ) {
                                    std::cerr << "Recreate satelliteGroup!\n" << "Previous timestamp " << lastLogTime_.getStringIsoTime() << "Current timestamp " << currentLogTime.getStringIsoTime() << "\n";

                                    std::this_thread::sleep_for(std::chrono::seconds(10));

                                    ThreadManager::getInstance().releaseAll();

                                    std::this_thread::sleep_for(std::chrono::seconds(1));

                                    delete satelliteGroup_;
                                    SvmPredictionData::getInstance().restart();
                                    satelliteGroup_ = new SatelliteGroup();

                                    logsCount_ = 0;
                                }
                            }

                            lastLogTime_ = currentLogTime;
                        }

                        logsCount_++;

                        switch (currentHeader->messageID) {
                            case 43:
                                parseRawRANGE(sourceData + countOfProcessed);
                                break;
                            case 47:
                                parseRawPSRPOS(sourceData + countOfProcessed);
                                break;
                            case 243:
                                parseRawPSRXYZ(sourceData + countOfProcessed);
                                break;
                            case 1043:
                                parseRawSATVIS2(sourceData + countOfProcessed);
                                break;
                            case 1389:
                                //                                    parseRawISMRAWOBS(sourceData + countOfProcessed);
                                break;
                            case 1390:
                                //                                    parseRawISMRAWTEC(sourceData + countOfProcessed);
                                break;
                            case 1451:
                                parseRawSATXYZ2(sourceData + countOfProcessed);
                                break;
                            case 1395:
                                parseRawISMDETOBS(sourceData + countOfProcessed);
                                break;
                            case 1393:
                                parseRawISMREDOBS(sourceData + countOfProcessed);
                                break;
                            default:
                                Logger::getInstance().error("Unknown log type MessageID = " + intToString(currentHeader->messageID));
                                break;
                        }

                        countOfProcessed += fullLogLength;
                        //this is only one true method because I can't move pointer with ++ correctly so I need to use it
                        currentHeader = reinterpret_cast<LogRawHeader*> (sourceData + countOfProcessed);

                        if (logsCount_ % 10000 == 0) {
                            Logger::getInstance().info("Found " + intToString(logsCount_) + " logs");
                        }
                    } else {
                        //if something wrong will be found strange inside the data it must be ignored
                        ++countOfProcessed;
                        currentHeader = reinterpret_cast<LogRawHeader*> (sourceData + countOfProcessed);
                    }
                }

                rawDataNetwork_.erase(rawDataNetwork_.begin(), rawDataNetwork_.end());
                //save tail
                try {
                    size_t sizeOfTail = sizeOfData - countOfProcessed;
                    if (sizeOfTail > 0) {
                        BinaryRawData *el = new BinaryRawData(sourceData + countOfProcessed, sizeOfTail);
                        rawDataNetwork_.push_back(*el); //add like first because it's right order of data
                        delete el;
                    }
                } catch (std::exception& ex) {
                    Logger::getInstance().error(std::string("Save tail error: ") + ex.what());
                }

                freeHugePages(sourceData, allocatedMemorySize);
            }
        }
    } catch (const std::exception &ex) {
        Logger::getInstance().error(ex.what());
    }
    rawDataNetwork_.unlock();
}

void
LogParserThread::parseRawRANGE(unsigned char* data) {
    LogRawHeader* currentHeader = reinterpret_cast<LogRawHeader*> (data);
    unsigned int CurrentCRC32Value = *(reinterpret_cast<unsigned int *> (data + currentHeader->headerLength + currentHeader->messageLength));

    if (CurrentCRC32Value == NovatelEvaluateCRC32Value(currentHeader->headerLength + currentHeader->messageLength, data)) {
        unsigned int numberOfObservation = *(reinterpret_cast<unsigned int *> (data + currentHeader->headerLength));

        for (unsigned int i = 0; i < numberOfObservation; ++i) {
            size_t dataOffset = currentHeader->headerLength + 4 + sizeof (LogRawBodyRANGE) * i;
            LogRawBodyRANGE* bodyRange = reinterpret_cast<LogRawBodyRANGE *> (data + dataOffset);

            //write data to group
            satelliteGroup_->writeRANGE(currentHeader, bodyRange);
        }
    } else {
        Logger::getInstance().error("bad crc but good header!!!");
    }
}

void
LogParserThread::parseRawPSRPOS(unsigned char* data) {
    LogRawHeader* currentHeader = reinterpret_cast<LogRawHeader*> (data);
    unsigned int CurrentCRC32Value = *(reinterpret_cast<unsigned int*> (data + currentHeader->headerLength + currentHeader->messageLength));

    if (CurrentCRC32Value == NovatelEvaluateCRC32Value(currentHeader->headerLength + currentHeader->messageLength, data)) {
        LogRawBodyPSRPOS* bodyPSRPOS = reinterpret_cast<LogRawBodyPSRPOS *> (data + 28);
        //write data to group
        satelliteGroup_->writePSRPOS(currentHeader, bodyPSRPOS);
    } else {
        Logger::getInstance().error("bad crc but good header!!!");
    }
}

void
LogParserThread::parseRawPSRXYZ(unsigned char* data) {
    LogRawHeader* currentHeader = reinterpret_cast<LogRawHeader*> (data);
    unsigned int CurrentCRC32Value = *(reinterpret_cast<unsigned int *> (data + currentHeader->headerLength + currentHeader->messageLength));

    if (CurrentCRC32Value == NovatelEvaluateCRC32Value(currentHeader->headerLength + currentHeader->messageLength, data)) {
        LogRawBodyPSRXYZ* bodyPSRXYZ = reinterpret_cast<LogRawBodyPSRXYZ *> (data + 28);
        //write data to group
        satelliteGroup_->writePSRXYZ(currentHeader, bodyPSRXYZ);
    } else {
        Logger::getInstance().error("bad crc but good header!!!");
    }
}

void
LogParserThread::parseRawSATVIS2(unsigned char* data) {
    LogRawHeader* currentHeader = reinterpret_cast<LogRawHeader*> (data);
    unsigned int CurrentCRC32Value = *(reinterpret_cast<unsigned int *> (data + currentHeader->headerLength + currentHeader->messageLength));

    if (CurrentCRC32Value == NovatelEvaluateCRC32Value(currentHeader->headerLength + currentHeader->messageLength, data)) {
        unsigned int satelliteSystem = *(reinterpret_cast<unsigned int *> (data + currentHeader->headerLength));
        unsigned int isSatelliteVisibilityValid = *(reinterpret_cast<unsigned int *> (data + currentHeader->headerLength + 4));
        unsigned int isCompleteGNSSAlmanacUsed = *(reinterpret_cast<unsigned int *> (data + currentHeader->headerLength + 8));
        unsigned int numberOfSatellites = *(reinterpret_cast<unsigned int *> (data + currentHeader->headerLength + 12));

        for (unsigned int i = 0; i < numberOfSatellites; ++i) {
            size_t dataOffset = currentHeader->headerLength + 16 + sizeof (LogRawBodySATVIS2) * i;
            LogRawBodySATVIS2* bodySATVIS2 = reinterpret_cast<LogRawBodySATVIS2 *> (data + dataOffset);

            LogBodySATVIS2 bodyForWriting;
            bodyForWriting.satelliteSystem = satelliteSystem;
            bodyForWriting.isSatelliteVisibilityValid = isSatelliteVisibilityValid == 1;
            bodyForWriting.isCompleteGNSSAlmanacUsed = isCompleteGNSSAlmanacUsed == 1;
            bodyForWriting.satelliteID = bodySATVIS2->satelliteID;
            bodyForWriting.glonassFrequency = bodySATVIS2->glonassFrequency;
            bodyForWriting.satelliteHealth = bodySATVIS2->satelliteHealth;
            bodyForWriting.elevationAngle = bodySATVIS2->elevationAngle;
            bodyForWriting.azimuthAngle = bodySATVIS2->AzimuthAngle;
            bodyForWriting.theoreticalDopplerOfSatellite = bodySATVIS2->theoreticalDopplerOfSatellite;
            bodyForWriting.ApparentDopplerForThisReceiver = bodySATVIS2->ApparentDopplerForThisReceiver;
            //write data to group
            satelliteGroup_->writeSATVIS2(currentHeader, &bodyForWriting);
        }


    } else {
        Logger::getInstance().error("bad crc but good header!!!");
    }
}

void
LogParserThread::parseRawSATXYZ2(unsigned char* data) {
    LogRawHeader* currentHeader = reinterpret_cast<LogRawHeader*> (data);
    unsigned int CurrentCRC32Value = *(reinterpret_cast<unsigned int *> (data + currentHeader->headerLength + currentHeader->messageLength));

    if (CurrentCRC32Value == NovatelEvaluateCRC32Value(currentHeader->headerLength + currentHeader->messageLength, data)) {
        unsigned int numberOfSatellites = *(reinterpret_cast<unsigned int *> (data + currentHeader->headerLength));

        for (unsigned int i = 0; i < numberOfSatellites; ++i) {
            size_t dataOffset = currentHeader->headerLength + 4 + sizeof (LogRawBodySATXYZ2) * i;
            LogRawBodySATXYZ2* bodySATXYZ2 = reinterpret_cast<LogRawBodySATXYZ2 *> (data + dataOffset);

            //write data to group
            satelliteGroup_->writeSATXYZ2(currentHeader, bodySATXYZ2);
        }
    } else {
        Logger::getInstance().error("bad crc but good header!!!");
    }
}

void
LogParserThread::parseRawISMDETOBS(unsigned char* data) {
    LogRawHeader* currentHeader = reinterpret_cast<LogRawHeader*> (data);
    unsigned int CurrentCRC32Value = *(reinterpret_cast<unsigned int *> (data + currentHeader->headerLength + currentHeader->messageLength));

    if (CurrentCRC32Value == NovatelEvaluateCRC32Value(currentHeader->headerLength + currentHeader->messageLength, data)) {
        unsigned int satelliteSystem = *(reinterpret_cast<unsigned int *> (data + currentHeader->headerLength));
        unsigned int numberOfObservation = *(reinterpret_cast<unsigned int *> (data + currentHeader->headerLength + 4));

        LogBodyISMDETOBS* bodyForWriting = new LogBodyISMDETOBS[numberOfObservation];
        for (unsigned int i = 0; i < numberOfObservation; ++i) {
            size_t dataOffset = currentHeader->headerLength + 8 + sizeof (LogRawBodyISMDETOBS) * i;
            LogRawBodyISMDETOBS* bodyISMDETOBS = reinterpret_cast<LogRawBodyISMDETOBS *> (data + dataOffset);

            bodyForWriting[i].satellitePRN = bodyISMDETOBS->satellitePRN;
            bodyForWriting[i].glonassFrequency = bodyISMDETOBS->glonassFrequency;
            bodyForWriting[i].satelliteSystem = satelliteSystem;
            bodyForWriting[i].signalType = bodyISMDETOBS->signalType;
            bodyForWriting[i].ismData[0].deltaMilliseconds = 0;
            bodyForWriting[i].ismData[0].accumulatedDopplerRange = bodyISMDETOBS->firstADR;
            bodyForWriting[i].ismData[0].power = (double) bodyISMDETOBS->firstPower / 1048576.0;
            for (size_t j = 1; j < 50; ++j) {
                bodyForWriting[i].ismData[j].deltaMilliseconds = j * 20;
                bodyForWriting[i].ismData[j].accumulatedDopplerRange = bodyISMDETOBS->firstADR + ((double) bodyISMDETOBS->ismData[j - 1].lDeltaAdr) / 1000.0;
                if (bodyISMDETOBS->ismData[j - 1].sign == 0)
                    bodyForWriting[i].ismData[j].power = (double) bodyISMDETOBS->firstPower * (2048.0 / (double) (bodyISMDETOBS->ismData[j - 1].lDeltaPower + 1.0));
                else
                    bodyForWriting[i].ismData[j].power = (double) bodyISMDETOBS->firstPower * ((double) bodyISMDETOBS->ismData[j - 1].lDeltaPower + 1.0) / 2048.0;

                bodyForWriting[i].ismData[j].power = bodyForWriting[i].ismData[j].power / 1048576.0;
            }
            //write data to group
            satelliteGroup_->writeISMDETOBS(currentHeader, &bodyForWriting[i]);
        }
        delete[] bodyForWriting;
    }
}

void
LogParserThread::parseRawISMREDOBS(unsigned char* data) {
    LogRawHeader* currentHeader = reinterpret_cast<LogRawHeader*> (data);
    unsigned int CurrentCRC32Value = *(reinterpret_cast<unsigned int *> (data + currentHeader->headerLength + currentHeader->messageLength));

    if (CurrentCRC32Value == NovatelEvaluateCRC32Value(currentHeader->headerLength + currentHeader->messageLength, data)) {
        unsigned int numberOfObservation = *(reinterpret_cast<unsigned int *> (data + currentHeader->headerLength));

        for (unsigned int i = 0; i < numberOfObservation; ++i) {
            size_t dataOffset = currentHeader->headerLength + 4 + sizeof (LogRawBodyISMREDOBS) * i;
            LogRawBodyISMREDOBS* bodyISMREDOBS = reinterpret_cast<LogRawBodyISMREDOBS *> (data + dataOffset);

            satelliteGroup_->writeISMREDOBS(currentHeader, bodyISMREDOBS);
        }
    } else {
        Logger::getInstance().error("bad crc but good header!!!");
    }
}

