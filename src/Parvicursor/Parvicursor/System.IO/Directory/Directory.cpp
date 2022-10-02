/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "Directory.h"

//**************************************************************************************************************//

namespace System
{
	namespace IO
	{
		//----------------------------------------------------
		void Directory::CreateDirectory(const String &path)
		{
			if(path.get_BaseStream() == null)
				throw ArgumentNullException("path");

			if(Directory::Exists(path))
				return ;

#if defined WIN32 || WIN64

			BOOL ret = ::CreateDirectory(path.get_BaseStream(), null);

			if(!ret)
			{
				switch(GetLastError())
                {
                    case ERROR_ALREADY_EXISTS:
                        throw IOException("The specified directory already exists.");
                        break;
                    case ERROR_PATH_NOT_FOUND:
                        throw DirectoryNotFoundException("The specified path is invalid (for example, it is on an unmapped drive). ");
                        break;
                    default:
                        throw IOException(ErrorString(GetLastError()));
                }
			}

#else

        int ret = mkdir(path.get_BaseStream(), S_IRWXU);

        if(ret < 0)
        {
            switch(errno)
            {
                case EEXIST:
                    throw IOException("The specified directory already exists.");
                    break;
                case ENOENT:
                    throw DirectoryNotFoundException("The specified path is invalid (for example, it is on an unmapped drive). ");
                    break;
                default:
                    throw IOException((const char *)_sys_errlist[errno]);
            }
        }


#endif

		}
		//----------------------------------------------------
		void Directory::Delete(const String &path)
		{
			if(path.get_BaseStream() == null)
				throw ArgumentNullException("path");

			if(!Directory::Exists(path))
				new DirectoryNotFoundException("Could not find a part of the path " + path);

#if defined WIN32 || WIN64

			BOOL ret = ::RemoveDirectory(path.get_BaseStream());

			if(!ret)
				throw IOException(ErrorString(GetLastError()));

#else

			int ret = ::rmdir(path.get_BaseStream());

        if(ret < 0)
        {
            switch(errno)
            {
                case ENOENT:
                    throw DirectoryNotFoundException("The specified path is invalid (for example, it is on an unmapped drive). ");
                    break;
                default:
                    throw IOException((const char *)_sys_errlist[errno]);
            }
        }


#endif

		}
		//----------------------------------------------------
		void Directory::Delete(const String &path, bool recursive)
		{
			if(recursive)
			{
				if(!Directory::Exists(path))
					throw DirectoryNotFoundException(path + " is invalid or is not existed(for example, it is on an unmapped drive).");

				ArrayList *files;
				String *ss;
				try
				{
					files = Directory::GetFiles(path);

					if(files != null)
					{
						for(register Int32 j = 0 ; j < files->get_Count() ; j++)
						{
							ss = (String *)files->get_Value(j);
							//printf("%s\n", ss->get_BaseStream());
							File::Delete(*ss);
							delete ss;
						}
						files->Clear();
						delete files;
					}
				}
				catch(Exception e)
				{
					if(ss != null)
						delete ss;
					if(files != null)
					{
						files->Clear();
						delete files;
					}
					throw e;
					return;
				}

				RecursiveDelete(path);
				Directory::Delete(path);
			}
			else
				Directory::Delete(path);
		}
		//----------------------------------------------------
		void Directory::RecursiveDelete(const String &dir)
		{
			ArrayList *dirs = Directory::GetDirectories(dir);

			if(dirs == null)
				return;

			for(register Int32 i = 0 ; i < dirs->get_Count() ; i++)
			{
				String *ss;
				String *s = (String *)dirs->get_Value(i);
				ArrayList *files;
				try
				{
					files = Directory::GetFiles(*s);

					if(files != null)
					{
						for(register Int32 j = 0 ; j < files->get_Count() ; j++)
						{
							ss = (String *)files->get_Value(j);
							//printf("%s\n", ss->get_BaseStream());
							File::Delete(*ss);
							delete ss;
						}
						files->Clear();
						delete files;
					}
				}
				catch(System::Exception &e)
				{
					if(s != null)
						delete s;
					if(ss != null)
						delete ss;
					if(files != null)
					{
						files->Clear();
						delete files;
					}
					if(dirs != null)
					{
						dirs->Clear();
						delete dirs;
					}
					throw Exception(e.get_Message());
					return;
				}
				//printf("%s\n", s->get_BaseStream());
				RecursiveDelete(*s);
				Directory::Delete(*s);
				if(s != null)
					delete s;
			}

			dirs->Clear();
			delete dirs;

		}
		/*void Directory::RecursiveDelete(const String &dir)
		{
			ArrayList *dirs = Directory::GetDirectories(dir);

			if(dirs == null)
				return;

			for(int i = 0 ; i < dirs->get_Count() ; i++)
			{
				String *s = (String *)dirs->get_Value(i);
				ArrayList *files = Directory::GetFiles(*s);

				if(files != null)
				{
					for(int j = 0 ; j < files->get_Count() ; j++)
					{
						String *ss = (String *)files->get_Value(j);
						printf("%s\n", ss->get_BaseStream());
						delete ss;
					}
					files->Clear();
					delete files;
				}
				//printf("%s\n", s->get_BaseStream());
				RecursiveDelete(*s);
				delete s;
			}

			dirs->Clear();
			delete dirs;

		}*/
		//----------------------------------------------------
		bool Directory::Exists(const String &path)
		{
			if(path.get_BaseStream() == null)
				throw ArgumentNullException("path");

#if defined WIN32 || WIN64

			int ret = ::GetFileAttributes(path.get_BaseStream());

			if(ret >= 0 && (ret&FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
				return true;


			if(ret == INVALID_FILE_ATTRIBUTES)
			{
				if(GetLastError() == ERROR_PATH_NOT_FOUND || GetLastError() == ERROR_FILE_NOT_FOUND)
					return false;
				else
					throw IOException(ErrorString(GetLastError()));
			}
			else
				throw IOException(path + " is a file. No a directory.");

#else

        struct stat st;
		int ret = stat(path.get_BaseStream(), &st);

		if(ret >=0 && (st.st_mode&S_IFDIR) == S_IFDIR)
			return true;

        if(ret >=0 && (st.st_mode&S_IFREG) == S_IFREG)
			throw IOException(path + " is a file. No a directory.");

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
		ArrayList *Directory::GetFiles(const String &path)
		{
			if(path.get_BaseStream() == null)
				throw ArgumentNullException("path");

			if(!Directory::Exists(path))
				throw DirectoryNotFoundException(path + " is invalid or is not existed(for example, it is on an unmapped drive).");

			ArrayList *files = null;
//			ArrayList *directories = null;

#if defined WIN32 || WIN64

			WIN32_FIND_DATA FindFileData;
			HANDLE hFind = INVALID_HANDLE_VALUE;
			DWORD dwError;
			LPTSTR DirSpec;
			size_t length_of_arg;
			INT retval;


			DirSpec = (LPTSTR) malloc (BUFSIZE);

			if( DirSpec == NULL )
			{
				//printf( "Insufficient memory available\n" );
				retval = 1;
				goto Cleanup;
			}

			// Check that the input is not larger than allowed.
			StringCbLength(path.get_BaseStream(), BUFSIZE, &length_of_arg);

			if (length_of_arg > (BUFSIZE - 2))
			{
				//_tprintf(TEXT("Input directory is too large.\n"));
				retval = 3;
				goto Cleanup;
			}

			//_tprintf (TEXT("Target directory is %s.\n"), path.get_BaseStream());

			// Prepare string for use with FindFile functions.  First,
			// copy the string to a buffer, then append '\*' to the
			// directory name.
			::StringCbCopyN (DirSpec, BUFSIZE, path.get_BaseStream(), length_of_arg+1);
			::StringCbCatN (DirSpec, BUFSIZE, TEXT("\\*"), 2*sizeof(TCHAR));

			// Find the first file in the directory.
			hFind = ::FindFirstFile(DirSpec, &FindFileData);

			if (hFind == INVALID_HANDLE_VALUE)
			{
				/*_tprintf (TEXT("Invalid file handle. Error is %u.\n"),
							GetLastError());*/
				retval = (-1);
			}
			else
			{
				/*_tprintf (TEXT("First file name is: %s\n"),
							FindFileData.cFileName);*/

				// List all the other files in the directory.
				while (::FindNextFile(hFind, &FindFileData) != 0)
				{
					/*printf("hFind: %d\n", hFind);
					_tprintf (TEXT("Next file name is: %s\n"),
							FindFileData.cFileName);*/

					if((FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY )//||
						//(data.dwFileAttributes & mask) == attrs
						//(FindFileData.dwFileAttributes == (FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_REPARSE_POINT)))
					{
						//printf("im a directory\n");

//						if(directories == null)
//							directories = new ArrayList();
//						String *s = new String(path + "/" + FindFileData.cFileName);
//						directories->Add((Object *)s);
						continue;
					}
					else
					{
						//printf("im a file\n");

						if(files == null)
							files = new ArrayList();
						String *s = new String(path + "/" + FindFileData.cFileName);
						files->Add((Object *)s);
					}

					/*if(ret == INVALID_FILE_ATTRIBUTES)
						break;

					else
						int h = 1;*/
						//files->Add((Object *)&String(FindFileData.cFileName));
				}

				dwError = ::GetLastError();
				::FindClose(hFind);
				if (dwError != ERROR_NO_MORE_FILES)
				{
					/*(_tprintf (TEXT("FindNextFile error. Error is %u.\n"),
							dwError);*/
					retval = (-1);
					goto Cleanup;
				}
			}

			retval  = 0;

			Cleanup:
			::free(DirSpec);
			//return retval;


			switch(retval)
			{
				case 1:
					throw IOException("Insufficient memory available");
				case 3:
					throw IOException("Input directory is too large");
				default:
					break;
			}

			if(retval < 0 )
			{
				if(files != null)
				{
				    files->Clear();
					delete files;
				}

//				if(directories != null)
//					delete directories;

				throw IOException(String(ErrorString(GetLastError())) + " for " + path);
			}

#else

            bool errorOccured = false;
            int err = 0;
            DIR *dir;
			dir = ::opendir(path.get_BaseStream());

            if(dir == null)
                throw IOException(String((const char *)_sys_errlist[errno]) + " for " + path);

            struct dirent *dp;          /* returned from readdir() */
            //int len;
            //bool one = false, two = false;

			while ((dp = ::readdir (dir)) != NULL)
            {

                if(dp == null && errno<=0)
                    break;

                if(dp == null && errno>0)
                {
                    err = errno;
                    errorOccured = true;
                    break;
                }

                if(dp->d_type == DT_REG)
                {
                    /*if(one && two)
                        goto here;

                    if(!one || !two)
                    {
                        len = strlen(dp->d_name);

                        if(len == 1 || len == 2)
                        {
                            if(len == 1)
                            {
                                if(dp->d_name[0] == '.')
                                {
                                    one = true;
                                    continue;
                                }
                            }

                            if(len == 2)
                            {
                                if(dp->d_name[0] == '.' && dp->d_name[1] == '.')
                                {
                                    two = true;
                                    continue;
                                }
                            }
                        }
                    }
                here:*/
                    if(files == null)
                        files = new ArrayList();
                    String *s = new String(path + "/" + dp->d_name);
                    files->Add((Object *)s);
                }
            }

            if(closedir(dir) < 0)
            {
                    err = errno;
                    errorOccured = true;
            }
            if(errorOccured)
            {
                if(files != null)
				{
				    files->Clear();
					delete files;
				}
                throw IOException(String((const char *)_sys_errlist[errno])  + " for " + path);
            }

#endif

			return files;
		}
		//----------------------------------------------------
		ArrayList *Directory::GetDirectories(const String &path)
		{
			if(path.get_BaseStream() == null)
				throw ArgumentNullException("path");

			if(!Directory::Exists(path))
				throw DirectoryNotFoundException(path + " is invalid or is not existed(for example, it is on an unmapped drive).");

//			ArrayList *files = null;
			ArrayList *directories = null;

#if defined WIN32 || WIN64

			WIN32_FIND_DATA FindFileData;
			HANDLE hFind = INVALID_HANDLE_VALUE;
			DWORD dwError;
			LPTSTR DirSpec;
			size_t length_of_arg;
			INT retval;


			DirSpec = (LPTSTR)::malloc(BUFSIZE);

			if( DirSpec == NULL )
			{
				//printf( "Insufficient memory available\n" );
				retval = 1;
				goto Cleanup;
			}

			// Check that the input is not larger than allowed.
			::StringCbLength(path.get_BaseStream(), BUFSIZE, &length_of_arg);

			if (length_of_arg > (BUFSIZE - 2))
			{
				//_tprintf(TEXT("Input directory is too large.\n"));
				retval = 3;
				goto Cleanup;
			}

			//_tprintf (TEXT("Target directory is %s.\n"), path.get_BaseStream());

			// Prepare string for use with FindFile functions.  First,
			// copy the string to a buffer, then append '\*' to the
			// directory name.
			::StringCbCopyN(DirSpec, BUFSIZE, path.get_BaseStream(), length_of_arg+1);
			::StringCbCatN(DirSpec, BUFSIZE, TEXT("\\*"), 2*sizeof(TCHAR));

			// Find the first file in the directory.
			hFind = ::FindFirstFile(DirSpec, &FindFileData);

			if (hFind == INVALID_HANDLE_VALUE)
			{
				/*_tprintf (TEXT("Invalid file handle. Error is %u.\n"),
							GetLastError());*/
				retval = (-1);
			}
			else
			{
				/*_tprintf (TEXT("First file name is: %s\n"),
							FindFileData.cFileName);*/

				// List all the other files in the directory.
				int i = 0;
				while(::FindNextFile(hFind, &FindFileData) != 0)
				{
					/*printf("hFind: %d\n", hFind);
					_tprintf (TEXT("Next file name is: %s\n"),
							FindFileData.cFileName);*/

					if((FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY )//||
						//(data.dwFileAttributes & mask) == attrs
						//(FindFileData.dwFileAttributes == (FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_HIDDEN|FILE_ATTRIBUTE_REPARSE_POINT)))
					{
						//printf("im a directory\n");
						if(i == 0)
						{
							i++;
							continue;
						}

						if(directories == null)
							directories = new ArrayList();
						String *s = new String(path + "/" + FindFileData.cFileName);
						directories->Add((Object *)s);
					}
					else
					{
						//printf("im a file\n");

//						if(files == null)
//							files = new ArrayList();
//						String *s = new String(path + "/" + FindFileData.cFileName);
//						files->Add((Object *)s);
						continue;
					}

					i++;
					/*if(ret == INVALID_FILE_ATTRIBUTES)
						break;

					else
						int h = 1;*/
						//files->Add((Object *)&String(FindFileData.cFileName));
				}

				dwError = ::GetLastError();
				::FindClose(hFind);
				if (dwError != ERROR_NO_MORE_FILES)
				{
					/*(_tprintf (TEXT("FindNextFile error. Error is %u.\n"),
							dwError);*/
					retval = (-1);
					goto Cleanup;
				}
			}

			retval  = 0;

			Cleanup:
			::free(DirSpec);
			//return retval;


			switch(retval)
			{
				case 1:
					throw IOException("Insufficient memory available");
				case 3:
					throw IOException("Input directory is too large");
				default:
					break;
			}

			if(retval < 0 )
			{
//				if(files != null)
//					delete files;

				if(directories != null)
				{
				    directories->Clear();
					delete directories;
				}

				throw IOException(String(ErrorString(GetLastError()))  + " for " + path);
			}

#else

            bool errorOccured = false;
            int err = 0;
            DIR *dir;
			dir = ::opendir(path.get_BaseStream());

            if(dir == null)
                throw IOException(String((const char *)_sys_errlist[errno]) + " for " + path);

            struct dirent *dp;          /* returned from readdir() */
            int len;
            bool one = false, two = false;

			while ((dp = ::readdir (dir)) != NULL)
            {

                if(dp == null && errno<=0)
                    break;

                if(dp == null && errno>0)
                {
                    err = errno;
                    errorOccured = true;
                    break;
                }

                if(dp->d_type == DT_DIR)
                {
                    if(one && two)
                        goto here;

                    if(!one || !two)
                    {
						len = ::strlen(dp->d_name);

                        if(len == 1 || len == 2)
                        {
                            if(len == 1)
                            {
                                if(dp->d_name[0] == '.')
                                {
                                    one = true;
                                    continue;
                                }
                            }

                            if(len == 2)
                            {
                                if(dp->d_name[0] == '.' && dp->d_name[1] == '.')
                                {
                                    two = true;
                                    continue;
                                }
                            }
                        }
                    }
                here:
                    if(directories == null)
                        directories = new ArrayList();
                    String *s = new String(path + "/" + dp->d_name);
                    directories->Add((Object *)s);
                }
            }

            if(closedir(dir) < 0)
            {
                    err = errno;
                    errorOccured = true;
            }
            if(errorOccured)
            {
                if(directories != null)
				{
				    directories->Clear();
					delete directories;
				}
                throw IOException(String((const char *)_sys_errlist[errno]) + " for " + path);
            }

#endif

			return directories;
		}
		//----------------------------------------------------
		void Directory::Move(const String &sourceDirName, const String &destDirName)
		{
			if(sourceDirName.get_BaseStream() == null)
				throw ArgumentNullException("sourceDirName");

			if(destDirName.get_BaseStream() == null)
				throw ArgumentNullException("destDirName");

			if (sourceDirName == destDirName)
				throw IOException ("Source and destination path must be different.");

			if (Directory::Exists(destDirName))
				throw IOException (destDirName + " already exists.");

			if (!Directory::Exists(sourceDirName) && !File::Exists(sourceDirName))
				throw new DirectoryNotFoundException (sourceDirName + " does not exist");


#			if defined WIN32 || WIN64

			BOOL ret = ::MoveFile(sourceDirName.get_BaseStream(), destDirName.get_BaseStream());

			if(!ret)
				throw IOException(ErrorString(GetLastError()));


#			else

			int ret = ::rename(sourceDirName.get_BaseStream(), destDirName.get_BaseStream());

        if(ret < 0)
			throw IOException((const char *)_sys_errlist[errno]);


#			endif

		}
		//----------------------------------------------------
		String Directory::GetCurrentDirectory()
		{
			Int32 ini = 256;
			register Int32 i = 1;
			char *dir = null;
		here:
			dir = new char[i * ini];
			if(getcwd(dir, i * ini) == null)
			{
				if(errno == ERANGE)
				{
					i++;
					if(dir != null)
						delete dir;

					goto here;
				}
				else
				{

#					if defined WIN32 || WIN64

					throw IOException(ErrorString(errno));


#					else

					throw IOException((const char *)_sys_errlist[errno]);


#					endif

				}
			}

			return String(dir);
		}
	    //----------------------------------------------------

	}
}
//**************************************************************************************************************//
