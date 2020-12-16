/*
 * Tasks.hpp
 *
 *  Created on: Dec 16, 2020
 *      Author: whitfijs
 */

#ifndef TASKS_TASKS_HPP_
#define TASKS_TASKS_HPP_

namespace Tasks {
    enum {
        FVC_ID, //!< Freevalve control task ID

        NUM_IDS,
    } TaskId;

    int initTasks();
};

#endif /* TASKS_TASKS_HPP_ */