#pragma once

////////////////////////////////////////////////////////

#include "Configuration_ProxxonMF70.h"

////////////////////////////////////////////////////////

#include <LCD.h>
#include "RotaryButton.h"

#define ROTARY_ACCURACY	4

////////////////////////////////////////////////////////

class CMyLcd : public CLcd
{
private:

	typedef CLcd super;

public:

	CMyLcd()												{ _curretDraw = NULL; _expectButtonOff = false; _rotaryFocus = RotaryMainPage; }

	virtual void Init();
	virtual void Idle(unsigned int idletime);
	virtual void TimerInterrupt();

protected:

	typedef signed char rotarypos_t;

	enum EPage
	{
		SplashPage = 0,
		DebugPage = 1,
		AbsPage = 2,
		PresetPage = 3,
		StartSDPage = 4,
		PausePage = 5,
		ErrorPage = 6,
		MenuPage = 7,
		PageCount
	};

	enum ERotaryFocus
	{
		RotaryMainPage,
		RotaryMenuPage
	};

	EnumAsByte(EPage) GetPage();

	virtual void Draw(EDrawType draw);
	virtual unsigned long Splash();
	virtual void FirstDraw();

private:

	void SetRotaryFocusMainPage();
	void SetRotaryFocusMenuPage();

	typedef bool(CMyLcd::*DrawFunction)(bool setup);
	typedef void(CMyLcd::*ButtonFunction)();
	typedef void(CMyLcd::*MenuButtonFunction)(unsigned short param);

	void DrawLoop(DrawFunction drawfnc)						{ _curretDraw = drawfnc; DrawLoop(); }
	void DrawLoop();

	struct SPageDef
	{
		DrawFunction draw;
		ButtonFunction buttonpress;
	};

	struct SMenuDef
	{
		const char* text;
		MenuButtonFunction buttonpress;
		unsigned short param;
	};

	EnumAsByte(ERotaryFocus) _rotaryFocus;
	EnumAsByte(EPage)		_currentpage;
	unsigned char			_currentMenuIdx;
	unsigned char			_currentMenuOffset;
	axis_t					_currentMenuAxis;

	const SMenuDef*			_currentMenu;
	const __FlashStringHelper* _currentMenuName;

	bool _expectButtonOff;

	DrawFunction _curretDraw;

	CRotaryButton<rotarypos_t, ROTARY_ACCURACY> _button;

	static SPageDef _pagedef[];

	void SetDefaultPage();
	void SetMenuPage();

	void Beep();
	static bool SendCommand(const __FlashStringHelper* cmd);
	static bool SendCommand(char* cmd);


	void ButtonPress();

	void ButtonPressStartSDPage();
	void ButtonPressPause();
	void ButtonPressMenuPage();
	void ButtonPressShowMenu();

	bool DrawLoopSplash(bool setup);
	bool DrawLoopDebug(bool setup);	
	bool DrawLoopPosAbs(bool setup);
	bool DrawLoopPreset(bool setup);
	bool DrawLoopStartSD(bool setup);
	bool DrawLoopPause(bool setup);	
	bool DrawLoopError(bool setup);
	bool DrawLoopMenu(bool setup);

	bool DrawLoopSetupDefault();
	void DrawLoopDefaultHead();

	// Menu Page

	unsigned char GetMenuIdx();
	unsigned char GetMenuCount();
	const __FlashStringHelper* GetMenuText(unsigned char idx);
	MenuButtonFunction GetMenuFnc(unsigned char idx);
	unsigned short GetMenuParam(unsigned char idx)				{ return (unsigned short)pgm_read_word(&_currentMenu[idx].param); }

	unsigned char FindMenuIdx(MenuButtonFunction f, unsigned short param, unsigned char valueIffail);

	char* AddAxisName(char*buffer, axis_t axis);

	void ButtonPressMenuG92Clear(unsigned short)				{ SendCommand(F("g92")); Beep(); }
	void ButtonPressMenuEnd(unsigned short param);

	void ButtonPressMenuHome(unsigned short);
	void ButtonPressMenuProbe(unsigned short);
	void ButtonPressMenuSpindle(unsigned short);
	void ButtonPressMenuCoolant(unsigned short);

	void ButtonPressMenuMoveG92(unsigned short);
	void ButtonPressMenuMove(unsigned short movetype);
	void ButtonPressMenuMoveBack(unsigned short);

	void ButtonPressMenuSDInit(unsigned short)				{ SendCommand(F("m21")); Beep(); }
	void ButtonPressMenuSDBack(unsigned short);

	void ButtonPressMenuSetMove(unsigned short axis);
	void ButtonPressMenuSetSD(unsigned short);

	enum EMoveType
	{
		MoveP10,
		MoveP1,
		MoveP01,
		MoveP001,
		MoveM10,
		MoveM1,
		MoveM01,
		MoveM001,
		MoveHome
	};

	void SetMenu(const SMenuDef* pMenu,const __FlashStringHelper* name)			{ _currentMenu = pMenu; _currentMenuName = name; _currentMenuIdx = 0; _currentMenuOffset = 0; };
	void SetMainMenu()															{ SetMenu(_mainMenu,F("Main")); }

	static const SMenuDef _mainMenu[] PROGMEM;
	static const SMenuDef _axisMenuMove[] PROGMEM;
	static const SMenuDef _SDMenu[] PROGMEM;

#if defined(__AVR_ARCH__)

        static ButtonFunction GetButtonPress_P(const void* adr);
        static MenuButtonFunction GetMenuButtonPress_P(const void* adr);
        static DrawFunction GetDrawFunction_P(const void* adr);

#endif

};

////////////////////////////////////////////////////////

extern CMyLcd Lcd;
