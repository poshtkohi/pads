/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System__Threading_ThreadStateException_h__
#define __System__Threading_ThreadStateException_h__

#include "../../System/Exception/Exception.h"
#include "../../System/String/String.h"

//**************************************************************************************************************//
namespace System
{
	namespace Threading
	{
		class ThreadStateException : public Exception
		{
			//----------------------------------------------------
			public: ThreadStateException();
			public: ~ThreadStateException();
			public: ThreadStateException(const String &message);
			public: String get_Message();
			//----------------------------------------------------
		};
	};
};
//**************************************************************************************************************//

#endif
