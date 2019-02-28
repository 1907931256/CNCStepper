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
