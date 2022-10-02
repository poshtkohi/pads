#ifndef __Concurrency_ThreadPoolExample3_h__
#define __Concurrency_ThreadPoolExample3_h__

#include "../../../../Parvicursor/general.h"
#include "../../../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../../../Parvicursor/System/Object/Object.h"
#include "../../../../Parvicursor/System/Environment/Environment.h"
#include "../../../../Parvicursor/System/Math/Math.h"
#include "../../../../Parvicursor/System/ObjectDisposedException/ObjectDisposedException.h"
#include "../../../../Parvicursor/System.Threading/ThreadPool/ThreadPool.h"
#include "../../../../Parvicursor/System.Threading/Mutex/Mutex.h"
#include "../../../../Parvicursor/System.Threading/ConditionVariable/ConditionVariable.h"
#include "../../../../Parvicursor/Atomic/atomic.h"
#include "../../../../Parvicursor/Parvicursor/Profiler/ResourceProfiler.h"

using namespace System;
using namespace System::Threading;

#define __ThreadPoolExample3_LockFree__

//_________________________________
//Parvicursor_RESOURCE_PROFILER_INITIALIZE()
//_________________________________

//**************************************************************************************************************//

namespace Concurrency
{
	//----------------------------------------------------
	class ThreadPoolExample3 : public Object
	{
		/*---------------------fields----------------*/
		private: struct MethodInfo : public Object
		{
			Int32 n;
			Int32 *prime;
			Long volatile *nextbase;
			Mutex *nextbaselock;
			Int32 methodNum;
			Int32 *counter_addr;
			Mutex *mutex;
			ConditionVariable *cv;
		};
		/*---------------------methods----------------*/
		 // Gets the current value of the count variable and increments it.
		private: static inline Long Increment_And_Get_Value(Long volatile *count, Long value)
		{
			Long volatile temp1 = -10;
			Long temp2 = -10;
			while(!Parvicursor_AtomicCompareAndSwap(&temp1, count, temp2))
			{
				temp2 = temp1 + value;
			}
			return temp2;
		}
		/*--------------------------------------------*/
		private: static void crossout(Int32 *prime, Int32 k, Int32 n)
		{  
			Int32 i;
			for (i = k ; i*k <= n ; i++)
				prime[i*k] = 0;
		}
		/*-------------------------------------------*/
		private: static void SieveCallback(Object *state)
		{
			MethodInfo *info = (MethodInfo *)state;
			register Int32 lim, base;
			// no need to check multipliers bigger than sqrt(n)
			lim = Math::Sqrt(info->n);

			while(true)
			{
				// get next sieve multiplier, avoiding duplication across threads
#ifdef __ThreadPoolExample3_LockFree__
				base = Increment_And_Get_Value(info->nextbase, 2);
#else
				info->nextbaselock->Lock();
				(*info->nextbase) += 2; // Increment two units to the counter value by its address.
				base = (*info->nextbase);
				info->nextbaselock->Unlock();
#endif

				if (base <= lim)  {
					//work[tn]++;  // log work done by this thread
					// don't bother with crossing out if base is known to be
					// composite
					if (info->prime[base])
						crossout(info->prime, base, info->n);
				}
				else 
					break;
			}

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
			// Shared variables
			const Int32 methodNum = 100;
			const Int32 n = 100000000; // upper bound of range in which to find primes
			Int32 *prime = new Int32[n + 1];  // in the end, prime[i] = 1 if i prime, else 0
			Long volatile nextbase = 1;  // next sieve multiplier to be used
			Mutex *nextbaselock = new Mutex();

			for(Int32 i = 2 ; i <= n; i++) 
				prime[i] = 1;
			crossout(prime, 2, n);

			// Intilizes the thread pool instance.
			ThreadPool pool = ThreadPool(methodNum, 10000);
			MethodInfo methods[methodNum];
			Mutex *mutex = new Mutex();
			ConditionVariable *cv = new ConditionVariable(mutex);
			Int32 counter = 0; // The main thread waits on the value of this shared counter.

			//_________________________________
			//Parvicursor_RESOURCE_PROFILER_BEGIN()
			//_________________________________

			for(register Int32 i = 0 ; i < methodNum ; i++)
			{
				methods[i].n = n;
				methods[i].prime = prime;
				methods[i].nextbaselock = nextbaselock;
				methods[i].methodNum = methodNum;
				methods[i].nextbase = &nextbase;
				methods[i].counter_addr = &counter;
				methods[i].mutex = mutex;
				methods[i].cv = cv;
				pool.QueueUserWorkItem(SieveCallback, &methods[i]);
			}

			// Waits on counter until all methods complete.
			mutex->Lock();
			cv->Wait();
			mutex->Unlock();

			//_________________________________
			//Parvicursor_RESOURCE_PROFILER_END()
			//_________________________________

			//_________________________________
			//Parvicursor_RESOURCE_PROFILER_FINALIZE()
			//_________________________________

			// report results
			Int32 nprimes = 0;  // number of primes found 
			for(Int32 i = 2 ; i <= n ; i++)  
				if(prime[i])
					nprimes++;
			printf("the number of primes found was %d\n",nprimes);

			delete prime;
			delete nextbaselock;
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
