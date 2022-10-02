/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_ArgumentOutOfRangeException_h__
#define __System_ArgumentOutOfRangeException_h__

#include "../Exception/Exception.h"
#include "../String/String.h"

//**************************************************************************************************************//
namespace System
{
	class ArgumentOutOfRangeException : public Exception
	{
	    private: String paramName;
		//----------------------------------------------------
		public: ArgumentOutOfRangeException();
		public: ~ArgumentOutOfRangeException();
		public: ArgumentOutOfRangeException(const String &paramName);
	    public: ArgumentOutOfRangeException(const String &paramName, const String &message);
		public: String get_Message();
		//----------------------------------------------------
	};
};
//**************************************************************************************************************//

#endif
