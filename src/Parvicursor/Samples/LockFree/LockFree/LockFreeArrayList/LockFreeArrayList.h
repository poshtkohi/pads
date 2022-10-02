#ifndef __LockFree_LockFreeArrayList_h__
#define __LockFree_LockFreeArrayList_h__


#include "../../../../Parvicursor/general.h"
#include "../../../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../../../Parvicursor/Atomic/atomic.h"
#include "../../../../Parvicursor/System.Threading/Thread/Thread.h"
#include "../../../../Parvicursor/System/Object/Object.h"
#include "../../../../Parvicursor/System/ArgumentException/ArgumentOutOfRangeException.h"

#include "../../LockFree/LockFreeMemoryPool/LockFreeMemoryPool.h"
#include "../../LockFree/LockFreeCounter/LockFreeCounter.h"

#define __REPEAT_Array_List__ 10
#define __Threads__ 20
#define __maxCapacity_Array_List__ 100000

//------------------------------------------------------------------------------

using namespace System;
using namespace System::Threading;

//**************************************************************************************************************//

namespace LockFree
{
		enum Status
		{
			ArrayListIsFull = 0,
			DuplicateData = 1,
			NoSuchData = 2,
			Success = 3
		};
		struct Node
		{
			Object *data;
			struct Node volatile *next;
		};
		//----------------------------------------------------
		// A lock-free array list based on a lock-free singly linked list designed by Harris.
		class LockFreeArrayList : public Object
        {
			/*---------------------fields----------------*/
			private: Node volatile *head; // The head sentinel node.
			private: Node volatile *tail; // The tail sentinel node.
			private: UInt32 maxCapacity; // The maximum capacity of a ArrayList is the number of elements that the ArrayList can hold.
			private: LockFreeCounter counter; // Indicates the number of non-free slots in the LockFreeArrayList.
			private: bool disposed; // For destructor use.
			private: LockFreeMemoryPool *pool; // The memory pool used to allocate and deallocate nodes.
			/*---------------------methods----------------*/
			// LockFreeArrayList Class constructor.
			public: LockFreeArrayList(UInt32 maxCapacity);
			// LockFreeArrayList Class destructor.
			public: ~LockFreeArrayList();
			//public: void Clear();
			// Adds an object to the end of the ArrayList.
			public: Status Add(Object *data);
			// Removes the occurrence of a specific object from the LockFreeArrayList.
			public: void Remove(Object *data);
			//public: void Insert(Int32 index, Object *value);
			// Gets the number of elements actually contained in the LockFreeArrayList.
			public: Long get_Count();
			// Determines whether an element is in the LockFreeArrayList.
			public: bool Contains(Object *data);
			//public: void RemoveAt(Int32 index);
			//public: Object *get_Value(Int32 index);
			//public: void set_Value(Int32 index, Object *obj);
			public: void Print();
			// My methods based on Timotht L. Harris's modified methods.
			private: static inline bool CAS(void volatile *address, Long old, Long _new);
			private: Status insert(Object *data);
			private: bool find(Object *data);
			public: Status _delete(Object *data);
			private: Node volatile *search(Object *data, Node volatile  **left_node);
		};
		//----------------------------------------------------



		//----------------------------------------------------
		// Test class for the lock-free array list.
		class LockFreeArrayListTest : public Object
		{
			/*---------------------fields----------------*/
			private: LockFreeArrayList *al;
			private: bool disposed;
			private: Thread **workers;
			private: UInt32 maxCapacity;
			private: LockFreeCounter threadNum;
			/*---------------------methods----------------*/
			public: LockFreeArrayListTest(UInt32 maxCapacity)
			{
				this->maxCapacity = maxCapacity;
				al = new LockFreeArrayList(this->maxCapacity);
				disposed = false;
				workers = new Thread*[__Threads__];
			}
			/*-------------------------------------------*/
			public: ~LockFreeArrayListTest()
			{
				if(!disposed)
				{
					if(workers != null)
					{
						for(Int32 i = 0 ; i < __Threads__ ; i++)
							if(workers[i] != null)
								delete workers[i];

						delete workers;
					}

					if(al != null)
						delete al;

					this->disposed = true;
				}
			}
			/*-------------------------------------------*/
			public: void Run()
			{
				printf("__Threads__: %d\n", __Threads__);

				for(Int32 i = 0 ; i < __Threads__ ; i++)
				{
					workers[i] = new Thread(LockFreeArrayListTest::Wrapper_To_Call_Worker, (void *)this);
					workers[i]->Start();
				}

				for(Int32 i = 0 ; i < __Threads__ ; i++)
					workers[i]->Join();
			}
			/*-------------------------------------------*/
			public: Long get_Count()
			{
				return al->get_Count();
			}
			/*-------------------------------------------*/
			public: void Print()
			{
				al->Print();
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
				Long *x = new Long[maxCapacity/__Threads__];

				Int32 counter = 0;
				Int32 i;
				while (true)
				{
					if(counter == __REPEAT_Array_List__)
						break;

					for(i = 0 ; i < maxCapacity/__Threads__ ; i++)
					{
						x[i] = rand();
						if(al->Add((Object *)&x[i]) == ArrayListIsFull)
						{
							printf("full%d: %d\n", threadID, i);
							//cout << "Full" << threadID << ": " << i << endl;
							i--;
							break;
						}

						printf("Add%d: %d\n", threadID, i);
						//cout << "Add" << threadID << ": " << i << "\n" << endl;
					}
					//Thread::Sleep(1000);

					for(Int32 j =  0 ; j < i ; j++)
					{
						al->Remove((Object *)&x[j]);
						printf("Remove%d: %d\n", threadID, j);
						//cout << "Remove" << threadID << ": " << j << "\n" << endl;
					}
					//Thread::Sleep(1000);

					counter++;
				}

				delete x;

				return ptr;
			}
			/*-------------------------------------------*/
			private: Long get_threadNum_And_Increment()
			{

				volatile Long temp1 = -10;
				Long temp2 = -10;

				while(!Parvicursor_AtomicCompareAndSwap(&temp1, &threadNum, temp2))
				{
					temp2 = temp1 + 1;
				}

				return temp1;
			}
			/*-------------------------------------------*/
			private: static void *Wrapper_To_Call_Worker(void *pt2Object)
			{
				// explicitly cast to a pointer to LockFreeArrayListTest
				LockFreeArrayListTest *mySelf = (LockFreeArrayListTest *)pt2Object;

				// call member
				mySelf->Worker(pt2Object);

				return pt2Object;
			}
			/*-------------------------------------------*/
			public: static int Parvicursor_main(int argc, char *argv[])
			{
				/*Int64 accum = 10, dest = 10, newVal = 20;
				Parvicursor_AtomicCompareAndSwap64bit(&accum, &dest, newVal);

				cout << "accum: " << accum << " dest: " << dest << " newVal: " << newVal << endl;

				cout << sizeof(Int32) << endl;
				return 0;*/

				/*Int32 *x = new Int32[1000];
				cout << convBase((ULong)x, 2) << endl;
				cout << convBase((ULong)get_marked_reference(x), 2) << endl;
				cout << convBase((ULong)get_unmarked_reference(x), 2) << endl;
				cout << sizeof(x) << endl;
				delete x;
				return 0;*/

				LockFreeArrayListTest *al_test = new LockFreeArrayListTest(__maxCapacity_Array_List__);
				al_test->Run();
				printf("al_test->get_Count=%d\n", al_test->get_Count());
				delete al_test;

				return 0;

				/*Int32 *x = new Int32[__maxCapacity_Array_List__/2];
				LockFreeArrayList *al = new LockFreeArrayList(__maxCapacity_Array_List__);
				//ArrayList *al = new ArrayList();

				while(true)
				{
				    Int32 i =  0 ;
					for(i =  0 ; i < __maxCapacity_Array_List__/2 ; i++)
					{
						x[i] = i;//rand();
						if(al->Add((Object *)&x[i]) == ArrayListIsFull)
						{
							cout << "full" << endl;
							//i--;
							break;
						}

						printf("Add: %d\n", i);
					}

					for(Int32 j =  0 ; j < i ; j++)
					{
						printf("Remove: %d\n", j);
						al->Remove((Object *)&x[j]);
					}

					//cout << "count2: " << al->get_Count() << endl;
					Thread::Sleep(1000);
				}

				delete x;
				delete al;

				getchar();

				return 0;*/
			}
			/*-------------------------------------------*/
		};
		//----------------------------------------------------
};
//**************************************************************************************************************//

#endif

