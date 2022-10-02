#ifndef __Parvicursor_xDFS_SessionFileStreamRequest_h__
#define __Parvicursor_xDFS_SessionFileStreamRequest_h__


#include "../../../../general.h"
#include "../../../../StaticFunctions/StaticFunctions.h"
#include "../../../../Parvicursor/Shared/Enums/xDFS/TransferChannelMode.h"
#include "../../../../Parvicursor/xDFS/xDFSServer/xDFSServer.h"
#include "../../../../Parvicursor/Parvicursor.Net/ParvicursorSocket/ParvicursorSocket.h"
#include "../../../../../Parvicursor/System.Threading/Thread/Thread.h"
#include "../../../../../Parvicursor/System.IO/IOException/IOException.h"
#include "../../../../Parvicursor/Shared/Enums/xDFS/TransferChannelMode.h"
#include "../../../../Parvicursor/Shared/Enums/xDFS/CircularBufferState.h"
#include "../../../../Parvicursor/Serialization/Serializer.h"
#include "../../../../Parvicursor/Serialization/DeSerializer.h"
#include "../../../../System/BasicTypes/BasicTypes.h"
#include "../../../../System/Object/Object.h"
#include "../../../../System/ArgumentException/ArgumentNullException.h"
#include "../../../../System.Collections/ArrayList/ArrayList.h"
#include "../../../../System.IO/FileStream/FileStream.h"
#include "../../../../System.IO/Directory/Directory.h"
#include "../../../../System.Net.Sockets/Socket/Socket.h"
#include "../../../../System.Net/Dns/Dns.h"
#include "../../../../System.Net/NetworkCredential/NetworkCredential.h"
#include "../../../../System/Guid/Guid.h"
#include "../../../../System/String/String.h"
#include "../../../../Parvicursor/xDFS/FileTransferInfo/FileTransferInfo.h"
#include "../../../../Parvicursor/Profiler/ResourceProfiler.h"
#include "../../../../Parvicursor/Shared/Enums/xDFS/Methods.h"

//**************************************************************************************************************//

using namespace System;
using namespace System::Collections;
using namespace System::Net;
using namespace System::IO;
using namespace System::Net::Sockets;
using namespace System::Threading;

using namespace Parvicursor::Net;
using namespace Parvicursor::xDFS;
using namespace Parvicursor::Shared::Enums::xDFS;

namespace Parvicursor
{
   namespace xDFS
   {
		class FileStreamInfo: public Object
		{
			public: String guid;
			public: String remoteFilename;
			public: FileMode mode;
			public: FileAccess access;
			public: Int32 blockSize;
		};

		class SessionFileStreamRequest : public Object
		{
			/*---------------------fields----------------*/
			private: FileStream *fs;
			private: ParvicursorSocket *socket;
			private: FileStreamInfo *info;
			private: char *buffer;
			private: char *paramBuffer;
			/*---------------------methods----------------*/
			public: SessionFileStreamRequest(ParvicursorSocket *socket, const FileStreamInfo &info);
			public: void Run();
			private: bool OpenFileHandle();
			private: void WorkerExit(); 
			private: void Close();
			private: inline bool Read();
			private: inline bool Write();
			private: inline bool Flush();
			private: inline bool SetLength();
			private: inline bool Lock();
			private: inline bool UnLock();
			private: inline bool Seek();
			private: inline bool get_Position();
			private: inline bool get_Length();
			private: bool SendUnsupportedMethod();
		};
	}
}
//**************************************************************************************************************//

#endif

