/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_DateTime_h__
#define __System_DateTime_h__


//nclude <stdio.h>
//#include <errno.h>
#include "../../System/Object/Object.h"
#include "../../System/BasicTypes/BasicTypes.h"
//#include "../../System/Exception/Exception.h"
//#include "../../System/String/String.h"



#if defined WIN32 || WIN64
#include <windows.h>
#else
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#endif

//


//**************************************************************************************************************//

namespace System
{
		//----------------------------------------------------
		class DateTime : public Object
        {
			//----------------------------------------------------
			//public: DateTime() { }
#if defined WIN32 || WIN64

			private: SYSTEMTIME st;
			private: DateTime(SYSTEMTIME &_st);

#else
            private: long milliseconds;
            private: tm st;
            private: DateTime(tm &_st, long _milliseconds);
#endif
			//----------------------------------------------------
			public: static DateTime get_Now();
			public: Int32 get_Year();
			public: Int32 get_Month();
			public: Int32 get_DayOfWeek();
			public: Int32 get_Day();
			public: Int32 get_Hour();
			public: Int32 get_Minute();
			public: Int32 get_Second();
			public: Int32 get_Milliseconds();
			//----------------------------------------------------
			//----------------------------------------------------
			//----------------------------------------------------
			//----------------------------------------------------
		};
		//----------------------------------------------------
};
//**************************************************************************************************************//

#endif
