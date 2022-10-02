/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "IOException.h"

//**************************************************************************************************************//
namespace System
{
	//----------------------------------------------------
   namespace IO
   {
		//----------------------------------------------------
		IOException::IOException() : Exception("I/O Error")
		{
		}
		//----------------------------------------------------
		IOException::IOException (const String &message) : Exception(message)
		{
		}
		//----------------------------------------------------
		IOException::IOException (const String &message, const Exception *inner) : Exception (message, inner)
		{
		}
		//----------------------------------------------------
		IOException::IOException(int error) : Exception("Unhandled Exception: System.IO.IOException: For more information see the ErrorCode and refer to Win or GNU file system API documentations.")
		{
			this->error = error;
		}
		//----------------------------------------------------
		int IOException::get_ErrorCode()
		{
			return this->error;
		}
		//----------------------------------------------------
		String IOException::get_Message()
		{
			String s = "Unhandled Exception: System.IO.IOException: " + Exception::get_Message() + ".";
			return s;//String::Copy(s);
		}
		//----------------------------------------------------

	};
};
//**************************************************************************************************************//
