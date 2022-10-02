/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "DateTime.h"

//**************************************************************************************************************//

namespace System
{
		//----------------------------------------------------
#if defined WIN32 || WIN64

		DateTime::DateTime(SYSTEMTIME &_st)
		{
			this->st = _st;
		}

#else

		DateTime::DateTime(tm &_st, long _milliseconds)
		{
			this->st = _st;
			this->milliseconds = _milliseconds;
		}

#endif
		//----------------------------------------------------
		DateTime DateTime::get_Now()
		{
#if defined WIN32 || WIN64

			SYSTEMTIME st;
			//GetSystemTime(&st);
			GetLocalTime(&st);

			return DateTime(st);

#else

            /*time_t rawtime;
			tm *st;
			time(&rawtime);
			st = localtime(&rawtime);*/



            struct timeval tv;
            tm* ptm;
            long milliseconds;
            gettimeofday (&tv, NULL);
            ptm = localtime ((time_t*)&tv);

			return DateTime(*ptm, tv.tv_usec/1000);

#endif
		}
		//----------------------------------------------------
		Int32 DateTime::get_Year()
		{
#if defined WIN32 || WIN64

			return this->st.wYear;

#else

            return this->st.tm_year + 1900;

#endif
		}
		//----------------------------------------------------
		Int32 DateTime::get_Month()
		{
#if defined WIN32 || WIN64

			return this->st.wMonth;

#else

            return this->st.tm_mon + 1;

#endif
		}
		//----------------------------------------------------
		Int32 DateTime::get_DayOfWeek()
		{
#if defined WIN32 || WIN64

			return this->st.wDayOfWeek;

#else

            return this->st.tm_wday;

#endif
		}
		//----------------------------------------------------
		Int32 DateTime::get_Day()
		{
#if defined WIN32 || WIN64

			return this->st.wDay;

#else

            return this->st.tm_mday;

#endif
		}
		//----------------------------------------------------
		Int32 DateTime::get_Hour()
		{
#if defined WIN32 || WIN64

			return this->st.wHour;
#else

            return this->st.tm_hour;

#endif
		}
		//----------------------------------------------------
		Int32 DateTime::get_Minute()
		{
#if defined WIN32 || WIN64

			return this->st.wMinute;

#else

			return this->st.tm_min;

#endif
		}
		//----------------------------------------------------
		Int32 DateTime::get_Second()
		{
#if defined WIN32 || WIN64

            return this->st.wSecond;

#else

			return this->st.tm_sec;
#endif
		}
		//----------------------------------------------------
		Int32 DateTime::get_Milliseconds()
		{
#if defined WIN32 || WIN64

			return this->st.wMilliseconds;
#else

            return this->milliseconds;
#endif
		}
	    //----------------------------------------------------

}
//**************************************************************************************************************//
