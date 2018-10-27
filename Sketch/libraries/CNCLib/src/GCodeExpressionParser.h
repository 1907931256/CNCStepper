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
*/
////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////

#include "ExpressionParser.h"
#include "GCodeParser.h"

////////////////////////////////////////////////////////
//
class CGCodeExpressionParser : public CExpressionParser
{
private:

	typedef CExpressionParser super;

public:

	CGCodeExpressionParser(CGCodeParser* parser) : super(parser->GetReader(), parser->GetOutput())
	{
		_gcodeParser      = parser;
		_LeftParenthesis  = '[';
		_RightParenthesis = ']';
	};

protected:

	CGCodeParser* _gcodeParser;

	virtual void ScanNextToken() override;
	virtual void ReadIdent() override;
	virtual bool IsIdentStart(char ch) override { return ch == '#' || super::IsIdentStart(ch); } // start of function or variable

	virtual bool EvalVariable(const char* var_name, expr_t& answer) override;
};

////////////////////////////////////////////////////////
