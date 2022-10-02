#ifndef __System_Net_Sockets_AsyncSocketRuntimeManager_h__
#define __System_Net_Sockets_AsyncSocketRuntimeManager_h__

#include "../../Parvicursor/general.h"
#include "../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../Parvicursor/System/Guid/Guid.h"
#include "../../Parvicursor/System/DateTime/DateTime.h"
#include "../../Parvicursor/System.Net.Sockets/Socket/Socket.h"
#include "../../Parvicursor/System.Net/Dns/Dns.h"
#include "../../Parvicursor/System.Threading/Thread/Thread.h"
#include "../../Parvicursor/System.Threading/Mutex/Mutex.h"
#include "../../Parvicursor/System.Threading/Barrier/Barrier.h"
#include "../../Parvicursor/System.Threading/ConditionVariable/ConditionVariable.h"
#include "../../Samples/LockFree/LockFree/LockFreeCounter/LockFreeCounter.h"
#include "../../Parvicursor/Parvicursor/Collections/SinglyLinkedList.h"

#include "AsyncSocketGlobals.h"
#include <signal.h>
#include <time.h>
#include <sys/resource.h>

using namespace System;
using namespace System::Net;
using namespace System::Net::Sockets;
using namespace System::Threading;

using namespace LockFree;
using namespace Parvicursor::Collections;
//**************************************************************************************************************//
namespace System
{
    namespace Net
    {
        namespace Sockets
        {
            //----------------------------------------------------
            class AsyncSocketRuntimeManager : public Object
            {
                /*---------------------fields----------------*/
                private: Socket *acceptorSocket;
                private: AsyncCallback acceptCallback;
                private: Object *acceptState;
                private: Mutex *acceptCallback_mutex;
                private: ConditionVariable *acceptCallback_cv;
                private: Int32 ThreadPoolNum;
                private: Thread *acceptorThread;
                private: LockFreeCounter currentConnections;
                private: ReaderWriterThreadInfo **readerWriterThreads;
                private: ReaderWriterThreadInfo **connectorThreads;
                //private: Thread **acceptorThreads;
                private: bool hasRun;
                private: Long volatile StopRequested;
                //private: Long volatile accept_has_executed;
                public: Long volatile LastCoreAssigned_ReaderWriterThreads;
                public: Long volatile LastCoreAssigned_ConnectorThreads;
                private: Barrier *cleanupBarrier;
                private: bool serverMode;
                private: bool disposed;
                private: Long volatile available;
                /*---------------------methods----------------*/
                public: AsyncSocketRuntimeManager(Socket *acceptorSocket, Int32 ThreadPoolNum, bool serverMode);
                // for internal use only.
                public: AsyncSocketRuntimeManager();
                public: ~AsyncSocketRuntimeManager();
                public: void Run();
                public: void Dispose();
                public: void set_AcceptCallback(AsyncCallback acceptCallback, Object *acceptState);
                private: void *acceptor_proc(void *arg);
                private: static void *Wrapper_To_Call_acceptor_proc(void *pt2Object);
                private: static void *reader_writer_proc(void *arg);
                private: static void *connector_proc(void *arg);
                public: static void SignalReaderWriterThreadToUnblockEpollWait(ReaderWriterThreadInfo *info, bool enable);
                private: inline static void DeletionPhaseForMarkedSockets(ReaderWriterThreadInfo *info);
                public: static bool PullSocketFromThread(fd_state *state, ReaderWriterThreadInfo *info);
                public: static bool PushSocketToThread(fd_state *state, ReaderWriterThreadInfo *info);
                public: ReaderWriterThreadInfo *AssignSocketInstanceToReaderWriterThreadPool(ReaderWriterThreadInfo **threads, Int32 ThreadPoolNum, struct fd_state *state, bool GuaranteeMulticoreExecution, Long volatile *LastCoreAssigned_XThreads);
                public: ReaderWriterThreadInfo **get_ReaderWriterThreads() const;
                public: ReaderWriterThreadInfo **get_ConnectorThreads() const;
            };
        };
    };
};
//**************************************************************************************************************//

#endif

