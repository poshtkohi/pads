/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Exception_h__
#define __System_Exception_h__

#include "../Object/Object.h"
#include "../String/String.h"
#include "../BasicTypes/BasicTypes.h"

//**************************************************************************************************************//
using namespace System;

namespace System
{
	class Exception : public Object// ISerializable
	{
		String message;
		Exception *inner_exception;
		public: Exception(void);
		public: Exception(const String &msg);
		public: Exception(const String &msg, const Exception *e);
		public: ~Exception();
		public: Exception *get_InnerException();
		private: void set_Message(const String &s);
		public: virtual String get_Message();
	};
};
//**************************************************************************************************************//

#endif
