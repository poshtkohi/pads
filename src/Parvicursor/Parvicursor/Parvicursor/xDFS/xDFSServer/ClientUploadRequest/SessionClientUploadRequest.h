#ifndef __Parvicursor_xDFS_ClientUploadRequest_h__
#define __Parvicursor_xDFS_ClientUploadRequest_h__


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


#if !defined WIN32 || WIN64
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/utsname.h>
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
using namespace Parvicursor::Shared::Enums::xDFS;

namespace Parvicursor
{
	//----------------------------------------------------
   namespace xDFS
   {
		//----------------------------------------------------
	   class SessionClientUploadRequest : public Object
		{
			//----------------------------------------------------
			private: FileStream *fs;
			/*private: Thread *worker;*/
			private: Int64 written;
			private: ArrayList *sockets;
			//private: Hashtable *sessions;
			private: FileTransferInfo info;
			private: int timeout; // 60s timeout
			private: Int64 lastOffset;
			private: Int64 lastLength;
            private: Int64 seekValue;
			private: Int64 readValue;
			private: int j;  // seek numbers
			private: int k ; // total writes
			private: bool secure;
			private: bool memmoryToMemoryTests;
			private: char *buffer;//
			private: char *val1;//
			private: char *val2;//
			//private: ParvicursorSocket *sock;//
			private: Int64 n; //
			//private: ArrayList *connections;
			private: fd_set alRead;
			private: int highsock;
			//private: ArrayList *alRead;
			//private: int _Available = 0;
			//private: TimeSpan _timeout = new TimeSpan(1);*/

 			/*******for X-xDFS Gathered/Scattered I/O transfers*****/
			private: char one_byte_buf_Scattered_IO [1];

#if defined WIN32 || WIN64
			private: WSABUF	win_Scattered_IO [3];
#else
            private: iovec posix_Scattered_IO [3];
#endif
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
            private: int pipes_socket_to_disk [2];
            private: bool pipes_socket_to_disk_is_defined;
#endif
            /***********************************************/


			//----------------------------------------------------
			public: SessionClientUploadRequest(ParvicursorSocket *socket/*, Thread* worker*/, const FileTransferInfo &transferInfo);
			public: void Run();
			private: inline void WriteToFile();
			private: inline bool ReadFileBlockFTSMMode(ParvicursorSocket *socket);
			private: inline bool ZeroCopyTransfer(ParvicursorSocket *socket);
			//private: int ReadFromOriginialSocket(ParvicursorSocket *socket, char array[], int offset, int count);
			public: void AddNewClientStream(ParvicursorSocket *socket);
			private: void WriteNoException(ParvicursorSocket *socket);
			private: void RemoveSocketFromArrayList(ParvicursorSocket *socket);
			private: void SendOneExceptionToAllSockets(System::Exception &e);
			private: void SendExceptionToOneSocketAndClose(System::Exception &e, ParvicursorSocket *socket);
			private: inline void MakeReadSocketArrayList();
			private: int OpenFileHandle();
			private: inline int FindSocketIndex(ParvicursorSocket *socket);
			private: int WaitForAllConnections();
			private: void WorkerExit();
 			/*******for disk thread *************/
			private: CircularBufferState buffer_device(void);
			private: CircularBufferState unbuffer_device(void);
			private: void DiskThreadWorkerProc_Consumer();
			private: static void *Wrapper_To_Call_DiskThreadWorkerProc(void* pt2Object);
			private: pthread_mutex_t mutex;
			private: Int32 circularBufferCount;
			private: bool circularBufferEnabled;
			private: Thread *disk_thread;
			private: Int32 blocks;
			private: Int32 start;
			private: Int32 end;
			private: Int32 BUFFER_SIZE;
			//private: Int32 _n;
			//private: Int32 __n;
			//private: Int32 _last_n;
			//private: Int64 currentOffset;
			private: Int64 nextOffset;
			//private: Int64 _offset;
			private: char *circular_buffer;
			private: char *buffer1; // temprorary buffer for disk thread;
			private: const static Int32 xFTSM_HeaderLength = sizeof(Int64) + sizeof(Int64); // offset+count
			//----------------------------------------------------
		};
		//----------------------------------------------------
	}
}
//**************************************************************************************************************//

#endif

