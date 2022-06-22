#include "pch.h"
#include "CppUnitTest.h"
#include "../OS_2/Main.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest1
{
	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			double expectedResult = 0.143007;

			int param1[] = { 0, 4, 3, 1, 2, 3, 4, 5 };
			int param2[] = { 0, 5, 3, 6, 7, 8, 9, 10, 11 };

			double result = calculateResult(param1, param2);

			Assert::AreEqual(expectedResult, result, 0.0001);
		}

		TEST_METHOD(TestMethod2)
		{
			//denomerator is 0
			double expectedResult = INFINITY;

			int param1[] = { 0, 3, 8, 1, 2, 10, 7 };
			int param2[] = { 0, 0, 8, 0 };

			double result = calculateResult(param1, param2);

			Assert::IsTrue(result == INFINITY);
		}

		TEST_METHOD(TestMethod3)
		{
			double expectedResult = 1.080338;

			int param1[] = { 0, 8, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
			int param2[] = { 0, 5, 2, 7, 7, 3, 3, 8, 9 };

			double result = calculateResult(param1, param2);

			Assert::AreEqual(expectedResult, result, 0.0001);
		}
	};
}
