/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Convert_h__
#define __System_Convert_h__


#include "../String/String.h"
#include "../ArgumentException/ArgumentNullException.h"
#include "../FormatException/FormatException.h"
#include "../OverflowException/OverflowException.h"
#include "../BasicTypes/BasicTypes.h"


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <sstream>

#ifndef INT_MAX
#define INT_MAX       2147483647    /* maximum (signed) int value */
#endif

#ifndef INT_MIN
#define INT_MIN     (-2147483647 - 1) /* minimum (signed) int value */
#endif

//**************************************************************************************************************//

namespace System
{
		//----------------------------------------------------
		class Convert : public Object
        {
			//----------------------------------------------------
			public: static Int16 ToInt16(const String &value);
			public: static UInt16 ToUInt16(const String &value);
			public: static Int32 ToInt32(const String &value);
			public: static UInt32 ToUInt32(const String &value);
			public: static Int64 ToInt64(const String &value);
			public: static double ToDouble(const String &value);
			public: static UInt64 ToUInt64(const String &value);
			public: static String ToString(Int16 value);
			public: static String ToString(Int32 value);
			public: static String ToString(Int64 value);
			//public: static String ToString(double value);
			//----------------------------------------------------
		};
		//----------------------------------------------------
};
//**************************************************************************************************************//

#endif
