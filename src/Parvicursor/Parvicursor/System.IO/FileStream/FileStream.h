/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_IO_FileStream_h__
#define __System_IO_FileStream_h__

/*Int32 ReadFileInternal(Int32, void *, unsigned Int32);
Int32 WriteFileInternal(Int32, const void *, unsigned Int32);*/

#include "../../System/Object/Object.h"
#include "../IOException/IOException.h"
#include "../../System/ArgumentException/ArgumentException.h"
#include "../../System/ArgumentException/ArgumentNullException.h"
#include "../../System/ArgumentException/ArgumentOutOfRangeException.h"
#include "../../System/NotSupportedException/NotSupportedException.h"
#include "../../System/ObjectDisposedException/ObjectDisposedException.h"
#include "../../System/BasicTypes/BasicTypes.h"

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
//#include "mingw/share.h"

#if !defined WIN32 || WIN64 //including file system functions for unix.
#include <sys/io.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#else
#include <io.h>
#include <share.h>
#include <sys/locking.h>
#include <windows.h>
#endif


/*#ifndef __System_Exception_h__
#include "../../System/Exception/Exception.h"
#endif*/

#include "../../System/String/String.h"
#include "../IOException/IOException.h"
#include "../../StaticFunctions/StaticFunctions.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif



#if !defined ParvicursorLib_inlining
//#warning hello world
#ifndef __Parvicursor_inline__
#define __Parvicursor_inline__ inline
#endif
#else
#ifndef __Parvicursor_inline__
#define __Parvicursor_inline__
//#warning hello
#endif
#endif
//**************************************************************************************************************//

namespace System
{
	namespace IO
	{
		//----------------------------------------------------
		/*enum FileMode
		{
		    Append = O_APPEND ,
			Create = O_CREAT ,
			CreateNew = O_CREAT | O_EXCL ,
			Open = O_BINARY ,
			OpenOrCreate = O_CREAT ,
			Truncate = O_TRUNC
		};*/
		enum FileMode
		{
		    Append = 0 ,
			CreateNew = 1 ,
			Create = 2,
			Open = 3,
			OpenOrCreate = 4 ,
			Truncate = 5
		};
		//----------------------------------------------------
		enum FileAccess
		{
			Read = 6,
			ReadWrite = 7 ,
			Write = 8,
		};
		//----------------------------------------------------
/*#if defined WIN32 || WIN64
enum FileShare
{
	Inheritable = SH_DENYNO, //same ReadWrite selection
	None = SH_DENYRW,
	read = SH_DENYWR,
	readWrite = SH_DENYNO,
	write  = SH_DENYRD,
};
#else
enum FileShare
{
	Inheritable = 3, //same ReadWrite selection
	None = 1,
	read = 2,
	readWrite = 3,
	write  = 4,
};
#endif*/
		//----------------------------------------------------
		enum SeekOrigin
		{
			Begin = SEEK_SET,
			Current = SEEK_CUR,
			End = SEEK_END
		};
		//----------------------------------------------------

		class FileStream : public Object
        {
			private: Int32 fd;
		    private: String path;
		    private: FileMode mode;
			private: FileAccess access;
			private: Int64 position;
		    private: bool disposed;
			//private: char *one_byte;
			private: char *buf;				// the buffer
			private: Int32 buf_size;			// buffer size
			private: Int32 current_offset;	// current offset of buffer
			private: bool buf_dirty;		// true if buffer has been written to
			private: Int32 buf_length;			// number of valid bytes in buffer
		    //private: bool firstLocked;
#if defined WIN32 || WIN64
			private: HANDLE handle;
#endif
			//----------------------------------------------------
		    public: FileStream();
			public: ~FileStream();
		    public: FileStream(String path, FileMode mode, FileAccess access/*, FileShare share*/, Int32 bufferSize);
			public: FileStream(String path, FileMode mode, FileAccess access/*, FileShare share*/);
		    public: __Parvicursor_inline__ Int32 Read(char array[], Int32 offset, Int32 count);
		    public: __Parvicursor_inline__ void Write(const char array[], Int32 offset, Int32 count);
			/*public: void WriteByte(char value);
			public: Int32 ReadByte();*/
		    public: void Close();
			public: void SetLength(Int64 value);
			public: __Parvicursor_inline__ void Flush();
			public: void Lock(Int64 position, Int64 length);
			public: void UnLock(Int64 position, Int64 length);
			public: __Parvicursor_inline__ Int64 Seek(Int64 offset, SeekOrigin origin);
			public: Int64 get_Position();
		    public: void set_Position(Int64 position);
			public: __Parvicursor_inline__ String get_Name();
			public: __Parvicursor_inline__ Int64 get_Length();
			public: __Parvicursor_inline__ bool get_CanRead();
			//public: long get_CanSeek();
			public: __Parvicursor_inline__ Int32 get_Handle();
			public: __Parvicursor_inline__ bool get_CanWrite();
			private: inline void FlushBuffer();
			private: inline void RefillBuffer();
			private: inline Int32 ReadSegment(char array[], Int32 offset, Int32 count);
			private: inline void WriteSegment(const char array[], Int32 offset, Int32 count);
			//public: void operator delete(void *p);
			//----------------------------------------------------
      };


	};
};
//**************************************************************************************************************//

#endif
