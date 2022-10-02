#ifndef __Concurrency_FiberExample2_h__
#define __Concurrency_FiberExample2_h__

#include "../../../../Parvicursor/general.h"
#include "../../../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../../../Parvicursor/System/Object/Object.h"
#include "../../../../Parvicursor/System.Threading/Fiber/Fiber.h"
#include "../../../../Parvicursor/System.Threading/Thread/Thread.h"


// for getch() function.
#if defined WIN32 || WIN64
#include <conio.h>
#else
/*
    In Unix-like operating systems, compile this sample
    with -lncurses
*/
#include <curses.h>
#define printf printw
#endif

#include <ctype.h>

using namespace System;
using namespace System::Threading;
//**************************************************************************************************************//

namespace Concurrency
{
	//----------------------------------------------------
	class FiberExample2 : public Object
	{
		/*---------------------fields----------------*/
		private: struct FiberInfo
		{
			Int32 fiberID;
			Fiber *mainFib;
			Fiber **fibs;
		};
		/*---------------------methods----------------*/
		private: static void *fiber_proc(void *arg)
		{
			FiberInfo *info = (FiberInfo *)arg;

			Int32 c;
			while(true)
			{
				printf("\n------\nCurrent state: %d\nEnter a character: ", info->fiberID);
				c = getch();
				printf("%c", c);
				if(islower(c))
				{
					Fiber::SwitchToFiber(info->fibs[info->fiberID], info->fibs[0]);
					continue;
				}
				if(isupper(c))
				{
					Fiber::SwitchToFiber(info->fibs[info->fiberID], info->fibs[1]);
					continue;
				}
				if(isdigit(c))
				{
					Fiber::SwitchToFiber(info->fibs[info->fiberID], info->fibs[2]);
					continue;
				}
				if(c == '@')
				{
					Fiber::SwitchToFiber(info->fibs[info->fiberID], info->mainFib);
					//continue;
				}
				else
				{
					printf("\nInvalid character");
					//continue;
				}
			}

			return arg;
		}
		/*-------------------------------------------*/
		public: static int Parvicursor_main(int argc, char *argv[])
		{
// for getch() function.
// See here: NCURSES Programming HOWTO
// http://tldp.org/HOWTO/NCURSES-Programming-HOWTO/init.html
#if !defined WIN32 || WIN64
            ::initscr();			/* Start curses mode 		*/
            ::raw();				/* Line buffering disabled	*/
            ::keypad(stdscr, true);		/* We get F1, F2 etc..		*/
            ::noecho();			/* Don't echo() while we do getch */
            ::scrollok(stdscr, true); /* Enable scrolling*/
#endif

			const Int32 FibNum = 3;
			FiberInfo infos[FibNum];
			Fiber **fibs = new Fiber*[FibNum];
			Fiber *mainFib = Fiber::ConvertThreadToFiber();
			for(Int32 i = 0 ; i < FibNum ; i++)
			{
				infos[i].fiberID = i;
				infos[i].mainFib = mainFib;
				infos[i].fibs = fibs;
				fibs[i] = new Fiber(4*1024, fiber_proc, (void *)&infos[i]);
			}

			Fiber::SwitchToFiber(mainFib, fibs[0]);
			Fiber::ConvertFiberToThread();

			printf("\nThe program is to terminate ...\n");

			for(Int32 i = 0 ; i < FibNum ; i++)
				delete fibs[i];
			delete fibs;
			delete mainFib;

// for getch() function.
#if !defined WIN32 || WIN64
            ::refresh();			/* Print it on to the real screen */
			::endwin();
#endif

			return 0;
		}
	};
	//----------------------------------------------------
};
//**************************************************************************************************************//

#endif
