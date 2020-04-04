/**************************************************************************/
/*!
  @file     AW9523B.cpp

  Author: Manuel Polo (https://about.me/mrmx)
  License: MIT (see LICENSE)
*/
/**************************************************************************/

#ifdef __AVR_ATtiny85__
#include <TinyWireM.h>
#define Wire TinyWireM
#else
#include <Wire.h>
#endif

#include "AW9523B.h"

#ifdef __AVR__
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#elif defined(ARDUINO_ARCH_SAMD)
// nothing special needed
#elif defined(ARDUINO_SAM_DUE)
#define PROGMEM
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define Wire Wire1
#endif

#if (ARDUINO >= 100)
#include <Arduino.h> // capital A so it is error prone on case-sensitive filesystems
// Macro to deal with the difference in I2C write functions from old and new Arduino versions.
#define _I2C_WRITE write ///< Modern I2C write
#define _I2C_READ read   ///< Modern I2C read
#else
#include <WProgram.h>
#define _I2C_WRITE send   ///< Legacy I2C write
#define _I2C_READ receive ///< Legacy I2C read
#endif

#define _REG(port, reg) (port == P0 ? reg : reg + 1)

AW9523B::AW9523B(uint8_t address)
{
    _addr = AW9523B_I2C_ADDRESS + (address & 0x03);
}

boolean AW9523B::begin()
{
    Wire.begin();
    return readI2C(AW9523B_REG_ID) == AW9523B_ID;
}

void AW9523B::configInOut(AW9523BPort port, uint8_t inout)
{
    writeI2C(_REG(port, AW9523B_P0_CONF_STATE), inout);
}

void AW9523B::configLedGPIO(AW9523BPort port, uint8_t ledGpio)
{
    writeI2C(_REG(port, AW9523B_P0_LED_MODE), ledGpio);
}

void AW9523B::setPort0Mode(AW9523BPortMode mode)
{
    writeI2C(AW9523B_REG_GLOB_CTR, mode | ledsDim);
}

void AW9523B::setLedsDim(AW9523BLedsDim dim)
{
    writeI2C(AW9523B_REG_GLOB_CTR, dim | portMode);
}

void AW9523B::setLed(AW9523BLedDimCtrl led, uint8_t value)
{
    writeI2C(led, value);
}

void AW9523B::portIn(AW9523BPort port)
{
    _portIn = _REG(port, AW9523B_P0_IN_STATE);
}

void AW9523B::portOut(AW9523BPort port)
{
    _portOut = _REG(port, AW9523B_P0_OUT_STATE);
}

uint8_t AW9523B::read()
{
    return readI2C(_portIn);
}

uint8_t AW9523B::read(AW9523BPort port)
{
    return readI2C(AW9523B_P0_IN_STATE + port);
}

uint8_t AW9523B::write(uint8_t data)
{
    return writeI2C(_portOut, data);
}

uint8_t AW9523B::write(AW9523BPort port, uint8_t data)
{
    return writeI2C(AW9523B_P0_OUT_STATE + port, data);
}

void AW9523B::reset()
{
    writeI2C(AW9523B_REG_SOFT_RST, 0);
}

/**************************************************************************/
/*!
    @brief  Write a byte to an I2C register
    @param reg Register address
    @param val Value to write
    @return I2C transmission result 
*/
/**************************************************************************/
uint8_t AW9523B::writeI2C(uint8_t reg, uint8_t val)
{
    DEBUG_PRINT("writeI2C reg ")
    DEBUG_PRINT(reg, HEX)
    DEBUG_PRINT(" -> ") DEBUG_PRINTLN(val, HEX)
    Wire.beginTransmission(this->_addr);
    Wire._I2C_WRITE((byte)reg);
    Wire._I2C_WRITE((byte)val);
    return Wire.endTransmission();
}

/**************************************************************************/
/*!
    @brief  Read a byte from an I2C register
    @param reg Register address
    @return Register value
*/
/**************************************************************************/
uint8_t AW9523B::readI2C(uint8_t reg)
{
    Wire.beginTransmission(this->_addr);
    Wire._I2C_WRITE((byte)reg);
    Wire.endTransmission();
    Wire.requestFrom(this->_addr, (byte)1);
    return Wire._I2C_READ();
}