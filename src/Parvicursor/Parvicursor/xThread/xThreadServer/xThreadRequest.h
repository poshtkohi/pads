#ifndef __Parvicursor_xThread_xThreadRequest_h__
#define __Parvicursor_xThread_xThreadRequest_h__


#include "../../../../stdafx.h"
#include "../../../../StaticFunctions/StaticFunctions.h"

#include "../../../../Parvicursor/Parvicursor.Net/ParvicursorSocket/ParvicursorSocket.h"
#include "../../../../Parvicursor/Shared/Enums/xDFS/TransferChannelMode.h"

//**************************************************************************************************************//

using namespace System;
using namespace System::Collections;
using namespace System::Net;
using namespace System::IO;
using namespace System::Net::Sockets;
using namespace System::Threading;

using namespace Parvicursor::Net;
//using namespace Parvicursor::xDFS;

namespace Parvicursor
{
	//----------------------------------------------------
   namespace xThread
   {
		//----------------------------------------------------
	   class xThreadRequest : public Object
		{
			//----------------------------------------------------
			private: FileStream *fs;
			/*private: Thread *worker;*/
			private: ArrayList *sockets;
			//private: Hashtable *sessions;
			private: int timeout; // 60s timeout
			private: bool secure;
			private: char *buffer;//
			//private: ParvicursorSocket *sock;//
			//private: ArrayList *connections;
			private: fd_set alRead;
			private: int highsock;
			//private: ArrayList *alRead;
			//private: int _Available = 0;
			//private: TimeSpan _timeout = new TimeSpan(1);*/
			//----------------------------------------------------
			public: xThreadRequest(ParvicursorSocket *socket);
			public: void Run();
			public: void AddNewClientStream(ParvicursorSocket *socket);
			private: void RemoveSocketFromArrayList(ParvicursorSocket *socket);
			private: void SendOneExceptionToAllSockets(Exception &e);
			private: void SendExceptionToOneSocketAndClose(Exception &e, ParvicursorSocket *socket);
			private: void MakeReadSocketArrayList();
			private: int FindSocketIndex(ParvicursorSocket *socket);
			private: int WaitForAllConnections();
			private: void WorkerExit();
			private: pthread_mutex_t mutex;
			//----------------------------------------------------
		};
		//----------------------------------------------------
	}
}
//**************************************************************************************************************//

#endif

