#include "AW9523B.h"

AW9523B gpio;
int brightness = 0; // how bright the LED is
int fadeAmount = 5; // how many points to fade the LED by

void writeChar(char ch, uint8_t column)
{
  uint8_t dataWrt = (ch & 0x7F) | 0x80; //data + WRT H
  gpio.write(P0, dataWrt);
  gpio.write(P1, (column & 0x03) << 6); //addr
  gpio.write(P0, ch & 0x7F);            //data + WRT L
  delayMicroseconds(1);
  gpio.write(P0, dataWrt); //data + WRT H
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
  gpio.configLedGPIO(P1, 0); //0->Led mode, 1->GPIO mode
  gpio.setLedsDim(DIM_MED);  //Medium led intensity
}

void loop()
{
  gpio.setLed(P1_6, brightness);

  // change the brightness for next time through the loop:
  brightness = brightness + fadeAmount;

  // reverse the direction of the fading at the ends of the fade:
  if (brightness <= 0 || brightness >= 255)
  {
    fadeAmount = -fadeAmount;
  }
  // wait for 20 milliseconds to see the dimming effect
  delay(20);
}
