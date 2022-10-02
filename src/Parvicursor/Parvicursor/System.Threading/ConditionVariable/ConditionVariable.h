/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Threading_ConditionVariable_h__
#define __System_Threading_ConditionVariable_h__

#include "../../general.h"
#include "../../System/Object/Object.h"
#include "../../System/BasicTypes/BasicTypes.h"
#include "../../System/Exception/Exception.h"
#include "../../System/ArgumentException/ArgumentNullException.h"
#include "../../System/ObjectDisposedException/ObjectDisposedException.h"
#include "../../System.Threading/Thread/Thread.h"
#include "../../System.Threading/Mutex/Mutex.h"

using namespace System;
using namespace System::Threading;
//**************************************************************************************************************//
namespace System
{
	namespace Threading
	{
		class ConditionVariable : public Object
		{
			//----------------------------------------------------
			private: Int32 ret;
			private: pthread_cond_t cv;
			private: Mutex *mutex;
			private: struct timespec time;
			private: bool disposed;
			//----------------------------------------------------
			public: ConditionVariable(Mutex *mutex);
			public: ~ConditionVariable();
		    public: void Close();
			// Waits on a ConditionVariable.
			public: inline void Wait()
			{
                ret = pthread_cond_wait(&cv, mutex->get_Handle());
			}
            // Unblocks at least one of the threads that are blocked on the specified ConditionVariable.
			public: inline void Signal()
			{
                ret = pthread_cond_signal(&cv);
			}
			// Unblock all threads currently blocked on the specified ConditionVariable.
			public: inline void Broadcast()
			{
                ret = pthread_cond_broadcast(&cv);
			}
			// Waits on a ConditionVariable with the sepcified timout.
			public: void TimedWait(Long seconds, Long nanoseconds);
			//----------------------------------------------------
		};


	};
};
//**************************************************************************************************************//

#endif
