#include "RTC.h"

// #define USE_DS1302
// #define USE_DS1307

#if defined(USE_DS1302)
#include "GPIO.h"
#include "Driver/DS1302.h"
DS1302<BOARD::D11, BOARD::D12, BOARD::D13> rtc;

#elif defined(USE_DS1307)
#include "TWI.h"
#include "Driver/DS1307.h"
#include "Software/TWI.h"
Software::TWI<BOARD::D6, BOARD::D7> twi;
DS1307 rtc(twi);

#else
#define USE_RTC
RTC rtc;
#endif

void setup()
{
  Serial.begin(57600);
  while (!Serial);

  set_zone(ONE_HOUR);
  struct tm now(SATURDAY, 2000, JANUARY, 1, 23, 59, 30);
  rtc.set_time(now);
}

void loop()
{
  struct tm now;
  time_t time = 0;
  char buf[32];

#if defined(USE_RTC)
  rtc.tick();
#endif

  rtc.get_time(now);
  Serial.print(time = mktime(&now));
  Serial.print(F(":wday="));
  Serial.print(now.tm_wday);
  Serial.print(F(",yday="));
  Serial.print(now.tm_yday);
  Serial.print(F(",utc+"));
  Serial.print(get_zone() / ONE_HOUR);
  Serial.print(F("=\""));
  Serial.print(isotime_r(&now, buf));
  Serial.print(F("\",utc=\""));
  Serial.print(isotime_r(gmtime_r(&time, &now), buf));
  Serial.println('"');

  delay(1000);
}
