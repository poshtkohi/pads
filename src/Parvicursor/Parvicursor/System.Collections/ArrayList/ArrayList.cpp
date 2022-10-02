/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include  "ArrayList.h"

//**************************************************************************************************************//
namespace System
{
	namespace Collections
	{
		//----------------------------------------------------
		ArrayList::node::node()
		{
		}
		ArrayList::node::~node()
		{
		}
		//----------------------------------------------------
		ArrayList::ArrayList()
		{
			this->disposed = false;
			this->lastIndex = 0;
			this->first = null;
			this->last = null;
		}
		//----------------------------------------------------
		ArrayList::~ArrayList()
		{
            if(!disposed)
            {
                Clear();
				disposed = true;
                /*if(first == last && first != null && last != null)
                {
                    delete first;
                }
                else
                {
                    if(first != null)
                        delete first;
                    if(last != null)
                        delete last;
                }*/
            }
		}
		//----------------------------------------------------
		void ArrayList::Clear()
		{
		    if(disposed)
                return;
			if(this->lastIndex == 0)
				return ;
			this->lastIndex = 0;
			node *curPtr = first;
			node *temp ;
			while(curPtr != null)
			{
				temp = curPtr;
				curPtr = curPtr->next;
				delete temp;
			}
			first = last = null;
			return ;
		}
		//----------------------------------------------------
		void ArrayList::Add(Object *data)
		{
            if(disposed)
                return;
			node *newPtr = new node();
			if(newPtr != null)
			{
				newPtr->index = this->lastIndex;
				this->lastIndex++;
				newPtr->data = data;
				newPtr->next = null;
				if(first == null)
					first = last = newPtr;
				else
				{
					last->next = newPtr;
					last = newPtr;
				}
				return ;
			}
			else
			{
				delete newPtr;
				return ;
			}
		}
		//----------------------------------------------------
		Int32 ArrayList::get_Count()
		{
            if(disposed)
                -1;
			return this->lastIndex;
		}
		//----------------------------------------------------
		bool ArrayList::Contains(Object *item)
		{
			if(this->lastIndex == 0)
				return false;
			node *curPtr = first;
			while(curPtr != null)
			{
				if(curPtr->data == item)
					return true;
				curPtr = curPtr->next;
			}
			return false;
		}
		//----------------------------------------------------
		void ArrayList::Remove(Object *data)
		{
            if(disposed)
                return;
			node *curPtr = this->first ;
			node *nextPtr = this->first ;
			while(nextPtr != null)
			{
				if(data == nextPtr->data)
				{
					if(nextPtr == this->first)
					{
						first = first->next;
						DecreaseIndexes(nextPtr, nextPtr->index);//
						this->lastIndex--;
						delete nextPtr;
						break;
					}
					else
					{
						if(nextPtr == this->last)
						{
							this->last = curPtr;
						}
						curPtr->next = nextPtr->next;
						DecreaseIndexes(nextPtr, nextPtr->index);//
						this->lastIndex--;
						delete nextPtr;
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
		void ArrayList::RemoveAt(Int32 index)
		{
            if(disposed)
                return;
			if(index < 0)
				throw ArgumentOutOfRangeException("index", "index is less than zero.");
			if(index >= this->lastIndex)
				throw ArgumentOutOfRangeException("index", "index is equal to or greater than Count.");
			if(index < 0 || index > this->lastIndex)//must be implemented exception ArgumentOutOfRangeException
				return ;
			node *curPtr = this->first ;
			node *nextPtr = this->first ;
			while(nextPtr != null)
			{
				if(nextPtr->index == index)
				{
					if(nextPtr == this->first)
					{
						first = first->next;
						DecreaseIndexes(nextPtr, nextPtr->index);//
						this->lastIndex--;
						delete nextPtr;
						break;
					}
					else
					{
						if(nextPtr == this->last)
						{
							this->last = curPtr;
						}
						curPtr->next = nextPtr->next;
						DecreaseIndexes(nextPtr, nextPtr->index);//
						this->lastIndex--;
						delete nextPtr;
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
		void ArrayList::DecreaseIndexes(node* deletedNode, Int32 index)
		{
			if(index == 0 || index < this->lastIndex)
			{
				node *curPtr = deletedNode->next;
				while(curPtr != null)
				{
					curPtr->index--;
					curPtr = curPtr->next;
				}
				return ;
			}
			else//if(index == this->lastIndex)
				return;
		}
		//----------------------------------------------------
		Object *ArrayList::get_Value(Int32 index)
		{
            if(disposed)
                return null;
			if(index < 0)
				throw ArgumentOutOfRangeException("index", "index is less than zero.");
			if(this->lastIndex == 0 || index > this->lastIndex)
				return null;
			node *curPtr = first;
			while(curPtr != null)
			{
				if(curPtr->index == index)
					return (Object *)curPtr->data;
				curPtr = curPtr->next;
			}
			return null;
		}
		//----------------------------------------------------
		void ArrayList::set_Value(Int32 index, Object *obj)
		{
            if(disposed)
                return;
			if(this->lastIndex == 0 || index == this->get_Count())
			{
				this->Add(obj);
				return ;
			}
			if(index < 0)
				throw ArgumentOutOfRangeException("index", "index is less than zero.");
			if(index > this->lastIndex)
				throw ArgumentOutOfRangeException("index", "index is greater than Count.");
			node *curPtr = first;
			while(curPtr != null)
			{
				if(curPtr->index == index)
				{
					curPtr->data = obj;
					return;
				}
				curPtr = curPtr->next;
			}
		}
		//----------------------------------------------------
		Object *ArrayList::operator[](Int32 index)
		{
			return get_Value(index);
		}
		//----------------------------------------------------
		void ArrayList::Insert(Int32 index, Object *value)
		{
            if(disposed)
                return;
			this->set_Value(index, value);
		}
		//----------------------------------------------------
		/*void ArrayList::operator[](Int32 index, Object *obj)
		{
			this->set_Value(obj, index);
		}*/
		//----------------------------------------------------
		/*public: Array ToArray[]()
			{
				return array;
			}*/ // must be implement
		//----------------------------------------------------
	};
};
//**************************************************************************************************************//
