/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "SinglyLinkedList.h"

//**************************************************************************************************************//

namespace Parvicursor
{
	namespace Collections
	{
		//----------------------------------------------------
		SinglyLinkedList::SinglyLinkedList()
		{
			first = last = null;
			counter = 0;
			disposed = false;
		}
		//----------------------------------------------------
		SinglyLinkedList::~SinglyLinkedList()
		{
			if(!disposed)
			{
				Clear();
				disposed = true;
			}
		}
		//----------------------------------------------------
		void SinglyLinkedList::Add(const Object *data)
		{
			if(disposed)
				throw ObjectDisposedException("SinglyLinkedList", "The SinglyLinkedList has been disposed");

			Node *newPtr = new Node();
			if(newPtr != null)
			{
				newPtr->data = data;
				newPtr->next = null;

				if(first == null)
					first = last = newPtr;
				else
				{
					last->next = newPtr;
					last = newPtr;
				}
				counter++;
				return ;
			}
			else
			{
				delete newPtr;
				return ;
			}
		}
		//----------------------------------------------------
		void SinglyLinkedList::Remove(const Object *data)
		{
			if(disposed)
				throw ObjectDisposedException("SinglyLinkedList", "The SinglyLinkedList has been disposed");

			Node *curPtr = first;
			Node *nextPtr = first;
			while(nextPtr != null)
			{
				if(data == nextPtr->data)
				{
					if(nextPtr == first)
					{
						first = first->next;
						delete nextPtr;
						counter--;
						break;
					}
					else
					{
						if(nextPtr == last)
							last = curPtr;

						curPtr->next = nextPtr->next;
						delete nextPtr;
						counter--;
						break;
					}
				}
				else
				{
					curPtr = nextPtr;
					nextPtr = nextPtr->next;
				}
			}
		}
		//----------------------------------------------------
		const Object *SinglyLinkedList::RemoveBeginning()
		{
			if(disposed)
				throw ObjectDisposedException("SinglyLinkedList", "The SinglyLinkedList has been disposed");

			if(counter == 0)
				return null;

			Node *obsoleteNode = first;
			const Object *data = obsoleteNode->data;
			first = first->next; // point past deleted node
			delete obsoleteNode;
			counter--;
			return data;
		}
		//----------------------------------------------------
		const Object *SinglyLinkedList::Peek()
		{
			if(disposed)
				throw ObjectDisposedException("SinglyLinkedList", "The SinglyLinkedList has been disposed");

			if(counter == 0)
				return null;

			return first;
		}
		//----------------------------------------------------
		void SinglyLinkedList::Clear()
		{
			if(disposed)
				throw ObjectDisposedException("SinglyLinkedList", "The SinglyLinkedList has been disposed");

			if(counter == 0)
				return ;

			Node *curPtr = first;
			Node *temp ;
			while(curPtr != null)
			{
				temp = curPtr;
				curPtr = curPtr->next;
				delete temp;
			}
			first = last = null;
			counter = 0;
		}
		//----------------------------------------------------
		Int32 SinglyLinkedList::get_Count()
		{
			if(disposed)
				throw ObjectDisposedException("SinglyLinkedList", "The SinglyLinkedList has been disposed");

			return counter;
		}
		//----------------------------------------------------
		bool SinglyLinkedList::Contains(const Object *item)
		{
			if(disposed)
				throw ObjectDisposedException("SinglyLinkedList", "The SinglyLinkedList has been disposed");

			if(counter == 0)
				return false;

			Node *curPtr = first;
			while(curPtr != null)
			{
				if(curPtr->data == item)
					return true;
				curPtr = curPtr->next;
			}
			return false;
		}
		//----------------------------------------------------
	}
};
//**************************************************************************************************************//
