/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Threading_Timer_h__
#define __System_Threading_Timer_h__

#include "../../System/BasicTypes/BasicTypes.h"
#include "../../System/Object/Object.h"
#include "../../System/ArgumentException/ArgumentNullException.h"
#include "../../System/ArgumentException/ArgumentOutOfRangeException.h"
#include "../../System.Threading/Thread/Thread.h"


#include <ctype.h>

using namespace System;


namespace System
{
	namespace Threading
	{
		// the Callback definition.
		typedef Object *(*Callback)(Object *);
		//--------------------------------------------
		class TimerCallback : public Object
		{
			private: Callback callback;
			public: TimerCallback(Callback &callback);
			public: TimerCallback();
			public: Callback &get_BaseCallback();
			//public: Callback& TimerCallback::operator=(const TimerCallback &rhs) ;
		};
		//--------------------------------------------
		class Timeout : public Object
		{
				// Summary:
				//     A constant used to specify an infinite waiting period. This field is constant.
			public: const static Int32 Infinite = -1;
		};
		//--------------------------------------------
		class Timer : public Object
		{
			// A TimerCallback delegate representing a method to be executed.
			private: TimerCallback callback;
		    // An object containing information to be used by the callback method, or null.
			private: Object *state;
			// The amount of time to delay before callback is invoked, in milliseconds. Specify Timeout::Infinite to prevent the timer from starting.
			private: Int32 dueTime;
			// The time interval between invocations of callback, in milliseconds. Specify Timeout::Infinite to disable periodic signaling.
			private: Int32 period;
			private: bool disposed;
			private: Thread worker;
			// Initializes a new instance of the Timer class, using a 32-bit signed integer to specify the time interval.
			public: Timer(const TimerCallback &callback, Object *state, Int32 dueTime, Int32 period);
			//public: Timer();
			public: ~Timer();
			// A static method that will be executed by the worker thread.
			private: protected: static void *Wrapper_To_Call_Worker(void* timer);
			public: void Finalize();
			// Releases all resources used by the current instance of Timer.
			public: void Dispose();
			// Changes the start time and the interval between method invocations for a timer,
			public: bool Change(Int32 dueTime, Int32 period);
		};
		//--------------------------------------------

	};
};

#endif
