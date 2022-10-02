/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __Parvicursor_Collections_SinglyLinkedList_h__
#define __Parvicursor_Collections_SinglyLinkedList_h__

#include "../../general.h"
#include "../../System/BasicTypes/BasicTypes.h"
#include "../../System/Object/Object.h"
#include "../../System/ObjectDisposedException/ObjectDisposedException.h"

using namespace System;
//**************************************************************************************************************//

namespace Parvicursor
{
	namespace Collections
	{
		//----------------------------------------------------
		class SinglyLinkedList : public Object
		{
			/*---------------------fields----------------*/
			// The node definition.
			private: struct Node : public Object
			{
				const Object *data;
				struct Node *next; // The successor node with respect to this node.
			};
			private: Node *first; // The first pointer.
			private: Node *last; // The first pointer.
			private: bool disposed; // For destructor use.
			private: Int32 counter; // Stores the number of elements in the SinglyLinkedList.
			/*---------------------methods----------------*/
			// SinglyLinkedList Class constructor.
			public: SinglyLinkedList();
			// SinglyLinkedList Class destructor.
			public: ~SinglyLinkedList();
			// Gets the number of elements available in the stack.
			public: Int32 get_Count();
			// Adds a new data item to the SinglyLinkedList.
			public: void Add(const Object *data);
			// Removes a data item from the SinglyLinkedList.
			public: void Remove(const Object *data);
			// Removes the fist data item from the SinglyLinkedList. If the SinglyLinkedList is empty, this method returns null.
			public: const Object *RemoveBeginning();
			// Determines whether an element is in the SinglyLinkedList.
			public: bool Contains(const Object *item);
			// Clears all data items witin the SinglyLinkedList.
			public: void Clear();
			// Returns the object at the beginning of the SinglyLinkedList without removing it.
			public: const Object *Peek();
		};
		//----------------------------------------------------
	}
};
//**************************************************************************************************************//

#endif

