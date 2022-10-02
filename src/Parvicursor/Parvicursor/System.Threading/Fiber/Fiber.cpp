/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "Fiber.h"

//**************************************************************************************************************//
namespace System
{
	namespace Threading
	{
		//----------------------------------------------------
		Fiber::Fiber(Long stackSize, void *(*start_routine)(void *), void *arg)
		{
			if(stackSize <= 0)
				stackSize = FIBER_STACK;

#if defined WIN32 || WIN64
			if((fiber = ::CreateFiber(stackSize, (LPFIBER_START_ROUTINE)start_routine, arg)) == null)
			{
				disposed = true;
				throw Exception(ErrorString(GetLastError()));
			}
#else
            //create_fiber(fiber, (void (*)(void *))start_routine, arg, stackSize);
			// Get the current execution context
			if(::getcontext(&fiber) == -1)
			{
				disposed = true;
				throw Exception((const char *)_sys_errlist[errno]);
			}

			// Modify the context to a new stack
			fiber.uc_link = NULL;
			//memmove(fiber->uc_stack.ss_sp + FIBER_STACK - 2 * sizeof(void *), &func, sizeof(void *));
			//memmove(fiber->uc_stack.ss_sp, &func, sizeof(int));
			//memmove(fiber->uc_stack.ss_sp + FIBER_STACK - sizeof(void *), args, sizeof(void *));
			if((fiber.uc_stack.ss_sp = ::malloc(stackSize)) == NULL) // here we can allocate stach via mamp for better performance.
			{
				disposed = true;
				throw Exception((const char *)_sys_errlist[errno]);
			}
			fiber.uc_stack.ss_size = stackSize;
			fiber.uc_stack.ss_flags = 0;

			// Create the new context
			// printf( "Creating fiber fiber\n" );
			::makecontext(&fiber, (void (*)(void))start_routine, 1, arg);
			/* On architectures where int and pointer types are the same size (e.g., x86-32,
			where both types are 32 bits), you may be able to get away with passing
			pointers as arguments to makecontext() following argc.  However, doing this is
			not guaranteed to be portable, is undefined according to the standards, and
			won't work on architectures where pointers are larger than ints.
			Nevertheless, starting with version 2.8, glibc makes some changes to
			makecontext(), to permit this on some 64-bit architectures (e.g., x86-64). */
			// On x64 architectures for arg, and pass two integers (which when combined represent the
			// address of the arg) and later pack them in a 64-bit integer variable, and use a scondary function pinter to warp this.
#endif

			disposed = false;
		}
		//----------------------------------------------------
		Fiber::Fiber()
		{
			disposed = true;
		}
		//----------------------------------------------------
		Fiber::~Fiber()
		{
			if(!disposed)
			{
#if defined WIN32 || WIN64
				::DeleteFiber(fiber);
#else
				::free(fiber.uc_stack.ss_sp);
				//::free(fiber.fib.uc_stack.ss_sp);
#endif
				disposed = true;
			}
		}
		//----------------------------------------------------
		void Fiber::SwitchToFiber(const Fiber *from, const Fiber *to)
		{
			/*if(from == null)
				throw ArgumentNullException("from", "from is null");

			if(to == null)
				throw ArgumentNullException("to", "to is null");*/

#if defined WIN32 || WIN64
			::SwitchToFiber(to->fiber);
#else
            //ucontext_t fiber = from.fiber;
			if(::swapcontext((ucontext_t *)&from->fiber, (ucontext_t*)&to->fiber) == -1)
			//if(::swapcontext(&fiber, &fiber) == -1)
				throw Exception((const char *)_sys_errlist[errno]);

            //switch_to_fiber((fiber_t &)to->fiber, (fiber_t &)from->fiber);
#endif
		}
		//----------------------------------------------------
		void Fiber::SwitchToFiber(const Fiber &from, const Fiber &to)
		{
			SwitchToFiber(&from, &to);
		}
		//----------------------------------------------------
/*#if defined WIN32 || WIN64
		Fiber Fiber::ConvertThreadToFiber()
#else
		Fiber &Fiber::ConvertThreadToFiber()
#endif*/
		Fiber *Fiber::ConvertThreadToFiber()
		{
			Fiber *fib = new Fiber();
#if defined WIN32 || WIN64
			fib->fiber = ::ConvertThreadToFiber(null);
			if(fib->fiber == null)
			{
				fib->disposed = true;
				throw Exception(ErrorString(GetLastError()));
			}
#else
			if(::getcontext(&fib->fiber) == -1)
			{
				fib->disposed = true;
				throw Exception((const char *)_sys_errlist[errno]);
			}
#endif
			return fib;
		}
		//----------------------------------------------------
		void Fiber::ConvertFiberToThread()
		{
#if defined WIN32 || WIN64
			::ConvertFiberToThread();
#else
			return;
#endif
		}
		//----------------------------------------------------

	}
};
//**************************************************************************************************************//
