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

#include "stdafx.h"

#include <LinearLookUp.h>

#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

////////////////////////////////////////////////////////

namespace StepperSystemTest
{
	CLinearLookup<int, int>::SLookupTable linearII[] =
	{
		{ 10, 10 },
		{ 20, 100 },
		{ 30, 1000 }
	};

	CLinearLookup<int, float>::SLookupTable linearIF[] =
	{
		{ 10, float(0.1) },
		{ 20, float(0.01) },
		{ 30, float(0.001) }
	};

	CLinearLookup<long, long>::SLookupTable linearLL[255];


	TEST_CLASS(CLinearLookupTests)
	{
	public:

		TEST_METHOD(LinearLookupIntToIntTest)
		{
			CLinearLookup<int, int> ii(linearII, sizeof(linearII) / sizeof(CLinearLookup<int, int>::SLookupTable));

			Assert::AreEqual(10, ii.Lookup(10));
			Assert::AreEqual(100, ii.Lookup(20));
			Assert::AreEqual(1000, ii.Lookup(30));

			Assert::AreEqual(10, ii.Lookup(9));		// less is lower bound
			Assert::AreEqual(1000, ii.Lookup(31));	// more is upper bound

			Assert::AreEqual(10 + (90 * 1 / 10), ii.Lookup(11));
			Assert::AreEqual(10 + (90 * 5 / 10), ii.Lookup(15));
			Assert::AreEqual(10 + (90 * 9 / 10), ii.Lookup(19));
		}

		TEST_METHOD(LinearLookupIntToFloatTest)
		{
			CLinearLookup<int, float> lf(linearIF, sizeof(linearIF) / sizeof(CLinearLookup<int, float>::SLookupTable));

			Assert::AreEqual(float(0.1), lf.Lookup(10));
			Assert::AreEqual(float(0.01), lf.Lookup(20));
			Assert::AreEqual(float(0.001), lf.Lookup(30));

			Assert::AreEqual(float(0.1), lf.Lookup(9));		// less is lower bound
			Assert::AreEqual(float(0.001), lf.Lookup(31));		// more is upper bound

			Assert::AreEqual(float(0.1 - (0.09 * 1 / 10)), lf.Lookup(11));
			Assert::AreEqual(float(0.1 - (0.09 * 5 / 10)), lf.Lookup(15));
			Assert::AreEqual(float(0.1 - (0.09 * 9 / 10)), lf.Lookup(19), 0.001f);
		}

		TEST_METHOD(LinearLookupLongToLongTest)
		{
			// test max Size (255)

			CLinearLookup<long, long> ll(linearLL, sizeof(linearLL) / sizeof(CLinearLookup<long, long>::SLookupTable));

			for (int i = 0; i < 255; i++)
			{
				linearLL[i].input  = i * 256;
				linearLL[i].output = i * 2560;
			}

			Assert::AreEqual(0l, ll.Lookup(-1));
			Assert::AreEqual(254 * 2560l, ll.Lookup(254 * 256 + 1l));
		}
	};
}
