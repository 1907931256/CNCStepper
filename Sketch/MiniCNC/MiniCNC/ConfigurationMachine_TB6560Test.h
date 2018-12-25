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

#define USBBAUDRATE 250000

////////////////////////////////////////////////////////

#include "ConfigurationMachine_Default.h"

////////////////////////////////////////////////////////

#define STEPPERDIRECTION 0
//#define STEPPERDIRECTION (1 << X_AXIS) + (1 << Y_AXIS)    // set bit to invert direction of each axis

#define STEPSPERROTATION	200
#define MICROSTEPPING		8
#define SCREWLEAD			1.0

////////////////////////////////////////////////////////

#define CNC_MAXSPEED 14000
#define CNC_ACC  350
#define CNC_DEC  400
#define CNC_JERKSPEED	500

////////////////////////////////////////////////////////

#include "ConfigurationStepper_TB6560.h"

////////////////////////////////////////////////////////

#define CONTROLLERFAN_ONTIME	10000			// switch off controllerfan if idle for 10 Sec
#define CONTROLLERFAN_FAN_PIN	14 // AD0

#define CONTROLLERFAN_DIGITAL_ON  HIGH
#define CONTROLLERFAN_DIGITAL_OFF LOW

////////////////////////////////////////////////////////

#define SPINDLE_ENABLE_PIN	11

#define SPINDLE_DIGITAL_ON  LOW
#define SPINDLE_DIGITAL_OFF HIGH

////////////////////////////////////////////////////////

#define PROBE_PIN	12

#define PROBE_PIN_ON  LOW
#define PROBE_PIN_OFF HIGH

////////////////////////////////////////////////////////

#undef KILL_PIN

#define KILL_ON  LOW
#define KILL_OFF HIGH

////////////////////////////////////////////////////////

#define MESSAGE_MYCONTROL_VERSION          F(", TB6560")
