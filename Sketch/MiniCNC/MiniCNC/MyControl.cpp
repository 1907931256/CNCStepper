/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) Herbert Aitenbichler

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
  to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/
////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#include <CNCLib.h>
#include <GCodeParser.h>

#include "MyControl.h"

////////////////////////////////////////////////////////////

CMyControl            Control;
CMotionControlDefault MotionControl;
CConfigEeprom         Eprom;
HardwareSerial&       StepperSerial = Serial;

////////////////////////////////////////////////////////////

#ifndef MYNUM_AXIS
#error Please define MYNUM_AXIS
#endif

////////////////////////////////////////////////////////////

const CConfigEeprom::SCNCEeprom CMyControl::_eepromFlash PROGMEM =
{
	EPROM_SIGNATURE,
	NUM_AXIS, MYNUM_AXIS, offsetof(CConfigEeprom::SCNCEeprom,Axis), sizeof(CConfigEeprom::SCNCEeprom::SAxisDefinitions),
	GetInfo1a(), GetInfo1b(),
	0,
	STEPPERDIRECTION, 0, 0,SPINDLE_FADETIMEDELAY,
	SPINDLE_MAXSPEED,
	CNC_JERKSPEED,
	CNC_MAXSPEED,
	CNC_ACC,
	CNC_DEC,
	STEPRATE_REFMOVE,
	MOVEAWAYFROMREF_MM1000,
	X_STEPSPERMM / 1000.0,
	{
		{ AXISPARAMETER(X_MAXSIZE, X_USEREFERENCE, REFMOVE_1_AXIS, X_REFERENCEHITVALUE_MIN, X_REFERENCEHITVALUE_MAX, 0,0,0,0,0,0.0,0) },
		{ AXISPARAMETER(Y_MAXSIZE, Y_USEREFERENCE, REFMOVE_2_AXIS, Y_REFERENCEHITVALUE_MIN, Y_REFERENCEHITVALUE_MAX, 0,0,0,0,0,0.0,0) },
		{ AXISPARAMETER(Z_MAXSIZE, Z_USEREFERENCE, REFMOVE_3_AXIS, Z_REFERENCEHITVALUE_MIN, Z_REFERENCEHITVALUE_MAX, 0,0,0,0,0,0.0,0) },
#if NUM_AXIS > 3
		{ AXISPARAMETER(A_MAXSIZE,     A_USEREFERENCE, REFMOVE_4_AXIS,  A_REFERENCEHITVALUE_MIN, A_REFERENCEHITVALUE_MAX, 0,0,0,0,0,0.0,0) },
#endif
#if NUM_AXIS > 4
		{ AXISPARAMETER(B_MAXSIZE,     B_USEREFERENCE, REFMOVE_5_AXIS,  B_REFERENCEHITVALUE_MIN, B_REFERENCEHITVALUE_MAX, 0,0,0,0,0,0.0,0) },
#endif
#if NUM_AXIS > 5
		{ AXISPARAMETER(C_MAXSIZE,     C_USEREFERENCE, REFMOVE_6_AXIS,  C_REFERENCEHITVALUE_MIN, C_REFERENCEHITVALUE_MAX, 0,0,0,0,0,0.0,0) },
#endif
	}
};

////////////////////////////////////////////////////////////

void CMyControl::Init()
{
	CSingleton<CConfigEeprom>::GetInstance()->Init(sizeof(CConfigEeprom::SCNCEeprom), &_eepromFlash, EPROM_SIGNATURE);

#ifdef DISABLELEDBLINK
	DisableBlinkLed();
#endif

	super::Init();

	InitFromEeprom();

	_data.Init();

	CGCodeParserDefault::InitAndSetFeedRate(-STEPRATETOFEEDRATE(GO_DEFAULT_STEPRATE), G1_DEFAULT_FEEDPRATE, STEPRATETOFEEDRATE(G1_DEFAULT_MAXSTEPRATE));

#ifdef MYUSE_LCD
	InitSD(SD_ENABLE_PIN);
#endif
}

////////////////////////////////////////////////////////////

void CMyControl::PrintVersion()
{
	super::PrintVersion();
	StepperSerial.println(MESSAGE_MYCONTROL_VERSION);
}

////////////////////////////////////////////////////////////

void CMyControl::IOControl(uint8_t tool, uint16_t level)
{
	if (!_data.IOControl(tool, level))
	{
		super::IOControl(tool, level);
	}
}

////////////////////////////////////////////////////////////

uint16_t CMyControl::IOControl(uint8_t tool)
{
	switch (tool)
	{
#ifndef REDUCED_SIZE
		case SpindleCW:
		case SpindleCCW: { return _data._spindle.IsOn(); }
		case Coolant: { return _data._coolant.IsOn(); }
		case ControllerFan: { return _data._controllerfan.GetLevel(); }
#endif
		case Probe: { return _data._probe.IsOn(); }
	}

	return super::IOControl(tool);
}

////////////////////////////////////////////////////////////

void CMyControl::Kill()
{
	super::Kill();
	_data.Kill();
}

////////////////////////////////////////////////////////////

bool CMyControl::IsKill()
{
	if (_data.IsKill())
	{
#ifdef MYUSE_LCD
		Lcd.Diagnostic(F("E-Stop"));
#endif
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////

void CMyControl::TimerInterrupt()
{
	super::TimerInterrupt();
	_data.TimerInterrupt();
}

////////////////////////////////////////////////////////////

void CMyControl::Initialized()
{
	super::Initialized();
	_data.Initialized();
}

////////////////////////////////////////////////////////////

void CMyControl::Poll()
{
	super::Poll();

	if (IsHold())
	{
		if (_data._resume.IsOn() || _data._holdresume.IsOn())
		{
			Resume();
#ifdef MYUSE_LCD
			Lcd.ClearDiagnostic();
#endif
		}
	}
	else if (_data._hold.IsOn() || _data._holdresume.IsOn())
	{
		Hold();
#ifdef MYUSE_LCD
		Lcd.Diagnostic(F("LCD Hold"));
#endif
	}
}

////////////////////////////////////////////////////////////

bool CMyControl::OnEvent(EnumAsByte(EStepperControlEvent) eventType, uintptr_t addInfo)
{
	_data.OnEvent(eventType, addInfo);
	return super::OnEvent(eventType, addInfo);
}
