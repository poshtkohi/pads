/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Type_h__
#define __System_Type_h__

#include "../String/String.h"

#define null 0
//**************************************************************************************************************//
using namespace System;

namespace System
{
	class Type
	{
		String _Namespace; //For example: System
		String _Name; //For Example: String
		//String _AssemblyQualifiedName; //For Example: System.String, mscorlib, Version=1.0.5000.0
		//String _FullName; //For Example: System.String
		String _Version; //For Example: 1.0.5000.0
		String _LibraryName;
		public: Type();
		public: Type(const String &Namespace, const String &Name, const String &LibraryName, const String &Version);
		public: ~Type();
	    public: String get_Namespace();
		public: String get_Name();
		public: String get_AssemblyQualifiedName();
		public: String get_FullName();
	};
};
//**************************************************************************************************************//

#endif
