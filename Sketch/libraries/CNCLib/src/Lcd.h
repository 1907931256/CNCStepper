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

#include <StepperLib.h>
#include "CNCLibTypes.h"
#include "Beep.h"

////////////////////////////////////////////////////////

class CControl;

////////////////////////////////////////////////////////

class CLcd : public CSingleton<CLcd>
{
	friend CControl;

public:

	CLcd()
	{
	}

	enum EDrawType
	{
		DrawFirst,
		// draw after splash
		DrawForceAll,
		// draw now
		DrawAll // draw with timeout
	};

	virtual void Init();
	virtual void DrawRequest(EDrawType draw);

	void Invalidate(); // draw with next timeout

	////////////////////////////////////////////////////////////

	static void InvalidateLcd()
	{
#ifdef _USE_LCD

		if (CLcd::GetInstance())
			CLcd::GetInstance()->Invalidate();
#endif
	}

protected:

	virtual void Poll();
	virtual void TimerInterrupt();

	virtual void Command(char* cmd);

	virtual unsigned long Draw(EDrawType draw) = 0; // return => timeout for next draw

	virtual unsigned long Splash() = 0; // return time to display

	bool IsSplash() const { return _splash; };

public:

	virtual void Beep(const SPlayTone*, bool fromProgmem) =0;

	void OKBeep() { Beep(SPlayTone::PlayOK, true); }
	void ErrorBeep() { Beep(SPlayTone::PlayError, true); }

	bool PostCommand(EnumAsByte(EGCodeSyntaxType) syntaxtype, FLSTR cmd, Stream* output = NULL);
	bool PostCommand(char*                        cmd, Stream*      output              = NULL);

	virtual uint8_t InitPostCommand(EnumAsByte(EGCodeSyntaxType) syntaxtype, char* cmd);

private:

	unsigned long _nextdrawtime = 0;

	bool _splash     = false;
	bool _invalidate = false;

public:

	void Diagnostic(FLSTR diag) { _diagnostics = diag; }

	bool  IsDiagnostic() { return _diagnostics != NULL; };
	FLSTR GetDiagnostic() { return _diagnostics; }
	void  ClearDiagnostic() { _diagnostics = NULL; }

private:

	FLSTR _diagnostics = NULL;
};

////////////////////////////////////////////////////////
