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
////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////

#include "Stepper.h"

////////////////////////////////////////////////////////

#define SMC800_NUM_AXIS	3

////////////////////////////////////////////////////////

class CStepperSMC800 : public CStepper
{
private:
	typedef CStepper super;
public:

	CStepperSMC800();

	virtual void Init() override;

	void Remove();

public:

	virtual bool    IsAnyReference() override { return GetReferenceValue(0) == HIGH; };
	virtual uint8_t GetReferenceValue(uint8_t referenceId) override;

	void SetFullStepMode(axis_t axis, bool fullStepMode) { _fullStepMode[axis] = fullStepMode; };

protected:

	virtual void    Step(const uint8_t steps[NUM_AXIS], axisArray_t directionUp, bool isSameDirection) override;
	virtual void    SetEnable(axis_t   axis, uint8_t                level, bool       force) override;
	virtual uint8_t GetEnable(axis_t   axis) override;

	virtual void MoveAwayFromReference(axis_t axis, sdist_t dist, steprate_t vMax) override;

	////////////////////////////////////////////////////////

private:

	uint8_t _stepIdx[SMC800_NUM_AXIS];
	uint8_t _level[SMC800_NUM_AXIS];
	bool    _fullStepMode[NUM_AXIS];

	void SetPhase(axis_t axis);

	static void OutSMC800Cmd(const uint8_t val);
};
