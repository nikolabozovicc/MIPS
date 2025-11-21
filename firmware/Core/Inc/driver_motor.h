/*
 * driver_motor.h
 *
 *  Created on: Jan 6, 2022
 *      Author: Marko Micovic
 */

#ifndef CORE_INC_DRIVER_MOTOR_H_
#define CORE_INC_DRIVER_MOTOR_H_

extern void MOTOR_SpeedIncrease();
extern void MOTOR_SpeedDecrease();

extern void MOTOR_Init();

typedef enum{
	OFF,
	MID,
	HIGH
} MotorState;

#endif /* CORE_INC_DRIVER_MOTOR_H_ */
