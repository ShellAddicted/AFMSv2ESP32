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

#include "Adafruit_MS_PWMServoDriver.h"
#include <cmath>

i2c_ack_type_t ACK_EN = (i2c_ack_type_t)0x01;

Adafruit_MS_PWMServoDriver::Adafruit_MS_PWMServoDriver(uint8_t addr, i2c_port_t i2c_num) {
	_i2c_num = i2c_num;
	_i2caddr = addr;
}

void Adafruit_MS_PWMServoDriver::begin(void) {
	reset();
}

void Adafruit_MS_PWMServoDriver::reset(void) {
	write8(PCA9685_MODE1, 0x0, 0);
}

void Adafruit_MS_PWMServoDriver::setPWMFreq(float freq) {
	freq *= 0.9;  // Correct for overshoot in the frequency setting

	float prescaleval = 25000000;
	prescaleval /= 4096;
	prescaleval /= freq;
	prescaleval -= 1;
	uint8_t prescale = std::floor(prescaleval + 0.5);
	
	//set timeoutMS of write8() to 0 means DO NOT EXPECT ACK
	uint8_t oldmode = read8(PCA9685_MODE1);
	uint8_t newmode = (oldmode&0x7F) | 0x10; // sleep
	write8(PCA9685_MODE1, newmode,0); // go to sleep
	write8(PCA9685_PRESCALE, prescale,0); // set the prescaler
	write8(PCA9685_MODE1, oldmode,0);
	vTaskDelay(5/portTICK_PERIOD_MS);
	write8(PCA9685_MODE1, oldmode | 0xa1,0);  //  This sets the MODE1 register to turn on auto increment.
}

esp_err_t Adafruit_MS_PWMServoDriver::setPWM(uint8_t num, uint16_t on, uint16_t off) {
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (_i2caddr << 1) | I2C_MASTER_WRITE, ACK_EN);
	i2c_master_write_byte(cmd, LED0_ON_L+4*num, ACK_EN);
	i2c_master_write_byte(cmd, on, ACK_EN);
	i2c_master_write_byte(cmd, (on>>8), ACK_EN);
	i2c_master_write_byte(cmd, off, ACK_EN);
	i2c_master_write_byte(cmd, (off>>8), ACK_EN);
	i2c_master_stop(cmd);
	esp_err_t errx = i2c_master_cmd_begin(_i2c_num, cmd, 10/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);
	if (errx != ESP_OK){
		throw Adafruit_MS_PWMServoDriverException(errx);
	}
	return errx;
}

uint8_t Adafruit_MS_PWMServoDriver::read8(uint8_t reg){
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (_i2caddr << 1) | I2C_MASTER_WRITE, ACK_EN);
	i2c_master_write_byte(cmd, reg, ACK_EN);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(_i2c_num, cmd, 10/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);

	uint8_t val;
	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (_i2caddr << 1) | I2C_MASTER_READ, ACK_EN);
	i2c_master_read_byte(cmd, &val, ACK_EN);
	i2c_master_stop(cmd);
	esp_err_t errx = i2c_master_cmd_begin(_i2c_num, cmd, 10/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);
	if (errx != ESP_OK){
		throw Adafruit_MS_PWMServoDriverException(errx);
	}
	return val;
}

esp_err_t Adafruit_MS_PWMServoDriver::write8(uint8_t reg, uint8_t value, unsigned int timeoutMS){
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (_i2caddr << 1) | I2C_MASTER_WRITE, ACK_EN);
	i2c_master_write_byte(cmd, reg, ACK_EN);
	i2c_master_write_byte(cmd, value, ACK_EN);
	i2c_master_stop(cmd);
	esp_err_t errx = i2c_master_cmd_begin(_i2c_num, cmd, timeoutMS/portTICK_PERIOD_MS);
	i2c_cmd_link_delete(cmd);
	if (errx != ESP_OK){
		throw Adafruit_MS_PWMServoDriverException(errx);
	}
	return errx;
}
