/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "ObjectDisposedException.h"

//**************************************************************************************************************//
namespace System
{
	//----------------------------------------------------
	ObjectDisposedException::~ObjectDisposedException()
	{
	}
	//----------------------------------------------------
	ObjectDisposedException::ObjectDisposedException() : Exception("The object was used after being disposed.")
	{
	}
	//----------------------------------------------------
	ObjectDisposedException::ObjectDisposedException(const String &objectName): Exception("The object was used after being disposed.")
	{
		this->objectName = objectName;//String::Copy(objectName);
	}
	//----------------------------------------------------
	ObjectDisposedException::ObjectDisposedException(const String &objectName, const String &message) : Exception(message)
	{
		this->objectName = objectName;// String::Copy(objectName);
	}
	//----------------------------------------------------
	String ObjectDisposedException::get_Message()
	{
		String s = "Unhandled Exception: System.ObjectDisposedException: " + Exception::get_Message();
		if(objectName.get_BaseStream() != null)
			s += "\nObject name: " + objectName;
		return s;//String::Copy(s);
	}
	//----------------------------------------------------
};
//**************************************************************************************************************//
