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

//#define STEPPERTYPE 1		// CStepperL298N
//#define STEPPERTYPE 2		// CStepperSMC800
//#define STEPPERTYPE 3		// CStepperTB6560
#define STEPPERTYPE 4		// CStepperRamps14

//#define LCD_TYPE  1         // LCD_2004_LiquidCrystal_I2C
#define LCD_TYPE  2         // LCD_12864_u8g

#define PENTYPE_ZAXIS 0
#define PENTYPE_SERVO 1

#define PENTYPE PENTYPE_SERVO

////////////////////////////////////////////////////////

#define USBBAUDRATE 115200

////////////////////////////////////////////////////////

#define EPROM_SIGNATURE_PLOTTER 0x21438701

////////////////////////////////////////////////////////

#define MYUSE_LCD

////////////////////////////////////////////////////////

#define FROMPENTYPE(a,b) (PENTYPE==PENTYPE_ZAXIS ? (a) : (b))

////////////////////////////////////////////////////////

#define X_MAXSIZE 510000		//515mm
#define Y_MAXSIZE 295000		//295mm
#define Z_MAXSIZE 8000			//8
#define A_MAXSIZE 360000
#define B_MAXSIZE 360000
#define C_MAXSIZE 360000

////////////////////////////////////////////////////////

#define STEPPERDIRECTION 0		// set bit to invert direction of each axis

#define STEPSPERROTATION	200
#define MICROSTEPPING		16

// 3200steps/rot, T2.5 belt pully with 12teeth

#define TOOTH 12
#define TOOTHSIZE 2.5
#define SCREWLEAD (TOOTH*TOOTHSIZE)

#define X_STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)
#define Y_STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)
#define Z_STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)
#define A_STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)
#define B_STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)
#define C_STEPSPERMM ((STEPSPERROTATION*MICROSTEPPING)/SCREWLEAD)

////////////////////////////////////////////////////////

#define CNC_MAXSPEED	25000        // steps/sec
#define CNC_ACC			400
#define CNC_DEC			450
#define CNC_JERKSPEED	1000

////////////////////////////////////////////////////////
// NoReference, ReferenceToMin, ReferenceToMax

#define X_USEREFERENCE	EReverenceType::ReferenceToMin
#define Y_USEREFERENCE	EReverenceType::ReferenceToMin
#define Z_USEREFERENCE	FROMPENTYPE(EReverenceType::ReferenceToMin,EReverenceType::NoReference)
#define A_USEREFERENCE	EReverenceType::NoReference
#define B_USEREFERENCE	EReverenceType::NoReference
#define C_USEREFERENCE	EReverenceType::NoReference

#define REFMOVE_1_AXIS  Z_AXIS
#define REFMOVE_2_AXIS  Y_AXIS
#define REFMOVE_3_AXIS  X_AXIS
#define REFMOVE_4_AXIS  255
#define REFMOVE_5_AXIS  255
#define REFMOVE_6_AXIS  255

#define X_REFERENCEHITVALUE_MIN LOW
#define Y_REFERENCEHITVALUE_MIN LOW
#define Z_REFERENCEHITVALUE_MIN LOW
#define A_REFERENCEHITVALUE_MIN LOW
#define B_REFERENCEHITVALUE_MIN LOW
#define C_REFERENCEHITVALUE_MIN LOW

#define X_REFERENCEHITVALUE_MAX LOW
#define Y_REFERENCEHITVALUE_MAX LOW
#define Z_REFERENCEHITVALUE_MAX LOW
#define A_REFERENCEHITVALUE_MAX LOW
#define B_REFERENCEHITVALUE_MAX LOW
#define C_REFERENCEHITVALUE_MAX LOW

#define MOVEAWAYFROMREF_MM1000 500

#define SPINDLE_ANALOGSPEED
#define SPINDLE_MAXSPEED		10000		// analog 255
#define SPINDLE_FADETIMEDELAY  0    		// 8ms * 255 => 2040ms from 0 to max, 4080 from -max to +max

////////////////////////////////////////////////////////

#if STEPPERTYPE==1
#include "Configuration_L298N.h"
#elif STEPPERTYPE==2
#include "Configuration_SMC800.h"
#elif STEPPERTYPE==3
#include "Configuration_TB6560.h"
#elif STEPPERTYPE==4

#include "Configuration_Ramps14.h"

#endif

////////////////////////////////////////////////////////

#define STEPRATE_REFMOVE	(CNC_MAXSPEED/4)

////////////////////////////////////////////////////////
// PlotterControl

#define SERVO1_PIN RAMPS14_SERVO3_PIN
#define SERVO1_CLAMPOPEN		1000
#define SERVO1_CLAMPCLOSE		2000
#define SERVO1_CLAMPOPENDELAY	1000
#define SERVO1_CLAMPCLOSEDELAY	1000

#define PLOTTER_DEFAULT_PENUP_FEEDRATE			LONG_MAX	 // reduced to maxsteprate
#define PLOTTER_DEFAULT_PENDOWN_FEEDRATE		3600000l     // 60 mm/ sec;

#if PENTYPE == PENTYPE_ZAXIS			// Z-AXIS

#define PLOTTER_DEFAULT_Z_PENUP_FEEDRATE		2400000l // 40 mm / sec
#define PLOTTER_DEFAULT_Z_PENDOWN_FEEDRATE		1800000l // 30 mm / sec
#define PLOTTER_DEFAULT_Z_PENCHANGE_FEEDRATE	2400000l // 40 mm / sec

#define PLOTTER_PENDOWNPOS_Z		LONG_MAX
#define PLOTTER_PENUPPOS_Z			(Z_MAXSIZE/2)
#define PLOTTER_PENCHANGEPOS_Z		0

#elif PENTYPE == PENTYPE_SERVO		// servo

#define SERVO2_PIN RAMPS14_SERVO2_PIN

// feedrate are used as delays (in ms)
#define PLOTTER_DEFAULT_Z_PENUP_FEEDRATE		200
#define PLOTTER_DEFAULT_Z_PENDOWN_FEEDRATE		200
#define PLOTTER_DEFAULT_Z_PENCHANGE_FEEDRATE	200

// servo positions (micro sec)
#define PLOTTER_PENDOWNPOS_Z		2000
#define PLOTTER_PENUPPOS_Z			1500
#define PLOTTER_PENCHANGEPOS_Z		1000

#else

error;

#endif

#define PLOTTER_PENCOUNT		8

#define PLOTTER_PENCHANGEPOS_X		LONG_MAX
#define PLOTTER_PENCHANGEPOS_X_OFS	0
#define PLOTTER_PENCHANGEPOS_Y		0
#define PLOTTER_PENCHANGEPOS_Y_OFS	30000l	//mm1000

#define PLOTTER_PENUP_TIMEOUT		1000

////////////////////////////////////////////////////////

#define MESSAGE_MYCONTROL_Starting F("Plotter(HA) is starting ... (" __DATE__ ", " __TIME__ ")")
