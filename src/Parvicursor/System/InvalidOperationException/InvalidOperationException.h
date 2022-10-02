/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_InvalidOperationException_h__
#define __System_InvalidOperationException_h__

#include "../Exception/Exception.h"
#include "../String/String.h"

//**************************************************************************************************************//
namespace System
{
	class InvalidOperationException : public Exception
	{
		//----------------------------------------------------
		public: InvalidOperationException();
		public: ~InvalidOperationException();
	    public: InvalidOperationException(const String &message);
		public: String get_Message();
		//----------------------------------------------------
	};
};
//**************************************************************************************************************//

#endif
