/*
 * mktime.c
 *
 *  Created on: 21 sept. 2020
 *      Author: j.imbert
 */
#include "mktime.h"
#include <time.h>

 time_t _mktime(const struct tm *t){
	register long year;
	register time_t result;
	#define MONTHSPERYEAR   12      /* months per calendar year */
	static const int cumdays[MONTHSPERYEAR] =
		{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };

	/*@ +matchanyintegral @*/
	year = 1900 + t->tm_year + t->tm_mon / MONTHSPERYEAR;
	result = (year - 1970) * 365 + cumdays[t->tm_mon % MONTHSPERYEAR];
	//Gestion nombre d'annnée bissexstille depuis 1970
	result += (year - 1968) / 4;
	result -= (year - 1900) / 100;
	result += (year - 1600) / 400;
	//Si l'anneé en cours est bissextile mais qu'on a pas passé le moi de le fevvrier
	if ((year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0) &&
		(t->tm_mon % MONTHSPERYEAR) < 2)
		result--;
	result += t->tm_mday - 1;
	result *= 24;
	result += t->tm_hour;
	result *= 60;
	result += t->tm_min;
	result *= 60;
	result += t->tm_sec;
	if (t->tm_isdst == 1)
		result -= 3600;
	/*@ -matchanyintegral @*/
	return (result);
 }
