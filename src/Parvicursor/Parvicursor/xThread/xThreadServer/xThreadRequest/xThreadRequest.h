#ifndef __Parvicursor_xThread_xThreadRequest_h__
#define __Parvicursor_xThread_xThreadRequest_h__

#include "../../../../general.h"
#include "../../../../StaticFunctions/StaticFunctions.h"
#include "../../../../Parvicursor/Shared/Enums/xDFS/TransferChannelMode.h"
#include "../../../../Parvicursor/xDFS/xDFSServer/xDFSServer.h"
#include "../../../../Parvicursor/Parvicursor.Net/ParvicursorSocket/ParvicursorSocket.h"
#include "../../../../../Parvicursor/System.Threading/Thread/Thread.h"
#include "../../../../../Parvicursor/System.Threading/Mutex/Mutex.h"
#include "../../../../../Parvicursor/System.IO/IOException/IOException.h"
#include "../../../../Parvicursor/Shared/Enums/xDFS/TransferChannelMode.h"
#include "../../../../Parvicursor/Serialization/Serializer.h"
#include "../../../../Parvicursor/Serialization/DeSerializer.h"
#include "../../../../System/BasicTypes/BasicTypes.h"
#include "../../../../System/Object/Object.h"
#include "../../../../System/ArgumentException/ArgumentNullException.h"
#include "../../../../System.Collections/ArrayList/ArrayList.h"
#include "../../../../System.IO/FileStream/FileStream.h"
#include "../../../../System.Net.Sockets/Socket/Socket.h"
#include "../../../../System.Net/NetworkCredential/NetworkCredential.h"
#include "../../../../System/Guid/Guid.h"
#include "../../../../System/String/String.h"
#include "../../../../Parvicursor/xThread/xThreadCollection.h"
#include "../../../../Parvicursor/xThread/xThreadBase.h"
#include "../../../../Parvicursor/xThread/xThreadEvent.h"

#if !defined WIN32 || WIN64
#include <sys/types.h>
#include <dlfcn.h>
#endif

//**************************************************************************************************************//

using namespace System;
using namespace System::Collections;
using namespace System::Net;
using namespace System::IO;
using namespace System::Net::Sockets;
using namespace System::Threading;

using namespace Parvicursor::Net;
using namespace Parvicursor::Serialization;

namespace Parvicursor
{
	//----------------------------------------------------
   namespace xThread
   {
		//----------------------------------------------------
	   class xThreadRequest : public Object
		{
			//----------------------------------------------------
			private: Mutex *mutex;
			private: ArrayList *sockets;
			private: ParvicursorSocket *threadChannel;
			private: ParvicursorSocket * threadControlChannel;
			private: xThreadCollection *collection;
			private: Serializer *serializer;
			private: DeSerializer *deserializer;
			private: Int32 MaxThreadSupport;
			private: String sessionGuid;
			private: int timeout; // 120s timeout
			private: bool secure;
			private: char *buffer;//
			private: Int32 currentBufferSize;//
			private: static const Int32 DefaultBufferSize = 1024; // 1KB
			private: fd_set alRead;
			private: fd_set alWrite;
			private: Int32 highsock;
			/*******for xThread Gathered/Scattered I/O transfers*****/
/*#			if defined WIN32 || WIN64
			private: WSABUF	win_Scattered_IO  [2];
#			else
            private: iovec posix_Scattered_IO [2];
#			endif*/
			/********************************************************/
			//----------------------------------------------------
			public: xThreadRequest(ParvicursorSocket *socket, const String &sessionGuid);
			public: void Run();
			public: void AddNewClientStream(ParvicursorSocket *socket);
			private: void RemoveSocketFromArrayList(ParvicursorSocket *socket);
			private: void SendOneExceptionToAllSockets(System::Exception &e);
			private: void SendExceptionToOneSocketAndClose(System::Exception &e, ParvicursorSocket *socket);
			private: void MakeReadyReadSocketArrayList();
			private: void MakeReadyWriteSocketArrayList();
			private: void MakeReadSocketArrayList();
			private: int FindSocketIndex(ParvicursorSocket *socket);
			private: int WaitForAllConnections();
			private: void WorkerExit();
			//private: pthread_mutex_t mutex;
			//----------------------------------------------------
		};
		//----------------------------------------------------
	}
}
//**************************************************************************************************************//

#endif

