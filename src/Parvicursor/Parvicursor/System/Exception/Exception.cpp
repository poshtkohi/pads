/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include  "Exception.h"
//**************************************************************************************************************//
namespace System
{
	//----------------------------------------------------
	Exception::Exception(void)
	{
		inner_exception = null;
	}
	//----------------------------------------------------
	Exception::Exception(const String &msg)
	{
		inner_exception = null;
		set_Message(msg);
	}
	//----------------------------------------------------
	Exception::Exception(const String &msg, const Exception *e)
	{
		inner_exception = (Exception *)e;
		set_Message(msg);
	}
	//----------------------------------------------------
	Exception::~Exception()
	{
		if(inner_exception != null)
			delete inner_exception;
	}
	//----------------------------------------------------
	Exception *Exception::get_InnerException()
	{
		return inner_exception;
	}
	//----------------------------------------------------
	void Exception::set_Message(const String &msg)
	{
		message = msg;//String::Copy(msg);
	}
	//----------------------------------------------------
	String Exception::get_Message()
	{
		return this->message;//String::Copy(this->message);
	}
	//----------------------------------------------------
};
//**************************************************************************************************************//
