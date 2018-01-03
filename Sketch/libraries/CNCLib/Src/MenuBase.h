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

#include "CNCLibTypes.h"
#include "LCD.h"
#include "MenuHelper.h"

////////////////////////////////////////////////////////

class CMenuBase
{

public:

	CMenuBase()														{ SetMenu(NULL); }

	struct SMenuItemDef;
	struct SMenuDef;

	typedef uint8_t menupos_t;
	typedef void(CMenuBase::*MenuFunction)(const SMenuItemDef*);
	typedef uintptr_t menuparam_t;

	struct SMenuItemDef
	{
	public:
		const char* _text;
		MenuFunction _buttonpress;
		menuparam_t _param1;
		menuparam_t _param2;
		menuparam_t _param3;
	public:
		const __FlashStringHelper* GetText() const					{ return (const __FlashStringHelper*)pgm_read_ptr(&this->_text); }
		MenuFunction GetButtonPress()const;
		menuparam_t GetParam1()	const								{ return (menuparam_t)pgm_read_ptr(&this->_param1); }
		menuparam_t GetParam2()	const								{ return (menuparam_t)pgm_read_ptr(&this->_param2); }
		menuparam_t GetParam3()	const								{ return (menuparam_t)pgm_read_ptr(&this->_param3); }
	};

	struct SMenuDef
	{
	public:
		const char* _text;
		const SMenuItemDef* _items;
		menuparam_t _param1;
		menuparam_t _param2;

		uint8_t GetItemCount() const
		{
			const SMenuItemDef* items = GetItems();
			for (uint8_t x = 0;; x++)
			{
				if (items[x].GetText() == NULL) return x;
			}
		}

		uint8_t FindMenuIdx(uintptr_t param, bool(*check)(const SMenuItemDef*, uintptr_t param)) const
		{
			const SMenuItemDef* item = &GetItems()[0];
			for (uint8_t x = 0; item->GetText() != NULL; x++, item++)
			{
				if (check(item, param)) return x;
			}

			return 255;
		}

	public:
		const __FlashStringHelper* GetText() const					{ return (const __FlashStringHelper*)pgm_read_ptr(&this->_text); }
		const SMenuItemDef* GetItems() const						{ return (const SMenuItemDef*)pgm_read_ptr(&this->_items); }
		menuparam_t GetParam1()	const								{ return (menuparam_t)pgm_read_ptr(&this->_param1); }
		menuparam_t GetParam2()	const								{ return (menuparam_t)pgm_read_ptr(&this->_param2); }
	};

public:

	menupos_t GetPosition()											{ return _menuHelper.GetPosition(); }
	void SetPosition(menupos_t position)							{ _menuHelper.SetPosition(position); }

	menupos_t GetMenuItemCount();
	const __FlashStringHelper*  GetItemText(menupos_t idx);
	const __FlashStringHelper*  GetText();

	CMenuHelper& GetMenuHelper()									{ return _menuHelper; }

	void SetMainMenu()												{ SetMenu(_main); }

	bool Select();
	virtual void Changed()=0;

	void OKBeep()													{ return CLcd::GetInstance()->OKBeep(); };
	void ErrorBeep()												{ return CLcd::GetInstance()->ErrorBeep(); };

	const SMenuDef*GetMenuDef() { return _current; }
	const SMenuDef*GetMainMenuDef() { return _main; }

	protected:

	//////////////////////////////////////////

	bool PostCommand(EnumAsByte(EGCodeSyntaxType) syntaxtype, const __FlashStringHelper* cmd, Stream* output=NULL)	{ return CLcd::GetInstance()->PostCommand(syntaxtype,cmd,output); }
	bool PostCommand(char* cmd, Stream* output=NULL)				{ return CLcd::GetInstance()->PostCommand(cmd,output); }
	uint8_t InitPostCommand(EnumAsByte(EGCodeSyntaxType) syntaxtype, char* cmd)			{ return CLcd::GetInstance()->InitPostCommand(syntaxtype,cmd); }

	//////////////////////////////////////////

	const SMenuDef*		_main=NULL;

private:

	CMenuHelper			_menuHelper;
	const SMenuDef*		_current;

public:

	void MenuButtonPressSetMenu(const SMenuItemDef*);				// param1 => new menu, param2 != 0 => SetPosition, MenuFunction must be MenuButtonPressSetMenu & Menu = param2
	void MenuButtonPressMenuBack(const SMenuItemDef*);				// param1 => new menu, find and set position to "this" menu in new menu

	void MenuButtonPressSetCommand(const SMenuItemDef*def);			// param1 => const char* to command

	void SetMenu(const SMenuDef* pMenu)								{ _current = pMenu; _menuHelper.Clear(); };

	////////////////////////////////////////////////////////

	void MenuButtonPressHomeA(axis_t axis);
	void MenuButtonPressHome(const SMenuItemDef*);					// param1 : axis
	void MenuButtonPressProbe(axis_t axis);
	void MenuButtonPressProbe(const SMenuItemDef*);					// param1 : axis
	void MenuButtonPressSpindle(const SMenuItemDef*);
	void MenuButtonPressCoolant(const SMenuItemDef*);
	void MenuButtonPressMoveG92(const SMenuItemDef*);
	void MenuButtonPressMove(const SMenuItemDef*);					// param1 : enum EMoveType, TODO=>String
	void MenuButtonPressRotate(const SMenuItemDef*);
	void MenuButtonPressResurrect(const SMenuItemDef*);
	void MenuButtonPressHold(const SMenuItemDef*);
	void MenuButtonPressResume(const SMenuItemDef*);

	enum EMoveType
	{
		MoveP100,
		MoveP10,
		MoveP1,
		MoveP01,
		MoveP001,
		MoveM100,
		MoveM10,
		MoveM1,
		MoveM01,
		MoveM001,
		MoveHome
	};

	enum ERotateType
	{
		RotateClear,
		RotateOffset,
		RotateSetYZ,
		RotateSetX
	};
};

////////////////////////////////////////////////////////
