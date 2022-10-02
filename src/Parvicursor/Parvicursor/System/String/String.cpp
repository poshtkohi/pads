/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include <string.h>
#include <stdio.h>


#include  "String.h"

#include "../../System.Collections/ArrayList/ArrayList.h"
#include "../../System/ArgumentException/ArgumentNullException.h"
#include "../../System/ArgumentException/ArgumentOutOfRangeException.h"
#include "../../System/ObjectDisposedException/ObjectDisposedException.h"

//**************************************************************************************************************//

using namespace System::Collections;

namespace System
{
	//----------------------------------------------------
	String::String()
	{
		this->fromReplace = false;
		this->size = 0;
		this->bytes = null;
		fromBuffer = false;
	}
	//----------------------------------------------------
    String::String(const String & str)
	{
		this->fromReplace = false;
		if(str.get_BaseStream() == null)
		{
			String();
			return ;
		}
		else
		{
			this->bytes = new char[str.get_Length() + 1];
			this->size = str.get_Length();
			for(register Int32 i = 0 ; i < this->size ; i++)
				this->bytes[i] = str.get_Value(i);
			this->bytes[this->size] = 0;
			return;
		}
		fromBuffer = false;
	}
	//----------------------------------------------------
	String::String(const char *buffer)
	{
		this->fromReplace = false;
		if(buffer != null)
		{
			//add here exception for large Size
			this->size = (Int32)strlen(buffer);
			this->bytes = new char[this->size + 1];
			for(register Int32 i = 0 ; i < this->size ; i++)
				this->bytes[i] = buffer[i];
			this->bytes[this->size] = 0;
		}
		else
			this->bytes = (char *)buffer;
		//this->obj = this;
	}
	//----------------------------------------------------
	String::String(char *buffer)
	{
		this->fromReplace = false;
		if(buffer != null)
		{
			//add here exception for large Size
			this->size = (Int32)strlen(buffer);
			this->bytes = buffer;
		}
		else
			bytes = buffer;

		fromBuffer = true;
		//this->obj = this;
	}
	//----------------------------------------------------
	String::~String()// attend on this method
	{
		if(this->bytes != null && !fromBuffer)
			delete this->bytes;

		this->bytes = null;
		this->fromReplace = false;
		this->size = 0;
	}
	//----------------------------------------------------
	bool String::Equals(String &str1, String &str2)
	{
		if(str1 == str2)
			return true;
		else
			return false;
	}
    //----------------------------------------------------
	char *String::get_BaseStream() const
	{
		return this->bytes;
	}
	//----------------------------------------------------
	char String::get_Value(Int32 index) const
	{
		if(this->bytes != null)
			return this->bytes[index];
		//here must be implemented an exception
		else
			throw ObjectDisposedException("String");
	}
	//----------------------------------------------------
	Int32 String::get_Length() const
	{
		if(bytes == null)
			return 0;
		else
			return size;
	}
	//----------------------------------------------------
	String String::Trim()
		{
			if(this->bytes == null)
				return String::Copy(*this);

			Int32 startIndex = -1, endIndex = -1;
			for(register Int32 i = 0 ; i < size ; i++)
			{
				if(this->bytes[i] == ' ')
				{
					startIndex = i;
					continue;
				}
				else
				{
					if(i < size - 1)
					{
						if(startIndex != size - 1)
						{
							for(register Int32 j = size - 1 ; j >= 0 ; j--)
							{
								if(this->bytes[j] == ' ')
								{
									endIndex = j;
									continue;
								}
								else
									break;
							}
						}
					}
					break;
				}
			}
			if(startIndex >= 0 && endIndex > 0)
			{
				Int32 len = endIndex - (startIndex + 1);
				char  *newBuffer = new char[len + 1];
				for(register Int32 i = startIndex + 1 , j = 0; i < endIndex ; i++, j++)
					newBuffer[j] =  this->bytes[i];
				newBuffer[len] = 0;
				return String(newBuffer);
			}
			if(startIndex >= 0 && endIndex == -1)
			{
				Int32 len = size - (startIndex + 1);
				char *newBuffer = new char[len + 1];
				for(register Int32 i = startIndex + 1 , j = 0; i < size ; i++, j++)
					newBuffer[j] =  this->bytes[i];
				newBuffer[len] = 0;
				return String(newBuffer);
			}
			if(startIndex == -1 && endIndex > 0)
			{
				char* newBuffer = new char[endIndex + 1];
				for(register Int32 i = 0 , j = 0; i < endIndex ; i++, j++)
					newBuffer[j] =  this->bytes[i];
				newBuffer[endIndex] = 0;
				return String(newBuffer);
			}
			else
			{
				return String::Copy(*this);
			}
		}
	//----------------------------------------------------
	String String::Substring(Int32 startIndex, Int32 count)
	{
		if(bytes == null)
			return String::Copy(*this);
		//add here exception for startIndex and count
		char* buffer = new char[count + 1];
		for(register Int32 i = 0 ; i < count ; i++)
			buffer[i] = this->get_Value(startIndex + i);
		buffer[count] = 0;
		return String(buffer);
	}
	//----------------------------------------------------
	String String::Substring(Int32 startIndex)
	{
		return this->Substring(startIndex, this->get_Length() - startIndex);
	}
	//----------------------------------------------------
	String String::Copy(const String &str)
	{
		if(str.get_BaseStream() == null)
			throw ArgumentNullException("str");

		char *buffer = new char[str.get_Length() + 1];
		for(register Int32 i = 0 ; i < str.get_Length() ; i++)
			buffer[i] = str.get_Value(i);
		buffer[str.get_Length()] = 0;
		return String(buffer);
	}
	//----------------------------------------------------
	String String::ToUpper()
	{
		if(bytes == null)
			return String::Copy(*this);
		char *buffer = new char[this->get_Length() + 1];
		for(register Int32 i = 0 ; i < this->size ; i++)
		{
			if(this->get_Value(i) >= 'a' && this->get_Value(i) <= 'z')
				buffer[i] = this->get_Value(i) - 32;
			else
				buffer[i] = this->get_Value(i);
		}
		buffer[this->get_Length()] = 0;
		return String(buffer);
	}
	//----------------------------------------------------
	String String::ToLower()
	{
		if(bytes == null)
			return String::Copy(*this);

		char *buffer = new char[this->get_Length() + 1];
		for(register Int32 i = 0 ; i < this->get_Length() ; i++)
		{
			if(this->get_Value(i) >= 'A' && this->get_Value(i) <= 'Z')
				buffer[i] = this->get_Value(i) + 32;
			else
				buffer[i] = this->get_Value(i);
		}
		buffer[this->get_Length()] = 0;
		return String(buffer);
	}
	//----------------------------------------------------
	Int32 String::IndexOf(const String &search, Int32 startIndex, Int32 count)
	{
		if(search.get_BaseStream() == null)
			throw ArgumentNullException("search");

		if(startIndex < 0 || count < 0)
			throw ArgumentOutOfRangeException("startIndex or count", "count or startIndex is negative.");

		if(count == 0)
			throw ArgumentOutOfRangeException("count", "count is zero.");

		bool find = false;
		Int32 n = size;
		Int32 m = search.get_Length();
		if(m > n)
			return -1;
		if((count + startIndex)  > n && !this->fromReplace)
			throw ArgumentOutOfRangeException("count + startIndex", "count plus startIndex specify a position not within this instance.");
		Int32 start = 0;
		for(register Int32 i = startIndex ; i < startIndex + count ; i++)
		{
			if(this->get_Value(i) == search.get_Value(0))
			{
				if(search.get_Length() == 1)
					return i;
				for(register Int32 j = 1 ; j < (Int32)search.get_Length() ; j++)
				{
					if(i + j >= n)
						return -1;
					if(this->get_Value(i + j) != search.get_Value(j))
					{
						find = false;
						break;
					}
					if(j == search.get_Length() - 1)
					{
						find = true;
						start = i;
						goto Exit;
					}
				}

			}
		}
	Exit:
		if(find)
			return start;
		else
			return -1;
	}
	//----------------------------------------------------
	Int32 String::IndexOf(const String &search)
	{
		return IndexOf(search, 0);
	}
	//----------------------------------------------------
	Int32 String::IndexOf(const String &search, Int32 startIndex)
	{
		return IndexOf(search, startIndex, this->get_Length() - startIndex);
	}
	//----------------------------------------------------
	Int32 String::LastIndexOf(const String &search, Int32 startIndex, Int32 count)
	{
		if(search.get_BaseStream() == null)
			throw ArgumentNullException("search");

		if(startIndex < 0 || count < 0)
			throw ArgumentOutOfRangeException("startIndex or count", "count or startIndex is negative.");

		if(count == 0)
			throw ArgumentOutOfRangeException("count", "count is zero.");

		bool first = false, last = false ;
		Int32 n = this->get_Length();
		Int32 m = search.get_Length();
		if((count + startIndex)  > n)
			throw ArgumentOutOfRangeException("count + startIndex", "count plus startIndex specify a position not within this instance.");
		if(m > n)
			return -1;
		Int32 start = 0, end = 0;
		for(register Int32 i = startIndex ; i < startIndex + count ; i++)
		{
			if(this->get_Value(i) == search.get_Value(0))
			{
				if(search.get_Length() == 1)
				{
					if(first == false)
					{
						first = true;
						start = i;
					}
					if(last == false)
					{
						last = true;
					}
					end = i;
				}
				else
				{
					for(register Int32 j = 1 ; j < search.get_Length() ; j++)
					{
						if(i + j >= n)
							return -1;
						if(this->get_Value(i + j) != search.get_Value(j))
						{
							if(!last)
								last = true;
							break;
						}
						if(j == search.get_Length() - 1)
						{
							if(first == false)
							{
								first = true;
								start = i;
							}
							if(last == false)
							{
								last = true;
							}
							end = i;
						}
					}
				}
			}
		}
		if(last)
			return end;
		else if(first)
			return start;
		else return -1;
	}
	//----------------------------------------------------
	Int32 String::LastIndexOf(const String &search)
	{
		return LastIndexOf(search, 0);
	}
	//----------------------------------------------------
	Int32 String::LastIndexOf(const String &search, Int32 startIndex)
	{
		return LastIndexOf(search, startIndex, this->get_Length());
	}
	//----------------------------------------------------
	const String &String::operator+=(const String &str)
	{
		if(str.get_BaseStream() == null)
			return *this;
		else
		{
			char *b = new char[this->get_Length() + str.get_Length() + 1];
			register Int32 index = str.get_Length() + (this->get_Length() - str.get_Length());
			for(register Int32 i = 0 ; i < this->get_Length() + str.get_Length() ; i++)
			{
				if(i < this->get_Length())
					b[i] = this->get_Value(i);
				if(i >= this->get_Length())
					b[i] = str.get_Value(i - index);
			}
			b[this->get_Length() + str.get_Length()] = 0;
			this->size = this->get_Length() + str.get_Length();
			delete bytes;
			bytes = b;
			return *this;
		}
	}
	//----------------------------------------------------
	const String &String::operator+=(const char *str)
	{
		String Str(str);
		return *this += Str;
	}
	//----------------------------------------------------
	String String::Concat(const String &str0, const String &str1)
	{
		return str0 + str1;
	}
	//----------------------------------------------------
	String operator+(const String &StrA, const String &StrB)
	{
		if(StrA.get_BaseStream() == null)
			return String::Copy(StrB);

		if(StrB.get_BaseStream() == null)
			return String::Copy(StrA);

		char *b = new char[StrA.get_Length() + StrB.get_Length() + 1];
		register Int32 index = StrB.get_Length() + (StrA.get_Length() - StrB.get_Length());
		for(register Int32 i = 0 ; i < StrA.get_Length() + StrB.get_Length() ; i++)
		{
			if(i < StrA.get_Length())
				b[i] = StrA.get_Value(i);
			if(i >= StrA.get_Length())
				b[i] = StrB.get_Value(i - index);
		}
		b[StrA.get_Length() + StrB.get_Length()] = 0;
		return String(b);
	}
	//----------------------------------------------------
	String operator+(const String &StrA, const char *StrB)
	{
		String b(StrB);
		return StrA + b;
	}
	//----------------------------------------------------
	String operator+(const char *StrA, const String &StrB)
	{
		String a(StrA);
		return a + StrB;
	}
	//----------------------------------------------------
	const String &String::operator =(const String & str)
	{
		if(this != &str)
		{
			if(this->bytes != null)
				delete this->bytes;
			this->bytes = new char[str.get_Length() + 1];
			for(register Int32 i = 0 ; i < str.get_Length() ; i++)
				this->bytes[i] = str.get_Value(i);
			this->bytes[str.get_Length()] = 0;
			this->size = str.size;
		}
		return *this;    // Return ref for multiple assignment
	}
	//----------------------------------------------------
	Int32 String::Compare(const String &StrA, const String &StrB)
	{
		if(StrA.get_BaseStream() == null && StrB.get_BaseStream() == null)
			return 0;

		if(null ==  StrA.get_BaseStream() && null == StrB.get_BaseStream())
			return -1;

		if(null == StrA.get_BaseStream() && null == StrB.get_BaseStream())
			return -1;

		else
		{
			if(StrA.get_Length() == 0 && StrB.get_Length() > 0)
				return -1;
			if(StrB.get_Length() == 0 && StrA.get_Length() > 0)
				return 1;
			if(StrA.get_Length() > StrB.get_Length())
				return 1;
			if(StrA.get_Length() < StrB.get_Length())
				return -1;
			else
			{
				for(register Int32 i = 0 ; i < StrA.get_Length() ; i++)
					if(StrA.get_Value(i) != StrB.get_Value(i))
					{
						if(StrA.get_Value(i) > StrB.get_Value(i))
							return 1;
						else
							return -1;
					}
				return 0;
			}
		}
	}
	//----------------------------------------------------
	/*friend bool String::operator ==(const String &StrA, const String &StrB)
	{
		if(String::Compare(StrA, StrB) == 0)
			return true;
		else
			return false;
	}
	//----------------------------------------------------
	friend bool String::operator !=(const String &StrA, const String &StrB)
	{
		if(String::Compare(StrA, StrB) == 0)
			return false;
		else
			return true;
	}*/
	//----------------------------------------------------
	String String::Replace(const String &oldValue, const String &newValue)
	{
		if(oldValue.get_BaseStream() == null)
			throw ArgumentNullException("oldValue");

		ArrayList al;
		Int32 p1 = 0;
		this->fromReplace = true;
		while(true)
		{
			if(p1 < this->get_Length())
			{
				p1 = this->IndexOf(oldValue, p1);
				if(p1 >= 0)
				{
					al.Add((Object *)p1);
					p1 += oldValue.get_Length();
				}
				else
					break;
			}
			else
				break;
		}
		if(al.get_Count() == 0)
		{
			this->fromReplace = false;
			al.Clear();
			return String::Copy(*this);
		}
		else
		{
			String str;
			for(register Int32 i = 0 ; i < al.get_Count() ; i++)
			{
				if(i == 0)
					str = this->Substring(0, (Int32)(Long)al[i]) + newValue;
				else
					str = str + this->Substring((Int32)(Long)al[i - 1] + oldValue.get_Length(), (Int32)(Long)al[i] - (Int32)(Long)al[i - 1] - oldValue.get_Length()) + newValue;
			}
			if((Int32)(Long)al[al.get_Count() - 1] + oldValue.get_Length() < this->get_Length())
				str = str + this->Substring((Int32)(Long)al[al.get_Count() - 1] + oldValue.get_Length(), this->get_Length());
			al.Clear();
			this->fromReplace = false;
			return Copy(str);
		}
	}
	//----------------------------------------------------
}
//**************************************************************************************************************//
