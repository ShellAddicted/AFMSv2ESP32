/*
	Forked & Ported to ESP32 (esp-idf) by ShellAddicted
	GitHub: https://github.com/ShellAddicted

	based on:
	https://github.com/adafruit/Adafruit_Motor_Shield_V2_Library
*/

/******************************************************************
 This is the library for the Adafruit Motor Shield V2 
 It supports DC motors & Stepper motors with microstepping as well
 as stacking-support. It is *not* compatible with the V1 library!

 It will only work with https://www.adafruit.com/products/1483
 
 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source hardware
 by purchasing products from Adafruit!
 
 Written by Limor Fried/Ladyada for Adafruit Industries.
 BSD license, check license.txt for more information.
 All text above must be included in any redistribution.
 ******************************************************************/

#include "Adafruit_MotorShield.h"
#include "Adafruit_MS_PWMServoDriver.h"

#if (MICROSTEPS == 8)
uint8_t microstepcurve[] = {0, 50, 98, 142, 180, 212, 236, 250, 255};
#elif (MICROSTEPS == 16)
uint8_t microstepcurve[] = {0, 25, 50, 74, 98, 120, 141, 162, 180, 197, 212, 225, 236, 244, 250, 253, 255};
#endif

Adafruit_MotorShield::Adafruit_MotorShield(uint8_t addr, i2c_port_t i2c_num) {
	_addr = addr;
	_pwm = Adafruit_MS_PWMServoDriver(_addr, i2c_num);
}

void Adafruit_MotorShield::begin(uint16_t freq){
	// init PWM w/_freq
	_pwm.begin();
	_freq = freq;
	_pwm.setPWMFreq(_freq); // This is the maximum PWM frequency
	for (uint8_t i=0; i<16; i++){
		_pwm.setPWM(i, 0, 0);
	}
}

void Adafruit_MotorShield::setPWM(uint8_t pin, uint16_t value) {
	if (value > 4095){
		_pwm.setPWM(pin, 4096, 0);
	}
	else{
		_pwm.setPWM(pin, 0, value);
	}
}

void Adafruit_MotorShield::setPin(uint8_t pin, bool value) {
	if (value == LOW){
		_pwm.setPWM(pin, 0, 0);
	}
	else{
		_pwm.setPWM(pin, 4096, 0);
	}
}

Adafruit_DCMotor *Adafruit_MotorShield::getMotor(uint8_t num) {
	if (num > 4){
		return NULL;
	}

	num--;

	if (dcmotors[num].motornum == 0) {
		// not init'd yet!
		dcmotors[num].motornum = num;
		dcmotors[num].MC = this;

		if (num == 0) {
			dcmotors[num].PWMpin = 8;
			dcmotors[num].IN1pin = 10;
			dcmotors[num].IN2pin = 9;
		}

		else if (num == 1) {
			dcmotors[num].PWMpin = 13;
			dcmotors[num].IN1pin = 11;
			dcmotors[num].IN2pin = 12;
		}

		else if (num == 2) {
			dcmotors[num].PWMpin = 2;
			dcmotors[num].IN1pin = 4;
			dcmotors[num].IN2pin = 3;
		}

		else if (num == 3) {
			dcmotors[num].PWMpin = 7;
			dcmotors[num].IN1pin = 5;
			dcmotors[num].IN2pin = 6;
		}
	}
	return &dcmotors[num];
}


Adafruit_StepperMotor *Adafruit_MotorShield::getStepper(uint16_t steps, uint8_t num) {
	if (num > 2){
		return NULL;
	}

	num--;

	if (steppers[num].steppernum == 0) {
		// not init'd yet!
		steppers[num].steppernum = num;
		steppers[num].revsteps = steps;
		steppers[num].MC = this;

		if (num == 0) {
			steppers[num].PWMApin = 8;
			steppers[num].AIN2pin = 9;
			steppers[num].AIN1pin = 10;
			steppers[num].PWMBpin = 13;
			steppers[num].BIN2pin = 12;
			steppers[num].BIN1pin = 11;
		}

		else if (num == 1) {
			steppers[num].PWMApin = 2;
			steppers[num].AIN2pin = 3;
			steppers[num].AIN1pin = 4;
			steppers[num].PWMBpin = 7;
			steppers[num].BIN2pin = 6;
			steppers[num].BIN1pin = 5;
		}
	}
	return &steppers[num];
}


/******************************************
							 MOTORS
******************************************/

Adafruit_DCMotor::Adafruit_DCMotor(void) {
	MC = NULL;
	motornum = 0;
	PWMpin = IN1pin = IN2pin = 0;
}

void Adafruit_DCMotor::run(uint8_t cmd) {
	switch (cmd) {
	case FORWARD:
		MC->setPin(IN2pin, LOW);  // take low first to avoid 'break'
		MC->setPin(IN1pin, HIGH);
		break;
	case BACKWARD:
		MC->setPin(IN1pin, LOW);  // take low first to avoid 'break'
		MC->setPin(IN2pin, HIGH);
		break;
	case RELEASE:
		MC->setPin(IN1pin, LOW);
		MC->setPin(IN2pin, LOW);
		break;
	}
}

void Adafruit_DCMotor::setSpeed(uint8_t speed) {
	MC->setPWM(PWMpin, speed*16);
}

/******************************************
							 STEPPERS
******************************************/

Adafruit_StepperMotor::Adafruit_StepperMotor(void) {
	revsteps = steppernum = currentstep = 0;
}

void Adafruit_StepperMotor::setSpeed(uint16_t rpm) {
	usperstep = 60000000 / ((uint32_t)revsteps * (uint32_t)rpm);
}

void Adafruit_StepperMotor::release(void) {
	MC->setPin(AIN1pin, LOW);
	MC->setPin(AIN2pin, LOW);
	MC->setPin(BIN1pin, LOW);
	MC->setPin(BIN2pin, LOW);
	MC->setPWM(PWMApin, 0);
	MC->setPWM(PWMBpin, 0);
}

void Adafruit_StepperMotor::step(uint16_t steps, uint8_t dir,  uint8_t style) {
	uint32_t uspers = usperstep;

	if (style == INTERLEAVE) {
		uspers /= 2;
	}
	else if (style == MICROSTEP) {
		uspers /= MICROSTEPS;
		steps *= MICROSTEPS;
	}

	while (steps--) {
		onestep(dir, style);
		ets_delay_us(uspers); // TODO: Change ets_delay_us() with something better (ets_delay_us burns cpu cycles)
	}
}

uint8_t Adafruit_StepperMotor::onestep(uint8_t dir, uint8_t style) {
	uint8_t ocra, ocrb;
	ocra = ocrb = 255;

	// next determine what sort of stepping procedure we're up to
	if (style == SINGLE) {
		if ((currentstep/(MICROSTEPS/2)) % 2){ // we're at an odd step, weird
			if (dir == FORWARD) {
				currentstep += MICROSTEPS/2;
			}
			else {
				currentstep -= MICROSTEPS/2;
			}
		}
		else {           // go to the next even step
			if (dir == FORWARD) {
				currentstep += MICROSTEPS;
			}
			else {
				currentstep -= MICROSTEPS;
			}
		}
	}
	else if (style == DOUBLE) {
		if (! (currentstep/(MICROSTEPS/2) % 2)) { // we're at an even step, weird
			if (dir == FORWARD) {
				currentstep += MICROSTEPS/2;
			}
			else {
				currentstep -= MICROSTEPS/2;
			}
		}
		else {           // go to the next odd step
			if (dir == FORWARD) {
				currentstep += MICROSTEPS;
			}
			else {
				currentstep -= MICROSTEPS;
			}
		}
	}
	else if (style == INTERLEAVE) {
		if (dir == FORWARD) {
			 currentstep += MICROSTEPS/2;
		}
		else {
			 currentstep -= MICROSTEPS/2;
		}
	}

	if (style == MICROSTEP) {
		if (dir == FORWARD) {
			currentstep++;
		}
		else {
			// BACKWARDS
			currentstep--;
		}

		currentstep += MICROSTEPS*4;
		currentstep %= MICROSTEPS*4;

		ocra = ocrb = 0;
		if (currentstep < MICROSTEPS) {
			ocra = microstepcurve[MICROSTEPS - currentstep];
			ocrb = microstepcurve[currentstep];
		}
		else if  ( (currentstep >= MICROSTEPS) && (currentstep < MICROSTEPS*2)) {
			ocra = microstepcurve[currentstep - MICROSTEPS];
			ocrb = microstepcurve[MICROSTEPS*2 - currentstep];
		}
		else if  ( (currentstep >= MICROSTEPS*2) && (currentstep < MICROSTEPS*3)) {
			ocra = microstepcurve[MICROSTEPS*3 - currentstep];
			ocrb = microstepcurve[currentstep - MICROSTEPS*2];
		}
		else if  ( (currentstep >= MICROSTEPS*3) && (currentstep < MICROSTEPS*4)) {
			ocra = microstepcurve[currentstep - MICROSTEPS*3];
			ocrb = microstepcurve[MICROSTEPS*4 - currentstep];
		}
	}

	currentstep += MICROSTEPS*4;
	currentstep %= MICROSTEPS*4;

	MC->setPWM(PWMApin, ocra*16);
	MC->setPWM(PWMBpin, ocrb*16);
	
	// release all
	uint8_t latch_state = 0; // all motor pins to 0

	if (style == MICROSTEP) {
		if (currentstep < MICROSTEPS){
			latch_state |= 0x03;
		}
		if ((currentstep >= MICROSTEPS) && (currentstep < MICROSTEPS*2)){
			latch_state |= 0x06;
		}
		if ((currentstep >= MICROSTEPS*2) && (currentstep < MICROSTEPS*3)){
			latch_state |= 0x0C;
		}
		if ((currentstep >= MICROSTEPS*3) && (currentstep < MICROSTEPS*4)){
			latch_state |= 0x09;
		}
	}
	else {
		switch (currentstep/(MICROSTEPS/2)) {
		case 0:
			latch_state |= 0x1; // energize coil 1 only
			break;
		case 1:
			latch_state |= 0x3; // energize coil 1+2
			break;
		case 2:
			latch_state |= 0x2; // energize coil 2 only
			break;
		case 3:
			latch_state |= 0x6; // energize coil 2+3
			break;
		case 4:
			latch_state |= 0x4; // energize coil 3 only
			break; 
		case 5:
			latch_state |= 0xC; // energize coil 3+4
			break;
		case 6:
			latch_state |= 0x8; // energize coil 4 only
			break;
		case 7:
			latch_state |= 0x9; // energize coil 1+4
			break;
		}
	}

	if (latch_state & 0x1) {
		MC->setPin(AIN2pin, HIGH);
	}
	else {
		MC->setPin(AIN2pin, LOW);
	}

	if (latch_state & 0x2) {
		MC->setPin(BIN1pin, HIGH);
	}
	else {
		MC->setPin(BIN1pin, LOW);
	}

	if (latch_state & 0x4) {
		MC->setPin(AIN1pin, HIGH);
	}
	else {
		MC->setPin(AIN1pin, LOW);
	}

	if (latch_state & 0x8) {
		MC->setPin(BIN2pin, HIGH);
	}
	else {
		MC->setPin(BIN2pin, LOW);
	}
	return currentstep;
}

