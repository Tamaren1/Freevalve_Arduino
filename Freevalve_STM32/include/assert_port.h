/*
 * assert.h
 *
 *  Created on: Feb 27, 2019
 *      Author: jwhitfield
 */

#ifndef SRC_ASSERT_PORT_H_
#define SRC_ASSERT_PORT_H_

#ifdef FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#else
#ifndef configASSERT
#define configASSERT( x ) if( ( x ) == 0 ) { assert_failed(__FILE__, __LINE__); }
#endif

#ifndef configMessageASSERT
#define configMessageASSERT(condition, message) if ((condition) == 0) { assert_failed((char *)message, __LINE__); }
#endif

#endif



#endif /* ASSERT_PORT_H_ */
