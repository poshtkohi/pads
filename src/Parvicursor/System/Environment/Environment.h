/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Environment_h__
#define __System_Environment_h__


#include "../String/String.h"
#include "../BasicTypes/BasicTypes.h"


#if defined WIN32 || WIN64
#include <windows.h>
#elif MACOS
#include <sys/param.h>
#include <sys/sysctl.h>
#else
#include <unistd.h>
#endif



//**************************************************************************************************************//

namespace System
{
	//----------------------------------------------------
	class Environment : public Object
    {
		//----------------------------------------------------
		public: static Int32 get_ProcessorCount();
		//----------------------------------------------------
	};
	//----------------------------------------------------
};
//**************************************************************************************************************//

#endif
