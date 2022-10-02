/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/
#ifndef __Parvicursor_xThread_xThreadBase_h__
#define __Parvicursor_xThread_xThreadBase_h__

#include "../../System/BasicTypes/BasicTypes.h"
#include "../../Parvicursor/Serialization/Serializer.h"
#include "../../Parvicursor/Serialization/DeSerializer.h"
#include "../../System/Object/Object.h"
#include "../../System/ArgumentException/ArgumentNullException.h"


#if defined WIN32 || WIN64

#ifndef xThreadBase_IMPORTS
#define xThreadBase_API __declspec (dllexport)
#else
#define xThreadBase_API __declspec (dllimport)
#endif

#ifndef xThreadBase_DLL_Import
#define xThreadBase_DLL_Import  __declspec (dllimport)
#endif

#ifndef xThreadBase_DLL_Export
#define xThreadBase_DLL_Export  __declspec (dllexport)
#endif


#else
#define xThreadBase_API

#ifndef xThreadBase_DLL_Import
#define xThreadBase_DLL_Import
#endif

#ifndef xThreadBase_DLL_Export
#define xThreadBase_DLL_Export
#endif

#endif
//**************************************************************************************************************//

using namespace System;
using namespace Parvicursor::Serialization;

namespace Parvicursor
{
	//----------------------------------------------------
   namespace xThread
   {
		//----------------------------------------------------
		class xThreadBase_API xThreadBase
		{
			// An internal data member to be used within the implementation of  Serialize() method.
			protected: Serializer *serializer;
			// An internal data member to be used within the implementation of  DeSerialize() method.
			protected: DeSerializer *deserializer;
		    // The method to execute by xThread server.
			public: virtual void Start() = 0;
			// Realizes an extensive RPC/RMI-like mechanism for xThread abstraction.
			public: virtual void AuxiliaryMehtod(In const void *input, Int32 inputLength, Out void **output, Out Int32 *outputLength);
			// Serializes class data into the address of DllBuffer with output parameter len, which
			// indicates the length of the DllBuffer.
			public: virtual void Serialize(char **DllBuffer, int *len) = 0;
			// Deserializes class data into the address of buffer with output parameter len, which
			// indicates the length of the buffer.
			public: virtual void DeSerialize(char **buffer, int *len) = 0;
			// Allocates memory used by xThread runtime. This method must be implemented in any
			// inherited class to resolve the problem associated with the OS loader-lock deadlock.
			public: virtual void *AllocateDllMemory(UInt32 size) = 0;
			// Frees the allocated memory by AllocateDllMemory.
			public: virtual void FreeDllMemory(void *mem) = 0;
			// Destructs the allocated object allocated by xThreadBase_creator() function.
			public: virtual void Deconstructor(xThreadBase *obj) = 0;
			// Release the code of your critical sections within this method.
			public: virtual void ReleaseCriticalSesctions();
			//--------------
			// Copies the buffer associated with serializer in the address pointed by DllBuffer.
			// If *DllBuffer is null, the method allocate the needed memory.
			public: static void MemoryCopy(char **DllBuffer, int *len, Serializer *serializer);
			//--------------
		};
		//----------------------------------------------------
	}
}
//**************************************************************************************************************//

#endif


