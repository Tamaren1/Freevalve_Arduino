/*
 * FreeValveControl.cpp
 *
 *  Created on: Dec 16, 2020
 *      Author: whitfijs
 */

#include <FreeValveControl.hpp>
#include "Tasks.hpp"

FreeValveControl::FreeValveControl(const char * name, UBaseType_t prio,
        configSTACK_DEPTH_TYPE stackSize) : Task(Tasks::FVC_ID, name, prio, stackSize) {

}

FreeValveControl::~FreeValveControl() {
}

int FreeValveControl::setup() {
    return ReturnType::OK;
}

int FreeValveControl::loop() {
    /* any last setup here */

    /* task loop */
    for (;;) {
        /* wait for the sync from the trigger wheel interrupt*/
    }
}
