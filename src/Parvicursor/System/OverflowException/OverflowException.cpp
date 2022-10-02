/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "OverflowException.h"

//**************************************************************************************************************//
namespace System
{
	//----------------------------------------------------
	OverflowException::~OverflowException()
	{
	}
	//----------------------------------------------------
	OverflowException::OverflowException() : Exception("Number overflow.")
	{
	}
	//----------------------------------------------------
	OverflowException::OverflowException(const String &objectName): Exception("Number overflow.")
	{
		this->objectName = objectName;//String::Copy(objectName);
	}
	//----------------------------------------------------
	OverflowException::OverflowException(const String &objectName, const String &message) : Exception(message)
	{
		this->objectName = objectName;// String::Copy(objectName);
	}
	//----------------------------------------------------
	String OverflowException::get_Message()
	{
		String s = "Unhandled Exception: System.OverflowException: " + Exception::get_Message();
		if(objectName.get_BaseStream() != null)
			s += "\nObject name: " + objectName;
		return s;//String::Copy(s);
	}
	//----------------------------------------------------
};
//**************************************************************************************************************//
