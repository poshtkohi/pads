#ifndef __System_Net_Sockets_AsyncSocket_h__
#define __System_Net_Sockets_AsyncSocket_h__


#include "../../Samples/LockFree/LockFree/LockFreeCounter/LockFreeCounter.h"
#include "../../Parvicursor/general.h"
#include "../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../Parvicursor/System/Guid/Guid.h"
#include "../../Parvicursor/System/InvalidOperationException/InvalidOperationException.h"
#include "../../Parvicursor/System/OverflowException/OverflowException.h"
#include "../../Parvicursor/System/ArgumentException/ArgumentException.h"
#include "../../Parvicursor/System.Net.Sockets/Socket/Socket.h"
#include "../../Parvicursor/System.Net/Dns/Dns.h"
#include "../../Parvicursor/System.Threading/Mutex/Mutex.h"
#include "../../Parvicursor/System.Threading/Barrier/Barrier.h"
#include "../../Parvicursor/System.Threading/ConditionVariable/ConditionVariable.h"
#include "../../Parvicursor/Parvicursor/Collections/SinglyLinkedList.h"

#include "AsyncSocketGlobals.h"
#include "AsyncSocketRuntimeManager.h"

#include <signal.h>


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
            class AsyncSocket : public Object
            {
                /*---------------------fields----------------*/
                private: Socket *internalSocket;//
                private: IAsyncResult volatile *asyncResult;
                private: bool serverMode;
                private: bool isListening;
				private: bool isConnected;
				private: bool isAccepted;
				private: bool connectRequested;
				private: bool isClosedByRuntimeManager;
                private: bool disposed;
                private: bool GuaranteeMulticoreExecution;
                private: AsyncSocketRuntimeManager *serverModeRuntime;
                private: bool isAsynchCloseRequested;
                private: Mutex *mutex;
                /*---------------------methods----------------*/
                public: AsyncSocket(AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType);
                public: AsyncSocket(Socket *sock); // for internal use only
                public: ~AsyncSocket();
                // Begins an asynchronous operation to accept an incoming connection attempt.
                public: IAsyncResult *BeginAccept(AsyncCallback callback, Object *state);
                // Begins an asynchronous operation to accept an incoming connection attempt.
                // The GuaranteeMulticoreExecution makes the AsyncSocket runtime manager try to disptach the socket onto a different CPU core within the asynchronous event-driven system.
                public: IAsyncResult *BeginAccept(AsyncCallback callback, Object *state, bool GuaranteeMulticoreExecution);
                // Asynchronously accepts an incoming connection attempt. For internal use only: If you finish your work with the returned AsyncSocket instance, you must release it with calling 'delete' keyword.
                public: AsyncSocket *EndAccept(IAsyncResult *asyncResult);
                // Begins an asynchronous request for a remote host connection.
                public: IAsyncResult *BeginConnect(IPEndPoint &remoteEP, AsyncCallback callback, Object *state);
                // Begins an asynchronous request for a remote host connection.
                // The GuaranteeMulticoreExecution makes the AsyncSocket runtime manager try to disptach the socket onto a different CPU core within the asynchronous event-driven system.
                public: IAsyncResult *BeginConnect(IPEndPoint &remoteEP, AsyncCallback callback, Object *state, bool GuaranteeMulticoreExecution);
                // Ends a pending asynchronous connection request.
                public: void EndConnect(IAsyncResult *state);
                // Sends data asynchronously to a connected Socket.
                public: IAsyncResult *BeginSend(char buffer[], Int32 offset, Int32 size, SocketFlags socketFlags, AsyncCallback callback, Object *state);
                // Ends a pending asynchronous send. If successful, returns the number of bytes sent to the Socket; otherwise, an invalid Socket error.
                public: Int32 EndSend(IAsyncResult *asyncResult);
                // Begins to asynchronously receive data from a connected Socket.
                public: IAsyncResult *BeginReceive(char buffer[], Int32 offset, Int32 size, SocketFlags socketFlags, AsyncCallback callback, Object *state);
                // Ends a pending asynchronous read. Returns the number of bytes received. The value of retuern, if the connection has been closed.
                public: Int32 EndReceive(IAsyncResult *asyncResult);
                public: void Bind(IPEndPoint &localEP);
				public: void Listen(Int32 backlog);
				public: void Close();
				// Begins an asynchronous operation to close an AsyncSocket object. This method can specifically be used to perform finalization opeartions such as memory deallocation.
				public: void BeginClose(AsyncCallback callback, Object *state);
				// Completes an asynchronous operation to close an AsyncĶSocket object.
				public: void EndClose(IAsyncResult *asyncResult);
				public: void Shutdown(SocketShutdown how);
				public: inline const Socket *get_InternalSocket() const { return internalSocket; }
				public: inline const Socket *get_BaseSocket() const { return internalSocket; }
				// Gets the logical, physical processor core's ID on which this socket is running.
				public: Long get_CoreID() const;
				// Gets the number of underlying logical, physical processor cores.
				public: Int32 get_CoreCount() const;
                // The method atomically tries to migrate the AsyncSocket instance from current core to another (destination) core specified by DestCoreID.
                // This method returns false, if the destination core has not enough empty slot to queue this AsyncSocket instance.
                // This mehtod may throw an exception that indicates the occurred error during the socket migration process.
                // Thus, catch the exception and ensure that the instance can not be migrated to another core in this time.
                // In this situation, try later againto call this method or igonore the migration process as a whole.
                // To be able to invoke this method, you must had been called one of the BeginSend() or BeginReceive() methods, at least one time,
                // on this AsyncSocket instance.
                // This method must be only called on an AsyncSocket instance with a connected or accepted state.
				public: bool TryMigrateToCore(Int32 DestCoreID);
				public: static void InitClientModeRuntime(void);
                public: static void DestroyClientModeRuntime(void);
            };
            //----------------------------------------------------
        };
    };
};
//**************************************************************************************************************//

#endif

