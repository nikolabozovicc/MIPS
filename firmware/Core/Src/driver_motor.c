/*
 * driver_motor.c
 *
 *  Created on: Jan 6, 2022
 *      Author: Marko Micovic
 */

#include "tim.h"

#define ARR 9
#define STEP (ARR + 1) / 2

uint32_t volatile currentCCR = 0;

void MOTOR_SpeedIncrease() {
	if (currentCCR < (ARR + 1)) {
		currentCCR += STEP;
		htim3.Instance->CCR1 = currentCCR;
	}
}

void MOTOR_SpeedDecrease() {
	if (currentCCR > 0) {
		currentCCR -= STEP;
		htim3.Instance->CCR1 = currentCCR;
	}
}

void MOTOR_Init() {
	HAL_TIM_OC_Start(&htim3, TIM_CHANNEL_1);
}
