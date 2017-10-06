/**
 * @file Hardware/AVR/time.h
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2017, Mikael Patel
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 */

#ifndef HARDWARE_AVR_TIME_H
#define HARDWARE_AVR_TIME_H

#include <stdint.h>
#include <string.h>

/**
 * time_t represents seconds elapsed from Midnight, Jan 1 2000 UTC
 * (the Y2K 'epoch'). Its range allows this implementation to
 * represent time up to Tue Feb 7 06:28:15 2136 UTC.
 */
typedef uint32_t time_t;

/**
 * The tm structure contains a representation of time 'broken down'
 * into components of the Gregorian calendar.
 */
struct tm {
  int8_t tm_sec;	//!< Seconds [0-60].
  int8_t tm_min;	//!< Minutes [0-59].
  int8_t tm_hour;	//!< Hours [0-23].
  int8_t tm_mday;	//!< Day in Month [1-31].
  int8_t tm_wday;	//!< Days since Sunday [0-6].
  int8_t tm_mon;	//!< 0-11 Months since January [0-11].
  int16_t tm_year;	//!< Years since 1900.
  int16_t tm_yday;	//!< days since January 1 [0-365].
  int16_t tm_isdst;	//!< Daylight Saving Time flag [-1/0/1].

  tm()
  {
    memset(this, 0, sizeof(tm));
  }

  tm(int8_t wday, int16_t year, int8_t mon, int8_t mday,
     int8_t hour, int8_t min, int8_t sec) :
    tm_sec(sec),
    tm_min(min),
    tm_hour(hour),
    tm_mday(mday),
    tm_wday(wday),
    tm_mon(mon),
    tm_year(year - 1900),
    tm_yday(0),
    tm_isdst(0)
  {
  }
} __attribute__((packed));

enum {
  SUNDAY,
  MONDAY,
  TUESDAY,
  WEDNESDAY,
  THURSDAY,
  FRIDAY,
  SATURDAY
};

enum {
  JANUARY,
  FEBRUARY,
  MARCH,
  APRIL,
  MAY,
  JUNE,
  JULY,
  AUGUST,
  SEPTEMBER,
  OCTOBER,
  NOVEMBER,
  DECEMBER
};

/**
 * The difftime function returns the difference between two binary
 * time stamps, time1 - time0.
 */
int32_t difftime(time_t time1, time_t time0);

/**
 * This function 'compiles' the elements of a broken-down time
 * structure, returning a binary time stamp.  The elements of timeptr
 * are interpreted as representing Local Time. The original values of
 * the tm_wday and tm_yday elements of the structure are ignored, and
 * the original values of the other elements are not restricted to the
 * ranges stated for struct tm.
 *
 * On successful completion, the values of all elements of timeptr are
 * set to the appropriate range.
 */
time_t mktime(struct tm* timeptr);

/**
 * This function 'compiles' the elements of a broken-down time
 * structure, returning a binary time stamp. The elements of timeptr
 * are interpreted as representing UTC.
 *
 * The original values of the tm_wday and tm_yday elements of the
 * structure are ignored, and the original values of the other
 * elements are not restricted to the ranges stated for struct tm.
 *
 * Unlike mktime(), this function DOES NOT modify the elements of timeptr.
 */
time_t mk_gmtime(const struct tm* timeptr);

/**
 * The gmtime function converts the time stamp pointed to by timer
 * into broken-down time, expressed as UTC.
 */
struct tm* gmtime_r(const time_t* timer, struct tm* timeptr);

/**
 * The localtime function converts the time stamp pointed to by timer
 * into broken-down time, expressed as Local time.
 */
struct tm* localtime_r(const time_t* timer, struct tm* timeptr);

/**
 * The isotime function constructs an ascii string in the form
 * \code YYYY-MM-DD hh:mm:ss\endcode
 */
char* isotime_r(const struct tm* tmptr, char* buf);

/**
 * Return 1 if year is a leap year, zero if it is not.
 */
uint8_t is_leap_year(int16_t year);

/** One hour, expressed in seconds */
#define ONE_HOUR 3600

/** Angular degree, expressed in arc seconds */
#define ONE_DEGREE 3600

/** One day, expressed in seconds */
#define ONE_DAY 86400UL

/** Difference between the Y2K and the UNIX epochs, in seconds. */
#define UNIX_OFFSET 946684800UL

/** Difference between the Y2K and the NTP epochs, in seconds. */
#define NTP_OFFSET 3155673600UL

/**
 * Specify the Daylight Saving function. The Daylight Saving function
 * should examine its parameters to determine whether Daylight Saving
 * is in effect, and return a value appropriate for tm_isdst.
 *
 * If a Daylight Saving function is not specified, the system will
 * ignore Daylight Saving.
 */
void set_dst(int (*)(const time_t*, int32_t*));

/**
 * Set the 'time zone'. The parameter is given in seconds East of the
 * Prime Meridian. Example for New York City: \code set_zone(-5 *
 * ONE_HOUR);\endcode
 *
 * If the time zone is not set, the time system will operate in UTC only.
 */
void set_zone(int32_t);
uint32_t get_zone();
#endif
