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

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#include <HelpParser.h>

#include "MyMotionControl.h"
#include "MyParser.h"

////////////////////////////////////////////////////////////

bool CMyParser::MCommand(mcode_t mcode)
{
	if (super::MCommand(mcode))
		return true;

	switch (mcode)
	{
		case 116: _OkMessage = PrintInfo;
			return true;
		case 117: M117Command();
			return true;
		case 118: M118Command();
			return true;
	}

	return false;
}

////////////////////////////////////////////////////////////

void CMyParser::PrintInfo()
{
	//	PrintPosition();
	static_cast<CMyMotionControl*>(CMotionControlBase::GetInstance())->PrintInfo();
}

////////////////////////////////////////////////////////////

bool CMyParser::GetAxisAbs(SAxisMove& move)
{
	for (char ch = _reader->SkipSpacesToUpper(); ch; ch = _reader->SkipSpacesToUpper())
	{
		axis_t axis;
		if ((axis = CharToAxis(ch)) < NUM_AXIS) GetAxis(axis, move, AbsolutWithZeroShiftPosition);
		else break;

		if (CheckError()) { return false; }
	}

	return move.axes != 0;
}

////////////////////////////////////////////////////////

void CMyParser::M117Command()
{
	SAxisMove move(false);

	if (GetAxisAbs(move))
	{
		auto pMC = static_cast<CMyMotionControl*>(CMotionControlBase::GetInstance());
		pMC->MoveAngle(move.newpos);
	}
}

////////////////////////////////////////////////////////////

void CMyParser::M118Command()
{
	SAxisMove move(false);

	if (GetAxisAbs(move))
	{
		CMyMotionControl* pMC = (CMyMotionControl*)CMotionControlBase::GetInstance();
		pMC->MoveAngleLog(move.newpos);
	}
}
