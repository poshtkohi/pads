#ifndef __LockFree_LockFreeStack_h__
#define __LockFree_LockFreeStack_h__


#include "../../../../Parvicursor/general.h"
#include "../../../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../../../Parvicursor/Atomic/atomic.h"
#include "../../../../Parvicursor/System.Threading/Thread/Thread.h"

#include "../LockFreeCounter/LockFreeCounter.h"

using namespace System;
using namespace System::Threading;

//**************************************************************************************************************//

namespace LockFree
{
		//----------------------------------------------------
		//The lock-free stack is implemented as a doubly linked list.
		class LockFreeStack : public Object
        {
			/*---------------------fields----------------*/
			// The node definition.
			private: struct Node
			{
				Int32 data;
				Int32 index;
				struct Node volatile *prev; // The predecessor node with respect to this node.
				struct Node volatile *next; // The successor node with respect to this node.
			};
			private: Node volatile *head; // The head pointer.
			private: Node volatile *first; // The first pointer.
			private: Node volatile *last; // The last pointer.
			private: Int32 stackSize; // The stackSize is equivalent to the n.
			private: bool disposed; // For destructor use.
			private: Node *dummy; // A dummy node used in the stack implementation.
			private: LockFreeCounter counter; // Stores the number of elements in the stack.
			/*---------------------methods----------------*/
			// LockFreeStack Class constructor.
			public: LockFreeStack(UInt32 stackSize);
			// LockFreeStack Class destructor.
			public: ~LockFreeStack();
			// Pre-allocates the bounded doubly linked list.
			private: void PreAllocate();
			// Deallocates the bounded doubly linked list.
			private: void DeAllocate();
			// Inserts an element at the top of the stack.
			// This method is an O(1) operation.
			public: bool Push(Int32 data); // 
			// Removes and returns the element at the top of the stack.
			// This method is an O(1) operation.
			public: bool Pop(Out Int32 *data);
			// Gets the number of elements available in the stack.
			public: Long get_Count();
			public: void Print();
		};
		//----------------------------------------------------





		//----------------------------------------------------
		// Test class for the lock-free stack.
		class LockFreeStackTest : public Object
		{
			/*---------------------fields----------------*/
			private: LockFreeStack *stack;
			private: bool disposed;
			private: Thread *pusher;
			private: Thread *popper;
			private: Int32 stackSize;
			/*---------------------methods----------------*/
			public: LockFreeStackTest(UInt32 stackSize)
			{
				this->stackSize = stackSize;
				this->pusher = null;
				this->popper = null;
				this->stack = new LockFreeStack(stackSize);
				this->disposed = false;
			}
			/*-------------------------------------------*/
			public: ~LockFreeStackTest()
			{
				if(!this->disposed)
				{
					if(pusher != null)
						delete pusher;
					if(popper != null) 
						delete popper;
					if(stack != null)
						delete stack;
					disposed = true;
				}
			}
			/*-------------------------------------------*/
			public: void Run()
			{
				pusher = new Thread(LockFreeStackTest::Wrapper_To_Call_Pusher, (void *)this);
				popper = new Thread(LockFreeStackTest::Wrapper_To_Call_Popper, (void *)this);

				pusher->Start();
				popper->Start();

				pusher->Join();
				popper->Join();
			}
			/*-------------------------------------------*/
			private: void *Pusher(void *ptr)
			{
				Int32 i = 0;
				Int32 j = 0;
				while(true)
				{
					if(!stack->Push(i))
					{
						i++;
						j++;
						if(j == 10000)
						{
							j = 0;
							Thread::Sleep(2000);
						}
					}
				}

				return ptr;
			}
			/*-------------------------------------------*/
			private: void *Popper(void *ptr)
			{
				Int32 temp;

				while(true)
				{
					if(stack->Pop(&temp))
					{
						printf("i: %d\n", temp); //Thread::Sleep(1);
					}
				}

				return ptr;
			}
			/*-------------------------------------------*/
			private: static void *Wrapper_To_Call_Pusher(void *pt2Object)
			{
				// explicitly cast to a pointer to LockFreeStackTest
				LockFreeStackTest *mySelf = (LockFreeStackTest *)pt2Object;

				// call member
				mySelf->Pusher(pt2Object);

				return pt2Object;
			}
			/*-------------------------------------------*/
			private: static void *Wrapper_To_Call_Popper(void *pt2Object)
			{
				// explicitly cast to a pointer to LockFreeStackTest
				LockFreeStackTest *mySelf = (LockFreeStackTest *)pt2Object;

				// call member
				mySelf->Popper(pt2Object);

				return pt2Object;
			}
			/*-------------------------------------------*/
			public: static int Parvicursor_main(int argc, char *argv[])
			{
				//while(true){
					LockFreeStackTest *test = new LockFreeStackTest(100000);
					test->Run();
					delete test;

				//Thread::Sleep(1);}

				/*UInt32 n = 10;
				LockFreeStack *stack = new LockFreeStack(n);
while (true){
				for(UInt32 i = 0 ; i < n/2 ; i++) 
					if(stack->Push(i)) {cout << "The stack is full" <<endl; break;}

				//stack->Print();

				Int32 temp;
				while(stack->Pop(&temp)) printf("i: %d\n", temp);

				for(UInt32 i = 0 ; i < n/2 + 1 ; i++) 
					if(stack->Push(i)) {cout << "The stack is full" <<endl; break;}


Thread::Sleep(1);}

				delete stack;*/
					
				getchar();

				return 0;
			}
			/*-------------------------------------------*/
		};
		//----------------------------------------------------
};
//**************************************************************************************************************//

#endif

