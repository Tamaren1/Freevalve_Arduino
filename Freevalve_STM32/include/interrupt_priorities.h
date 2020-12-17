#ifndef INTERRUPT_PRIORITIES_H_
#define INTERRUPT_PRIORITIES_H_

/**
 * @ingroup priority
 * @{
 */

#include "FreeRTOSConfig.h"

#define TASK_PRIORITY_TASK_MONITOR            configMAX_PRIORITIES - 1    //!< software health task priority
#define TASK_PRIORITY_FREEVALVE_TASK          configMAX_PRIORITIES - 2    //!< etb control task priority

#define INTERRUPT_PRIORITY_HALL_SENSOR_EXTI  configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 3    //!< Hall sensor exti priority
#define INTERRUPT_PRIORITY_RTC_WKUP          configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2    //!< RTC wake up interrupt priority

/**
 * @}
 */

#endif
