#ifndef SRC_EXPERIMENTS_IONOZONDEXPERIMENT21_H_
#define SRC_EXPERIMENTS_IONOZONDEXPERIMENT21_H_

#include <atomic>

#include "ICalculation.hpp"
#include "ISatellite.hpp"
#include "GnssDataMeasurements2.hpp"
#include "GnssLogTime.hpp"

class IonoZondExperiment21 : public ICalculation {
public:
	explicit IonoZondExperiment21(ISatellite* satellite);
	virtual ~IonoZondExperiment21();

    virtual void mergeData() override;
	virtual void calculateData() override;

	virtual void stopThreads() override;

	virtual void closePlotData() override;
	virtual void forceCloseOutDataFiles(GnssLogTime& time) override;

	void pushS4Data(const unsigned short week, const unsigned int milliseconds, const double s4);
private:
	struct data {
	    unsigned short week;
	    unsigned int milliseconds;
	    double s4;
	};

	std::atomic <bool> isStopThreads_;

	ISatellite* satellite_;
	GnssDataMeasurements2<data> listOfValues_;
	size_t lengthOfPreviousValues_;
	size_t stepOfVector_;

	size_t sizeOfStringBuffer_;
	char *stringBuffer_;

	boost::mutex calcLock_;

    IonoZondExperiment21(const IonoZondExperiment21& orig) = delete;
    IonoZondExperiment21& operator=(const IonoZondExperiment21& ) = delete;
};

#endif /* SRC_EXPERIMENTS_IONOZONDEXPERIMENT21_H_ */
