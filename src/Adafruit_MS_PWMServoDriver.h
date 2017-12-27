/*
	Forked & Ported to ESP32 (esp-idf) by ShellAddicted
	GitHub: https://github.com/ShellAddicted

	based on:
	https://github.com/adafruit/Adafruit_Motor_Shield_V2_Library
*/
/*************************************************** 
  This is a library for our Adafruit 16-channel PWM & Servo driver

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/815

  These displays use I2C to communicate, 2 pins are required to  
  interface.

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#ifndef _Adafruit_MS_PWMServoDriver_H
#define _Adafruit_MS_PWMServoDriver_H

#include <cmath> // std::floor
#include "driver/i2c.h" //ESP32

#define PCA9685_SUBADR1 0x2
#define PCA9685_SUBADR2 0x3
#define PCA9685_SUBADR3 0x4

#define PCA9685_MODE1 0x0
#define PCA9685_PRESCALE 0xFE

#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9

#define ALLLED_ON_L 0xFA
#define ALLLED_ON_H 0xFB
#define ALLLED_OFF_L 0xFC
#define ALLLED_OFF_H 0xFD

class Adafruit_MS_PWMServoDriver {
 public:
  Adafruit_MS_PWMServoDriver(uint8_t addr = 0x40, i2c_port_t i2c_num = I2C_NUM_0);
  void begin(void);
  void reset(void);
  void setPWMFreq(float freq);
  esp_err_t setPWM(uint8_t num, uint16_t on, uint16_t off);

 private:
  uint8_t _i2caddr;
  i2c_port_t _i2c_num;

  uint8_t read8(uint8_t addr);
  esp_err_t write8(uint8_t addr, uint8_t d);
};

#endif
