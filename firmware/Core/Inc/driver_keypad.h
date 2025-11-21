/*
 * driver_keypad.h
 *
 *  Created on: 27.09.2025.
 *      Author: bn190327d
 */

#ifndef CORE_INC_DRIVER_KEYPAD_H_
#define CORE_INC_DRIVER_KEYPAD_H_

#include "FreeRTOS.h"
#include "task.h"

void KEY_Init();

extern TaskHandle_t KEY_TaskHandle;

extern volatile uint8_t numOfDigits;
extern char digits[2];


#endif /* CORE_INC_DRIVER_KEYPAD_H_ */
