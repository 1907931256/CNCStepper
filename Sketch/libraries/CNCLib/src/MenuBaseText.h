////////////////////////////////////////////////////////
/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2019 Herbert Aitenbichler

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

#pragma once

////////////////////////////////////////////////////////////
// Main Menu

static const char _mMoveX[] PROGMEM  = "Move X             >";
static const char _mMoveY[] PROGMEM  = "Move Y             >";
static const char _mMoveZ[] PROGMEM  = "Move Z             >";
static const char _mMoveA[] PROGMEM  = "Move A             >";
static const char _mMoveB[] PROGMEM  = "Move B             >";
static const char _mMoveC[] PROGMEM  = "Move C             >";
static const char _mRotate[] PROGMEM = "Rotate             >";
static const char _mSD[] PROGMEM     = "SD                 >";
static const char _mExtra[] PROGMEM  = "Extra              >";
static const char _mBack[] PROGMEM   = "Back               <";
static const char _mEnd[] PROGMEM    = "End";


////////////////////////////////////////////////////////////
// Move Menu

static const char _mNextAxis[] PROGMEM = "Next axis";
static const char _mPrevAxis[] PROGMEM = "Prev axis";
static const char _mP100[] PROGMEM     = "+100";
static const char _mP10[] PROGMEM      = "+10";
static const char _mP1[] PROGMEM       = "+1";
static const char _mP01[] PROGMEM      = "+0.1";
static const char _mP001[] PROGMEM     = "+0.01";
static const char _mM001[] PROGMEM     = "-0.01";
static const char _mM01[] PROGMEM      = "-0.1";
static const char _mM1[] PROGMEM       = "-1";
static const char _mM10[] PROGMEM      = "-10";
static const char _mM100[] PROGMEM     = "-100";
static const char _mHome[] PROGMEM     = "Home";
static const char _mG92[] PROGMEM      = "Zero Offset(G92)";

////////////////////////////////////////////////////////////
// Rotate Menu

static const char _mRClr[] PROGMEM = "Rotation Clear";
static const char _mR0[] PROGMEM   = "Rotation Center";
static const char _mRX[] PROGMEM   = "Rotate YZ";
static const char _mRYZ[] PROGMEM  = "Rotate X";

////////////////////////////////////////////////////////////
// SD Menu

static const char _mSDInit[] PROGMEM   = "Init Card";
static const char _mSDStart[] PROGMEM  = "Start";
static const char _mSDSelect[] PROGMEM = "Select file";

////////////////////////////////////////////////////////////
// Extra Menu

static const char _mSpindle[] PROGMEM   = "Spindle On/Off";
static const char _mCoolant[] PROGMEM   = "Coolant On/Off";
static const char _mHomeZ[] PROGMEM     = "Home Z";
static const char _mProbeZ[] PROGMEM    = "Probe Z";
static const char _mG92Clear[] PROGMEM  = "G92 Clear";
static const char _mFuerElise[] PROGMEM = "Fuer Elise";
static const char _mResurrect[] PROGMEM = "Resurrect";
static const char _mHold[] PROGMEM      = "Hold";
static const char _mResume[] PROGMEM    = "Resume";

////////////////////////////////////////////////////////////
// Main Menu

static const char _mmMain[] PROGMEM   = "Main";
static const char _mmMoveX[] PROGMEM  = "Move X";
static const char _mmMoveY[] PROGMEM  = "Move Y";
static const char _mmMoveZ[] PROGMEM  = "Move Z";
static const char _mmMoveA[] PROGMEM  = "Move A";
static const char _mmMoveB[] PROGMEM  = "Move B";
static const char _mmMoveC[] PROGMEM  = "Move C";
static const char _mmRotate[] PROGMEM = "Rotate";
static const char _mmSD[] PROGMEM     = "SD Card";
static const char _mmExtra[] PROGMEM  = "Extra";

static const char _mmSDSelect[] PROGMEM = "SD Select file";

////////////////////////////////////////////////////////////
// Commands

static const char _g92[] PROGMEM = "g92";
static const char _m21[] PROGMEM = "m21";

////////////////////////////////////////////////////////////
