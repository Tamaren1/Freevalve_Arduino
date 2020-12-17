/*
 * FreeValveControl.hpp
 *
 *  Created on: Dec 16, 2020
 *      Author: whitfijs
 */

#ifndef INCLUDE_TASKS_FREEVALVECONTROL_HPP_
#define INCLUDE_TASKS_FREEVALVECONTROL_HPP_

#include <Task.hpp>
#include "ExternalInterrupt.hpp"


class FreeValveControl: public Task {
public:
    static constexpr int HALL_GPIO_PIN = GPIO_PIN_2;
    static constexpr int HALL_EXTI_LINE = 2;
    static constexpr int EXHAUST_GPIO_PIN = GPIO_PIN_6;
    static constexpr int INTAKE_GPIO_PIN = GPIO_PIN_7;


    static constexpr int NUM_TEETH = 60;
    static constexpr int NUM_MISSING_TEETH = 2;
    static constexpr int NUM_TRIGGER_EDGES = NUM_TEETH;
    static constexpr int DEG_PER_MAGNET = 360 / NUM_TEETH; // Number of degrees for per magnet.
    static constexpr int TRIGGERS_PER_CYCLE = NUM_TRIGGER_EDGES * 2;     // Number of interrupts in the full cycle.
    static constexpr int TRIGGERS_PER_ROTATION = NUM_TRIGGER_EDGES; // Number of interrupts in a half cycle.

    static constexpr int INTAKE_OPEN_DEG   = 6;
    static constexpr int INTAKE_CLOSED_DEG = 180;
    static constexpr int EXHAUST_OPEN_DEG  = 540;
    static constexpr int EXHAUST_CLOSED_DEG = 714;

    typedef struct TriggerParams_t{
        uint32_t triggerCount;
        bool cycle;
        uint32_t timeGap;
        uint32_t lastTimeGap;
        uint32_t lastTriggerTime;

        TriggerParams_t() : triggerCount(0), cycle(0),
                timeGap(0), lastTimeGap(0), lastTriggerTime(0) {};
    } TriggerParams_t;

    FreeValveControl(const char * name, UBaseType_t priority,
            configSTACK_DEPTH_TYPE stackSize);
    virtual ~FreeValveControl();

    int setup();
    int loop();

protected:
    volatile TriggerParams_t mTriggerParams;
    uint8_t mIntakeMap[TRIGGERS_PER_ROTATION];
    uint8_t mExhaustMap[TRIGGERS_PER_ROTATION];

private:
    int initGpio();

    int calcValveMaps(uint8_t * intake, uint8_t * exhaust, uint16_t len);

    static void onHallDetected(void * param);

};

#endif /* INCLUDE_TASKS_FREEVALVECONTROL_HPP_ */
