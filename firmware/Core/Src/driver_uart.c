/*
 * driver_uart.c
 *
 *  Created on: Jan 6, 2022
 *      Author: Marko Micovic
 */

#include "driver_uart.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include <string.h>

#include "usart.h"

// TRANSMIT
// -----------------------------------------------------------------------------

static TaskHandle_t UART_TransmitTaskHandle;
static QueueHandle_t UART_TransmitQueueHandle;
static SemaphoreHandle_t UART_TransmitMutexHandle;

static void UART_TransmitTask(void *parameters) {
	uint8_t buffer;
	while (1) {
		xQueueReceive(UART_TransmitQueueHandle, &buffer, portMAX_DELAY);
		HAL_UART_Transmit_IT(&huart1, &buffer, sizeof(uint8_t));
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
	}

}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {

	if (huart->Instance == huart1.Instance) {
		BaseType_t woken = pdFALSE;
		vTaskNotifyGiveFromISR(UART_TransmitTaskHandle, &woken);
		portYIELD_FROM_ISR(woken);
	}
}

// RECEIVE
// -----------------------------------------------------------------------------

//static TaskHandle_t UART_ReceiveTaskHandle;
//static QueueHandle_t UART_ReceiveQueueHandle;
//static SemaphoreHandle_t UART_ReceiveMutexHandle;
//
//static void UART_ReceiveTask(void *parameters)
//{
//
//}
//
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//
//}

// GENERAL
// -----------------------------------------------------------------------------

void UART_Init() {
	xTaskCreate(UART_TransmitTask, "transmit", 128, NULL, 4,
			&UART_TransmitTaskHandle);
	UART_TransmitQueueHandle = xQueueCreate(64, sizeof(uint8_t));
	UART_TransmitMutexHandle = xSemaphoreCreateMutex();
}

// TRANSMIT UTIL
// -----------------------------------------------------------------------------

void UART_AsyncTransmitCharacter(char character) {
	xSemaphoreTake(UART_TransmitMutexHandle, portMAX_DELAY);
	xQueueSend(UART_TransmitQueueHandle, &character, portMAX_DELAY);
	xSemaphoreGive(UART_TransmitMutexHandle);
}

//void UART_AsyncTransmitString(char const *string)
//{
//
//}
//
//void UART_AsyncTransmitDecimal(uint32_t decimal)
//{
//
//}

// RECEIVE UTIL
// -----------------------------------------------------------------------------

//char UART_BlockReceiveCharacter()
//{
//
//}
//
//char* UART_BlockReceiveString()
//{
//
//}
//
//uint32_t UART_BlockReceiveDecimal()
//{
//
//}
