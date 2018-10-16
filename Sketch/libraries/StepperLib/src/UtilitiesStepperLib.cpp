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

#ifdef _MSC_VER

#define _CRT_SECURE_NO_WARNINGS
#define _AFX_SECURE_NO_WARNINGS 

#endif

//////////////////////////////////////////

#include "UtilitiesStepperLib.h"

////////////////////////////////////////////////////////

uint8_t ToPrecisionU10(uint16_t v)
{
	if (v < 1) return 0;
	if (v < 10) return 1;
	if (v < 100) return 2;
	if (v < 1000) return 3;
	if (v < 10000) return 4;
	return 5;
}

uint8_t ToPrecisionU10(uint32_t v)
{
	if (v < 10000) return ToPrecisionU10(uint16_t(v));
	if (v < 100000) return 5;
	if (v < 1000000) return 6;
	if (v < 10000000) return 7;
	if (v < 100000000) return 8;
	if (v < 1000000000) return 9;
	return 10;
}

uint8_t ToPrecisionS10(int32_t v)
{
	if (v < 0)
	{
		return ToPrecisionU10(uint32_t(-v));
	}
	return ToPrecisionU10(uint32_t(v));
}


////////////////////////////////////////////////////////

uint8_t ToPrecisionU2(uint16_t v)
{
	uint8_t i = 0;
	for (; v != 0; i++)
	{
		v /= 2;
	}
	return i;
}

uint8_t ToPrecisionU2(uint32_t v)
{
	uint8_t i = 0;
	for (; v != 0; i++)
	{
		v /= 2;
	}
	return i;
}

uint8_t ToPrecisionS2(int32_t v)
{
	if (v < 0)
	{
		return ToPrecisionU2((uint32_t)-v);
	}
	return ToPrecisionU2((uint32_t)v);
}

////////////////////////////////////////////////////////

uint32_t _ulsqrt_round(uint32_t val, bool round)
{
	uint32_t temp;
	uint32_t g     = 0;
	uint32_t b     = 0x8000;
	uint32_t bshft = 15;

	for (uint8_t i = 0; i < 15; i++)
	{
		temp = g;
		temp = temp + (b >> 1);
		temp = temp << (bshft + 1);
		if (val >= temp)
		{
			g += b;
			val -= temp;
		}
		bshft--;
		b = b >> 1;
	}
	temp = (g << 1);
	temp = temp | 1;
	if (val >= temp)
	{
		g += b;
		val -= temp;
	}

	if (round && val > g)
	{
		g++;
	}

	return g;
}

////////////////////////////////////////////////////////

uint32_t _ulsqrt_round(uint32_t val)
{
	return _ulsqrt_round(val, true);
}

////////////////////////////////////////////////////////

uint32_t _ulsqrt(uint32_t val)
{
	return _ulsqrt_round(val, false);
}
