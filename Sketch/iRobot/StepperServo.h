////////////////////////////////////////////////////////
/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2017 Herbert Aitenbichler

  CNCLib is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  CNCLib is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  http://www.gnu.org/licenses/
*/
////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////

#include "Stepper.h"
#include <Adafruit_TiCoServo.h>

#define Servo Adafruit_TiCoServo

#define MG995_MIN_PULSE_WIDTH       600 
#define MG995_MAX_PULSE_WIDTH		2000
#define MG995_MAX_PULSE_DIFF		(MG995_MAX_PULSE_WIDTH-MG995_MIN_PULSE_WIDTH)
#define MG995_MAX_ROTATION			M_PI
#define MG995_MsForPI				((MG995_MAX_PULSE_DIFF*M_PI/MG995_MAX_ROTATION))
#define MG995_CENTER				(MG995_MIN_PULSE_WIDTH+(MG995_MAX_PULSE_WIDTH-MG995_MIN_PULSE_WIDTH)/2)

// Servo Pins => do not change because of hardware PWM of Adafruit_TiCoServo
#define MG995_SERVO1_PIN			5
#define MG995_SERVO2_PIN			6
#define MG995_SERVO3_PIN			7
#define MG995_SERVO4_PIN			8

////////////////////////////////////////////////////////

#define SERVO_POSITION_SCALE		2   // for rounding

#define CENTER_LIMIT  (MG995_CENTER*SERVO_POSITION_SCALE)
#define MIN_LIMIT     (MG995_MIN_PULSE_WIDTH*SERVO_POSITION_SCALE)
#define MAX_LIMIT     (MG995_MAX_PULSE_WIDTH*SERVO_POSITION_SCALE)
#define MAX_ROTATION  MG995_MAX_ROTATION
#define MsForPI       (MG995_MsForPI*SERVO_POSITION_SCALE)

#define INIT_PULS1		(1740*SERVO_POSITION_SCALE)
#define INIT_PULS2		(1230*SERVO_POSITION_SCALE)
#define INIT_PULS3		CENTER_LIMIT



////////////////////////////////////////////////////////

class CStepperServo : public CStepper
{
private:
	typedef CStepper super;
public:

	CStepperServo();
	virtual void Init() override;

	void SetServo();   

protected:

	virtual void  SetEnable(axis_t axis, uint8_t level, bool force) override;
	virtual uint8_t GetEnable(axis_t axis) override;
	virtual void  Step(const uint8_t cnt[NUM_AXIS], uint8_t directionUp) override;

public:

	virtual uint8_t GetReferenceValue(uint8_t referenceid) override;
	virtual bool IsAnyReference() override;

////////////////////////////////////////////////////////

private:

  Servo		_servo[NUM_AXIS];
  udist_t	_lastPos[NUM_AXIS];

};
