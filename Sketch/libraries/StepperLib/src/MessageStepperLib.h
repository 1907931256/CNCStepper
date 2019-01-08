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
*/
////////////////////////////////////////////////////////

#pragma once

////////////////////////////////////////////////////////

#ifdef _NO_LONG_MESSAGE

#define StepperMessage(a,b)	F(a)
#define StepperMessageOr(a,b)	F(a)
#define StepperMessageOne(a)	F(a)

#define MESSAGE(a)

#else

//#define StepperMessage(a,b) b
#define StepperMessage(a,b) F(a ":" b)
#define StepperMessageOr(a,b)	F(b)
#define StepperMessageOne(a)	F(a)

#define MESSAGE(a) a

#endif

#define MESSAGE_CTRLX								StepperMessageOne("Grbl 0.9j ['$' for help]")
#define MESSAGE_OK_INITIALIZED						MESSAGE_OK
#define MESSAGE_OK_RESURRECT						MESSAGE_OK
#define MESSAGE_OK_EMPTYLINE						MESSAGE_OK
#define MESSAGE_OK									StepperMessageOne("ok")
#define MESSAGE_ERROR								StepperMessageOne("error:")
#define MESSAGE_INFO								StepperMessageOne("info:")
#define MESSAGE_WARNING								StepperMessageOne("warning:")

#define MESSAGE_UNKNOWNERROR						StepperMessage("?","Unknown error")

#define MESSAGE_STEPPER_EmptyMoveSkipped			StepperMessage("1","EmptyMove skipped")
#define MESSAGE_STEPPER_Backlash					StepperMessage("2","Backlash")
#define MESSAGE_STEPPER_IsAnyReference				StepperMessage("3","IsAnyReference")
#define MESSAGE_STEPPER_RangeLimit					StepperMessage("1","Range limit")
#define MESSAGE_STEPPER_IsReferenceIsOn				StepperMessage("2","IsReference is on")
#define MESSAGE_STEPPER_MoveReferenceFailed			StepperMessage("4","MoveReference failed")

#define MESSAGE_STEPPER_MoveAwayFromReference		StepperMessage("5","Move away from reference")
