/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __Parvicursor_xThread_xThreadClient_h__
#define __Parvicursor_xThread_xThreadClient_h__

#include "../../../general.h"
#include "../../../System/BasicTypes/BasicTypes.h"
#include "../../../System/Object/Object.h"
#include "../../../System/Guid/Guid.h"
#include "../../../System/ArgumentException/ArgumentNullException.h"
#include "../../../System.Collections/ArrayList/ArrayList.h"
#include "../../../System.IO/FileStream/FileStream.h"
#include "../../../System.Net.Sockets/Socket/Socket.h"
#include "../../../System.Net/Dns/Dns.h"
#include "../../../System.Net/NetworkCredential/NetworkCredential.h"
#include "../../../StaticFunctions/StaticFunctions.h"
#include "../../../Parvicursor/Parvicursor.Net/ParvicursorSocket/ParvicursorSocket.h"
#include "../../../../Parvicursor/System.Threading/Thread/Thread.h"
#include "../../../../Parvicursor/System.Threading/Mutex/Mutex.h"
#include "../../../../Parvicursor/System.Threading/ConditionVariable/ConditionVariable.h"
#include "../../../../Parvicursor/System.Threading/ThreadStateException/ThreadStateException.h"
#include "../../../../Parvicursor/System.IO/IOException/IOException.h"
#include "../../../Parvicursor/Shared/Enums/xDFS/TransferChannelMode.h"
#include "../../../Parvicursor/Serialization/Serializer.h"
#include "../../../Parvicursor/Serialization/DeSerializer.h"
#include "../../../../Parvicursor/Parvicursor/xThread/xThreadCollection.h"
#include "../../../../Parvicursor/Parvicursor/xThread/xThreadEvent.h"

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
   namespace xThread
   {
		class xThreadClient : public Object
		{
			private: FileStream *fs;
			private: String guid;
			private: bool secure;
			//private: Thread *worker;
			private: pthread_t thread;
			//private: QueueRead qread;
			private: NetworkCredential *nc;
			private: bool closed;
			private: bool isClosedByRuntimeManager;
			private: bool disposed;
			private: bool sessionInitiated;
			private: bool exited;// = false;
			////private: Int32 timeout = 15 * 1000; // 15s timeout
			//private: RijndaelEncryption rijndael;
			//private: SecureBinaryReader reader;
			//private: SecureBinaryWriter writer;
			private: ArrayList *errors;
			////private: TimeSpan timeout = new TimeSpan(1);
			private: bool ended;// = false;
			//private: Socket *sock;
			private: ParvicursorSocket *socket;
			private: ArrayList *sockets;
			private: ParvicursorSocket *threadChannel;
			private: ParvicursorSocket *threadControlChannel;
			private: fd_set alRead;
			private: fd_set alWrite;
			private: Int32 highsock;
			private: bool alRead_enable;
			private: bool alWrite_enable;
			private: char *buffer;
			private: Int32 currentBufferSize;//
			private: static const Int32 DefaultBufferSize = 1024; // 1KB
			private: String xThreadServerAddress;
			private: bool _IsRemoteStarted;
			private: xThreadCollection *collection;
			private: Serializer *serializer;
			private: DeSerializer *deserializer;
			private: Mutex *mutex;
			private: ConditionVariable *cv;

			/*******for xThread Gathered/Scattered I/O transfers*****/
/*#			if defined WIN32 || WIN64
			private: WSABUF	win_Scattered_IO [2]; //
#			else
            private: iovec posix_Scattered_IO [2];
#			endif*/
			/********************************************************/
			//----------------------------------------------------
			public: xThreadClient(xThreadCollection *collection, const String &xThreadServerAddress, NetworkCredential *nc, bool secure/*, ref RijndaelEncryption rijndael*/, ArrayList *errors);
			public: ~xThreadClient();
			//public: const DateTime &get_StartTime();
			//public: const DateTime &get_EndTime();
			private: void WorkerProc();
			// Asynchronously runs xThreadClient instance via creating a detached thread.
			public: void Run();
			// Waits until the initiation of remote xThread session on the server.
			public: void WaitForSessionInitiation();
			private: void WorkerExit();
			// Closes the xThread session and terminates the execution of remote threads.
			private: void Close();
			// Gets a value indicating the execution status of the current xThread session.
			public: bool get_IsAlive();

			//--- Remote xThread Operations---
			// Either suspends all of the remote threads, or if the remote threads are already suspended, has no effect.
			public: void SuspendAllThreads();
			// Resumes all of the remote threads that have been suspended.
			public: void ResumeAllThreads();
			// Either suspends the remote thread specified by the index, or if the remote thread is already suspended, has no effect.
			public: void SuspendOneThread(Int32 index);
			// Resumes a remote thread  specified by the index that has been suspended.
			public: void ResumeOneThread(Int32 index);
			// Performs a checkpoint operation over the remote thread specified by the index, and synchronizes the data of the local xThread instance (in client) with its remote instance (in server).
			public: void SyncReceiveOneThread(Int32 index);
			// Performs a restore operation over the remote thread specified by the index, and synchronizes the data of the remote xThread instance (in server) with its local instance (in client).
			public: void SyncSendOneThread(Int32 index);
			// Causes a remote thread specified by the index to begin the process of terminating the thread. Calling this method usually terminates the remote thread.
			public: void AbortOneThread(Int32 index);
			// Implements the xRMI functionality in xThread. Invokes a remote AuxiliaryMehtod() specified by the index, and input/output parameters over the xThreadBase's instance.
			public: void AuxiliaryMehtodOneThread(Int32 index, In const void *input, Int32 inputLength, Out void *output, Out Int32 *outputLength);
			// Terminates the current xThread session and releases all used resources.
			public: void TerminateSession();
			//------------ SyncReceiveAllThreads ----------------------
			//public: void SyncReceiveAllThreads();
			// This can be implemented by client via calling successive SyncReceiveOneThread(Int32 index) method.
			//------------ SyncSendAllThreads -------------------------
			//public: void SyncSendAllThreads();
			// This can be implemented by client via calling successive SyncSendOneThread(Int32 index) method.
			//----------------------------------------------------------
			// Gets the completion status of all remote threads
			public: bool get_AreAllThreadsCompleted();
			private: bool are_SuspendedAllThreads;
			private: bool _AreAllThreadsCompleted;
			//--------------------------------

			private: void SendOneExceptionToAllSocketsAndClose(System::Exception &e);
			private: void SendExceptionToOneSocketAndClose(System::Exception &e, ParvicursorSocket *socket);
			private: void MakeReadyReadSocketArrayList();
			private: void RemoveSocketFromArrayList(ParvicursorSocket *socket);
			private: Int32 FindSocketIndex(ParvicursorSocket *socket);

			//private void PublicKeyAuthentication(Socket sock);
			//private Byte[] AuthenticationHeaderBuilder(string username, string password);
			private: static void *Wrapper_To_Call_WorkerProc(void* pt2Object);
			//----------------------------------------------------

		};
   };
};
//**************************************************************************************************************//

#endif
