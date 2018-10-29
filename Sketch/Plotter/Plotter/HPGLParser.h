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

#include <Parser.h>

////////////////////////////////////////////////////////

class CHPGLParser : public CParser
{
private:

	typedef CParser super;

public:

	CHPGLParser(CStreamReader* reader, Stream* output) : CParser(reader, output) { };

	virtual void Parse() override;

	static mm1000_t HPGLToMM1000X(int32_t xx);
	static mm1000_t HPGLToMM1000Y(int32_t yy);

	static void Init()
	{
		super::Init();
		_state.Init();
	}

	struct SState
	{
		bool _HPGLIsAbsolut;
		bool dummy;

		int _HPOffsetX;
		int _HPOffsetY;

		feedrate_t FeedRate;
		feedrate_t FeedRateUp;
		feedrate_t FeedRateDown;

		// Plotter

		void Init()
		{
			_HPGLIsAbsolut = true;

			_HPOffsetX = 0;
			_HPOffsetY = 0;

			SetFeedRates();
		}

		void SetFeedRates()
		{
			FeedRateUp   = CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, penupFeedrate));
			FeedRateUp   = CMotionControlBase::GetInstance()->GetMaxFeedRate(X_AXIS, FeedRateUp);
			FeedRateUp   = -FeedRateUp;
			FeedRateDown = CConfigEeprom::GetConfigU32(offsetof(CMyControl::SMyCNCEeprom, pendownFeedrate));
		}
	};

	static SState _state;

private:

	void ReadAndSkipSemicolon();

	void SelectPenCommand();
	void PenVelocityCommand();
	void PenVelocityNormalCommand();

	void IgnoreCommand();
	void InitCommand();
	void PenMoveCommand(uint8_t cmdIdx);
};

////////////////////////////////////////////////////////
