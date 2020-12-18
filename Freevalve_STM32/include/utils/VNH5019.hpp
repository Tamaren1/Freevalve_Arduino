/*
 * VNH5019.hpp
 *
 *  Created on: Aug 19, 2020
 *      Author: whitfijs
 */

#ifndef INCLUDE_UTILS_VNH5019_HPP_
#define INCLUDE_UTILS_VNH5019_HPP_

#include <motor.hpp>
#include <L298N.hpp>
#include "stm32f1xx_hal.h"

class VNH5019: public L298N {
public:
    static constexpr float VOLTS_PER_AMP = 0.140f;

    /**
     * Constructor
     */
    VNH5019();

    /**
     * Destructor
     */
    virtual ~VNH5019();

    float calculateCurrent(float voltage);

};

#endif /* INCLUDE_UTILS_VNH5019_HPP_ */
