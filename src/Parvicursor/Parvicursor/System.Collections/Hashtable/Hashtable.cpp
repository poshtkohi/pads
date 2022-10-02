/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "Hashtable.h"

//**************************************************************************************************************//
namespace System
{
	namespace Collections
	{
		//----------------------------------------------------
		Hashtable::Hashtable()
		{
			//if(this->al != null)
				this->al = new ArrayList();
		}
		//----------------------------------------------------
		Hashtable::~Hashtable()
		{
			if(this->al != null)
			{
				/*Info *info;
				for(Int32 i = 0 ; i < this->al->get_Count() ; i++)
				{
					info = (Info *)this->al->get_Value(i);
					if(info != null)
						delete info;
				}*/
				this->Clear();
				delete this->al;
				this->al = null;
			}
		}
		//----------------------------------------------------
		Int32 Hashtable::get_Count()
		{
			if(this->al != null)
				return this->al->get_Count();
			else
				return 0;
		}
		//----------------------------------------------------
		/*ArrayList *Hashtable::get_Keys();
		{
			if(this->get_Count() > 0)
			{
				ArrayList *temp = new ArrayList();

			}
			else
				return null;
		}*/
		//----------------------------------------------------
		Object *Hashtable::get_Item(const String &key)
		{
			/*if(index < 0)
				throw ArgumentOutOfRangeException("index", "index is less than zero.");
			if(index > (this->get_Count() - 1))
				throw ArgumentOutOfRangeException("index", "index is equal to or greater than Count.");*/

			register Int32 i = 0, hash = 0;
			Info *item;

			hash = this->GetHashKey(key);

			for(i = 0 ; i < this->al->get_Count() ; i++)
			{
				item = (Info *)this->al->get_Value(i);
				if(item->hash == hash)
				{
					return item->value;
				}
			}

			return null;
		}
		//----------------------------------------------------
		Object *Hashtable::get_Value(Int32 index)
		{
			if(index < 0)
				throw ArgumentOutOfRangeException("index", "index is less than zero.");
			if(index > (this->get_Count() - 1))
				throw ArgumentOutOfRangeException("index", "index is equal to or greater than Count.");

			return ((Info *)this->al->get_Value(index))->value;
		}
		//----------------------------------------------------
		void Hashtable::Add(const String &key, Object *value)
		{
			if(key.get_BaseStream() == null)
				throw ArgumentNullException("key is null");

			if(value == null)
				throw ArgumentNullException("value is null");

			if(Contains(key))
				throw ArgumentException("An element with the same key already exists in the Hashtable");

		/*
			// the following code snippet results to memory leakage
			Info info;
			info.hash = GetHashKey(key); printf("%d\n", info.hash);
			info.value = value;

			this->al->Add((Object *)&info);
		*/

			Info *info = new Info();
			info->hash = this->GetHashKey(key);
			info->value = value;

			this->al->Add(info);

		}
		//----------------------------------------------------
		Int32 Hashtable::GetHashKey(const String &key)
		{
			register Int32 _key = 0, i = 0;

			for(i = 0 ; i < key.get_Length() ; i++)
				_key +=  toascii(key.get_Value(i));

			_key = _key - 97;
			return _key;
		}
		//----------------------------------------------------
		void Hashtable::Clear()
		{
			Info *info;

			register Int32 i = 0;

			for(i = 0 ; i < this->al->get_Count() ; i++)
			{
				info = (Info *)this->al->get_Value(i);
				if(info != null)
					delete info;
			}
			this->al->Clear();
		}
		//----------------------------------------------------
		bool Hashtable::Contains(const String &key)
		{
			if(this->al->get_Count() == 0)
				return false;

			register Int32 i = 0, hash = 0;
			//Info *info;

			hash = this->GetHashKey(key);

			for(i = 0 ; i < this->al->get_Count() ; i++)
				if(((Info *)this->al->get_Value(i))->hash == hash)
					return true;
				/*info = (Info *)this->al->get_Value(i);
				if(info->hash == hash)
					return true;*/

			return false;
		}
		//----------------------------------------------------
		bool Hashtable::ContainsKey(const String &key)
		{
			return this->Contains(key);
		}
		//----------------------------------------------------
		void Hashtable::Remove(const String &key)
		{
			if(key.get_BaseStream() == null)
				throw ArgumentNullException("key is null");

			Info *info;
			register Int32 i = 0, hash = 0;

			hash = this->GetHashKey(key);

			for(i = 0 ; i < this->al->get_Count() ; i++)
			{
				info = (Info *)this->al->get_Value(i);
				if(info->hash == hash)
				{
					if(info != null)
						delete info;
					this->al->RemoveAt(i);
					break;
				}
			}

		}
		//----------------------------------------------------
	};
};
//**************************************************************************************************************//
