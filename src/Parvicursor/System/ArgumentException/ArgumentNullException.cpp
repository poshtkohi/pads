/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "ArgumentNullException.h"

//**************************************************************************************************************//
namespace System
{
	//----------------------------------------------------
	ArgumentNullException::~ArgumentNullException()
	{
	}
	//----------------------------------------------------
	ArgumentNullException::ArgumentNullException() : Exception("Argument cannot be null.")
	{
	}
	//----------------------------------------------------
	ArgumentNullException::ArgumentNullException(const String &paramName): Exception("Argument cannot be null.")
	{
		this->paramName = paramName;//String::Copy(paramName);
	}
	//----------------------------------------------------
	ArgumentNullException::ArgumentNullException(const String &paramName, const String &message) : Exception(message)
	{
		this->paramName = paramName;// String::Copy(paramName);
	}
	//----------------------------------------------------
	String ArgumentNullException::get_Message()
	{
		String s = "Unhandled Exception: System.ArgumentNullException: " + Exception::get_Message();
		if(paramName.get_BaseStream() != null)
			s += "\nParameter name: " + paramName;
		return s;//String::Copy(s);
	}
	//----------------------------------------------------
};
//**************************************************************************************************************//
