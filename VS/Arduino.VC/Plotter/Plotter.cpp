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

#include "stdafx.h"
#include <math.h>
#include "..\MsvcStepper\MsvcStepper.h"
#include "TestTools.h"
#include "..\..\..\Sketch\Plotter\Plotter\MyControl.h"
#include "..\..\..\Sketch\Plotter\Plotter\MyLcd.h"
#include "..\..\..\Sketch\Plotter\Plotter\PlotterControl.h"

#include <SPI.h>
#include <SD.h>

CSerial Serial;

static void setup();
static void loop();
static void Idle();

CMsvcStepper    MyStepper;
class CStepper& Stepper = MyStepper;
CMyControl      Control;
CPlotter        Plotter;
SDClass         SD;

int _tmain(int /* argc */, _TCHAR* /* argv*/[])
{
	setup();

#pragma warning(suppress:4127)
	while (!CGCodeParserBase::_exit)
	{
		loop();
	}

	MyStepper.EndTest();
}

void setup()
{
	MyStepper.DelayOptimization = false;
	MyStepper.UseSpeedSign      = true;
	MyStepper.CacheSize         = 50000;
	MyStepper.InitTest("Plotter.csv");
	Serial.SetIdle(Idle);
	CHAL::SetEepromFilename("Plotter.eeprom");
}

void loop()
{
	Control.Run();
}

static void Idle()
{
	MyStepper.HandleIdle();
}
