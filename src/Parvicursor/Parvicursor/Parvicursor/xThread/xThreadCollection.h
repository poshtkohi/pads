/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __Parvicursor_xThread_xThreadCollection_h__
#define __Parvicursor_xThread_xThreadCollection_h__

#include "../../StaticFunctions/StaticFunctions.h"
#include "../../Parvicursor/xThread/xThreadBase.h"
#include "../../Parvicursor/Serialization/Serializer.h"
#include "../../System.IO/File/File.h"
#include "../../System.IO/FileStream/FileStream.h"
#include "../../System.IO/IOException/IOException.h"
#include "../../System/BasicTypes/BasicTypes.h"
#include "../../System.Threading/Thread/Thread.h"
#include "../../System.Threading/Barrier/Barrier.h"
#include "../../System/Object/Object.h"
#include "../../System/ArgumentException/ArgumentNullException.h"
#include "../../System.Collections/ArrayList/ArrayList.h"
#include "../../System.IO/Directory/Directory.h"
#include "../../System/Guid/Guid.h"
//#include "../../Parvicursor/xThread/xThreadState.h"


#if !defined WIN32 || WIN64
#include <sys/types.h>
#include <dlfcn.h>
#endif
//**************************************************************************************************************//

using namespace System;
using namespace System::Collections;
using namespace System::IO;
using namespace System::Threading;

using namespace Parvicursor::Serialization;
using namespace Parvicursor::xThread;


/*struct CleanupInfo
{
	xThreadBase *obj;
	Barrier *barrier;
};*/

namespace Parvicursor
{
	//----------------------------------------------------
   namespace xThread
   {
		//----------------------------------------------------
	   class xThreadCollection : public Object
	   {
			public: class ThreadInfo : public Object
			{
				public: xThreadBase *objInstance;
				public: String DllFilename;
				public: String workingDirectory;
				public: String guid;
				public: char *serializedBuffer;
				public: char *serializedDLLBuffer;
				public: Int32 serializedBuffer_size;
				public: Int32 currentSerializedBuffer_size;
				public:	ThreadState state;
				public: pthread_t thread;
				public: Thread *worker;
				public: bool numbered;
				//public: CleanupInfo *cleanupInfo;

#if defined WIN32 || WIN64
				HINSTANCE lib_handle;
#else
				void *lib_handle;
#endif

				public: ThreadInfo();
				public: ~ThreadInfo();
				public: static void *Wrapper_To_Call_StartProc(void* pt2Object);
			};
			//----------------------------------------------------
			public: ArrayList *set;
			private: bool fromXThreadClientClass;
			//----------------------------------------------------
			public: xThreadCollection();
			public: ~xThreadCollection();
			// Adds a new xThread instance to current xThreadCollection.
			public: void AddNewThreadInstance(const xThreadBase *objInstance, const String &DllFilename);
			public: Serializer *Serialize();
			public: void DeSerialize(DeSerializer &deserializer, Int32 MaxThreadSupport, const String &sessionGuid);
			//public: ArrayList &get_Collection() const;
			public: Int32 get_Count();
			public: xThreadCollection::ThreadInfo *get_Value(Int32 index);
			//----------------------------------------------------
		};
		//----------------------------------------------------
	}
}
//**************************************************************************************************************//

#endif

