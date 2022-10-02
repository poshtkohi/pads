/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include  "Queue.h"

//**************************************************************************************************************//
namespace System
{
	namespace Collections
	{
		//----------------------------------------------------
		Queue::Queue()
		{
			sll = SinglyLinkedList();
			disposed = false;
		}
		//----------------------------------------------------
		Queue::~Queue()
		{
			if(!disposed)
			{
				Clear();
				disposed = true;
			}
		}
		//----------------------------------------------------
		void Queue::Enqueue(const Object *data)
		{
			if(disposed)
				throw ObjectDisposedException("Queue", "The Queue has been disposed");

			sll.Add(data);
		}
		//----------------------------------------------------
		const Object *Queue::Dequeue()
		{
			if(disposed)
				throw ObjectDisposedException("Queue", "The Queue has been disposed");

			return sll.RemoveBeginning();
		}
		//----------------------------------------------------
		const Object *Queue::Peek()
		{
			if(disposed)
				throw ObjectDisposedException("SinglyLinkedList", "The SinglyLinkedList has been disposed");

			return sll.Peek();
		}
		//----------------------------------------------------
		void Queue::Clear()
		{
			if(disposed)
				throw ObjectDisposedException("Queue", "The Queue has been disposed");

			sll.Clear();
		}
		//----------------------------------------------------
		Int32 Queue::get_Count()
		{
			if(disposed)
				throw ObjectDisposedException("Queue", "The Queue has been disposed");

			return sll.get_Count();
		}
		//----------------------------------------------------
		bool Queue::Contains(const Object *item)
		{
			if(disposed)
				throw ObjectDisposedException("Queue", "The Queue has been disposed");

			return sll.Contains(item);
		}
		//----------------------------------------------------
	};
};
//**************************************************************************************************************//
