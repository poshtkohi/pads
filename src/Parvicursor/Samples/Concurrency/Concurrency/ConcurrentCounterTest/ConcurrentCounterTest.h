#ifndef __Concurrency_ConcurrentCounterTest_h__
#define __Concurrency_ConcurrentCounterTest_h__

#include "../../../../Parvicursor/general.h"
#include "../../../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../../../Parvicursor/System.Threading/Thread/Thread.h"
#include "../../../../Parvicursor/System.Threading/Mutex/Mutex.h"
#include "../../../../Parvicursor/System.Threading/ConditionVariable/ConditionVariable.h"

using namespace System;
using namespace System::Threading;
//**************************************************************************************************************//

namespace Concurrency
{
	//----------------------------------------------------
	// Test class for the condition-varibale counter.
	class ConcurrentCounterTest : public Object
	{
		/*---------------------fields----------------*/
		private: bool disposed;
		private: Thread *counter_thread;
		private: Thread *watcher_thread;
		private: Int32 count;
		private: Mutex *mutex;
		private: ConditionVariable *cv;
		/*---------------------methods----------------*/
		public: ConcurrentCounterTest()
		{
			count = 0;
			mutex = new Mutex();
			cv = new ConditionVariable(mutex);
			disposed = false;
		}
		/*-------------------------------------------*/
		public: ~ConcurrentCounterTest()
		{
			if(!disposed)
			{
				if(counter_thread != null)
					delete counter_thread;
				if(watcher_thread != null) 
					delete watcher_thread;
				if(mutex != null)
					delete mutex;
				if(cv != null)
					delete cv;

				disposed = true;
			}
		}
		/*-------------------------------------------*/
		public: void Run()
		{
			counter_thread = new Thread(ConcurrentCounterTest::Wrapper_To_Call_Counter, (void *)this);
			watcher_thread = new Thread(ConcurrentCounterTest::Wrapper_To_Call_Watcher, (void *)this);

			counter_thread->Start();
			watcher_thread->Start();

			counter_thread->Join();
			watcher_thread->Join();
		}
		/*-------------------------------------------*/
		private: void *Counter_proc(void *ptr)
		{
			while(true)
			{
				mutex->Lock();
				count++;
				cv->Signal();
				mutex->Unlock();
				Thread::Sleep(1);
			}
			return ptr;
		}
		/*-------------------------------------------*/
		private: void *Watcher_proc(void *ptr)
		{
			while(true)
			{
				mutex->Lock();
				cv->Wait();
				printf("%d\n", count);
				mutex->Unlock();
			}
			return ptr;
		}
		/*-------------------------------------------*/
		private: static void *Wrapper_To_Call_Counter(void *pt2Object)
		{
			// explicitly cast to a pointer to ConcurrentCounterTest
			ConcurrentCounterTest *mySelf = (ConcurrentCounterTest *)pt2Object;

			// call member
			mySelf->Counter_proc(pt2Object);

			return pt2Object;
		}
		/*-------------------------------------------*/
		private: static void *Wrapper_To_Call_Watcher(void *pt2Object)
		{
			// explicitly cast to a pointer to ConcurrentCounterTest
			ConcurrentCounterTest *mySelf = (ConcurrentCounterTest *)pt2Object;

			// call member
			mySelf->Watcher_proc(pt2Object);

			return pt2Object;
		}
		/*-------------------------------------------*/
		public: static int Parvicursor_main(int argc, char *argv[])
		{

			ConcurrentCounterTest *counter = new ConcurrentCounterTest();
			counter->Run();
			delete counter;

			return 0;
		}
		/*-------------------------------------------*/
	};
	//----------------------------------------------------
};
//**************************************************************************************************************//

#endif

