////////////////////////////////////////////////////////
/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2019 Herbert Aitenbichler

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

#include "StepperL298N.h"

////////////////////////////////////////////////////////

// 1010 -> 1000 -> 1001 -> 0001 -> 0101 -> 0100 -> 0110 -> 0010
static const uint8_t _L298NHalfStep4Pin[8] PROGMEM = { 10, 8, 9, 1, 5, 4, 6, 2 };

// 1010 -> 1001 -> 0101 -> 0110
static const uint8_t _L298NFullStep4Pin[4] PROGMEM = { 10, 9, 5, 6 };
// static uint8_t _L298NFullStep4Pin[4] = { 1+2, 2+4, 4+8, 8+1 };

// 1010 -> 1001 -> 0101 -> 0110
// aAbB => a => !a=A 
static const uint8_t _L298NFullStep2Pin[4] PROGMEM = { 3, 2, 0, 1 };

////////////////////////////////////////////////////////

CStepperL298N::CStepperL298N() {}

////////////////////////////////////////////////////////

pin_t CStepperL298N::_pin[NUM_AXIS][4] =
{
	{ 2, 3, 4, 5 },
	{ 6, 7, 8, 9 },
	{ PIN_A0, PIN_A1, PIN_A2, PIN_A3 }, // A0-A3
#if NUM_AXIS > 3
#if defined(__AVR_ATmega328P__) || defined(__SAMD21G18A__)
	{ PIN_A4, PIN_A5, 12, 13 },					// A4&5,12&13, leave 11 for Spindle PWM, A6&A7 are input only!
#else
	{ PIN_A4, PIN_A5, PIN_A6, PIN_A7 }			// A4-A5
#endif
#endif
};

pin_t CStepperL298N::_pinEnable[NUM_AXIS] =
{
	10,
	10,
	10
#if NUM_AXIS > 3
	,10
#endif
	//	{}
};

pin_t CStepperL298N::_pinRef[NUM_AXIS * 2] =
{
	0 // 0 .. not used
};

////////////////////////////////////////////////////////

void CStepperL298N::Init()
{
	super::Init();

	for (unsigned char& stepIdx : _stepIdx)
	{
		stepIdx = 0;
	}
	_fullStepMode = false;

	uint8_t i;

	for (i = 0; i < NUM_AXIS; i++)
	{
		if (IsActive(i))
		{
			CHAL::pinModeOutput(_pin[i][0]);
			CHAL::pinModeOutput(_pin[i][1]);
			if (Is4Pin(i))
			{
				CHAL::pinModeOutput(_pin[i][2]);
				CHAL::pinModeOutput(_pin[i][3]);
			}

			if (IsUseEN1(i))
			{
				CHAL::pinModeOutput(_pinEnable[i]);
				//				CHAL::pinModeOutput(_pinEnable[i][0]);
				//				if (IsUseEN2(i)) CHAL::pinModeOutput(_pinEnable[i][1]);
			}
		}
	}

	for (i = 0; i < NUM_AXIS * 2; i++)
	{
		if (_pinRef[i] != 0)
		{
			CHAL::pinMode(_pinRef[i], INPUT_PULLUP);
		}
	}
}

////////////////////////////////////////////////////////

void CStepperL298N::Step(const uint8_t steps[NUM_AXIS], axisArray_t directionUp, bool)
{
	for (axis_t axis = 0; axis < NUM_AXIS; axis++)
	{
		if (steps[axis])
		{
			if (directionUp & 1)
			{
				_stepIdx[axis] += steps[axis];
			}
			else
			{
				_stepIdx[axis] -= steps[axis];
			}
			SetPhase(axis);
		}
		directionUp /= 2;
	}
}

////////////////////////////////////////////////////////

void CStepperL298N::SetEnable(axis_t axis, uint8_t level, bool force)
{
	if (IsActive(axis))
	{
		if (IsUseEN1(axis))
		{
			CHAL::digitalWrite(_pinEnable[axis], level != LevelOff ? HIGH : LOW);
			//			CHAL::digitalWrite(_pinEnable[axis][0], level != LevelOff ? HIGH : LOW);
			//			if (IsUseEN2(axis)) CHAL::digitalWrite(_pinEnable[axis][1], level != LevelOff ? HIGH : LOW);
		}
		else if (Is2Pin(axis))
		{
			// 2PIN and no enable => can't be turned off
		}
		else
		{
			if (level == LevelOff)
			{
				// 4 PIN => set all to off
				SetPhase(axis, 0);
			}
			else if (force)
			{
				SetPhase(axis);
			}
		}
	}
}

////////////////////////////////////////////////////////

uint8_t CStepperL298N::GetEnable(axis_t axis)
{
	if (!IsActive(axis)) return LevelOff;

	if (IsUseEN1(axis))
	{
		return ConvertLevel(CHAL::digitalRead(_pinEnable[axis]) != LOW);
		//		return ConvertLevel(CHAL::digitalRead(_pinEnable[axis][0]) != LOW &&
		//			(IsUseEN2(axis) ? CHAL::digitalRead(_pinEnable[axis][1]) != LOW : true));
	}

	if (Is2Pin(axis))
	{
		return LevelMax; // 2PIN and no enable => can't be turned off
	}

	// no enable PIN => with 4 PIN test if one PIN is set

	return ConvertLevel(
		CHAL::digitalRead(_pin[axis][0]) != LOW ||
		CHAL::digitalRead(_pin[axis][1]) != LOW ||
		CHAL::digitalRead(_pin[axis][2]) != LOW ||
		CHAL::digitalRead(_pin[axis][3]) != LOW);
}

////////////////////////////////////////////////////////

void CStepperL298N::SetPhase(axis_t axis)
{
	if (IsActive(axis))
	{
		uint8_t bitmask;

		if (Is4Pin(axis))
		{
			if (_fullStepMode)
			{
				bitmask = pgm_read_byte(&_L298NFullStep4Pin[_stepIdx[axis] & 0x3]);
			}
			else
			{
				bitmask = pgm_read_byte(&_L298NHalfStep4Pin[_stepIdx[axis] & 0x7]);
			}
		}
		else
		{
			// 2 pin, only full step
			bitmask = pgm_read_byte(&_L298NFullStep2Pin[_stepIdx[axis] & 0x3]);
		}

		SetPhase(axis, bitmask);
	}
}

////////////////////////////////////////////////////////

void CStepperL298N::SetPhase(axis_t axis, uint8_t bitmask)
{
	for (uint8_t i = 0; i < 4; i++, bitmask /= 2)
	{
		CHAL::digitalWrite(_pin[axis][i], bitmask & 1);
	}
}

////////////////////////////////////////////////////////

uint8_t CStepperL298N::GetReferenceValue(uint8_t referenceId)
{
	if (_pinRef[referenceId] != 0)
	{
		return CHAL::digitalRead(_pinRef[referenceId]);
	}

	return 255;
}

////////////////////////////////////////////////////////

bool CStepperL298N::IsAnyReference()
{
	for (axis_t axis2 = 0; axis2 < NUM_AXIS * 2; axis2++)
	{
		if (_pinRef[axis2] && CHAL::digitalRead(_pinRef[axis2]) == _pod._referenceHitValue[axis2])
		{
			return true;
		}
	}
	return false;
}
