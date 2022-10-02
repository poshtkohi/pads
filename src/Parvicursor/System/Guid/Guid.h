/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Guid_h__
#define __System_Guid_h__


#include <stdio.h>
//#include <errno.h>
#include "../../System/Object/Object.h"
//#include "../../System/Exception/Exception.h"
#include "../../System/String/String.h"



#if defined WIN32 || WIN64
#pragma comment(lib, "RpcRT4.lib")
//#include <windows.h>
//#ifndef __RPCDCE_H__
//#include <Rpcdce4.h>
#include <objbase.h>
#include <rpc.h>
//#endif
#else
#include <uuid/uuid.h>
#endif

//


//**************************************************************************************************************//

namespace System
{
		//----------------------------------------------------
		class Guid : public Object
        {
			//----------------------------------------------------
			//----------------------------------------------------
			//----------------------------------------------------
			public: static String NewGuid();
			//----------------------------------------------------
			//----------------------------------------------------
			//----------------------------------------------------
		};
		//----------------------------------------------------
};
//**************************************************************************************************************//

#endif
