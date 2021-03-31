/*
 * BspModem.c
 *
 *  Created on	: 02 oct. 2019
 *  Author		: j.imbert
 */

//##############################################################################
//                        INCLUDES
//##############################################################################
#include "stm32l4xx_hal.h"
#include "stm32l4xx_it.h"
#include "cmsis_os.h"

//##############################################################################
//                 EXTERNAL variables, functions
//##############################################################################
extern void _Error_Handler(char *, int);

//##############################################################################
//                 PRIVATE SYMBOLIC CONSTANTS and MACROS
//##############################################################################

//##############################################################################
//               PRIVATE TYPES, STRUCTURES, UNIONS and ENUMS
//##############################################################################

//##############################################################################
//                    PRIVATE VARIABLES DEFINITION
//##############################################################################

//##############################################################################
//                      PRIVATE FUNCTION PROTOTYPE
//##############################################################################

//##############################################################################
//                      FUNCTIONS CODE
//##############################################################################
void HAL_MspInit(void) {

	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();

	HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

	HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
	HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
	HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
	HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
	HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
	HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
	HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

void HAL_SD_MspInit(SD_HandleTypeDef* hsd) {

	GPIO_InitTypeDef GPIO_InitStruct;
	if (hsd->Instance == SDMMC1) {

		__HAL_RCC_SDMMC1_CLK_ENABLE();

		GPIO_InitStruct.Pin = SD_D0_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF12_SDMMC1;
		HAL_GPIO_Init(SD_D0_Port, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = SD_D1_Pin;
		HAL_GPIO_Init(SD_D1_Port, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = SD_D2_Pin;
		HAL_GPIO_Init(SD_D2_Port, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = SD_D3_Pin;
		HAL_GPIO_Init(SD_D3_Port, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = SD_CLK_Pin;
		HAL_GPIO_Init(SD_CLK_Port, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = SD_CMD_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF12_SDMMC1;
		HAL_GPIO_Init(SD_CMD_Port, &GPIO_InitStruct);

		HAL_NVIC_SetPriority(SDMMC1_IRQn, SD_SDMMC_PRIORITY+1, 0);
		HAL_NVIC_EnableIRQ(SDMMC1_IRQn);
	}
}

void HAL_SD_MspDeInit(SD_HandleTypeDef* hsd) {

	if (hsd->Instance == SDMMC1) {
		__HAL_RCC_SDMMC1_CLK_DISABLE();
		HAL_GPIO_DeInit(SD_D0_Port, SD_D0_Pin);
		HAL_GPIO_DeInit(SD_D1_Port, SD_D1_Pin);
		HAL_GPIO_DeInit(SD_D2_Port, SD_D2_Pin);
		HAL_GPIO_DeInit(SD_D3_Port, SD_D3_Pin);
		HAL_GPIO_DeInit(SD_CLK_Port, SD_CLK_Pin);
		HAL_GPIO_DeInit(SD_CMD_Port, SD_CMD_Pin);

	    HAL_DMA_DeInit(hsd->hdmarx);
	    HAL_DMA_DeInit(hsd->hdmatx);

	    HAL_NVIC_DisableIRQ(SDMMC1_IRQn);

	}

}


/**
 * @}
 */

/**
 * @}
 */
