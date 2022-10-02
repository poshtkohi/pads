#ifndef __Concurrency_ThreadPoolExample2_h__
#define __Concurrency_ThreadPoolExample2_h__

#include "../../../../Parvicursor/general.h"
#include "../../../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../../../Parvicursor/System/Object/Object.h"
#include "../../../../Parvicursor/System/Environment/Environment.h"
#include "../../../../Parvicursor/System/Random/Random.h"
#include "../../../../Parvicursor/Parvicursor/Profiler/ResourceProfiler.h"
#include "../../../../Parvicursor/System/ObjectDisposedException/ObjectDisposedException.h"
#include "../../../../Parvicursor/System.Threading/ThreadPool/ThreadPool.h"
#include "../../../../Parvicursor/System.Threading/Mutex/Mutex.h"
#include "../../../../Parvicursor/System.Threading/ConditionVariable/ConditionVariable.h"

using namespace System;
using namespace System::Threading;

//_________________________________
Parvicursor_RESOURCE_PROFILER_INITIALIZE()
//_________________________________
//**************************************************************************************************************//

namespace Concurrency
{
	//----------------------------------------------------
	class ThreadPoolExample2 : public Object
	{
		/*---------------------fields----------------*/
		private: struct MethodInfo : public Object
		{
			Float *A, *B, *C;
			Int32 rows;
			Int32 columns;
			Int32 slice;
			Int32 methodNum;
			Int32 *counter_addr;
			Mutex *mutex;
			ConditionVariable *cv;
		};
		/*---------------------methods----------------*/
		private: static void MatrixFillRandom(Float *matrix, Int32 rows, Int32 columns)
		{
			Random rnd = Random();
			register Int32 i, j;
			for(i = 0 ; i < rows; i++)
				for(j = 0 ; j < columns ; j++)
					matrix[i*rows + j] = (Float)(rnd.Next() * rnd.Next(-1, 1) * 0.528);
		}
		/*-------------------------------------------*/
		private: static void MatrixPrint(const String &name, Float *matrix, Int32 rows, Int32 columns)
		{
			register Int32 i, j;
			for(i = 0 ; i < rows ; i++)
			{
				for(j = 0 ; j < columns ; j++)
					printf("%s[%d][%d]=%f ", name.get_BaseStream(), i + 1, j + 1, matrix[i*rows + j]);
				printf("\n");
			}
			printf("\n");
		}
		/*-------------------------------------------*/
		private: static void MatrixMultiplyCallback(Object *state)
		{
			MethodInfo *info = (MethodInfo *)state;

			Float *A, *B ,*C;
			A = info->A;
			B = info->B;
			C = info->C;
			Int32 rows = info->rows;
			Int32 columns = info->columns;
			Int32 methodNum = info->methodNum;
			Int32 s = (Int32)info->slice;
			Int32 from = (s * rows)/methodNum;    // note that this 'slicing' works fine
			Int32 to = ((s + 1) * rows)/methodNum;  // even if rows(n) is not divisible by methodNum
			register Int32 i , j, k;
			Float temp;

			printf("computing slice %d (from row %d to %d)\n", s, from, to - 1);

			for(i = from ; i < to ; i++) 
			{
				for(j = 0 ; j < rows ; j++) 
				{
					temp = 0.0;
					for(k = 0 ; k < rows ; k++)
						temp += A[i*rows + k]*B[k*rows + j];
					C[i*rows + j] = temp;
				}
			}

			printf("finished slice %d\n", s);

			info->mutex->Lock();
			(*info->counter_addr)++; // Increment the counter value by its address
			if(*info->counter_addr == methodNum)
				info->cv->Signal(); // Singnal the main thread of all methods's completion.
			info->mutex->Unlock();

			return;
		}
		/*-------------------------------------------*/
		public: static int Parvicursor_main(int argc, char *argv[])
		{
			const Int32 methodNum = 10;
			// Intilizes the thread pool instance.
			ThreadPool pool = ThreadPool(methodNum/*Environment::get_ProcessorCount()*/, 10000);

			// This example only works on square matrices, i.e., the n*n matrices.
			const Int32 rows = 2000, columns = 2000;
			MethodInfo methods[methodNum];
			Float *A = new Float[rows*columns];
			Float *B = new Float[rows*columns];
			Float *C = new Float[rows*columns];
			MatrixFillRandom(A, rows, columns);
			MatrixFillRandom(B, rows, columns);
			Mutex *mutex = new Mutex();
			ConditionVariable *cv = new ConditionVariable(mutex);
			Int32 counter = 0; // The main thread waits on the value of this shared counter.

			//_________________________________
			Parvicursor_RESOURCE_PROFILER_BEGIN()
			//_________________________________

			for(register Int32 i = 0 ; i < methodNum ; i++)
			{
				methods[i].A = A;
				methods[i].B = B;
				methods[i].C = C;
				methods[i].slice = i;
				methods[i].rows = rows;
				methods[i].columns = columns;
				methods[i].methodNum = methodNum;
				methods[i].counter_addr = &counter;
				methods[i].mutex = mutex;
				methods[i].cv = cv;
				pool.QueueUserWorkItem(MatrixMultiplyCallback, &methods[i]);
			}

			// Waits on counter until all methods complete.
			mutex->Lock();
			cv->Wait();
			mutex->Unlock();

			//_________________________________
			Parvicursor_RESOURCE_PROFILER_END()
			//_________________________________

			//_________________________________
			Parvicursor_RESOURCE_PROFILER_FINALIZE()
			//_________________________________

			//MatrixPrint("A", A, rows, columns);
			//MatrixPrint("B", B, rows, columns);
			//MatrixPrint("C", C, rows, columns);

			delete A;
			delete B;
			delete C;
			delete mutex;
			delete cv;

			return 0;
		}
		/*-------------------------------------------*/

	};
	//----------------------------------------------------
};
//**************************************************************************************************************//

#endif
