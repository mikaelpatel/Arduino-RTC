/**
 * @file DS1302.h
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

#ifndef DS1302_H
#define DS1302_H

#include "GPIO.h"
#include "RTC.h"

#ifndef CHARBITS
#define CHARBITS 8
#endif

/**
 * GPIO based device driver for DS1302, Trickle-Charge Timekeeping Chip.
 * @param[in] CS_PIN chip select board pin.
 * @param[in] SDA_PIN serial data board pin.
 * @param[in] CLK_PIN clock board pin.
 *
 * @section Circuit
 * @code
 *                         DS1302/RTC
 *                       +------------+
 * (VCC)---------------1-|VCC         |
 * (GND)---------------2-|GND         |
 * (CLK)---------------3-|CLK         |
 * (SDA)---------------4-|DAT         |
 * (CS)----------------5-|RST         |
 *                       +------------+
 * @endcode
 *
 * @section References
 * 1. On-line product description,
 * http://www.maximintegrated.com/datasheet/index.mvp/id/2685
 * 2. Datasheet, http://datasheets.maximintegrated.com/en/ds/DS1302.pdf
 */
template<BOARD::pin_t CS_PIN, BOARD::pin_t SDA_PIN, BOARD::pin_t CLK_PIN>
class DS1302 {
public:
  /** Static memory size. */
  static const size_t RAM_MAX = 31;

  /**
   * Construct device driver for DS1302 Real-Time Clock with the given
   * pins. Initiate pins (output mode).
   */
  DS1302()
  {
    m_cs.output();
    m_cs.low();
    m_sda.output();
    m_clk.output();
    m_clk.low();
  }

  /**
   * Read clock and calender from the device. Return in standard
   * time structure.
   * @param[in,out] now time structure for return value.
   */
  void get_time(struct tm& now)
  {
    // Burst read clock and calender from device
    rtc_t rtc;
    m_cs.high();
    write(RTC_BURST | READ);
    m_sda.input();
    uint8_t* rp = (uint8_t*) &rtc;
    for (size_t i = 0; i < sizeof(rtc); i++, rp++)
      *rp = read();
    m_sda.output();
    m_cs.low();

    // Convert to standard time structure
    now.tm_sec = rtc.seconds;
    now.tm_min = rtc.minutes;
    now.tm_hour = rtc.hours;
    now.tm_mday = rtc.date;
    now.tm_wday = rtc.day - 1;
    now.tm_mon = rtc.month - 1;
    now.tm_year = rtc.year + 100;
  }

  /**
   * Write clock and calender in given standard time structure to the
   * device.
   * @param[in] now time to set.
   */
  void set_time(struct tm& now)
  {
    // Convert from standard time structure, and add write disable
    rtc_t rtc;
    rtc.seconds = now.tm_sec;
    rtc.minutes = now.tm_min;
    rtc.hours = now.tm_hour;
    rtc.date = now.tm_mday;
    rtc.day = now.tm_wday + 1;
    rtc.month = now.tm_mon + 1;
    rtc.year = now.tm_year - 100;
    rtc.wp = 0x80;

    // Burst write clock and calender to device
    write_enable();
    m_cs.high();
    write(RTC_BURST | WRITE);
    uint8_t* rp = (uint8_t*) &rtc;
    for (size_t i = 0; i < sizeof(rtc); i++, rp++)
      write(*rp);
    m_cs.low();
  }

  /**
   * Enable write of clock/calender or memory.
   */
  void write_enable()
    __attribute__((always_inline))
  {
    write(WP, 0x00);
  }

  /**
   * Disable write of clock/calender or memory.
   */
  void write_disable()
    __attribute__((always_inline))
  {
    write(WP, 0x80);
  }

  /**
   * Read given static memory address on the device and return byte.
   * @param[in] addr memory address on the device (0..RAM_MAX-1).
   */
  uint8_t read_ram(uint8_t addr)
    __attribute__((always_inline))
  {
    return (read(RAM_START | (addr & ADDR_MASK)));
  }

  /**
   * Write given data to the static memory (31 bytes). Requires
   * handling of write enable/disable.
   * @param[in] addr memory address (0..RAM_MAX-1).
   * @param[in] data to write to the memory address.
   */
  void write_ram(uint8_t addr, uint8_t data)
    __attribute__((always_inline))
  {
    write(RAM_START | (addr & ADDR_MASK), data);
  }

  /**
   * Burst read memory block from the device starting at address
   * zero(0). Data block is returned in the given buffer.
   * @param[in] buf pointer to buffer to store data read.
   * @param[in] size number of bytes to read (max RAM_MAX(31)).
   */
  void read_ram(void* buf, size_t size)
  {
    if (size == 0) return;
    uint8_t* bp = (uint8_t*) buf;
    if (size > RAM_MAX) size = RAM_MAX;
    m_cs.high();
    write(RAM_BURST | READ);
    m_sda.input();
    do *bp++ = read(); while (--size);
    m_sda.output();
    m_cs.low();
  }

  /**
   * Burst write data in buffer with given size to the static memory
   * in the device (max 31 bytes). Burst write is always from address
   * zero(0) and includes handling of write protect.
   * @param[in] buf pointer to memory block to write.
   * @param[in] size number of bytes to write (max RAM_MAX(31)).
   */
  void write_ram(void* buf, size_t size)
  {
    if (size == 0) return;
    uint8_t* bp = (uint8_t*) buf;
    if (size > RAM_MAX) size = RAM_MAX;
    write_enable();
    m_cs.high();
    write(RAM_BURST | WRITE);
    do write(*bp++); while (--size);
    m_cs.low();
    write_disable();
  }

protected:
  /** Start address of static memory. */
  static const uint8_t RAM_START = 32;

  /** Write protect register. */
  static const uint8_t WP = 0x07;

  /** Command byte. */
  enum {
    WRITE = 0x80,		//!< Read/write bit in write mode.
    READ = 0x81,		//!< Read/write bit in read mode.
    RTC_BURST = 0xbe,		//!< RTC register burst transfer.
    RAM_BURST = 0xfe,		//!< RAM burst transfer.
    ADDR_MASK = 0x3f		//!< Mask address bits.
  } __attribute__((packed));

  /**
   * Table 3. Register Address/Definition.
   */
  struct rtc_t {
    bcd_t seconds;		//!< 00-59 Seconds.
    bcd_t minutes;		//!< 00-59 Minutes.
    bcd_t hours;		//!< 00-23 Hours.
    bcd_t date;			//!< 01-31 Date.
    bcd_t month;		//!< 01-12 Month.
    bcd_t day;			//!< 01-07 Day.
    bcd_t year;			//!< 00-99 Year.
    uint8_t wp;			//!< Write protect register.
  } __attribute__((packed));

  GPIO<CS_PIN> m_cs;		//!< Chip select, asserted high.
  GPIO<SDA_PIN> m_sda;		//!< Serial data, bidirectional.
  GPIO<CLK_PIN> m_clk;		//!< Clock for synchronized data.

  /**
   * Low level RTC access function. Read data from the clock/calender
   * register or static memory on device.
   * @param[in] addr device address.
   * @return data.
   */
  uint8_t read(uint8_t addr)
  {
    addr = ((addr << 1) | READ);
    uint8_t res = 0;
    m_cs.high();
    write(addr);
    m_sda.input();
    res = read();
    m_sda.output();
    m_cs.low();
    return (res);
  }

  /**
   * Low level RTC access function. Write given data to the clock
   * register or static memory on device.
   * @param[in] addr device address.
   * @param[in] data to write.
   */
  void write(uint8_t addr, uint8_t data)
  {
    addr = ((addr << 1) | WRITE);
    m_cs.high();
    write(addr);
    write(data);
    m_cs.low();
  }

  /**
   * Low level read data from the device. Internal transfer
   * function. Used within a chip select block. Data direction must be
   * set before calling this function.
   * @return data read from the device.
   */
  uint8_t read()
  {
    uint8_t mask = 0x01;
    uint8_t res = 0;
    do {
      res |= (m_sda ? mask : 0x00);
      m_clk.toggle();
      __asm__ __volatile__("nop");
      mask <<= 1;
      m_clk.toggle();
      __asm__ __volatile__("nop");
    } while (mask);
    return (res);
  }

  /**
   * Write low level data to the device. Internal transfer
   * function. Used within a chip select block.
   * @param[in] data to write to the device.
   */
  void write(uint8_t data)
  {
    uint8_t mask = 0x01;
    do {
      m_sda = data & mask;
      __asm__ __volatile__("nop");
      m_clk.toggle();
      __asm__ __volatile__("nop");
      mask <<= 1;
      m_clk.toggle();
    } while (mask);
  }
};

#endif
