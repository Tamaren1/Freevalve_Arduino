/*
 * Tasks.cpp
 *
 *  Created on: Dec 16, 2020
 *      Author: whitfijs
 */

#include "Tasks.hpp"
#include "Task.hpp"
#include "interrupt_priorities.h"

#include "FreeValveControl.hpp"

static FreeValveControl FVCtrlTask("EtbCtrl", TASK_PRIORITY_FREEVALVE_TASK, configMINIMAL_STACK_SIZE*4);     //!< FV Control Task declaration;

int Tasks::initTasks() {
    int ret = FVCtrlTask.initTask();

    return ret;
}
