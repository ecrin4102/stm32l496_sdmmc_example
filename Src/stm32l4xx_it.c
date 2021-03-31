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
//----------------------------------------------------------------------------
#include "FreeRTOS.h"
#include "task.h"
//----------------------------------------------------------------------------

//##############################################################################
//                 EXTERNAL variables, functions
//##############################################################################

extern TaskHandle_t FileManagerTaskHandle;

//----------------------------------------------------------------------------
extern SD_HandleTypeDef Hsd1;
extern TIM_HandleTypeDef  HtimTick;
//----------------------------------------------------------------------------


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

void debugHardFault( uint32_t *pulFaultStackAddress ){
	uint32_t i;
	/* These are volatile to try and prevent the compiler/linker optimising them
	away as the variables never actually get used.  If the debugger won't show the
	values of the variables, make them global my moving their declaration outside
	of this function. */
	volatile uint32_t r0;
	volatile uint32_t r1;
	volatile uint32_t r2;
	volatile uint32_t r3;
	volatile uint32_t r12;
	volatile uint32_t lr; /* Link register. */
	volatile uint32_t pc; /* Program counter. */
	volatile uint32_t psr;/* Program status register. */
	uint32_t freeStack;


	r0 = pulFaultStackAddress[ 0 ];
	r1 = pulFaultStackAddress[ 1 ];
	r2 = pulFaultStackAddress[ 2 ];
	r3 = pulFaultStackAddress[ 3 ];

	r12 = pulFaultStackAddress[ 4 ];
	lr = pulFaultStackAddress[ 5 ];
	pc = pulFaultStackAddress[ 6 ];
	psr = pulFaultStackAddress[ 7 ];

	printf("Hard Fault :\r\n", r0);
	printf("r0:%x\r\n", r0);
	printf("r1:%x\r\n", r1);
	printf("r2:%x\r\n", r2);
	printf("r3:%x\r\n", r3);
	printf("r12:%x\r\n", r12);
	printf("lr:%x\r\n", lr);
	printf("pc:%x\r\n", pc);
	printf("psr:%x\r\n", psr);
	printf("\r\n");

	const char *line = "%s : %d free stack space\r\n";

	freeStack = uxTaskGetStackHighWaterMark(FileManagerTaskHandle);
	printf(line, pcTaskGetTaskName(FileManagerTaskHandle), freeStack);



	TaskHandle_t currentTask = xTaskGetCurrentTaskHandle();
	printf("Tache active : %s\r\n", pcTaskGetName(currentTask));
	printf("Free heap : %d\r\n", xPortGetFreeHeapSize());
    /* When the following line is hit, the variables contain the register values. */
    while (1)
    {
  	  HAL_GPIO_TogglePin(CMD_LED_ROUGE_Port, CMD_LED_ROUGE_Pin);
  	 for(i=0;i<500000;i++);
    }
}

/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void) {

}

/**
* @brief This function handles Hard fault interrupt.
*/
#ifndef SIMULATOR
__attribute__((naked)) void HardFault_Handler( void ){
	__asm volatile
	    (
	        "tst lr, #4                                    \n"
	        "ite eq                                        \n"
	        "mrseq r0, msp                                 \n"
	        "mrsne r0, psp                                 \n"
	        "bl debugHardFault                   		   \n"
	    );
   // "bx r1                                         \n"
//	"debugHardfault_address: .word prvGetRegistersFromStack  \n"
	//"debugHardfault_address: .word prvGetRegistersFromStack  \n"
}
#else
void HardFault_Handler(void) {
	printf("\r\nHardFault_Handler\r\n");
	while (1);
}
#endif


/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void) {

	while (1) {

	}
}

/**
 * @brief This function handles Prefetch fault, memory access fault.
 */
void BusFault_Handler(void) {

	while (1) {

	}
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void) {

	while (1) {

	}
}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void) {

}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void) {
	//Systick handle a TIM
	//osSystickHandler();
}




/**
  * @brief This function handles TIM6 global interrupt, DAC channel1 and channel2 underrun error interrupts.
  */
void TIM6_DAC_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&HtimTick);
}


/**
  * @brief This function handles SDMMC1 global interrupt.
  */
void SDMMC1_IRQHandler(void)
{
	HAL_SD_IRQHandler(&Hsd1);
}

/**
  * @brief This function handles DMA2 channel5 global interrupt.
  */
void DMA2_Channel5_IRQHandler(void)
{
  if((Hsd1.Context == (SD_CONTEXT_DMA | SD_CONTEXT_READ_SINGLE_BLOCK)) ||
     (Hsd1.Context == (SD_CONTEXT_DMA | SD_CONTEXT_READ_MULTIPLE_BLOCK)))
  {
	  HAL_DMA_IRQHandler(Hsd1.hdmarx);
  }
  else if((Hsd1.Context == (SD_CONTEXT_DMA | SD_CONTEXT_WRITE_SINGLE_BLOCK)) ||
          (Hsd1.Context == (SD_CONTEXT_DMA | SD_CONTEXT_WRITE_MULTIPLE_BLOCK)))
  {
	  HAL_DMA_IRQHandler(Hsd1.hdmatx);
  }
}

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM1 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim == &HtimTick) {
		HAL_IncTick();
		if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED){
		    xPortSysTickHandler();
		  }
	}
}

void HAL_SD_ErrorCallback(SD_HandleTypeDef *hsd){
	_Error_Handler(__FILE__, __LINE__);
}








/******************************************************************************/
/* STM32L4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l4xx.s).                    */
/******************************************************************************/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
