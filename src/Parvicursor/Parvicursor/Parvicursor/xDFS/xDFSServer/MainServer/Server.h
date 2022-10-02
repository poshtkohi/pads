#ifndef __Parvicursor_xDFS_Server_h__
#define __Parvicursor_xDFS_Server_h__

#include "../../../../general.h"
#include "../../../../StaticFunctions/StaticFunctions.h"
#include "../../../../Parvicursor/Shared/Enums/xDFS/TransferChannelMode.h"
#include "../../../../Parvicursor/xDFS/xDFSServer/xDFSServer.h"
#include "../../../../Parvicursor/Parvicursor.Net/ParvicursorSocket/ParvicursorSocket.h"
#include "../../../../../Parvicursor/System.Threading/Thread/Thread.h"
#include "../../../../../Parvicursor/System.IO/IOException/IOException.h"
#include "../../../../../Parvicursor/System/Exception/Exception.h"
#include "../../../../Parvicursor/Shared/Enums/xDFS/TransferChannelMode.h"
#include "../../../../Parvicursor/Serialization/Serializer.h"
#include "../../../../Parvicursor/Serialization/DeSerializer.h"
#include "../../../../System/BasicTypes/BasicTypes.h"
#include "../../../../System/Object/Object.h"
#include "../../../../System/ArgumentException/ArgumentNullException.h"
#include "../../../../System.Collections/ArrayList/ArrayList.h"
#include "../../../../System.Net.Sockets/Socket/Socket.h"
#include "../../../../System.Net/NetworkCredential/NetworkCredential.h"
#include "../../../../System/Guid/Guid.h"
#include "../../../../System/String/String.h"
#include "../../../../Parvicursor/xThread/xThreadCollection.h"
#include "../../../../Parvicursor/xThread/xThreadBase.h"
#include "../../../../Parvicursor/xDFS/xDFSServer/FileStreamRequest/SessionFileStreamRequest.h"
#include "../../../../Parvicursor/xDFS/xDFSServer/ClientUploadRequest/SessionClientUploadRequest.h"
#include "../../../../Parvicursor/xDFS/xDFSServer/ClientDownloadRequest/SessionClientDownloadRequest.h"
#include "../../../../Parvicursor/xThread/xThreadServer/xThreadRequest/xThreadRequest.h"

#include <stdio.h>

//**************************************************************************************************************//
using namespace Parvicursor::Net;

using namespace System;
using namespace System::Collections;
using namespace System::Net;
using namespace System::Net::Sockets;
using namespace System::Threading;

using namespace Parvicursor::Serialization;
using namespace Parvicursor::Shared::Enums::xDFS;
using namespace Parvicursor::xDFS;
using namespace Parvicursor::xThread;


static Hashtable *sessions = new Hashtable();

namespace Parvicursor
{
	//----------------------------------------------------
   namespace xDFS
   {
		//----------------------------------------------------
	   class Server : public Object
		{
			public: Object *serverInstance;
			//private: RSA rsa;
			private: ParvicursorSocket *socket;//private: ParvicursorSocket socket;
			//private: SecureBinaryReader reader;
			//private: SecureBinaryWriter writer;
			private: NetworkCredential *nc;//= new NetworkCredential("alireza", "furnaces2002");////
			//----------------------------------------------------
			public: Server(Socket *sock/*,ref RSA ServerRSA*/);
			public: void Run();
			private: void ProtocolInterpreter();
			//private: FileTransferInfo GetFileTransferInfo();
			private: bool PublicKeyAuthentication(Socket sock);
			private: void ThreadExit();
			private: static void *Wrapper_To_Call_ProtocolInterpreter(void* pt2Object);
			//----------------------------------------------------

		};
		//----------------------------------------------------
	}
}
//**************************************************************************************************************//

#endif

