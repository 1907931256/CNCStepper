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

#include "StreamReader.h"
#include <ConfigurationStepperLib.h>
#include <MessageStepperLib.h>
#include "MessageCNCLib.h"

////////////////////////////////////////////////////////

typedef float expr_t;

////////////////////////////////////////////////////////
//
// Base class for command parser/Scanner
//
class CParser
{
public:

	CParser(CStreamReader* reader, Stream* output)
	{
		_reader    = reader;
		_output    = output;
		_error     = nullptr;
		_OkMessage = nullptr;
	};

	void ParseCommand();

	bool    IsError() const { return _error != nullptr; };
	error_t GetError() const { return _error; }

	typedef void (*PrintOKMessage)();
	PrintOKMessage GetOkMessage() const { return _OkMessage; }

	CStreamReader* GetReader() const { return _reader; }
	Stream*        GetOutput() const { return _output; }

	static void Init() { }

protected:

	////////////////////////////////////////////////////////

	virtual void Parse() = 0;
	virtual bool InitParse() { return true; }; // begin parsing of a command (override for prechecks), return true to continue
	virtual void CleanupParse() { }; // called after call to Parse()

	bool         CheckError();
	virtual char SkipSpacesOrComment() { return _reader->SkipSpaces(); }
	void         SkipCommentSingleLine() { _reader->MoveToEnd(); };

	bool ExpectEndOfCommand();
	void ErrorNotImplemented() { Error(MESSAGE(MESSAGE_PARSER_NotImplemented)); }
	void ErrorNotANumber() { Error(MESSAGE(MESSAGE_PARSER_NotANumber)); };
	void ErrorNumberRange() { Error(MESSAGE(MESSAGE_PARSER_OverrunOfNumber)); };
	void Error() { Error(MESSAGE_UNKNOWNERROR); }

	////////////////////////////////////////////////////////

	void ErrorAdd(error_t error) { if (!IsError()) Error(error); }

	void Error(error_t error)
	{
		_error = error;
		_reader->MoveToEnd();
	}

	void Info(error_t s1)
	{
		if (_output)
		{
			_output->print(MESSAGE_INFO);
			_output->println(s1);
		}
	}

	void Warning(error_t s1)
	{
		if (_output)
		{
			_output->print(MESSAGE_WARNING);
			_output->println(s1);
		}
	}

	Stream*        _output;
	CStreamReader* _reader;
	error_t        _error;
	PrintOKMessage _OkMessage;

public:

	long   GetInt32Scale(long minvalue, long maxvalue, uint8_t scale, uint8_t maxscale); // get "float" e.g. 1.234 => 1234 or 12 => 12000, limit with scale
	expr_t GetDouble();

	uint8_t        GetUInt8();
	uint16_t GetUInt16();
	uint32_t  GetUInt32();
	char           GetInt8();
	short          GetInt16();
	long           GetInt32();
	sdist_t        GetSDist();

	static bool IsUInt(char ch) { return CStreamReader::IsDigit(ch); }
	static bool IsInt(char  ch) { return CStreamReader::IsMinus(ch) || CStreamReader::IsDigit(ch); }

protected:

	//////////////////////////////////////////////////////
	// Textsearch  (=Tokens)

	// Content of "b": 
	//	  \001	=> any digit (last command char)
	//	  \002	=> space or end or not digit (last comand char), e.g. g0\002 will find g0 and not g01
	//	  \003	=> \000 (end of line)

	bool IsToken(FLSTR        b, bool       expectdel, bool ignorecase);
	bool TryToken(FLSTR       b, bool       expectdel, bool ignorecase) { return TryToken(_reader->GetBuffer(), b, expectdel, ignorecase); }
	bool TryToken(const char* buffer, FLSTR b, bool         expectdel, bool ignorecase); // scan from different location, but do not remove it

	bool TryToken(const char* buffer, FLSTR b, bool ignorecase); // same as stricmp (with Progmem)	

	//////////////////////////////////////////////////////

private:

	static char unsigned ConvertChar(const char ch, bool ignorecase) { return ignorecase ? CStreamReader::Toupper(ch) : ch; }

	bool TryToken(const char* buffer, FLSTR b, bool expectdel, bool ignorecase, const char*& nextchar); // scan, but do not remove it

	////////////////////////////////////////////////////////

private:

	template <class T> T GetUInt()
	{
		if (!CStreamReader::IsDigit(_reader->GetChar()))
		{
			ErrorNotANumber();
			return 0;
		}
		T value = 0;
		while (CStreamReader::IsDigit(_reader->GetChar()))
		{
			T old = value;
			value *= T(10);
			value += _reader->GetChar() - '0';
			if (old > value)
			{
				ErrorNumberRange();
				return 0;
			}
			_reader->GetNextChar();
		}
		return value;
	}

	template <class T> T GetInt()
	{
		bool negativ;
		if ((negativ = CStreamReader::IsMinus(_reader->GetChar())) != 0)
			_reader->GetNextChar();
		auto value = T(GetUInt<T>());
		return negativ ? -value : value;
	}

	template <class T> T GetUInt(T minvalue, T maxvalue)
	{
		T value = GetUInt<T>();
		if (value < minvalue) Error(MESSAGE_PARSER_ValueLessThanMin);
		else if (value > maxvalue) Error(MESSAGE_PARSER_ValueGreaterThanMax);
		return value;
	}

	template <class T> T GetInt(T minvalue, T maxvalue)
	{
		T value = GetInt<T>();
		if (value < minvalue) Error(MESSAGE_PARSER_ValueLessThanMin);
		else if (value > maxvalue) Error(MESSAGE_PARSER_ValueGreaterThanMax);
		return value;
	}
};

////////////////////////////////////////////////////////
