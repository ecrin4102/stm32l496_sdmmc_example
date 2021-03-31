/*
 * global.c
 *
 *  Created on: 07 fev. 2020
 *      Author: j.imbert
 */

 //##############################################################################
 //                        INCLUDES
 //##############################################################################
#include "global.h"
//----------------------------------------------------------------------------
#include <string.h>
#include <stdlib.h>
#include <time.h>
//##############################################################################
//                 EXTERNAL variables, functions
//##############################################################################


//##############################################################################
//                 PRIVATE SYMBOLIC CONSTANTS and MACROS
//##############################################################################


//----------------------------------------------------------------------------


//##############################################################################
//               PRIVATE TYPES, STRUCTURES, UNIONS and ENUMS
//##############################################################################

//----------------------------------------------------------------------------

//##############################################################################
//                    PRIVATE VARIABLES DEFINITION
//##############################################################################

//##############################################################################
//                      PRIVATE FUNCTION PROTOTYPE
//##############################################################################

//##############################################################################
//                       PUBLIC FUNCTION CODE
//##############################################################################



void Float2Str(char *str, float val) {
	char tmp[10];
	uint32_t dec = ((float)val - (int)val) * 100000;

	itoa((int)val, str, 10);
	strcat(str, ",");
	itoa(dec, tmp, 10);
	if ((dec / 10000) < 1) {
		strcat(str, "0");
	}
	if ((dec / 1000) < 1) {
		strcat(str, "0");
	}
	if ((dec / 100) < 1) {
		strcat(str, "0");
	}
	if ((dec / 10) < 1) {
		strcat(str, "0");
	}

	strcat(str, tmp);
}

uint32_t Log2n(uint32_t n) {
	return (n > 1) ? 1 + Log2n(n / 2) : 0;
}

/**@brief Arrondi à la seconde sup ou inf. Par example, 1451 donne 1000 ou 1888 donne 2000
 * @param ms Pointeur vers le Nombre a arrondir
 */
void RoundToSecond(uint32_t *ms){
	uint32_t modulo = *ms%1000;
	if(modulo > 500){
		*ms += 1000-modulo;
	}else if(modulo < 500){
		*ms -= modulo;
	}
}

uint32_t CompareArray(uint8_t *ary1, uint8_t *ary2, uint32_t size) {
	uint32_t i;
	for(i=0;i<size;i++){
		if (ary1[i] != ary2[i]) {
			return(i+1);
		}
	}

	return(0);
}

bool StringToDateTime(char * str, struct tm * dateTime){
	char * endPointer;
	
	dateTime->tm_mday = (int)strtol( str, &endPointer, 10);
	dateTime->tm_mon = (int)strtol(++endPointer, &endPointer, 10) - 1;
	dateTime->tm_year = (int)strtol(++endPointer, &endPointer, 10) - 1900;
	dateTime->tm_hour = (int)strtol(++endPointer, &endPointer, 10);
	dateTime->tm_min = (int)strtol(++endPointer, &endPointer, 10);
	dateTime->tm_sec = (int)strtol(++endPointer, &endPointer, 10);

	if (endPointer != &str[19]) {
		return(false);
	}

	if (!DateTimeIsValid(dateTime)) {
		return(false);
	}

	return(true);
}


bool DateTimeIsValid(struct tm * dateTime) {
	if (dateTime->tm_mday <= 0 || dateTime->tm_mday > 31) {
		return(false);
	}
	if (dateTime->tm_mon < 0 || dateTime->tm_mon > 11) {
		return(false);
	}
	if (dateTime->tm_year < 120) {
		return(false);
	}
	if (dateTime->tm_hour < 0 || dateTime->tm_hour > 23) {
		return(false);
	}
	if (dateTime->tm_min < 0 || dateTime->tm_min > 59) {
		return(false);
	}
	if (dateTime->tm_sec < 0 || dateTime->tm_sec > 59) {
		return(false);
	}

	return(true);
}


