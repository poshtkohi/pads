/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "ConditionVariable.h"

//**************************************************************************************************************//
namespace System
{
	namespace Threading
	{
		//----------------------------------------------------
		ConditionVariable::ConditionVariable(Mutex *mutex)
		{
			if(mutex == null)
				throw ArgumentNullException("mutex");
			this->mutex = mutex;
			/* Initialize condition variable object */
			pthread_cond_init(&cv, null); //printf("init: %d\n", ret);//
			disposed = false;
		}
		//----------------------------------------------------
		ConditionVariable::~ConditionVariable()
		{
			Close();
		}
		//----------------------------------------------------
		void ConditionVariable::Close()
		{
			if(!disposed)
			{
				// destroy a condition variable*
				ret = pthread_cond_destroy(&cv); //printf("destroy: %d\n", ret);//
				disposed = true;
			}
		}
		//----------------------------------------------------
		/*void ConditionVariable::Wait()
		{
			//if(disposed)
			//	throw ObjectDisposedException("ConditionVariable", "The ConditionVariable has been closed");

			ret = pthread_cond_wait(&cv, mutex->get_Handle());
		}
        //----------------------------------------------------
		void ConditionVariable::Signal()
		{
			//if(disposed)
			//	throw ObjectDisposedException("ConditionVariable", "The ConditionVariable has been closed");

			ret = pthread_cond_signal(&cv);
		}
		//----------------------------------------------------
		void ConditionVariable::Broadcast()
		{
			//if(disposed)
			//	throw ObjectDisposedException("ConditionVariable", "The ConditionVariable has been closed");

			ret = pthread_cond_broadcast(&cv);
		}*/
		//----------------------------------------------------
		void ConditionVariable::TimedWait(Long seconds, Long nanoseconds)
		{
			if(disposed)
				throw ObjectDisposedException("ConditionVariable", "The ConditionVariable has been closed");

			time.tv_sec = seconds;
			time.tv_nsec = nanoseconds;

			ret = pthread_cond_timedwait(&cv, mutex->get_Handle(), &time);
		}
		//----------------------------------------------------

	}
}
//**************************************************************************************************************//
