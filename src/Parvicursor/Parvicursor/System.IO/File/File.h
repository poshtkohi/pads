/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_IO_File_h__
#define __System_IO_File_h__


#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>

#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
//#include "mingw/share.h"

#include "../../general.h"
#include "../../System/Object/Object.h"
#include "../IOException/IOException.h"
#include "../../System/ArgumentException/ArgumentException.h"
#include "../../System/ArgumentException/ArgumentNullException.h"
#include "../../System/ArgumentException/ArgumentOutOfRangeException.h"
#include "../../System/NotSupportedException/NotSupportedException.h"
#include "../../System/ObjectDisposedException/ObjectDisposedException.h"
#include "../../System/String/String.h"
#include "../DirectoryNotFoundException/DirectoryNotFoundException.h"
#include "../../StaticFunctions/StaticFunctions.h"


#if !defined WIN32 || WIN64 //including file system functions for unix.
#include <sys/io.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#else
#include <io.h>
#include <share.h>
#include <sys/locking.h>
#endif

//#include "../../System/Exception/Exception.h"


//**************************************************************************************************************//

namespace System
{
	namespace IO
	{
		//----------------------------------------------------

		class File : public Object
        {
			//----------------------------------------------------
			public: static void Delete(const String &path);
			public:	static bool Exists(const String &path);
			public: static void Move(const String &sourceFileName, const String &destFileName);
			//----------------------------------------------------
      };


	};
};
//**************************************************************************************************************//

#endif
