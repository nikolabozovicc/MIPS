/*
 * homework.c
 *
 *  Created on: Jan 6, 2022
 *      Author: Marko Micovic
 */

#include "homework.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include <stdlib.h>
#include <string.h>

#include "gpio.h"

#include "driver_lcd.h"
#include "driver_uart.h"
#include "driver_motor.h"
#include "driver_temp.h"
#include "driver_keypad.h"

#define TRIGER_LVL 0.01
#define MS_IN_HOUR 3600000

uint32_t volatile temperatureLimit = 30;
uint32_t volatile passedMs = 0;
uint32_t volatile rainflow = 0;

TimerHandle_t RAIN_TimerHandle;
TimerHandle_t LED_TimerHandle;

static void homeworkTask(void *parameters) {
	//Initial write on terminal and LCD
	char kisa[7] = "Kisa: ";
	char temp[7] = "Temp: ";
	char thld[14] = "Thld:(??)=>30";

	for (uint8_t i = 0; i < strlen(temp); i++) {
		UART_AsyncTransmitCharacter(temp[i]);
	}

	LCD_CommandEnqueue(LCD_INSTRUCTION,
	LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | 0x00);

	for (uint8_t i = 0; i < strlen(kisa); i++) {
		LCD_CommandEnqueue(LCD_DATA, kisa[i]);
	}

	LCD_CommandEnqueue(LCD_INSTRUCTION,
	LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | 0x50);
	for (uint32_t i = 0; i < strlen(thld); i++) {
		LCD_CommandEnqueue(LCD_DATA, thld[i]);
	}

	uint32_t temperature;
	char rainString[4];
	char tempString[4];

	MotorState state = OFF, nextState = OFF;

	while (1) {

		temperature = TEMP_GetCurrentValue();
		itoa(temperature, tempString, 10);
		itoa(rainflow, rainString, 10);

		//Showing temperature in terminal
		for (uint8_t i = 0; i < strlen(tempString); i++) {
			UART_AsyncTransmitCharacter(tempString[i]);
		}

		//Showing rainflow on LCD
		LCD_CommandEnqueue(LCD_INSTRUCTION,
		LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | 0x06);

		for (uint8_t i = 0; i < strlen(rainString); i++) {
			LCD_CommandEnqueue(LCD_DATA, rainString[i]);
		}

		//Fan controll
		if (temperature < temperatureLimit) {
			nextState = OFF;
		} else {
			nextState = MID;
		}

		if (nextState > state) {
			MOTOR_SpeedIncrease();
			state = nextState;
		} else {
			if (nextState < state) {
				MOTOR_SpeedDecrease();
				state = nextState;
			}
		}

		//Keypad - controlling of temperatureLimit in driver_keypad.c
		if(numOfDigits == 1){
			LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | 0x56);
			LCD_CommandEnqueue(LCD_DATA, digits[0]);
		} else if(numOfDigits == 2){
			LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | 0x56);
			LCD_CommandEnqueue(LCD_DATA, '?');
			LCD_CommandEnqueue(LCD_INSTRUCTION, LCD_SET_DD_RAM_ADDRESS_INSTRUCTION | 0x5B);
			for(uint8_t i = 0; i < 2; i++){
				LCD_CommandEnqueue(LCD_DATA, digits[i]);
			}
			numOfDigits = 0;
		}


		//Suspend for 200ms
		vTaskDelay(pdMS_TO_TICKS(200));

		//Going backslash in terminal
		for (uint8_t i = 0; i < strlen(tempString); i++) {
			UART_AsyncTransmitCharacter('\b');
		}

	}

}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == GPIO_PIN_14) {
		rainflow = (TRIGER_LVL * MS_IN_HOUR) / passedMs + 1;
		passedMs = 0;
	} else if (GPIO_Pin == GPIO_PIN_7) {
		BaseType_t woken = pdFALSE;
		vTaskNotifyGiveFromISR(KEY_TaskHandle, &woken);
		portYIELD_FROM_ISR(woken);
	}
}

void rainCallback(TimerHandle_t t) {
	passedMs++;
}

void ledCallback(TimerHandle_t t) {
	uint32_t temperature = TEMP_GetCurrentValue();
	if (temperature < temperatureLimit) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_13, GPIO_PIN_RESET);
	} else {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_13);
	}
}

void homeworkInit() {
	UART_Init();
	LCD_Init();
	TEMP_Init();
	MOTOR_Init();
	KEY_Init();

	xTaskCreate(homeworkTask, "homework", 128, NULL, 5, NULL);

	RAIN_TimerHandle = xTimerCreate("rain", pdMS_TO_TICKS(1), pdTRUE, NULL,
			rainCallback);
	xTimerStart(RAIN_TimerHandle, portMAX_DELAY);

	LED_TimerHandle = xTimerCreate("led", pdMS_TO_TICKS(500), pdTRUE, NULL,
			ledCallback);
	xTimerStart(LED_TimerHandle, portMAX_DELAY);
}

