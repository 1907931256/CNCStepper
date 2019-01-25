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
////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////

#include <Control3D.h>

#include <OnOffIOControl.h>
#include <Analog8IOControl.h>
#include <ReadPinIOControl.h>
#include <PushButtonLow.h>
#include <DummyIOControl.h>
#include <ConfigEeprom.h>

#include "Configuration.h"

// must be after "Configuration.h" because of defines
#include <ControlImplementation.h>

////////////////////////////////////////////////////////

class CMyControl : public CControl3D
{
private:

	typedef CControl3D super;

public:

	CMyControl() { }

	virtual void Kill() override;

	virtual void     IOControl(uint8_t tool, uint16_t level) override;
	virtual uint16_t IOControl(uint8_t tool) override;

protected:

	virtual void Init() override;
	virtual void Initialized() override;

	virtual void TimerInterrupt() override;

	virtual bool IsKill() override;
	virtual void Poll() override;

	virtual bool OnEvent(EnumAsByte(EStepperControlEvent) eventType, uintptr_t addInfo) override;

private:

	static const CConfigEeprom::SCNCEeprom _eepromFlash;

	ControlData _data;


	CAnalog8IOControl<LASER_PWM_PIN>                                             _laserPWM;
	COnOffIOControl<LASER_ENABLE_PIN, LASER_ENABLE_PIN_ON, LASER_ENABLE_PIN_OFF> _laserOnOff;

	COnOffIOControl<LASERWATER_PIN, LASERWATER_PIN_ON, LASERWATER_PIN_OFF>    _laserWater;
	COnOffIOControl<LASERVACUUM_PIN, LASERVACUUM_PIN_ON, LASERVACUUM_PIN_OFF> _laserVacuum;
};

////////////////////////////////////////////////////////

extern CMyControl Control;
