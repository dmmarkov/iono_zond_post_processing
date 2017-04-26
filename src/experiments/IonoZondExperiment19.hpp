/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   IonoZondExperiment19.hpp
 * Author: dmitri
 *
 * Created on 22 апреля 2017 г., 16:31
 */

#ifndef IONOZONDEXPERIMENT19_HPP
#define IONOZONDEXPERIMENT19_HPP

#include <string>
#include <vector>

#include "ISatellite.hpp"

class IonoZondExperiment19 {
public:
    explicit IonoZondExperiment19(ISatellite *satellite, std::string channelName);
    virtual ~IonoZondExperiment19();

    void pushSigmaPhi(const double sigmaPhi);
private:
    double minSigmaPhiValue_;
    double maxSigmaPhiValue_;
    size_t intervalCount_;
    double stepOfCounter_;

    std::vector<size_t> counters_;

    ISatellite *satellite_;
    std::string channelName_;

    IonoZondExperiment19(const IonoZondExperiment19& orig) = delete;
    IonoZondExperiment19& operator=(const IonoZondExperiment19& ) = delete;
};

#endif /* IONOZONDEXPERIMENT19_HPP */

