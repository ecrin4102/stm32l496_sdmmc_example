/*
 * mktime.h
 *
 *  Created on: 21 sept. 2020
 *      Author: j.imbert
 */

#ifndef MKTIME_H_
#define MKTIME_H_
#include <time.h>
#define mktime(t) _mktime

 time_t _mktime(const struct tm *t);

#endif /* MKTIME_H_ */
