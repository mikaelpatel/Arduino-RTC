/**
 * @file Hardware/SAM/RTC.h
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

#ifndef HARDWARE_SAM_RTC_H
#define HARDWARE_SAM_RTC_H

#include <time.h>
#undef RTC

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
 * The isotime function constructs an ascii string in the form
 * \code YYYY-MM-DD hh:mm:ss\endcode
 */
#define isotime_r(tm,buf) (strftime (buf, 32, "%F %T", tm), buf)

/**
 * Set the 'time zone'. The parameter is given in seconds East of the
 * Prime Meridian. Example for New York City: \code set_zone(-5 *
 * ONE_HOUR);\endcode
 *
 * If the time zone is not set, the time system will operate in UTC only.
 */
#define set_zone(x) _timezone = x
#define get_zone() _timezone

/**
 * Software Real-Time Clock.
 */
class RTC {
public:
  /**
   * Construct software real-time clock based on millis().
   */
  RTC() :
    m_millis(0),
    m_time(0)
  {}

  /**
   * Increment seconds counter when time has elapsed.
   * @return true(1) if an increment occured, otherwise false(0).
   */
  bool tick()
  {
    uint32_t now = millis();
    if (now - m_millis < 1000) return (false);
    m_time += 1;
    m_millis = now;
    return (true);
  }

  /**
   * Current time in seconds from epoch.
   * @return seconds from epoch.
   */
  time_t get_time()
  {
    time_t res = m_time;
    return (res);
  }

  /**
   * Set the current time (seconds) from epoch.
   */
  void set_time(time_t time)
  {
    m_time = time;
  }

  /**
   * Return the current time as a time structure.
   */
  void get_time(struct tm& now)
  {
    time_t time = get_time();
    gmtime_r(&time, &now);
  }

  /**
   * Set the current time based on the given time structure.
   */
  void set_time(struct tm& now)
  {
    set_time(mktime(&now) + get_zone());
  }

protected:
  /** Timestamp for previous tick call. */
  volatile uint16_t m_millis;

  /** Current time from epoch. */
  volatile time_t m_time;
};
#endif
