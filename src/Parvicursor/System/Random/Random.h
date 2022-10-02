/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Random_h__
#define __System_Random_h__


#include "../String/String.h"
#include "../ArgumentException/ArgumentNullException.h"
#include "../FormatException/FormatException.h"
#include "../OverflowException/OverflowException.h"
#include "../BasicTypes/BasicTypes.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//**************************************************************************************************************//

namespace System
{
		//----------------------------------------------------
		/*
			Represents a pseudo-random number generator, a device that produces a sequence of
			numbers that meet certain statistical requirements for randomness.
		*/
		class Random : public Object
        {
			//----------------------------------------------------
			// Initializes a new instance of the Random class, using a time-dependent default seed value.
			public: Random();
			// Initializes a new instance of the Random class, using the specified seed value.
			public: Random(Int32 seed);
			// Returns a nonnegative random number.
			public: Int32 Next();
			// Returns a nonnegative random number less than the specified maximum.
			public: Int32 Next(Int32 maxValue);
			// Returns a random number within a specified range.
			public: Int32 Next(Int32 minValue, Int32 maxValue);
			//----------------------------------------------------
		};
		//----------------------------------------------------
};
//**************************************************************************************************************//

#endif
