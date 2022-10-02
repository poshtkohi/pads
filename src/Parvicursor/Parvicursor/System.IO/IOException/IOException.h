/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_IO_IOException_h__
#define __System_IO_IOException_h__

#include "../../System/Exception/Exception.h"
#include "../../System/String/String.h"



//**************************************************************************************************************//
namespace System
{
	//----------------------------------------------------
   namespace IO
   {
		class IOException : public Exception
		{
			private: int error;
			//----------------------------------------------------
			public: IOException();
			public: IOException(const String &message);
			public: IOException(const String &message, const Exception *inner);
			public: IOException(int error);
			public: int get_ErrorCode();
			public: String get_Message();
			//----------------------------------------------------
		};

	}
}
//**************************************************************************************************************//

#endif
