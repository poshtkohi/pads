//#include "stdafx.h"
#include "LockFreeArrayList.h"


//**************************************************************************************************************//

namespace LockFree
{
		//----------------------------------------------------
		LockFreeArrayList::LockFreeArrayList(UInt32 maxCapacity)
		{
			if(maxCapacity == 0)
				maxCapacity = 10;

			this->maxCapacity = maxCapacity;
															/* maxCapacity + head + tail */
			pool = new LockFreeMemoryPool(sizeof(struct Node), maxCapacity + 2);
			head = (struct Node *)pool->Malloc();
			tail = (struct Node *)pool->Malloc();

			head->next = tail;

			disposed = false;
		}
		//----------------------------------------------------
		LockFreeArrayList::~LockFreeArrayList()
		{
			if(!disposed)
			{
				if(pool != null)
				{
					delete pool;
					pool = null;
				}
				disposed = true;
			}
		}
		//----------------------------------------------------
		Long LockFreeArrayList::get_Count()
		{
			return counter.get_Count();
		}
		//----------------------------------------------------
		/*
			This method performs a linear search; therefore,
			this method is an O(n) operation, where n is Count.
		*/
		bool LockFreeArrayList::Contains(Object *data)
		{
			return find(data);
		}
		//----------------------------------------------------
		/*
			Adds an object to the end of the ArrayList.
			Returns false if the array list reached its maximum capacity and there was not any free slot.
			This method is an O(1) operation.
		*/
		Status LockFreeArrayList::Add(Object *data)
		{
			Status status = insert(data);

			if(status == Success)
				counter.Increment();

			return status;
		}
		//----------------------------------------------------
		/*
			Removes the first occurrence of a specific object from the ArrayList.
			This method performs a linear search; therefore, this method is an O(n)
			operation, where n is Count.
		*/
		void LockFreeArrayList::Remove(Object *data) // we must set the header if the remove data is located at the end of list.
		{
			Status status = _delete(data);

			if(status == Success)
				counter.Decrement();

		}
		//----------------------------------------------------
		bool LockFreeArrayList::CAS(volatile void *address, Long old, Long _new)
		{
			if(Parvicursor_AtomicCompareAndSwap(&old, address, _new))
				return true;

			return false;
		}
		//----------------------------------------------------
		Status LockFreeArrayList::insert(Object *data)
		{
			//Node *new_node = (struct Node *)malloc(sizeof(struct Node));
			Node *new_node = (Node *)pool->Malloc();

			if(new_node == null)
				return ArrayListIsFull;

			new_node->data = data;

			volatile Node *right_node, *left_node;

			do {
				right_node = search(data, &left_node);
				if ((right_node != tail) && (right_node->data == data)) /*T1*/
				{
					pool->Free(new_node);
					return DuplicateData;
				}
				new_node->next = right_node;
				if(CAS(&(left_node->next), (Long)right_node, (Long)new_node)) /*C2*/
					return Success;
			} while (true); /*B3*/
		}
		//----------------------------------------------------
		bool LockFreeArrayList::find(Object *data)
		{
			Node volatile *right_node;
			Node volatile *left_node;
			right_node = search(data, &left_node);
			if((right_node == tail) || (right_node->data != data))
				return false;
			else
				return true;
		}
		//----------------------------------------------------
		Status LockFreeArrayList::_delete(Object *data)
		{
			Node volatile *right_node;
			Node volatile *right_node_next;
			Node volatile *left_node;

			do {
				right_node = search(data, &left_node);
				if ((right_node == tail) || (right_node->data != data)) /*T1*/
					return NoSuchData;
				right_node_next = right_node->next;
				if(!is_marked_reference((void *)right_node_next))
				{
					if(CAS(&(right_node->next), /*C3*/ (Long)right_node_next, (Long)get_marked_reference((void *)right_node_next)))
					{
						if(!CAS(&(left_node->next), (Long)right_node, (Long)right_node_next)) /*C4*/
							right_node = search(right_node->data, &left_node);

						pool->Free((void *)right_node);
						return Success;
					}
				}
			} while (true); /*B4*/
		}
		//----------------------------------------------------
		volatile Node *LockFreeArrayList::search(Object *data, volatile Node **left_node)
		{
			Node volatile *left_node_next;
			Node volatile *right_node;
	search_again:
			do {
				Node volatile *t = head;
				Node volatile *t_next = head->next;
				/* 1: Find left_node and right_node */
				do {
					if(!is_marked_reference((void *)t_next))
					{
						(*left_node) = t;
						left_node_next = t_next;
					}
					t = (Node *)get_unmarked_reference((void *)t_next);
					if(t == tail)
						break;
					t_next = t->next;
				} while (is_marked_reference((void *)t_next) || (t->data != data)); /*B1*/
				//} while (is_marked_reference(t_next) || (t->data < data)); /*B1*/
				right_node = t;
				/* 2: Check nodes are adjacent */
				if (left_node_next == right_node)
				{
					if ((right_node != tail) && is_marked_reference((void *)right_node->next))
						goto search_again; /*G1*/
					else
						return right_node; /*R1*/
				}
				/* 3: Remove one or more marked nodes */
				if(CAS(&((*left_node)->next), (Long)left_node_next, (Long)right_node)) /*C1*/
				{
					if ((right_node != tail) && is_marked_reference((void *)right_node->next))
						goto search_again; /*G2*/
					else
						return right_node; /*R2*/
				}
			} while (true); /*B2*/
		}
	    //----------------------------------------------------
}
//**************************************************************************************************************//
