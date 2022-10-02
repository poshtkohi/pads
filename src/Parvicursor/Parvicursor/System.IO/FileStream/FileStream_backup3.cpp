#define null 0
#include "stdafx.h"

#include "../../System/Object/Object.h"
#include "../../System/ArgumentException/ArgumentException.h"
#include "../../System/ArgumentException/ArgumentNullException.h"
#include "../../System/ArgumentException/ArgumentOutOfRangeException.h"
#include "../../System/NotSupportedException/NotSupportedException.h"
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
		FileStream::~FileStream()
        {
			if(!disposed)
				Close();
	    }
		//----------------------------------------------------
		FileStream::FileStream(String path, FileMode mode, FileAccess access/*, FileShare share*/, int bufferSize)
		{
			// add exceptions here
			// if the file has been closed, throw ObjectDisposedException
			if(path.get_BaseStream() == null)
				throw ArgumentNullException("path");
			if(mode != IO::Append && mode != IO::Create && mode != IO::CreateNew && mode != IO::Open && mode != IO::OpenOrCreate && mode != IO::Truncate)
				throw ArgumentOutOfRangeException("mode", "mode contains an invalid value.");
			if(access != IO::Read && access != IO::ReadWrite && access != IO::Write)
				throw ArgumentOutOfRangeException("access", "access contains an invalid value.");
			if(bufferSize <= 0)
				throw ArgumentOutOfRangeException("bufferSize", "bufferSize is zero or negative.");
			/*if(share != IO::Inheritable && share != IO::None && share != IO::Read && share != IO::ReadWrite && share != IO::Write)
				throw ArgumentOutOfRangeException("share", "share contains an invalid value.");*/
			String s = path.Replace("\\", "/");
			switch(mode)
			{
				case IO::Open:
					if(access == IO::Read)
						fd = open(s.get_BaseStream(), O_RDONLY);
					if(access == IO::Write)
						fd = open(s.get_BaseStream(), O_WRONLY);
					if(access == IO::ReadWrite)
						fd = open(s.get_BaseStream(), O_RDWR);
					break;
				case IO::Create:
					if(access == IO::Read)
						fd = open(s.get_BaseStream(), O_CREAT | O_RDONLY , S_IREAD);
					if(access == IO::Write)
						fd = open(s.get_BaseStream(), O_CREAT | O_WRONLY, S_IWRITE);
					if(access == IO::ReadWrite)
						fd = open(s.get_BaseStream(), O_CREAT | O_RDWR, S_IREAD | S_IWRITE);
					break;
				case IO::OpenOrCreate:
					if(access == IO::Read)
						fd = open(s.get_BaseStream(), O_CREAT | O_RDONLY , S_IREAD);
					if(access == IO::Write)
						fd = open(s.get_BaseStream(), O_CREAT | O_WRONLY, S_IWRITE);
					if(access == IO::ReadWrite)
						fd = open(s.get_BaseStream(), O_CREAT | O_RDWR, S_IREAD | S_IWRITE);
					break;
				case IO::CreateNew:
					if(access == IO::Read)
						fd = open(s.get_BaseStream(), O_CREAT | O_EXCL | O_RDONLY , S_IREAD);
					if(access == IO::Write)
						fd = open(s.get_BaseStream(), O_CREAT | O_EXCL | O_WRONLY, S_IWRITE);
					if(access == IO::ReadWrite)
						fd = open(s.get_BaseStream(), O_CREAT | O_EXCL | O_RDWR, S_IREAD | S_IWRITE);
					break;
				case IO::Append:
					if(access == IO::Write)
						fd = open(s.get_BaseStream(), O_APPEND | O_WRONLY);
					else 
						throw ArgumentException("access", "An invalid FileAccess mode was specified for a file opened with Append.");
					break;
				case IO::Truncate:
					if(access == IO::Write)
						fd = open(s.get_BaseStream(), O_TRUNC | O_WRONLY);
					else 
						throw ArgumentException("access", "An invalid FileAccess mode was specified for a file opened with Truncate.");
					break;
			}
/*#if defined WIN32 || WIN64
			if(share == IO::None || share == IO::Read || share == IO::Write)
				this->firstLocked = true;
			else
				this->firstLocked = false;
			fd = sopen(s.get_BaseStream(), mode, share, access);
			if(fd == -1)
				throw IOException((const char *)_sys_errlist[errno]);
#else
			fd = open(s.get_BaseStream(), mode, access);
			if(fd == -1)
				throw IOException((const char *)_sys_errlist[errno]);

			if(fd == -1)
				throw IOException((const char *)_sys_errlist[errno]);
            if(share != IO::ReadWrite && share != IO::Inheritable)
            {
                int shareFlag;
                //locks: read locks and write locks (also referred to as shared locks and exclusive locks, respectively.)
                if(share == IO::Read || share == IO::ReadWrite  || share == IO::Write || share == IO::Inheritable)
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
                }
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
            else  this->firstLocked = false;
#endif*/
			//printf("path: %s\ns: %s\n", path.get_BaseStream(), s.get_BaseStream());
			if(fd == -1)
				throw IOException((const char *)_sys_errlist[errno]);
			//printf("fd: %d\n", fd);
			this->disposed = false;
			this->path = s;
			this->mode = mode;
			this->access = access;
			//this->DefaultBufferSize = 8192;
			this->buf_size = bufferSize;
			this->buf = new char[this->buf_size];
			this->buf_dirty = false;
			this->buf_length = 0;
			this->current_offset = 0;
			this->position = 0;
			//printf("buffer size: %d\n", this->buf_size);
			//setmaxstdio Set a maximum for the number of simultaneously open files at the stream I/O level.
			//setvbuf(FILE *stream, char *buffer, int mode, size_t size );
		}
		 //----------------------------------------------------
		FileStream::FileStream(String path, FileMode mode, FileAccess access/* FileShare share*/)
        {
			// add exceptions here
			// if the file has been closed, throw ObjectDisposedException
			if(path.get_BaseStream() == null)
				throw ArgumentNullException("path");
			if(mode != IO::Append && mode != IO::Create && mode != IO::CreateNew && mode != IO::Open && mode != IO::OpenOrCreate && mode != IO::Truncate)
				throw ArgumentOutOfRangeException("mode", "mode contains an invalid value.");
			if(access != IO::Read && access != IO::ReadWrite && access != IO::Write)
				throw ArgumentOutOfRangeException("access", "access contains an invalid value.");
			String s = path.Replace("\\", "/");
			switch(mode)
			{
				case IO::Open:
					if(access == IO::Read)
						fd = open(s.get_BaseStream(), O_RDONLY | O_BINARY);
					if(access == IO::Write)
						fd = open(s.get_BaseStream(), O_WRONLY | O_BINARY);
					if(access == IO::ReadWrite)
						fd = open(s.get_BaseStream(), O_RDWR | O_BINARY);
					break;
				case IO::Create:
					if(access == IO::Read)
						fd = open(s.get_BaseStream(), O_CREAT | O_RDONLY  | O_BINARY, S_IREAD);
					if(access == IO::Write)
						fd = open(s.get_BaseStream(), O_CREAT | O_WRONLY | O_BINARY, S_IWRITE);
					if(access == IO::ReadWrite)
						fd = open(s.get_BaseStream(), O_CREAT | O_RDWR | O_BINARY, S_IREAD | S_IWRITE);
					break;
				case IO::OpenOrCreate:
					if(access == IO::Read)
						fd = open(s.get_BaseStream(), O_CREAT | O_RDONLY | O_BINARY, S_IREAD);
					if(access == IO::Write)
						fd = open(s.get_BaseStream(), O_CREAT | O_WRONLY | O_BINARY, S_IWRITE);
					if(access == IO::ReadWrite)
						fd = open(s.get_BaseStream(), O_CREAT | O_RDWR | O_BINARY, S_IREAD | S_IWRITE);
					break;
				case IO::CreateNew:
					if(access == IO::Read)
						fd = open(s.get_BaseStream(), O_CREAT | O_EXCL | O_RDONLY | O_BINARY, S_IREAD);
					if(access == IO::Write)
						fd = open(s.get_BaseStream(), O_CREAT | O_EXCL | O_WRONLY | O_BINARY, S_IWRITE);
					if(access == IO::ReadWrite)
						fd = open(s.get_BaseStream(), O_CREAT | O_EXCL | O_RDWR | O_BINARY, S_IREAD | S_IWRITE);
					break;
				case IO::Append:
					if(access == IO::Write)
						fd = open(s.get_BaseStream(), O_APPEND | O_WRONLY | O_BINARY);
					else 
						throw ArgumentException("access", "An invalid FileAccess mode was specified for a file opened with Append.");
					break;
				case IO::Truncate:
					if(access == IO::Write)
						fd = open(s.get_BaseStream(), O_TRUNC | O_WRONLY | O_BINARY);
					else 
						throw ArgumentException("access", "An invalid FileAccess mode was specified for a file opened with Truncate.");
					break;
			}
 			//fd = open(s.get_BaseStream(), mode, access);
			//printf("path: %s\ns: %s\n", path.get_BaseStream(), s.get_BaseStream());
			if(fd == -1)
				throw IOException((const char *)_sys_errlist[errno]);
			//printf("fd: %d\n", fd);
			this->disposed = false;
			this->path = s;
			this->mode = mode;
			this->access = access;
			this->buf_size = 8192; // 8KB
			this->buf = new char[this->buf_size];
			this->buf_dirty = false;
			this->buf_length = 0;
			this->current_offset = 0;
			this->position = 0;
			//printf("buffer size: %d\n", this->buf_size);
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
			if(!get_CanRead())
				throw NotSupportedException ("Stream does not support reading.");
			if(offset > 0)
				array += offset;
			int ret = ReadFileInternal(this->fd, array, count);
			if(ret == -1)
			{
				array -= offset;
				throw IOException((const char *)_sys_errlist[errno]);
			}
			if(ret > 0)
				this->position += ret;
			if(offset > 0)
				array -= offset;
			return ret;
		}
		//----------------------------------------------------
		void FileStream::Write(const char array[], int offset, int count)
		{
			if(!get_CanWrite())
				throw NotSupportedException("Stream does not support writing.");
			// add exceptions here
			//ObjectDisposedException
			if(array == null)
				throw ArgumentNullException("array");
			if(offset < 0 || count < 0)
				throw ArgumentOutOfRangeException("offset or count", "offset or count is negative.");
			if(count == 0)
				throw ArgumentOutOfRangeException("count", "count is zero.");
			if(count > this->buf_size || count > this->buf_size - (this->current_offset + this->buf_length))
			{
				FlushBuffer();
				if(offset > 0)
					array += offset;
				int ret = WriteFileInternal(this->fd, array, count);
				if(ret == -1)
				{
					if(offset > 0)
						array -= offset;
					throw IOException((const char *)_sys_errlist[errno]);
				}
				if(offset > 0)
					array -= offset;
				this->position += count;
				return ;
			}
			else
			{
				this->position += count;
				WriteSegment(array, offset, count);
				return ;
			}
			//printf("fd: %d\n", fd);
			/*if(offset > 0)
				array += offset;
			int ret = WriteFileInternal(this->fd, array, count);
			if(ret == -1)
			{
				array -= offset;
				throw IOException((const char *)_sys_errlist[errno]);
			}
			if(ret > 0)
				this->position += ret;
			if(offset > 0)
				array -= offset;*/
		}
		//----------------------------------------------------
		void FileStream::FlushBuffer()
		{
			if(this->buf_dirty)
			{
				if(this->current_offset > 0)
					this->buf += this->current_offset;
				int ret = WriteFileInternal(this->fd, this->buf, this->buf_length);
				//printf("im FlushBuffer\n");
				if(ret == -1)
				{
					if(this->current_offset > 0)
						this->buf -= this->current_offset;
					throw IOException((const char *)_sys_errlist[errno]);
				}
				if(this->current_offset > 0)
					this->buf -= this->current_offset;
				this->buf_length = 0;
				this->current_offset = 0;
				this->buf_dirty = false;
			}
		}
		//----------------------------------------------------
		void FileStream::WriteSegment(const char array[], int offset, int count)
		{
			//printf("im WriteSegment\n");
			this->buf_dirty = true;
			int seek = this->current_offset + this->buf_length - offset;
			for(int i = offset ; i < offset + count ; i++)
				this->buf[i + seek] = array[i];
			this->buf_length += count;
			/*this->buf_dirty = true;
			if(offset > 0)
				array += offset;
			for(int i = 0 ; i < count ; i++)
				this->buf[i + this->current_offset + this->buf_length] = array[i];
			if(offset > 0)
				array -= offset;
			this->buf_length += count;*/
		}
		//----------------------------------------------------
		/*void FileStream::Write(const char array[], int offset, int count)
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
		}*/
	    //----------------------------------------------------
		long FileStream::get_Length()
		{
			// add exceptions here
			//ObjectDisposedException
			FlushBuffer();
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
		bool FileStream::get_CanRead()
		{
			if(this->access == IO::Read || this->access == IO::ReadWrite)
				return true;
			else
				return false;
		}
		//----------------------------------------------------
		bool FileStream::get_CanWrite()
		{
			if(this->access == IO::Write || this->access == IO::ReadWrite)
				return true;
			else
				return false;
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
			/*if(!firstLocked)
			{*/
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
			//}
		}
		//----------------------------------------------------
		//attention for implementation: Prevents other processes from changing the FileStream while permitting read access.
		void FileStream::UnLock(long position, long length)
		{
			//printf("lock called\n");
			// add exceptions here
			//ArgumentOutOfRangeException
			/*if(!firstLocked)
			{*/
				long currentPosition = get_Position();
				Seek(position, IO::Begin);
#if defined WIN32 || WIN64
				int closeErr = locking(this->fd, LK_UNLCK, length);
				if(closeErr == -1)
						throw IOException((const char *)_sys_errlist[errno]);
#else
				struct flock fl;
				/*This is where you signify the type of lock you want to set.
				It's either F_RDLCK, F_WRLCK, or F_UNLCK if you want to set
				a read lock, write lock, or clear the lock, respectively.*/
				fl.l_type = F_UNLCK;  /* F_RDLCK, F_WRLCK, F_UNLCK   */
				fl.l_whence = SEEK_SET; /* SEEK_SET, SEEK_CUR, SEEK_END */
				fl.l_start = position;  /* Offset from l_whence  */
				fl.l_len = length;  /* length, 0 = to EOF  */
				fl.l_pid = getpid(); /* our PID  */
				int fcntlErr = fcntl(this->fd, F_SETLK, &fl);  /* F_GETLK, F_SETLK, F_SETLKW */
				if(fcntlErr == -1)
					throw IOException((const char *)_sys_errlist[errno]);
#endif
				Seek(currentPosition, IO::Begin);
			//}
			//else printf("not locked\n");
		}
		//----------------------------------------------------
		void FileStream::Close()
		{
			if(!this->disposed)
			{
				this->disposed = true;
				delete this->buf;
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
			long seekErr = lseek(this->fd , 0, origin);
			if(seekErr == -1)
				throw IOException((const char *)_sys_errlist[errno]);
			if(seekErr >= 0)
				this->position = seekErr;
		}
		//----------------------------------------------------
		void FileStream::SetLength(long value)
		{
			// add exceptions here
			// add exception for undefined orgin parameter
			//ObjectDisposedException
			int error = 0;
#if defined WIN32 || WIN64
			error = chsize(this->fd, value);
#else
			error = ftruncate(this->fd, value);
#endif
			if(error == -1)
				throw IOException((const char *)_sys_errlist[errno]);
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
