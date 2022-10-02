/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "InvalidOperationException.h"

//**************************************************************************************************************//
namespace System
{
	//----------------------------------------------------
	InvalidOperationException::~InvalidOperationException()
	{
	}
	//----------------------------------------------------
	InvalidOperationException::InvalidOperationException() : Exception("Operation is not valid due to the current state of the object.")
	{
	}
	//----------------------------------------------------
	InvalidOperationException::InvalidOperationException(const String &message) : Exception(message)
	{
	}
	//----------------------------------------------------
	String InvalidOperationException::get_Message()
	{
		String s = "Unhandled Exception: System.InvalidOperationException: " + Exception::get_Message();
		return s;
	}
	//----------------------------------------------------
};
//**************************************************************************************************************//
