#include "AsyncSocketRuntimeManager.h"

//**************************************************************************************************************//

//static sem_t sem;
//static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

namespace System
{
    namespace Net
    {
        namespace Sockets
        {
            //----------------------------------------------------
            AsyncSocketRuntimeManager::AsyncSocketRuntimeManager(Socket *acceptorSocket, Int32 ThreadPoolNum, bool serverMode)
            {
                struct rlimit rlim;
                rlim.rlim_max = MaxFileDescriptorSupportForSetrlimit;
                rlim.rlim_cur = MaxFileDescriptorSupportForSetrlimit;
                setrlimit(RLIMIT_NOFILE, &rlim);

                //sem_init(&sem, 0, 1);
                if(serverMode)
                {
                    if(acceptorSocket == null)
                        throw ArgumentNullException("acceptorSocket");
                    this->acceptorSocket = acceptorSocket;
                }

                if(ThreadPoolNum <= 0)
                    throw ArgumentOutOfRangeException("ThreadPoolNum", "ThreadPoolNum must be greater than zero");

                this->serverMode = serverMode;
                this->ThreadPoolNum = ThreadPoolNum;
                if(serverMode)
                    cleanupBarrier = new Barrier(1 + ThreadPoolNum); // caller thread + reader/Writer (acceptor) threads
                else
                    cleanupBarrier = new Barrier(1 + 2 * ThreadPoolNum); // caller thread + (reader/writer + connector) threads


                StopRequested = false;

                if(serverMode)
                {
                    acceptCallback = null;
                    acceptCallback_mutex = new Mutex();
                    acceptCallback_cv = new ConditionVariable(acceptCallback_mutex);

                    ///acceptorThread = new Thread(Wrapper_To_Call_acceptor_proc, this);
                    ///currentConnections = LockFreeCounter();
                    ///accept_has_executed = false;
                }
                else
                {
                    connectorThreads = new ReaderWriterThreadInfo *[ThreadPoolNum];
                    for(register Int32 i = 0 ; i < ThreadPoolNum ; i++)
                        connectorThreads[i] = new ReaderWriterThreadInfo(i, connector_proc, cleanupBarrier);
                }

                readerWriterThreads = new ReaderWriterThreadInfo *[ThreadPoolNum];
                for(register Int32 i = 0 ; i < ThreadPoolNum ; i++)
                {
                    readerWriterThreads[i] = new ReaderWriterThreadInfo(i, reader_writer_proc, cleanupBarrier, this->serverMode);
                    readerWriterThreads[i]->AsyncSocketRuntimeManager_instance = (Object *)this;

                }

                LastCoreAssigned_ReaderWriterThreads = -1; // -1 means non-assigned
                LastCoreAssigned_ConnectorThreads = -1; // -1 means non-assigned

                hasRun = false;
                disposed = false;
            }
            //----------------------------------------------------
            AsyncSocketRuntimeManager::AsyncSocketRuntimeManager()
            {
                disposed = true;
            }
            //----------------------------------------------------
			AsyncSocketRuntimeManager::~AsyncSocketRuntimeManager()
			{
			    /*if(!disposed)
			    {
			        // cleanup here, terminate threads,mutexex and etc.
			        disposed = true;
			    }*/
			    Dispose();
			}
            //----------------------------------------------------
			void AsyncSocketRuntimeManager::Run()
            {
                if(!hasRun)
                {
                    if(serverMode)
                    {
                        ///acceptorThread->Start();
                        ///acceptorThread->SetDetached();
                    }
                    else
                    {
                        for(register Int32 i = 0 ; i < ThreadPoolNum ; i++)
                        {
                            connectorThreads[i]->thread->Start();
                            connectorThreads[i]->thread->SetDetached();
                        }
                    }

                    for(register Int32 i = 0 ; i < ThreadPoolNum ; i++)
                    {
                        readerWriterThreads[i]->thread->Start();
                        readerWriterThreads[i]->thread->SetDetached();
                    }

                    hasRun = true;
                }
            }
            //----------------------------------------------------
			void AsyncSocketRuntimeManager::Dispose()
            {
                if(!disposed)
			    {
			        // cleanup here, terminate threads,mutexes and etc.
			        disposed = true;

                    if(hasRun)
                    {
                        if(serverMode) // acceptorThread
                        {
                            SetBoolFlagAtomic(&StopRequested, true);
                            acceptCallback_cv->Broadcast();
                            ///if(GetBoolFlagAtomic(&accept_has_executed))
                           /// {
                                //printf("GetReaderSelectFlag true\n"); //
                             ///   int ret = pthread_kill(acceptorThread->thread, SIGUSR1);
                             ///   if(ret != 0)
                           //     {
                            //        printf("pthread_kill error: %s\n", (const char *)_sys_errlist[errno]);
                            //    }
                           // }
                            /*else
                                printf("GetReaderSelectFlag false\n");*/
                        }
                        else // connectorThreads
                        {
                            for(register Int32 i = 0 ; i < ThreadPoolNum ; i++)
                            {
                                SetBoolFlagAtomic(&connectorThreads[i]->StopRequested, true);

                                if(GetBoolFlagAtomic(&connectorThreads[i]->epoll_has_executed))
                                {
                                    //printf("GetReaderSelectFlag true\n"); //
                                    int ret = ::pthread_kill(connectorThreads[i]->thread->thread, SIGUSR1);
                                    if(ret != 0)
                                    {
                                        printf("pthread_kill error: %s\n", (const char *)_sys_errlist[errno]);
                                    }
                                }
                                ///connectorThreads[i]->states_mutex->Lock();
                                ///if(connectorThreads[i]->available == 0)
                                ///    connectorThreads[i]->states_cv->Signal();
                                ///connectorThreads[i]->states_mutex->Unlock();
                                /*else
                                    printf("GetReaderSelectFlag false\n");*/
                            }
                        }

                        for(register Int32 i = 0 ; i < ThreadPoolNum ; i++)
                        {
                            SetBoolFlagAtomic(&readerWriterThreads[i]->StopRequested, true);
                            if(GetBoolFlagAtomic(&readerWriterThreads[i]->epoll_has_executed))
                            {
                                //printf("GetReaderSelectFlag true\n"); //
                                int ret = ::pthread_kill(readerWriterThreads[i]->thread->thread, SIGUSR1);
                                if(ret != 0)
                                {
                                    printf("pthread_kill error: %s\n", (const char *)_sys_errlist[errno]);
                                }
                            }
                            ///readerWriterThreads[i]->states_mutex->Lock();
                            ///if(connectorThreads[i]->available == 0)
                                ///readerWriterThreads[i]->states_cv->Signal();
                            ///readerWriterThreads[i]->states_mutex->Unlock();
                            /*else
                                printf("GetReaderSelectFlag false\n");*/
                        }

                        // Waits until all threads terminate.
                        cleanupBarrier->SignalAndWait();
                    }

                    if(serverMode)
                    {
                        delete acceptCallback_mutex;
                        delete acceptCallback_cv;
                        ///delete acceptorThread;
                    }
                    else
                    {
                        for(register Int32 i = 0 ; i < ThreadPoolNum ; i++)
                            delete connectorThreads[i];

                        delete connectorThreads;
                    }

                    for(register Int32 i = 0 ; i < ThreadPoolNum ; i++)
                        delete readerWriterThreads[i];

                    delete readerWriterThreads;
                    delete cleanupBarrier;
			    }
            }
            //----------------------------------------------------
            void AsyncSocketRuntimeManager::set_AcceptCallback(AsyncCallback acceptCallback, Object *acceptState)
            {
                if(this->acceptCallback != null)
                    return;

                if(serverMode)
                {
                    acceptCallback_mutex->Lock();
                    this->acceptCallback = acceptCallback;
                    this->acceptState = acceptState;
                    acceptCallback_cv->Broadcast();
                    acceptCallback_mutex->Unlock();
                }
            }
            //----------------------------------------------------
            static void signal_handler_reader_writer(int sig)
            {
                //printf("signal_handler_reader_writer\n");
                /*sigset_t signal_set;
                sigfillset(&signal_set);
                sem_post(&sem);*/
                //signal(SIGUSR1, signal_handler_reader_writer);//
                return;
            }

			void *AsyncSocketRuntimeManager::reader_writer_proc(void *arg)
            {
                my_signal(SIGUSR1, signal_handler_reader_writer);
                //signal(SIGUSR1, signal_handler_reader_writer);

               ReaderWriterThreadInfo *info = (ReaderWriterThreadInfo *)arg;

                bool __stopRequested;

                Int32 acceptorSocket_handle = -1;
                AsyncSocketRuntimeManager *AsyncSocketRuntimeManager_instance = (AsyncSocketRuntimeManager *)info->AsyncSocketRuntimeManager_instance;
                if(AsyncSocketRuntimeManager_instance->serverMode)
                {
                    struct epoll_event event;
                    event.data.fd = AsyncSocketRuntimeManager_instance->acceptorSocket->get_Handle();
                    event.events = EPOLLIN | EPOLLET;
                    Int32 error = ::epoll_ctl(info->epollfd, EPOLL_CTL_ADD, AsyncSocketRuntimeManager_instance->acceptorSocket->get_Handle(), &event);
                    if (error == -1)
                    {
                        printf("epoll_ctl() error: %s\n", (const char *)_sys_errlist[errno]);
                        abort();
                    }

                    __stopRequested = AsyncSocketRuntimeManager_instance->StopRequested;

                    AsyncSocketRuntimeManager_instance->acceptCallback_mutex->Lock();
                    while(AsyncSocketRuntimeManager_instance->acceptCallback == null && !(__stopRequested = GetBoolFlagAtomic(&AsyncSocketRuntimeManager_instance->StopRequested)) )
                        AsyncSocketRuntimeManager_instance->acceptCallback_cv->Wait();

                    //printf("acceptCallback_cv\n");//
                    if(__stopRequested)
                    {
                        AsyncSocketRuntimeManager_instance->acceptCallback_mutex->Unlock();
                        AsyncSocketRuntimeManager_instance->cleanupBarrier->SignalAndWait();
                        return arg;
                    }
                    //info->available++;
                    info->current_states++;
                    acceptorSocket_handle = AsyncSocketRuntimeManager_instance->acceptorSocket->get_Handle();
                    AsyncSocketRuntimeManager_instance->acceptCallback_mutex->Unlock();

                    //printf("acceptorSocket_handle: %d\n", acceptorSocket_handle);//
                }

                register Int32 i, j = 0;
                Int32 n, nError;

                struct epoll_event temp_event;

                __stopRequested = info->StopRequested;
                //printf("state 1.\n");//
                while(!GetBoolFlagAtomic(&info->StopRequested))
                {
                    j++;
                    //printf("begining\n");

                    DeletionPhaseForMarkedSockets(info);

                    SetBoolFlagAtomic(&info->epoll_has_executed, true);
                    n = ::epoll_wait(info->epollfd, info->events, __epoll_event_num__, -1);
                    SetBoolFlagAtomic(&info->epoll_has_executed, false);

                    if(n == 0)
                        continue;

                    if(n == -1)
                    {
                        //printf("select() error: %s, number: %d\n", (const char *)_sys_errlist[nError], nError);
                        nError = errno;
                        if(nError == EINTR)
                        {
                            //printf("EINTR\n");
                            continue;
                        }
                        //printf("select() reader error\n"); // handle here this error.
                        break;
                    }

                    for(i = 0 ; i < n ; i++)
                    {
                        //info->states_mutex->Lock();

                        if(acceptorSocket_handle == info->events[i].data.fd)
                        {
                            //info->states_mutex->Unlock();
                            //printf("accept stage 1\n");//
                            /* We have a notification on the listening socket, which
                            means one or more incoming connections. */
                            while(true)
                            {
                                Socket *newsock = null;
                                try { newsock = AsyncSocketRuntimeManager_instance->acceptorSocket->Accept(); }
                                catch(SocketException &e)
                                {
                                    if ((e.get_ErrorCode() == EAGAIN) || (e.get_ErrorCode() == EWOULDBLOCK))
                                    {
                                        /* We have processed all incoming
                                        connections. */
                                        break;
                                    }
                                    else
                                    {
                                        printf("listener.Accept() error: %s\n", (const char *)_sys_errlist[e.get_ErrorCode()]);
                                        break;
                                    }
                                }
                                catch(...) { printf("listener.Accept() error\n"); break; }


                                make_nonblocking(newsock);

                                //printf("new connection\n");

                                if(AsyncSocketRuntimeManager_instance->currentConnections.get_Count() == __epoll_event_num__ * AsyncSocketRuntimeManager_instance->ThreadPoolNum) // handle here for a new thread
                                {
                                    printf("__epoll_event_num__*ThreadPoolNum overflow error\n");//
                                    newsock->Close();
                                    delete newsock;
                                    //info->states_mutex->Unlock();
                                    break;
                                }
                                //printf("accept stage 2\n");//
                                //info->states_mutex->Unlock();

                                fd_state *fdState = alloc_fd_state(newsock, &AsyncSocketRuntimeManager_instance->currentConnections);//
                                AcceptedConnectedInfo *_info = new AcceptedConnectedInfo();
                                fdState->asyncResult = _info;
                                fdState->CoreID = info->CoreID;
                                _info->callback = AsyncSocketRuntimeManager_instance->acceptCallback;
                                //_info->AsyncState = new AsyncSocket(newsock);///
                                _info->AsyncState = AsyncSocketRuntimeManager_instance->acceptState;
                                _info->AcceptedConnectedSocket = newsock;
                                _info->AsyncSocketRuntimeManager_instance_ref = AsyncSocketRuntimeManager_instance;
                                _info->ReaderWriterThreadInfo_instance_ref = info;
                                //_info->ReaderWriterThreadInfo_instance_ref = null;
                                _info->fdState = fdState;
                                _info->CompletedSynchronously = false;
                                //_info->IsCompleted = true;//false;
                                _info->EndXHasCalled = false;
                                _info->opcode = BeginAccept_op;
                                _info->IsCompleted = false;
                                //printf("accept stage 3\n");//
                                struct epoll_event event;
                                event.data.fd = _info->fdState->sock->get_Handle();
                                event.data.ptr = _info->fdState;
                                event.events = EPOLLONESHOT;
                                if(::epoll_ctl(info->epollfd, EPOLL_CTL_ADD, _info->fdState->sock->get_Handle(), &event) == -1)
                                {
                                    ////threadInstance->states_mutex->Unlock();
                                    //throw Exception(String("epoll_ctl() error: ") + (const char *)_sys_errlist[errno]);
                                    free_fd_state(fdState);
                                    delete _info;
                                    //info->states_mutex->Unlock();
                                    printf("epoll_ctl() error: %s", (const char *)_sys_errlist[errno]);
                                    //abort();////
                                    break;
                                }
                                LockFreeCounter::AtomicIncrement(&info->current_states);
                                //info->current_states++; // conisder lock-free counterpart

                                //info->states_mutex->Unlock();
                                //info->AsyncWaitHandle_mutex = new Mutex();
                                //info->AsyncWaitHandle_cv = new AsyncWaitHandle_cv(info->AsyncWaitHandle_mutex);
                                //printf("hello j: %d\n", j);
                                _info->callback(_info);
                                //printf("accept stage 4\n");//
                            }
                            continue;

                        }

                        fd_state *state = (fd_state *)info->events[i].data.ptr;
                        //info->states_mutex->Unlock();

                        if(state == null)
                        {
                            //info->states_mutex->Unlock();
                            continue;
                        }

                        info->states_mutex->Lock();
                        if(state->marked_for_deletion)
                        {
							if(!state->closed)
							{
								try { state->sock->Shutdown(System::Net::Sockets::Both); } catch(Exception &e) { } catch(...) {}
								try { state->sock->Close(); } catch(Exception &e) { } catch(...) {}
								state->closed = true;
							}
                            info->states_mutex->Unlock();
                            continue;
                        }
                        info->states_mutex->Unlock();

                        /*if(state->marked_for_deletion)
                        {
                            delete (AcceptedConnectedInfo *)state->asyncResult;
                            info->states_mutex->Unlock();
                            continue;
                        }*/
                        /*if(state->marked_for_deletion)
                        {
                            info->current_states--; // conisder lock-free counterpart
                            //info->available--;
                            //state->errorCode = errno;
                            if(::epoll_ctl(info->epollfd, EPOLL_CTL_DEL, state->sock->get_Handle(), &temp_event) == -1)
                            {
                                printf("epoll_ctl() error: %s", (const char *)_sys_errlist[errno]);
                                //abort();
                            }
                            delete (AcceptedConnectedInfo *)state->asyncResult;
                            info->states_mutex->Unlock();
                            printf("marked_for_deletion 1\n");
                            continue;
                        }*/
                        if(info->events[i].events & (EPOLLHUP | EPOLLERR/* | EPOLLRDHUP*/)) // EPOLLRDHUP
                        {
                            //info->states_mutex->Lock();
                            //info->current_states--; // conisder lock-free counterpart
                            LockFree::LockFreeCounter::AtomicDecrement(&info->current_states);
                            //info->available--;
                            state->hasClosed = true;
                            //state->errorCode = errno;
                            if(::epoll_ctl(info->epollfd, EPOLL_CTL_DEL, state->sock->get_Handle(), &temp_event) == -1)
                            {
                                printf("epoll_ctl() error: %s", (const char *)_sys_errlist[errno]);
                                //abort();
                            }

                            /*if(state->marked_for_deletion)
                            {
                                delete (AcceptedConnectedInfo *)state->asyncResult;
                                info->states_mutex->Unlock();
                                printf("marked_for_deletion 2\n");
                                continue;
                            }*/
                            //info->states_mutex->Unlock();
                            AcceptedConnectedInfo *acceptedConnectedSockInfo = (AcceptedConnectedInfo *)state->asyncResult;
                            //acceptedConnectedSockInfo->ReaderWriterThreadInfo_instance_ref = null; // This socket is removed from connector threads due to error.
                            acceptedConnectedSockInfo->IsCompleted = true;
                            acceptedConnectedSockInfo->callback(acceptedConnectedSockInfo); // Place it inside a try/catch block.
                            continue;
                        }
                        if(state->toRead)
                        {
                        Here1:
                            if(NonBlockingRecv(state, info->states_mutex) < 0)
								continue;

                            if(state->errorCode == EINTR)
                                printf("NonBlockingRecv state->errorCode == EINTR\n");

                            if(state->hasClosed)
                            {
                                //info->states_mutex->Lock();
                                //info->current_states--; // conisder lock-free counterpart
                                LockFree::LockFreeCounter::AtomicDecrement(&info->current_states);
                                //info->available--;
                                //info->states_mutex->Unlock();
                                if(::epoll_ctl(info->epollfd, EPOLL_CTL_DEL, state->sock->get_Handle(), &temp_event) == -1)
                                {
                                    printf("epoll_ctl() error: %s", (const char *)_sys_errlist[errno]);
                                    //abort();
                                }
                                //info->states_mutex->Unlock();
                                AcceptedConnectedInfo *acceptedConnectedSockInfo = (AcceptedConnectedInfo *)state->asyncResult;
                                //acceptedConnectedSockInfo->ReaderWriterThreadInfo_instance_ref = null; // This socket is removed from connector threads due to error.
                                acceptedConnectedSockInfo->IsCompleted = true;
                                acceptedConnectedSockInfo->callback(acceptedConnectedSockInfo); // Place it inside a try/catch block.
                            }
                            else if(state->hasError && state->errorCode == EINTR)
                                 goto Here1;
                            else if(state->hasError && state->errorCode != EINTR)
                            {
                                //info->states_mutex->Lock();
                                //info->current_states--; // conisder lock-free counterpart
                                LockFree::LockFreeCounter::AtomicDecrement(&info->current_states);
                                //info->available--;
                                //info->states_mutex->Unlock();
                                if(::epoll_ctl(info->epollfd, EPOLL_CTL_DEL, state->sock->get_Handle(), &temp_event) == -1)
                                {
                                    printf("epoll_ctl() error: %s", (const char *)_sys_errlist[errno]);
                                    //abort();
                                }
                                //info->states_mutex->Unlock();
                                AcceptedConnectedInfo *acceptedConnectedSockInfo = (AcceptedConnectedInfo *)state->asyncResult;
                                //acceptedConnectedSockInfo->ReaderWriterThreadInfo_instance_ref = null; // This socket is removed from connector threads due to error.
                                acceptedConnectedSockInfo->IsCompleted = true;
                                acceptedConnectedSockInfo->callback(acceptedConnectedSockInfo); // Place it inside a try/catch block.
                            }
                            else
                            {
                                //info->states_mutex->Unlock();
                                AcceptedConnectedInfo *acceptedConnectedSockInfo = (AcceptedConnectedInfo *)state->asyncResult;
                                acceptedConnectedSockInfo->IsCompleted = true;
                                acceptedConnectedSockInfo->callback(acceptedConnectedSockInfo); // Place it inside a try/catch block.
                            }
                            continue;
                        }

                        if(state->toWrite)
                        {
                        Here2:
                            if(NonBlockingSend(state, info->states_mutex) < 0)
								continue;

                            if(state->errorCode == EINTR)
                                printf("NonBlockingSend state->errorCode == EINTR\n");

                            if(state->hasClosed)
                            {
                                //info->states_mutex->Lock();
                                //info->current_states--; // conisder lock-free counterpart
                                LockFree::LockFreeCounter::AtomicDecrement(&info->current_states);
                                //info->available--;
                                //info->states_mutex->Unlock();
                                if(::epoll_ctl(info->epollfd, EPOLL_CTL_DEL, state->sock->get_Handle(), &temp_event) == -1)
                                {
                                    printf("epoll_ctl() error: %s", (const char *)_sys_errlist[errno]);
                                    //abort();
                                }
                                //info->states_mutex->Unlock();
                                AcceptedConnectedInfo *acceptedConnectedSockInfo = (AcceptedConnectedInfo *)state->asyncResult;
                                //acceptedConnectedSockInfo->ReaderWriterThreadInfo_instance_ref = null; // This socket is removed from connector threads due to error.
                                acceptedConnectedSockInfo->IsCompleted = true;
                                acceptedConnectedSockInfo->callback(acceptedConnectedSockInfo); // Place it inside a try/catch block.
                            }
                            else if(state->hasError && state->errorCode == EINTR)
                                 goto Here2;
                            else if(state->hasError && state->errorCode != EINTR)
                            {
                                //info->states_mutex->Lock();
                                //info->current_states--; // conisder lock-free counterpart
                                LockFree::LockFreeCounter::AtomicDecrement(&info->current_states);
                                //info->available--;
                                //info->states_mutex->Unlock();
                                if(::epoll_ctl(info->epollfd, EPOLL_CTL_DEL, state->sock->get_Handle(), &temp_event) == -1)
                                {
                                    printf("epoll_ctl() error: %s", (const char *)_sys_errlist[errno]);
                                    //abort();
                                }
                                //info->states_mutex->Unlock();
                                AcceptedConnectedInfo *acceptedConnectedSockInfo = (AcceptedConnectedInfo *)state->asyncResult;
                                //acceptedConnectedSockInfo->ReaderWriterThreadInfo_instance_ref = null; // This socket is removed from connector threads due to error.
                                acceptedConnectedSockInfo->IsCompleted = true;
                                acceptedConnectedSockInfo->callback(acceptedConnectedSockInfo); // Place it inside a try/catch block.
                            }
                            else
                            {
                                //info->states_mutex->Unlock();
                                AcceptedConnectedInfo *acceptedConnectedSockInfo = (AcceptedConnectedInfo *)state->asyncResult;
                                acceptedConnectedSockInfo->IsCompleted = true;
                                acceptedConnectedSockInfo->callback(acceptedConnectedSockInfo); // Place it inside a try/catch block.
                            }
                            continue;
                        }
                    }

                    //Thread::Sleep(1);
                }

                info->cleanupBarrier->SignalAndWait();
                return arg;
            }
            //----------------------------------------------------
            static void signal_handler_connector(int sig)
            {
                //printf("signal_handler_connector\n");
                /*sigset_t signal_set;
                sigfillset(&signal_set);
                sem_post(&sem);*/
                //signal(SIGUSR1, signal_handler_reader_writer);//
                //my_signal(SIGUSR1, signal_handler_connector);
                return;
            }
            void *AsyncSocketRuntimeManager::connector_proc(void *arg)
            {
                my_signal(SIGUSR1, signal_handler_connector);
                //signal(SIGUSR1, signal_handler_reader_writer);

                //__sync_synchronize();
                //printf("I'm the conector thread.\n"); //
                ReaderWriterThreadInfo *info = (ReaderWriterThreadInfo *)arg;

                register Int32 i, j = 0;
                Int32 n, nError;

                /*Double elapsed = 0;
                time_t t1;
                time_t t2;*/

                struct epoll_event temp_event;

                bool __stopRequested = info->StopRequested;
                //printf("state 1.\n");//
                while(!GetBoolFlagAtomic(&info->StopRequested))
                {
                    j++;

                    //printf("count: %d\n", count);//
                    //printf("before epoll_wait()\n");//
                    DeletionPhaseForMarkedSockets(info);


                    SetBoolFlagAtomic(&info->epoll_has_executed, true);
                    //t1 = ::time(null);
                    n = ::epoll_wait(info->epollfd, info->events, __epoll_event_num__, -1/*NonBlockingConnectTimeout * 1000*/);
                    //t2 = ::time(null);
                    SetBoolFlagAtomic(&info->epoll_has_executed, false);
                    //elapsed = ::difftime(t2, t1);
                    //printf("after epoll_wait()\n");//

                    //printf("elapsed: %d\n",  elapsed);

                    if(n == 0)//
                    {
                        //printf("connector_proc 1\n");
                        continue;
                    }

                    if(n == -1)
                    {
                        nError = errno;
                        if(nError == EINTR)
                        {
                            //printf("connector_proc 2\n");
                            //printf("EINTR\n");
                            continue;
                        }
                        //printf("connector_proc 3\n");
                        break;
                    }

                    for(i = 0 ; i < n ; i++)
                    {
                        //info->states_mutex->Lock();
                        fd_state *state = (fd_state *)info->events[i].data.ptr;
                        //info->states_mutex->Unlock();

                        if(state == null)
                        {
                            //info->states_mutex->Unlock();
                            continue;
                        }

                        info->states_mutex->Lock();
                        if(state->marked_for_deletion)
                        {
							if(!state->closed)
							{
								try { state->sock->Shutdown(System::Net::Sockets::Both); } catch(Exception &e) { } catch(...) {}
								try { state->sock->Close(); } catch(Exception &e) { } catch(...) {}
								state->closed = true;
							}
                            info->states_mutex->Unlock();
                            continue;
                        }
                        info->states_mutex->Unlock();

                        if(info->events[i].events & (EPOLLHUP | EPOLLERR/* | EPOLLRDHUP*/))
                        {
                            //printf("connector_proc 4\n");
                            //info->current_states--;
                            //info->available--;
                            LockFree::LockFreeCounter::AtomicDecrement(&info->current_states);
                            LockFree::LockFreeCounter::AtomicDecrement(&info->available);
                            state->hasClosed = true;
                            //state->hasError = true;
                            //state->errorCode = errno;
                            //info->states_mutex->Unlock();
                            if(::epoll_ctl(info->epollfd, EPOLL_CTL_DEL, state->sock->get_Handle(), &temp_event) == -1)
                            {
                                printf("epoll_ctl() error: %s", (const char *)_sys_errlist[errno]);
                                //abort();
                            }
                            AcceptedConnectedInfo *acceptedConnectedSockInfo = (AcceptedConnectedInfo *)state->asyncResult;
                            acceptedConnectedSockInfo->ReaderWriterThreadInfo_instance_ref = null; // This socket is removed from connector threads due to error.
                            acceptedConnectedSockInfo->IsCompleted = true;
                            acceptedConnectedSockInfo->callbackCalledByRuntimeManager = true;
                            acceptedConnectedSockInfo->callback(acceptedConnectedSockInfo); // Place it inside a try/catch block.
                            continue;
                        }
                        else if(info->events[i].events & EPOLLOUT)
                        {
                            //printf("connector_proc 5\n");
                            Int32 error;
                            socklen_t optLen = sizeof(Int32);
                            if(::getsockopt(state->sock->get_Handle(), SOL_SOCKET, SO_ERROR, (char *)&error, &optLen) == 0)
                            {
                                if(error == 0)
                                {
                                    //printf("connector_proc 6\n");
                                    state->hasError = false;
                                    state->hasClosed = false;
                                }
                                else
                                {
                                    //printf("connect error 7\n");
                                    state->hasError = true;
                                    state->hasClosed = false;
                                    state->errorCode = error;
                                }
                            }
                            else
                            {
                                //printf("connector_proc 8\n");
                                state->hasError = true;
                                state->hasClosed = false;
                                state->errorCode = errno;
                            }
                            //info->current_states--;
                            //info->available--;
                            LockFree::LockFreeCounter::AtomicDecrement(&info->current_states);
                            LockFree::LockFreeCounter::AtomicDecrement(&info->available);
                            //info->states_mutex->Unlock();
                            if(::epoll_ctl(info->epollfd, EPOLL_CTL_DEL, state->sock->get_Handle(), &temp_event) == -1)
                            {
                                printf("epoll_ctl() error: %s", (const char *)_sys_errlist[errno]);
                                //abort();
                            }

                            AcceptedConnectedInfo *acceptedConnectedSockInfo = (AcceptedConnectedInfo *)state->asyncResult;
                            acceptedConnectedSockInfo->ReaderWriterThreadInfo_instance_ref = null; // This socket is removed from connector threads due to error.
                            acceptedConnectedSockInfo->IsCompleted = false;
                            acceptedConnectedSockInfo->callbackCalledByRuntimeManager = true;
                            //printf("from connector thread, completed %p\n", acceptedConnectedSockInfo->ReaderWriterThreadInfo_instance_ref);
                            acceptedConnectedSockInfo->callback(acceptedConnectedSockInfo); // Place it inside a try/catch block
                            continue;
                        }
                    }

                    //Thread::Sleep(1);
                }

                //asm volatile("" ::: "memory");
                //asm volatile("mfence");

                info->cleanupBarrier->SignalAndWait();
                return arg;
            }
			//----------------------------------------------------
			ReaderWriterThreadInfo *AsyncSocketRuntimeManager::AssignSocketInstanceToReaderWriterThreadPool(ReaderWriterThreadInfo **threads, Int32 ThreadPoolNum, struct fd_state *state, bool GuaranteeMulticoreExecution, Long volatile *LastCoreAssigned_XThreads)
			{
                Long CoreIndex = 0;
                Long to = ThreadPoolNum;
                bool FirstTimeSearch = false;
			    if(GuaranteeMulticoreExecution)
			    {
			        Long volatile current = -10;
			        while(!Parvicursor_AtomicCompareAndSwap(&current, LastCoreAssigned_XThreads, CoreIndex))
			        {
			            if(current == ThreadPoolNum - 1)
                            CoreIndex = 0;
                        else
                            CoreIndex = current + 1;
			        }
			    }

			    ReaderWriterThreadInfo *info;
			    register Long i = CoreIndex;
            here:

                for( ; i < to ; i++)
                {
                    info = threads[i];
                    //info->states_mutex->Lock();
                    {
                        Long volatile num = -10;
                        Parvicursor_AtomicCompareAndSwap(&num, &info->current_states, -1);
                        if(/*info->current_states*/num == __epoll_event_num__)
                        {
                            //info->states_mutex->Unlock();
                            continue;
                        }

                        ((AcceptedConnectedInfo *)state->asyncResult)->ReaderWriterThreadInfo_instance_ref = (Object *)info;
                        struct epoll_event event;
                        event.data.fd = state->sock->get_Handle();
                        event.data.ptr = state;

                        if(state->toRead)
                            event.events = EPOLLIN | EPOLLONESHOT;
                        else if(state->toWrite)
                            event.events = EPOLLOUT | EPOLLONESHOT;
                        else
                            event.events = EPOLLIN | EPOLLOUT | EPOLLONESHOT;

                        if(::epoll_ctl(info->epollfd, EPOLL_CTL_ADD, state->sock->get_Handle(), &event) == -1)
                        {
                            //info->states_mutex->Unlock();
                            throw Exception(String("epoll_ctl() error: ") + (const char *)_sys_errlist[errno]);
                            //abort();
                        }

                        //info->current_states++;
                        LockFree::LockFreeCounter::AtomicIncrement(&info->current_states);
                        if(state->toRead || state->toWrite)
                            LockFree::LockFreeCounter::AtomicIncrement(&info->available);
                            //info->available++; //
                        ///if((state->toRead || state->toWrite) && (info->available - 1 == 0 || info->current_states - 1 == 0))
                        ///    info->states_cv->Signal();

                        SignalReaderWriterThreadToUnblockEpollWait(info, false);
                        state->CoreID = CoreIndex;
                        return info;
                    }
                    //info->states_mutex->Unlock();
                }

                if(GuaranteeMulticoreExecution && FirstTimeSearch)
                {
                    if((ThreadPoolNum - CoreIndex - 1 >= 0) && CoreIndex != 0)
                    {
                        i = 0;
                        to = CoreIndex;
                        FirstTimeSearch = false;
                        goto here;
                    }
                }
			    return null;
			}
			//----------------------------------------------------
            bool AsyncSocketRuntimeManager::PullSocketFromThread(fd_state *state, ReaderWriterThreadInfo *info)
            {
                //info->states_mutex->Lock();
                struct epoll_event event;

                if(::epoll_ctl(info->epollfd, EPOLL_CTL_DEL, state->sock->get_Handle(), &event) == -1)
                {
                    if(errno == ENOENT)
                        return false;

                    printf("epoll_ctl() error: %s\n", (const char *)_sys_errlist[errno]);
                    return false;
                    //throw Exception(String("epoll_ctl() error: ") + (const char *)_sys_errlist[errno]);
                    //abort();
                }
                //info->current_states--;
                LockFree::LockFreeCounter::AtomicDecrement(&info->current_states);
                if(state->toRead || state->toWrite)
                    //info->available--;
                    LockFree::LockFreeCounter::AtomicDecrement(&info->available);
                //if(info->available == 0)
                    //info->states_cv->Signal();
                //if(*current_number_state != 0)
                //cv->Signal();
                //info->states_mutex->Unlock();
                return true;
            }
            //----------------------------------------------------
            bool AsyncSocketRuntimeManager::PushSocketToThread(fd_state *state, ReaderWriterThreadInfo *info)
            {
                //info->states_mutex->Lock();
                struct epoll_event event;
                event.data.fd = state->sock->get_Handle();
                event.data.ptr = state;

                if(state->toRead)
                    event.events = EPOLLIN | EPOLLONESHOT;
                else if(state->toWrite)
                    event.events = EPOLLOUT | EPOLLONESHOT;
                else
                    event.events = EPOLLIN | EPOLLOUT | EPOLLONESHOT;

                if(::epoll_ctl(info->epollfd, EPOLL_CTL_ADD, state->sock->get_Handle(), &event) == -1)
                {
                    if(errno == EEXIST)
                        return true;
                    throw Exception(String("epoll_ctl() error: ") + (const char *)_sys_errlist[errno]);
                    //abort();
                }

                //info->current_states++;
                LockFree::LockFreeCounter::AtomicIncrement(&info->current_states);
                if(state->toRead || state->toWrite)
                    //info->available++; //
                    LockFree::LockFreeCounter::AtomicIncrement(&info->available);
                ///if((state->toRead || state->toWrite) && (info->available - 1 == 0 || info->current_states - 1 == 0))
                ///    info->states_cv->Signal();
                //if(*current_number_state - 1 == 0)
                //info->states_cv->Signal(); // consider "available" variable, here.
                return true;
                //info->states_mutex->Unlock();
            }
            //----------------------------------------------------
            void AsyncSocketRuntimeManager::SignalReaderWriterThreadToUnblockEpollWait(ReaderWriterThreadInfo *info, bool enable)
            {
                return;
                if(!enable)
                    return;

                if(GetBoolFlagAtomic(&info->epoll_has_executed))
                {
                    int status;
                    //status = pthread_mutex_lock(&mut);
                    //printf("GetReaderSelectFlag true\n"); //
                    //printf("SignalReaderWriterThreadToUnblockEpollWait\n");
                    int ret = ::pthread_kill(info->thread->thread, SIGUSR1);
                    if(ret != 0)
                    {
                        printf("pthread_kill error: %s\n", (const char *)_sys_errlist[errno]);
                    }

                    /*// Wait for the victim to acknowledge suspension.
                    while ((status = sem_wait(&sem)) != 0)
                    {
                        if (errno != EINTR)
                        {
                            break;
                        }
                    }

                    status = pthread_mutex_unlock(&mut);*/
                }
                /*else
                    printf("GetReaderSelectFlag false\n");*/
            }
            //----------------------------------------------------
            void AsyncSocketRuntimeManager::DeletionPhaseForMarkedSockets(ReaderWriterThreadInfo *info)
            {
                /*info->states_mutex->Lock();
                AcceptedConnectedInfo *temp;
                if(info->queue->get_Count() > 0)
                    while(( temp = (AcceptedConnectedInfo *)info->queue->RemoveBeginning() ) != null)
                    {
                        if(temp->closeCallback != null)
                        {
                            //printf("delete callback: %p\n", temp->closeCallback);
                            temp->EndXHasCalled = false;
                            temp->opcode = BeginClose_op;
                            temp->AsyncState = temp->closeState;
                            temp->closeCallback(temp);
                        }
                        delete temp;
                    }
                info->states_mutex->Unlock();*/

                AcceptedConnectedInfo *temp;
                while(true)
                {
                    info->states_mutex->Lock();
                    temp = (AcceptedConnectedInfo *)info->queue->RemoveBeginning();
                    if(temp == null)
                    {
                        info->states_mutex->Unlock();
                        break;
                    }
                    info->states_mutex->Unlock();
                    if(temp->closeCallback != null)
                    {
                        //printf("delete callback: %p\n", temp->closeCallback);
                        temp->EndXHasCalled = false;
                        temp->opcode = BeginClose_op;
                        temp->AsyncState = temp->closeState;
                        temp->closeCallback(temp);
                    }
                    delete temp;
                }
            }
			//----------------------------------------------------
            ReaderWriterThreadInfo **AsyncSocketRuntimeManager::get_ReaderWriterThreads() const
            {
                return readerWriterThreads;
            }
            //----------------------------------------------------
            ReaderWriterThreadInfo **AsyncSocketRuntimeManager::get_ConnectorThreads() const
            {
                return connectorThreads;
            }
            //----------------------------------------------------
        };
    };
};
//**************************************************************************************************************//
