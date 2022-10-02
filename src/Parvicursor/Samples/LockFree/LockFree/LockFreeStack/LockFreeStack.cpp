//#include "stdafx.h"
#include "LockFreeStack.h"

//**************************************************************************************************************//

namespace LockFree
{
		//----------------------------------------------------
		LockFreeStack::LockFreeStack(UInt32 stackSize)
		{
			if(stackSize == 0)
				stackSize = 10;

			this->stackSize = stackSize;
			PreAllocate();
			disposed = false;
		}
		//----------------------------------------------------
		LockFreeStack::~LockFreeStack()
		{
			if(!disposed)
			{
				DeAllocate();
				disposed = true;
			}
		}
		//----------------------------------------------------
		void LockFreeStack::PreAllocate()
		{
			dummy = new Node();
			Node *node = null;
			Node *temp = null;
			head = null;

			for(Int32 i = 0 ; i < stackSize ; i++)
			{
				node = new Node();
				node->index = i;
				node->prev = head;
				head = node;

				if(temp != null)
					temp->next = node;
				temp = node;

				if(i == 0)
					first = head;
			}

			head->next = null;
			this->last = head;

			head = null; // Pointing to the beginning of the stack.
		}
		//----------------------------------------------------
		void LockFreeStack::DeAllocate()
		{
			if(!disposed)
			{
				Node volatile *current = last;
				Node volatile *temp;

				while(true)
				{
					temp = current;
					current = current->prev;

					if(current == null)
					{
						delete temp;
						break;
					}

					delete temp;

				}

				delete dummy;
				disposed = true;
			}
		}
		//----------------------------------------------------
		// Return true if the stack is full and nothing is pushed onto the stack.
		bool LockFreeStack::Push(Int32 data)
		{
			/*Node *node = new Node();
			node->data = data;*/

			/*if(head == null)
			{
				node->prev = null;
				head = node;
			}
			else
			{
				node->prev = head;
				head = node;
			}*/

			Node volatile *temp = null;
			Node volatile *current = dummy;

			while(!Parvicursor_AtomicCompareAndSwap(&current, &head, (Long)temp))
			{
				if(current == null) // The beginning of the stack was reached.
					temp = first;
				else
				{
					if(current->next == null)
						return true; // The stack is full.

					temp = current->next;
				}

				temp->data = data;
			}

			counter.Increment();

			return false; // The stack is not full.
		}
		//----------------------------------------------------
		// Return false if the stack is empty and nothing is popped from the stack.
		bool LockFreeStack::Pop(Out Int32 *data)
		{
			/*Node *current = head;

			if(current == null)
				return false;
			else
			{
				*data = current->data;
				head = current->prev;

				free(current);
				return true;
			}*/

			Node volatile *temp = null;
			Node volatile *current = dummy;

			while(!Parvicursor_AtomicCompareAndSwap(&current, &head, (Long)temp))
			{
				if(current == null)
					return false; // The stack is empty.

				*data = current->data;
				temp = current->prev;
			}

			counter.Decrement();

			return true; // The stack is not empty.
		}
		//----------------------------------------------------
		Long LockFreeStack::get_Count()
		{
			return counter.get_Count();
		}
		//----------------------------------------------------
		void LockFreeStack::Print()
		{
			/*Node *current = last;

			while(true)
			{
				if(current == null)
					break;

				cout << "data: " << current->data << endl;

				current = current->prev;
			}*/

			/*Node *current = first;

			while(true)
			{
				if(current == null)
					break;
				cout << "index: " << current->index << " data: " << current->data << " prev: " << current->prev << " next: " << current->next << endl;

				current = current->next;

			}*/
		}
	    //----------------------------------------------------
}
//**************************************************************************************************************//
