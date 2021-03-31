/*
 * main.c
 *
 *  Created on	: 02 oct. 2019
 *  Author		: j.imbert
 */

//##############################################################################
//                        INCLUDES
//##############################################################################
#include "appCore.h"
#include "main.h"
#include "global.h"
#include "main.h"


//----------------------------------------------------------------------------
#include "stm32l4xx_hal.h"
#include "stm32l4xx_it.h"
//----------------------------------------------------------------------------
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "fatfs.h"
//----------------------------------------------------------------------------
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
//----------------------------------------------------------------------------

//##############################################################################
//                 EXTERNAL variables, functions
//##############################################################################
extern SemaphoreHandle_t ExtAdcDmaErrorSemaphone;
//----------------------------------------------------------------------------

//##############################################################################
//                 PRIVATE SYMBOLIC CONSTANTS and MACROS
//##############################################################################



Trace_Typedef TraceTask = {0};

void TraceAdd(Trace_Typedef * trace, uint32_t tick, uint32_t data){
	trace->tick[trace->id] = tick;
	trace->data[trace->id] = data;
	trace->id++;
	if(trace->id == TRACE_SIZE){
		trace->id = 0;
	}
}


//##############################################################################
//               PRIVATE TYPES, STRUCTURES, UNIONS and ENUMS
//##############################################################################


//##############################################################################
//                    PRIVATE VARIABLES DEFINITION
//##############################################################################
TIM_HandleTypeDef  HtimTick;
SD_HandleTypeDef Hsd1;

//----------------------------------------------------------------------------
osThreadId defaultTaskHandle;
//----------------------------------------------------------------------------
//##############################################################################
//                      PRIVATE FUNCTION PROTOTYPE
//##############################################################################
void SystemClock_Config(void);
//----------------------------------------------------------------------------
void MX_GPIO_Init(void);
void MX_SDMMC1_SD_Init(void);
void MX_DMA2_Init(void);
void MX_SDMMC1_SD_Init(void);
//----------------------------------------------------------------------------


//##############################################################################
//                      FUNCTIONS CODE
//##############################################################################


/**
 * @brief  The application entry point.
 *
 * @retval None
 */
int main(void) {

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();


	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA2_Init();
	MX_SDMMC1_SD_Init();

	MX_FREERTOS_Init();

	// Start scheduler
	vTaskStartScheduler();

	/* We should never get here as control is now taken by the scheduler */
	/* Infinite loop */

	while (1){

	}
}

void vApplicationStackOverflowHook( TaskHandle_t xTask, signed char *pcTaskName ){
	uint32_t i;
	printf("Stack overflow of task %s\r\n", pcTaskName);
	HardFault_Handler();
	while(1){
		HAL_GPIO_TogglePin(CMD_LED_ROUGE_Port, CMD_LED_ROUGE_Pin);
		for(i=0;i<5000000;i++);
	}
}


/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	HAL_PWR_EnableBkUpAccess();

	__HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE
			| RCC_OSCILLATORTYPE_LSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 3;
	RCC_OscInitStruct.PLL.PLLN = 40;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;

	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}


	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC
			| RCC_PERIPHCLK_USART1 | RCC_PERIPHCLK_USART2 | RCC_PERIPHCLK_USART3
			| RCC_PERIPHCLK_UART4 | RCC_PERIPHCLK_LPUART1 | RCC_PERIPHCLK_SDMMC1
			| RCC_PERIPHCLK_ADC;
	PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
	PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
	PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
	PeriphClkInit.Uart4ClockSelection = RCC_UART4CLKSOURCE_SYSCLK;
	PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
	PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
	PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
	PeriphClkInit.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_PLLSAI1;
	PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSE;
	//PLLSAI1 = HSE/PLLSAI1M*PLLSAI1N/RCC_PLLQ_DIV2 = 12Mhz/3*25/2=50Mhz
	//TODO: diminuer la clock et baisser le prescaler du SDMMC si influence de la conso
	PeriphClkInit.PLLSAI1.PLLSAI1M = 3;
	PeriphClkInit.PLLSAI1.PLLSAI1N = 25;
	PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV2;
	PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
	PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
	PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK
			| RCC_PLLSAI1_ADC1CLK;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1)
			!= HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	HAL_InitTick(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
}


/** Configure pins as 
 * Analog
 * Input
 * Output
 * EVENT_OUT
 * EXTI
 */
void MX_GPIO_Init(void) {

	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	//############ I/O g�n�riques ###############

}

void MX_DMA2_Init(void){
	__HAL_RCC_DMA2_CLK_ENABLE();
}


void MX_SDMMC1_SD_Init(void) {

	Hsd1.Instance = SDMMC1;
	Hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
	Hsd1.Init.ClockBypass = SDMMC_CLOCK_BYPASS_DISABLE;
	Hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
	Hsd1.Init.BusWide = SDMMC_BUS_WIDE_1B;
	Hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_ENABLE;
	//Clock = PLLSAI1 / (ClockDiv+2 ) = 50Mhz / 8 = 6.25Mhz
	//Inutile d'augmenter la fréquence, le débit n'augmentera pas et sera limité par le CPU en mode Irq
	Hsd1.Init.ClockDiv = 6;
	if (HAL_SD_Init(&Hsd1) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}
}



/**
 * @brief  This function is executed in case of error occurrence.
 * @param  file: The file name as string.
 * @param  line: The line in file as a number.
 * @retval None
 */
void _Error_Handler(char *file, int line) {
	printf("%s => %d\r\n", file, line);
	HAL_GPIO_WritePin(CMD_LED_ROUGE_Port, CMD_LED_ROUGE_Pin, GPIO_PIN_SET);
}

void *malloc(size_t size) {

	while(1){
		printf("/!\\ Utilisation de malloc/!\\\r\n");

	}
	return(0);
}

void free(void *alloc){
	printf("/!\\ Utilisation de free/!\\\r\n");
}



#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line){

	// User can add his own implementation to report the file name and line number,
	// tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line)

}
#endif /* USE_FULL_ASSERT */



/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
