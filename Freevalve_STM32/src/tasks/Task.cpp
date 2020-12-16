/*
 * Task.cpp
 *
 *  Created on: Dec 16, 2020
 *      Author: whitfijs
 */

#include "Task.hpp"

Task::Task(uint8_t id, const char * name,
        UBaseType_t priority, configSTACK_DEPTH_TYPE stackSize) :
        mId(id), mHandle(nullptr), mName(name), mPrio(priority),
        mStackSize(stackSize) {
}

Task::~Task() {
}

int Task::initTask() {
    /* Create the task */
    if (xTaskCreate(&runner, mName, mStackSize, this, mPrio, &mHandle) == pdTRUE) {
        /* task was created successfully */
        return ReturnType::OK;
    }

    /* task failed to start */
    return ReturnType::ERROR;
}

void Task::runner(void * param) {
    /* Cast param to task */
    Task * task = (Task *) param;

    /* setup the task */
    if (task->setup() != ReturnType::OK) {
        goto runner_failure;
    }

    if (task->waitForReady() != ReturnType::OK) {
        goto runner_failure;
    }

    /* Now we can run the task */
    task->loop();

    /* if we get here we messed up */
    vTaskSuspend(task->mHandle);

runner_failure:
    /* it's gone wrong -- TODO: signal task startup failure */
    for (;;) {
        /* hang here */
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

int Task::waitForReady() {
    /* TODO: wait for other tasks to become ready to run as well -- will need external task monitor for this */
    /* Event groups are a simple solution for this or message queues  */

    return ReturnType::OK;
}
