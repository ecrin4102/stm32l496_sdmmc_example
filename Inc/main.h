/*
 * BspModem.h
 *
 *  Created on: 17 sept. 2019
 *      Author: j.imbert
 */

#ifndef __MAIN_H__
#define __MAIN_H__

//#############################################################################
//                        INCLUDES
//#############################################################################
#include <stdint.h>
#include "stm32l4xx_hal.h"
//#############################################################################
// 			PUBLIC SYMBOLIC CONSTANTS and MACROS
//#############################################################################
#define CMD_LED_ROUGE_Pin GPIO_PIN_0
#define CMD_LED_ROUGE_Port GPIOF
#define CMD_LED_VERTE_Pin GPIO_PIN_1
#define CMD_LED_VERTE_Port GPIOF
#define CMD_RELAY1_Pin GPIO_PIN_2
#define CMD_RELAY1_Port GPIOF
#define CMD_RELAY2_Pin GPIO_PIN_3
#define CMD_RELAY2_Port GPIOF
#define EN_5V_Pin GPIO_PIN_9
#define EN_5V_Port GPIOE
#define RADIO_UART_RX_Pin GPIO_PIN_0
#define RADIO_UART_RX_Port GPIOC
#define RADIO_UART_TX_Pin GPIO_PIN_1
#define RADIO_UART_TX_Port GPIOC
#define MESURE_ALIM_Pin GPIO_PIN_2
#define MESURE_ALIM_Port GPIOC
#define RADIO_CMD_PWR_Pin GPIO_PIN_3
#define RADIO_CMD_PWR_Port GPIOC
#define MODEM_STATUS_Pin GPIO_PIN_5
#define MODEM_STATUS_Port GPIOB
#define MODEM_PWR_EN_Pin GPIO_PIN_9
#define MODEM_PWR_EN_Port GPIOF
#define MODEM_CTS_Pin GPIO_PIN_7
#define MODEM_CTS_Port GPIOB
#define MODEM_TX_Pin GPIO_PIN_0
#define MODEM_TX_Port GPIOA
#define MODEM_RX_Pin GPIO_PIN_1
#define MODEM_RX_Port GPIOA
#define MODEM_RTS_Pin GPIO_PIN_15
#define MODEM_RTS_Port GPIOA
#define GPS_TX_Pin GPIO_PIN_2
#define GPS_TX_Port GPIOA
#define GPS_RX_Pin GPIO_PIN_3
#define GPS_RX_Port GPIOA
#define GPS_PPS_Pin GPIO_PIN_5
#define GPS_PPS_Port GPIOA
#define GPS_FORCEON_Pin GPIO_PIN_4
#define GPS_FORCEON_Port GPIOA
#define GPS_CMD_ACTIVE_ANT_Pin GPIO_PIN_7
#define GPS_CMD_ACTIVE_ANT_Port GPIOA
#define PC_TX_Pin GPIO_PIN_4
#define PC_TX_Port GPIOC
#define PC_RX_Pin GPIO_PIN_5
#define PC_RX_Port GPIOC
#define EXP_CS_Pin GPIO_PIN_10
#define EXP_CS_Port GPIOE
#define ADC_EXT_SCK_Pin GPIO_PIN_13
#define ADC_EXT_SCK_Port GPIOE
#define ADC_EXT_MISO_Pin GPIO_PIN_14
#define ADC_EXT_MISO_Port GPIOE
#define ADC_EXT_MOSI_Pin GPIO_PIN_15
#define ADC_EXT_MOSI_Port GPIOE
#define ADC_EXT_CS1_Pin GPIO_PIN_11
#define ADC_EXT_CS1_Port GPIOE
#define ADC_EXT_CS2_Pin GPIO_PIN_12
#define ADC_EXT_CS2_Port GPIOB
#define ADC_EXT_BUSY1_Pin GPIO_PIN_12
#define ADC_EXT_BUSY1_Port GPIOE
#define ADC_EXT_BUSY2_Pin GPIO_PIN_10
#define ADC_EXT_BUSY2_Port GPIOB
#define ADC_EXT_RESET_Pin GPIO_PIN_13
#define ADC_EXT_RESET_Port GPIOB
#define ADC_EXT_CONVSTAB_Pin GPIO_PIN_14
#define ADC_EXT_CONVSTAB_Port GPIOB
#define ADC_EXT_RANGE_Pin GPIO_PIN_15
#define ADC_EXT_RANGE_Port GPIOB
#define ADC_EXT_STBY_Pin GPIO_PIN_8
#define ADC_EXT_STBY_Port GPIOD
#define ADC_EXT_OS2_Pin GPIO_PIN_9
#define ADC_EXT_OS2_Port GPIOD
#define ADC_EXT_OS1_Pin GPIO_PIN_10
#define ADC_EXT_OS1_Port GPIOD
#define ADC_EXT_OS0_Pin GPIO_PIN_11
#define ADC_EXT_OS0_Port GPIOD
#define SD_D0_Pin GPIO_PIN_8
#define SD_D0_Port GPIOC
#define SD_D1_Pin GPIO_PIN_9
#define SD_D1_Port GPIOC
#define RS485_TX_Pin GPIO_PIN_9
#define RS485_TX_Port GPIOA
#define RS485_RX_Pin GPIO_PIN_10
#define RS485_RX_Port GPIOA
#define RS485_RE_Pin GPIO_PIN_11
#define RS485_RE_Port GPIOA
#define RS485_DE_Pin GPIO_PIN_12
#define RS485_DE_Port GPIOA
#define MODEM_RTS_Pin GPIO_PIN_15
#define MODEM_RTS_Port GPIOA
#define SD_D2_Pin GPIO_PIN_10
#define SD_D2_Port GPIOC
#define SD_D3_Pin GPIO_PIN_11
#define SD_D3_Port GPIOC
#define SD_CLK_Pin GPIO_PIN_12
#define SD_CLK_Port GPIOC
#define CAN_RX_Pin GPIO_PIN_0
#define CAN_RX_Port GPIOD
#define CAN_TX_Pin GPIO_PIN_1
#define CAN_TX_Port GPIOD
#define SD_CMD_Pin GPIO_PIN_2
#define SD_CMD_Port GPIOD

//#############################################################################
//            PUBLIC TYPES, STRUCTURES, UNIONS and ENUMS
//#############################################################################

//-----------------------------------------------------------------------------
#define TRACE_SIZE 8192
typedef struct{
	uint16_t id;
	uint32_t tick[TRACE_SIZE];
	uint32_t data[TRACE_SIZE];
}Trace_Typedef;
//-----------------------------------------------------------------------------




//#############################################################################
//						PUBLIC VARIABLES DECLARATION
//#############################################################################
//-----------------------------------------------------------------------------

//#############################################################################
//                     PUBLIC FUNCTION PROTOTYPES
//#############################################################################
void _Error_Handler(char *file, int line);
//-----------------------------------------------------------------------------



#ifdef __cplusplus
 extern "C" {
#endif
void _Error_Handler(char *, int);

#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H__ */
