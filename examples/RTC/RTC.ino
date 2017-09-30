#include "RTC.h"

// Configure: Set Real-Time Clock
// #define SET_TIME

// Configure: Use DS1302, DS1307 or software RTC
// #define USE_DS1302
#define USE_DS1307

#if defined(USE_DS1302)
#include "GPIO.h"
#include "Driver/DS1302.h"
DS1302<BOARD::D11, BOARD::D12, BOARD::D13> rtc;

#elif defined(USE_DS1307)
#include "TWI.h"
#include "Driver/DS1307.h"
// Configure: TWI bus manager (software or hardware)
// #define USE_SOFTWARE_TWI
#if defined(USE_SOFTWARE_TWI)
#include "GPIO.h"
#include "Software/TWI.h"
Software::TWI<BOARD::D18, BOARD::D19> twi;
#else
// Configure: Hardware TWI bus clock frequency (100 or 400 kHz)
#define FREQ 100000UL
// #define FREQ 400000UL
#include "Hardware/TWI.h"
Hardware::TWI twi(FREQ);
#endif
DS1307 rtc(twi);

#else
#define USE_RTC
RTC rtc;
#endif

void setup()
{
  Serial.begin(57600);
  while (!Serial);

  // Set Central European Time Zone: UTC+01:00
  set_zone(ONE_HOUR);

#if defined(SET_TIME)
  // Set clock to the test
  struct tm now(SATURDAY, 2000, JANUARY, 1, 23, 59, 30);
  rtc.set_time(now);
#endif
}

void loop()
{
  struct tm now;
  time_t time = 0;
  char buf[32];

#if defined(USE_RTC)
  if (!rtc.tick()) return;
#else
  uint32_t start = millis();
#endif

  // Read Real-Time Clock
  rtc.get_time(now);
  Serial.print(millis() / 1000.0);
  Serial.print(':');

  // Convert from tm struct to time_t (seconds since epoch, utc)
  Serial.print(time = mktime(&now));

  // Print calculated day-of week and year
  Serial.print(F(":wday="));
  Serial.print(now.tm_wday);
  Serial.print(F(",yday="));
  Serial.print(now.tm_yday);
  Serial.print(F(",utc+"));

  // Print time zone adjustment
  Serial.print(get_zone() / ONE_HOUR);
  Serial.print(F("=\""));

  // Print date in ISO format
  Serial.print(isotime_r(&now, buf));
  Serial.print(F("\",utc=\""));

  // Convert from time_t to struct tm and print
  Serial.print(isotime_r(gmtime_r(&time, &now), buf));
  Serial.println('"');

#if !defined(USE_RTC)
  // Approx delay for the next tick
  delay(1000 - (millis() - start));
#endif
}
