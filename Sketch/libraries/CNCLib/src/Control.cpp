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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#include "Control.h"
#include "Lcd.h"

#include "GCodeParser.h"
#include "ConfigEeprom.h"

////////////////////////////////////////////////////////////

template<> CControl* CSingleton<CControl>::_instance = NULL;

////////////////////////////////////////////////////////

CControl::CControl()
{
	_bufferidx = 0;
}

////////////////////////////////////////////////////////////

void CControl::Init()
{
	CStepper::GetInstance()->Init();
	CStepper::GetInstance()->AddEvent(StaticStepperEvent, (uintptr_t) this, _oldStepperEvent);

#ifdef _USE_LCD
	
	if (CLcd::GetInstance())
		CLcd::GetInstance()->Init();

#endif
	
	if (_timeBlink==0)
		CHAL::pinModeOutput(BLINK_LED);

	CHAL::InitTimer0(HandleInterrupt);
	CHAL::StartTimer0(IDLETIMER0VALUE);
}

////////////////////////////////////////////////////////////

void CControl::Initialized()
{
	StepperSerial.println(MESSAGE_OK_INITIALIZED);
	GoToReference();
	CMotionControlBase::GetInstance()->SetPositionFromMachine();
}

////////////////////////////////////////////////////////////

void CControl::InitFromEeprom()
{
	CStepper::GetInstance()->SetDirection(CConfigEeprom::GetConfigU8(offsetof(CConfigEeprom::SCNCEeprom, stepperdirections)));

#ifdef REDUCED_SIZE
	CMotionControlBase::GetInstance()->InitConversionStepsPer(CConfigEeprom::GetConfigFloat(offsetof(CConfigEeprom::SCNCEeprom, StepsPerMm1000)));
#else
	CMotionControlBase::GetInstance()->InitConversionBestStepsPer(CConfigEeprom::GetConfigFloat(offsetof(CConfigEeprom::SCNCEeprom, StepsPerMm1000)));
#endif

	uint16_t jerkspeed = CConfigEeprom::GetConfigU16(offsetof(CConfigEeprom::SCNCEeprom, jerkspeed));
	if (jerkspeed == 0) jerkspeed = 1024;
	
	CStepper::GetInstance()->SetDefaultMaxSpeed(
		((steprate_t)CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, maxsteprate))),
		((steprate_t)CConfigEeprom::GetConfigU16(offsetof(CConfigEeprom::SCNCEeprom, acc))),
		((steprate_t)CConfigEeprom::GetConfigU16(offsetof(CConfigEeprom::SCNCEeprom, dec))),
		jerkspeed);

	for (uint8_t axis = 0; axis < NUM_AXIS; axis++)
	{
		eepromofs_t ofs = sizeof(CConfigEeprom::SCNCEeprom::SAxisDefinitions)*axis;
		CStepper::GetInstance()->SetReferenceHitValue(CStepper::GetInstance()->ToReferenceId(axis, true),  CConfigEeprom::GetConfigU8(offsetof(CConfigEeprom::SCNCEeprom, axis[0].referenceValue_min) + ofs));
		CStepper::GetInstance()->SetReferenceHitValue(CStepper::GetInstance()->ToReferenceId(axis, false), CConfigEeprom::GetConfigU8(offsetof(CConfigEeprom::SCNCEeprom, axis[0].referenceValue_max) + ofs));

		CStepper::GetInstance()->SetLimitMax(axis, CMotionControlBase::GetInstance()->ToMachine(axis, CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, axis[0].size) + ofs)));

#ifndef REDUCED_SIZE

		steprate_t steprate = CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, axis[0].maxsteprate) + ofs);
		if (steprate != 0) CStepper::GetInstance()->SetMaxSpeed(axis, steprate);

		steprate = CConfigEeprom::GetConfigU16(offsetof(CConfigEeprom::SCNCEeprom, axis[0].acc) + ofs);
		if (steprate != 0) CStepper::GetInstance()->SetAcc(axis, steprate);

		steprate = CConfigEeprom::GetConfigU16(offsetof(CConfigEeprom::SCNCEeprom, axis[0].dec) + ofs);
		if (steprate != 0) CStepper::GetInstance()->SetDec(axis, steprate);

		float stepsperMM1000 = CConfigEeprom::GetConfigFloat(offsetof(CConfigEeprom::SCNCEeprom, axis[0].StepsPerMm1000) + ofs);
		if (stepsperMM1000 != 0.0)
		{
			CMotionControlBase::GetInstance()->SetConversionStepsPerEx();
			CMotionControlBase::GetInstance()->SetConversionStepsPerEx(axis, stepsperMM1000);
		}
#endif
	}
}

////////////////////////////////////////////////////////////

uint8_t CControl::ConvertSpindleSpeedToIO8(unsigned short maxspeed, unsigned short level) 
{ 
	if (level >= maxspeed) return 255;
	return (uint8_t)MulDivU32(level, 255, maxspeed); 
}

////////////////////////////////////////////////////////////

void CControl::GoToReference()
{
	for (axis_t i = 0; i < NUM_AXIS; i++)
	{
		axis_t axis = CConfigEeprom::GetConfigU8(offsetof(CConfigEeprom::SCNCEeprom, axis[0].refmoveSequence) + sizeof(CConfigEeprom::SCNCEeprom::SAxisDefinitions)*i);
		if (axis < NUM_AXIS)
		{
			GoToReference(axis);
		}
	}
}

////////////////////////////////////////////////////////////

bool CControl::GoToReference(axis_t axis)
{
	EnumAsByte(EReverenceType) referenceType = (EReverenceType)CConfigEeprom::GetConfigU8(offsetof(CConfigEeprom::SCNCEeprom, axis[0].referenceType) + sizeof(CConfigEeprom::SCNCEeprom::SAxisDefinitions)*axis);
	if (referenceType == EReverenceType::NoReference) 
		return false;

	GoToReference(axis, 0, referenceType == EReverenceType::ReferenceToMin);
	return true;
}

////////////////////////////////////////////////////////////

bool CControl::GoToReference(axis_t axis, steprate_t steprate, bool toMinRef)
{
	if (steprate == 0)
	{
		steprate = (steprate_t) CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, refmovesteprate));
	}
	// goto min/max
	return CStepper::GetInstance()->MoveReference(
		axis, CStepper::GetInstance()->ToReferenceId(axis, toMinRef), toMinRef, steprate,0, 
		CMotionControlBase::GetInstance()->ToMachine(axis,CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, moveAwayFromRefernece))));
}

////////////////////////////////////////////////////////////

void CControl::Kill()
{
	// may be in ISR context, do not print anything
	CStepper::GetInstance()->EmergencyStop();
	CMotionControlBase::GetInstance()->SetPositionFromMachine();
}

////////////////////////////////////////////////////////

void CControl::Resurrect()
{
	CStepper::GetInstance()->EmergencyStopResurrect();
	CMotionControlBase::GetInstance()->SetPositionFromMachine();

#ifdef _USE_LCD
	
	if (CLcd::GetInstance())
		CLcd::GetInstance()->ClearDiagnostic();

#endif

	_bufferidx = 0;
	StepperSerial.println(MESSAGE_OK_RESURRECT);
}

////////////////////////////////////////////////////////////

void CControl::StopProgram(bool /*checkconditional*/)
{
}

////////////////////////////////////////////////////////////

void CControl::Idle(unsigned int /*idletime*/)
{
}

////////////////////////////////////////////////////////////

void CControl::Hold()
{
	CStepper::GetInstance()->PauseMove();
}

////////////////////////////////////////////////////////////

void CControl::Resume()
{
	CStepper::GetInstance()->ContinueMove();
}

////////////////////////////////////////////////////////////

void CControl::Poll()
{
#ifdef _USE_LCD
	if (CLcd::GetInstance())
		CLcd::GetInstance()->Poll();
#endif
}

////////////////////////////////////////////////////////////

bool CControl::Parse(CStreamReader* reader, Stream* output)
{
	CGCodeParserDefault gcode(reader, output);
	return ParseAndPrintResult(&gcode, output);
}

////////////////////////////////////////////////////////////

bool CControl::ParseAndPrintResult(CParser *parser, Stream* output)
{
	bool ret = true;

	///////////////////////////////////////////////////////////////////////////
	// send OK after Parse

	parser->ParseCommand();

	if (parser->IsError())
	{
		if (output) 
		{
			PrintError(output);
			output->print(parser->GetError());
			output->print(MESSAGE_CONTROL_RESULTS);
			output->print(_buffer);
//			output->print(millis());
		}
		ret = false;
	}
	else
	{
		// => not in "else" if "OK" should be sent after "Error:"
		if (output) output->print(MESSAGE_OK);
		if (parser->GetOkMessage() != NULL)
		{
			if (output)
			{
				output->print(F(" "));
			}
			parser->GetOkMessage()();
		}
	}

	if (output) output->println();

	return ret;
}

////////////////////////////////////////////////////////////

bool CControl::Command(char* buffer, Stream* output)
{
#ifdef _USE_LCD

	if (CLcd::GetInstance())
		CLcd::GetInstance()->Command(buffer);

#endif

	if (IsKilled())
	{
#ifndef REDUCED_SIZE
		if (IsResurrectCommand(buffer))		// restart with "!!!"
		{
			Resurrect();
			return true;
		}
#endif
		if (output)
		{
			PrintError(output); 
			output->println(MESSAGE_CONTROL_KILLED);
// => uncomment if "OK" will not be sent after "Error:"
//			output->println(MESSAGE_OK);
		}
		return false;
	}

	// if one Parse failes, return false
	
	bool ret = true;
	
	_reader.Init(buffer);

	if (_reader.GetChar())
	{
		while (_reader.GetChar())
		{
			if (!Parse(&_reader,output))
				ret = false;
		}
	}
	else if (output)
	{
		// send OK on empty line (command)
		output->println(MESSAGE_OK_EMPTYLINE);
	}
	
	return ret;
}

////////////////////////////////////////////////////////////

bool CControl::IsEndOfCommandChar(char ch)
{
	//return ch == '\n' || ch == '\r' || ch == -1;
	return ch == '\n' || ch == (char) -1;
}

////////////////////////////////////////////////////////////

void CControl::ReadAndExecuteCommand(Stream* stream, Stream* output, bool filestream)
{
	// call this methode if ch is available in stream

	if (stream->available() > 0)
	{
		while (stream->available() > 0)
		{
			char ch = _buffer[_bufferidx] = stream->read();

			if (IsEndOfCommandChar(ch))
			{
				_buffer[_bufferidx] = 0;			// remove from buffer 
				Command(_buffer, output);
				_bufferidx = 0;

				_lasttime = millis();

				return;
			}

			_bufferidx++;
			if (_bufferidx >= sizeof(_buffer))
			{
				if (output)
				{
					PrintError(output); output->println(MESSAGE_CONTROL_FLUSHBUFFER);
				}
				_bufferidx = 0;
			}
/*
			if (ch == '\x18')
			{
				StepperSerial.println(MESSAGE_CTRLX);
				_bufferidx = 0;
			}
*/
		}

		if (filestream)						// e.g. SD card => execute last line without "EndOfLine"
		{
			if (_bufferidx > 0)
			{
				_buffer[_bufferidx + 1] = 0;
				Command(_buffer,output);
				_bufferidx = 0;
			}
		}
		_lasttime = millis();
	}
}

////////////////////////////////////////////////////////////

bool CControl::SerialReadAndExecuteCommand()
{
	if (StepperSerial.available() > 0)
	{
		ReadAndExecuteCommand(&StepperSerial, &StepperSerial, false);			
	}

	return _bufferidx > 0;		// command pending, buffer not empty
}

////////////////////////////////////////////////////////

void CControl::FileReadAndExecuteCommand(Stream* stream, Stream* output)
{
	ReadAndExecuteCommand(stream, output, true);
}

////////////////////////////////////////////////////////////

void CControl::Run()
{
	_bufferidx = 0;
	_lasttime = _timeBlink = _timePoll = 0;

	Init();
	Initialized();

#ifdef _MSC_VER
	while (!CGCodeParserBase::_exit)
#else
	while (true)
#endif
	{
		if (IsHold())
		{
			CheckIdlePoll(true);
		}
		else
		{
			while (SerialReadAndExecuteCommand())
			{
				// wait until serial command processed
				CheckIdlePoll(true);
			}

			CheckIdlePoll(true);

			ReadAndExecuteCommand();
		}
	}
}

////////////////////////////////////////////////////////////

void CControl::CheckIdlePoll(bool isidle)
{
	unsigned long time = millis();

	if (isidle && _lasttime + TIMEOUTCALLIDEL < time)
	{
		Idle(time - _lasttime);
		Poll();
		_timePoll = time;
	}
	else if(_timePoll + TIMEOUTCALLPOLL < time)
	{
		Poll();
		_timePoll = time;
	}

	if (_timeBlink < time)
	{
		HALFastdigitalWrite(BLINK_LED, CHAL::digitalRead(BLINK_LED) == HIGH ? LOW : HIGH);
		_timeBlink = time + TIMEOUTBLINK;
	}
}

////////////////////////////////////////////////////////////

void CControl::ReadAndExecuteCommand()
{
	// override for alternative command source e.g. File
}

////////////////////////////////////////////////////////////

bool CControl::PostCommand(const __FlashStringHelper* cmd, Stream* output)
{
// use _buffer to execute command

	const char* cmd1 = (const char*)cmd;
	uint8_t idx = _bufferidx;
	uint8_t idxprogmem = 0;

	for (;idx<sizeof(_buffer); idx++, idxprogmem++)
	{
		_buffer[idx] = pgm_read_byte(&cmd1[idxprogmem]);

		if (_buffer[idx] == 0)
		{
			return Command(&_buffer[_bufferidx], output);
		}
	}
	
	// do not fit in buffer
	return false;
}

////////////////////////////////////////////////////////////

bool CControl::PostCommand(char* cmd, Stream* output)
{
	return Command(cmd, output);
}

////////////////////////////////////////////////////////////

void CControl::TimerInterrupt()
{
	CHAL::EnableInterrupts();	// enable irq for timer1 (Stepper)

	if (!IsKilled())
	{
		if (IsKill())
		{
			Kill();
		}
	}

#ifdef _USE_LCD

	if (CLcd::GetInstance())
		CLcd::GetInstance()->TimerInterrupt();

#endif
}

////////////////////////////////////////////////////////////

void CControl::Delay(unsigned long ms)
{
	unsigned long expected_end = millis() + ms;

	while (expected_end > millis())
	{
#ifdef _USE_LCD
		if (CLcd::GetInstance())
			CLcd::GetInstance()->Poll();
#endif
	}
}

////////////////////////////////////////////////////////////

bool CControl::StaticStepperEvent(CStepper* /*stepper*/, uintptr_t param, EnumAsByte(CStepper::EStepperEvent) eventtype, uintptr_t addinfo)
{ 
	return ((CControl*)param)->StepperEvent(eventtype, addinfo);
}


////////////////////////////////////////////////////////////

bool CControl::StepperEvent(EnumAsByte(CStepper::EStepperEvent) eventtype, uintptr_t addinfo)
{
	if (CallOnEvent(eventtype, addinfo))
		return true;

	return _oldStepperEvent.Call(CStepper::GetInstance(), eventtype, addinfo);
}

////////////////////////////////////////////////////////////

bool CControl::CallOnEvent(uint8_t eventtype, uintptr_t param)
{
	return OnEvent((EnumAsByte(EStepperControlEvent)) eventtype, param);
}

////////////////////////////////////////////////////////////

bool CControl::OnEvent(EnumAsByte(EStepperControlEvent) eventtype, uintptr_t addinfo)
{
	switch (eventtype)
	{
		case OnWaitEvent:

			if (CStepper::WaitTimeCritical > (CStepper::EWaitType) (unsigned int)addinfo)
			{
				CheckIdlePoll(false);
			}
			break;

		case OnIoEvent:
			
			IOControl(((CStepper::SIoControl*) addinfo)->_tool, ((CStepper::SIoControl*) addinfo)->_level);
			break;
	}
	return true;
}
