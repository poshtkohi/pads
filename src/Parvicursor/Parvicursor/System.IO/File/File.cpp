/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "File.h"

//**************************************************************************************************************//

namespace System
{
	namespace IO
	{
		//----------------------------------------------------
		void File::Delete(const String &path)
		{
			if(path.get_BaseStream() == null)
				throw ArgumentNullException("path");

			if(!File::Exists(path))
				new DirectoryNotFoundException("Could not find a part of the path " + path);

#if defined WIN32 || WIN64

			BOOL ret = ::DeleteFile(path.get_BaseStream());

			if(!ret)
				throw IOException(String(ErrorString(GetLastError())) +  + " for " + path);

#else

		int ret = remove(path.get_BaseStream());

        if(ret < 0)
			throw IOException(String((const char *)_sys_errlist[errno])  + " for " + path);


#endif

		}
		//----------------------------------------------------
		void File::Move(const String &sourceFileName, const String &destFileName)
		{
			if(sourceFileName.get_BaseStream() == null)
				throw ArgumentNullException("sourceFileName");

			if(destFileName.get_BaseStream() == null)
				throw ArgumentNullException("destFileName");

            if (sourceFileName == destFileName)
				throw IOException ("Source and destination path must be different.");

#if defined WIN32 || WIN64

			BOOL ret = ::MoveFile(sourceFileName.get_BaseStream(), destFileName.get_BaseStream());

			if(!ret)
			{
				//printf("GetLastError(): %d", GetLastError());
				//throw IOException(GetLastError());//(const char *)_sys_errlist[GetLastError()]);
				throw IOException(ErrorString(GetLastError()));
			}

#else

        int ret = rename(sourceFileName.get_BaseStream(), destFileName.get_BaseStream());

        if(ret < 0)
			throw IOException((const char *)_sys_errlist[errno]);


#endif

		}
		//----------------------------------------------------
		bool File::Exists(const String &path)
		{
			if(path.get_BaseStream() == null)
				throw ArgumentNullException("path");

#if defined WIN32 || WIN64

			int ret = ::GetFileAttributes(path.get_BaseStream());

			if(ret >= 0 && (ret&FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
				throw IOException(path + " is a directory. No a file.");

			if(ret == INVALID_FILE_ATTRIBUTES)
			{
				//printf("errno: %d\n", GetLastError());
				if(GetLastError() == ERROR_PATH_NOT_FOUND || GetLastError() == ERROR_FILE_NOT_FOUND)
					return false;
				else
					throw IOException(ErrorString(GetLastError()));
			}
			else
				return true;

#else

        struct stat st;
		int ret = stat(path.get_BaseStream(), &st);

		if(ret >=0 && (st.st_mode&S_IFREG) == S_IFREG)
			return true;

        if(ret >=0 && (st.st_mode&S_IFDIR) == S_IFDIR)
			throw IOException(path + " is a directory. No a file.");

		if(ret < 0)
		{
		    if(errno == ENOENT)
                return false;
            else
                throw IOException((const char *)_sys_errlist[errno]);
		}

		else
			return false;



#endif

		}
	    //----------------------------------------------------

	}
}
//**************************************************************************************************************//
