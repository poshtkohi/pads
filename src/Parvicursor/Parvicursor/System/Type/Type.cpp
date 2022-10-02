/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "../String/String.h"
#include  "Type.h"

//**************************************************************************************************************//

namespace System
{
	//----------------------------------------------------
	Type::Type()
	{
	}
	//----------------------------------------------------
	Type::Type(const String &Namespace, const String &Name, const String &LibraryName, const String &Version)
	{
		this->_LibraryName = LibraryName;
		this->_Namespace = Namespace;
		this->_Name = Name;
		this->_Version = Version;
	}
	//----------------------------------------------------
	Type::~Type()
	{
	}
	//----------------------------------------------------
	String Type::get_Namespace()
	{
		return this->_Namespace;
	}
	//----------------------------------------------------
	String Type::get_Name()
	{
		return this->_Name;
	}
	//----------------------------------------------------
	String Type::get_AssemblyQualifiedName()
	{
		return this->get_FullName() + ", " + this->_LibraryName + ", Version=" + this->_Version;
	}
	//----------------------------------------------------
	String Type::get_FullName()
	{
		return this->_Namespace + "." + this->_Name;
	}
	//----------------------------------------------------
};
//**************************************************************************************************************//

