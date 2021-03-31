/*
 * freertos.c
 *
 *  Created on: 25 oct. 2019
 *      Author: j.imbert
 */


 //##############################################################################
 //                        INCLUDES
 //##############################################################################
#include "appCore.h"
#include "printf.h"
#include "taskMessage.h"
#include "fileManager.h"
#include "main.h"
//----------------------------------------------------------------------------
//#include "stm32l4xx_hal.h"
//----------------------------------------------------------------------------
#include "FreeRTOS.h"
#include "queue.h"
//----------------------------------------------------------------------------
#include <string.h>
//##############################################################################
//                 EXTERNAL variables, functions
//##############################################################################


//##############################################################################
//                 PRIVATE SYMBOLIC CONSTANTS and MACROS
//##############################################################################




//Temps max pour qu'une tache s'initialise. Si dépassement => restart
#define MAXIMUM_CONFIG_TASK_TIME (5*60*1000)

//----------------------------------------------------------------------------


//##############################################################################
//               PRIVATE TYPES, STRUCTURES, UNIONS and ENUMS
//##############################################################################
//----------------------------------------------------------------------------

//##############################################################################
//                    PRIVATE VARIABLES DEFINITION
//##############################################################################
//----------------------------------------------------------------------------
TaskHandle_t	SuperviserTaskHandle;
TaskHandle_t	FileManagerTaskHandle;

//----------------------------------------------------------------------------
QueueHandle_t	FileManagerQueueHandle;
QueueHandle_t	SuperviserQueueHandle;
//##############################################################################
//                      PRIVATE FUNCTION PROTOTYPE
//##############################################################################
void SuperviserTask(void * argument);
void WaitEndInitTask(void);

//##############################################################################
//                       PUBLIC FUNCTION CODE
//##############################################################################
void MX_FREERTOS_Init(void) {
	
	//Création des queues globales
	if ((FileManagerQueueHandle = xQueueCreate(3, sizeof(TaskMessage_TypeDef))) == NULL) {
		_Error_Handler(__FILE__, __LINE__);
	}

	if ((SuperviserQueueHandle = xQueueCreate(3, sizeof(TaskMessage_TypeDef))) == NULL) {
		_Error_Handler(__FILE__, __LINE__);
	}


	//Création des tâches
	if (xTaskCreate(SuperviserTask, "superviser", 1024, NULL, configMAX_PRIORITIES, &SuperviserTaskHandle) != pdPASS) {
		_Error_Handler(__FILE__, __LINE__);
	}


	if (xTaskCreate(FileManagerTask, "fileManager", 2048, NULL, configMAX_PRIORITIES-2, &FileManagerTaskHandle) != pdPASS) {
		_Error_Handler(__FILE__, __LINE__);
	}
}

void SuperviserTask(void * argument) {
	BaseType_t result;
	TaskMessage_TypeDef  msg;
	FileManagerMeasures_TypeDef * tData;
	//Fake data
	uint8_t buff[] = {"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed ut enim leo. Duis aliquet quam accumsan nisl venenatis, id elementum risus luctus. Duis in velit mattis, porta risus non, scelerisque mauris. In tempus justo sit amet porta bibendum. In hac habitasse platea dictumst. Sed vel dolor purus. Integer facilisis non nisl sed maximus. Integer quis sapien purus. Donec ultricies auctor erat, at aliquet magna accumsan vitae. Nam sollicitudin at nulla vitae vestibulum. Maecenas sollicitudin lacus a mi nullam.Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed ut enim leo. Duis aliquet quam accumsan nisl venenatis, id elementum risus luctus. Duis in velit mattis, porta risus non, scelerisque mauris. In tempus justo sit amet porta bibendum. In hac habitasse platea dictumst. Sed vel dolor purus. Integer facilisis non nisl sed maximus. Integer quis sapien purus. Donec ultricies auctor erat, at aliquet magna accumsan vitae. Nam sollicitudin at nulla vitae vestibulum. Maecenas sollicitudin lacus a mi nullam.Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed ut enim leo. Duis aliquet quam accumsan nisl venenatis, id elementum risus luctus. Duis in velit mattis, porta risus non, scelerisque mauris. In tempus justo sit amet porta bibendum. In hac habitasse platea dictumst. Sed vel dolor purus. Integer facilisis non nisl sed maximus. Integer quis sapien purus. Donec ultricies auctor erat, at aliquet magna accumsan vitae. Nam sollicitudin at nulla vitae vestibulum. Maecenas sollicitudin lacus a mi "};

	xTaskNotifyGive(FileManagerTaskHandle);
	WaitEndInitTask();
	DEBUG_LOG("FileManager init...OK\r\n");






	while (1) {

		tData = pvPortMalloc(sizeof(FileManagerMeasures_TypeDef));
		if(tData == NULL){
			_Error_Handler(__FILE__, __LINE__);
			while(1);
		}

		tData->size = strlen(buff);
		tData->data = pvPortMalloc(tData->size);
		if(tData->data == NULL){
			_Error_Handler(__FILE__, __LINE__);
			while(1);
		}

		memcpy(tData->data, buff, tData->size);


		TaskMessageInit(&msg, &SuperviserQueueHandle, MESSAGE_CMD_SAVE_DATA, tData, sizeof(FileManagerMeasures_TypeDef));
		result = xQueueSend(FileManagerQueueHandle, &msg, 0);
		if (result != pdPASS) {
			_Error_Handler(__FILE__, __LINE__);
		}
		vTaskDelay(pdMS_TO_TICKS(1000));
	}

}

void WaitEndInitTask(void){
	 if(ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(MAXIMUM_CONFIG_TASK_TIME)) == 0){
		ResetSystem();
	}
}

void ResetSystem(void){
	NVIC_SystemReset();
}



