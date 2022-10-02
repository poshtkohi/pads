/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include  "Timer.h"

//**************************************************************************************************************//
namespace System
{
	namespace Threading
	{
		//----------------------------------------------------
		TimerCallback::TimerCallback(Callback &callback)
		{
			if(callback == null)
				throw ArgumentNullException("callback");

			this->callback = callback;
		}
		//----------------------------------------------------
		TimerCallback::TimerCallback()
		{
		}
		//----------------------------------------------------
		Callback &TimerCallback::get_BaseCallback()
		{
			return this->callback;
		}
		//----------------------------------------------------
		/*Callback& TimerCallback::operator=(const TimerCallback &rhs)
		{
			return this->get_BaseCallback();
		}*/
		//----------------------------------------------------
		Timer::Timer(const TimerCallback &callback, Object *state, Int32 dueTime, Int32 period)
		{
			if(dueTime == Timeout::Infinite)
				goto Continue;

			if(dueTime < 0 || period < 0)
				throw ArgumentOutOfRangeException("dueTime or period", "The dueTime or period parameter is negative");

	Continue:

			this->callback = callback;
			this->state = state;
			this->dueTime = dueTime;
			this->period = period;
			this->disposed = false;

			worker = Thread(this->Wrapper_To_Call_Worker, (void *)this);
			worker.Start();
			worker.SetDetached();
			//worker.Join();
		}
		//----------------------------------------------------
		Timer::~Timer()
		{
			this->Dispose();
		}
		//----------------------------------------------------
		bool Timer::Change(Int32 dueTime, Int32 period)
		{
			if(!disposed)
			{
				if(dueTime == Timeout::Infinite)
					goto Continue;

				if(dueTime < 0 || period < 0)
					throw ArgumentOutOfRangeException("dueTime or period", "The dueTime or period parameter is negative");

		Continue:

				this->dueTime = dueTime;
				this->period = period;

				return true;
			}
			else
				return false;
		}
		//----------------------------------------------------
		void Timer::Finalize()
		{
			this->Dispose();
		}
		//----------------------------------------------------
		void Timer::Dispose()
		{
			if(!disposed)
			{
				this->disposed = true;
				this->worker.Abort();
			}
		}
		//----------------------------------------------------
		void *Timer::Wrapper_To_Call_Worker(void *timer)
		{
				// explicitly cast to a pointer to Timer
				Timer *_timer = (Timer*) timer;
				//Int32 elapsed = 0;

				if(_timer->dueTime != Timeout::Infinite)
					Thread::Sleep(_timer->dueTime);

				if(_timer->dueTime == Timeout::Infinite)
					return timer;

				while(true)
				{
					if(_timer->disposed)
						return timer;

					_timer->callback.get_BaseCallback()(_timer->state);
					//cout << "\n" << _timer->period << "\n" << endl;
					Thread::Sleep(_timer->period);
					//elapsed += 2;
				}

				return timer;
		}
		//----------------------------------------------------
	};
};
//**************************************************************************************************************//
