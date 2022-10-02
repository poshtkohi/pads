#define null NULL
#include <string.h>
#include "../ArrayList/ArrayList.h"
//#include "../Object/Object.h"
//**************************************************************************************************************//
namespace Parvicursor
{
	class String : public Object
	{
		//----------------------------------------------------
	private: char *bytes;
	private: int size;
		//----------------------------------------------------
	    public: String()
		{
			this->size = 0;
			//this->bytes = (char *)null;
		}
		//----------------------------------------------------
	public: String(const char *buffer)
		{
			if(buffer != null)
			{
			   //add here exception for large Size
			   this->size = (int)strlen(buffer);
			   this->bytes = new char[this->size + 1];
			   for(int i = 0 ; i < this->size ; i++)
				   this->bytes[i] = buffer[i];
			   this->bytes[this->size] = 0;
			}
			else
				this->bytes = (char *)buffer;
			//this->obj = this;
		}
		//----------------------------------------------------
	public: String(char *buffer)
		{
			if(buffer != null)
			{
			   //add here exception for large Size
			   this->size = (unsigned int)strlen(buffer);
			   this->bytes = buffer;
			}
			else 
				bytes = buffer;
			//this->obj = this;
		}
		//----------------------------------------------------
	public: ~String()// attend on this method
			 {
				 //if(this->size > 0)
				   delete bytes;
			 }
		//----------------------------------------------------
	public: bool Equals(String &str1, String &str2)
			{
				if(str1 == str2)
					return true;
				else 
					return false;
			}
		//----------------------------------------------------
	public: char *get_BaseStream()
		{
		    return this->bytes;
		}
		//----------------------------------------------------
	public: char get_Value(int index)
		{
			if(this->size != 0)
				return this->bytes[index];
			//here must be implemented an exception
		}
		//----------------------------------------------------
	public: int get_Length()
		{
			return size;
		}
		//----------------------------------------------------
		/* Sample using
	      for(int i = 0 ;  ; i++)
	      {
	          String s("         hello world!           ");
	          String ss = s.Trim();
	          printf("%s i:%d\n", ss.get_BaseStream(), i);
              #if defined WIN32 | WIN64
	                 Sleep(1);
              #endif
	      }
		*/
	public: String Trim()
		{
			if(size == 0)
				return String();
			int startIndex = -1, endIndex = -1;
			for(int i = 0 ; i < size ; i++)
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
						if(startIndex != size -1)
						{
							for(int j = size - 1 ; j >= 0; j--)
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
				int len = endIndex - (startIndex + 1);
				char  *newBuffer = new char[len + 1];
				for(int i = startIndex + 1 , j = 0; i < endIndex ; i++, j++)
					newBuffer[j] =  this->bytes[i];
				newBuffer[len] = 0;
				return String(newBuffer);
			}
			if(startIndex >= 0 && endIndex == -1)
			{
				int len = size - (startIndex + 1);
				char *newBuffer = new char[len + 1];
				for(int i = startIndex + 1 , j = 0; i < size ; i++, j++)
					newBuffer[j] =  this->bytes[i];
				newBuffer[len] = 0;
				return String(newBuffer);
			}
			if(startIndex == -1 && endIndex > 0)
			{
				char* newBuffer = new char[endIndex + 1];
				for(int i = 0 , j = 0; i < endIndex ; i++, j++)
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
		public: String Substring(int startIndex, int count)
		{
			if(this->size == 0)
				return String();
			//add here exception for startIndex and count
			char* buffer = new char[count + 1];
			for(int i = 0 ; i < count ; i++)
				buffer[i] = this->get_Value(startIndex + i);
			buffer[count] = 0;
			return String(buffer);
		}
		public: String Substring(int startIndex)
		{
			return this->Substring(startIndex, this->get_Length() - startIndex);
		}
		//----------------------------------------------------
	    /* Sample using		
	       for(int i = 0 ;  ; i++)
	       {
	          String s("hello world!");
	          String ss = String::Copy(s);
	          printf("%s i:%d\n", ss.get_BaseStream(), i);
              #if defined WIN32 | WIN64
	               Sleep(1);
              #endif
	       }
		*/
		public: static String Copy(String &str)
				{
					if(str.get_Length() == 0)
						return String();
					char *buffer = new char[str.get_Length() + 1];
					for(int i = 0 ; i < str.get_Length() ; i++)
						buffer[i] = str.get_Value(i);
					buffer[str.get_Length()] = 0;
					return String(buffer);
				}
		//----------------------------------------------------
	   /* Sample using	
	      for(int i = 0 ;  ; i++)
	      {
	         String s("hello world!");
	         String ss = s.ToUpper();
	         printf("%s i:%d\n", ss->get_BaseStream(), i);
             #if defined WIN32 | WIN64
	             Sleep(1);
             #endif
	      }
		*/
		public: String ToUpper()
		{
			if(this->get_Length() == 0)
				return *this;
			char *buffer = new char[this->get_Length() + 1];
			for(int i = 0 ; i < this->size ; i++)
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
		public: String ToLower()
		{
			if(this->get_Length() == 0)
				return *this;
			char *buffer = new char[this->get_Length() + 1];
			for(int i = 0 ; i < this->get_Length() ; i++)
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
	    /* Sample using
	        for(int i = 0 ;  ; i++)
	        {
	           String s("hello world!");
	           String s1("wo");
	           printf("position: %d\n", s.IndexOf(&s1, 0, s.get_Length()));
               #if defined WIN32 | WIN64
	                 Sleep(1);
               #endif
	        }
	    */
		public: int IndexOf(String &search, int startIndex, int count)
		{
			bool find = false;
			int n = size;
			int m = search.get_Length();
			if(m > n) 
				return -1;
			//add here exception
			/*if(count + startIndex  >= n)
				throw new Exception("Index was out of range.");*/
			int start = 0;
			for(int i = startIndex ; i < startIndex + count ; i++)
			{
				if(this->get_Value(i) == search.get_Value(0))
				{
					if(search.get_Length() == 1)
						return i;
					for(int j = 1 ; j < (int)search.get_Length() ; j++)
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
			else return -1;
		}

		public: int IndexOf(String &search)
		{
			return IndexOf(search, 0);
		}

		public: int IndexOf(String &search, int startIndex)
		{
			return IndexOf(search, startIndex, this->get_Length());
		}
		//----------------------------------------------------
		/* Sample using
		    for(int i = 0 ;  ; i++)
	        {
	           String s("hello world!");
	           String s1("o");
	           printf("position: %d\n", s.LastIndexOf(s1, 0, s.get_Length()));
               #if defined WIN32 | WIN64
	                Sleep(1);
               #endif
	        }
		*/
		public: int LastIndexOf(String &search, int startIndex, int count)
		{
			bool first = false, last = false ;
			int n = this->get_Length();
			int m = search.get_Length();
			if(m > n) 
				return -1;
			//add here exception
			/*if(startIndex >= n)
				throw new Exception("Index was out of range.");*/
			int start = 0, end = 0;
			for(int i = startIndex ; i < startIndex + count ; i++)
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
						for(int j = 1 ; j < search.get_Length() ; j++)
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

		public: int LastIndexOf(String &search)
		{
			return LastIndexOf(search, 0);
		}

		public: int LastIndexOf(String &search, int startIndex)
		{
			return LastIndexOf(search, startIndex, this->get_Length());
		}
		//----------------------------------------------------
	    /* Sample using
			for(int i = 0 ;  ; i++)
			{
	           String s1("hi,");
	           String s2(" how are you?");
	           s2 += s1;
	           printf("%s i:%d\n", s2.get_BaseStream(), i);
               #if defined WIN32 | WIN64
	                  Sleep(1);
               #endif
	        }
		*/
		public: void operator+=(String &str)
		{
			if(str.get_Length() == 0)
				return ;//*this;
			else
			{
			   char *b = new char[this->get_Length() + str.get_Length() + 1];
			   int index = this->get_Length() + (str.get_Length() - this->get_Length());
			   for(int i = 0 ; i < this->get_Length() + str.get_Length() ; i++)
			   {
				  if(i < str.get_Length())
					  b[i] = str.get_Value(i);
				  if(i >= str.get_Length())
					  b[i] = this->get_Value(i - index);
			   }
			   b[this->get_Length() + str.get_Length()] = 0;
			   delete bytes;
			   bytes = b;
			   //return *this;
			}
		}
		//----------------------------------------------------
		public: static String Concat(String &str0, String &str1)
		{
			return str0 + str1;
		}
		//----------------------------------------------------
		/* Sample using
		    char *str;
	        for(int i = 0 ;  ; i++)
	        {
	            String s1("hi,");
	            String s2(" how are you?");
	            String ss = s1 + s2;
	            printf("%s i:%d\n", ss.get_Basestream(), i);
	            delete ss;
                #if defined WIN32 | WIN64
	                  Sleep(1);
                #endif
	        }
		*/
		public: friend String operator+(String &StrA, String &StrB)
		{
			if(StrA.get_Length() == 0)
				return String::Copy(StrB);
			if(StrB.get_Length() == null)
				return String::Copy(StrA);
			char *b = new char[StrA.get_Length() + StrB.get_Length() + 1];
			int index = StrB.get_Length() + (StrA.get_Length() - StrB.get_Length());
			for(int i = 0 ; i < StrA.get_Length() + StrB.get_Length() ; i++)
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
		/* Sample using
	        for(int i = 0 ;  ; i++)
	        {
	           String s1("hi,");
	           String s2("hi,");
	           printf("Compare: %d\n", String::Compare(s1, s2)); 
			   #if defined WIN32 | WIN64
	           Sleep(1);
               #endif
			}
		*/
		public: static int Compare(String &StrA, String &StrB)
		{
			if(StrA.get_Length() == 0 && StrB.get_Length() == 0)
				return 0;
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
					for(int i = 0 ; i < StrA.get_Length() ; i++)
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
		/* Sample using
	        for(int i = 0 ;  ; i++)
	        {
	           String s1("hi,");
	           String s2("hi,");
	           if(s1 == s2)
		          printf("s1 equals s2.\n");
               #if defined WIN32 | WIN64
	                  Sleep(1);
               #endif
	         }
		*/
		public: friend bool operator ==(String &StrA, String &StrB)
		{
			if(String::Compare(StrA, StrB) == 0)
				return true;
			else
				return false;
		}
		//----------------------------------------------------
		/* Sample using
	        for(int i = 0 ;  ; i++)
	        {
	           String s1("hi,");
	           String s2(" how are u?");
	           if(s1 != s2)
		          printf("s1 not equals s2.\n");
               #if defined WIN32 | WIN64
	                  Sleep(1);
               #endif
	         }
	    */
		public: friend bool operator !=(String &StrA, String &StrB)
		{
			if(String::Compare(StrA, StrB) == 0)
				return false;
			else
				return true;
		}
		//----------------------------------------------------
		public:  void operator =(String &str)
		{
			if(this->size != 0)
			{
				delete this->bytes;
				this->bytes = new char[str.get_Length() + 1];
				for(int i = 0 ; i < str.get_Length() ; i++)
					this->bytes[i] = str.get_Value(i);
				this->bytes[str.get_Length()] = 0; 
				return ;
			}
		}
		//----------------------------------------------------
		public: String Replace(String &oldValue, String &newValue)
		{
			ArrayList al;
			int p1 = 0;
			while(true)
			{
				if(p1 < this->get_Length())
				{
					//printf("hi\n");
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
				al.Clear();
				return String::Copy(*this);
			}
			else
			{
				String str("");
				for(int i = 0 ; i < al.get_Count() ; i++)
				{
					if(i == 0)
						str = this->Substring(0, (int)al[i]) + newValue;
					else
						str = str + this->Substring((int)al[i - 1] + oldValue.get_Length(), (int)al[i] - (int)al[i - 1] - oldValue.get_Length()) + newValue;
				}
				//if((int)al[al.get_Count() - 1] + oldValue.get_Length() < this->get_Length())
				//	str = str + this->Substring((int)al[al.get_Count() - 1] + oldValue.get_Length(), this->get_Length());
				//al.Clear();
				//al = null;
				//printf("str: %s\n", str.get_BaseStream());
				//printf("hi%d\n", al.get_Count());
				return str;
			}
		}
		//----------------------------------------------------
	};
};
//**************************************************************************************************************//
