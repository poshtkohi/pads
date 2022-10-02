/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Object_h__
#define __System_Object_h__

#include "../BasicTypes/BasicTypes.h"

//**************************************************************************************************************//
namespace System
{
    class Object
	{
		//public: void * obj;
		//----------------------------------------------------
			// <summary>
			//   Compares this Object to the specified Object.
			//   Returns true if they are equal, false otherwise.
			// </summary>
		public: virtual bool Equals (Object o);
		//----------------------------------------------------
			// <summary>
			//   Compares two objects for equality
			// </summary>
		public: static bool Equals (Object a, Object b);
		//----------------------------------------------------
			// <summary>
			//   Initializes a new instance of the Object class.
			// </summary>
		public: Object ();
		//----------------------------------------------------
			// <summary>
			//   Object destructor.
			// </summary>
		public: ~Object ();
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
};
//**************************************************************************************************************//

#endif
