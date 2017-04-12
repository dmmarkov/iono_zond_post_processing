/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   GnssDataMeasurements2.hpp
 * Author: dmitri
 *
 * Created on 5 марта 2017 г., 19:46
 */

#ifndef GNSSDATAMEASUREMENTS2_HPP
#define GNSSDATAMEASUREMENTS2_HPP

#include "project_defines.hpp"

#include <string>
#include <atomic>

#include "struct_headers.hpp"
#include "Logger.hpp"
#include "common_functions.hpp"

template <class T>
class GnssDataMeasurements2 {
public:

    class GnssIterator : public std::iterator<std::forward_iterator_tag, T>
    {
        T* position_;
        T* realBegin_;
        T* realEnd_;
        T* bufferBegin_;
        T* bufferEnd_;
    public:
        explicit GnssIterator(T* x, T* realBufferBegin, T* realBufferEnd, T* bufferBegin, T* bufferEnd) {
            position_ = x;
            realBegin_ = realBufferBegin;
            realEnd_ = realBufferEnd;

            bufferBegin_ = bufferBegin;
            bufferEnd_ = bufferEnd;
        }

        GnssIterator(const GnssIterator& mit){
            position_ = mit.position_;
            realBegin_ = mit.realBegin_;
            realEnd_ = mit.realEnd_;
            bufferBegin_ = mit.bufferBegin_;
            bufferEnd_ = mit.bufferEnd_;
        }

        GnssIterator& operator++() {
            position_++;

            if (position_ >= realEnd_ && bufferEnd_ < realEnd_) {
                position_ = realBegin_;
            } else {
                if (position_ > realEnd_ || position_ < realBegin_) {
                    Logger::getInstance().error("GnssIterator out of range");
                }
            }

            return *this;
        }

        bool operator==(const GnssIterator& rhs) {
            return position_==rhs.position_;
        }

        bool operator!=(const GnssIterator& rhs) {
            return position_!=rhs.position_;
        }

        T& operator*() {
            return *position_;
        }

        T* getPosition() {
            return position_;
        }

        T* getRealBegin() {
            return realBegin_;
        }

        T* getRealEnd() {
            return realEnd_;
        }

        T* getBufferBegin() {
            return bufferBegin_;
        }

        T* getBufferEnd() {
            return bufferEnd_;
        }
    };

    explicit GnssDataMeasurements2(const size_t sizeOfRing = 100000, const bool useHugePages = true) : isDestroying_(false), sizeOfReadBuffer_(0) {
        sizeOfRing_ = sizeOfRing;
        memorySizeOfRing_ = sizeOfRing_ * sizeof(T);
        useHugePages_ = useHugePages;

        if (useHugePages_) {
            buffer_ = (T*)allocateHugePages(&memorySizeOfRing_);
            sizeOfRing_ = memorySizeOfRing_ / sizeof(T);
        } else {
            buffer_ = (T*)malloc(memorySizeOfRing_);
        }

        bufferEnd_ = buffer_ + sizeOfRing_ - 1;
        sizeOfRing_ -= 1;

        beginWrite_ = buffer_;
        endWrite_ = buffer_;

        beginRead_ = buffer_;
        endRead_ = buffer_;
        pushedElements_ = 0;
    }

    virtual
    ~GnssDataMeasurements2() {
        isDestroying_ = true;

        sizeOfReadBuffer_ = 0;
        boost::unique_lock<boost::mutex> lockPush(mutexPushData_);
        boost::unique_lock<boost::mutex> lockRead(mutexReadData_);

        if (useHugePages_) {
            freeHugePages(buffer_, memorySizeOfRing_);
        } else {
            free(buffer_);
        }
    }

    void
    push_back(T& newData) {
        if (!isDestroying_) {
            boost::unique_lock<boost::mutex> lock(mutexPushData_);

            if (pushedElements_ == sizeOfRing_) {
                Logger::getInstance().error("Ring is full. Stop processing");
                exit(EXIT_FAILURE);
            }

            if (endWrite_ >= bufferEnd_) {
                endWrite_ = buffer_;
            }

            *endWrite_ = newData;
            ++endWrite_;
            ++pushedElements_;
        }
    }

    void
    lock() {
        if (!isDestroying_) {
            mutexReadData_.lock();

            //move data from push writeData_ to readData_
            //lock data for moving
            boost::unique_lock<boost::mutex> lock(mutexPushData_);
            try {
                if (pushedElements_ > 0) {
                    if ((pushedElements_ + sizeOfReadBuffer_) > sizeOfRing_) {
                        Logger::getInstance().error("Ring is full. Stop processing");
                        exit(EXIT_FAILURE);
                    }

                    if (beginRead_ == endRead_) {
                        beginRead_ = beginWrite_;
                    }

                    endRead_ = endWrite_;
                    beginWrite_ = endWrite_;

                    sizeOfReadBuffer_ += pushedElements_;

#ifdef DEBUG
//                    Logger::getInstance().info("GnssDataMeasurements2::lock sizeOfReadBuffer_ = " + intToString(sizeOfReadBuffer_));
#endif

                    pushedElements_ = 0;
                }
            }
            catch (std::exception& ex) {
                Logger::getInstance().error(std::string("error: GnssDataMeasurements::lock(): ") + ex.what());
            }
        }
    }

    void
    unlock() {
        mutexReadData_.unlock();
    }

    GnssIterator
    begin() {
        return GnssIterator(beginRead_, buffer_, bufferEnd_, beginRead_, endRead_);
    }

    GnssIterator
    end() {
        return GnssIterator(endRead_, buffer_, bufferEnd_, endRead_, endRead_);
    }

    void
    erase(GnssIterator first, GnssIterator last) {
        if (!isDestroying_) {
            size_t countOfErased = 0;

            if (last.getPosition() > first.getPosition()) {
                countOfErased = last.getPosition() - first.getPosition();
            } else {
                countOfErased = (bufferEnd_ - first.getPosition()) + (last.getPosition() - buffer_);
            }

            if (((int)sizeOfReadBuffer_ - (int)countOfErased) < 0) {
                sizeOfReadBuffer_ = 0;
                Logger::getInstance().error("Number of element in read will be less then zero! " + intToString(sizeOfReadBuffer_) + " " + intToString(countOfErased));
            } else {
                sizeOfReadBuffer_ -= countOfErased;
            }

#ifdef DEBUG
//            Logger::getInstance().info("GnssDataMeasurements2::erase sizeOfReadBuffer_ = " + intToString(sizeOfReadBuffer_));
#endif

            beginRead_ += countOfErased;
            if (beginRead_ > bufferEnd_) {
                beginRead_ = buffer_ + (beginRead_ - bufferEnd_);
            }
        }
    }

    size_t
    size() {
        if (!isDestroying_) {
            return sizeOfReadBuffer_;
        } else {
            return 0;
        }
    }

    size_t
    capacity() {
        return sizeOfRing_;
    }

    size_t
    percentOfFull() {
        return (sizeOfReadBuffer_ * 100) / sizeOfRing_;
    }

    bool
    is_empty() {
        if (!isDestroying_) {
            return sizeOfReadBuffer_ == 0;
        } else {
            return true;
        }
    }

private:
    size_t sizeOfRing_;
    size_t memorySizeOfRing_;
    bool useHugePages_;

    T* buffer_;
    T* bufferEnd_; //pointer to the last+1 element of buffer; //like container.end()

    T* beginWrite_; //first element of write part of buffer;
    T* endWrite_; //last+1 of write part of buffer; //like container.end()

    T* beginRead_; //first element of read part of buffer;
    T* endRead_; //last+1 of write part of buffer; //like container.end()

    boost::mutex mutexPushData_;
    boost::mutex mutexReadData_;

    size_t pushedElements_;

    std::atomic<bool> isDestroying_;
    std::atomic<size_t> sizeOfReadBuffer_;

    GnssDataMeasurements2(const GnssDataMeasurements2& orig) = delete;
    GnssDataMeasurements2& operator=(const GnssDataMeasurements2& ) = delete;
};

#endif /* GNSSDATAMEASUREMENTS2_HPP */

