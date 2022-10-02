#ifndef __LockFree_LockFreeCounter_h__
#define __LockFree_LockFreeCounter_h__


#include "../../../../Parvicursor/general.h"
#include "../../../../Parvicursor/System/Object/Object.h"
#include "../../../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../../../Parvicursor/Atomic/atomic.h"

using namespace System;


// GCC versions earlier than 4.8 has seroius problems
// with Parvicursor atomic operations.So we make use of GCC
// atomic builtins.
/*#ifdef __GNUC__
#  include <features.h>
#  if __GNUC_PREREQ(4,8)
//      If  gcc_version >= 4.8
#define ___NormalAtomic___
//#  elif __GNUC_PREREQ(3,2)
//       If gcc_version >= 3.2
#  else
//       Else
#  endif
#else
//    If not gcc
#define ___NormalAtomic___
#endif*/

#ifdef WIN32 || WIN64
#define ___NormalAtomic___
#else
#include <features.h>
#endif
//**************************************************************************************************************//

namespace LockFree
{
		//----------------------------------------------------
		// A simple, straightforward, lock-free counter.
		class LockFreeCounter : public Object
        {
			/*---------------------fields----------------*/
			private: Long volatile count;
			/*---------------------methods----------------*/
			// LockFreeCounter Class constructor.
			public: LockFreeCounter()
			{
				count = 0;
			}
			/*--------------------------------------------*/
			// LockFreeCounter Class constructor.
			public: LockFreeCounter(Long intitialCount)
			{
				count = intitialCount;
			}
			//public: ~LockFreeCounter(){}
			/*--------------------------------------------*/
			// Gets the current value of the count variable.
			public: inline Long get_Count()
			{
#if defined ___NormalAtomic___
				volatile Long temp = -10;

				Parvicursor_AtomicCompareAndSwap(&temp, &count, null);

				return temp;
#else
				Long ret = __sync_fetch_and_sub(&count, 0);
				return ret;
#endif
			}
			/*--------------------------------------------*/
			// Increments the count variable.
			public: inline void Increment()
			{

#if defined ___NormalAtomic___
                Long volatile temp1 = -10;
				Long temp2 = -10;

				while(!Parvicursor_AtomicCompareAndSwap(&temp1, &count, temp2))
				{
					temp2 = temp1 + 1;
				}
#else
				__sync_fetch_and_add(&count, 1);
#endif
			}
			/*--------------------------------------------*/
			// Increments the count variable non-atomically.
			public: inline void NonAtomicIncrement()
			{
				count++;
			}
			/*--------------------------------------------*/
			// Decrements the count variable non-atomically.
			public: inline void NonAtomicDecrement()
			{
				count--;
			}
			/*--------------------------------------------*/
			// Decrements the count variable non-atomically.
			public: inline Long NonAtomicGetCount()
			{
				return count;
			}
			/*--------------------------------------------*/
			// Decrements the count variable.
			public: inline void Decrement()
			{
#if defined ___NormalAtomic___
                Long volatile temp1 = -10;
				Long temp2 = -10;

				while(!Parvicursor_AtomicCompareAndSwap(&temp1, &count, temp2))
				{
					temp2 = temp1 - 1;
				}
#else
                __sync_fetch_and_sub(&count, 1);
#endif
			}
			/*--------------------------------------------*/
			// Gets the current value of the count variable and increments it.
			public: inline Long get_Count_And_Increment()
			{

#if defined ___NormalAtomic___
                Long volatile temp1 = -10;
				Long temp2 = -10;

				while(!Parvicursor_AtomicCompareAndSwap(&temp1, &count, temp2))
				{
					temp2 = temp1 + 1;
				}

				return temp1;
#else
                Long ret = __sync_fetch_and_add(&count, 1);
				return ret;
#endif
			}
			/*--------------------------------------------*/
			// First increments the count variable and then gets the current value of the count variable.
			public: inline Long Increment_And_get_Count()
			{
#if defined ___NormalAtomic___
                Long volatile temp1 = -10;
				Long temp2 = -10;

				while(!Parvicursor_AtomicCompareAndSwap(&temp1, &count, temp2))
				{
					temp2 = temp1 + 1;
				}

				return temp2;
#else
                Long ret = __sync_add_and_fetch(&count, 1);
				return ret;
#endif
			}
			/*--------------------------------------------*/
			// Gets the current value of the count variable and decrements it.
			public: inline Long get_Count_And_Decrement()
			{
#if defined ___NormalAtomic___
                Long volatile temp1 = -10;
				Long temp2 = -10;

				while(!Parvicursor_AtomicCompareAndSwap(&temp1, &count, temp2))
				{
					temp2 = temp1 - 1;
				}

				return temp1;
#else
                Long ret = __sync_fetch_and_sub(&count, 1);
				return ret;
#endif
			}
			/*--------------------------------------------*/
			// First decrements the count variable and then gets the current value of the count variable.
			public: inline Long Decrement_And_get_Count()
			{
#if defined ___NormalAtomic___
                Long volatile temp1 = -10;
				Long temp2 = -10;

				while(!Parvicursor_AtomicCompareAndSwap(&temp1, &count, temp2))
				{
					temp2 = temp1 - 1;
				}

				return temp2;
#else
				Long ret = __sync_sub_and_fetch(&count, 1);
				return ret;
#endif
			}
            /*--------------------------------------------*/
			// Returns the current value of val and increments the value pointed by val.
			public: inline static Long AtomicIncrement(Long volatile *val)
			{
#if defined ___NormalAtomic___
				Long volatile temp1 = -10;
				Long temp2 = -10;

				while(!Parvicursor_AtomicCompareAndSwap(&temp1, val, temp2))
				{
					temp2 = temp1 + 1;
				}
				return temp1;
#else
                __sync_add_and_fetch(val, 1);
#endif
			}
            /*--------------------------------------------*/
			// Returns the current value of val and decrements the value pointed by val.
			public: inline static Long AtomicDecrement(Long volatile *val)
			{
#if defined ___NormalAtomic___
				Long volatile temp1 = -10;
				Long temp2 = -10;

				while(!Parvicursor_AtomicCompareAndSwap(&temp1, val, temp2))
				{
					temp2 = temp1 - 1;
				}
				return temp1;
#else
                __sync_sub_and_fetch(val, 1);
#endif
			}
			/*--------------------------------------------*/

		};
		//----------------------------------------------------
};
//**************************************************************************************************************//

#endif

