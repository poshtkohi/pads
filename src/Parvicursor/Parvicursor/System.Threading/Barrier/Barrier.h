/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Threading_Barrier_h__
#define __System_Threading_Barrier_h__

#include "../../general.h"
#include "../../System/Object/Object.h"
#include "../../System/BasicTypes/BasicTypes.h"
#include "../../System/Exception/Exception.h"
#include "../../System/ObjectDisposedException/ObjectDisposedException.h"
#include "../../System.Threading/Thread/Thread.h"
#include "../../System.Threading/Mutex/Mutex.h"

using namespace System;
using namespace System::Threading;
//**************************************************************************************************************//
namespace System
{
	namespace Threading
	{
		// Enables multiple tasks to cooperatively work on an algorithm in parallel through multiple phases.
		class Barrier : public Object
		{
			//----------------------------------------------------
			private: Int32 ret;
			private: Int32 count;
			private: pthread_barrier_t barrier;
			private: bool disposed;
			//----------------------------------------------------
			// Initializes a new instance of the Barrier class. Count is the number of participating threads.
			public: Barrier(Int32 count);
			public: ~Barrier();
		    public: void Dispose();
			// Signals that a participant has reached the Barrier and waits for all other participants to reach the barrier as well.
			public: void SignalAndWait();
			//----------------------------------------------------
		};


	};
};
//**************************************************************************************************************//

#endif
