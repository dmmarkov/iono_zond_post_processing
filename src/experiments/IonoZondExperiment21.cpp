#include "IonoZondExperiment21.h"

#include "ThreadManager.hpp"
#include "Logger.hpp"

IonoZondExperiment21::IonoZondExperiment21(ISatellite* satellite) :
		isStopThreads_(false), satellite_(satellite), lengthOfPreviousValues_(
				200), stepOfVector_(3000) {
	sizeOfStringBuffer_ = 131072;
	stringBuffer_ = new char[sizeOfStringBuffer_];

	ThreadManager::getInstance().addNewCalculation(this);

	Logger::getInstance().info("IonoZondExperiment21::IonoZondExperiment21 created. Satellite " + satellite_->getSatelliteName());
}

IonoZondExperiment21::~IonoZondExperiment21() {
	Logger::getInstance().info("IonoZondExperiment21::IonoZondExperiment21 destroyed. Satellite " + satellite_->getSatelliteName());
}

void IonoZondExperiment21::mergeData() {
	//dummy method
}

void IonoZondExperiment21::calculateData() {
	boost::unique_lock<boost::mutex> lockMethod(calcLock_, boost::try_to_lock);

	if (calcLock_ && !isStopThreads_) {
		listOfValues_.lock();

		if (listOfValues_.size() > (stepOfVector_ + lengthOfPreviousValues_)) {
			auto strPos = stringBuffer_;
//            auto strEnd = stringBuffer_ + sizeOfStringBuffer_; //used for protection, but not used now

#if 0
			std::strcpy(strPos, "satellite_system_id:");
			strPos += 20;

			u32toa_sse2(satelliteSystem_, strPos);
			while (*strPos) {
				++strPos;
			}

#endif

#if 0
			std::strcpy(strPos, " frequency:");
			strPos += 11;
#else
#if 0
			*strPos = ';';
			++strPos;
#endif
#endif

#if 0
			u32toa_sse2((size_t) frequency_, strPos);
			while (*strPos) {
				++strPos;
			}
#endif

			size_t counter = 1;
			auto it = listOfValues_.begin();
			auto end = listOfValues_.end();
			auto erased = s4Data_.begin();
			for (; it != end; ++it) {
#if 1
				std::strcpy(strPos, " s4_");
				strPos += 4;

				u32toa_sse2(counter, strPos);
				while (*strPos) {
					++strPos;
				}

				*strPos = ':';
				strPos += 1;
#else
				*strPos = ';';
				++strPos;
#endif

				dtoa_milo((*it).s4, strPos);
				while (*strPos) {
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

	}

	listOfValues_.unlock();
}
}

void IonoZondExperiment21::stopThreads() {
isStopThreads_ = true;

ThreadManager::getInstance().releaseCalculation(this);
}

void IonoZondExperiment21::closePlotData() {
//dummy method
}

void IonoZondExperiment21::forceCloseOutDataFiles(GnssLogTime& time) {
// dummy method
}

void IonoZondExperiment21::pushS4Data(const unsigned short week,
	const unsigned int milliseconds, const double s4) {
if (!isStopThreads_) {
	IonoZondExperiment21::data t = { .week = week, .milliseconds = milliseconds,
			.s4 = s4 };

	listOfValues_.push_back(t);
}
}
