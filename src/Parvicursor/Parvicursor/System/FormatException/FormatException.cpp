/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "FormatException.h"

//**************************************************************************************************************//
namespace System
{
	//----------------------------------------------------
	FormatException::~FormatException()
	{
	}
	//----------------------------------------------------
	FormatException::FormatException() : Exception("Invalid format.")
	{
	}
	//----------------------------------------------------
	FormatException::FormatException(const String &objectName): Exception("Invalid format.")
	{
		this->objectName = objectName;//String::Copy(objectName);
	}
	//----------------------------------------------------
	FormatException::FormatException(const String &objectName, const String &message) : Exception(message)
	{
		this->objectName = objectName;// String::Copy(objectName);
	}
	//----------------------------------------------------
	String FormatException::get_Message()
	{
		String s = "Unhandled Exception: System.FormatException: " + Exception::get_Message();
		if(objectName.get_BaseStream() != null)
			s += "\nObject name: " + objectName;
		return s;//String::Copy(s);
	}
	//----------------------------------------------------
};
//**************************************************************************************************************//
