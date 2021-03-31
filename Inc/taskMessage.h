/*
 * TaskMessage.h
 *
 *  Created on: 25 oct. 2019
 *      Author: j.imbert
 */

#ifndef TASKMESSAGE_H_
#define TASKMESSAGE_H_

 //#############################################################################
 //                        INCLUDES
 //#############################################################################
#include <stdint.h>
//----------------------------------------------------------------------------
#include "FreeRTOS.h"
#include "task.h"
//----------------------------------------------------------------------------

//##############################################################################
//                 EXTERNAL variables, functions
//##############################################################################

//#############################################################################
// 			PUBLIC SYMBOLIC CONSTANTS and MACROS
//#############################################################################


//-----------------------------------------------------------------------------

//#############################################################################
//            PUBLIC TYPES, STRUCTURES, UNIONS and ENUMS
//#############################################################################
typedef enum {
	TASK_INDEFINI,
	TASK_Superviseur,
	TASK_RS232,
	TASK_Capteurs,
	TASK_Geophones,
	TASK_RS485,
	TASK_CAN,
	TASK_Gyro_Accelero,
	TASK_Compute,
	TASK_Modem,
	TASK_SDCAR
} TaskSender_TypeDef;

//Sparer les diffrents types de message ?
typedef enum {
	MESSAGE_INDEFINI,
	MESSAGE_START,
	MESSAGE_STOP,
	MESSAGE_REBOOT,
	MESSAGE_UPDATE,
	MESSAGE_CONFIG,
	MESSAGE_DATA,
	MESSAGE_TEST,
	MESSAGE_INIT,
	MESSAGE_CMD_POSITIONNING,
	MESSAGE_CMD_SEND_SMS,
	MESSAGE_CMD_TREAT_SMS,
	MESSAGE_CMD_SEND_FILES,
	MESSAGE_CMD_RECEIVE_FILE_FTP,
	MESSAGE_CMD_SEND_CFG,
	MESSAGE_CMD_RECEIVE_CFG,
	MESSAGE_CMD_RECEIVE_FIRMWARE,
	MESSAGE_CMD_HOTLINE,
	MESSAGE_CMD_SAVE_DATA,
	MESSAGE_CMD_ALARM
} TaskMessageType_TypeDef;

typedef struct {
	TaskHandle_t * sourceHandle;
	TaskMessageType_TypeDef messageType;
	void *data;
	uint32_t dataSize;
} TaskMessage_TypeDef;

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

//#############################################################################
//						PUBLIC VARIABLES DECLARATION
//#############################################################################

//-----------------------------------------------------------------------------

//#############################################################################
//                     PUBLIC FUNCTION PROTOTYPES
//#############################################################################
void TaskMessageInit(TaskMessage_TypeDef *msg,
		TaskHandle_t * sourceHandle,
		TaskMessageType_TypeDef messageType,
		void *data,
		uint32_t dataSize);
//-----------------------------------------------------------------------------


#endif /* TASKMESSAGE_H_ */
