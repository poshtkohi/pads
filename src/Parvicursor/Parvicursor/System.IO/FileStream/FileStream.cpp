/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

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
		FileStream::FileStream(String path, FileMode mode, FileAccess access/*, FileShare share*/, Int32 bufferSize)
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

            //In Windows: The pmode argument is required only when O_CREAT is specified.
            // If the file already exists, pmode is ignored. Otherwise, pmode specifies the file permission settings,
            //which are set when the new file is closed the first time.

#if defined WIN32 || WIN64

			Int32 pmode = S_IREAD | S_IWRITE;

#else

            Int32 pmode = S_IREAD | S_IWRITE | S_IRGRP | S_IROTH;

#endif

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
						fd = open(s.get_BaseStream(), O_CREAT | O_RDONLY  | O_BINARY, pmode);
					if(access == IO::Write)
						fd = open(s.get_BaseStream(), O_CREAT | O_WRONLY | O_BINARY, pmode);
					if(access == IO::ReadWrite)
						fd = open(s.get_BaseStream(), O_CREAT | O_RDWR | O_BINARY, pmode);
					break;
				case IO::OpenOrCreate:
					if(access == IO::Read)
						fd = open(s.get_BaseStream(), O_CREAT | O_RDONLY | O_BINARY, pmode);
					if(access == IO::Write)
						fd = open(s.get_BaseStream(), O_CREAT | O_WRONLY | O_BINARY, pmode);
					if(access == IO::ReadWrite)
						fd = open(s.get_BaseStream(), O_CREAT | O_RDWR | O_BINARY, pmode);
					break;
				case IO::CreateNew:
					if(access == IO::Read)
						fd = open(s.get_BaseStream(), O_CREAT | O_EXCL | O_RDONLY | O_BINARY, pmode);
					if(access == IO::Write)
						fd = open(s.get_BaseStream(), O_CREAT | O_EXCL | O_WRONLY | O_BINARY, pmode);
					if(access == IO::ReadWrite)
						fd = open(s.get_BaseStream(), O_CREAT | O_EXCL | O_RDWR | O_BINARY, pmode);
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
			{
#if defined WIN32 || WIN64
				throw IOException(ErrorString(GetLastError()));
#else
				throw IOException((const char *)_sys_errlist[errno]);
#endif
			}

			//printf("fd: %d\n", fd);
			this->disposed = false;
			this->path = s;
			this->mode = mode;
			this->access = access;
			this->buf_size = bufferSize;
			this->buf = new char[this->buf_size];
			//this->one_byte = new char[1];
			this->buf_dirty = false;
			this->buf_length = 0;
			this->current_offset = 0;
			this->position = 0;
			//printf("buffer size: %d\n", this->buf_size);
#if defined WIN32 || WIN64
			this->handle = (HANDLE)_get_osfhandle(this->fd);
			if(this->handle == INVALID_HANDLE_VALUE)
			{
				Close();
				throw IOException(ErrorString(GetLastError()));
			}
#endif
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
			{
#if defined WIN32 || WIN64

				throw IOException(ErrorString(GetLastError()));

#else

				throw IOException((const char *)_sys_errlist[errno]);

#endif
			}
			//printf("fd: %d\n", fd);
			this->disposed = false;
			this->path = s;
			this->mode = mode;
			this->access = access;
			this->buf_size = 8192; // 8KB
			if(access == IO::Read)
			{
				/* Avoid allocating a large buffer for small files */
				Int64 len = get_Length();
				if(this->buf_size > len)
				{
					this->buf_size = (Int32)(len < 1000 ? 1000 : len);
				}
			}
			this->buf = new char[this->buf_size];
			//this->one_byte = new char[1];
			this->buf_dirty = false;
			this->buf_length = 0;
			this->current_offset = 0;
			this->position = 0;
			//printf("buffer size: %d\n", this->buf_size);
	    }
		//----------------------------------------------------
		void FileStream::Write(const char array[], Int32 offset, Int32 count)
		{
			if(this->fd < 0)
				throw ObjectDisposedException ("Stream has been closed.");
			if(array == null)
				throw ArgumentNullException("array");
			if(offset < 0 || count < 0)
				throw ArgumentOutOfRangeException("offset or count", "offset or count is negative.");
			if(count == 0)
				throw ArgumentOutOfRangeException("count", "count is zero.");
			if(!get_CanWrite())
				throw NotSupportedException("Stream does not support writing.");

			//printf("write position: %d\n", this->get_Position());//

			if(count > this->buf_size || count > this->buf_size - (this->current_offset + this->buf_length))
			{
				FlushBuffer();
				if(offset > 0)
					array += offset;
				//Int32 ret = WriteFileInternal(this->fd, array, count);
				Int32 ret = ::write(this->fd, array, count);
				if(ret == -1)
				{
					if(offset > 0)
						array -= offset;
#if defined WIN32 || WIN64

					throw IOException(ErrorString(GetLastError()));

#else

					throw IOException((const char *)_sys_errlist[errno]);

#endif
				}
				if(offset > 0)
					array -= offset;
				this->position += count;
				return ;
			}
			else
			{
				this->position += count;
				this->buf_dirty = true;
				WriteSegment(array, offset, count);
				return ;
			}
			//printf("fd: %d\n", fd);
			/*if(offset > 0)
				array += offset;
			Int32 ret = WriteFileInternal(this->fd, array, count);
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
		/*void FileStream::WriteByte(char value)
		{
			this->one_byte[0] = value;
			Write(this->one_byte, 0, 1);
		}*/
		//----------------------------------------------------
		void FileStream::WriteSegment(const char array[], Int32 offset, Int32 count)
		{
			//printf("im WriteSegment\n");
			Int32 seek = this->current_offset + this->buf_length - offset;
			for(Int32 i = offset ; i < offset + count ; i++)
				this->buf[i + seek] = array[i];
			this->buf_length += count;
			/*this->buf_dirty = true;
			if(offset > 0)
				array += offset;
			for(Int32 i = 0 ; i < count ; i++)
				this->buf[i + this->current_offset + this->buf_length] = array[i];
			if(offset > 0)
				array -= offset;
			this->buf_length += count;*/
		}
		//----------------------------------------------------
		void FileStream::FlushBuffer()
		{
			if(this->buf_dirty)
			{
				if(this->current_offset > 0)
					this->buf += this->current_offset;
				//Int32 ret = WriteFileInternal(this->fd, this->buf, this->buf_length);
				Int32 ret = ::write(this->fd, this->buf, this->buf_length);
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
		Int32 FileStream::Read(char array[], Int32 offset, Int32 count)
		{
			if(this->fd < 0)
				throw ObjectDisposedException ("Stream has been closed.");

			if(array == null)
				throw ArgumentNullException("array");

			if(offset < 0 || count < 0)
				throw ArgumentOutOfRangeException("offset or count", "offset or count is negative.");

			if(count == 0)
				throw ArgumentOutOfRangeException("count", "count is zero.");

			if(!get_CanRead())
				throw NotSupportedException ("Stream does not support reading.");

			FlushBuffer();

			//printf("read position: %d\n", this->get_Position());//

			if(count > this->buf_size)
			{
				if(offset > 0)
					array += offset;
				//Int32 ret = ReadFileInternal(this->fd, array, count);
				Int32 ret = ::read(this->fd, array, count);
				if(ret == -1)
				{
					if(offset > 0)
						array -= offset;
#if defined WIN32 || WIN64

					throw IOException(ErrorString(GetLastError()));

#else

					throw IOException((const char *)_sys_errlist[errno]);

#endif
				}
				if(offset > 0)
					array -= offset;
				this->position += ret;
				return ret;
			}
			else
			{
				if(this->buf_length == -1) // meaning the end of file.
					return 0;
				if(this->buf_length == 0)
					RefillBuffer();
				if(count > this->buf_length)
				{
					Int32 readLength = this->buf_length;
					for(Int32 i = offset ; i < offset + readLength ; i++)
						array[i] = this->buf[i + this->current_offset - offset];
					RefillBuffer();
					if(this->buf_length == -1) // meaning the end of file.
						return readLength;
					return ReadSegment(array, offset + readLength, count - readLength) + readLength;
				}
				else
					return ReadSegment(array, offset, count);
			}
			/*if(offset > 0)
				array += offset;
			Int32 ret = ReadFileInternal(this->fd, array, count);
			if(ret == -1)
			{
				array -= offset;
				throw IOException((const char *)_sys_errlist[errno]);
			}
			if(ret > 0)
				this->position += ret;
			if(offset > 0)
				array -= offset;
			return ret;*/
		}
		//----------------------------------------------------
		/*Int32 FileStream::ReadByte() // this method has some problems in returning -1 for some chars
		{
			Int32 ret = Read(this->one_byte, 0, 1);
			if(ret == 0)
				return -1;
			return this->one_byte[0];
		}*/
		//----------------------------------------------------
		void FileStream::RefillBuffer()
		{
			//printf("RefillBuffer\n");
			Int32 ret = ::read(this->fd, this->buf, this->buf_size);
			//Int32 ret = ReadFileInternal(this->fd, this->buf, this->buf_size);

			if(ret == -1)
			{
#if defined WIN32 || WIN64

				throw IOException(ErrorString(GetLastError()));
#else

				throw IOException((const char *)_sys_errlist[errno]);
#endif
			}

			this->current_offset = 0;
			if(ret == 0)
				this->buf_length = -1; // meaning end of file.
			else
				this->buf_length = ret;
			//this->position += ret;
		}
		//----------------------------------------------------
		Int32 FileStream::ReadSegment(char array[], Int32 offset, Int32 count)
		{
			Int32 ret = count;
			if(count > this->buf_length)
				ret = this->buf_length;

			for(Int32 i = offset ; i < offset + ret ; i++)
				 array[i] = this->buf[i + this->current_offset - offset];

			this->buf_length -= ret;
			this->current_offset += ret;
			this->position += ret;
			return ret;
		}
		//----------------------------------------------------
		/*void FileStream::Write(const char array[], Int32 offset, Int32 count)
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
			Int32 ret = WriteFileInternal(this->fd, array, count);
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
		Int64 FileStream::get_Length()
		{
			if(this->fd < 0)
				throw ObjectDisposedException ("Stream has been closed.");

			if(this->fd >= 0)
				FlushBuffer();

			Int64 currentPosition = get_Position();
			Seek(0, End);
			Int64 fileSize = get_Position();
			Seek(currentPosition, Begin);

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
		Int32 FileStream::get_Handle()
		{
			if(this->fd < 0)
				throw ObjectDisposedException ("Stream has been closed.");

			return this->fd;
		}
		//----------------------------------------------------
		void FileStream::Flush()
		{
			if(this->fd < 0)
				throw ObjectDisposedException ("Stream has been closed.");
			/*Int32 commitErr = commit(this->fd);
			if(commitErr == -1)
				throw IOException((const char *)_sys_errlist[errno]);*/
			FlushBuffer();
		}
		//----------------------------------------------------
		/*void FileStream::SetLength(long value)
		{
			// add exceptions here
			//ArgumentOutOfRangeException
		}*/
		//Function: Int32 truncate (const char *filename, off_t length) SetLength()
		//----------------------------------------------------
		//attention for implementation: Prevents other processes from changing the FileStream while permitting read access.
		void FileStream::Lock(Int64 position, Int64 length)
		{
			//printf("lock called\n");
			if(this->fd < 0)
				throw ObjectDisposedException ("Stream has been closed.");

			if(position < 0 || length < 0)
				throw ArgumentOutOfRangeException("position or length", "position or length is negative.");

			/*if(!firstLocked)
			{*/
				Int64 currentPosition = get_Position();
				Seek(position, IO::Begin);
#if defined WIN32 || WIN64
				/*Int32 lockingErr = locking(this->fd, LK_LOCK, (long)length);
				if(lockingErr == -1)
					throw IOException(ErrorString(GetLastError()));*/

				if(!LockFile(this->handle, (Int32)(position & 0x00000000FFFFFFFFLL), (Int32)((position & 0xFFFFFFFF00000000LL) > 32),
						(Int32)(length & 0x00000000FFFFFFFFLL), (Int32)((length & 0xFFFFFFFF00000000LL)) > 32))
						throw IOException(ErrorString(GetLastError()));
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
				Int32 fcntlErr = fcntl(this->fd, F_SETLK, &fl);  /* F_GETLK, F_SETLK, F_SETLKW */
				if(fcntlErr == -1)
					throw IOException((const char *)_sys_errlist[errno]);
#endif
				Seek(currentPosition, IO::Begin);
			//}
		}
		//----------------------------------------------------
		//attention for implementation: Prevents other processes from changing the FileStream while permitting read access.
		void FileStream::UnLock(Int64 position, Int64 length)
		{
				if(this->fd < 0)
					throw ObjectDisposedException ("Stream has been closed.");

				if(position < 0 || length < 0)
					throw ArgumentOutOfRangeException("position or length", "position or length is negative.");

				Int64 currentPosition = get_Position();
				Seek(position, IO::Begin);

#if defined WIN32 || WIN64
				/*Int32 closeErr = locking(this->fd, LK_UNLCK, (long)length);
				if(closeErr == -1)
						throw IOException(ErrorString(GetLastError()));*/

				if(!UnlockFile(this->handle, (Int32)(position & 0x00000000FFFFFFFFLL), (Int32)((position & 0xFFFFFFFF00000000LL) > 32),
							(Int32)(length & 0x00000000FFFFFFFFLL), (Int32)((length & 0xFFFFFFFF00000000LL)) > 32))
								throw IOException(ErrorString(GetLastError()));
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
				Int32 fcntlErr = fcntl(this->fd, F_SETLK, &fl);  /* F_GETLK, F_SETLK, F_SETLKW */
				if(fcntlErr == -1)
					throw IOException((const char *)_sys_errlist[errno]);
#endif
				Seek(currentPosition, IO::Begin);
		}
		//----------------------------------------------------
		void FileStream::Close()
		{
			if(!this->disposed)
			{
				/*if(this->buf_dirty)
					printf("buf is dirty, fd: %d\n", this->buf_length);
				else
					printf("buf is not dirty, fd: %d\n", this->buf_length);*/
				if(this->fd >= 0)
					FlushBuffer();
				this->disposed = true;
				//delete this->one_byte;
				delete this->buf;
				Int32 closeErr = close(this->fd);
				if(closeErr == -1)
				{
#if defined WIN32 || WIN64

				throw IOException(ErrorString(GetLastError()));
#else

				throw IOException((const char *)_sys_errlist[errno]);
#endif
				}

				this->fd = -10;
			}
		}
		//----------------------------------------------------
		/*void FileStream::operator delete(void *p)
		{
			if(!disposed)
				Close();
			delete p;
		}*/
		//----------------------------------------------------
		Int64 FileStream::get_Position()
		{
			if(this->fd < 0)
				throw ObjectDisposedException ("Stream has been closed.");
			return this->position;
		}
		//----------------------------------------------------
		void FileStream::set_Position(Int64 position)
		{
			Seek(position, IO::Begin);
		}
		//----------------------------------------------------
		Int64 FileStream::Seek(Int64 offset, SeekOrigin origin)
		{
			if(this->fd < 0)
				throw ObjectDisposedException ("Stream has been closed.");

			FlushBuffer();

#if defined WIN32 || WIN64
				Int64 newOffset = _lseeki64(this->fd , offset, origin);
#else
				Int64 newOffset = lseek64(this->fd , offset, origin);
#endif

			if(newOffset == -1)
			{
#if defined WIN32 || WIN64

				throw IOException(ErrorString(GetLastError()));
#else

				throw IOException((const char *)_sys_errlist[errno]);
#endif
			}

			if(newOffset >= 0)
				this->position = newOffset;

			return newOffset;
		}
		//----------------------------------------------------
		void FileStream::SetLength(Int64 value)
		{
			if(this->fd < 0)
				throw ObjectDisposedException ("Stream has been closed.");
			if(value < 0)
				throw ArgumentOutOfRangeException("value", "Attempted to set the value parameter to less than 0.");
			FlushBuffer();
			Int32 error = 0;

#if defined WIN32 || WIN64
/*#if defined (_MSC_VER) && (_MSC_VER > 1400)
			error = _chsize_s(this->fd, value);
#else*/

			Int64 currentPosition = this->get_Position();
			Seek(value, Begin);
			error = SetEndOfFile(this->handle);
			if(error != -1)
				Seek(currentPosition, Begin);
			//error = chsize(this->fd, (long)value);

#else
			Int64 currentPosition = this->get_Position();
			Seek(value, Begin);
			error = ftruncate64(this->fd, value);
			if(error != -1)
				Seek(currentPosition, Begin);
#endif
			if(error == -1)
			{
#if defined WIN32 || WIN64

				throw IOException(ErrorString(GetLastError()));
#else

				throw IOException((const char *)_sys_errlist[errno]);
#endif
			}

		}
	    //----------------------------------------------------

	}
}
//**************************************************************************************************************//
/*Int32 ReadFileInternal(Int32 fd, void *array, unsigned Int32 count)
{
	return read(fd, array, count);
}
//----------------------------------------------------
Int32 WriteFileInternal(Int32 fd, const void *array, unsigned Int32 count)
{
	return write(fd, array, count);
}*/
//**************************************************************************************************************//
