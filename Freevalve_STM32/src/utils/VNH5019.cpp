/*
 * VNH5019.cpp
 *
 *  Created on: Aug 19, 2020
 *      Author: whitfijs
 */

#include <VNH5019.hpp>
#include <math.h>

VNH5019::VNH5019() {

}

VNH5019::~VNH5019() {
}


float VNH5019::calculateCurrent(float voltage) {
    return voltage / VOLTS_PER_AMP;
}
