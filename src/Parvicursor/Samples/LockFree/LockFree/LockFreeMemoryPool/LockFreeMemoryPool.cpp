//#include "stdafx.h"
#include "LockFreeMemoryPool.h"

//**************************************************************************************************************//

namespace LockFree
{
		//----------------------------------------------------
		LockFreeMemoryPool::LockFreeMemoryPool(UInt64 slotSize, UInt64 poolSlots)
		{
			if(poolSlots == 0)
				poolSlots = 1024;

			if(slotSize == 0)
				slotSize = sizeof(Int64);

			this->slotSize = slotSize;
			this->poolSlots = poolSlots;

			buffer = null;
			dummy = _dummy = null;

			PreAllocate();
			disposed = false;
		}
		//----------------------------------------------------
		LockFreeMemoryPool::~LockFreeMemoryPool()
		{
			if(!disposed)
			{
				DeAllocate();
				disposed = true;
			}
		}
		//----------------------------------------------------
		void LockFreeMemoryPool::PreAllocate()
		{
			buffer = (void volatile *)::malloc(poolSlots * (slotSize + sizeof(struct Header)));

			if(buffer == null)
			{
				printf("malloc failed\n");//
				exit(-1);
			}

			Header volatile *node = null;
			Header volatile *temp = null;
			head = null;
			dummy = (struct Header volatile*)::malloc(sizeof(struct Header));
			_dummy = dummy;

			for(UInt64 i = 0 ; i < poolSlots ; i++)
			{
				struct Header volatile *node = (struct Header volatile *)( (char *)buffer + i*(slotSize + sizeof(struct Header)) );
				//node->index = i;
				head = node;

				if(temp != null)
					temp->next = node;

				temp = node;

				if(i == 0)
					first = (Header *)head;

				//printf("i: %d\n", i);//
			}

			head->next = null;
			last = (Header *)head;
			head = first;//
		}
		//----------------------------------------------------
		void LockFreeMemoryPool::DeAllocate()
		{
			if(!disposed)
			{
				if(buffer != null)
				{
					::free((void *)buffer);
					buffer = null;
				}

				if(_dummy != null)
				{
					::free((void *)_dummy);
					_dummy = null;
				}

				disposed = true;
			}
		}
		//----------------------------------------------------
		void LockFreeMemoryPool::Free(void *memslot)
		{
			if(memslot == null)
				return;

			Header *header = (struct Header *)( (char *)memslot - sizeof(struct Header) );

			if(header == null)
				return;

			/* None thread-safe Free() implementation. */
			/*Header *temp1 = null;
			Header *temp2 = null;

			temp1 = header->next;

			temp2 = (Header *)TaggedPointer_RecoverPointer(temp1);
			header->next = temp2;
			head = header;*/

			/* Lock-free Free() implementation. */
			Header volatile *temp1 = dummy;
			Header volatile *temp2 = null;


			/* Safely clear the head->next to indicate that this slot was freed. */
			while(!Parvicursor_AtomicCompareAndSwap(&temp1, &header->next, (Long)temp2))
			{
				temp2 = (Header volatile *)get_unmarked_reference((void *)temp1); // Clear the LSB.
			}

			/* Atomically update the head to the header (head = header;) */
			temp1 = dummy;
			temp2 = header; //
			while(!Parvicursor_AtomicCompareAndSwap(&temp1, &head, (Long)temp2))
			{
				//temp2 = header;
			}

			counter.Decrement(); //
		}
		//----------------------------------------------------
		/************************************************************************/
		/* If there are not any free slots within the memory pool, then null is returned. */
		/************************************************************************/
		void *LockFreeMemoryPool::Malloc()
		{
			//printf("hello3\n");
			if(counter.get_Count() == poolSlots)
				return null;

			//printf("hello2\n");
			/* None thread-safe Malloc() implementation. */
			/*Header *current = head;
			Header *temp = null;
			Header *entrance = null;
			bool found = false;
			bool fullySearched = false;
			bool lsb;

			entrance = head;

			while(true)
			{
				if(current == null)
					goto here2;
here1:
				temp = current->next;

				lsb = (bool)TaggedPointer_RecoverData(temp);

				if(!lsb) // free slot is here
				{
					temp = (Header *)TaggedPointer_Smuggle(temp, true);
					current->next = temp;
					found = true;
					break;
				}
				else
				{
					temp = (Header *)TaggedPointer_RecoverPointer(temp);
				}

				current = temp;
				head = current;//
			}


			if(found)
				return (void *)( (char *)current + sizeof(struct Header) );
			else
			{
				head = null;
				return null;
			}

here2:
			if(fullySearched)
			{
				head = null;
				return null;
			}

			if(entrance <= last && entrance > first)
			{
				current = first;
				head = first;
				fullySearched = true;
				goto here1;
			}
			else
			{
				head = null;
				return null;
			}*/

			/* Lock-free Malloc() implementation. */
			/*Header *temp1 = dummy;
			Header *temp2 = null;
			Header *temp = null;
			Header *current = null;
			Header *entrance = null;
			bool found = false;
			bool fullySearched = false;
			bool lsb;

			Parvicursor_AtomicCompareAndSwap((Long *)&temp1, (Long *)&head, (Long)temp2);
			current = temp1;
			entrance = temp1;

			while(true)
			{
				if(current == null)
					goto here2;
			here1:
				while(!Parvicursor_AtomicCompareAndSwap((Long *)&temp1, (Long *)&current->next, (Long)temp2))
				{
					lsb = (bool)TaggedPointer_RecoverData(temp1);

					if(!lsb) // free slot is here
					{
						temp = (Header *)TaggedPointer_Smuggle(temp1, true);  // Set the LSB.
						temp2 = temp;
						found = true;
					}
					else
						break;
				}

				if(found)
					break;

				temp = (Header *)TaggedPointer_RecoverPointer(temp1);
				current = temp;
			}

			if(found)
			{
				temp1 = dummy;
				while(!Parvicursor_AtomicCompareAndSwap((Long *)&temp1, (Long *)&head, (Long)temp2))
				{
					temp2 = current;
				}
				return (void *)( (char *)current + sizeof(struct Header) );
			}
			else
				return null;

		here2:
			if(fullySearched)
				return null;

			if(entrance <= last && entrance > first)
			{
				current = first;
				fullySearched = true;
				goto here1;
			}
			else
				return null;*/

			/* Lock-free Malloc() implementation. */
			Header volatile *temp1 = dummy;
			Header volatile *temp2 = null;
			Header volatile *current;// = null;
			Header *entrance;// = null;
			bool found = false;
			bool fullySearched = false;
			bool lsb = false;

			Parvicursor_AtomicCompareAndSwap(&temp1, &head, (Long)temp2);
			current = temp1;
			entrance = (Header *)temp1;

			while(true)
			{
				if(current == null)
				{
					if(fullySearched)
						return null;

					if(entrance <= last && entrance > first)
					{
						current = first;
						fullySearched = true;
					}
					else
						return null;
				}

				while(!Parvicursor_AtomicCompareAndSwap(&temp1, &current->next, (Long)temp2))
				{

					lsb = is_marked_reference((void *)temp1);

					if(!lsb) // free slot is here
					{
						temp2 = (Header *)get_marked_reference((void *)temp1);  // Set the LSB.
						found = true;
					}
					else
						break;
				}

				if(found)
					break;

				//temp = (Header *)get_unmarked_reference((void *)temp1);
				//current = temp;
				current = (Header *)get_unmarked_reference((void *)temp1);
			}

			if(found)
			{
				temp1 = dummy;
				temp2 = current;
				while(!Parvicursor_AtomicCompareAndSwap(&temp1, &head, (Long)temp2))
				{
					//temp2 = current;
				}

				counter.Increment();//

				//printf("hello6\n");
				return (void *)( (char *)current + sizeof(struct Header) );
			}
			else
				return null;
		}
		//----------------------------------------------------
		Long LockFreeMemoryPool::get_Count()
		{
			return counter.get_Count();
		}
		//----------------------------------------------------
		/* None thread-safe*/
		void LockFreeMemoryPool::Print()
		{

			/*volatile Header *current = first;
			volatile Header *temp = null;
			Int32 lsb = 0;

			while(true)
			{
				if(current == null)
					return;

				temp = current->next;
				lsb = TaggedPointer_RecoverData((void *)temp);

				//if(lsb)
					cout << "current address: " << current << " "<< convBase((ULong)(current), 2) << " lsb: " << lsb << endl;

				temp = (volatile Header *)TaggedPointer_RecoverPointer((void *)temp);
				current = temp;

				}*/

			Header volatile *temp = dummy;
			Header volatile *current = first;

			while(true)
			{
				temp = dummy;
				Parvicursor_AtomicCompareAndSwap(&temp, &current, (Long)current);
				if(temp == null)
					return ;


				cout << "current address: " << (Long)temp << " " << convBase((ULong)(temp), 2)  << endl;


				temp = dummy;
				while(!Parvicursor_AtomicCompareAndSwap(&temp, &current, (Long)current->next));

			}
		}
	    //----------------------------------------------------
}
//**************************************************************************************************************//
