/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Collections_Hashtable_h__
#define __System_Collections_Hashtable_h__

#include "../../System/Object/Object.h"
#include "../../System/ArgumentException/ArgumentNullException.h"
#include "../../System/ArgumentException/ArgumentException.h"
#include "../../System.Collections/ArrayList/ArrayList.h"

#include <ctype.h>

using namespace System;


namespace System
{
	namespace Collections
	{
		class Hashtable : public Object
		{
			private: class Info : public Object
			{
				public: Int32 hash;
				//public: String key;
				public: Object *value;
			};
			private: ArrayList *al;
			//----------------------------------------------------
			public: Hashtable();
			public: ~Hashtable();
			public: Int32 get_Count();
			public: Object *get_Item(const String &key);
			public: Object *get_Value(Int32 index);
			//public: ArrayList *get_Keys();
			public: void Add(const String &key, Object *value);
			public: void Clear();
			public: bool Contains(const String &key);
			public: bool ContainsKey(const String &key);
			public: void Remove(const String &key);
			public: Int32 GetHashKey(const String &key);
			//----------------------------------------------------
		};

	};
};

#endif
