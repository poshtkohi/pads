/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __Parvicursor_xThread_xThreadEvent_h__
#define __Parvicursor_xThread_xThreadEvent_h__

#include "../../System/BasicTypes/BasicTypes.h"

using namespace System;

//**************************************************************************************************************//
namespace Parvicursor
{
	//----------------------------------------------------
   namespace xThread
   {
		//----------------------------------------------------
		enum xThreadEvent
		{
			TerminateSession				= (Byte)0,
			SuspendAllThreads				= (Byte)1,
			ResumeAllThreads				= (Byte)2,
			SuspendOneThread				= (Byte)3,
			ResumeOneThread					= (Byte)4,
			SyncReceiveOneThread			= (Byte)5,
			SyncSendOneThread				= (Byte)6,
			SyncReceiveAllThreads			= (Byte)7,
			SyncSendAllThreads				= (Byte)8,
			AbortOneThread					= (Byte)9,
			AllThreadsCompleted				= (Byte)10,
			AuxiliaryMehtodOneThread        = (Byte)11
		};
		//----------------------------------------------------
	}
}
//**************************************************************************************************************//

#endif
