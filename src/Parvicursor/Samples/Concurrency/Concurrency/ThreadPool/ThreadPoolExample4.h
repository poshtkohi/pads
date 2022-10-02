#ifndef __Concurrency_ThreadPoolExample4_h__
#define __Concurrency_ThreadPoolExample4_h__

#include "../../../../Parvicursor/general.h"
#include "../../../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../../../Parvicursor/System/Object/Object.h"
#include "../../../../Parvicursor/System/Environment/Environment.h"
#include "../../../../Parvicursor/System/Math/Math.h"
#include "../../../../Parvicursor/System/ObjectDisposedException/ObjectDisposedException.h"
#include "../../../../Parvicursor/System.Threading/ThreadPool/ThreadPool.h"
#include "../../../../Parvicursor/System.Threading/Mutex/Mutex.h"
#include "../../../../Parvicursor/System.Threading/ConditionVariable/ConditionVariable.h"
#include "../../../../Parvicursor/System.Threading/Barrier/Barrier.h"
#include "../../../../Parvicursor/Atomic/atomic.h"
#include "../../../../Parvicursor/Parvicursor/Profiler/ResourceProfiler.h"

using namespace System;
using namespace System::Threading;
//**************************************************************************************************************//

namespace Concurrency
{
	//----------------------------------------------------
	class ThreadPoolExample4 : public Object
	{
		/*---------------------fields----------------*/
		private: struct MethodInfo : public Object
		{
			Int32 methodID;
			Barrier *barrier;
			Int32 methodNum;
			Int32 *counter_addr;
			Mutex *mutex;
			ConditionVariable *cv;
		};
		/*---------------------methods----------------*/
		private: static void MethodCallback(Object *state)
		{
			MethodInfo *info = (MethodInfo *)state;

			printf("I'm thread %d at phase 1\n", info->methodID);
			Thread::Sleep(1000);

			info->barrier->SignalAndWait();

			printf("I'm thread %d at phase 2\n", info->methodID);


			info->mutex->Lock();
			(*info->counter_addr)++; // Increment the counter value by its address
			if(*info->counter_addr == info->methodNum)
				info->cv->Signal(); // Singnal the main thread of all methods's completion.
			info->mutex->Unlock();

			return;
		}
		 /*-------------------------------------------*/
		public: static int Parvicursor_main(int argc, char *argv[])
		{
			const Int32 methodNum = 2;
			// Intilizes the thread pool instance.
			ThreadPool pool = ThreadPool(Environment::get_ProcessorCount()*4, 1000);

			MethodInfo methods[methodNum];
			Mutex *mutex = new Mutex();
			ConditionVariable *cv = new ConditionVariable(mutex);
			Barrier *barrier = new Barrier(methodNum);
			Int32 counter = 0; // The main thread waits on the value of this shared counter.
			for(Int32 i = 0 ; i < methodNum ; i++)
			{
				methods[i].methodID = i;
				methods[i].methodNum = methodNum;
				methods[i].counter_addr = &counter;
				methods[i].barrier = barrier;
				methods[i].mutex = mutex;
				methods[i].cv = cv;
				pool.QueueUserWorkItem(MethodCallback, &methods[i]);
			}

			// Waits on counter until all methods complete.
			mutex->Lock();
			cv->Wait();
			mutex->Unlock();

			delete mutex;
			delete cv;
			delete barrier;

			return 0;
		}
	};
	//----------------------------------------------------
};
//**************************************************************************************************************//

#endif
