// FileStream.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "../../System/Exception/Exception.h"
#include "../../System/String/String.h"
#include "../../System.Threading/Thread/Thread.h"

//**************************************************************************************************************//

using namespace System;
using namespace System::Threading;

namespace System
{
	namespace IO
	{
        class FileStream
        {
		public:
		 enum FileMode
		    {
		       Append = O_APPEND ,
			   Create = O_CREAT ,
			   CreateNew = O_CREAT | O_EXCL ,
			   Open = O_BINARY ,
			   OpenOrCreate = O_CREAT ,
			   Truncate = O_TRUNC
			};
		    enum FileAccess
		    {
			   read = 0,
			   readWrite = 1,
			   write = 2,
			};
		    enum FileShare
		    {
			   Inheritable = 0,
			   None = 1,
			   _Read = 2,
			   ReadWrite = 3,
			   Write  = 4
		    };
			enum SeekOrigin
		    {
			   Begin = SEEK_SET,
			   Current = SEEK_CUR,
			   End = SEEK_END
			};
		private:
			//int fp;
			FILE *fp;
	        //**************************************************************************************************************//
		public:/*const String &path, FileMode mode, FileAccess access, FileShare share*/
			FileStream(String path, FileMode mode, FileAccess access, FileShare share)
	        {
				// add exceptions here
				// if the file has been closed, throw ObjectDisposedException
				/*if(mode == CreateNew)
					printf("hello world!\n");
				printf("value: %d\n", mode);
				fp = open("h:/test/test.txt", O_CREAT, S_IWRITE);
				//Thread::SleepThread(0);
				//printf("fp: %d\n", fp);
				close(fp);*/
				String s = path.Replace("\\", "/");
				fp = fopen(s.get_BaseStream(), "r");  //fsopen, for file sharing
				/*printf("fp: %d\n", fp);
				printf("str: %s\n", s.get_BaseStream());*/
				//setmaxstdio Set a maximum for the number of simultaneously open files at the stream I/O level.
				//setvbuf(FILE *stream, char *buffer, int mode, size_t size );
	        }
	        //**************************************************************************************************************//
		    public: int Read(char array[], int offset, int count)
			{
				// add exceptions here
				array += offset;
				int ret = (int)fread(array, sizeof(char), (size_t)count, this->fp);
				array -= offset;
				return ret;
			}
	        //**************************************************************************************************************//
			public: long get_Length()
			{
				// add exceptions here
				long currentPosition = ftell(this->fp);
				int seekErr = fseek(this->fp , 0 , SEEK_END);
				long fileSize = ftell(this->fp);
				seekErr = fseek (this->fp , currentPosition , SEEK_SET);
				//if(fileSize == -1)
				return fileSize;
			}
			 //**************************************************************************************************************//
			public: void Seek(long offset, SeekOrigin origin)
			{
				// add exceptions here
				// add exception for undefined orgin parameter
				int seekErr = fseek(this->fp , 0 , origin);
				//if(seekErr == -1)
			}
	        //**************************************************************************************************************//
      };
	}
}
//**************************************************************************************************************//
