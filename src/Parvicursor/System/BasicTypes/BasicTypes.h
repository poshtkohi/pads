/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_BasicTypes_h__
#define __System_BasicTypes_h__

#include "../../general.h"
//**************************************************************************************************************//

namespace System
{
	/*typedef unsigned char __u_char;
	typedef unsigned short int __u_short;
	typedef unsigned int __u_int;
	typedef unsigned long int __u_long;*/
#if defined __OSIC___
	#define Null 0
#else
    #define null 0
#endif
	#define Out // indicates the output param
	#define In	// indicates the input param
	#define InOut	// indicates the input/output param
	/* Fixed-size types, underlying types depend on word size and compiler.  */
	typedef signed char Int8;
	typedef unsigned char UInt8;
	typedef signed short int Int16;
	typedef unsigned short int UInt16;
	typedef signed int Int32;
	typedef unsigned int UInt32;
	typedef double Double;
	typedef float Float;
	/*
		On older, 16-bit systems, single long was 32-bit.
		On 32-bit systems, single long is 32-bit, while long long is 64-bits.
			This is also true for 64-bit processors running 32-bit programs.
		On 64-bit systems, single long and long long are 64-bits.
	*/

#if defined WIN32 || WIN64
#	if !defined _M_X64
	typedef signed long int Long;
	typedef unsigned long int ULong;
#	else if (defined _M_X64)
	typedef signed long long int Long;
	typedef unsigned long long int ULong;
#	endif
#else

#	if !defined __x86_64__
	typedef signed long int Long;
	typedef unsigned long int ULong;
#	else if (defined __x86_64__)
	typedef signed long long int Long;
	typedef unsigned long long int ULong;
#	endif
#endif



/*#if defined WIN32 || WIN64
	typedef byte UInt8;
#else
	typedef UInt8 byte;
#endif*/

	typedef UInt8 Byte;

	//typedef Byte UInt8;
/*
	To make an integer constant of type long long int in GCC compiler, add the suffix `LL'
	to the integer. To make an integer constant of type unsigned long long int,
	add the suffix `ULL' to the integer.
*/
	typedef signed long long int Int64;		// sample usage 0xFF00000000000000LL
	typedef unsigned long long int UInt64;	// sample usage 0xFF00000000000000ULL
	typedef long double Int128;
	//typedef unsigned long double UInt128;

    // An interface contains only the signatures of methods, properties, events or indexers.
    // A class or struct that implements the interface must implement the members of the
    // interface that are specified in the interface definition.
    // A .NET interface is equivalent to C++ abstarct classed with completely pure virtual methods.
	#define interface

};
//**************************************************************************************************************//

#endif
