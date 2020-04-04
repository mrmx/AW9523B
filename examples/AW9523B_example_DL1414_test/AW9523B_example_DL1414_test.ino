#include "AW9523B.h"

AW9523B gpio;

void writeChar(char ch, uint8_t column)
{
  uint8_t dataWrt = (ch & 0x7F) | 0x80; //data + WRT H
  gpio.write(P0, dataWrt);
  gpio.write(P1, (column & 0x03) << 6); //addr
  gpio.write(P0, ch & 0x7F);            //data + WRT L
  delayMicroseconds(1);
  gpio.write(P0, dataWrt); //data + WRT H
}

void clear()
{
  writeChar(' ', 0);
  writeChar(' ', 1);
  writeChar(' ', 2);
  writeChar(' ', 3);
}

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ; // If it's an Arduino Leonardo, wait for serial monitor

  if (!gpio.begin())
  {
    Serial.println("Error: AW9523B not found!");
    return;
  }
  gpio.reset();
  gpio.setPort0Mode(PUSH_PULL);
  clear();
}

void loop()
{
  for (uint8_t i = 0x20; i < 0x60; i++)
    for (uint8_t j = 0; j < 4; j++)
    {
      writeChar(i, j);
      delay(100);
    }
}
