/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __StaticFunctions__
#define __StaticFunctions__

#include "../general.h"
#include "../System/BasicTypes/BasicTypes.h"
#include "../System/String/String.h"
//#include "../System.Net.Sockets/Socket/Socket.h"

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


using namespace System;
//**************************************************************************************************************//
__Parvicursor_inline__ void GetBytesOfLongNumberForFTSMMode(char *buffer, Int64 value);
__Parvicursor_inline__ Int64 GetLongNumberFromBytesForFTSMMode(Byte buffer[]);


#if defined WIN32 || WIN64
//#include <atlstr.h>
//#include <windows.h>
const char* ErrorString(int err);
#else
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#endif

//void Parvicursor_network_cleanup();

// This function posix_memalign() allocates size bytes and places the address of the allocated memory in *memptr.
// The address of the allocated memory will be a multiple of alignment, which must be a power of two and a multiple
// of sizeof(void *). If size is 0, then Parvicursor_memalign() returns either NULL, or a unique pointer value that can
// later be successfully passed to Parvicursor_free(3).
void *Parvicursor_memalign(size_t alignment, size_t size);
// Frees aligned allocated memory by Parvicursor_memalign() function.
void Parvicursor_free(void *mem);
// Gets memory page size.
Int32 Parvicursor_getpagesize(void);


#if defined WIN32 || WIN64
#include < time.h >
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

struct timezone
{
	int  tz_minuteswest; /* minutes W of Greenwich */
	int  tz_dsttime;     /* type of dst correction */
};
#else
#include <sys/time.h>
#endif
Int32 Parvicursor_gettimeofday(struct timeval *tv, struct timezone *tz);


#if !defined WIN32 || WIN64
#ifdef _GNU_SOURCE

#include <sys/types.h>

__Parvicursor_inline__ int TransferFromSocketToDisk_ZeroCopySplice(int fd_write, Int64 *offset_write, int socket, size_t size, int pipes_socket_to_disk [2], int PIPE_SIZE, bool &pipes_socket_to_disk_is_defined, bool MustToBeClosed);
__Parvicursor_inline__ int TransferFromDiskToSocket_ZeroCopySplice(int fd_read, Int64 *offset_read, int socket, size_t size, int pipes_socket_to_disk [2], int PIPE_SIZE, bool &pipes_socket_to_disk_is_defined, bool MustToBeClosed);
__Parvicursor_inline__ int FileCopyFromReadtoWrite_ZeroCopySplice(int fd_write, Int64 *offset_write, int fd_read, Int64 *offset_read, size_t size);

#endif
#endif
//**************************************************************************************************************//
// Gets the binary form of the value and stores it into the buffer.
inline static void Parvicursor_GetBytesFromInt64Number(Out char *buffer, In Int64 value)
{
	buffer[0] = (Byte)((value & 0xFF00000000000000) >> 56);
	buffer[1] = (Byte)((value & 0x00FF000000000000) >> 48);
	buffer[2] = (Byte)((value & 0x0000FF0000000000) >> 40);
	buffer[3] = (Byte)((value & 0x000000FF00000000) >> 32);
	buffer[4] = (Byte)((value & 0x00000000FF000000) >> 24);
	buffer[5] = (Byte)((value & 0x0000000000FF0000) >> 16);
	buffer[6] = (Byte)((value & 0x000000000000FF00) >> 8);
	buffer[7] = (Byte) (value & 0x00000000000000FF);
}
// Gets the binary form of the value and stores it into the buffer.
inline static void Parvicursor_GetBytesFromInt32Number(Out char *buffer, In Int32 value)
{
	buffer[0] = (Byte)((value & 0xFF000000) >> 24);
	buffer[1] = (Byte)((value & 0x00FF0000) >> 16);
	buffer[2] = (Byte)((value & 0x0000FF00) >> 8);
	buffer[3] = (Byte)(value & 0x000000FF);
}
// Gets the binary form of the value and stores it into the buffer.
inline static void Parvicursor_GetBytesFromInt16Number(Out char *buffer, In Int16 value)
{
	buffer[0] = (Byte)((value & 0xFF00) >> 8);
	buffer[1] = (Byte) (value & 0x00FF);
}
// Gets the Int64 value from buffer.
inline static Int64 Parvicursor_GetInt64NumberFromBytes(In Byte *buffer)
{
	return ((Int64)buffer[0]  << 56) | ((Int64)buffer[1] << 48) | ((Int64)buffer[2] << 40) | ((Int64)buffer[3] << 32) | ((Int64)buffer[4] << 24) | ((Int64)buffer[5]  << 16) | ((Int64)buffer[6]  << 8) | (Int64)buffer[7];
}
// Gets the Int32 value from buffer.
inline static Int32 Parvicursor_GetInt32NumberFromBytes(In Byte *buffer)
{
	return ((Int32)buffer[0] << 24) | ((Int32)buffer[1]  << 16) | ((Int32)buffer[2]  << 8) | (Int32)buffer[3];
}
// Gets the Int16 value from buffer.
inline static Int16 Parvicursor_GetInt16NumberFromBytes(In Byte *buffer)
{
	return ((Int16)buffer[0] << 8) | (Int16)buffer[1];
}


#endif

//static char *Parvicursor_test_pli();
