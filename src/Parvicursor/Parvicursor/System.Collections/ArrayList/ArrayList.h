/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Collections_ArrayList_h__
#define __System_Collections_ArrayList_h__

#include "../../System/Object/Object.h"
#include "../../System/ArgumentException/ArgumentOutOfRangeException.h"

namespace System
{
	/* Sample using
	   for(Int32 i = 0 ;   ; i++)
	   {
	     String s1("hi,");
	     String s2(" how are u?");
	     String *ss = s1 + s2;
	     al.Add(ss);
	     al.Remove(ss);
	     delete ss;
	     ss = s1 + s2;
	     al.Add(ss);
	     ss = (String *)al[0];
	     printf("%s\n", ss->GetBaseStream());
	     al.Remove(ss);
	     delete ss;
         #if defined WIN32 | WIN64
	             Sleep(1);
         #endif
	   }
	*/
	namespace Collections
	{
		class ArrayList : public Object
		{
			private: class node;
			private: node *first;
			private: node *last;
			private: Int32 lastIndex;
			private: bool disposed;
			//----------------------------------------------------
			private: class node
			{
				public: node *next;
				public: void *data;
				public: Int32 index;
				public: node();
				public: ~node();
			};
			//----------------------------------------------------
			public: ArrayList();
			public: ~ArrayList();
			public: void Clear();
			public: void Add(Object *data);
			public: void Insert(Int32 index, Object *value);
			public: Int32 get_Count();
			public: bool Contains(Object *item);
			public: void Remove(Object *data);
			public: void RemoveAt(Int32 index);
			private: inline void DecreaseIndexes(node* deletedNode, Int32 index);
			public: Object *get_Value(Int32 index);
			public: void set_Value(Int32 index, Object *obj);
			public: Object *operator[](Int32 index);
			//public: void operator[](Int32 index, Object *obj);
			//public: Array ToArray[]();
			//----------------------------------------------------
		};

	};
};

#endif
