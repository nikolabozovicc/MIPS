/*
 * driver_keypad.c
 *
 *  Created on: 27.09.2025.
 *      Author: bn190327d
 */

#include "driver_keypad.h"
#include "homework.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "gpio.h"

#include "driver_lcd.h"

#include <stdlib.h>

TaskHandle_t KEY_TaskHandle;
TimerHandle_t KEY_TimerHandle;

char keys[4][3] = { { '1', '2', '3' }, { '4', '5', '6' }, { '7', '8', '9' }, {
		'*', '0', '#' } };

volatile uint8_t keyReleased = 1;
volatile uint8_t numOfDigits = 0;
char digits[2];

void KEY_Task(void *parameters) {

	while (1) {
		vTaskDelay(pdMS_TO_TICKS(100));
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		if(keyReleased){
			for(uint8_t row = 0; row < 4; row++){
				GPIOB->ODR = 0x01 << row;

				uint32_t idr = (GPIOB->IDR >> 4) & 0x07;
				for(uint8_t col = 0; col < 3; col++){
					if((0x01 << col) & idr){
						keyReleased = 0;
						xTimerStart(KEY_TimerHandle, portMAX_DELAY);
						char key = keys[row][col];

						if(numOfDigits == 0){
							digits[numOfDigits] = key;
							numOfDigits++;
						} else if(numOfDigits == 1){
							digits[numOfDigits] = key;
							numOfDigits++;
							temperatureLimit = atoi(digits);
						}

					}
				}
			}
			GPIOB->ODR = 0x0F;
		}
	}

}

void keyCallback(TimerHandle_t t){
	if(!keyReleased){
		if(((GPIOB->IDR >> 4) & 0x07) == 0){
			keyReleased = 1;
		} else{
			xTimerStart(KEY_TimerHandle, portMAX_DELAY);
		}
	}
}

void KEY_Init() {
	GPIOB->ODR = 0x0F;
	xTaskCreate(KEY_Task, "keypad", 128, NULL, 10, &KEY_TaskHandle);
	KEY_TimerHandle = xTimerCreate("keypad timer", pdMS_TO_TICKS(10), pdFALSE, NULL, keyCallback);
}
