#ifndef INTERRUPT_PRIORITIES_H_
#define INTERRUPT_PRIORITIES_H_

/**
 * @ingroup priority
 * @{
 */

#include "FreeRTOSConfig.h"

#define TASK_PRIORITY_SW_HEALTH             configMAX_PRIORITIES - 1    //!< software health task priority
#define TASK_PRIORITY_POWER_MGMT            configMAX_PRIORITIES - 4    //!< power management task priority
#define TASK_PRIORITY_ETB_CTRL              configMAX_PRIORITIES - 2    //!< etb control task priority

#define INTERRUPT_PRIORITY_USB		    configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 9 	//!< USB interrupt priority
#define INTERRUPT_PRIORITY_DMA1_STREAM5	configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 9	//!< LCD DMA2 Stream 0 interrupt priority
#define INTERRUPT_PRIORITY_DMA2D	    configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 8	//!< DMA2D interrupt priority
#define INTERRUPT_PRIORITY_LTDC		    configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 7	//!< LTDC interrupt priority
#define INTERRUPT_PRIORITY_BLUETOOTH    configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 6    //!< Bluetooth module interrupt priority
#define INTERRUPT_PRIORITY_DMA2_STREAM7	configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 5	//!< QSPI DMA2 Stream 7 interrupt priority
#define INTERRUPT_PRIORITY_QSPI		    configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 4	//!< QSPI interrupt priority
#define INTERRUPT_PRIORITY_USER_BUTTON  configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 3    //!< User button interrupt priority
#define INTERRUPT_PRIORITY_RTC_WKUP     configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2    //!< RTC wake up interrupt priority
#define INTERRUPT_PRIORITY_ADS		    configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1	//!< ADS interrupt priority

/**
 * @}
 */

#endif
