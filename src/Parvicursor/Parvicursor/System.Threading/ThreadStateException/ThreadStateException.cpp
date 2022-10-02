/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "ThreadStateException.h"

//**************************************************************************************************************//
namespace System
{
	namespace Threading
	{
		//----------------------------------------------------
		ThreadStateException::~ThreadStateException()
		{
		}
		//----------------------------------------------------
		ThreadStateException::ThreadStateException() : Exception("The requested operation cannot be performed on the thread due to its current state.")
		{
		}
		//----------------------------------------------------
		ThreadStateException::ThreadStateException(const String &message) : Exception(message)
		{
		}
		//----------------------------------------------------
		String ThreadStateException::get_Message()
		{
			String s = "Unhandled Exception: System.Threading.ThreadStateException: " + Exception::get_Message();
			return s;//String::Copy(s);
		}
		//----------------------------------------------------
	};
};
//**************************************************************************************************************//
