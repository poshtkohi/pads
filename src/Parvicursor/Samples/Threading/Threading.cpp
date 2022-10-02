// Threading.cpp : Defines the entry point for the console application.
//

//------------------------------------------------------------------------------
#include "../../Parvicursor/System.Threading/Thread/Thread.h"
//------------------------------------------------------------------------------
using namespace System;
using namespace System::Threading;
//------------------------------------------------------------------------------
class TestThread : public Object
{
	private: Int32 threadID;
	public: TestThread(Int32 threadID)
	{
		this->threadID = threadID;
	}
	public: void Display(const char *message)
	{
		Int32 i = 0;
		while(true)
		{
			printf("I'm thread %d. Message: %s. i: %d\n", threadID, message, i);
			Thread::Sleep(1); // Suspends the current thread for 1ms.
			i++;
		}
		return ;
	}
	public: static void *Wrapper_To_Call_Display(void *pt2Object)
	{
		// explicitly cast to a pointer to TestThread
		TestThread *mySelf = (TestThread *)pt2Object;

		// call member
		mySelf->Display("Hello World");

		return pt2Object;
	}
};
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	TestThread test1 = TestThread(0);
	TestThread test2 = TestThread(1);
	Thread t1 = Thread(test1.Wrapper_To_Call_Display, (void *)&test1);
	Thread t2 = Thread(test2.Wrapper_To_Call_Display, (void *)&test2);
	t1.Start();
	t2.Start();
	t1.SetDetached();
	t2.SetDetached();
	Thread::Sleep(1);
	while(true)
	{
		t1.Suspend();
		t2.Resume();
		Thread::Sleep(2000); // Suspends the main() thread for 2 seconds.
		t1.Resume();
		t2.Suspend();
		Thread::Sleep(2000);
	}
	return 0;
}
//------------------------------------------------------------------------------

