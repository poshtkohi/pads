/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "DirectoryNotFoundException.h"

//**************************************************************************************************************//
namespace System
{
	//----------------------------------------------------
   namespace IO
   {
		//----------------------------------------------------
		DirectoryNotFoundException::DirectoryNotFoundException() : IOException("I/O Error")
		{
		}
		//----------------------------------------------------
		DirectoryNotFoundException::DirectoryNotFoundException (const String &message) : IOException(message)
		{
		}
		//----------------------------------------------------
		DirectoryNotFoundException::DirectoryNotFoundException (const String &message, const Exception *inner) : IOException (message, inner)
		{
		}
		//----------------------------------------------------
		String DirectoryNotFoundException::get_Message()
		{
			String s = "Unhandled Exception: System.IO.DirectoryNotFoundException: " + Exception::get_Message() + ".";
			return s;//String::Copy(s);
		}
		//----------------------------------------------------

	};
};
//**************************************************************************************************************//
