#ifndef __Parvicursor_HybirdConcurrencyPattern_HcpManager_h__
#define __Parvicursor_HybirdConcurrencyPattern_HcpManager_h__

#include "../../System/BasicTypes/BasicTypes.h"
#include "../../System/Object/Object.h"
#include "../../System/ArgumentException/ArgumentOutOfRangeException.h"
#include "../../System/ArgumentException/ArgumentNullException.h"
#include "../../System/Convert/Convert.h"
#include "../../../Parvicursor/System.Threading/Thread/Thread.h"
#include "../../System.Net.Sockets/Socket/Socket.h"

#include "../../Parvicursor/HybirdConcurrencyPattern/ClientContext.h"

//#include <memory.h>
//#include <string.h>
//#include <stdio.h>


//**************************************************************************************************************//

using namespace System;
using namespace System::Threading;


namespace Parvicursor
{
	//----------------------------------------------------
   namespace HybirdConcurrencyPattern
   {
		//----------------------------------------------------
		class HcpManager : public Object
		{
			private: static const Int32 MaxSupportedBufferSize = 100 * 1024 * 1024; //100MB
			private: static const Int32 InitialQueueSize = 100;
			private: Thread *worker_NetworkIoDispatcher;
			private: Thread *worker_master;
			private: static ArrayList *contexts_lifo_queue;
			private: Int32 contexts_lifo_queue_index;
			private: static pthread_mutex_t  _contexts_lifo_mutex_lock_;
			private: Socket *AcceptSocket;
			private: UInt16 listenPort;
			//----------------------------------------------------
			public: HcpManager(UInt16 listenPort);
			public: ~HcpManager();
			public: void WorkerProc_NetworkIoDispatcher();
			public: void WorkerProc_master();
			private: static void *Wrapper_To_Call_WorkerProc_NetworkIoDispatcher(void* pt2Object);
			private: static void *Wrapper_To_Call_WorkerProc_master(void* pt2Object);
			private: static void InitilizeLifoQueue(Out ArrayList **al, UInt32 initialCapacity);
			private: void CloseConnection(In ClientContext *context);
#if defined WIN32 || WIN64
			private: void InitializeIOCP();
			private: bool AssociateWithIOCP(In ClientContext *context);
			private: HANDLE g_hIOCompletionPort;
			private: WSAEVENT g_hAcceptEvent;
			private: HANDLE g_hShutdownEvent;
					 //Time out interval for wait calls
			private: static const Int32 WAIT_TIMEOUT_INTERVAL = 100;
#else
#endif
			//----------------------------------------------------
		}; 
		//----------------------------------------------------
	}
}
//ArrayList *Parvicursor::HybirdConcurrencyPattern::HcpManager::contexts_lifo_queue = new ArrayList();
//pthread_mutex_t Parvicursor::HybirdConcurrencyPattern::HcpManager::_contexts_lifo_mutex_lock_ = PTHREAD_MUTEX_INITIALIZER;
//**************************************************************************************************************//

#endif

