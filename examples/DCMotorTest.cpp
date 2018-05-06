/*
	Forked & Ported to ESP32 (esp-idf) by ShellAddicted
	GitHub: https://github.com/ShellAddicted

	based on:
	https://github.com/adafruit/Adafruit_Motor_Shield_V2_Library
*/

#include "esp_log.h"
#include "Adafruit_MotorShield.h"

static const char *TAG = "AFMSv2Test";

extern "C" void app_main(void){
	// Setup I²C bus 
	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = GPIO_NUM_21;
	conf.scl_io_num = GPIO_NUM_22;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = 100000;
	i2c_param_config(I2C_NUM_0, &conf);
	i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
	
	// Create the motor shield object with the default I2C address
	Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x60, I2C_NUM_0); 
	// Or, create it with a different I2C address (say for stacking)
	// Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x61); 

	// Select which 'port' M1, M2, M3 or M4. In this case, M1
	Adafruit_DCMotor *myMotor = AFMS.getMotor(1);
	AFMS.begin();  // create with the default frequency 1.6KHz
	//AFMS.begin(1000);  // OR with a different frequency, say 1KHz
  
	// Set the speed to start, from 0 (off) to 255 (max speed)
	myMotor->setSpeed(150);
	myMotor->run(FORWARD);
	// turn on motor
	myMotor->run(RELEASE);

	while (1){
		uint8_t i;
  
		ESP_LOGI(TAG, "tick");

		myMotor->run(FORWARD);
		for (i=0; i<255; i++) {
			myMotor->setSpeed(i);  
			vTaskDelay(10 / portTICK_PERIOD_MS);
		}

		for (i=255; i!=0; i--) {
			myMotor->setSpeed(i);  
			vTaskDelay(10 / portTICK_PERIOD_MS);
		}

		ESP_LOGI(TAG, "tock");

		myMotor->run(BACKWARD);

		for (i=0; i<255; i++) {
			myMotor->setSpeed(i);  
			vTaskDelay(10 / portTICK_PERIOD_MS);
		}
		
		for (i=255; i!=0; i--) {
			myMotor->setSpeed(i);  
			vTaskDelay(10 / portTICK_PERIOD_MS);
		}

		ESP_LOGI(TAG, "tech");
		myMotor->run(RELEASE);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

