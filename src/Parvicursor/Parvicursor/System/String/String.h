
/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/
#ifndef __System_String_h__
#define __System_String_h__

#include "../Object/Object.h"
#include "../BasicTypes/BasicTypes.h"


//**************************************************************************************************************//
namespace System
{
	class String : public Object
	{
		//----------------------------------------------------
		private: char *bytes;
		private: Int32 size;
	    private: bool fromReplace;
		private: bool fromBuffer;
		//----------------------------------------------------
		public: String();
		public: String(const String & str);
		//----------------------------------------------------
		public: String(const char *buffer);
		//----------------------------------------------------
		public: String(char *buffer);
		//----------------------------------------------------
		public: ~String();// attend on this method
		//----------------------------------------------------
		public: bool Equals(String &str1, String &str2);
        //----------------------------------------------------
		public: char *get_BaseStream() const;
		//----------------------------------------------------
		public: char get_Value(Int32 index) const;
		//----------------------------------------------------
		public: Int32 get_Length() const;
		//----------------------------------------------------
		/* Sample using
			for(Int32 i = 0 ;  ; i++)
			{
				String s("         hello world!           ");
				String ss = s.Trim();
				printf("%s i:%d\n", ss.get_BaseStream(), i);
				#if defined WIN32 | WIN64
						Sleep(1);
				#endif
			}
		*/
		public: String Trim();
		//----------------------------------------------------
		public: String Substring(Int32 startIndex, Int32 count);
		//----------------------------------------------------
		public: String Substring(Int32 startIndex);
		//----------------------------------------------------
		/* Sample using
			for(Int32 i = 0 ;  ; i++)
			{
				String s("hello world!");
				String ss = String::Copy(s);
				printf("%s i:%d\n", ss.get_BaseStream(), i);
				#if defined WIN32 | WIN64
					Sleep(1);
				#endif
			}
		*/
		public: static String Copy(const String &str);
		//----------------------------------------------------
		/* Sample using
			for(Int32 i = 0 ;  ; i++)
			{
				String s("hello world!");
				String ss = s.ToUpper();
				printf("%s i:%d\n", ss->get_BaseStream(), i);
				#if defined WIN32 | WIN64
					Sleep(1);
				#endif
			}
		*/
		public: String ToUpper();
		//----------------------------------------------------
		public: String ToLower();
		//----------------------------------------------------
		/* Sample using
			for(Int32 i = 0 ;  ; i++)
			{
				String s("hello world!");
				String s1("wo");
				printf("position: %d\n", s.IndexOf(&s1, 0, s.get_Length()));
				#if defined WIN32 | WIN64
						Sleep(1);
				#endif
			}
		*/
		public: Int32 IndexOf(const String &search, Int32 startIndex, Int32 count);
		//----------------------------------------------------
		public: Int32 IndexOf(const String &search);
		//----------------------------------------------------
		public: Int32 IndexOf(const String &search, Int32 startIndex);
		//----------------------------------------------------
		/* Sample using
			for(Int32 i = 0 ;  ; i++)
			{
				String s("hello world!");
				String s1("o");
				printf("position: %d\n", s.LastIndexOf(s1, 0, s.get_Length()));
				#if defined WIN32 | WIN64
					Sleep(1);
				#endif
			}
		*/
		public: Int32 LastIndexOf(const String &search, Int32 startIndex, Int32 count);
		//----------------------------------------------------
		public: Int32 LastIndexOf(const String &search);
		//----------------------------------------------------
		public: Int32 LastIndexOf(const String &search, Int32 startIndex);
		/* Sample using
			for(Int32 i = 0 ;  ; i++)
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
		//----------------------------------------------------
		public: const String &operator+=(const String &str);
		public: const String &operator+=(const char *str);
		//----------------------------------------------------
		public: static String Concat(const String &str0, const String &str1);
		//----------------------------------------------------
		/* Sample using
			for(Int32 i = 0 ;  ; i++)
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
		public: friend String operator+(const String &StrA, const String &StrB);
		public: friend String operator+(const String &StrA, const char *StrB);
		public: friend String operator+(const char *StrA, const String &StrB);
		//----------------------------------------------------
		public: const String &operator =(const String & str);
		//----------------------------------------------------
		/* Sample using
			for(Int32 i = 0 ;  ; i++)
			{
				String s1("hi,");
				String s2("hi,");
				printf("Compare: %d\n", String::Compare(s1, s2));
				#if defined WIN32 | WIN64
				Sleep(1);
				#endif
			}
		*/
		public: static Int32 Compare(const String &StrA, const String &StrB);
		//----------------------------------------------------
		/* Sample using
			for(Int32 i = 0 ;  ; i++)
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
		public: friend bool operator ==(const String &StrA, const String &StrB)
		{
			if(String::Compare(StrA, StrB) == 0)
			   return true;
		    else
			   return false;
		}
		//----------------------------------------------------
		/* Sample using
			for(Int32 i = 0 ;  ; i++)
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
		public: friend bool operator !=(const String &StrA, const String &StrB)
		{
			if(String::Compare(StrA, StrB) == 0)
			   return false;
		    else
			   return true;
		}
		//----------------------------------------------------
		public: String Replace(const String &oldValue, const String &newValue);
		//----------------------------------------------------
	};
}
//**************************************************************************************************************//

#endif
