/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "ArgumentOutOfRangeException.h"

//**************************************************************************************************************//
namespace System
{
	//----------------------------------------------------
	ArgumentOutOfRangeException::~ArgumentOutOfRangeException()
	{
	}
	//----------------------------------------------------
	ArgumentOutOfRangeException::ArgumentOutOfRangeException() : Exception("Argument is out of range.")
	{
	}
	//----------------------------------------------------
	ArgumentOutOfRangeException::ArgumentOutOfRangeException(const String &paramName): Exception("Argument is out of range.")
	{
		this->paramName = paramName;//String::Copy(paramName);
	}
	//----------------------------------------------------
	ArgumentOutOfRangeException::ArgumentOutOfRangeException(const String &paramName, const String &message) : Exception(message)
	{
		this->paramName = paramName;// String::Copy(paramName);
	}
	//----------------------------------------------------
	String ArgumentOutOfRangeException::get_Message()
	{
		String s = "Unhandled Exception: System.ArgumentOutOfRangeException: " + Exception::get_Message();
		if(paramName.get_BaseStream() != null)
			s += "\nParameter name: " + paramName;
		return s;//String::Copy(s);
	}
	//----------------------------------------------------
};
//**************************************************************************************************************//
