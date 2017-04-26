/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   IonoZondExperiment20.hpp
 * Author: dmitri
 *
 * Created on 22 апреля 2017 г., 16:31
 */

#ifndef IONOZONDEXPERIMENT20_HPP
#define IONOZONDEXPERIMENT20_HPP

#include <string>
#include <vector>

#include "ISatellite.hpp"

class IonoZondExperiment20 {
public:
    explicit IonoZondExperiment20(ISatellite *satellite);
    virtual ~IonoZondExperiment20();

    void pushTec(const double tec);
private:
    double minTecValue_;
    double maxTecValue_;
    size_t intervalCount_;
    double stepOfCounter_;

    std::vector<size_t> counters_;

    ISatellite *satellite_;

    std::ofstream outData_;

    IonoZondExperiment20(const IonoZondExperiment20& orig) = delete;
    IonoZondExperiment20& operator=(const IonoZondExperiment20& ) = delete;
};

#endif /* IONOZONDEXPERIMENT20_HPP */

