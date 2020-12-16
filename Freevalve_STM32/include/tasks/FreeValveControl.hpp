/*
 * FreeValveControl.hpp
 *
 *  Created on: Dec 16, 2020
 *      Author: whitfijs
 */

#ifndef INCLUDE_TASKS_FREEVALVECONTROL_HPP_
#define INCLUDE_TASKS_FREEVALVECONTROL_HPP_

#include <Task.hpp>


class FreeValveControl: public Task {
public:
    FreeValveControl(const char * name, UBaseType_t priority,
            configSTACK_DEPTH_TYPE stackSize);
    virtual ~FreeValveControl();

    int setup();
    int loop();
};

#endif /* INCLUDE_TASKS_FREEVALVECONTROL_HPP_ */
