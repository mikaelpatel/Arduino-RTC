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
#if defined(AVR)
#include "Hardware/AVR/time.h"
#include "Hardware/AVR/RTC.h"
#elif defined(SAM)
#include "Hardware/SAM/RTC.h"
#endif
#endif
