/**
 * @file RTC.h
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

#ifndef RTC_H
#define RTC_H

#include "bcd.h"
#include "time.h"

/**
 * Software Real-Time Clock.
 */
class RTC {
public:
  /**
   * Construct software real-time clock based on millis().
   */
  RTC() : m_millis(0), m_time(0) {}

  /**
   * Increment seconds counter when time has elapsed. Call
   * at least every 1-50 milli-seconds.
   * Return true(1) if an increment occured, otherwise false(0).
   */
  bool tick()
  {
    uint16_t now = millis();
    if (now - m_millis < 1000) return (false);
    uint8_t sreg = SREG;
    __asm__ __volatile__("cli" ::: "memory");
    while (now - m_millis > 1000) {
      m_time += 1;
      m_millis += 1000;
    }
    m_millis = now;
    SREG = sreg;
    __asm__ __volatile__("" ::: "memory");
    return (true);
  }

  /**
   * Return the current time in seconds from epoch.
   */
  time_t get_time()
  {
    uint8_t sreg = SREG;
    __asm__ __volatile__("cli" ::: "memory");
    time_t res = m_time;
    SREG = sreg;
    __asm__ __volatile__("" ::: "memory");
    return (res);
  }

  /**
   * Set the current time (seconds) from epoch.
   */
  void set_time(time_t time)
  {
    uint8_t sreg = SREG;
    __asm__ __volatile__("cli" ::: "memory");
    m_time = time;
    SREG = sreg;
    __asm__ __volatile__("" ::: "memory");
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
    extern long __utc_offset;
    set_time(mktime(&now) + __utc_offset);
  }

protected:
  /** Timestamp for previous tick call. */
  volatile uint16_t m_millis;

  /** Current time from epoch. */
  volatile time_t m_time;
};

#endif
