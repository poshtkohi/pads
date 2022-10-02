#ifndef __Parvicursor_xDFS_xDFSClients_UploadClient_h__
#define __Parvicursor_xDFS_xDFSClients_UploadClient_h__


#include "../../../../general.h"
#include "../../../../StaticFunctions/StaticFunctions.h"
#include "../../../../System/BasicTypes/BasicTypes.h"
#include "../../../../System/Object/Object.h"
#include "../../../../System/ArgumentException/ArgumentNullException.h"
#include "../../../../System.Collections/ArrayList/ArrayList.h"
#include "../../../../System.IO/FileStream/FileStream.h"
#include "../../../../System.Net.Sockets/Socket/Socket.h"
#include "../../../../System.Net/Dns/Dns.h"
#include "../../../../System.Net/NetworkCredential/NetworkCredential.h"
#include "../../../../System/Guid/Guid.h"
#include "../../../../System.Threading/Thread/Thread.h"
#include "../../../../System.Threading/Mutex/Mutex.h"
#include "../../../../System.Threading/ConditionVariable/ConditionVariable.h"
#include "../../../../System.IO/IOException/IOException.h"
#include "../../../../Parvicursor/Parvicursor.Net/ParvicursorSocket/ParvicursorSocket.h"
#include "../../../../Parvicursor/Shared/Enums/xDFS/TransferChannelMode.h"
#include "../../../../Parvicursor/Shared/Enums/xDFS/CircularBufferState.h"
#include "../../../../Parvicursor/Serialization/Serializer.h"
#include "../../../../Parvicursor/Serialization/DeSerializer.h"
#include "../../../../Parvicursor/Profiler/ResourceProfiler.h"

//#include <stdio.h>
#if !defined WIN32 || WIN64
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/utsname.h>
#endif

//**************************************************************************************************************//
using namespace System;
using namespace System::Collections;
using namespace System::IO;
using namespace System::Net;
using namespace System::Net::Sockets;
using namespace System::Threading;


using namespace Parvicursor::Net;
using namespace Parvicursor::Serialization;
using namespace Parvicursor::Shared::Enums::xDFS;


namespace Parvicursor
{
   namespace xDFS
   {
	   namespace xDFSClients
	   {
			class UploadClient : public Object
			{
				private: FileStream *fs;
				private: char *seekValue;
				private: char *readValue;
				private: char *buffer;
				private: Int64 offsetSeek;
				private: Int32 k ; // total reads
				private: Int64 n ;
				private: Int64 last_n ;
				private: String guid;
				private: bool secure;
				//private: DateTime *t1;
				//private: DateTime *t2;
				private: Int64 fileSize;
				private: Int32 parallel;
				private: Thread *worker;
				private: Mutex *completion_mutex;
				private: ConditionVariable *completion_cv;
				private: bool completion_met;
				private: Int64 written;
				private: Mutex *written_mutex;
				private: Int32 blockSize;
				private: Int32 tcpBufferSize;
				//private: QueueRead qread;
				private: NetworkCredential *nc;
				////private: ParvicursorSocket socket;
				private: String readFilename;
				private: String remoteFilename;
				private: String xDFSServerAddress;
				private: bool closed;// = false;
				private: bool exited;// = false;
				////private: Int32 timeout = 15 * 1000; // 15s timeout
				//private: RijndaelEncryption rijndael;
				//private: SecureBinaryReader reader;
				//private: SecureBinaryWriter writer;
				private: bool memmoryToMemoryTests;
				private: ArrayList *errors;
				////private: TimeSpan timeout = new TimeSpan(1);
				private: bool ended;// = false;
				//private: Socket *sock;
				private: ParvicursorSocket *socket;
				private: ArrayList *sockets;
				private: fd_set alRead;
				private: fd_set alWrite;
				private: Int32 highsock;
				private: bool alRead_enable;
				private: bool alWrite_enable;

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
				private: bool isClientSideZeroCopyEnabled;
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
				public: UploadClient(const String &readFilename, const String &remoteFilename, Int32 parallel,
							Int32 blockSize, Int32 tcpBufferSize, Int32 circularBufferCountClientSide, Int32 circularBufferServerSide,
							const String &xDFSServerAddress, const NetworkCredential &nc, bool secure/*, ref RijndaelEncryption rijndael*/,
							bool memmoryToMemoryTests, ArrayList *errors, bool IsClientSideZeroCopyEnabled, bool IsServerSideZeroCopyEnabled);

				public: ~UploadClient();
				public: Int64 get_CurrentTransferredBytes();
				public: Int64 get_FileSize();
				//public: const DateTime &get_StartTime();
				//public: const DateTime &get_EndTime();
				private: void WorkerProc();
				public: void Run();
				private: void WorkerExit();
				public: void Close();
				private: inline void ReadFromFile();
				private: inline void ZeroCopyTransfer(ParvicursorSocket *socket);

				private: void SendOneExceptionToAllSocketsAndClose(System::Exception &e);
				private: void SendExceptionToOneSocketAndClose(System::Exception &e, ParvicursorSocket *socket);
				private: inline void MakeReadyReadSocketArrayList();
				private: inline void MakeReadyWriteSocketArrayList();
				private: void RemoveSocketFromArrayList(ParvicursorSocket *socket);
				private: inline Int32 FindSocketIndex(ParvicursorSocket *socket);
				//private void PublicKeyAuthentication(Socket sock);
				//private byte[] AuthenticationHeaderBuilder(string username, string password);
				private: static void *Wrapper_To_Call_WorkerProc(void* pt2Object);
				/*******for disk thread *************/
				private: CircularBufferState buffer_device(void);
				private: CircularBufferState unbuffer_device(ParvicursorSocket *socket);
				private: void DiskThreadWorkerProc_Producer();
				private: static void *Wrapper_To_Call_DiskThreadWorkerProc(void* pt2Object);
				private: void DoScatteredIO_For_CircularBuffer(ParvicursorSocket *socket, char *buffer);
				private: pthread_mutex_t mutex;
				private: Int32 circularBufferCountClientSide;
				private: Int32 circularBufferCountServerSide;
				private: bool circularBufferEnabledClientSide;
				private: Thread *disk_thread;
				private: Int32 blocks;
				private: Int32 start;
				private: Int32 end;
				private: Int32 BUFFER_SIZE;
				private: Int32 _n;
				private: Int32 __n;
				private: Int32 _last_n;
				private: Int64 currentOffset;
				private: Int64 nextOffset;
				private: Int64 _offset;
				private: char *circular_buffer;
				private: char *buffer1; // temprorary buffer for disk thread;
				//----------------------------------------------------

			};
	   };
   };
};
//**************************************************************************************************************//

#endif
