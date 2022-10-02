/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Asynch_h__
#define __System_Asynch_h__

#include "../Object/Object.h"
#include "../BasicTypes/BasicTypes.h"

//**************************************************************************************************************//

namespace System
{
	//----------------------------------------------------
	class IAsyncResult : public Object // An interface class in a general sense - pure virtual methods
	{
		public: Object *AsyncState; // "state" object passed to Begin.
		//WaitHandle AsyncWaitHandle; // Signaled when complete. // cv
		//public: Mutex *AsyncWaitHandle_mutex;
		//public: ConditionVariable *AsyncWaitHandle_cv;
		public: bool CompletedSynchronously; // Did it complete on BeginX?
		public: bool IsCompleted; // Has it completed yet?
	};

	typedef void (*AsyncCallback)(IAsyncResult *ar);
	//----------------------------------------------------
};
//**************************************************************************************************************//

#endif
