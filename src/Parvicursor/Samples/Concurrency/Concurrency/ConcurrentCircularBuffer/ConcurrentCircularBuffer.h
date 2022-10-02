#ifndef __Concurrency_ConcurrentCircularBuffer_h__
#define __Concurrency_ConcurrentCircularBuffer_h__


#include "../../../../Parvicursor/general.h"
#include "../../../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../../../Parvicursor/System/Random/Random.h"
#include "../../../../Parvicursor/System.Threading/Thread/Thread.h"
#include "../../../../Parvicursor/System.Threading/Mutex/Mutex.h"
#include "../../../../Parvicursor/System.Threading/ConditionVariable/ConditionVariable.h"

using namespace System;
using namespace System::Threading;
//**************************************************************************************************************//

#define BUFFER_SIZE 1024
#define ERROR_CHAR (-1)

namespace Concurrency
{
	//----------------------------------------------------
	// The ConcurrentCircularBuffer class.
	// The following code implements a producer/consumer queue.
	// The queue is represented as a bounded circular buffer,
	// and is protected by a mutex-> The code uses two condition variables:
	// one used by producers (bufferNotFull) and one used by consumers (bufferNotEmpty).
	class ConcurrentCircularBuffer : public Object
    {
		/*---------------------fields----------------*/
		private: bool disposed;
		private: Thread *producer_thread;
		private: Thread *consumer_thread;
		private: Int32 count;
		private: Mutex *mutex;
		private: ConditionVariable *bufferNotEmpty, *bufferNotFull;
		private: char circular_buffer[BUFFER_SIZE]; // A buffer with BUFFER_SIZE slots.
		private: Int32 start, end; // Integers to index circular_buffer.
		/*---------------------methods----------------*/
		// ConcurrentCircularBuffer Class constructor.
		public: ConcurrentCircularBuffer()
		{
			start = end = 0;
			mutex = new Mutex();
			bufferNotEmpty = new ConditionVariable(mutex);
			bufferNotFull = new ConditionVariable(mutex);
			disposed = false;
		}
		/*-------------------------------------------*/
		public: ~ConcurrentCircularBuffer()
		{
			if(!disposed)
			{
				if(producer_thread != null)
					delete producer_thread;
				if(consumer_thread != null)
					delete consumer_thread;
				if(mutex != null)
					delete mutex;
				if(bufferNotEmpty != null)
					delete bufferNotEmpty;
				if(bufferNotFull != null)
					delete bufferNotFull;

				disposed = true;
			}
		}
		/*--------------------------------------------*/
		public: void Run()
		{
			producer_thread = new Thread(ConcurrentCircularBuffer::Wrapper_To_Call_producer, (void *)this);
			consumer_thread = new Thread(ConcurrentCircularBuffer::Wrapper_To_Call_consumer, (void *)this);

			producer_thread->Start();
			consumer_thread->Start();

			producer_thread->Join();
			consumer_thread->Join();
		}
		/*-------------------------------------------*/
		private: void *producer(void *ptr)
		{
			static const char *str = "abcdefghiklmnopqrstvxyzABCDEFGHIKLMNOPQRSTVXYZ0123456789";
			static Random rnd = Random();

			while(true)
			{
				Thread::Sleep(1); // Simulates some work.

				mutex->Lock();
				{
					// Use modulo as a trick to wrap around the end of the buffer back to the beginning
					// Wait until the buffer is full
					while((end + 1) % BUFFER_SIZE == start)
					{
						// Buffer is full - sleep so consumers can get items.
						bufferNotFull->Wait();
					}

					char c = str[rnd.Next(0, 55)]; // strlen(str) - 1 = 56 – 1 = 55
					circular_buffer[end] = c;
					end = (end + 1) % BUFFER_SIZE;

				}
				mutex->Unlock();

				// If a consumer is waiting, wake it.
				bufferNotEmpty->Signal(); //
			}
			return ptr;
		}
		/*-------------------------------------------*/
		private: void *consumer(void *ptr)
		{
			while(true)
			{
				mutex->Lock();
				{
					// Wait until the buffer is empty
					while(end == start)
					{
						// Buffer is empty - sleep so producers can create items.
						bufferNotEmpty->Wait();
					}
					char temp = circular_buffer[start];
					start = (start + 1) % BUFFER_SIZE;
					//return(temp);
					printf("c: %c\n", temp);
				}
				mutex->Unlock();

				// If a producer is waiting, wake it.
				bufferNotFull->Signal();


				// Simulate processing of the item.
				Thread::Sleep(1);
			}
			return ptr;
		}
		/*-------------------------------------------*/
		private: static void *Wrapper_To_Call_producer(void *pt2Object)
		{
			// explicitly cast to a pointer to ConcurrentCircularBuffer
			ConcurrentCircularBuffer *mySelf = (ConcurrentCircularBuffer *)pt2Object;

			// call member
			mySelf->producer(pt2Object);

			return pt2Object;
		}
		/*-------------------------------------------*/
		private: static void *Wrapper_To_Call_consumer(void *pt2Object)
		{
			// explicitly cast to a pointer to ConcurrentCircularBuffer
			ConcurrentCircularBuffer *mySelf = (ConcurrentCircularBuffer *)pt2Object;

			// call member
			mySelf->consumer(pt2Object);

			return pt2Object;
		}
		/*-------------------------------------------*/
		public: static int Parvicursor_main(int argc, char *argv[])
		{

			ConcurrentCircularBuffer *cfc = new ConcurrentCircularBuffer();
			cfc->Run();
			delete cfc;

			return 0;
		}
		/*-------------------------------------------*/

	};
	//----------------------------------------------------
};
//**************************************************************************************************************//

#endif
