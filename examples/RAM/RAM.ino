#include "GPIO.h"
#include "RTC.h"
#include "Driver/DS1302.h"

DS1302<BOARD::D11, BOARD::D12, BOARD::D13> rtc;

void setup()
{
  Serial.begin(57600);
  while (!Serial);

  rtc.write_enable();
  for (size_t i = 0; i < rtc.RAM_MAX; i++)
    rtc.write_ram(i, i);
  rtc.write_disable();
  for (size_t i = 0; i < rtc.RAM_MAX; i++) {
    Serial.print(rtc.read_ram(i));
    Serial.print(F("  "));
  }
  Serial.println();
}

void loop()
{
  int8_t data[rtc.RAM_MAX];
  rtc.read_ram(&data, sizeof(data));

  for (size_t i = 0; i < rtc.RAM_MAX; i++) data[i] += 1;
  rtc.write_ram(&data, sizeof(data));

  for (size_t i = 0; i < rtc.RAM_MAX; i++) data[i] = 0;
  rtc.read_ram(&data, sizeof(data));

  for (size_t i = 0; i < rtc.RAM_MAX; i++) {
    Serial.print(data[i]);
    Serial.print(' ');
  }
  Serial.println();

  delay(1000);
}
