/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "Convert.h"

//**************************************************************************************************************//

namespace System
{
		//----------------------------------------------------
		Int16 Convert::ToInt16(const String &value)
		{
			if(value.get_BaseStream() == null)
				return 0;

			Int32 ret = atoi(value.get_BaseStream());

			if(ret == INT_MAX  || ret == INT_MIN)
				throw OverflowException("value", "value represents a number less than MinValue or greater than MaxValue.");

			return (Int16)ret;
		}
		//----------------------------------------------------
		UInt16 Convert::ToUInt16(const String &value)
		{
			if(value.get_BaseStream() == null)
				return 0;

			Int32 ret = atoi(value.get_BaseStream());

			if(ret == INT_MAX  || ret == INT_MIN)
				throw OverflowException("value", "value represents a number less than MinValue or greater than MaxValue.");

			return (UInt16)ret;
		}
		//----------------------------------------------------
		Int32 Convert::ToInt32(const String &value)
		{
			if(value.get_BaseStream() == null)
				return 0;

			Int32 ret = atoi(value.get_BaseStream());

			if(ret == INT_MAX  || ret == INT_MIN)
				throw OverflowException("value", "value represents a number less than MinValue or greater than MaxValue.");

			return (Int32)ret;
		}
		//----------------------------------------------------
		UInt32 Convert::ToUInt32(const String &value)
		{
			if(value.get_BaseStream() == null)
				return 0;

			Int32 ret = atoi(value.get_BaseStream());

			if(ret == INT_MAX  || ret == INT_MIN)
				throw OverflowException("value", "value represents a number less than MinValue or greater than MaxValue.");

			return (Int32)ret;
		}
		//----------------------------------------------------
		Int64 Convert::ToInt64(const String &value)
		{
			if(value.get_BaseStream() == null)
				return 0;

			/*long int ret = atol(value.get_BaseStream());

			if(ret == 0.0)
				throw FormatException("value", "value does not consist of an optional sign followed by a sequence of digits (zero through nine).");

			if(ret == HUGE_VAL)
				throw OverflowException("value", "value represents a number less than MinValue or greater than MaxValue.");*/


			Int64 ret;
#if defined WIN32 || WIN64
			if(sscanf(value.get_BaseStream(), "%I64u", &ret) <= 0)
#else
			if(sscanf(value.get_BaseStream(), "%llu", &ret) <= 0)
#endif
				throw FormatException("value", "value does not consist of an optional sign followed by a sequence of digits (zero through nine).");


			return ret;
		}
		//----------------------------------------------------
		double Convert::ToDouble(const String &value)
		{
			if(value.get_BaseStream() == null)
				return 0;

			double ret = atof(value.get_BaseStream());

			if(ret == 0.0)
				throw FormatException("value", "value does not consist of an optional sign followed by a sequence of digits (zero through nine).");

			if(ret == HUGE_VAL)
				throw OverflowException("value", "value represents a number less than MinValue or greater than MaxValue.");


			return ret;
		}
		//----------------------------------------------------
		UInt64 Convert::ToUInt64(const String &value)
		{
			return (UInt64)ToInt64(value);
		}
		//----------------------------------------------------
		String Convert::ToString(Int16 value)
		{
			char *str = new char[5 + 1]; //max: 65535,0xFFFF
			sprintf(str, "%hd", value);    // Convert int to string
			return String(str);
		}
		//----------------------------------------------------
		String Convert::ToString(Int32 value)
		{
			char *str = new char[10 + 1]; //max: 4294967295,0xFFFFFFFF
#if defined WIN32 || WIN64
			sprintf(str, "%I32u", value);    // Convert int to string
#else
			sprintf(str, "%lu", value);    // Convert int to string
#endif
			return String(str);
		}
		//----------------------------------------------------
		String Convert::ToString(Int64 value)
		{
			char *str = new char[20 + 1]; // max: 18446744073709551615, 0xFFFFFFFFFFFFFFFF
#if defined WIN32 || WIN64
			sprintf(str, "%I64u", value);    // Convert int to string
#else
			sprintf(str, "%llu", value);    // Convert int to string
#endif
			return String(str);
		}
		//----------------------------------------------------
		/*String Convert::ToString(double value)
		{
			char *str = new char[32];
			sprintf(str, "%.20g", value);    // Convert int to string
			return String(str);
		}*/
	    //----------------------------------------------------
}
//**************************************************************************************************************//
