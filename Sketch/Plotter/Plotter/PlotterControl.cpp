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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arduino.h>

#include <StepperLib.h>
#include <CNCLib.h>
#include "PlotterControl.h"
#include "MyControl.h"
#include "MyLcd.h"
#include "HPGLParser.h"

////////////////////////////////////////////////////////////

CPlotter::CPlotter()
{
	_isPenDown = false;
	_isPenDownTimeout = false;
	_isDelayPen = false;
	_pen = 0;
	_havePen = false;
}

////////////////////////////////////////////////////////////

void CPlotter::Init()
{
}

////////////////////////////////////////////////////////////

void CPlotter::Initialized()
{
	PenUpNow();
}

////////////////////////////////////////////////////////////

void CPlotter::Idle(unsigned int /* idletime */)
{
	if (!CStepper::GetInstance()->IsBusy() && (millis() - CStepper::GetInstance()->IdleTime()) > PLOTTER_PENUP_TIMEOUT)
	{
		if (_isDelayPen)
		{
			// abort delay but set position
			if (_isDelayPenDown)
			{
				_isPenDownTimeout = true;
			}
			PenUp();
		}
		if (_isPenDown)
		{
			_isPenDownTimeout = true;
			PenUp();
		}
	}
}

////////////////////////////////////////////////////////////

void CPlotter::Resume(bool restorePenDown)
{
	if (IsResumePenDown())
	{
		_isPenDownTimeout = false;
		if (restorePenDown)
			PenDown();
	}
}

////////////////////////////////////////////////////////////

void CPlotter::PenUp()
{
	_isDelayPen = false;
	if (_isPenDown)
	{
		PenUpNow();
	}
}

////////////////////////////////////////////////////////////

void CPlotter::PenUpNow()
{
	CStepper::GetInstance()->Wait(1);
	_isPenDown = false;
	MoveToPenPosition(
		CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, movepenupFeedrate)),
		ConvertConfigPos(CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, penuppos)), Z_AXIS));

#ifdef MYUSE_LCD
	// Lcd.DrawRequest(true,CLcd::DrawAll); => delay off movementbuffer
#endif
}


////////////////////////////////////////////////////////////

void CPlotter::PenDown()
{
	_isDelayPen = false;
	if (!_isPenDown)
	{
		_isPenDown = true;
		MoveToPenPosition(
			CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, movependownFeedrate)),
			ConvertConfigPos(CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, pendownpos)), Z_AXIS));

		CStepper::GetInstance()->Wait(1);
#ifdef MYUSE_LCD
		// Lcd.DrawRequest(true,CLcd::DrawAll); => delay off movementbuffer
		Lcd.DrawRequest(CLcd::DrawForceAll);
#endif
	}
}

////////////////////////////////////////////////////////////

void CPlotter::DelayPenNow()
{
	if (_isDelayPen)
	{
		_isDelayPen = false;
		if (_isDelayPenDown)
			PenDown();
		else
			PenUp();
	}
}

////////////////////////////////////////////////////////////

bool CPlotter::MoveToPenPosition(feedrate_t feedrate, mm1000_t pos)
{
#if PENTYPE == PENTYPE_ZAXIS      // Z-AXIS

  CMotionControlBase::GetInstance()->MoveAbsEx(
    feedrate,
    Z_AXIS, pos,
    -1);

  return !CStepper::GetInstance()->IsError();

#elif PENTYPE == PENTYPE_SERVO    // servo

  CStepper::GetInstance()->IoControl(CControl::Servo2, (short) pos);
  CStepper::GetInstance()->Wait(feedrate / 10);

  return true;

#else

error

#endif
}

////////////////////////////////////////////////////////////

bool CPlotter::ToPenChangePos(uint8_t pen)
{
	mm1000_t ofs_x = pen * CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, penchangepos_x_ofs));
	mm1000_t ofs_y = pen * CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, penchangepos_y_ofs));;
	
	CMotionControlBase::GetInstance()->MoveAbsEx(
		CHPGLParser::_state.FeedRateUp,
		X_AXIS, ConvertConfigPos(CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, penchangepos_x)), X_AXIS) + ofs_x,
		Y_AXIS, ConvertConfigPos(CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, penchangepos_y)), Y_AXIS) + ofs_y,
		-1);

	if (CStepper::GetInstance()->IsError())
		return false;

	return MoveToPenPosition(
			CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, movepenchangeFeedrate)),
			ConvertConfigPos(CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, penchangepos_z)), Z_AXIS));
}

////////////////////////////////////////////////////////////

bool CPlotter::OffPenChangePos(uint8_t pen)
{
	return MoveToPenPosition(
			CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, movepenchangeFeedrate)),
		ConvertConfigPos(CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, penuppos)), Z_AXIS));
}

////////////////////////////////////////////////////////////

bool CPlotter::SetPen(uint8_t pen)
{
	if (_pen == pen && _havePen)
		return true;

	if (!PenToDepot())
		return false;

	return PenFromDepot(pen);
}

////////////////////////////////////////////////////////////

bool CPlotter::PenToDepot()
{
	if (!_havePen)
		return true;

	PenUp();
	CStepper::GetInstance()->WaitBusy();

	/////////////////////////////////////
	// TODO: 

	if (!ToPenChangePos(_pen))
		return false;

	CStepper::GetInstance()->IoControl(CControl::Servo1, CConfigEeprom::GetConfigU16(offsetof(CMyControl::SMyCNCEeprom, penchangeServoClampOpenPos)));
	CStepper::GetInstance()->Wait(CConfigEeprom::GetConfigU16(offsetof(CMyControl::SMyCNCEeprom, penchangeServoClampOpenDelay)) / 10);

	//_servo1.write(CConfigEeprom::GetConfigU16(offsetof(CMyControl::SMyCNCEeprom, penchangeServoClampOpenPos)));
	//delay(CConfigEeprom::GetConfigU16(offsetof(CMyControl::SMyCNCEeprom, penchangeServoClampOpenDelay)));

	OffPenChangePos(_pen);

	////////////////////////////////////

	_havePen = false;
	return true;
}

////////////////////////////////////////////////////////////

bool CPlotter::PenFromDepot(uint8_t pen)
{

	/////////////////////////////////////
	// TODO: 

	if (!ToPenChangePos(pen))
		return false;

	CStepper::GetInstance()->IoControl(CControl::Servo1, CConfigEeprom::GetConfigU16(offsetof(CMyControl::SMyCNCEeprom, penchangeServoClampClosePos)));
	CStepper::GetInstance()->Wait(CConfigEeprom::GetConfigU16(offsetof(CMyControl::SMyCNCEeprom, penchangeServoClampClosePos))/10);

//	_servo1.write(CConfigEeprom::GetConfigU16(offsetof(CMyControl::SMyCNCEeprom, penchangeServoClampClosePos)));
//	delay(CConfigEeprom::GetConfigU16(offsetof(CMyControl::SMyCNCEeprom, penchangeServoClampCloseDelay)));

	OffPenChangePos(pen);

	////////////////////////////////////
	
	CStepper::GetInstance()->WaitBusy();

	_pen = pen;
	_havePen = true;
	return true;
}

////////////////////////////////////////////////////////////

mm1000_t CPlotter::ConvertConfigPos(mm1000_t pos, axis_t axis)
{
#if PENTYPE == PENTYPE_SERVO    // servo
	if (axis == Z_AXIS)
	{
		return pos;
	}
#endif

	if (pos >= 0x7fff000l)
	{
		eepromofs_t ofs = sizeof(CConfigEeprom::SCNCEeprom::SAxisDefinitions)*axis;
		pos = CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, axis[0].size) + ofs);
	}
	return pos;
}

