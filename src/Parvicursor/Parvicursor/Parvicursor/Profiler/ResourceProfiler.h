/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __Parvicursor_Profiler_ResourceProfiler_h__
#define __Parvicursor_Profiler_ResourceProfiler_h__


#include "../../general.h"
#include "../../StaticFunctions/StaticFunctions.h"
#include "../../System/BasicTypes/BasicTypes.h"
#include "../../System/Object/Object.h"
#include "../../System/Exception/Exception.h"

#include <time.h>
#include  <math.h>
#include  <stdlib.h>
//**************************************************************************************************************//


/*#if !defined ParvicursorLib_inlining
//#warning hello world
#ifndef __Parvicursor_inline__
#define __Parvicursor_inline__ inline
#endif
#else
#ifndef __Parvicursor_inline__
#define __Parvicursor_inline__
//#warning hello
#endif
#endif*/

using namespace System;

namespace Parvicursor
{
	//----------------------------------------------------
   namespace Profiler
   {
	   struct usage
	   {
			struct timeval ru_utime; // user CPU time used
			struct timeval ru_stime; // system CPU time used
			long   ru_maxrss;        // maximum resident set size
			long   ru_ixrss;         // integral shared memory size
			long   ru_idrss;         // integral unshared data size
			long   ru_isrss;         // integral unshared stack size
			long   ru_minflt;        // page reclaims (soft page faults)
			long   ru_majflt;        // page faults (hard page faults)
			long   ru_nswap;         // swaps
			long   ru_inblock;       // block input operations
			long   ru_oublock;       // block output operations
			long   ru_msgsnd;        // IPC messages sent
			long   ru_msgrcv;        // IPC messages received
			long   ru_nsignals;      // signals received
			long   ru_nvcsw;         // voluntary context switches
			long   ru_nivcsw;        // involuntary context switches

			/*
					 A voluntary context  switch  occurs  when  a task  blocks
					 because  it  requires  a  resource  that is unavailable.
			*/

			/*
					 An  involuntary  context switch takes place when a task
					 executes for the duration of its  time slice and then is
					 forced to relinquish the processor.

			*/

			usage operator -(usage &u);

		};
		//----------------------------------------------------
		class ResourceProfiler : public Object
		{
			/*
			    This method provides measures of the resources used by the current process
				or its terminated and waited-for child processes.
			*/
			public: /*__Parvicursor_inline__*/ static void GetResourceUsage(Out usage *u);
			public: static void PrintResourceUsage(In usage *u);
			/*
				This method tells how much time the calling process has used.
				A timer tick is approximately equal to 1/CLOCKS_PER_SEC second.
			*/
			public: /*__Parvicursor_inline__*/ static Int64 GetClock();
		};
		//----------------------------------------------------
	}
}
//----------------------------------------------------

#if defined ProfilerMode

using namespace Parvicursor::Profiler;

/*
*	This marco MUST only placed within .cpp files (implementation units) outside any
*	implementation units not the .h files, because it can lead to non thread-safe
*	behaviors in multi-threaded programs.
*/

#define Parvicursor_RESOURCE_PROFILER_INITIALIZE() \
	static struct usage *__start_u__ = null;\
	static struct usage *__end_u__ = null;\
	static Int64 __start__ = 0, __end__ = 0;

#define Parvicursor_RESOURCE_PROFILER_FINALIZE() \
	if(__start_u__ != null)\
	do {\
		delete __start_u__;\
		__start_u__ = null;\
    } while (0);\
	if(__end_u__ != null)\
	do {\
		delete __end_u__;\
		__end_u__ = null;\
    } while (0);\
	__start__ = __end__ = null;


#define Parvicursor_RESOURCE_PROFILER_BEGIN() \
	if(__start_u__ == null)\
	do {\
		__start_u__ = new usage();\
    } while (0);\
	if(__end_u__ == null)\
	do {\
		__end_u__ = new usage();\
    } while (0);\
	ResourceProfiler::GetResourceUsage(__start_u__);\
	__start__ = ResourceProfiler::GetClock();


#define Parvicursor_RESOURCE_PROFILER_END() \
	if(__start_u__ == null || __end_u__ == null)\
	do {\
		throw System::Exception("The Parvicursor_RESOURCE_PROFILER_BEGIN marco must be fisrt called.");\
    } while (0);\
	__end__ = ResourceProfiler::GetClock();\
	ResourceProfiler::GetResourceUsage(__end_u__);\
	struct usage __elapsed_u__;\
	__elapsed_u__ = *__end_u__ - *__start_u__;\
	cout << "\nelapsed resource usage:" << endl;\
	ResourceProfiler::PrintResourceUsage(&__elapsed_u__);\
	cout << "\nelapsed clock: " << (__end__ - __start__) << endl;

#else


#define Parvicursor_RESOURCE_PROFILER_INITIALIZE()
#define Parvicursor_RESOURCE_PROFILER_FINALIZE()
#define Parvicursor_RESOURCE_PROFILER_BEGIN()
#define Parvicursor_RESOURCE_PROFILER_END()

#endif
//----------------------------------------------------

/*
#define Parvicursor_RESOURCE_PROFILER_BEGIN() \
	struct usage __start_u__, __end_u__;\
	ResourceProfiler::GetResourceUsage(&__start_u__);\
	Int64 __start__, __end__;\
	__start__ = ResourceProfiler::GetClock();\


#define Parvicursor_RESOURCE_PROFILER_END() \
	__end__ = ResourceProfiler::GetClock();\
	ResourceProfiler::GetResourceUsage(&__end_u__);\
	cout << endl;\
	struct usage __elapsed_u__;\
	__elapsed_u__ = __end_u__ - __start_u__;\
	cout << "elapsed resource usage:" << endl;\
	ResourceProfiler::PrintResourceUsage(&__elapsed_u__);\
	cout << "\nelapsed clock: " << (__end__ - __start__) << endl;*/

//----------------------------------------------------

//**************************************************************************************************************//

#endif

