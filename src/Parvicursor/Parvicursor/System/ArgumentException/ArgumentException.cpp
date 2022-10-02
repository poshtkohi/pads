/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "ArgumentException.h"

//**************************************************************************************************************//
namespace System
{
	//----------------------------------------------------
	ArgumentException::~ArgumentException()
	{
	}
	//----------------------------------------------------
	ArgumentException::ArgumentException() : Exception("An invalid argument was specified.")
	{
	}
	//----------------------------------------------------
	ArgumentException::ArgumentException(const String &paramName): Exception("An invalid argument was specified.")
	{
		this->paramName = paramName;//String::Copy(paramName);
	}
	//----------------------------------------------------
	ArgumentException::ArgumentException(const String &paramName, const String &message) : Exception(message)
	{
		this->paramName = paramName;// String::Copy(paramName);
	}
	//----------------------------------------------------
	String ArgumentException::get_Message()
	{
		String s = "Unhandled Exception: System.ArgumentException: " + Exception::get_Message();
		if(paramName.get_BaseStream() != null)
			s += "\nParameter name: " + paramName;
		return s;//String::Copy(s);
	}
	//----------------------------------------------------
};
//**************************************************************************************************************//
