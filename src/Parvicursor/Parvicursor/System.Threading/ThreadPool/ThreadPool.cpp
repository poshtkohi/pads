/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "ThreadPool.h"

//**************************************************************************************************************//

namespace System
{
	namespace Threading
	{
		//----------------------------------------------------
		ThreadPool::ThreadPool(UInt32 numThreads, UInt32 queueMaxSize)
		{
			if(queueMaxSize == 0)
				queueMaxSize = 1;
			this->queueMaxSize = queueMaxSize;

			if(numThreads == 0)
				queueMaxSize = 1;
			this->numThreads = numThreads;

			q = Queue();
			mutex = new Mutex();
			poolNotEmpty = new ConditionVariable(mutex);
			poolNotFull = new ConditionVariable(mutex);
			waitcv = new ConditionVariable(mutex);
			workers = new Thread*[this->numThreads];

			for(UInt32 i = 0 ; i < this->numThreads ; i++)
			{
				workers[i] = new Thread(ThreadPool::Wrapper_To_Call_Worker, (void *)this);
				workers[i]->Start();
			}

			for(UInt32 i = 0 ; i < this->numThreads ; i++)
				workers[i]->SetDetached();

			disposed = false;
		}
		//----------------------------------------------------
		ThreadPool::~ThreadPool()
		{
			if(!disposed)
			{
				if(workers != null)
				{
					for(UInt32 i = 0 ; i < numThreads ; i++)
						if(workers[i] != null)
							delete workers[i];

					delete workers;
				}

				// delete queue items
				Object *item;
				while((item = (Object *)q.Dequeue()) != null)
					delete item;

				if(mutex != null)
					delete mutex;
				if(poolNotEmpty != null)
					delete poolNotEmpty;
				if(poolNotFull != null)
					delete poolNotFull;
				if(waitcv != null)
					delete waitcv;

				disposed = true;
			}
		}
		//----------------------------------------------------
		Int32 ThreadPool::get_Count()
		{
			if(disposed)
				throw ObjectDisposedException("ThreadPool", "The ThreadPool has been disposed");

			Int32 count;
			mutex->Lock();
			count = q.get_Count();
			mutex->Unlock();
			return count;
		}
		//----------------------------------------------------
		bool ThreadPool::QueueUserWorkItem(WaitCallback callBack, Object *state)
		{
			if(disposed)
				throw ObjectDisposedException("ThreadPool", "The ThreadPool has been disposed");
			//cout << "h2" << endl;//
			mutex->Lock();
			{
				while(q.get_Count() == queueMaxSize)
					poolNotFull->Wait();
				TaskInfo *task = new TaskInfo();
				task->callback = callBack;
				task->state = state;
				if(q.get_Count() == 0)
				{
					q.Enqueue(task);
					poolNotEmpty->Signal();
				}
				else
					q.Enqueue(task);
			}
			mutex->Unlock();
			return true;
		}
		//----------------------------------------------------
		void *ThreadPool::Worker(void *ptr)
		{
			TaskInfo *task; //cout << "h1" << endl;//

			while(true)
			{
				mutex->Lock();
				{
					while(q.get_Count() == 0)
						poolNotEmpty->Wait();
					task = (TaskInfo *)q.Dequeue();
					if(q.get_Count() == queueMaxSize - 1)
						poolNotFull->Broadcast();
				}
				mutex->Unlock();

				// signal must be placed here.
				task->callback(task->state); // Here, we must handle likely thrown exceptions.
				waitcv->Broadcast();
				delete task;
			}
			return ptr;
		}
		//----------------------------------------------------
		void ThreadPool::WaitOnTaskQueue()
		{
			if(disposed)
				throw ObjectDisposedException("ThreadPool", "The ThreadPool has been disposed");

			mutex->Lock();
			{
				while (q.get_Count() != 0)
					waitcv->Wait();
			}
			mutex->Unlock();
		}
		//----------------------------------------------------
		void *ThreadPool::Wrapper_To_Call_Worker(void *pt2Object)
		{
			// explicitly cast to a pointer to ThreadPool
			ThreadPool *mySelf = (ThreadPool *)pt2Object;

			// call member
			mySelf->Worker(pt2Object);

			return pt2Object;
		}
		//----------------------------------------------------
	}
};
//**************************************************************************************************************//
