#ifndef __Parvicursor_xDFS_xDFSServer_h__
#define __Parvicursor_xDFS_xDFSServer_h__


#include "../../../general.h"
#include "../../../../Parvicursor/System.Threading/Thread/Thread.h"
#include "../../../System/BasicTypes/BasicTypes.h"
#include "../../../System/Object/Object.h"
#include "../../../System/DateTime/DateTime.h"
#include "../../../System.Collections/Hashtable/Hashtable.h"
#include "../../../System.Net.Sockets/Socket/Socket.h"
#include "../../../System/String/String.h"
#include "../../../../Parvicursor/Parvicursor/xDFS/xDFSServer/MainServer/Server.h"

//**************************************************************************************************************//

using namespace System;
using namespace System::Collections;
using namespace System::Net;
using namespace System::Net::Sockets;
using namespace System::Threading;

namespace Parvicursor
{
	//----------------------------------------------------
   namespace xDFS
   {
		//----------------------------------------------------
		class xDFSServer : public Object
		{
			//private: Hashtable *sessions;
			//private: ArrayList *connections;
			//private: RSA ServerRSA;
			private: System::Net::Sockets::Socket *sock;
			private: Thread *worker;
			private: bool closed;// = false;
			//----------------------------------------------------
			public: xDFSServer();
			public: void Start();
			private: void WorkerProc();
			public: void Close();
			private: static void *Wrapper_To_Call_WorkerProc(void* pt2Object);
			//----------------------------------------------------
		};
		//----------------------------------------------------
	}
}
//**************************************************************************************************************//

#endif

