#ifndef __Parvicursor_xDFS_xDFSClients_DownloadClient_h__
#define __Parvicursor_xDFS_xDFSClients_DownloadClient_h__


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
#include "../../../../Parvicursor/Parvicursor.Net/ParvicursorSocket/ParvicursorSocket.h"
#include "../../../../System.Threading/Thread/Thread.h"
#include "../../../../System.Threading/Mutex/Mutex.h"
#include "../../../../System.Threading/ConditionVariable/ConditionVariable.h"
#include "../../../../System.IO/IOException/IOException.h"
#include "../../../../Parvicursor/Shared/Enums/xDFS/TransferChannelMode.h"
#include "../../../../Parvicursor/Serialization/Serializer.h"
#include "../../../../Parvicursor/Serialization/DeSerializer.h"

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
			class DownloadClient : public Object
			{
				private: FileStream *fs;
				private: char *seekValue;
				private: char *readValue;
				private: char *buffer;
				private: Int64 read_val;
				private: Int64 offsetSeek;
				private: Int64 lastOffset;
				private: Int64 lastLength;
				private: Int32 j;  // seek numbers
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
				private: String writeFilename;
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
				private: Int32 highsock;

				/*******for X-xDFS Gathered/Scattered I/O transfers*****/
				private: char one_byte_buf_Scattered_IO [1];

#if defined WIN32 || WIN64
				private: WSABUF	win_Scattered_IO [3];
#else
				private: iovec posix_Scattered_IO [3];
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
				public: DownloadClient(const String &writeFilename, const String &remoteFilename, Int32 parallel, Int32 blockSize, Int32 tcpBufferSize, const String &xDFSServerAddress, const NetworkCredential &nc, bool secure/*, ref RijndaelEncryption rijndael*/, bool memmoryToMemoryTests, ArrayList *errors, bool IsClientSideZeroCopyEnabled, bool IsServerSideZeroCopyEnabled);
				public: ~DownloadClient();
				public: Int64 get_CurrentTransferredBytes();
				public: Int64 get_FileSize();
				//public: const DateTime &get_StartTime();
				//public: const DateTime &get_EndTime();
				private: void WorkerProc();
				public: void Run();
				private: void WorkerExit();
				public: void Close();
				private: inline bool ReadFileBlockFTSMMode(ParvicursorSocket *socket);
				private: inline void WriteToFile();
				private: inline bool ZeroCopyTransfer(ParvicursorSocket *socket);

				private: void RemoveSocketFromArrayList(ParvicursorSocket *socket);
				private: void SendOneExceptionToAllSockets(System::Exception &e);
				private: void SendExceptionToOneSocketAndClose(System::Exception &e, ParvicursorSocket *socket);
				private: inline void MakeReadSocketArrayList();
				private: inline Int32 FindSocketIndex(ParvicursorSocket *socket);

				//private void PublicKeyAuthentication(Socket sock);
				//private byte[] AuthenticationHeaderBuilder(string username, string password);
				private: static void *Wrapper_To_Call_WorkerProc(void* pt2Object);
				//----------------------------------------------------

			};
	   };
   };
};
//**************************************************************************************************************//

#endif
