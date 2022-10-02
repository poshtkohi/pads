/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_IO_Directory_h__
#define __System_IO_Directory_h__


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
#include "../../System.Collections/ArrayList/ArrayList.h"
#include "../../System/String/String.h"
#include "../DirectoryNotFoundException/DirectoryNotFoundException.h"
#include "../../StaticFunctions/StaticFunctions.h"
#include "../File/File.h"


#if !defined WIN32 || WIN64 //including file system functions for unix.

#include <sys/io.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <dirent.h>
#include <string.h>

#else
#include <direct.h>
#include <io.h>
#include <share.h>
#include <sys/locking.h>
//#include <windows.h>
//#define _WIN32_WINNT 0x0501
#define BUFSIZE MAX_PATH
#include <stdio.h>
#include <malloc.h>
#include <tchar.h>
#include <wchar.h>
#include <strsafe.h>

#endif

//**************************************************************************************************************//

using namespace System::Collections;

namespace System
{
	namespace IO
	{
		//----------------------------------------------------

		class Directory : public Object
        {
			//----------------------------------------------------
			public: static void CreateDirectory(const String &path);
			public: static void Delete(const String &path);
			public: static void Delete(const String &path, bool recursive);
			private: static void RecursiveDelete(const String &dir);
			public:	static bool Exists(const String &path);
			public: static ArrayList *GetDirectories(const String &path);
			public: static ArrayList *GetFiles(const String &path);
			public: static void Move(const String &sourceDirName, const String &destDirName);
			public: static String GetCurrentDirectory();
			//----------------------------------------------------
      };


	};
};
//**************************************************************************************************************//

#endif
