/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "StaticFunctions.h"

//**************************************************************************************************************//
void GetBytesOfLongNumberForFTSMMode(char *buffer, Int64 value)
{
	/*if(value > 0xFFFFFFFFFFFFFFFF)
		throw ArgumentOutOfRangeException("Length of value field is greater than 18446744073709551615.");*/

	buffer[0] = (Byte)((value & 0xFF00000000000000LL) >> 56);
	buffer[1] = (Byte)((value & 0x00FF000000000000LL) >> 48);
	buffer[2] = (Byte)((value & 0x0000FF0000000000LL) >> 40);
	buffer[3] = (Byte)((value & 0x000000FF00000000LL) >> 32);
	buffer[4] = (Byte)((value & 0x00000000FF000000LL) >> 24);
	buffer[5] = (Byte)((value & 0x0000000000FF0000LL) >> 16);
	buffer[6] = (Byte)((value & 0x000000000000FF00LL) >> 8);
	buffer[7] = (Byte) (value & 0x00000000000000FFLL);
}
Int64 GetLongNumberFromBytesForFTSMMode(Byte buffer[])
{
	//if(buffer == null) throw new ArgumentNullException("Buffer can not be null.");
	//if(buffer.Length != 8) throw new ArgumentOutOfRangeException("Buffer length cannot be opposite of 8 bytes.");
	return ((Int64)buffer[0]  << 56) | ((Int64)buffer[1] << 48) | ((Int64)buffer[2] << 40) | ((Int64)buffer[3] << 32) | ((Int64)buffer[4] << 24) | ((Int64)buffer[5]  << 16) | ((Int64)buffer[6]  << 8) | (Int64)buffer[7];
}
//**************************************************************************************************************//
#if defined WIN32 || WIN64
const char* ErrorString(int err)
{
    CString Error;
     LPTSTR s;
     if(::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            err,
            0,
            (LPTSTR)&s,
            0,
            NULL) == 0)
    { /* failed */
     // Unknown error code %08x (%d)
     CString fmt;
     CString t;
     fmt.LoadString(8);
     t.Format(fmt, err, LOWORD(err));
     Error = t;
    } /* failed */
     else
    { /* success */
     LPTSTR p = _tcschr(s, _T('\r'));
     if(p != NULL)
        { /* lose CRLF */
         *p = _T('\0');
        } /* lose CRLF */
     Error = s;
     ::LocalFree(s);
    } /* success */
     return (const char*)Error;
}
#endif
//------------------------------------------------
/*void Parvicursor_network_cleanup()
{
#if defined WIN32 || WIN64

	ws2_32_is_loaded = false;
	WSACleanup();

#endif
}*/
//------------------------------------------------
char *Parvicursor_test_pli()
{
	String *str = new String("hello from pli");
	//printf("from libe: %s", str.get_BaseStream());
	return str->get_BaseStream();
}
//------------------------------------------------
void *Parvicursor_memalign(size_t alignment, size_t size)
{
#if defined WIN32 || WIN64
   // we need to allocate enough storage for the requested bytes, some
    // book-keeping (to store the location returned by malloc) and some extra
    // padding to allow us to find an aligned byte.  im not entirely sure if
    // 2 * alignment is enough here, its just a guess.
    const size_t total_size = size + (2 * alignment) + sizeof(size_t);

    // use malloc to allocate the memory.
	char *data = (char *)::malloc(sizeof(char) * total_size);

    if (data)
    {
        // store the original start of the malloc'd data.
        const void * const data_start = data;

        // dedicate enough space to the book-keeping.
        data += sizeof(size_t);

        // find a memory location with correct alignment.  the alignment minus
        // the remainder of this mod operation is how many bytes forward we need
        // to move to find an aligned byte.
        const size_t offset = alignment - (((size_t)data) % alignment);

        // set data to the aligned memory.
        data += offset;

        // write the book-keeping.
        size_t *book_keeping = (size_t*)(data - sizeof(size_t));
        *book_keeping = (size_t)data_start;
    }

    return data;
#else
    return memalign(alignment, size);
#endif

}
//------------------------------------------------
void Parvicursor_free(void *mem)
{
#if defined WIN32 || WIN64
	if(mem != null)
	{
		char *data = (char *)mem;

		// we have to assume this memory was allocated with malloc_aligned.
		// this means the sizeof(size_t) bytes before data are the book-keeping
		// which points to the location we need to pass to free.
		data -= sizeof(size_t);

		// set data to the location stored in book-keeping.
		data = (char*)(*((size_t*)data));

		// free the memory.
		::free(data);
	}
#else
    ::free(mem);
#endif
}
//------------------------------------------------
Int32 Parvicursor_getpagesize(void)
{
#if defined WIN32 || WIN64
    SYSTEM_INFO SysInfo;
    ::GetSystemInfo(&SysInfo);
    Int32 pageSize = SysInfo.dwAllocationGranularity;
    return pageSize;
#else
    return getpagesize();
#endif
}
//------------------------------------------------
Int32 Parvicursor_gettimeofday(struct timeval *tv, struct timezone *tz)
{
#if defined WIN32 || WIN64
	FILETIME ft;
	unsigned __int64 tmpres = 0;
	static int tzflag;

	if (NULL != tv)
	{
		GetSystemTimeAsFileTime(&ft);

		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		/*converting file time to unix epoch*/
		tmpres -= DELTA_EPOCH_IN_MICROSECS;
		tmpres /= 10;  /*convert into microseconds*/
		tv->tv_sec = (long)(tmpres / 1000000UL);
		tv->tv_usec = (long)(tmpres % 1000000UL);
	}

	if (NULL != tz)
	{
		if (!tzflag)
		{
			_tzset();
			tzflag++;
		}
		tz->tz_minuteswest = _timezone / 60;
		tz->tz_dsttime = _daylight;
	}

	return 0;
#else
	return gettimeofday(tv, tz);
#endif
}
//------------------------------------------------
/*for linux splice*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if !defined WIN32 || WIN64

#include <sys/types.h>

#ifdef __linux__
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#endif

#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/param.h>
#ifndef BSD
#include <sys/sendfile.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#endif

#ifndef SPLICE_F_MOVE
#define SPLICE_F_MOVE           0x01
#endif
#ifndef SPLICE_F_NONBLOCK
#define SPLICE_F_NONBLOCK       0x02
#endif
#ifndef SPLICE_F_MORE
#define SPLICE_F_MORE           0x04
#endif
#ifndef SPLICE_F_GIFT
#define SPLICE_F_GIFT           0x08
#endif
#ifndef __NR_splice
#define __NR_splice 275
#endif

#ifdef _GNU_SOURCE

#include <unistd.h>
#include <sys/syscall.h>
static inline ssize_t _____splice(Int32 __fdin, Int64 *__offin, Int32 __fdout,
	Int64 *__offout, size_t __len, UInt32 __flags) {
	//printf("flags: %d len:%d", __flags, __len);
#ifdef __NR_splice
	//printf("hello world\n");
	long ret = syscall(__NR_splice, __fdin, __offin, __fdout, __offout, __len, __flags);
	//flush(__fdout);
	 return ret;
#else
         (void)__fdin;
         (void)__offin;
         (void)__fdout;
         (void)__offout;
         (void)__len;
         (void)__flags;
         errno = ENOSYS;
//	 printf("bye world");
         return -1;
#endif
}

#endif
//------------------------------------------------
#ifdef _GNU_SOURCE

// Transfer from socket to disk.
//static int pipes_socket_to_disk [2];
//static int pipes_socket_to_disk_is_defined = 0;
//#ifndef PIPE_SIZE
//const int PIPE_SIZE = 16*1024;
//#endif
int TransferFromSocketToDisk_ZeroCopySplice(int fd_write, Int64 *offset_write, int socket, size_t size, int pipes_socket_to_disk [2], int PIPE_SIZE, bool &pipes_socket_to_disk_is_defined, bool MustToBeClosed)
{
	if(MustToBeClosed)
	{
	    if(pipes_socket_to_disk_is_defined)
	    {
            if(pipes_socket_to_disk[0] != -1)
            {
                close(pipes_socket_to_disk[0]);
                pipes_socket_to_disk[0] = -1;
            }
            if(pipes_socket_to_disk[1] != -1)
            {
                close(pipes_socket_to_disk[1]);
                pipes_socket_to_disk[1] = -1;
            }
            pipes_socket_to_disk_is_defined = false;
            return 0;
	    }
	    else
            return 0;
	}

	int a = ((int)size)/PIPE_SIZE;
	int b = ((int)size)%PIPE_SIZE;
	int  _PIPE_SIZE = PIPE_SIZE;
	if((int)size < _PIPE_SIZE)
	{
	    a = 1;
	    b = 0;
	    _PIPE_SIZE = size;
	}
	//printf("\na: %d b: %d size: %d div: %d PIPE_SIZE: %d\n",a, b, size, size/PIPE_SIZE, PIPE_SIZE);
	//return 0;

	int ret;
	size_t to_write = size;
	if(!pipes_socket_to_disk_is_defined)
	{
	    ret = pipe(pipes_socket_to_disk);
	    //printf("\nfrom pipe ret: %d %d", ret, errno);
	    pipes_socket_to_disk_is_defined = true;
	    if (ret < 0)
		goto out;
	}

	int n;
	//while(1)
	//
	register int i;
here:
	for(i = 0 ; i < a ; i++)
	{
	    //printf("\ni: %d", i);
	    to_write = _PIPE_SIZE;
	    n = 0;
	    // splice the file into the pipe (data in kernel memory).
	    while (to_write > 0) {
		    ret = _____splice (socket, null, pipes_socket_to_disk[1], null, to_write,
				    SPLICE_F_MORE | SPLICE_F_MOVE);
		    if (ret <= 0)
			    goto pipe;
		    if (ret == 0)
			    break;
		    n += ret;
		    to_write -= ret;
		    //printf("\nhellow from FileCopyFromReadtoWrite ret:%d\n ", ret);
	    }
	    //printf("\ngoto write to file i: %d", i);
	    to_write = n;
	    int n2 = 0;
	    // splice the data in the pipe (in kernel memory) into the file.
	    while (to_write > 0) {
		    ret = _____splice (pipes_socket_to_disk[0], null, fd_write,
				    offset_write, to_write,
				    SPLICE_F_MORE | SPLICE_F_MOVE);
		    if (ret < 0)
			    goto pipe;
		    n2+=ret;
		    to_write -= ret;
	    }
	    //printf("\nwritten size to file: %d\n", n2);
	}
	if(b != 0)
	{
	    //cout << "b: " << b << endl << endl;
	    a = 1;
	    i = 0;
	    _PIPE_SIZE = b;
	    b = 0;
	    goto here;
	}
pipe:
	if(errno < 0)
	{
        if(pipes_socket_to_disk[0] != -1)
        {
            close(pipes_socket_to_disk[0]);
            pipes_socket_to_disk[0] = -1;
        }
        if(pipes_socket_to_disk[1] != -1)
        {
            close(pipes_socket_to_disk[1]);
            pipes_socket_to_disk[1] = -1;
        }
	    pipes_socket_to_disk_is_defined = false;
	    //printf("\nexit from pipe ret: %d errno: %d\n", ret, errno);
	    return errno;
	}
out:
	if (ret < 0)
	{
		//printf("\nexit from ret ret:%d errno: %d\n", ret, errno);
		return errno;

	}
	return 0;
}
// Transfer from disk to socket.
int TransferFromDiskToSocket_ZeroCopySplice(int fd_read, Int64 *offset_read, int socket, size_t size, int pipes_socket_to_disk [2], int PIPE_SIZE, bool &pipes_socket_to_disk_is_defined, bool MustToBeClosed)
{
	if(MustToBeClosed)
	{
	    if(pipes_socket_to_disk_is_defined)
	    {
            if(pipes_socket_to_disk[0] != -1)
            {
                close(pipes_socket_to_disk[0]);
                pipes_socket_to_disk[0] = -1;
            }
            if(pipes_socket_to_disk[1] != -1)
            {
                close(pipes_socket_to_disk[1]);
                pipes_socket_to_disk[1] = -1;
            }
            pipes_socket_to_disk_is_defined = false;
            return 0;
	    }
	    else
            return 0;
	}

	int a = ((int)size)/PIPE_SIZE;
	int b = ((int)size)%PIPE_SIZE;
	int  _PIPE_SIZE = PIPE_SIZE;
	if((int)size < _PIPE_SIZE)
	{
	    a = 1;
	    b = 0;
	    _PIPE_SIZE = size;
	}
	//printf("\na: %d b: %d size: %d div: %d PIPE_SIZE: %d\n",a, b, size, size/PIPE_SIZE, PIPE_SIZE);
	//return 0;

	int ret;
	size_t to_read = size;
	if(!pipes_socket_to_disk_is_defined)
	{
	    ret = pipe(pipes_socket_to_disk);
	    //printf("\nfrom pipe ret: %d %d", ret, errno);
	    pipes_socket_to_disk_is_defined = true;
	    if (ret < 0)
		goto out;
	}

	int n;
	//while(1)
	//{
	register int i;
here:
	for(i = 0 ; i < a ; i++)
	{
	    //printf("\ni: %d", i);
	    to_read = _PIPE_SIZE;
	    n = 0;
	    // splice the file into the pipe (data in kernel memory).
  	    // filedes[0] is for reading, filedes[1] is for writing.
	    while (to_read > 0) {
            ret = _____splice (fd_read, offset_read, pipes_socket_to_disk[1], null, to_read,
				    SPLICE_F_MORE | SPLICE_F_MOVE); //

		    if (ret <= 0)
			    goto pipe;
		    if (ret == 0)
			    break;
		    n += ret;
		    to_read -= ret;
		    //printf("\nhellow from FileCopyFromReadtoWrite ret:%d\n ", ret);
	    }
	    //printf("\ngoto write to file i: %d", i);
	    to_read = n;
	    int n2 = 0;
	    // splice the data in the pipe (in kernel memory) into the socket.
	    while (to_read > 0) {
            ret = _____splice (pipes_socket_to_disk[0], null, socket,
				    null, to_read,
				    SPLICE_F_MORE | SPLICE_F_MOVE); //
		    if (ret < 0)
			    goto pipe;
		    n2+=ret;
		    to_read -= ret;
	    }
	    //printf("\nwritten size to socket: %d\n", n2);
	}
	if(b != 0)
	{
	    //cout << "b: " << b << endl << endl;
	    a = 1;
	    i = 0;
	    _PIPE_SIZE = b;
	    b = 0;
	    goto here;
	}
pipe:
	if(errno < 0)
	{
        if(pipes_socket_to_disk[0] != -1)
        {
            close(pipes_socket_to_disk[0]);
            pipes_socket_to_disk[0] = -1;
        }
        if(pipes_socket_to_disk[1] != -1)
        {
            close(pipes_socket_to_disk[1]);
            pipes_socket_to_disk[1] = -1;
        }
	    pipes_socket_to_disk_is_defined = false;
	    //printf("\nexit from pipe ret: %d errno: %d\n", ret, errno);
	    return errno;
	}
out:
	if (ret < 0)
	{
		//printf("\nexit from ret ret:%d errno: %d\n", ret, errno);
		return errno;

	}
	return 0;
}

// Transfer from disk to disk.
int FileCopyFromReadtoWrite_ZeroCopySplice(int fd_write, Int64 *offset_write, int fd_read, Int64 *offset_read, size_t size)
{
	int filedes[2];
	int ret;
	size_t to_write = size;
	ret = pipe(filedes);
	if (ret < 0)
		goto out;
	while(true)
	{
	    to_write = size;
	    // splice the file into the pipe (data in kernel memory).
	    while (to_write > 0) {
		    ret = _____splice (fd_read, offset_read, filedes [1], null, to_write,
				    SPLICE_F_MORE | SPLICE_F_MOVE);
		    if (ret <= 0)
			    goto pipe;
		    else
			    to_write -= ret;
		    //printf("\nhellow from FileCopyFromReadtoWrite :%d\n ", *offset_read);
	    }
	    to_write = size;
	    // splice the data in the pipe (in kernel memory) into the file.
	    while (to_write > 0) {
		    ret = _____splice (filedes[0], null, fd_write,
				    offset_write, to_write,
				    SPLICE_F_MORE | SPLICE_F_MOVE);
		    if (ret < 0)
			    goto pipe;
		    else
			    to_write -= ret;
	    }
	}
pipe:
	close(filedes[0]);
	close(filedes[1]);
out:
	if (ret < 0)
		return -errno;
	return 0;
}

#endif
//------------------------------------------------
//------------------------------------------------

#endif
