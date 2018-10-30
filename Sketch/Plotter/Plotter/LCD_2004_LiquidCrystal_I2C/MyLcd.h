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

//#define FORCE_NO_LCD

////////////////////////////////////////////////////////

#if  defined(FORCE_NO_LCD) || defined(__AVR_ATmega328P__) || defined(__SAM3X8E__)

#else

#define MYUSE_LCD

////////////////////////////////////////////////////////

#include <LiquidCrystal_I2C.h>
#include <Lcd.h>

////////////////////////////////////////////////////////

#define MYLCD_ROWS	4
#define MYLCD_COLS	20

////////////////////////////////////////////////////////

class CMyLcd : public CLcd
{
private:

	typedef CLcd super;

public:

	virtual void Init() override;
	virtual void TimerInterrupt() override;

	virtual void Beep(const SPlayTone*, bool) override { };

protected:

	virtual unsigned long Draw(EDrawType draw) override;
	virtual unsigned long Splash() override;
	void                  FirstDraw();

private:

	void DrawPos(uint8_t col, uint8_t row, mm1000_t pos);
	void DrawES(uint8_t  col, uint8_t row, bool     es);

	void DrawPen(uint8_t col, uint8_t row);
};

////////////////////////////////////////////////////////

extern CMyLcd Lcd;

#endif
