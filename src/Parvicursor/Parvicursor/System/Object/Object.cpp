/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include  "Object.h"

//**************************************************************************************************************//
namespace System
{
    bool Object::Equals (Object o)
	{
		if(this == &o)
			return true;
		else
			return false;
	}
    //----------------------------------------------------
	static bool Equals (Object a, Object b)
	{
		if(&a == &b)
			return true;

		if (&a == null || &b == null)
			return false;

		return a.Equals (b);
	}
    //----------------------------------------------------
	Object::Object ()
	{
	}
    //----------------------------------------------------
	Object::~Object ()
	{
		//delete obj;
	}
    //----------------------------------------------------
		//public extern Type GetType ();

		// <summary>
		//   Returns a stringified representation of the Object.
		//   This is not supposed to be used for user presentation,
		//   use Format() for that and IFormattable.
		//
		//   ToString is mostly used for debugging purposes.
		// </summary>
		/*public virtual string ToString ()
		{
			return GetType().FullName;
		}*/
};
//**************************************************************************************************************//
