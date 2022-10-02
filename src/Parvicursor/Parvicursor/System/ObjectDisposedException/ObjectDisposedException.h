/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_ObjectDisposedException_h__
#define __System_ObjectDisposedException_h__

#include "../Exception/Exception.h"
#include "../String/String.h"

//**************************************************************************************************************//
namespace System
{
	class ObjectDisposedException : public Exception
	{
	    private: String objectName;
		//----------------------------------------------------
		public: ObjectDisposedException();
		public: ~ObjectDisposedException();
		public: ObjectDisposedException(const String &objectName);
	    public: ObjectDisposedException(const String &objectName, const String &message);
		public: String get_Message();
		//----------------------------------------------------
	};
};
//**************************************************************************************************************//

#endif
