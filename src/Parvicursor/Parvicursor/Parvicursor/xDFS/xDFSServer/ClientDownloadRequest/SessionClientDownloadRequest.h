#ifndef __Parvicursor_xDFS_ClientDownloadRequest_h__
#define __Parvicursor_xDFS_ClientDownloadRequest_h__


#include "../../../../general.h"
#include "../../../../StaticFunctions/StaticFunctions.h"
#include "../../../../Parvicursor/Shared/Enums/xDFS/TransferChannelMode.h"
#include "../../../../Parvicursor/xDFS/xDFSServer/xDFSServer.h"
#include "../../../../Parvicursor/Parvicursor.Net/ParvicursorSocket/ParvicursorSocket.h"
#include "../../../../Parvicursor/Shared/Enums/xDFS/TransferChannelMode.h"
#include "../../../../Parvicursor/Serialization/Serializer.h"
#include "../../../../Parvicursor/Serialization/DeSerializer.h"
#include "../../../../System/BasicTypes/BasicTypes.h"
#include "../../../../System/Object/Object.h"
#include "../../../../System/ArgumentException/ArgumentNullException.h"
#include "../../../../System.Collections/ArrayList/ArrayList.h"
#include "../../../../System.IO/FileStream/FileStream.h"
#include "../../../../System.Net.Sockets/Socket/Socket.h"
#include "../../../../System.Net/Dns/Dns.h"
#include "../../../../System.Net/NetworkCredential/NetworkCredential.h"
#include "../../../../System/Guid/Guid.h"
#include "../../../../System/String/String.h"
#include "../../../../System.Threading/Thread/Thread.h"
#include "../../../../System.IO/IOException/IOException.h"
#include "../../../../Parvicursor/xDFS/FileTransferInfo/FileTransferInfo.h"
#include "../../../../Parvicursor/Profiler/ResourceProfiler.h"


#if !defined WIN32 || WIN64
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/uio.h>
#endif

//**************************************************************************************************************//

using namespace System;
using namespace System::Collections;
using namespace System::Net;
using namespace System::IO;
using namespace System::Net::Sockets;
using namespace System::Threading;

using namespace Parvicursor::Net;
using namespace Parvicursor::xDFS;

namespace Parvicursor
{
	//----------------------------------------------------
   namespace xDFS
   {
		//----------------------------------------------------
	   class SessionClientDownloadRequest : public Object
		{
			//----------------------------------------------------
			private: FileStream *fs;
			/*private: Thread *worker;*/
			private: Int64 written;
			private: ArrayList *sockets;
			//private: Hashtable *sessions;
			private: FileTransferInfo info;
			private: int timeout; // 60s timeout
            private: Int64 seekValue;
			private: Int64 readValue;
			private: int k ; // total reads
			private: bool secure;
			private: bool memmoryToMemoryTests;
			private: char *buffer;//
			private: char *val1;//seekValue
			private: char *val2;//readValue
			//private: ParvicursorSocket *sock;//
			private: Int64 n; // readValue
			private: Int64 last_n ;
			//private: ArrayList *connections;
			private: fd_set alRead;
			private: fd_set alWrite;
			private: Int32 highsock;
			private: bool alRead_enable;
			private: bool alWrite_enable;
			//private: ArrayList *alRead;
			//private: int _Available = 0;
			//private: TimeSpan _timeout = new TimeSpan(1);

 			/*******for X-xDFS Gathered/Scattered I/O transfers*****/
			private: char one_byte_buf_Scattered_IO [1];

#if defined WIN32 || WIN64
			private: WSABUF	win_Scattered_IO [4];
#else
            private: iovec posix_Scattered_IO [4];
#endif
			private: void DoScatteredIO(ParvicursorSocket *socket, char *buffer);
			private: void DoScatteredIO(ParvicursorSocket *socket);
			/************************************************/

			/*******for X-xDFS zero-copy transfers*********/
			private: bool isServerSideZeroCopyEnabled;
			private: bool isNormalZeroCopySupported;
			private: Int32 pageSize;
#if defined WIN32 || WIN64
			private: HANDLE win_file_handle;
			private: HANDLE win_map_handle;
			private: char *win_map_buffer;
#else
            private: char *posix_map_buffer;
            private: int pipes_disk_to_socket [2];
            private: bool pipes_disk_to_socket_is_defined;
#endif
            /***********************************************/


			//----------------------------------------------------
			public: SessionClientDownloadRequest(ParvicursorSocket *socket/*, Thread* worker*/, const FileTransferInfo &transferInfo);
			public: void Run();
			private: inline void ReadFromFile();
			private: inline void ZeroCopyTransfer(ParvicursorSocket *socket);
			//private: int ReadFromOriginialSocket(ParvicursorSocket *socket, char array[], int offset, int count);
			public: void AddNewClientStream(ParvicursorSocket *socket);
			private: void WriteNoException(ParvicursorSocket *socket);
			private: void SendOneExceptionToAllSockets(System::Exception &e);
			private: void SendExceptionToOneSocketAndClose(System::Exception &e, ParvicursorSocket *socket);
			private: inline void MakeReadyReadSocketArrayList();
			private: inline void MakeReadyWriteSocketArrayList();
			private: void RemoveSocketFromArrayList(ParvicursorSocket *socket);
			private: int OpenFileHandle();
			private: inline int FindSocketIndex(ParvicursorSocket *socket);
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

