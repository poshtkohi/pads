/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Threading_ThreadPool_h__
#define __System_Threading_ThreadPool_h__

#include "../../general.h"
#include "../../System/BasicTypes/BasicTypes.h"
#include "../../System/Object/Object.h"
#include "../../System.Collections/Queue/Queue.h"
#include "../../System/ObjectDisposedException/ObjectDisposedException.h"
#include "../../System.Threading/Thread/Thread.h"
#include "../../System.Threading/Mutex/Mutex.h"
#include "../../System.Threading/ConditionVariable/ConditionVariable.h"

using namespace System;
using namespace System::Collections;
using namespace System::Threading;
//**************************************************************************************************************//

namespace System
{
	namespace Threading
	{
		//----------------------------------------------------
		// Represents a callback method to be executed by a ThreadPool thread.
		typedef void (*WaitCallback)(Object *);

		class ThreadPool : public Object
		{
			/*---------------------fields----------------*/
			private: struct TaskInfo : public Object
			{
				Object *state;
				WaitCallback callback;
			};
			private: Queue q; // The ThreadPool task queue.
			private: bool disposed; // For destructor use.
			private: Mutex *mutex;
			private: ConditionVariable *poolNotEmpty, *poolNotFull, *waitcv;
			private: UInt32 numThreads;
			private: UInt32 queueMaxSize;
			private: Thread **workers;
			/*---------------------methods----------------*/
			// ThreadPool Class constructor.
			public: ThreadPool(UInt32 numThreads, UInt32 queueMaxSize);
			// ThreadPool Class destructor.
			public: ~ThreadPool();
			// Gets the number of elements available in the ThreadPool.
			public: Int32 get_Count();
			// Queues a method for execution, and specifies an object containing data to be used by the method.
			// The method executes when a thread pool thread becomes available.
			// If the method is successfully queued, true will be returned.
			public: bool QueueUserWorkItem(WaitCallback callBack, Object *state);
			// Waits on the ThreadPool task queue until the queue is empty.
			public: void WaitOnTaskQueue();
			private: void *Worker(void *ptr);
			private: static void *Wrapper_To_Call_Worker(void *pt2Object);
		};
		//----------------------------------------------------
	}
};
//**************************************************************************************************************//

#endif

