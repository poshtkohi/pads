// Concurrency.cpp : Defines the entry point for the console application.
//

//------------------------------------------------------------------------------
#include "Concurrency/ConcurrentCounterTest/ConcurrentCounterTest.h"
#include "Concurrency/ConcurrentCircularBuffer/ConcurrentCircularBuffer.h"
#include "Concurrency/ThreadPool/ThreadPoolExample1.h"
#include "Concurrency/ThreadPool/ThreadPoolExample2.h"
#include "Concurrency/ThreadPool/ThreadPoolExample3.h"
#include "Concurrency/ThreadPool/ThreadPoolExample4.h"
#include "Concurrency/Fiber/FiberExample1.h"
#include "Concurrency/Fiber/FiberExample2.h"
//------------------------------------------------------------------------------
using namespace Concurrency;
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	// return ConcurrentCounterTest::Parvicursor_main(argc, argv);
	// return ConcurrentCircularBuffer::Parvicursor_main(argc, argv);
	// return ThreadPoolExample1::Parvicursor_main(argc, argv);
	// return ThreadPoolExample2::Parvicursor_main(argc, argv);
	// return ThreadPoolExample3::Parvicursor_main(argc, argv);
    // return ThreadPoolExample4::Parvicursor_main(argc, argv);
	// return FiberExample1::Parvicursor_main(argc, argv);
	return FiberExample2::Parvicursor_main(argc, argv);
}
//------------------------------------------------------------------------------
