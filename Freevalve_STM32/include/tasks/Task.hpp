/*
 * Task.hpp
 *
 *  Created on: Dec 16, 2020
 *      Author: whitfijs
 */

#ifndef TASKS_TASK_HPP_
#define TASKS_TASK_HPP_

#include "FreeRTOS.h"
#include "return.hpp"
#include "task.h"

class Task {
public:
    Task(uint8_t id, const char * name,
            UBaseType_t priority, configSTACK_DEPTH_TYPE stackSize);
    virtual ~Task();

    virtual int initTask();

    virtual int setup() = 0;

    virtual int loop() = 0;

    static void runner(void * param);

    int waitForReady();

private:
    uint8_t mId;                        //!< Task ID
    TaskHandle_t mHandle;               //!< Task handle created when task is initialized
    const char * mName;                 //!< Name of task (for task creation)
    UBaseType_t mPrio;                  //!< Priority of task
    configSTACK_DEPTH_TYPE mStackSize;  //!< Size of task stack
};

#endif /* TASKS_TASK_HPP_ */
