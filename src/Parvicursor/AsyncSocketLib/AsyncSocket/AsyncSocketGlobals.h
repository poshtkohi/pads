#ifndef __System_Net_Sockets_AsyncSocketGlobals_h__
#define __System_Net_Sockets_AsyncSocketGlobals_h__


#include "../../Parvicursor/general.h"
#include "../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../Parvicursor/System/Async/Async.h"
#include "../../Parvicursor/System/Environment/Environment.h"
#include "../../Parvicursor/System.Net.Sockets/Socket/Socket.h"
#include "../../Parvicursor/System.Net/Dns/Dns.h"
#include "../../Parvicursor/System.Threading/Thread/Thread.h"
#include "../../Parvicursor/System.Threading/Mutex/Mutex.h"
#include "../../Parvicursor/System.Threading/Barrier/Barrier.h"
#include "../../Parvicursor/System.Threading/ConditionVariable/ConditionVariable.h"
#include "../../Parvicursor/System.Collections/Queue/Queue.h"
#include "../../Samples/LockFree/LockFree/LockFreeCounter/LockFreeCounter.h"
#include "../../Parvicursor/Parvicursor/Collections/SinglyLinkedList.h"

/* For fcntl */
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/resource.h>
#define __epoll_event_num__ 100*1024 // 10*1024 connection per each thread


using namespace System;
using namespace System::Net::Sockets;

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
            class AcceptedConnectedInfo;
            static struct fd_state *alloc_fd_state(Socket *sock, LockFreeCounter *CurrentConnections);
            static void free_fd_state(struct fd_state *state);
            static void make_nonblocking(Socket *sock);
            #define NonBlockingConnectTimeout 30//30 seconds
            #define MaxFileDescriptorSupportForSetrlimit RLIM_INFINITY
            static Int32 ServerModeRuntime_ThreadPool_Num = Environment::get_ProcessorCount() * 2;
            static Int32 ClientModeRuntime_ThreadPool_Num = Environment::get_ProcessorCount() * 2;
            //static Int32 Connectors_ThreadPool_Num = Environment::get_ProcessorCount() * 2;
            //----------------------------------------------------
            enum AsynchMethodOpcodes
            {
                Unknown_op = -1,
                BeginConnect_op = 0,
                BeginAccept_op = 1,
                BeginSend_op = 2,
                BeginReceive_op = 3,
                BeginClose_op = 4
            };
            //----------------------------------------------------
            struct fd_state
            {
                char *buffer;
                Int32 offset;
                Int32 size;
                SocketFlags socketFlags;
                Socket *sock;
                bool toRead;
                bool toWrite;
                Int32 n_written;
                Int32 n_read;
                Int32 errorCode;
                bool hasError;
                bool hasClosed;
                bool connectRequested;
                IPEndPoint *remoteEP;
                Double elapsed; // For nonblocking connect(), 'elapsed' is in seconds.
                IAsyncResult *asyncResult;
                LockFreeCounter *currentConnections;
                Long CoreID;
                Long volatile marked_for_deletion;
                Long volatile closed;
				Long volatile is_executing;
            };
            //----------------------------------------------------
            struct fd_state *alloc_fd_state(Socket *sock, LockFreeCounter *CurrentConnections)
            {
                struct fd_state *state = (struct fd_state *)::malloc(sizeof(struct fd_state)); // consider here memory pool
                if (!state)
                    return null; // Here, handle the exception
                state->sock = sock;
                state->offset = 0;
                state->size = 0;
                state->socketFlags = System::Net::Sockets::None;
                state->toRead = false;
                state->toWrite = false;
                state->n_read = 0;
                state->n_written = 0;
                state->errorCode = 0;
                state->hasError = false;
                state->hasClosed = false;
                state->connectRequested = false;
                //state->asyncResult = null;
                state->elapsed = 0;
                state->CoreID = -1;
                state->currentConnections = CurrentConnections;
                state->currentConnections->Increment();
                state->marked_for_deletion = false;
                state->closed = false;
				state->is_executing = false;

                return state;
            }
            //----------------------------------------------------
            void free_fd_state(struct fd_state *state)
            {
                if(state == null)
                    return ;

                if(state->sock != null)
                {
                    if(!state->closed)
                    {
                        try
                        {
                            state->sock->Shutdown(System::Net::Sockets::Both);
                        }
                        catch(Exception &e)
                        {
                            //printf("Error occured in free_fd_state() 1. Exception message: %s\n", e.get_Message().get_BaseStream());
                        }
                        catch(...) {}
                        try
                        {
                            state->sock->Close();
                        }
                        catch(Exception &e)
                        {
                            //printf("Error occured in free_fd_state() 1. Exception message: %s\n", e.get_Message().get_BaseStream());
                        }
                        catch(...) {}
                        state->closed = true;
                    }

                    delete state->sock;
                    state->sock = null;
                }

                ::free(state);
                state = null;
            }
            //----------------------------------------------------
            void make_nonblocking(Socket *sock)
            {
                //int flags = fcntl(sock->get_Handle(), F_GETFL, 0);
                //fcntl(sock->get_Handle(), F_SETFL, flags | O_NONBLOCK);
                if(::fcntl(sock->get_Handle(), F_SETFL, O_NONBLOCK) < 0)
                {
                    printf("make_nonblocking error: %s\n", (const char *)_sys_errlist[errno]);
                }
            }
            //----------------------------------------------------
            inline static void SetBoolFlagAtomic(Long volatile *flag, int val)
            {
                Long volatile temp = -10;

                while(!Parvicursor_AtomicCompareAndSwap(&temp, flag, val));
            }
            inline static bool GetBoolFlagAtomic(Long volatile *flag)
            {
                Long volatile temp = -10;

                Parvicursor_AtomicCompareAndSwap(&temp, flag, true);
                return (bool)temp;
            }
            //----------------------------------------------------
            inline static Int32 NonBlockingRecv(fd_state *state, Mutex *threadMutex)
            {
                //printf("NonBlockingRecv\n");//
                Int32 result = 0;
                state->n_read = 0;

				/*threadMutex->Lock();
				if(state->closed)
				{
					threadMutex->Unlock();
					return -1;
				}
				if(state->marked_for_deletion)
				{
					try { state->sock->Shutdown(System::Net::Sockets::Both); } catch(Exception &e) {} catch(...) {}
					try { state->sock->Close(); } catch(Exception &e) {} catch(...) {}
					state->closed = true;
					threadMutex->Unlock();
					return -1;
				}
				state->is_executing = true;
				threadMutex->Unlock();*/

				Int32 sockfd = state->sock->get_Handle();

                while((state->size - state->n_read) != 0)
                {
                    result = ::recv(sockfd, state->buffer + state->offset + state->n_read, state->size - state->n_read, state->socketFlags);
                    if (result <= 0)
                        break;

                    state->n_read += result;
                }

				/*threadMutex->Lock();
				state->is_executing = false;
				if(!state->closed && state->marked_for_deletion)
				{
					try { state->sock->Shutdown(System::Net::Sockets::Both); } catch(Exception &e) {} catch(...) {}
					try { state->sock->Close(); } catch(Exception &e) {} catch(...) {}
					state->closed = true;
					threadMutex->Unlock();
					return -1;
				}
				threadMutex->Unlock();*/

                //result = ::recv(state->sock->get_Handle(), state->buffer + state->offset, state->size, state->socketFlags);
                if(state->n_read > 0/*result > 0*/)
                {
                    state->hasError = false;
                    state->hasClosed = false;
                    state->errorCode = 0;
                    //state->n_read = result;
                    //printf("state->buffer: %s\n", state->buffer);
                    return 0;
                }

                if(result == 0)
                {
                    state->hasClosed = true;
                    state->hasError = false;
                    return 0;
                }

                int nError = errno;
                if(nError != EWOULDBLOCK && nError != 0)
                {
                    state->hasError = true;
                    state->hasClosed = false;
                    state->errorCode = nError;
                    return 0;
                }

                state->hasError = false;
                state->hasClosed = false;
                state->errorCode = 0;//
                return 0;
            }
            //----------------------------------------------------
            inline static Int32 NonBlockingSend(fd_state *state, Mutex *threadMutex)
            {
                //printf("NonBlockingSend\n");//
                Int32 result;
                state->n_written = 0;

				/*threadMutex->Lock();
				if(state->closed)
				{
					threadMutex->Unlock();
					return -1;
				}
				if(state->marked_for_deletion)
				{
					try { state->sock->Shutdown(System::Net::Sockets::Both); } catch(Exception &e) {} catch(...) {}
					try { state->sock->Close(); } catch(Exception &e) {} catch(...) {}
					state->closed = true;
					threadMutex->Unlock();
					return -1;
				}
				state->is_executing = true;
				threadMutex->Unlock();*/

				Int32 sockfd = state->sock->get_Handle();
                while((state->size - state->n_written) != 0)
                {
                    result = ::send(sockfd, state->buffer + state->offset + state->n_written, state->size - state->n_written, state->socketFlags | MSG_NOSIGNAL);
                    if (result <= 0)
                        break;

                    state->n_written += result;
                }

				/*threadMutex->Lock();
				state->is_executing = false;
				state->is_executing = false;
				if(!state->closed && state->marked_for_deletion)
				{
					try { state->sock->Shutdown(System::Net::Sockets::Both); } catch(Exception &e) {} catch(...) {}
					try { state->sock->Close(); } catch(Exception &e) {} catch(...) {}
					state->closed = true;
					threadMutex->Unlock();
					return -1;
				}
				threadMutex->Unlock();*/

                //result = ::send(state->sock->get_Handle(), state->buffer + state->offset, state->size, state->socketFlags);
                if(state->n_written > 0/*result > 0*/)
                {
                    state->hasError = false;
                    state->hasClosed = false;
                    state->errorCode = 0;
                    //state->n_written = result;
                    return 0;
                }

                int nError = errno;

				if(nError == EPIPE)
				{
				    state->hasError = true;//
                    state->hasClosed = false;//
                    state->errorCode = nError;
                    return 0;
				}
                if(nError != EWOULDBLOCK && nError != 0)
                {
                    state->hasError = true;
                    state->hasClosed = false;
                    state->errorCode = nError;
                    return 0;
                }

                state->hasError = false;
                state->hasClosed = false;
                state->errorCode = 0;//
                return 0;

            }
            //----------------------------------------------------
            typedef void (*Sigfunc)(int);

            inline static Sigfunc my_signal(int signum, Sigfunc func)
            {
                struct sigaction act, oact;

                act.sa_handler = func;
                ::sigemptyset(&act.sa_mask);
                act.sa_flags = 0;

                if (signum != SIGALRM)
                    act.sa_flags |= SA_NODEFER; //SA_RESTART;

                if (::sigaction(signum, &act, &oact) < 0)
                    return (SIG_ERR);
                return oact.sa_handler;
                /*
                    SA_RESTART provides the BSD-like behavior of allowing system calls to be restartable across
                    signals. SA_NODEFER means allow the signal to be received from within its own signal handler.
                    When the signal calls are replaced with "my_signal", the thread is interrupted. The output
                    prints out "interrupted system call" and recv returned a -1 when SIGUSR1 was sent. The program
                    stopped altogether with the same output when SIGINT was sent, but the abort was called at the end.
                */
            }
            //----------------------------------------------------
            class ReaderWriterThreadInfo : public Object
            {
                public: Thread *thread;
                public: Mutex *states_mutex;
                public: ConditionVariable *states_cv;
                public: Long volatile current_states;// use a lock-free counterpart
                public: Long volatile epoll_has_executed;
                public: Int32 epollfd;
                public: struct epoll_event event;
                public: struct epoll_event *events;
                public: Long volatile StopRequested;
                public: Barrier *cleanupBarrier;
                private: bool disposed;
                public: Long volatile available;
                public: Object *AsyncSocketRuntimeManager_instance;
                public: bool serverMode;
                public: SinglyLinkedList *queue;
                public: Int32 CoreID;
                public: ReaderWriterThreadInfo(Int32 CoreID, void *(*thread_func)(void *), Barrier *cleanupBarrier, bool serverMode = false)
                {
                    this->CoreID = CoreID;
                    thread = new Thread(thread_func, (void *)this);
                    states_mutex = new Mutex();
                    states_cv = new ConditionVariable(states_mutex);
                    current_states = 0;
                    epoll_has_executed = false;

                    events = (struct epoll_event *)::calloc(__epoll_event_num__, sizeof(struct epoll_event));
                    if((epollfd = ::epoll_create(__epoll_event_num__)) == -1)
                    {
                        printf("epoll_create() error: %s\n", (const char *)_sys_errlist[errno]);
                        abort();
                    }

                    StopRequested = false;
                    this->cleanupBarrier = cleanupBarrier;
                    this->serverMode = serverMode;
                    disposed = false;
                    available = 0;
                    queue = new SinglyLinkedList();
                }
                public: ~ReaderWriterThreadInfo()
                {
                    if(!disposed)
                    {
                        disposed = true;

                        if(thread != null)
                            delete thread;
                        if(states_mutex != null)
                            delete states_mutex;
                        if(states_cv != null)
                            delete states_cv;

                        if(events != null)
                        {
                            ::free(events);
                            ::close(epollfd);
                            events = null;
                        }
                        if(queue != null)
                        {
                            delete queue;
                            queue = null;
                        }
                        thread = null;
                        states_mutex = null;
                        states_cv = null;
                    }
                }
            };
            //----------------------------------------------------
            class AcceptedConnectedInfo : public IAsyncResult
            {
                public: AsyncCallback callback;
                public: Object *closeState;
                public: AsyncCallback closeCallback;
                public: Socket *AcceptedConnectedSocket; // must free
                public: fd_state *fdState; // must free
                public: Object *AsyncSocketRuntimeManager_instance_ref;
                public: Object *ReaderWriterThreadInfo_instance_ref;
                public: AsynchMethodOpcodes opcode;
                public: Int32 ProcessorCoreID;
                public: bool EndXHasCalled;
                public: bool callbackCalledByRuntimeManager;
                public: bool callbackCalledByRuntimeManagerDeletionPhase;
                public: AcceptedConnectedInfo()
                {
                    callback = null;
                    closeCallback = null;
                    closeState = null;
                    AcceptedConnectedSocket = null;
                    fdState = null;
                    AsyncSocketRuntimeManager_instance_ref = null;
                    ReaderWriterThreadInfo_instance_ref = null;
                    opcode = Unknown_op;
                    EndXHasCalled = false;
                    ProcessorCoreID = 0;
                    callbackCalledByRuntimeManager = true;
                    callbackCalledByRuntimeManagerDeletionPhase = false;
                }
                public: ~AcceptedConnectedInfo()
                {
                    //delete test;
                    //printf("dd1\n");//
                    // here, we must de-allocate fdState from AsyncSocketRuntimeManager_instance as well. (queue)
                    //delete AsyncWaitHandle_mutex;
                    //delete AsyncWaitHandle_cv;
                    if(fdState != null)
                    {
                        free_fd_state(fdState);
                        fdState = null;
                        callback = null;
                        AcceptedConnectedSocket = null;
                        AsyncSocketRuntimeManager_instance_ref = null;
                        ReaderWriterThreadInfo_instance_ref = null;
                        opcode = Unknown_op;
                        EndXHasCalled = false;
                    }
                }
            };
            //----------------------------------------------------
        };
    };
};
//**************************************************************************************************************//

#endif

