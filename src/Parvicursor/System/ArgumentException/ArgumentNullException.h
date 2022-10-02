/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_ArgumentNullException_h__
#define __System_ArgumentNullException_h__

#include "../Exception/Exception.h"
#include "../String/String.h"

//**************************************************************************************************************//
namespace System
{
	class ArgumentNullException : public Exception
	{
	    private: String paramName;
		//----------------------------------------------------
		public: ArgumentNullException();
		public: ~ArgumentNullException();
		public: ArgumentNullException(const String &paramName);
	    public: ArgumentNullException(const String &paramName, const String &message);
		public: String get_Message();
		//----------------------------------------------------
	};
};
//**************************************************************************************************************//

#endif
