/*
 * Global.h
 *
 *  Created on: 17 sept. 2019
 *      Author: j.imbert
 *
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_


//#############################################################################
//                        INCLUSIONS
//#############################################################################
#include <stdint.h>
#include <stdbool.h>
//----------------------------------------------------------------------------
#include "printf.h"
//----------------------------------------------------------------------------
#include "stm32l4xx.h"

//#############################################################################
// 			PUBLIC SYMBOLIC CONSTANTS and MACROS
//#############################################################################
#define UNIT_TEST 1
#if (UNIT_TEST == 1)
	#define IN_UNIT_TEST(X)	(X)
	#define	NOT_IN_UNIT_TEST(X)
#else 
	#define IN_UNIT_TEST(X)
	#define	NOT_IN_UNIT_TEST(X)	(X)
#endif
	
#define DEBUG	1
#if (DEBUG == 1)
	#define DEBUG_LOG	printf
	#define	DBG(x)	x
#else

	#define DEBUG_LOG
	#define	DBG(x)
#endif


//-----------------------------------------------------------------------------
#define BIT_SET(x,n)		((x)  |=  (1<<(n)))
#define BIT_CLR(x,n)		((x)  &=   (~(1<<(n))))
#define	BIT_TEST(x,n)		(((x) &   (1<<(n))) != 0)
//-----------------------------------------------------------------------------
#ifndef TRUE
	#define	TRUE	1
#endif
#ifndef FALSE
	#define	FALSE 	0
#endif
//-----------------------------------------------------------------------------
#define DIGIT_TO_CHAR(x)	(x+'0')	//Convertion d'un chiffre en caratère
#define CHAR_TO_DIGIT(x)   (x-'0') // Convertion d'un caractère en chiffre
//-----------------------------------------------------------------------------
#define sizeofof(type, member) sizeof(((type *)0)->member)	//Permet de connaitre la taille d'une membre d'une structure
//-----------------------------------------------------------------------------
#define IS_AFTERNOON(time)	(time > 12)

//#############################################################################
//            PUBLIC TYPES, STRUCTURES, UNIONS and ENUMS
//#############################################################################
typedef enum{
	STATUS_OK       = 0,
	STATUS_ERROR    = 1,
	STATUS_BUSY     = 2,
	STATUS_TIME_OUT  = 3
}Status_TypeDef;
//-----------------------------------------------------------------------------
typedef struct {
	GPIO_TypeDef *port;
	uint32_t pin;
}Pinout_TypeDef;
//-----------------------------------------------------------------------------
typedef enum {
	DATE_TIME_FORMAT_CSV_FILE,
	DATE_TIME_FORMAT_FILENAME,
	DATE_TIME_FORMAT_LOG,
	DATE_TIME_FORMAT_SMS
}DateTime_FormatTypeDef;
//-----------------------------------------------------------------------------
typedef struct tm tm_t;

//-----------------------------------------------------------------------------

//#############################################################################
//						PUBLIC VARIABLES DECLARATION
//#############################################################################

//-----------------------------------------------------------------------------

//#############################################################################
//                     PUBLIC FUNCTION PROTOTYPES
//#############################################################################

void Float2Str(char *str, float val);
uint32_t Log2n(uint32_t n);
void RoundToSecond(uint32_t *ms);
void Float2Str(char *str, float val);
uint32_t Log2n(uint32_t n);
uint32_t CompareArray(uint8_t *ary1, uint8_t *ary2, uint32_t size);
bool StringToDateTime(char * str, struct tm * datetime);
bool DateTimeIsValid(struct tm * dateTime);
//-----------------------------------------------------------------------------



#endif //GLOBAL_H_
