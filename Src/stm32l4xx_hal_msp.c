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

extern DMA_HandleTypeDef hdmaSpi1Rx;
extern DMA_HandleTypeDef HdmaSdmmc1;
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

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc) {

	GPIO_InitTypeDef GPIO_InitStruct;
	if (hadc->Instance == ADC1) {

		__HAL_RCC_ADC_CLK_ENABLE();

		GPIO_InitStruct.Pin = MESURE_ALIM_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_ANALOG_ADC_CONTROL;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(MESURE_ALIM_Port, &GPIO_InitStruct);
	}

}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc) {

	if (hadc->Instance == ADC1) {
		__HAL_RCC_ADC_CLK_DISABLE();
		HAL_GPIO_DeInit(MESURE_ALIM_Port, MESURE_ALIM_Pin);
	}
}

void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan) {

	GPIO_InitTypeDef GPIO_InitStruct;
	if (hcan->Instance == CAN1) {
		__HAL_RCC_CAN1_CLK_ENABLE();

		GPIO_InitStruct.Pin = CAN_RX_Pin | CAN_TX_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
		HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	}

}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* hcan) {

	if (hcan->Instance == CAN1) {
		__HAL_RCC_CAN1_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOD, CAN_RX_Pin | CAN_TX_Pin);
	}

}


void HAL_UART_MspInit(UART_HandleTypeDef* huart){

  GPIO_InitTypeDef GPIO_InitStruct;
  if(huart->Instance==LPUART1){
    __HAL_RCC_LPUART1_CLK_ENABLE();

    GPIO_InitStruct.Pin = RADIO_UART_RX_Pin|RADIO_UART_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_LPUART1;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  }
  else if(huart->Instance==UART4){
    __HAL_RCC_UART4_CLK_ENABLE();
  
    GPIO_InitStruct.Pin = MODEM_TX_Pin ;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
    HAL_GPIO_Init(MODEM_TX_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = MODEM_RX_Pin ;
    HAL_GPIO_Init(MODEM_RX_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = MODEM_RTS_Pin;
    HAL_GPIO_Init(MODEM_RTS_Port, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = MODEM_CTS_Pin;
    HAL_GPIO_Init(MODEM_CTS_Port, &GPIO_InitStruct);

    NVIC_SetPriority(UART4_IRQn, 5);
    NVIC_EnableIRQ(UART4_IRQn);

  }
  else if(huart->Instance==USART1){
    __HAL_RCC_USART1_CLK_ENABLE();
  
    GPIO_InitStruct.Pin = RS485_TX_Pin|RS485_RX_Pin|RS485_DE_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
  else if(huart->Instance == USART2){
	  //USART GPS
    __HAL_RCC_USART2_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPS_TX_Pin|GPS_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPS_TX_Port, &GPIO_InitStruct);
    HAL_GPIO_Init(GPS_RX_Port, &GPIO_InitStruct);

    NVIC_SetPriority(USART2_IRQn, 5);
    NVIC_EnableIRQ(USART2_IRQn);
  }
  else if(huart->Instance==USART3){

	  __HAL_RCC_USART3_CLK_ENABLE();

		GPIO_InitStruct.Pin = PC_TX_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
		HAL_GPIO_Init(PC_TX_Port, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = PC_RX_Pin;
		HAL_GPIO_Init(PC_RX_Port, &GPIO_InitStruct);

		NVIC_SetPriority(USART3_IRQn, 5);
	   	NVIC_EnableIRQ(USART3_IRQn);
	}

}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart) {
	if (huart->Instance == LPUART1) {
		__HAL_RCC_LPUART1_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOC, RADIO_UART_RX_Pin | RADIO_UART_TX_Pin);
	} else if (huart->Instance == UART4) {
		__HAL_RCC_UART4_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOA, MODEM_TX_Pin | MODEM_RX_Pin | MODEM_RTS_Pin);
		HAL_GPIO_DeInit(MODEM_CTS_Port, MODEM_CTS_Pin);
	} else if (huart->Instance == USART1) {
		__HAL_RCC_USART1_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOA, RS485_TX_Pin | RS485_RX_Pin | RS485_DE_Pin);
	} else if (huart->Instance == USART2) {
		__HAL_RCC_USART2_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOA, GPS_TX_Pin | GPS_RX_Pin);
	} else if (huart->Instance == USART3) {
		__HAL_RCC_USART3_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOC, PC_TX_Pin | PC_RX_Pin);
	}
}

void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc) {
	if (hrtc->Instance == RTC) {
		__HAL_RCC_RTC_ENABLE();
	}
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* hrtc) {

	if (hrtc->Instance == RTC) {
		__HAL_RCC_RTC_DISABLE();
	}

}

void HAL_SD_MspInit(SD_HandleTypeDef* hsd) {

	GPIO_InitTypeDef GPIO_InitStruct;
	if (hsd->Instance == SDMMC1) {

		__HAL_RCC_SDMMC1_CLK_ENABLE();

		GPIO_InitStruct.Pin = SD_D0_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
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

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi) {

	GPIO_InitTypeDef GPIO_InitStruct;
	if (hspi->Instance == SPI1) {

		__HAL_RCC_SPI1_CLK_ENABLE();


		//Configuration port SPI pour l'adc
		GPIO_InitStruct.Pin = ADC_EXT_SCK_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
		HAL_GPIO_Init(ADC_EXT_SCK_Port, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = ADC_EXT_MOSI_Pin;
		HAL_GPIO_Init(ADC_EXT_MOSI_Port, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = ADC_EXT_MISO_Pin;
		HAL_GPIO_Init(ADC_EXT_MISO_Port, &GPIO_InitStruct);

		hdmaSpi1Rx.Instance = DMA1_Channel2;
		hdmaSpi1Rx.Init.Request = DMA_REQUEST_1;
		hdmaSpi1Rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdmaSpi1Rx.Init.PeriphInc = DMA_PINC_DISABLE;
		hdmaSpi1Rx.Init.MemInc = DMA_MINC_ENABLE;
		hdmaSpi1Rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		hdmaSpi1Rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		hdmaSpi1Rx.Init.Mode = DMA_NORMAL;
		hdmaSpi1Rx.Init.Priority = DMA_PRIORITY_VERY_HIGH;
		if (HAL_DMA_Init(&hdmaSpi1Rx) != HAL_OK)
		{
		  Error_Handler();
		}

		__HAL_LINKDMA(hspi, hdmarx, hdmaSpi1Rx);

		HAL_NVIC_SetPriority(SPI1_IRQn, EXT_ADC_SPI_IRQ_PRIORITY, 0);
		HAL_NVIC_EnableIRQ(SPI1_IRQn);

	}
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi) {

	if (hspi->Instance == SPI1) {
		__HAL_RCC_SPI1_CLK_DISABLE();
		HAL_GPIO_DeInit(ADC_EXT_SCK_Port, ADC_EXT_SCK_Pin);
		HAL_GPIO_DeInit(ADC_EXT_MISO_Port, ADC_EXT_MISO_Pin);
		HAL_GPIO_DeInit(ADC_EXT_MOSI_Port, ADC_EXT_MOSI_Pin);
		HAL_DMA_DeInit(hspi->hdmarx);
		HAL_NVIC_DisableIRQ(SPI1_IRQn);
	}

}


void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base) {

	GPIO_InitTypeDef GPIO_InitStruct;
	if (htim_base->Instance == TIM2) {
		__HAL_RCC_TIM2_CLK_ENABLE();
		HAL_NVIC_SetPriority(TIM2_IRQn, MEASURE_TIMER_IRQ_PRIORITY, 0);
		HAL_NVIC_EnableIRQ(TIM2_IRQn);
	}
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base) {

	if (htim_base->Instance == TIM2) {
		__HAL_RCC_TIM2_CLK_DISABLE();
		HAL_GPIO_DeInit(GPS_PPS_Port, GPS_PPS_Pin);
		HAL_NVIC_DisableIRQ(TIM2_IRQn);
	}

}


/**
 * @}
 */

/**
 * @}
 */
