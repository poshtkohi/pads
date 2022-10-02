// LockFreeStackTest.cpp : Defines the entry point for the console application.
//

//------------------------------------------------------------------------------
#include "../../Parvicursor/general.h"
#include "../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../Parvicursor/System.Threading/Thread/Thread.h"

#include "LockFree/LockFreeStack/LockFreeStack.h"
#include "LockFree/LockFreeArrayList/LockFreeArrayList.h"
#include "LockFree/LockFreeMemoryPool/LockFreeMemoryPool.h"
//------------------------------------------------------------------------------
using namespace System;
using namespace System::Threading;

using namespace LockFree;
//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	//cout << sizeof(Int128) << endl; return 0;
	//return LockFreeStackTest::Parvicursor_main(argc, argv);
	return LockFreeArrayListTest::Parvicursor_main(argc, argv);
	//return LockFreeMemoryPoolTest::Parvicursor_main(argc, argv);
}
//------------------------------------------------------------------------------
/*Int32 counter = 0; // i++ or counter++
void *thread_function( void *ptr )
{
	Int32 temp;
	while(true)
	{
		AtomicAdd(&counter, &temp, 1);
		printf("%d\n", temp);
	}
	return ptr;
}
void AtomicIncrement_Test()
{
	 pthread_t threads[100];
     char *message1 = "Thread 1";

	 for(Int32 k = 0 ; k < 100 ; k++)
		pthread_create(&threads[k], NULL, thread_function, (void*) message1);

	 for(Int32 k = 0 ; k < 100 ; k++)
		pthread_join( threads[k], NULL);

}*/
//------------------------------------------------------------------------------
/*struct node
{
	void *data;
	struct node *next;
};

struct node *first = null;
struct node *last = null;

void add_data(void *data)
{
	node *newNode = (struct node *)malloc(sizeof(struct node));
	newNode->next = null;
	newNode->data = data;

	if(first == null)
	{
		first = newNode;
		last = newNode;
	}
	else
	{
		last->next = newNode;
		last = newNode;
	}
}*/
//------------------------------------------------------------------------------
/*class DoublyLinkedList : public Object
{
	private: struct Node
	{
		struct Node *prev; // A reference to the previous node
		struct Node *next; // A reference to the next node
		void *data; // Data or a reference to data
	};
	private: struct Node *firstNode;  // points to first node of list
	private: struct Node *lastNode;   // points to last node of list
	private: UInt32 listSize; // The size of the current linked list

	public: DoublyLinkedList(UInt32 size)
	{
		if(size == 0)
			size = 10;

		this->listSize = size;
	}
	
	private: void PreAllocate()
	{
		for(UInt32 i =  0 ; i < this->listSize; i++)
		{
		}
	}
	
	// Insert newNode after node.
	private: void InsertAfter(Node *node, Node *newNode)
	{
		newNode->prev = node;
		newNode->next = node->next;

		if(node->next == null)
			this->lastNode = newNode;
		else
			node->next->prev = newNode;

		node->next = newNode;
	}
	
	// Insert newNode before node.
	private: void InsertBefore(Node *node, Node *newNode)
	{
		newNode->prev = node->prev;
		newNode->next = node;
		if(node->prev == null)
			this->firstNode = newNode;
		else
			node->prev->next = newNode;

		node->prev = newNode;
	}
	
	// Insert a node at the beginning of a possibly empty doubly linked list.
	private: void InsertBeginning(Node *newNode)
	{
		if(this->firstNode == null)
		{
			this->firstNode = newNode;
			this->lastNode = newNode;
			newNode->prev = null;
			newNode->next = null;
		}
		else
			InsertBefore(this->firstNode, newNode);
	}
	
	// Insert at the end
	private: void InsertEnd(Node *newNode)
	{
		if(lastNode == null)
			InsertBeginning(newNode);
		else
			InsertAfter(this->lastNode, newNode);
	}
	
	private: void Remove(Node *node)
	{
		if(node->prev == null)
			this->firstNode = node->next;
		else
			node->prev->next = node->next;
		if(node->next == null)
			this->lastNode = node->prev;
		else
			node->next->prev = node->prev;
		//destroy node
	}

};*/