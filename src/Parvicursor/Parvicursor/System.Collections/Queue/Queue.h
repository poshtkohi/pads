/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Collections_Queue_h__
#define __System_Collections_Queue_h__

#include "../../System/Object/Object.h"
#include "../../System/ArgumentException/ArgumentOutOfRangeException.h"
#include "../../System/InvalidOperationException/InvalidOperationException.h"
#include "../../Parvicursor/Collections/SinglyLinkedList.h"

using namespace System;
using namespace Parvicursor::Collections;
//**************************************************************************************************************//

namespace System
{
	namespace Collections
	{
		//----------------------------------------------------
		class Queue : public Object
		{
			/*---------------------fields----------------*/
			private: SinglyLinkedList sll;
			private: bool disposed; // For destructor use.
			/*---------------------methods----------------*/
			// Queue Class constructor.
			public: Queue();
			// Queue Class destructor.
			public: ~Queue();
			// Gets the number of elements available in the Queue.
			public: Int32 get_Count();
			// Adds the data to the end of the Queue.
			public: void Enqueue(const Object *data);
			// Removes and returns the data at the beginning of the Queue. If the Queue is empty, this method returns null.
			public: const Object *Dequeue();
			// Determines whether an element is in the Queue.
			public: bool Contains(const Object *item);
			// Clears all data items witin the Queue.
			public: void Clear();
			// Returns the object at the beginning of the Queue without removing it.
			public: const Object *Peek();
		};
		//----------------------------------------------------

	};
};
//**************************************************************************************************************//

#endif
