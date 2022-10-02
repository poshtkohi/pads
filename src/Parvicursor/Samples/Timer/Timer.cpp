// SignalTest.cpp : Defines the entry point for the console application.
//

//---------------------------------------
#include "../../Parvicursor/general.h"
#include "../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../Parvicursor/System.Threading/Timer/Timer.h"
#include "../../Parvicursor/System/String/String.h"
//---------------------------------------
using namespace System;
using namespace System::Threading;
//---------------------------------------
static Int32 counter = 0; // Our counter.
// Our timer callback.
Object *test(Object *obj)
{
	printf("%s, %d\n", ((String *)obj)->get_BaseStream(), counter);
	counter++;
	return obj;
}
//---------------------------------------
int main(int argc, char *argv[])
{
	//getch();
	String s = "Alireza Poshtkohi";
	Callback callback = test;
	TimerCallback timerDelegate(callback);
	Timer timer(timerDelegate, &s, 0, 1000);

	while(true)
		Thread::Sleep(10); // Sleeps for 10ms.

	return 0;
}
//---------------------------------------

