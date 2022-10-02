/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "NotSupportedException.h"

//**************************************************************************************************************//
namespace System
{
	//----------------------------------------------------
	NotSupportedException::~NotSupportedException()
	{
	}
	//----------------------------------------------------
	NotSupportedException::NotSupportedException() : Exception("Operation is not supported.")
	{
	}
	//----------------------------------------------------
	NotSupportedException::NotSupportedException(const String &message) : Exception(message)
	{
	}
	//----------------------------------------------------
	String NotSupportedException::get_Message()
	{
		String s = "Unhandled Exception: System.NotSupportedException: " + Exception::get_Message();
		return s;//String::Copy(s);
	}
	//----------------------------------------------------
};
//**************************************************************************************************************//
