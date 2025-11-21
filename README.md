# STM32 Full Embedded Project

This project implements a complete embedded system using an STM32F103R6 microcontroller to control a fan based on temperature readings, measure rainfall intensity, handle user input via keypad, and display data on an LCD and virtual terminal.
All development was done using STM32CubeMX + FreeRTOS, with a full simulation in Proteus.

## Features
- ### Temperature Measurement
- LM35 sensor connected to ADC1, Channel 7
- Values displayed on USART1 Virtual Terminal:
  `Temp: <t>`

- ### Rainfall Intensity Measurement
- Rain gauge connected through 74HC221 monostable multivibrator
- 1 ms pulse for every bucket tip
- Displayed on LCD
  `Kisa: <k>`

- ### Keypad Threshold Input
- User sets temp_granica (temperature threshold for fan activation)
- Two-digit input via keypad
- Displayed on LCD (Line 4):
  `Thld:(??)=><value>`

- ### Bi-color LED
- Green when `temperature < threshold`
- Blinking red (0.5s ON / 0.5s OFF) when `temperature >= threshold`

- ### Fan Speed Control
- DC Motor controlled by PWM – TIM3, Channel 1
- Fan speed: 0% if `temperature < threshold`, 50% if `temperature ≥ threshold`


## Project Schema (Proteus)

<img width="1205" height="830" alt="image" src="https://github.com/user-attachments/assets/ba8fc1a7-7a57-4107-b428-bab0590d2bb0" />

