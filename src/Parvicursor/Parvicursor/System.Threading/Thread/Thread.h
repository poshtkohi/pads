/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Threading_Thread_h__
#define __System_Threading_Thread_h__

#include "../../general.h"
#include "../../StaticFunctions/StaticFunctions.h"
#include "../../System/Object/Object.h"
#include "../../System/Exception/Exception.h"
#include "../../System/String/String.h"
//**************************************************************************************************************//
#if defined WIN32 || WIN64
#include "pthread.h"
#if defined Yield()
#undef Yield()
#endif
//#include "semaphore.h"

#define WIN
#else
#define LINUX
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <semaphore.h>

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#endif

#include <stdio.h>
static pthread_mutex_t _xDFS_server_mutex_ = PTHREAD_MUTEX_INITIALIZER;


#if !defined WIN32 || WIN64
#include <signal.h>
#include <time.h>
#include <stdio.h>
#include <errno.h>
#endif
//pthread_mutex_init(&_xDFS_server_mutex_, NULL);


//**************************************************************************************************************//
namespace System
{
	namespace Threading
	{

		enum ThreadState
		{
			Unknown = 255,
			Running = 0,
			StopRequested = 1,
			SuspendRequested = 2,
			//Background = 4,
			Unstarted = 8,
			Stopped = 16,
			//WaitSleepJoin = 32,
			Suspended = 64,
			AbortRequested = 128,
			Aborted = 256,
		};

		class Thread : public Object
		{
			private: int  iret;
			public: pthread_t thread;
			private: bool started;
			private: bool detached;
			private: bool joined;
			private: void *(*start)(void *);
			private: void *arg;
			private: pthread_attr_t attr;
			private: ThreadState state;

#if !defined WIN32 || WIN64
			private: bool volatile _is_called_suspend_init_routine;
#endif

			//----------------------------------------------------
			// Initializes a new instance of the Thread class.
			// start: desired function pointer to execute.
			// arg: input arguments of the function pointer.
			public: Thread(void *(*start)(void *), void *arg);
			public: Thread();
			public: ~Thread();
			// public: Thread(void *(*start)(void *));
			// Causes the operating system to change the state of the current instance to Running state.
			public: void Start();
			// Gets a value indicating the execution status of the current thread.
			public: bool get_IsAlive();
			// Raises a thread abort request in the thread on which it is invoked, to begin the process of terminating the thread. Calling this method usually terminates the thread.
			public: void Abort();
			public: void Abort2();
			// Blocks the calling thread until a thread terminates.
			public: void Join();
			// Set the thread state to detached. If you know in advance that a thread will never need to join with another thread, consider creating it in a detached state.
			public: void SetDetached();
			// Resumes a thread that has been suspended.
			public: void Resume();
			// Either suspends the thread, or if the thread is already suspended, has no effect.
			public: void Suspend();
			public: ThreadState get_ThreadState();
			// Suspends the current thread for a specified time.
			public: static void Sleep(Int32 millisecondsTimeout);
			public: static void Yield();
			public: static void *Thread_Proc(void *ptr);
			//----------------------------------------------------
		};


	};
};
//**************************************************************************************************************//

#endif
