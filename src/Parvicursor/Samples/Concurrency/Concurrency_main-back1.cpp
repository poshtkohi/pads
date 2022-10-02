// Concurrency.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//------------------------------------------------------------------------------
#include "Concurrency/ConcurrentCounterTest/ConcurrentCounterTest.h"
#include "Concurrency/ConcurrentCircularBuffer/ConcurrentCircularBuffer.h"
#include "Concurrency/ThreadPool/SinglyLinkedList.h"
#include "Concurrency/ThreadPool/Queue.h"
#include "Concurrency/ThreadPool/ThreadPool.h"
#include "../../Parvicursor/System/Environment/Environment.h"
//------------------------------------------------------------------------------
using namespace System;
using namespace System::Threading;
using namespace Concurrency;
//------------------------------------------------------------------------------
#define __MethodNum__ 1000

Int32 counter = 0;
Mutex *mutex;
ConditionVariable *cv;

struct MethodInfo : public Object
{
	Int32 methodID;
	char *message;
};
void Print(Object *state)
{
	MethodInfo *info = (MethodInfo *)state;
	printf("I'm method %d. Method Message: %s\n", info->methodID, info->message);

	/*mutex->Lock();
	counter--;
	cv->Broadcast();
	mutex->Unlock();*/

	mutex->Lock();
	counter++;
	if(counter == __MethodNum__)
		cv->Broadcast();
	mutex->Unlock();
}


struct FibInfo : public Object
{
	Int64 n;
	Int64 out;
	Int32 *counter_addr;
	Mutex *mutex;
	ConditionVariable *cv;
};
ThreadPool *poolFib = new ThreadPool(Environment::get_ProcessorCount(), 1000);

void fib(Object *state)
{
	FibInfo *info = (FibInfo *)state;

	if(info->n < 2)
	{
		info->out = 1; return;
	}
	else
	{
		/*if(info->mutex != null)
		{
			info->mutex->Lock();
			cout << "n: " << info->n << endl ;
			(*info->counter_addr)--;
			info->cv->Broadcast();
			info->mutex->Unlock();
		}*/

		//Int32 _counter = 0;
		Int32 _counter = 2;
		FibInfo x, y;
		x.n = info->n - 1;
		y.n = info->n - 2;
		x.out = 0;
		y.out = 0;
		x.counter_addr = &_counter;
		y.counter_addr = &_counter;

		/*x.mutex = new Mutex();
		x.cv = new ConditionVariable(x.mutex);
		y.mutex = x.mutex;
		y.cv = x.cv;*/

		//poolFib->QueueUserWorkItem(fib, &x); //fib(&x);
		//poolFib->QueueUserWorkItem(fib, &y);//fib(&y);

		fib(&x);
		fib(&y);

		//synch2
		/*x.mutex->Lock();
		x.cv->Wait();
		x.mutex->Unlock();*/


		/*x.mutex->Lock();
		while(_counter != 0)
			x.cv->Wait();
		x.mutex->Unlock();*/

		/*delete x.mutex;
		delete x.cv;*/

		info->out = x.out + y.out;
		return;
	}

	//synch1
	/*if(info->mutex != null)
	{
		info->mutex->Lock();
		(*info->counter_addr)++;
		if((*info->counter_addr) == 2)
			info->cv->Broadcast();
		//cout << (*info->counter_addr) << endl;
		info->mutex->Unlock();
	}*/
}

Int64 Fibonacci(Int64 n)
{
	if(n==0 || n==1)
		return 1;
	else
		return Fibonacci(n-1) + Fibonacci(n-2);
}

//End function Fibonacci

int main(int argc, char* argv[])
{
	/*Int32 cc = 0;
	Int32 *counter_addr = null;
	counter_addr = &cc;
	(*counter_addr)++;
	cout << cc << endl;
	return 0;*/

	Int32 n = 10;
	FibInfo info;
	info.n = n;
	info.mutex = null;
	info.cv = null;
	info.out = 0;
	fib(&info);
	cout << "result: " << info.out << endl;
	cout << Fibonacci(n) << endl;
	return 0;
	//ConcurrentCounterTest::Parvicursor_main(argc, argv);
	//ConcurrentCircularBuffer::Parvicursor_main(argc, argv);

	//counter = __MethodNum__;
	mutex = new Mutex();
	cv = new ConditionVariable(mutex);
	ThreadPool pool = ThreadPool(Environment::get_ProcessorCount()*4, 100);

	MethodInfo methods[__MethodNum__];
	for(Int32 i = 0 ; i < __MethodNum__ ; i++)
	{
		methods[i].methodID = i;
		methods[i].message = "Hello World";
		pool.QueueUserWorkItem(Print, &methods[i]);
	}

	//Thread::Sleep(1000000);

	/*mutex.Lock();
	while(counter != 0)
		cv->Wait();
	mutex.Unlock();*/

	mutex->Lock();
	cv->Wait();
	mutex->Unlock();

	delete mutex;
	delete cv;

	return 0;
}
//------------------------------------------------------------------------------


/*while(true){
Int32 x = 10, y = 20;

SinglyLinkedList sll = SinglyLinkedList();

sll.Add(&x);
sll.Add(&y);

Int32 *ret = (Int32 *)sll.RemoveBeginning();
if(ret != null)
printf("ret: %d, count: %d\n", *ret, sll.get_Count());

Queue q = Queue();

q.Enqueue(&x);
q.Enqueue(&y);

cout << q.Contains(&y) << endl;

Int32 *ret = (Int32 *)q.Dequeue();
if(ret != null)
printf("ret: %d, count: %d\n", *ret, q.get_Count());

Thread::Sleep(1);}*/