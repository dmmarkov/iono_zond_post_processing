/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   pashintsev_const.cpp
 * Author: dmitri
 * 
 * Created on 12 Июнь 2016 г., 16:10
 */

#include <cmath>

#include "pashintsev_const.hpp"

namespace pashintsev {
    const size_t FIRST_STEP_WINDOW_SIZE = 1500;
    const size_t SECOND_STEP_WINDOW_SIZE = 25;
    const size_t STANDARD_DEVIATION_COUNTER = 500;
    const double CALCULATION_FREQUENCY = 1.2 * pow(10.0, 9.0);
    const double CALCULATION_SIGNAL_TO_NOISE = pow(10.0, 1.75);
}

