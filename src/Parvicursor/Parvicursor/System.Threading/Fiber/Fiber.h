/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Threading_Fiber_h__
#define __System_Threading_Fiber_h__


// 64kB stack
#define FIBER_STACK 1024*64

#if defined WIN32 || WIN64
#define _WIN32_WINNT 0x0600
#else
#include <malloc.h>
#include <ucontext.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>


// http://www.1024cores.net/home/lock-free-algorithms/tricks/fibers
/*#include <setjmp.h>
struct fiber_t
{
    ucontext_t  fib;
    jmp_buf     jmp;
};

struct fiber_ctx_t
{
    void(*      fnc)(void*);
    void*       ctx;
    jmp_buf*    cur;
    ucontext_t* prv;
};

static void fiber_start_fnc(void* p)
{
    fiber_ctx_t* ctx = (fiber_ctx_t*)p;
    void (*ufnc)(void*) = ctx->fnc;
    void* uctx = ctx->ctx;
    if (_setjmp(*ctx->cur) == 0)
    {
        ucontext_t tmp;
        swapcontext(&tmp, ctx->prv);
    }
    ufnc(uctx);
}

inline void create_fiber(fiber_t& fib, void(*ufnc)(void *), void* uctx, size_t stack_size)
{
    getcontext(&fib.fib);
    //size_t const stack_size = 64*1024;
    fib.fib.uc_stack.ss_sp = (::malloc)(stack_size);
    fib.fib.uc_stack.ss_size = stack_size;
    fib.fib.uc_link = 0;
    ucontext_t tmp;
    fiber_ctx_t ctx = {ufnc, uctx, &fib.jmp, &tmp};
    makecontext(&fib.fib, (void(*)())fiber_start_fnc, 1, &ctx);
    swapcontext(&tmp, &fib.fib);
}

inline void switch_to_fiber(fiber_t& fib, fiber_t& prv)
{
    if (_setjmp(prv.jmp) == 0)
        _longjmp(fib.jmp, 1);
}*/
#endif


#include "../../general.h"
#include "../../System/Object/Object.h"
#include "../../System/BasicTypes/BasicTypes.h"
#include "../../System/Exception/Exception.h"
#include "../../System/ObjectDisposedException/ObjectDisposedException.h"
#include "../../System/ArgumentException/ArgumentNullException.h"
#include "../../System.Threading/Thread/Thread.h"

using namespace System;
//**************************************************************************************************************//
namespace System
{
	namespace Threading
	{
		class Fiber : public Object
		{
			//----------------------------------------------------
#if defined WIN32 || WIN64
			private: LPVOID fiber;
#else
			private: ucontext_t fiber;
			//private: fiber_t fiber;
#endif
			private: bool disposed;
			//----------------------------------------------------
			// Allocates a fiber object, assigns it a stack, and sets up execution to begin at
			// the specified start address, typically the fiber function. This function does not
			// schedule the fiber.
			public: Fiber(Long stackSize, void *(*start_routine)(void *), void *arg);
			private: Fiber();
			public: ~Fiber();
			// This method saves the state information of the 'from' fiber and restores the state
			// of the specified 'to' fiber. You can call this method with the address of a fiber
			// created by a different thread. To do this, you must have the address returned to
			// the other thread when it called Fiber constructor and you must use proper synchronization.
			public: static void SwitchToFiber(const Fiber *from, const Fiber *to);
			public: static void SwitchToFiber(const Fiber &from, const Fiber &to);
			// Converts the current thread into a fiber. You must convert a thread into a fiber before you
			// can schedule other fibers. Only fibers can execute other fibers. If a thread needs to execute
			// a fiber, it must call ConvertThreadToFiber to create an area in which to save fiber state
			// information. The thread is now the current fiber.
/*#if defined WIN32 || WIN64
			public: static Fiber *ConvertThreadToFiber();
#else*/
			public: static Fiber *ConvertThreadToFiber();
//#endif
			// Converts the current fiber into a thread. This method releases the resources allocated by the
			// ConvertThreadToFiber method. After calling this method, you cannot call any of the fiber
			// functions from the thread.
			public: static void ConvertFiberToThread();
			//----------------------------------------------------
		};


	};
};
//**************************************************************************************************************//

#endif
