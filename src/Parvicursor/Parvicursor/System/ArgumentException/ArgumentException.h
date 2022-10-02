/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_ArgumentException_h__
#define __System_ArgumentException_h__

#include "../Exception/Exception.h"
#include "../String/String.h"

//**************************************************************************************************************//
namespace System
{
	class ArgumentException : public Exception
	{
	    private: String paramName;
		//----------------------------------------------------
		public: ArgumentException();
		public: ~ArgumentException();
		public: ArgumentException(const String &paramName);
	    public: ArgumentException(const String &paramName, const String &message);
		public: String get_Message();
		//----------------------------------------------------
	};
};
//**************************************************************************************************************//

#endif
