////////////////////////////////////////////////////////
/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2018 Herbert Aitenbichler

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

#define CMyStepper CStepperCNCShield

////////////////////////////////////////////////////////

#define CNCSHIELD_NUM_AXIS MYNUM_AXIS
//#define CNCSHIELD_GBRL09

#include <Steppers/StepperCNCShield_pins.h>

// change some pin definition here:

#include <Steppers/StepperCNCShield.h>

////////////////////////////////////////////////////////

#ifdef CNCSHIELD_ABORT_PIN
#define KILL_PIN		CNCSHIELD_ABORT_PIN
#define KILL_PIN_ON		CNCSHIELD_ABORT_PIN_ON
#endif

#ifdef CNCSHIELD_HOLD_PIN
#define HOLD_PIN CNCSHIELD_HOLD_PIN
#define HOLD_PIN_ON CNCSHIELD_HOLD_PIN_ON
#endif

#ifdef CNCSHIELD_RESUME_PIN
#define RESUME_PIN CNCSHIELD_RESUME_PIN
#define RESUME_PIN_ON CNCSHIELD_RESUME_PIN_ON
#endif

#ifdef CNCSHIELD_PROBE_PIN
#define PROBE_PIN		CNCSHIELD_PROBE_PIN
#define PROBE_PIN_ON	CNCSHIELD_PROBE_PIN_ON
#endif

#ifdef CNCSHIELD_COOLANT_PIN
#define COOLANT_PIN		CNCSHIELD_COOLANT_PIN
#define COOLANT_PIN_ON	CNCSHIELD_COOLANT_PIN_ON
#define COOLANT_PIN_OFF	CNCSHIELD_COOLANT_PIN_OFF
#endif

#ifdef CNCSHIELD_SPINDLE_ENABLE_PIN
#define SPINDLE_ENABLE_PIN	CNCSHIELD_SPINDLE_ENABLE_PIN
#define SPINDLE_DIGITAL_ON	CNCSHIELD_SPINDLE_DIGITAL_ON
#define SPINDLE_DIGITAL_OFF	CNCSHIELD_SPINDLE_DIGITAL_OFF
#define SPINDLE_DIR_PIN		CNCSHIELD_SPINDLE_DIR_PIN
#define SPINDLE_DIR_CLW		CNCSHIELD_SPINDLE_DIR_CLW
#define SPINDLE_DIR_CCLW	CNCSHIELD_SPINDLE_DIR_CCLW
#endif

////////////////////////////////////////////////////////
