#ifndef __Concurrency_ThreadPoolExample1_h__
#define __Concurrency_ThreadPoolExample1_h__

#include "../../../../Parvicursor/general.h"
#include "../../../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../../../Parvicursor/System/Object/Object.h"
#include "../../../../Parvicursor/System/Environment/Environment.h"
#include "../../../../Parvicursor/System/ObjectDisposedException/ObjectDisposedException.h"
#include "../../../../Parvicursor/System.Threading/ThreadPool/ThreadPool.h"
#include "../../../../Parvicursor/System.Threading/Mutex/Mutex.h"
#include "../../../../Parvicursor/System.Threading/ConditionVariable/ConditionVariable.h"

using namespace System;
using namespace System::Threading;
//**************************************************************************************************************//

namespace Concurrency
{
	//----------------------------------------------------
	class ThreadPoolExample1 : public Object
    {
		/*---------------------fields----------------*/
		private: struct MethodInfo : public Object
		{
			Int32 methodID;
			char *message;
			Int32 methodNum;
			Int32 *counter_addr;
			Mutex *mutex;
			ConditionVariable *cv;
		};
		/*---------------------methods----------------*/
		private: static void Print(Object *state)
		{
			MethodInfo *info = (MethodInfo *)state;
			printf("I'm method %d. Method Message: %s\n", info->methodID, info->message);

			info->mutex->Lock();
			(*info->counter_addr)++; // Increment the counter value by its address
			if(*info->counter_addr == info->methodNum)
				info->cv->Broadcast(); // Singnal the main thread of all methods's completion.
			info->mutex->Unlock();
		}
		/*-------------------------------------------*/
		public: static int Parvicursor_main(int argc, char *argv[])
		{
			// Intilizes the thread pool instance.
			ThreadPool pool = ThreadPool(Environment::get_ProcessorCount()*4, 10000);
			const Int32 methodNum = 1000;
			MethodInfo methods[methodNum];
			Int32 counter = 0;
			Mutex *mutex = new Mutex();
			ConditionVariable *cv = new ConditionVariable(mutex);
			for(Int32 i = 0 ; i < methodNum ; i++)
			{
				methods[i].methodID = i;
				methods[i].message = "Hello World";
				methods[i].methodNum = methodNum;
				methods[i].counter_addr = &counter;
				methods[i].mutex = mutex;
				methods[i].cv = cv;
				pool.QueueUserWorkItem(Print, &methods[i]);
			}

			// Waits on counter until all methods complete.
			mutex->Lock();
			cv->Wait();
			mutex->Unlock();

			// Releases the mutex and cv objects.
			delete mutex;
			delete cv;

			return 0;
		}
		/*-------------------------------------------*/
		
	};
	//----------------------------------------------------
};
//**************************************************************************************************************//

#endif

