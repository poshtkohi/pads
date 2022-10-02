#define null 0
#include "stdafx.h"

#include "../../System/Object/Object.h"
#include "../../System/ArgumentException/ArgumentNullException.h"
#include "../../System/ArgumentException/ArgumentOutOfRangeException.h"
#include "../IOException/IOException.h"

#include "FileStream.h"


//**************************************************************************************************************//

namespace System
{
	namespace IO
	{
		//----------------------------------------------------
		FileStream::FileStream()
        {
			this->fd = -10; //throw ObjectDisposedException
	    }
        //----------------------------------------------------
		FileStream::FileStream(String path, FileMode mode, FileAccess access, FileShare share)
        {
			// add exceptions here
			// if the file has been closed, throw ObjectDisposedException
			if(path.get_BaseStream() == null)
				throw ArgumentNullException("path");
			if(mode != IO::Append && mode != IO::Create && mode != IO::CreateNew && mode != IO::Open && mode != IO::OpenOrCreate && mode != IO::Truncate)
				throw ArgumentOutOfRangeException("mode", "mode contains an invalid value.");
			if(access != IO::read && access != IO::readWrite && access != IO::write)
				throw ArgumentOutOfRangeException("access", "access contains an invalid value.");
			if(share != IO::Inheritable && share != IO::None && share != IO::Read && share != IO::ReadWrite && share != IO::Write)
				throw ArgumentOutOfRangeException("share", "share contains an invalid value.");
			String s = path.Replace("\\", "/");
#if defined WIN32 || WIN64
			if(share == IO::None || share == IO::Read || share == IO::Write)
				this->firstLocked = true;
			else
				this->firstLocked = false;
			fd = sopen(s.get_BaseStream(), mode, share, access);
			if(fd == -1)
				throw IOException((const char *)_sys_errlist[errno]);
#else
			/*fd = open(s.get_BaseStream(), mode, access);
			if(fd == -1)
				throw IOException((const char *)_sys_errlist[errno]);*/

			switch(share)
			{
				case IO::ReadWrite: //same Inheritable sharing mode
					fd = open(s.get_BaseStream(), O_RDWR, access);
					break;
				case IO::Read:
					fd = open(s.get_BaseStream(), O_RDONLY, access);
					break;
				case IO::Write:
					fd = open(s.get_BaseStream(), O_WRONLY, access);
					break;
				case IO::None:
                    printf("hello how are u?\n");
                    fd = open(s.get_BaseStream(), O_RDWR, access);
					//fd = open(s.get_BaseStream(), O_RDONLY | O_WRONLY, access);
					break;
			}
			if(fd == -1)
				throw IOException((const char *)_sys_errlist[errno]);
            /*if(share != IO::ReadWrite && share != IO::Inheritable)
            {
                int shareFlag;
                //locks: read locks and write locks (also referred to as shared locks and exclusive locks, respectively.)
                /if(share == IO::Read || share == IO::ReadWrite  || share == IO::Write || share == IO::Inheritable)
                {
                    if(share == IO::None || share == IO::Read || share == IO::Write)
                        this->firstLocked = true;
                    else
                        this->firstLocked = false;
                    shareFlag = LOCK_SH; // Shared lock.
                }
                else
                {
                    this->firstLocked = true;
                    shareFlag = LOCK_EX; // Exclusive lock.
                }/
                if(share == IO::Read)
                {
                    this->firstLocked = true;
                    shareFlag = LOCK_SH; // Shared lock.
                }
                if(share == IO::Write)
                {
                    this->firstLocked = false;
                    shareFlag = LOCK_EX; // Exclusive lock.
                }
                else
                {
                    this->firstLocked = true;
                    shareFlag = LOCK_SH | LOCK_EX; // Shared lock + Exclusive lock.
                }
                int lockErr = flock(this->fd, shareFlag);
                if(lockErr == -1)
                {
                    printf("hi, invalid argument");
                    throw IOException((const char *)_sys_errlist[errno]);
                }
            }
            else  this->firstLocked = false;*/
#endif
			printf("fd: %d\n", fd);
			this->disposed = false;
			this->path = s;
			//setmaxstdio Set a maximum for the number of simultaneously open files at the stream I/O level.
			//setvbuf(FILE *stream, char *buffer, int mode, size_t size );
			this->position = 0;
	    }
	    //----------------------------------------------------
		int FileStream::Read(char array[], int offset, int count)
		{
			//ObjectDisposedException
			// add exceptions here
			if(array == null)
				throw ArgumentNullException("array");
			if(offset < 0 || count < 0)
				throw ArgumentOutOfRangeException("offset or count", "offset or count is negative.");
			if(count == 0)
				throw ArgumentOutOfRangeException("count", "count is zero.");
			/*printf("count + offset: %d, size: %d\n", count + offset, sizeof(&array));
			if((count + offset)  > sizeof(array))
				throw ArgumentOutOfRangeException("offset + count", "offset plus count specify a position not within the array.");*/
			array += offset;
			int ret = ReadFileInternal(this->fd, array, count);
			if(ret == -1)
			{
				array -= offset;
				throw IOException((const char *)_sys_errlist[errno]);
			}
			if(ret > 0)
				this->position += ret;
			array -= offset;
			return ret;
		}
		//----------------------------------------------------
		void FileStream::Write(const char array[], int offset, int count)
		{
			// add exceptions here
			//ObjectDisposedException
			if(array == null)
				throw ArgumentNullException("array");
			if(offset < 0 || count < 0)
				throw ArgumentOutOfRangeException("offset or count", "offset or count is negative.");
			if(count == 0)
				throw ArgumentOutOfRangeException("count", "count is zero.");
			array += offset;
			int ret = WriteFileInternal(this->fd, array, count);
			if(ret == -1)
			{
				array -= offset;
				throw IOException((const char *)_sys_errlist[errno]);
			}
			if(ret > 0)
				this->position += ret;
			array -= offset;
		}
	    //----------------------------------------------------
		long FileStream::get_Length()
		{
			// add exceptions here
			//ObjectDisposedException
			long currentPosition = get_Position();
			int seekErr = lseek(this->fd , 0 , SEEK_END);
			if(seekErr == -1)
				throw IOException((const char *)_sys_errlist[errno]);
			long fileSize = get_Position();
			seekErr = lseek (this->fd , currentPosition , SEEK_SET);
			if(seekErr == -1)
				throw IOException((const char *)_sys_errlist[errno]);
			return fileSize;
		}
		//----------------------------------------------------
		String FileStream::get_Name()
		{
			return this->path;
		}
		//----------------------------------------------------
		/*void FileStream::Flush()
		{
			//ObjectDisposedException
			int commitErr = commit(this->fd);
			if(commitErr == -1)
				throw IOException((const char *)_sys_errlist[errno]);
		}*/
		//----------------------------------------------------
		/*void FileStream::SetLength(long value)
		{
			// add exceptions here
			//ArgumentOutOfRangeException
		}*/
		//Function: int truncate (const char *filename, off_t length) SetLength()
		//----------------------------------------------------
		//attention for implementation: Prevents other processes from changing the FileStream while permitting read access.
		void FileStream::Lock(long position, long length)
		{
			//printf("lock called\n");
			// add exceptions here
			//ArgumentOutOfRangeException
			if(!firstLocked)
			{
				printf("lock called\n");
				long currentPosition = get_Position();
				Seek(position, IO::Begin);
#if defined WIN32 || WIN64
				int closeErr = locking(this->fd, LK_LOCK, length);
				if(closeErr == -1)
						throw IOException((const char *)_sys_errlist[errno]);
#else
				struct flock fl;
				/*This is where you signify the type of lock you want to set.
				It's either F_RDLCK, F_WRLCK, or F_UNLCK if you want to set
				a read lock, write lock, or clear the lock, respectively.*/
				fl.l_type = F_WRLCK;  /* F_RDLCK, F_WRLCK, F_UNLCK   */
				fl.l_whence = SEEK_SET; /* SEEK_SET, SEEK_CUR, SEEK_END */
				fl.l_start = position;  /* Offset from l_whence  */
				fl.l_len = length;  /* length, 0 = to EOF  */
				fl.l_pid = getpid(); /* our PID  */
				int fcntlErr = fcntl(this->fd, F_SETLK, &fl);  /* F_GETLK, F_SETLK, F_SETLKW */
				if(fcntlErr == -1)
					throw IOException((const char *)_sys_errlist[errno]);
#endif
				Seek(currentPosition, IO::Begin);
			}
			//else printf("not locked\n");
		}
		//----------------------------------------------------
		void FileStream::Close()
		{
			if(!disposed)
			{
				disposed = true;
				int closeErr = close(this->fd);
				if(closeErr == -1)
					throw IOException((const char *)_sys_errlist[errno]);
			}
		}
		//----------------------------------------------------
		long FileStream::get_Position()
		{
			// add exceptions here
			//ObjectDisposedException
			return this->position;
		}
		//----------------------------------------------------
		void FileStream::set_Position(long position)
		{
			Seek(position, IO::Begin);
		}
		//----------------------------------------------------
		void FileStream::Seek(long offset, SeekOrigin origin)
		{
			// add exceptions here
			// add exception for undefined orgin parameter
			//ObjectDisposedException
			long seekErr = lseek(this->fd , 0 , origin);
			if(seekErr == -1)
				throw IOException((const char *)_sys_errlist[errno]);
			if(seekErr >= 0)
				this->position = seekErr;
		}
	    //----------------------------------------------------

	}
}
//**************************************************************************************************************//
int ReadFileInternal(int fd, void *array, unsigned int count)
{
	return read(fd, array, count);
}
//----------------------------------------------------
int WriteFileInternal(int fd, const void *array, unsigned int count)
{
	return write(fd, array, count);
}
//**************************************************************************************************************//
