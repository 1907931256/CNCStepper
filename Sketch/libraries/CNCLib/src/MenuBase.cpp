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

#define _CRT_SECURE_NO_WARNINGS

////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Arduino.h>

#include "Control.h"
#include "GCodeBuilder.h"
#include "ConfigEeprom.h"
#include "MenuBase.h"

////////////////////////////////////////////////////////////

#ifndef REDUCED_SIZE

////////////////////////////////////////////////////////////

CMenuBase::MenuFunction CMenuBase::SMenuItemDef::GetButtonPress() const
{
#if defined(__AVR_ARCH__)

	struct ButtonFunctionWrapper
	{
		MenuFunction fnc;
	}x;

	memcpy_P(&x, &this->_buttonpress, sizeof(ButtonFunctionWrapper));

	return x.fnc;

#else

	return _buttonpress;

#endif
}

////////////////////////////////////////////////////////////

CMenuBase::menupos_t CMenuBase::GetMenuItemCount()
{
	return GetMenuDef()->GetItemCount();
}

////////////////////////////////////////////////////////////

FLSTR CMenuBase::GetItemText(menupos_t idx)
{
	return GetMenuDef()->GetItems()[idx].GetText();
}

////////////////////////////////////////////////////////////

FLSTR CMenuBase::GetText()
{
	return GetMenuDef()->GetText();
};

////////////////////////////////////////////////////////////

bool CMenuBase::Select(menupos_t idx)
{
	const SMenuItemDef* item = &GetMenuDef()->GetItems()[idx];
	MenuFunction        fnc  = item->GetButtonPress();
	if (fnc != NULL)
	{
		(this->*fnc)(item);
		return true;
	}

	return false;
}

////////////////////////////////////////////////////////////

void CMenuBase::MenuButtonPressSetMenu(const SMenuItemDef* def)
{
	auto newMenu = (const SMenuDef*)def->GetParam1();
	auto posMenu = (const SMenuDef*)def->GetParam2();
	SetMenu(newMenu);

	if (posMenu != nullptr)
	{
		// param2 != NULL => find index
		GetNavigator().SetPosition(newMenu->FindMenuIdx(uintptr_t(def), [](const SMenuItemDef* def, uintptr_t param) -> bool
		{
			return def->GetButtonPress() == &CMenuBase::MenuButtonPressSetMenu && // must be setMenu
				def->GetParam1() == ((const SMenuItemDef*)param)->GetParam2();    // param1 or new menu must be param2 of "Back from"
		}));
	}

	Changed();
}

////////////////////////////////////////////////////////////

void CMenuBase::MenuButtonPressMenuBack(const SMenuItemDef* def)
{
	auto newMenu = (const SMenuDef*)def->GetParam1();
	const struct SMenuDef* oldMenu = GetMenuDef();

	SetMenu(newMenu);

	GetNavigator().SetPosition(GetMenuDef()->FindMenuIdx(uintptr_t(oldMenu), [](const SMenuItemDef* def, uintptr_t oldMenu) -> bool
	{
		return def->GetParam1() == oldMenu && def->GetButtonPress() == &CMenuBase::MenuButtonPressSetMenu;
	}));

	Changed();
}

////////////////////////////////////////////////////////////

uint8_t CMenuBase::FindMenuIndexBack()
{
	return GetMenuDef()->FindMenuIdx(0, [](const SMenuItemDef* def, uintptr_t) -> bool
	{
		return def->GetButtonPress() == &CMenuBase::MenuButtonPressMenuBack;
	});
}

////////////////////////////////////////////////////////////

void CMenuBase::MenuButtonPressSetCommand(const SMenuItemDef* def)
{
	PostCommand(EGCodeSyntaxType::GCodeBasic, FLSTR(def->GetParam1()));
	CLcd::GetInstance()->OKBeep();
}

////////////////////////////////////////////////////////////

void CMenuBase::MenuButtonPressMove(const SMenuItemDef* def)
{
	auto axis = axis_t((unsigned int)GetMenuDef()->GetParam1());
	auto dist = uint8_t((unsigned int)def->GetParam1());

	if (dist == MoveHome)
	{
		MenuButtonPressHomeA(axis);
		return;
	}

	CGCodeBuilder builder;
	InitPostCommand(EGCodeSyntaxType::GCodeBasic, builder.GetCommand());

	builder.Add(F("g91 g0 "))
	       .AddAxisName(axis);

	switch (dist)
	{
		case MoveP100: builder.Add(F("100"));	break;
		case MoveP10:  builder.Add(F("10"));	break;
		case MoveP1:   builder.Add(F("1"));		break;
		case MoveP01:  builder.Add(F("0.1"));	break;
		case MoveP001: builder.Add(F("0.01"));	break;
		case MoveM001: builder.Add(F("-0.01"));	break;
		case MoveM01:  builder.Add(F("-0.1"));	break;
		case MoveM1:   builder.Add(F("-1"));	break;
		case MoveM10:  builder.Add(F("-10"));	break;
		case MoveM100: builder.Add(F("-100"));	break;
	}

	builder.Add(F(" g90"));

	PostCommand(builder.GetCommand());
}

////////////////////////////////////////////////////////////

void CMenuBase::MenuButtonPressRotate(const SMenuItemDef* def)
{
	auto req = uint8_t((unsigned int)def->GetParam1());

	switch (req)
	{
		case RotateClear:  PostCommand(EGCodeSyntaxType::GCode, F("g68.10"));break;
		case RotateOffset: PostCommand(EGCodeSyntaxType::GCode, F("g68.11"));break;
		case RotateSetYZ:  PostCommand(EGCodeSyntaxType::GCode, F("g68.13 j0k0"));break;
		case RotateSetX:   PostCommand(EGCodeSyntaxType::GCode, F("g68.14 i0"));break;
		default: CLcd::GetInstance()->ErrorBeep();
			return;
	}
	CLcd::GetInstance()->OKBeep();
}

////////////////////////////////////////////////////////////

void CMenuBase::MenuButtonPressProbe(const SMenuItemDef* def)
{
	MenuButtonPressProbe(axis_t((unsigned int)def->GetParam1()));
}

////////////////////////////////////////////////////////////


void CMenuBase::MenuButtonPressProbe(axis_t axis)
{
	CGCodeBuilder builder;
	InitPostCommand(EGCodeSyntaxType::GCode, builder.GetCommand());

	builder.Add(F("g91 g31 "))
	       .AddAxisName(axis)
	       .Add(-10000)
	       .Add(F(" F100 g90"));

	if (PostCommand(builder.GetCommand()))
	{
		eepromofs_t ofs = sizeof(CConfigEeprom::SCNCEeprom::SAxisDefinitions) * axis;

		builder.InitCommand();
		InitPostCommand(EGCodeSyntaxType::GCode, builder.GetCommand());

		builder.Add(F("g92 "))
		       .AddAxisName(axis)
		       .Add(- mm1000_t(CConfigEeprom::GetConfigU32(offsetof(CConfigEeprom::SCNCEeprom, axis[0].probesize) + ofs)));

		PostCommand(builder.GetCommand());

		builder.InitCommand();
		InitPostCommand(EGCodeSyntaxType::GCode, builder.GetCommand());

		builder.Add(F("g91 "))
		       .AddAxisName(axis)
		       .Add(3000)
		       .Add(F(" g90"));

		PostCommand(builder.GetCommand());
	}
}

////////////////////////////////////////////////////////////

void CMenuBase::MenuButtonPressHome(const SMenuItemDef* def)
{
	MenuButtonPressHomeA(axis_t((unsigned int)def->GetParam1()));
}

void CMenuBase::MenuButtonPressHomeA(axis_t axis)
{
	CGCodeBuilder builder;
	InitPostCommand(EGCodeSyntaxType::GCode, builder.GetCommand());

	builder.Add(F("g53 g0"))
	       .AddAxisName(axis);

	switch (axis)
	{
		case Z_AXIS: builder.Add(F("#5163"));	break;
		default: builder.Add(F("0"));			break;
	}
	PostCommand(builder.GetCommand());
	CLcd::GetInstance()->OKBeep();
};

////////////////////////////////////////////////////////////

void CMenuBase::MenuButtonPressMoveG92(const SMenuItemDef*)
{
	auto axis = axis_t((unsigned int)GetMenuDef()->GetParam1());

	CGCodeBuilder builder;
	InitPostCommand(EGCodeSyntaxType::GCode, builder.GetCommand());

	builder.Add(F("g92 "))
	       .AddAxisName(axis)
	       .Add(F("0"));

	PostCommand(builder.GetCommand());
	CLcd::GetInstance()->OKBeep();
}

////////////////////////////////////////////////////////////

void CMenuBase::MenuButtonPressSpindle(const SMenuItemDef*)
{
	if (CControl::GetInstance()->IOControl(CControl::SpindleCW) != 0)
	{
		PostCommand(EGCodeSyntaxType::GCodeBasic, F("m5"));
	}
	else
	{
		PostCommand(EGCodeSyntaxType::GCodeBasic, F("m3"));
	}
	CLcd::GetInstance()->OKBeep();
}

////////////////////////////////////////////////////////////

void CMenuBase::MenuButtonPressCoolant(const SMenuItemDef*)
{
	if (CControl::GetInstance()->IOControl(CControl::Coolant) != 0)
	{
		PostCommand(EGCodeSyntaxType::GCodeBasic, F("m9"));
	}
	else
	{
		PostCommand(EGCodeSyntaxType::GCodeBasic, F("m7"));
	}
	CLcd::GetInstance()->OKBeep();
}

////////////////////////////////////////////////////////////

void CMenuBase::MenuButtonPressResurrect(const SMenuItemDef*)
{
	if (CControl::GetInstance()->IsKilled())
	{
		CControl::GetInstance()->Resurrect();
		CLcd::GetInstance()->OKBeep();
	}
	else
	{
		CLcd::GetInstance()->ErrorBeep();
	}
}

////////////////////////////////////////////////////////////

void CMenuBase::MenuButtonPressHold(const SMenuItemDef*)
{
	if (!CControl::GetInstance()->IsHold())
	{
		CControl::GetInstance()->Hold();
		CLcd::GetInstance()->OKBeep();
	}
	else
	{
		CLcd::GetInstance()->ErrorBeep();
	}
}

///////////////////////////////////////////////////////////

void CMenuBase::MenuButtonPressResume(const SMenuItemDef*)
{
	if (CControl::GetInstance()->IsHold())
	{
		CControl::GetInstance()->Resume();
		CLcd::GetInstance()->OKBeep();
	}
	else
	{
		CLcd::GetInstance()->ErrorBeep();
	}
}

////////////////////////////////////////////////////////////

#endif // REDUCED_SIZE

////////////////////////////////////////////////////////////
