#ifndef __Concurrency_FiberExample1_h__
#define __Concurrency_FiberExample1_h__

#include "../../../../Parvicursor/general.h"
#include "../../../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../../../Parvicursor/System/Object/Object.h"
#include "../../../../Parvicursor/System.Threading/Fiber/Fiber.h"
#include "../../../../Parvicursor/System.Threading/Thread/Thread.h"

using namespace System;
using namespace System::Threading;
//**************************************************************************************************************//

static Fiber *fib;
static Fiber *fib1;
static Fiber *fib2;
namespace Concurrency
{
	//----------------------------------------------------
	class FiberExample1 : public Object
	{
		//public: static Fiber *fib1;
		//public: static Fiber *fib2;
		/*---------------------methods----------------*/
		private: static void *fiber_proc(void *arg)
		{
			Int32 fiberID =  *(Int32 *)arg;

			printf("Hello World!\n");

			if(fiberID == 1)
			{
				while(true)
				{
					printf("I'm fiber %d\n", fiberID);
					Fiber::SwitchToFiber(fib1, fib2);
					Thread::Sleep(1000);
				}
			}
			if(fiberID == 2)
			{
				while(true)
				{
					printf("I'm fiber %d\n", fiberID);
					Fiber::SwitchToFiber(fib2, fib1);
					Thread::Sleep(1000);
				}
			}

			return arg;
		}
		/*-------------------------------------------*/
		public: static int Parvicursor_main(int argc, char *argv[])
		{
			fib1 = new Fiber(0, fiber_proc, (void *)1);
			fib2 = new Fiber(0, fiber_proc, (void *)2);

			fib = Fiber::ConvertThreadToFiber();
			Fiber::SwitchToFiber(fib, fib1);
			Fiber::ConvertFiberToThread();

			delete fib1;
			delete fib2;
			delete fib;

			return 0;
		}
	};
	//----------------------------------------------------
};
//**************************************************************************************************************//

#endif
