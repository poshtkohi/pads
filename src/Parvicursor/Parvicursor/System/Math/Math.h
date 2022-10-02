/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Math_h__
#define __System_Math_h__


#include "../String/String.h"
#include "../Object/Object.h"
#include "../BasicTypes/BasicTypes.h"

#include <math.h>
//**************************************************************************************************************//

namespace System
{
		//----------------------------------------------------
		/*
			Provides constants and static methods for trigonometric, logarithmic,
			and other common mathematical functions.
		*/
		class Math : public Object
        {
			//----------------------------------------------------
			// Trigonometric functions:
			// Computes cosine
			public: inline static Double Cos(Double d)
			{
				return ::cos(d);
			}
			// Computes sine
			public: inline static Double Sin(Double d)
			{
				return ::sin(d);
			}
			// Computes tangent
			public: inline static Double Tan(Double d)
			{
				return ::tan(d);
			}
			// Computes arc cosine
			public: inline static Double Acos(Double d)
			{
				return ::acos(d);
			}
			// Computes arc sine
			public: inline static Double Asin(Double d)
			{
				return ::asin(d);
			}
			// Computes arc tangent
			public: inline static Double Atan(Double d)
			{
				return ::atan(d);
			}
			// Computes arc tangent with two parameters
			public: inline static Double Atan2(Double y, Double x)
			{
				return ::atan2(y, x);
			}
			//----------------------------------------------------
			// Hyperbolic functions:
			// Computes hyperbolic cosine
			public: inline static Double Cosh(Double d)
			{
				return ::cosh(d);
			}
			// Computes hyperbolic sine
			public: inline static Double Sinh(Double d)
			{
				return ::sinh(d);
			}
			// Computes hyperbolic tangent
			public: inline static Double Tanh(Double d)
			{
				return ::tanh(d);
			}
			//----------------------------------------------------
			// Exponential and logarithmic functions:
			// Computes exponential function
			public: inline static Double Exp(Double d)
			{
				return ::exp(d);
			}
			// Gets significand and exponent
			//public: inline static Double Frexp(Double d)
			//{
			//	return ::frexp();
			//}
			// Generates number from significand and exponent
			public: inline static Double Ldexp(Double x, Int32 y)
			{
				return ::ldexp(x, y);
			}
			// Computes natural logarithm
			public: inline static Double Log(Double d)
			{
				return ::log(d);
			}
			// Computes common logarithm
			public: inline static Double Log10(Double d)
			{
				return ::log10(d);
			}
			// Breaks into fractional and integral parts
			//public: inline static Double Modf(Double d)
			//{
			//	return ::modf();
			//}
			//----------------------------------------------------
			// Power functions:
			// Raises to power
			public: inline static Double Pow(Double x, Double y)
			{
				return ::pow(x, y);
			}
			// Computes square root
			public: inline static Double Sqrt(Double d)
			{
				return ::sqrt(d);
			}
			//----------------------------------------------------
			//Rounding, absolute value and remainder functions:
			// Rounds up value
			public: inline static Double Ceiling(Double d)
			{
				return ::ceill(d);
			}
			// Computes absolute value
			public: inline static Double Fabs(Double d)
			{
				return ::fabs(d);
			}
			// Rounds down value
			public: inline static Double Floor(Double d)
			{
				return ::floor(d);
			}
			// Computes remainder of division
			public: inline static Double Fmod(Double x, Double y)
			{
				return ::fmod(x, y);
			}
			//----------------------------------------------------
		};
		//----------------------------------------------------
};
//**************************************************************************************************************//

#endif
