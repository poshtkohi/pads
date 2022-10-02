#ifndef __DiskIO_ConcurrentFileCopy_h__
#define __DiskIO_ConcurrentFileCopy_h__


#include "../../../../Parvicursor/general.h"
#include "../../../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../../../Parvicursor/System.IO/FileStream/FileStream.h"
#include "../../../../Parvicursor/System.Threading/Thread/Thread.h"
#include "../../../../Parvicursor/System.Threading/Mutex/Mutex.h"
#include "../../../../Parvicursor/System.Threading/ConditionVariable/ConditionVariable.h"
#include "../../../../Parvicursor/System.Threading/Barrier/Barrier.h"

#include <string.h>

using namespace System;
using namespace System::IO;
using namespace System::Threading;
//**************************************************************************************************************//
namespace DiskIO
{
	//----------------------------------------------------
	// The ConcurrentFileCopy class.
	// The following code implements a producer/consumer queue.
	// The queue is represented as a bounded circular buffer_producer,
	// and is protected by a mutex-> The code uses two condition variables:
	// one used by producers (bufferNotFull) and one used by consumers (bufferNotEmpty).
	class ConcurrentFileCopy : public Object
    {
		/*---------------------fields----------------*/
		private: struct Cell
		{
			Int32 size;
			char *buff;
		};
		private: bool disposed;
		private: Thread *producer_thread;
		private: Thread *consumer_thread;
		private: Int32 count;
		private: Mutex *mutex;
		private: ConditionVariable *bufferNotEmpty, *bufferNotFull;
		//a buffer_producer with BUFFER_SIZE slots
		private: Cell *circular_buffer;
		//integers to index circular_buffer
		private: Int32 start;
		private: Int32 end;
		private: char *buffer_producer;
		private: char *buffer_consumer;
		private: FileStream *fsRead;
		private: FileStream *fsWrite;
		private: Int32 bufferSize;
		private: Int32 cells;
		private: bool StopRequested;
		private: bool EndOfFileReached;
		private: Barrier *barrier;
		/*---------------------methods----------------*/
		// ConcurrentFileCopy Class constructor.
		public: ConcurrentFileCopy(FileStream *fsRead, FileStream *fsWrite, Int32 bufferSize, Int32 cells, Barrier *barrier)
		{
			if(bufferSize <= 0)
				bufferSize = 64*1024;
			if(cells <= 0)
				bufferSize = 10;
			this->fsRead = fsRead;
			this->fsWrite = fsWrite;
			this->barrier = barrier;
			this->bufferSize = bufferSize;
			this->cells = cells;
			start = 0;
			end = 0;
			buffer_producer = new char[bufferSize];
			buffer_consumer = new char[bufferSize];
			circular_buffer = new Cell[cells];
			for(Int32 i = 0 ; i < cells ; i++)
			{
				circular_buffer[i].buff = new char[bufferSize];
				circular_buffer[i].size = -1;
			}
			mutex = new Mutex();
			bufferNotEmpty = new ConditionVariable(mutex);
			bufferNotFull = new ConditionVariable(mutex);
			StopRequested = false;
			EndOfFileReached = false;
			disposed = false;
		}
		/*-------------------------------------------*/
		public: ~ConcurrentFileCopy()
		{
			if(!disposed)
			{
				if(producer_thread != null)
					delete producer_thread;
				if(consumer_thread != null)
					delete consumer_thread;
				if(buffer_producer != null)
					delete buffer_producer;
				if(buffer_consumer != null)
					delete buffer_consumer;
				if(mutex != null)
					delete mutex;
				if(bufferNotEmpty != null)
					delete bufferNotEmpty;
				if(bufferNotFull != null)
					delete bufferNotFull;
				for (Int32 i = 0 ; i < cells; i++)
					if(circular_buffer[i].buff != null)
						delete circular_buffer[i].buff;
				if(circular_buffer != null)
					delete circular_buffer;

				disposed = true;
			}
		}
		/*--------------------------------------------*/
		public: void Run()
		{
			producer_thread = new Thread(ConcurrentFileCopy::Wrapper_To_Call_producer, (void *)this);
			consumer_thread = new Thread(ConcurrentFileCopy::Wrapper_To_Call_consumer, (void *)this);

			producer_thread->Start();
			consumer_thread->Start();

			//producer_thread->Join();
			//consumer_thread->Join();
			producer_thread->SetDetached();
			consumer_thread->SetDetached();
		}
		/*-------------------------------------------*/
		private: void *producer(void *ptr)
		{
			Int32 read;
			while(true)
			{
				//Thread::Sleep(1); // Simulates Some work.
				try
				{
					read = fsRead->Read(buffer_producer, 0, bufferSize);
					if(read <= 0)
					{
						mutex->Lock();
						{
							EndOfFileReached = true;
							//bufferNotEmpty->Wait();
							// Wait until the consumer thread completes its execution.
							while(end != start && !StopRequested)
								bufferNotEmpty->Wait();
						}
						mutex->Unlock();
						break;
					}
				}
				catch(IOException &e)
				{
					printf("3. Exception Message: %s\n", e.get_Message().get_BaseStream());
					mutex->Lock();
					StopRequested = true;
					mutex->Unlock();
					break;
				}
				catch(Exception &e)
				{
					printf("4. Exception Message: %s\n", e.get_Message().get_BaseStream());
					mutex->Lock();
					StopRequested = true;
					mutex->Unlock();
					break;
				}

				mutex->Lock();
				{
					// Use modulo as a trick to wrap around the end of the buffer_producer back to the beginning
					// Wait until the buffer_producer is full
					while((end + 1) % cells == start && !StopRequested)
					{
						// Buffer is full - sleep so consumers can get items.
						bufferNotFull->Wait();
					}

					if(StopRequested)
					{
						mutex->Unlock();
						break;
					}

					circular_buffer[end].size = read;
					memmove(circular_buffer[end].buff, buffer_producer, read);
					end = (end + 1) % cells;

				}
				mutex->Unlock();

				// If a consumer is waiting, wake it.
				bufferNotEmpty->Signal(); //
			}

			// for (StopRequested==true)
			bufferNotFull->Signal();
			bufferNotEmpty->Signal();

			barrier->SignalAndWait(); // Synchronizes with other threads before the completion of its execution.

			return ptr;
		}
		/*-------------------------------------------*/
		private: void *consumer(void *ptr)
		{
			Int32 write_bytes;

			while(true)
			{
				mutex->Lock();
				{
					if(end == start && EndOfFileReached && !StopRequested)
					{
						mutex->Unlock();
						break;
					}
					// Wait until the buffer_producer is empty
					while(end == start && !StopRequested && !EndOfFileReached)
					{
						// Buffer is empty - sleep so producers can create items.
						bufferNotEmpty->Wait();
					}

					if(StopRequested)
					{
						mutex->Unlock();
						break;
					}

					write_bytes = circular_buffer[start].size;
					if(write_bytes != -1)
						memmove(buffer_consumer, circular_buffer[start].buff, write_bytes);
					circular_buffer[start].size = -1;
					start = (start + 1) % cells;
				}
				mutex->Unlock();

				// If a producer is waiting, wake it.
				bufferNotFull->Signal();


				// Simulates processing of the item.
				//Thread::Sleep(1);
				if(write_bytes <= 0)
					continue;

				try
				{
					 fsWrite->Write(buffer_consumer, 0, write_bytes);
					 printf("\r...");
				}
				catch(IOException &e)
				{
					printf("5. Exception Message: %s\n", e.get_Message().get_BaseStream());
					mutex->Lock();
					StopRequested = true;
					mutex->Unlock();
					break;
				}
				catch(Exception &e)
				{
					printf("6. Exception Message: %s\n", e.get_Message().get_BaseStream());
					mutex->Lock();
					StopRequested = true;
					mutex->Unlock();
					break;
				}
			}

			// for (StopRequested==true)
			bufferNotFull->Signal();
			bufferNotEmpty->Signal();

			barrier->SignalAndWait(); // Synchronizes with other threads before the completion of its execution.

			printf("...\r");

			return ptr;
		}
		/*-------------------------------------------*/
		private: static void *Wrapper_To_Call_producer(void *pt2Object)
		{
			// explicitly cast to a pointer to ConcurrentFileCopy
			ConcurrentFileCopy *mySelf = (ConcurrentFileCopy *)pt2Object;

			// call member
			mySelf->producer(pt2Object);

			return pt2Object;
		}
		/*-------------------------------------------*/
		private: static void *Wrapper_To_Call_consumer(void *pt2Object)
		{
			// explicitly cast to a pointer to ConcurrentFileCopy
			ConcurrentFileCopy *mySelf = (ConcurrentFileCopy *)pt2Object;

			// call member
			mySelf->consumer(pt2Object);

			return pt2Object;
		}
		/*-------------------------------------------*/
		public: static int Parvicursor_main(int argc, char *argv[])
		{

			Int32 bufferSize = 256*1024;
			Int32 cells = 10;

			FileStream *fsRead = null;
			FileStream *fsWrite = null;
			Barrier *barrier = new Barrier(3); // main thread + producer and consumer threads.

			String readFilename = "c:/test.exe";  //to change
			String writeFilename = "c:/test1.exe";   //to change
			//String readFilename = "F:/dl/New Downloads/Microsoft_Office_2010_Plus_Professional_www.AsanDownload.com.iso";
			//String writeFilename = "c:/Microsoft_Office_2010_Plus_Professional_www.AsanDownload.com.iso";

			printf("Copying %s to %s\n", readFilename.get_BaseStream(), writeFilename.get_BaseStream());

			try
			{
				fsRead = new FileStream(readFilename, System::IO::Open, System::IO::Read, 8*1024);
				fsWrite = new FileStream(writeFilename, System::IO::OpenOrCreate, System::IO::Write, 8*1024);
			}
			catch(Exception &e)
			{
				printf("1. Exception Message: %s\n", e.get_Message().get_BaseStream());
				delete barrier;
				if(fsRead != null)
					delete fsRead;
				if(fsWrite != null)
					delete fsWrite;
				return 0;
			}
			catch(...)
			{
				printf("2. Unknown Exception\n");
				delete barrier;
				if(fsRead != null)
					delete fsRead;
				if(fsWrite != null)
					delete fsWrite;
				return 0;
			}

			ConcurrentFileCopy cfc = ConcurrentFileCopy(fsRead, fsWrite, bufferSize, cells, barrier);
			cfc.Run();
			barrier->SignalAndWait(); // Waits until the completion of other threads' execution.

			delete barrier;
			delete fsRead;
			delete fsWrite;

			return 0;
		}
		/*-------------------------------------------*/

	};
	//----------------------------------------------------
};
//**************************************************************************************************************//

#endif

