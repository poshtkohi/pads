/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "Barrier.h"

//**************************************************************************************************************//
namespace System
{
	namespace Threading
	{
		//----------------------------------------------------
		Barrier::Barrier(Int32 count)
		{
			if (count <= 0)
				count = 1;

			this->count = count;
			ret = pthread_barrier_init(&barrier, null, this->count);
			disposed = false;
		}
		//----------------------------------------------------
		Barrier::~Barrier()
		{
			Dispose();
		}
		//----------------------------------------------------
		void Barrier::Dispose()
		{
			if(!disposed)
			{
				ret = pthread_barrier_destroy(&barrier);
				disposed = true;
			}
		}
		//----------------------------------------------------
		void Barrier::SignalAndWait()
		{
			if(disposed)
				throw ObjectDisposedException("Barrier", "The Barrier has been closed");
			ret = pthread_barrier_wait(&barrier);
		}
		//----------------------------------------------------

	}
}
//**************************************************************************************************************//
