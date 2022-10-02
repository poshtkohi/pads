/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_IO_DirectoryNotFoundException_h__
#define __System_IO_DirectoryNotFoundException_h__

#include "../../System/Exception/Exception.h"
#include "../../System/String/String.h"
#include "../IOException/IOException.h"

//**************************************************************************************************************//
namespace System
{
	//----------------------------------------------------
   namespace IO
   {
		class DirectoryNotFoundException : public IOException
		{
			//----------------------------------------------------
			public: DirectoryNotFoundException();
			public: DirectoryNotFoundException(const String &message);
			public: DirectoryNotFoundException(const String &message, const Exception *inner);
			public: String get_Message();
			//----------------------------------------------------
		};

	}
}
//**************************************************************************************************************//

#endif
