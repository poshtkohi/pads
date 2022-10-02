/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "Mutex.h"

//**************************************************************************************************************//
namespace System
{
	namespace Threading
	{
		//----------------------------------------------------
		Mutex::Mutex()
		{
			/* initialize a mutex to its default value */
			ret = pthread_mutex_init(&mutex, null); //printf("init: %d\n", ret);//
			disposed = false;
		}
		//----------------------------------------------------
		Mutex::~Mutex()
		{
			Close();
		}
		//----------------------------------------------------
		void Mutex::WaitOne()
		{
			if(disposed)
				throw ObjectDisposedException("Mutex", "The Mutex has been closed");

			ret = pthread_mutex_lock(&mutex);
		}
		//----------------------------------------------------
		/*void Mutex::Lock()
		{
			//if(disposed)
			//	throw ObjectDisposedException("Mutex", "The Mutex has been closed");

			ret = pthread_mutex_lock(&mutex);
		}
        //----------------------------------------------------
		void Mutex::Unlock()
		{
			//if(disposed)
			//	throw ObjectDisposedException("Mutex", "The Mutex has been closed");

			ret = pthread_mutex_unlock(&mutex);
		}*/
		//----------------------------------------------------
		void Mutex::TryLock()
		{
			if(disposed)
				throw ObjectDisposedException("Mutex", "The Mutex has been closed");

			ret = pthread_mutex_trylock(&mutex);
		}
		//----------------------------------------------------
		void Mutex::ReleaseMutex()
		{
			if(disposed)
				throw ObjectDisposedException("Mutex", "The Mutex has been closed");

			ret = pthread_mutex_unlock(&mutex);
		}
		//----------------------------------------------------
		void Mutex::Close()
		{
			if(!disposed)
			{
				/* destroy a mutex */
				ret = pthread_mutex_destroy(&mutex); //printf("destroy: %d\n", ret);//
				disposed = true;
			}
		}
		//----------------------------------------------------

	}
};
//**************************************************************************************************************//
