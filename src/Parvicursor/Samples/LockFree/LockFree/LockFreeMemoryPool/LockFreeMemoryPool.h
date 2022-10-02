#ifndef __LockFree_LockFreeMemoryPool_h__
#define __LockFree_LockFreeMemoryPool_h__


#include "../../../../Parvicursor/general.h"
#include "../../../../Parvicursor/StaticFunctions/StaticFunctions.h"
#include "../../../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../../../Parvicursor/Atomic/atomic.h"
#include "../../../../Parvicursor/System.Threading/Thread/Thread.h"

#include "../LockFreeCounter/LockFreeCounter.h"
//#include "../../../../Parvicursor/Parvicursor/Profiler/ResourceProfiler.h"
//#define ProfilerMode

using namespace System;
//using namespace System::Collections;
using namespace System::Threading;

#define __REPEAT_MEMORY_POOL__ 10
#define __Threads_MEMORY_POOL__ 10
//**************************************************************************************************************//

namespace LockFree
{
		//----------------------------------------------------
		// A lock-free memory pool based on a singly free linked list.
		class LockFreeMemoryPool : public Object
        {
			/*---------------------fields----------------*/
			private: struct Header // The header definition.
			{
				struct Header volatile *next; // The successor node with respect to this node.
				// bool isFree;
				// Actually, this struct can locate in one word, we can use LSB of the next pointer as the free flag.
			};
			private: Header volatile *first; // The first pointer.
			private: Header volatile *last; // The last pointer.
			private: Header volatile *head; // The head pointer.
			private: Header volatile *dummy; // A dummy node used in the pool implementation.
			private: Header volatile *_dummy;
			private: bool disposed; // For destructor use.
			private: UInt64 poolSlots; // The number of slots within memory block.
			private: UInt64 slotSize; // The fixed size of the allocted memory associated with every slot.
			private: void volatile *buffer; // The allocated memory for the memory block and free linked list.
			private: LockFreeCounter counter; // Indicates the number of non-free slots in the memory pool.
			/*---------------------methods----------------*/
		    // LockFreeMemoryPool Class constructor.
			public: LockFreeMemoryPool(UInt64 slotSize, UInt64 poolSlots);
			// LockFreeMemoryPool Class destructor.
			public: ~LockFreeMemoryPool();
			// Pre-allocates the memory block and free linked list
			private: void PreAllocate();
			// Deallocates the whole allocated memory regions.
			private: void DeAllocate();
			// Returns a void pointer to the allocated space or null if there is insufficient memory available.
			public: void *Malloc();
			// Deallocates or frees a memory slot.
			public: void Free(void *memslot);
			// Gets the number of non-free slots in the memory pool.
			public: Long get_Count();
			public: void Print();
		};
		//----------------------------------------------------





		//----------------------------------------------------
		// Test class for the lock-free memory pool.
		class LockFreeMemoryPoolTest : public Object
		{
			/*---------------------fields----------------*/
			private: LockFreeMemoryPool *pool;
			private: bool disposed;
			private: UInt64 poolSlots;
			private: Thread **workers;
			private: LockFreeCounter threadNum;
			/*---------------------methods----------------*/
			public: LockFreeMemoryPoolTest(UInt64 poolSlots)
			{
				this->poolSlots = poolSlots;
				pool = new LockFreeMemoryPool(sizeof(Int32), this->poolSlots);
				workers = new Thread*[__Threads_MEMORY_POOL__];
				disposed = false;
			}
			/*-------------------------------------------*/
			public: ~LockFreeMemoryPoolTest()
			{
				if(!disposed)
				{
					if(pool != null)
						delete pool;

					if(workers != null)
					{
						for(Int32 i = 0 ; i < __Threads_MEMORY_POOL__; i++)
							if(workers[i] != null)
								delete workers[i];

						delete workers;
					}

					disposed = true;
				}
			}
			/*-------------------------------------------*/
			public: void Run()
			{
				for(Int32 i = 0 ; i < __Threads_MEMORY_POOL__ ; i++)
				{
					workers[i] = new Thread(LockFreeMemoryPoolTest::Wrapper_To_Call_Worker, (void *)this);
					workers[i]->Start();
				}

				for(Int32 i = 0 ; i < __Threads_MEMORY_POOL__ ; i++)
					workers[i]->Join();
			}
			/*-------------------------------------------*/
			public: Long get_Count()
			{
				return pool->get_Count();
			}
			/*-------------------------------------------*/
			public: void Print()
			{
				pool->Print();
			}
			/*-------------------------------------------*/
			private: void *Worker(void *ptr)
			{
				Int32 threadID = threadNum.get_Count_And_Increment();
				/*if(threadID == 0)
				{
					// Write the code for master worker here
					return ;
				}*/
				Long *xx = new Long[poolSlots];

				Int32 counter = 0;
				Int32 i;

				while (true)
				{
					if(counter == __REPEAT_MEMORY_POOL__)
						break;

					i = 0;
					while(true)
					{
						Int32 *x = (Int32 *)pool->Malloc();
						if(x == null)
							break;
						else
						{
							printf("malloc%d: %d\n", threadID, i);
							*x = i;
							xx[i] = (Long)x;
							i++;
						}
					}

					for(UInt64 j = 0 ; j < i ; j++)
					{
						printf("free%d: %d\n", threadID, j);
						pool->Free((Int32 *)xx[j]);
					}

					counter++;
				}

				delete xx;
				return ptr;
			}
			/*-------------------------------------------*/
			private: static void *Wrapper_To_Call_Worker(void *pt2Object)
			{
				// explicitly cast to a pointer to LockFreeMemoryPoolTest
				LockFreeMemoryPoolTest *mySelf = (LockFreeMemoryPoolTest *)pt2Object;

				// call member
				mySelf->Worker(pt2Object);

				return pt2Object;
			}
			/*-------------------------------------------*/
			public: static int Parvicursor_main(int argc, char *argv[])
			{
				LockFreeMemoryPoolTest *test = new LockFreeMemoryPoolTest(10);
				test->Run();
				printf("test->get_Count=%d\n", test->get_Count());
				getchar();
				delete test;
				return 0;

				//Long accum = -10, dest = -10, newVal = 20;

				/*volatile Int32 *accum = new Int32();
				volatile Int32 *dest = new Int32();
				*accum = 10;
				*dest = 10;
				Int32 newVal = 20;

				bool ret = false;

				ret = Parvicursor_AtomicCompareAndSwap(accum, dest, newVal);


				cout << "accum: " << *accum << " dest: " << *dest << " newVal: " << newVal << " ret: "<< ret << endl;

				return 0;*/

				/*Int32 maxCapacity = 10;
				Int32 *x = new Int32[maxCapacity];
				LockFreeMemoryPool *pool = new LockFreeMemoryPool(sizeof(Int32), maxCapacity);

				while(true)
				{
					Int32 i;

					for(i = 0 ; i < maxCapacity ; i++)
					{
						Int32 *temp = (Int32 *)pool->Malloc();
						if(temp == null)
						{
							printf("full\n");
							break;
						}
						*temp = rand();
						x[i] = (Long)temp;
						printf("malloc: %d\n", i);
					}

					for(Int32 j = 0 ; j < i ; j++)
					{
						Int32 *temp = (Int32 *)x[j];
						//printf("x[%d]=%d\n",  j, *temp);
						printf("free: %d\n", j);
						pool->Free(temp);
					}

					//pool->Print();

					printf("--------------------------------------------\n\n");

					Thread::Sleep(1000);
				}
				delete x;
				delete pool;

				getchar();
				return 0;*/
			}
			/*-------------------------------------------*/
		};
		//----------------------------------------------------
};
//**************************************************************************************************************//

#endif

