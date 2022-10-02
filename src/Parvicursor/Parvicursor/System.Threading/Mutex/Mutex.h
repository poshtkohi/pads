/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Threading_Mutex_h__
#define __System_Threading_Mutex_h__

#include "../../general.h"
#include "../../System/Object/Object.h"
#include "../../System/BasicTypes/BasicTypes.h"
#include "../../System/Exception/Exception.h"
#include "../../System/ObjectDisposedException/ObjectDisposedException.h"
#include "../../System.Threading/Thread/Thread.h"

using namespace System;
//**************************************************************************************************************//
namespace System
{
	namespace Threading
	{
		class Mutex : public Object
		{
			//----------------------------------------------------
			private: Int32 ret;
			private: pthread_mutex_t mutex;
			private: bool disposed;
			//----------------------------------------------------
			public: Mutex();
			public: ~Mutex();
			// Lock the Mutex.
			public: void WaitOne();
			// Lock the Mutex.
			public: inline void Lock()
			{
                ret = pthread_mutex_lock(&mutex);
			}
            // Unlocks the Mutex once.
			public: inline void Unlock()
			{
                ret = pthread_mutex_unlock(&mutex);
			}
			// Lock the Mutex.
			public: void TryLock();
			// Unlocks (releases) the Mutex once.
			public: void ReleaseMutex();
			// Releases all resources held by the current Mutex.
			public: void Close();
			public: inline pthread_mutex_t *get_Handle()
			{
				return &mutex;
			}
			//----------------------------------------------------
		};


	};
};
//**************************************************************************************************************//

#endif
