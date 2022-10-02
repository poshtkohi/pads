#include "AsyncSocket.h"

/*class Test
{
	public: int x;
	public: int y;
	public: void Run(Long i, Long j)
	{
		Test volatile *t1 = new Test();
		t1->x = 1;
		t1->y = 2;

		//Test volatile *t1 = null;
		printf("hello1\n");
		Test volatile *t2 = new Test();
		bool ret = Parvicursor_AtomicCompareAndSwap(&t2, &t1, (Long)t2);
		if(t2 == null)
			printf("t2 is null\n");
		else
			printf("ret: %d t2->x: %d t2->y: %d\n", (int)ret, t2->x, t2->y);
		printf("hello2\n");
	}
};*/

//**************************************************************************************************************//
namespace System
{
    namespace Net
    {
        namespace Sockets
        {
            //----------------------------------------------------
            static LockFreeCounter currentConnections_clientModeRuntime = LockFreeCounter();
            static AsyncSocketRuntimeManager volatile *clientModeRuntime = null;
            static AsyncSocketRuntimeManager *dummy_AsyncSocketRuntimeManager = new AsyncSocketRuntimeManager(); // This pointer must be freed after full termination of 'clientModeRuntime''s threads.
            //----------------------------------------------------
            inline AsyncSocketRuntimeManager *ClientModeRuntime_init(void)
            {
				//printf("debug-AsyncSocket1\n"); //
				//Test *test = new Test();
				//test->Run(10000, 100000);
				//return null;
                /*-- Allocates clientModeRuntime if it is null. --*/
                AsyncSocketRuntimeManager volatile *current = dummy_AsyncSocketRuntimeManager;
                AsyncSocketRuntimeManager *temp = null;
                bool instantiated = false;
				//printf("last - current: %p clientModeRuntime: %p temp: %p\n", &current, &clientModeRuntime, &temp);
				//printf("debug-AsyncSocket2\n"); //
                while(!Parvicursor_AtomicCompareAndSwap(&current, &clientModeRuntime, (Long)temp) )
                {
					//printf("debug-AsyncSocket3\n"); //
                    if(current == null && !instantiated)
                    {
						//printf("debug-AsyncSocket4\n"); //
                        temp = new AsyncSocketRuntimeManager(null, ClientModeRuntime_ThreadPool_Num, false);
                        instantiated = true;
                    }
                    if(current != null)
                        break;
					//printf("debug-AsyncSocket5\n"); //
                }
                if(current == null)
                    current = temp;

				//printf("debug-AsyncSocket6\n"); //
                // The AsyncSocketRuntimeManager instance has already been allocated by another thread.
                // Thus, we must delete out reference allocation stored within 'temp'.
                // This shows a real lock-free algorithm for one-time allocation of a global object.
                if(instantiated && current != temp)
                    delete temp;
				//printf("debug-AsyncSocket7\n"); //

                // We now run our allocated AsyncSocketRuntimeManager instance.
                if(instantiated && current == temp)
                    ((AsyncSocketRuntimeManager *)current)->Run();

				//printf("debug-AsyncSocket8\n"); //

                return (AsyncSocketRuntimeManager *)current;
            }
            //----------------------------------------------------
            inline void ClientModeRuntime_destroy(void)
            {
                AsyncSocketRuntimeManager volatile *current = dummy_AsyncSocketRuntimeManager;
                AsyncSocketRuntimeManager *temp = null;
                bool nullified = false;
                while(!Parvicursor_AtomicCompareAndSwap(&current, &clientModeRuntime, (Long)temp) )
                {
                    if(current == null)
                        return;
                    if(current != null && !nullified)
                    {
                        temp = null;
                        nullified = true;
                    }
                    else
                        break;
                }

                temp = (AsyncSocketRuntimeManager *)current;
                temp->Dispose();
                delete temp;
            }
            //----------------------------------------------------
            AsyncSocket::AsyncSocket(AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType)
            {
                internalSocket = null;
                serverModeRuntime = null;
                serverMode = false;
                isListening = false;
				isConnected = false;
				isAccepted = false;
				connectRequested = false;
				GuaranteeMulticoreExecution = false;
				isClosedByRuntimeManager = false;
                disposed = false;
                internalSocket = new Socket(addressFamily, socketType, protocolType);
                asyncResult = null;
                isAsynchCloseRequested = false;
                mutex = new Mutex();
            }
            //----------------------------------------------------
            AsyncSocket::AsyncSocket(Socket *sock)
            {
                if(sock == null)
                    throw ArgumentNullException("sock");

                //if(AsyncResult == null)
                 //   throw ArgumentNullException("AsyncResult");

                serverModeRuntime = null;
                internalSocket = sock;
                asyncResult = null;
                serverMode = false;
                GuaranteeMulticoreExecution = false;
                isClosedByRuntimeManager = false;
                isAsynchCloseRequested = false;
                disposed = false;
                mutex = new Mutex();
            }
            //----------------------------------------------------
			AsyncSocket::~AsyncSocket()
			{
				if(!disposed)
					Close();

                //mutex->Unlock();
                delete mutex;
			}
			//----------------------------------------------------
            void AsyncSocket::Close()
			{
                mutex->Lock();
				if(disposed)
				{
				    mutex->Unlock();
					//throw ObjectDisposedException("AsyncSocket", "The AsyncSocket has been disposed");
                    return ;
				}

				//disposed = true;
				//printf("Close()\n");

                if(serverMode)
                {
                    if(isListening) // here, we must terminate all threads and release all allocated resources
                    {
                        serverModeRuntime->Dispose();
                        delete serverModeRuntime;
                    }
                    if(isAccepted)
                    {
                        if(asyncResult !=  null)
                        {
                            AcceptedConnectedInfo *info = (AcceptedConnectedInfo *)asyncResult;
                            AsyncSocketRuntimeManager *manager = (AsyncSocketRuntimeManager *)info->AsyncSocketRuntimeManager_instance_ref;
                            ReaderWriterThreadInfo *threadInstance = (ReaderWriterThreadInfo *)info->ReaderWriterThreadInfo_instance_ref;

                            if(manager != null && threadInstance != null /*&& !isClosedByRuntimeManager*/ && manager != null) // Removes from reader/writer threads
                            {
								threadInstance->states_mutex->Lock();
                                if(!isClosedByRuntimeManager)
									bool found = AsyncSocketRuntimeManager::PullSocketFromThread(info->fdState, threadInstance);
								if(!info->fdState->closed) // double lock-free
								{
									try { info->fdState->sock->Shutdown(System::Net::Sockets::Both); } catch(Exception &e) {} catch(...) {}
									try { info->fdState->sock->Close(); } catch(Exception &e) {} catch(...) {}
									info->fdState->closed = true;
								}
								else
									info->fdState->closed = false;
								threadInstance->queue->Add((const Object *)info);
								info->fdState->marked_for_deletion = true;
								//if(found)
								threadInstance->states_mutex->Unlock();
								AsyncSocketRuntimeManager::SignalReaderWriterThreadToUnblockEpollWait(threadInstance, true);
                            }

                            if(threadInstance == null/* || isClosedByRuntimeManager*/)
                                delete ((AcceptedConnectedInfo *)asyncResult); // Deletes handler and allocated internal data structures.

                            asyncResult = null;
                        }
                    }
                }
                else // clientMode
                {
                    if(asyncResult !=  null)
                    {
                        AcceptedConnectedInfo *info = (AcceptedConnectedInfo *)asyncResult;
                        AsyncSocketRuntimeManager *manager = (AsyncSocketRuntimeManager *)info->AsyncSocketRuntimeManager_instance_ref;
                        ReaderWriterThreadInfo *threadInstance = (ReaderWriterThreadInfo *)info->ReaderWriterThreadInfo_instance_ref;
                        if(manager != null && threadInstance != null /*&& !isClosedByRuntimeManager*/ && ((!isConnected && connectRequested) || isConnected) && manager != null) // remove from connector or reader/writer threads
                        {
							threadInstance->states_mutex->Lock();
                            if(!isClosedByRuntimeManager)
								bool found = AsyncSocketRuntimeManager::PullSocketFromThread(info->fdState, threadInstance);

							if(!info->fdState->closed) // double lock-free
							{
								try { info->fdState->sock->Shutdown(System::Net::Sockets::Both); } catch(Exception &e) {} catch(...) {}
								try { info->fdState->sock->Close(); } catch(Exception &e) {} catch(...) {}
								info->fdState->closed = true;
							}
							else
								info->fdState->closed = false;
							threadInstance->queue->Add((const Object *)info);
							info->fdState->marked_for_deletion = true;
							//if(found)
							threadInstance->states_mutex->Unlock();
							AsyncSocketRuntimeManager::SignalReaderWriterThreadToUnblockEpollWait(threadInstance, true);
                        }

                        if(threadInstance == null/* || isClosedByRuntimeManager*/)
                            delete ((AcceptedConnectedInfo *)asyncResult); // Deletes handler and allocated internal data structures.

                        asyncResult = null;
                    }
                }

                serverMode = false;
				disposed = true;
				mutex->Unlock();
				return ;
			}
            //----------------------------------------------------
            void AsyncSocket::BeginClose(AsyncCallback callback, Object *state)
			{
                mutex->Lock();
				if(disposed)
				{
				    mutex->Unlock();
					//throw ObjectDisposedException("AsyncSocket", "The AsyncSocket has been disposed");
                    return ;
				}

				//disposed = true;
				//printf("Close()\n");

                if(serverMode)
                {
                    if(isListening) // here, we must terminate all threads and release all allocated resources
                    {
                        serverModeRuntime->Dispose();
                        delete serverModeRuntime;
                    }
                    if(isAccepted)
                    {
                        if(asyncResult !=  null)
                        {
                            AcceptedConnectedInfo *info = (AcceptedConnectedInfo *)asyncResult;
                            AsyncSocketRuntimeManager *manager = (AsyncSocketRuntimeManager *)info->AsyncSocketRuntimeManager_instance_ref;
                            ReaderWriterThreadInfo *threadInstance = (ReaderWriterThreadInfo *)info->ReaderWriterThreadInfo_instance_ref;

                            if(manager != null && threadInstance != null /*&& !isClosedByRuntimeManager*/ && manager != null) // Removes from reader/writer threads
                            {
								threadInstance->states_mutex->Lock();

                                if(!isClosedByRuntimeManager)
									bool found = AsyncSocketRuntimeManager::PullSocketFromThread(info->fdState, threadInstance);

								if(!info->fdState->closed) // double lock-free
								{
									try { info->fdState->sock->Shutdown(System::Net::Sockets::Both); } catch(Exception &e) {} catch(...) {}
									try { info->fdState->sock->Close(); } catch(Exception &e) {} catch(...) {}
									info->fdState->closed = true;
								}
								else
									info->fdState->closed = false;

                                info->closeCallback = callback;
                                info->closeState = state;
								threadInstance->queue->Add((const Object *)info);
								info->fdState->marked_for_deletion = true;
								//if(found)
								threadInstance->states_mutex->Unlock();
								AsyncSocketRuntimeManager::SignalReaderWriterThreadToUnblockEpollWait(threadInstance, true);
                            }

                            if(threadInstance == null/* || isClosedByRuntimeManager*/)
                            {
                                if(!info->fdState->closed) // double lock-free
                                {
                                    try { info->fdState->sock->Shutdown(System::Net::Sockets::Both); } catch(Exception &e) {} catch(...) {}
                                    try { info->fdState->sock->Close(); } catch(Exception &e) {} catch(...) {}
                                    info->fdState->closed = true;
                                }
                                else
                                    info->fdState->closed = false;

                                info->closeCallback = callback;
                                info->closeState = state;
                                info->AsyncState = state;
                                info->closeCallback(info);
                                delete ((AcceptedConnectedInfo *)asyncResult); // Deletes handler and allocated internal data structures.
                            }

                            asyncResult = null;
                        }
                    }
                }
                else // clientMode
                {
                    if(asyncResult !=  null)
                    {
                        AcceptedConnectedInfo *info = (AcceptedConnectedInfo *)asyncResult;
                        AsyncSocketRuntimeManager *manager = (AsyncSocketRuntimeManager *)info->AsyncSocketRuntimeManager_instance_ref;
                        ReaderWriterThreadInfo *threadInstance = (ReaderWriterThreadInfo *)info->ReaderWriterThreadInfo_instance_ref;
                        if(manager != null && threadInstance != null /*&& !isClosedByRuntimeManager*/ && ((!isConnected && connectRequested) || isConnected) && manager != null) // remove from connector or reader/writer threads
                        {
							threadInstance->states_mutex->Lock();

                            if(!isClosedByRuntimeManager)
								bool found = AsyncSocketRuntimeManager::PullSocketFromThread(info->fdState, threadInstance);

							if(!info->fdState->closed) // double lock-free
							{
								try { info->fdState->sock->Shutdown(System::Net::Sockets::Both); } catch(Exception &e) {} catch(...) {}
								try { info->fdState->sock->Close(); } catch(Exception &e) {} catch(...) {}
								info->fdState->closed = true;
							}
							else
								info->fdState->closed = false;

                            info->closeCallback = callback;
                            info->closeState = state;
							threadInstance->queue->Add((const Object *)info);
							info->fdState->marked_for_deletion = true;
							//if(found)
							threadInstance->states_mutex->Unlock();
							AsyncSocketRuntimeManager::SignalReaderWriterThreadToUnblockEpollWait(threadInstance, true);
                        }

                        if(threadInstance == null/* || isClosedByRuntimeManager*/)
                        {
                            if(!info->fdState->closed) // double lock-free
							{
								try { info->fdState->sock->Shutdown(System::Net::Sockets::Both); } catch(Exception &e) {} catch(...) {}
								try { info->fdState->sock->Close(); } catch(Exception &e) {} catch(...) {}
								info->fdState->closed = true;
							}
							else
								info->fdState->closed = false;

                            info->closeCallback = callback;
                            info->closeState = state;
                            info->AsyncState = state;
                            info->closeCallback(info);
                            delete ((AcceptedConnectedInfo *)asyncResult); // Deletes handler and allocated internal data structures.
                        }

                        asyncResult = null;
                    }
                }

                serverMode = false;
				disposed = true;
				mutex->Unlock();
				return ;
			}
            //----------------------------------------------------
			void AsyncSocket::EndClose(IAsyncResult *asyncResult)
			{
                if(asyncResult == null)
                    throw ArgumentNullException("asyncResult");

                AcceptedConnectedInfo *info = (AcceptedConnectedInfo *)asyncResult;

                AsynchMethodOpcodes opcode = info->opcode;
                bool EndXHasCalled = info->EndXHasCalled;
                info->EndXHasCalled = true;
                info->IsCompleted = true;

                if(opcode != BeginClose_op)
                    throw ArgumentException("asyncResult", "asyncResult was not returned by a call to the BeginConnect method.");

                if(EndXHasCalled)
                    throw InvalidOperationException("EndClose was previously called for the asynchronous close.");

                return;
			}
			//----------------------------------------------------
			IAsyncResult *AsyncSocket::BeginAccept(AsyncCallback callback, Object *state)
			{
                if(disposed)
					throw ObjectDisposedException("AsyncSocket", "The AsyncSocket has been disposed");

                if(callback == null)
                    throw ArgumentNullException("callback");

                if(isConnected)
                    throw InvalidOperationException("The operation was failed because the AsyncSocket instance is in an coonected state socket nor a listening one");

                if(isAccepted)
                    throw InvalidOperationException("The operation was failed because the AsyncSocket instance is in an accepted state socket nor a listening one");

			    if(!serverMode)
			    {
			        make_nonblocking(internalSocket);
                    serverModeRuntime = new AsyncSocketRuntimeManager(internalSocket, ServerModeRuntime_ThreadPool_Num/*Environment::get_ProcessorCount() * 2*/, true); //
                    serverModeRuntime->Run();
                    serverModeRuntime->set_AcceptCallback(callback, state);
                    serverMode = true;
			    }
			    return null;//
			}
            //----------------------------------------------------
			IAsyncResult *AsyncSocket::BeginAccept(AsyncCallback callback, Object *state, bool GuaranteeMulticoreExecution)
			{
			    this->GuaranteeMulticoreExecution = GuaranteeMulticoreExecution;
			    return BeginAccept(callback, state);
			}
            //----------------------------------------------------
			AsyncSocket *AsyncSocket::EndAccept(IAsyncResult *asyncResult)
			{
                if(disposed)
					throw ObjectDisposedException("AsyncSocket", "The AsyncSocket has been disposed");

                if(asyncResult == null)
                    throw ArgumentNullException("asyncResult");

                AcceptedConnectedInfo *info = (AcceptedConnectedInfo *)asyncResult;
                this->asyncResult = asyncResult;

                if(info->opcode != BeginAccept_op)
                    throw ArgumentException("asyncResult", "asyncResult was not returned by a call to the BeginAccept method.");

                if(info->EndXHasCalled)
                    throw InvalidOperationException("EndAccept method was previously called.");

                info->EndXHasCalled = true;
                info->IsCompleted = true;

                AsyncSocket *newsock = new AsyncSocket(info->AcceptedConnectedSocket);
                newsock->serverMode = true;
                newsock->isAccepted = true;
                newsock->isListening = false;
                newsock->GuaranteeMulticoreExecution = GuaranteeMulticoreExecution;
                newsock->asyncResult = asyncResult;
			    return newsock;
			}
            //----------------------------------------------------
			IAsyncResult *AsyncSocket::BeginConnect(IPEndPoint &remoteEP, AsyncCallback callback, Object *state)
			{
                if(&remoteEP == null)
						throw ArgumentNullException("remoteEP", "remoteEP is null");

                if(callback == null)
                    throw ArgumentNullException("callback");

                mutex->Lock();
                if(disposed)
                {
                    mutex->Unlock();//
					throw ObjectDisposedException("AsyncSocket", "The AsyncSocket has been disposed");
                }

                if(isListening)
                {
                    mutex->Unlock();//
                    throw InvalidOperationException("The operation was failed because the AsyncSocket instance is in the listening state");
                }

                if(isAccepted)
                {
                    mutex->Unlock();//
                    throw InvalidOperationException("The operation was failed because the AsyncSocket instance is in an accepted state socket nor a one to be connected");
                }

                if(isConnected)
                {
                    mutex->Unlock();//
                    throw InvalidOperationException("The operation was failed because the AsyncSocket instance has already been connected");
                }

                if(serverMode)
                {
                    mutex->Unlock();//
                    throw InvalidOperationException("The operation was failed because the AsyncSocket instance is in an accepted or listening state socket");
                }

                if(connectRequested)
                {
                    mutex->Unlock();//
                    throw InvalidOperationException("You have alreadey called BeginConnect on this AsyncSocket instance.");
                }

                serverMode = false;

                AsyncSocketRuntimeManager *manager = ClientModeRuntime_init();
                //__sync_synchronize();
                /*--Allocates fd_state for this new connection ----*/
                //int one = 1;
                //::setsockopt(internalSocket->get_Handle(), SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

                make_nonblocking(internalSocket);
                fd_state *fdState = alloc_fd_state(internalSocket, &currentConnections_clientModeRuntime);
                AcceptedConnectedInfo *info = new AcceptedConnectedInfo();
                info->callback = callback;
                info->AsyncState = state;
                info->AcceptedConnectedSocket = internalSocket;
                info->AsyncSocketRuntimeManager_instance_ref = (Object *)manager;
                info->ReaderWriterThreadInfo_instance_ref = null;
                info->fdState = fdState;
                info->fdState->toRead = false;
                info->fdState->toWrite = true;
                info->CompletedSynchronously = false;
                info->EndXHasCalled = false;
                info->opcode = BeginConnect_op;
                info->IsCompleted = false;
                fdState->asyncResult = info;
                this->asyncResult = info;

                // connect()
                connectRequested = true;

				if(info->fdState->sock->get_AddressFamily() == System::Net::Sockets::InterNetwork)
				{
				    ////printf("connect1\n");
					sockaddr_in clientService;
					struct in_addr addr;
					if(remoteEP.get_LongAddress() >= 0)
						addr.s_addr = remoteEP.get_LongAddress();
					else
						addr.s_addr = *(u_long *)remoteEP.get_Address().GetAddressBytes();
					clientService.sin_family = info->fdState->sock->get_AddressFamily();
					clientService.sin_port = htons(remoteEP.get_Port());
					clientService.sin_addr = addr;

					////printf("connect2\n");

					if(::connect((SOCKET)info->fdState->sock->get_Handle(), (SOCKADDR*) &clientService, sizeof(clientService)) == 0)
					{
						//printf("BeginConnect 1\n");
						printf("connect completed immediately\n");
						info->callbackCalledByRuntimeManager = false;
						info->IsCompleted = true;
						info->callback(info); // Place it inside a try/catch block.
						// connect completed immediately
						mutex->Unlock();
						return info;
					}
					else // error
					{
						int nError = errno;
						if(nError == EINPROGRESS)
						{
							//printf("BeginConnect 2\n");
							//printf("EINPROGRESS\n");//
							goto Here;
						}
						else
						{
							//printf("BeginConnect 3\n");
							info->fdState->hasClosed = false;
							info->fdState->hasError = true;
							info->fdState->errorCode = nError;
							info->IsCompleted = true;
							info->callbackCalledByRuntimeManager = false;
							info->callback(info); // Place it inside a try/catch block.
							// connect completed immediately
							mutex->Unlock();
							return info;
						}
					}
				}
				else //if(get_AddressFamily() == System::Net::Sockets::InterNetworkV6)
				{
					struct addrinfo *res = (struct addrinfo *)remoteEP.get_Address().GetAddressBytes();
					((struct sockaddr_in6 *)(res->ai_addr))->sin6_port = htons(remoteEP.get_Port());

					if(::connect((SOCKET)info->fdState->sock->get_Handle(), res->ai_addr, res->ai_addrlen) == 0)
					{
						//printf("BeginConnect 1\n");
						printf("connect completed immediately\n");
						info->callbackCalledByRuntimeManager = false;
						info->IsCompleted = true;
						info->callback(info); // Place it inside a try/catch block.
						// connect completed immediately
						mutex->Unlock();
						return info;
					}
					else // error
					{
						int nError = errno;
						if(nError == EINPROGRESS)
						{
							//printf("BeginConnect 2\n");
							goto Here;
						}
						else
						{
							//printf("BeginConnect 3\n");
							info->fdState->hasClosed = false;
							info->fdState->hasError = true;
							info->fdState->errorCode = nError;
							info->IsCompleted = true;
							info->callbackCalledByRuntimeManager = false;
							info->callback(info); // Place it inside a try/catch block.
							// connect completed immediately
							mutex->Unlock();
							return info;
						}
					}

				}


            Here:
                ReaderWriterThreadInfo *ReaderWriterThreadInfo_instance_ref = manager->AssignSocketInstanceToReaderWriterThreadPool(manager->get_ConnectorThreads(), ClientModeRuntime_ThreadPool_Num, info->fdState, GuaranteeMulticoreExecution, &manager->LastCoreAssigned_ConnectorThreads);
                if(ReaderWriterThreadInfo_instance_ref == null)
                {
                    mutex->Unlock();
                    throw OverflowException("Connect operations", "You have requested many outstanding connect operations. Try lateragain.");
                }
                //info->ReaderWriterThreadInfo_instance_ref = (Object *)ReaderWriterThreadInfo_instance_ref;
                //printf("BeginConnect() ReaderWriterThreadInfo_instance_ref: %p\n", ReaderWriterThreadInfo_instance_ref);//
                //printf("BeginConnect()\n");//
                //ReaderWriterThreadInfo_instance_ref->states_mutex->Unlock();
                /*------------------------------------------------*/
                this->asyncResult = info;
                mutex->Unlock();
                return info;
			}
			//----------------------------------------------------
			IAsyncResult *AsyncSocket::BeginConnect(IPEndPoint &remoteEP, AsyncCallback callback, Object *state, bool GuaranteeMulticoreExecution)
			{
			    this->GuaranteeMulticoreExecution = GuaranteeMulticoreExecution;
			    return BeginConnect(remoteEP, callback, state);
			}
            //----------------------------------------------------
			void AsyncSocket::EndConnect(IAsyncResult *asyncResult)
			{
                if(asyncResult == null)
                    throw ArgumentNullException("asyncResult");

                mutex->Lock();
				if(disposed)
				{
				    mutex->Unlock();//
					throw ObjectDisposedException("AsyncSocket", "The AsyncSocket has been disposed");
				}

                AcceptedConnectedInfo *info = (AcceptedConnectedInfo *)asyncResult;

                AsynchMethodOpcodes opcode = info->opcode;
                bool EndXHasCalled = info->EndXHasCalled;
                info->EndXHasCalled = true;
                bool hasError = info->fdState->hasError;
                bool hasClosed = info->fdState->hasClosed;
                Int32 errorCode = info->fdState->errorCode;
                info->IsCompleted = true;

                if(opcode != BeginConnect_op)
                {
                    mutex->Unlock();//
                    throw ArgumentException("asyncResult", "asyncResult was not returned by a call to the BeginConnect method.");
                }

                if(EndXHasCalled)
                {
                    mutex->Unlock();//
                    throw InvalidOperationException("EndConnect was previously called for the asynchronous connection.");
                }

                if(hasClosed)
                {
                    if(info->callbackCalledByRuntimeManager)
                        isClosedByRuntimeManager = true;
                    else
                        isClosedByRuntimeManager = false;
                    //Close();
                    mutex->Unlock();//
                    throw ObjectDisposedException("AsyncSocket", "The AsyncSocket has been closed");
                }
                if(hasError)
                {
                    if(info->callbackCalledByRuntimeManager)
                        isClosedByRuntimeManager = true;
                    else
                        isClosedByRuntimeManager = false;
                    //Close();
                    mutex->Unlock();//
                    throw SocketException(errorCode, (const char *)_sys_errlist[errorCode]);
                }

                //printf("isConnected: %p\n", info->ReaderWriterThreadInfo_instance_ref);
                isConnected = true;
                mutex->Unlock();//
			}
			//----------------------------------------------------
            IAsyncResult *AsyncSocket::BeginReceive(char buffer[], Int32 offset, Int32 size, SocketFlags socketFlags, AsyncCallback callback, Object *state)
            {
                if(buffer == null)
                    throw ArgumentNullException("buffer", "buffer is null");

                if(offset < 0)
					throw ArgumentOutOfRangeException("offset", "offset is less than 0");

				if(size <= 0)
					throw ArgumentOutOfRangeException("size", "size must be greater than 0");

                if(callback == null)
                    throw ArgumentNullException("callback");

                mutex->Lock();
				if(disposed)
				{
				    mutex->Unlock();//
					throw ObjectDisposedException("AsyncSocket", "The AsyncSocket has been disposed");
				}

                if(serverMode)
                {
                    if(isAccepted)
                    {
                        AcceptedConnectedInfo *info = (AcceptedConnectedInfo *)asyncResult;
                        if(info->ReaderWriterThreadInfo_instance_ref == null)
                        {
                            info->fdState->buffer = buffer;
                            info->fdState->offset = offset;
                            info->fdState->size = size;
                            info->fdState->socketFlags = socketFlags;
                            info->callback = callback;
                            info->AsyncState = state;
                            info->fdState->toRead = true;
                            info->fdState->toWrite = false;
                            info->fdState->hasClosed = false;
                            info->fdState->hasError = false;
                            info->fdState->n_read = 0;
                            info->fdState->n_written = 0;
                            info->EndXHasCalled = false;
                            info->IsCompleted = false;
                            info->opcode = BeginReceive_op;
                            AsyncSocketRuntimeManager *manager = (AsyncSocketRuntimeManager *)info->AsyncSocketRuntimeManager_instance_ref;
                            ReaderWriterThreadInfo *ReaderWriterThreadInfo_instance_ref = manager->AssignSocketInstanceToReaderWriterThreadPool(manager->get_ReaderWriterThreads(), ServerModeRuntime_ThreadPool_Num, info->fdState, GuaranteeMulticoreExecution, &manager->LastCoreAssigned_ReaderWriterThreads);
                            if(ReaderWriterThreadInfo_instance_ref == null)
                            {
                                mutex->Unlock();//
                                throw OverflowException("Overloaded Resources", "The thread pool has not enough space to process this operation. Try later again.");
                            }
                            info->ReaderWriterThreadInfo_instance_ref = (Object *)ReaderWriterThreadInfo_instance_ref;
                            //ReaderWriterThreadInfo_instance_ref->states_mutex->Unlock();
                        }
                        else
                        {
                            ReaderWriterThreadInfo *threadInstance = (ReaderWriterThreadInfo *)info->ReaderWriterThreadInfo_instance_ref;
                            //threadInstance->states_mutex->Lock();
                            {
                                info->fdState->buffer = buffer;
                                info->fdState->offset = offset;
                                info->fdState->size = size;
                                info->fdState->socketFlags = socketFlags;
                                info->callback = callback;
                                info->AsyncState = state;
                                info->fdState->toRead = true;
                                info->fdState->toWrite = false;
                                info->fdState->hasClosed = false;
                                info->fdState->hasError = false;
                                info->fdState->n_read = 0;
                                info->fdState->n_written = 0;
                                info->EndXHasCalled = false;
                                info->opcode = BeginReceive_op;

                                struct epoll_event event;
                                event.data.fd = info->fdState->sock->get_Handle();
                                event.data.ptr = info->fdState;
                                event.events = EPOLLIN | EPOLLONESHOT ;

                                if(::epoll_ctl(threadInstance->epollfd, EPOLL_CTL_MOD, info->fdState->sock->get_Handle(), &event) == -1)
                                {
                                    //threadInstance->states_mutex->Unlock();
                                    mutex->Unlock();//
                                    throw Exception(String("epoll_ctl() error: ") + (const char *)_sys_errlist[errno]);
                                    //abort();
                                }

                                if(info->IsCompleted) // change the mode with EndX
                                {
                                    //threadInstance->available++;
                                    LockFree::LockFreeCounter::AtomicIncrement(&threadInstance->available);
                                    ///if(threadInstance->available - 1 == 0)
                                     ///   threadInstance->states_cv->Signal();//
                                }
                                //else printf("!IsCompleted BeginReceive\n");
                                info->IsCompleted = false;
                            }
                            //threadInstance->states_mutex->Unlock();
                        }
                        mutex->Unlock();
                        return info;
                    }
                    if(isListening)
                    {
                        mutex->Unlock();//
                        throw InvalidOperationException("The operation was failed because the AsyncSocket instance is in listening state");
                    }
                }
                else // clientMode
                {
                    if(!isConnected)
                    {
                        mutex->Unlock();//
                        throw InvalidOperationException("The AsyncSocket instance has not been connected");
                    }

                    AcceptedConnectedInfo *info = (AcceptedConnectedInfo *)this->asyncResult;
                    //printf("BeginSend socket: %d pointer: %p last opcode: %d\n", info->fdState->sock->get_Handle(), info->ReaderWriterThreadInfo_instance_ref, info->opcode);
                    if(info->ReaderWriterThreadInfo_instance_ref == null)
                    {
                        //printf("BeginReceive info->opcode: %d\n", info->opcode);
                        info->fdState->buffer = buffer;
                        info->fdState->offset = offset;
                        info->fdState->size = size;
                        info->fdState->socketFlags = socketFlags;
                        info->callback = callback;
                        info->AsyncState = state;
                        info->fdState->toRead = true;
                        info->fdState->toWrite = false;
                        info->fdState->hasClosed = false;
                        info->fdState->hasError = false;
                        info->fdState->n_read = 0;
                        info->fdState->n_written = 0;
                        info->EndXHasCalled = false;
                        info->IsCompleted = false;
                        info->opcode = BeginReceive_op;
                        AsyncSocketRuntimeManager *manager = (AsyncSocketRuntimeManager *)info->AsyncSocketRuntimeManager_instance_ref;
                        ReaderWriterThreadInfo *ReaderWriterThreadInfo_instance_ref = manager->AssignSocketInstanceToReaderWriterThreadPool(manager->get_ReaderWriterThreads(), ClientModeRuntime_ThreadPool_Num, info->fdState, GuaranteeMulticoreExecution, &manager->LastCoreAssigned_ReaderWriterThreads);
                        if(ReaderWriterThreadInfo_instance_ref == null)
                        {
                            mutex->Unlock();
                            throw OverflowException("Overloaded Resources", "The thread pool has not enough space to process this operation. Try later again.");
                        }
                        info->ReaderWriterThreadInfo_instance_ref = (Object *)ReaderWriterThreadInfo_instance_ref;
                        //ReaderWriterThreadInfo_instance_ref->states_mutex->Unlock();
                    }
                    else
                    {
                        ReaderWriterThreadInfo *threadInstance = (ReaderWriterThreadInfo *)info->ReaderWriterThreadInfo_instance_ref;
                        //threadInstance->states_mutex->Lock(); // Remove this lock and update toRead by an atomic CAS.
                        {
                            info->fdState->buffer = buffer;
                            info->fdState->offset = offset;
                            info->fdState->size = size;
                            info->fdState->socketFlags = socketFlags;
                            info->callback = callback;
                            info->AsyncState = state;
                            info->fdState->toRead = true;
                            info->fdState->toWrite = false;
                            info->fdState->hasClosed = false;
                            info->fdState->hasError = false;
                            info->fdState->n_read = 0;
                            info->fdState->n_written = 0;
                            info->EndXHasCalled = false;
                            info->opcode = BeginReceive_op;

                            struct epoll_event event;
                            event.data.fd = info->fdState->sock->get_Handle();
                            event.data.ptr = info->fdState;
                            event.events = EPOLLIN | EPOLLONESHOT;

                            if(::epoll_ctl(threadInstance->epollfd, EPOLL_CTL_MOD, info->fdState->sock->get_Handle(), &event) == -1)
                            {
                                mutex->Unlock();
                                //threadInstance->states_mutex->Unlock();
                                throw Exception(String("epoll_ctl() error: ") + (const char *)_sys_errlist[errno]);
                                //abort();
                            }

                            if(info->IsCompleted) // change the mode with EndX
                            {
                                //threadInstance->available++;
                                LockFree::LockFreeCounter::AtomicIncrement(&threadInstance->available);
                                ///if(threadInstance->available - 1 == 0)
                                ///    threadInstance->states_cv->Signal();//
                            }
                            //else printf("!IsCompleted BeginReceive\n");
                            info->IsCompleted = false;
                        }
                        //threadInstance->states_mutex->Unlock();
                    }
                    mutex->Unlock();
                    return info;
                }
            }
            //----------------------------------------------------
            Int32 AsyncSocket::EndReceive(IAsyncResult *asyncResult)
            {
                if(asyncResult == null)
                    throw ArgumentNullException("asyncResult");


                mutex->Lock();
				if(disposed)
				{
				    mutex->Unlock();//
					throw ObjectDisposedException("AsyncSocket", "The AsyncSocket has been disposed");
				}

                AcceptedConnectedInfo *info = (AcceptedConnectedInfo *)asyncResult;
                if(info->callbackCalledByRuntimeManagerDeletionPhase)
                {
                    mutex->Unlock();//
					throw ObjectDisposedException("AsyncSocket", "The AsyncSocket has been disposed");
                }

                this->asyncResult = asyncResult;

                ReaderWriterThreadInfo *threadInstance = (ReaderWriterThreadInfo *)info->ReaderWriterThreadInfo_instance_ref;
                //threadInstance->states_mutex->Lock(); // Remove this lock and update toRead by an an atomic CAS..
                info->fdState->toRead = false; // conidser here lock (thread mutex) for connection migration.
                bool hasClosed = info->fdState->hasClosed;
                bool hasError = info->fdState->hasError;
                Int32 errorCode = info->fdState->hasError;
                Int32 n_read = info->fdState->n_read;
                AsynchMethodOpcodes opcode = info->opcode;
                bool EndXHasCalled = info->EndXHasCalled;
                info->EndXHasCalled = true;
                info->IsCompleted = true;
                //threadInstance->available--;
                if(threadInstance != null)
                    LockFree::LockFreeCounter::AtomicDecrement(&threadInstance->available);
                //threadInstance->states_mutex->Unlock();

                if(opcode != BeginReceive_op)
                {
                    mutex->Unlock();//
                    throw ArgumentException("asyncResult", "asyncResult was not returned by a call to the BeginReceive method.");
                }

                if(EndXHasCalled)
                {
                    mutex->Unlock();//
                    throw InvalidOperationException("EndReceive was previously called for the asynchronous read.");
                }

                if(hasClosed)
                {
                    isClosedByRuntimeManager = true;
                    //Close();
                    mutex->Unlock();//
                    return 0;
                    //throw ObjectDisposedException("AsyncSocket", "The AsyncSocket has been closed");
                }
                if(hasError)
                {
                    isClosedByRuntimeManager = true;
                    //Close();
                    mutex->Unlock();//
                    throw SocketException(errorCode, (const char *)_sys_errlist[errorCode]);
                }

                mutex->Unlock();//
                return n_read;
            }
            //----------------------------------------------------
            IAsyncResult *AsyncSocket::BeginSend(char buffer[], Int32 offset, Int32 size, SocketFlags socketFlags, AsyncCallback callback, Object *state)
            {
                if(buffer == null)
                    throw ArgumentNullException("buffer", "buffer is null");

                if(offset < 0)
					throw ArgumentOutOfRangeException("offset", "offset is less than 0");

				if(size <= 0)
					throw ArgumentOutOfRangeException("size", "size must be greater than 0");


                if(callback == null)
                    throw ArgumentNullException("callback");

                mutex->Lock();
                if(disposed)
                {
                    mutex->Unlock();//
					throw ObjectDisposedException("AsyncSocket", "The AsyncSocket has been disposed");
                }

                if(serverMode)
                {
                    if(isAccepted)
                    {
                        AcceptedConnectedInfo *info = (AcceptedConnectedInfo *)asyncResult;
                        if(info->ReaderWriterThreadInfo_instance_ref == null)
                        {
                            info->fdState->buffer = buffer;
                            info->fdState->offset = offset;
                            info->fdState->size = size;
                            info->fdState->socketFlags = socketFlags;
                            info->callback = callback;
                            info->AsyncState = state;
                            info->fdState->toRead = false;
                            info->fdState->toWrite = true;
                            info->fdState->hasClosed = false;
                            info->fdState->hasError = false;
                            info->fdState->n_read = 0;
                            info->fdState->n_written = 0;
                            info->EndXHasCalled = false;
                            info->IsCompleted = false;
                            info->opcode = BeginSend_op;
                            AsyncSocketRuntimeManager *manager = (AsyncSocketRuntimeManager *)info->AsyncSocketRuntimeManager_instance_ref;
                            ReaderWriterThreadInfo *ReaderWriterThreadInfo_instance_ref = manager->AssignSocketInstanceToReaderWriterThreadPool(manager->get_ReaderWriterThreads(), ServerModeRuntime_ThreadPool_Num, info->fdState, GuaranteeMulticoreExecution, &manager->LastCoreAssigned_ReaderWriterThreads);
                            if(ReaderWriterThreadInfo_instance_ref == null)
                            {
                                mutex->Unlock();//
                                throw OverflowException("Overloaded Resources", "The thread pool has not enough space to process this operation. Try later again.");
                            }
                            info->ReaderWriterThreadInfo_instance_ref = (Object *)ReaderWriterThreadInfo_instance_ref;
                            asyncResult = info;
                            //ReaderWriterThreadInfo_instance_ref->states_mutex->Unlock();
                        }
                        else
                        {
                            ReaderWriterThreadInfo *threadInstance = (ReaderWriterThreadInfo *)info->ReaderWriterThreadInfo_instance_ref;
                            //threadInstance->states_mutex->Lock(); // Remove this lock and update toWrite by an atomic CAS.
                            {
                                info->fdState->buffer = buffer;
                                info->fdState->offset = offset;
                                info->fdState->size = size;
                                info->fdState->socketFlags = socketFlags;
                                info->callback = callback;
                                info->AsyncState = state;
                                info->fdState->toRead = false;
                                info->fdState->toWrite = true;
                                info->fdState->hasClosed = false;
                                info->fdState->hasError = false;
                                info->fdState->n_read = 0;
                                info->fdState->n_written = 0;
                                info->EndXHasCalled = false;
                                info->opcode = BeginSend_op;
                                asyncResult = info;

                                struct epoll_event event;
                                event.data.fd = info->fdState->sock->get_Handle();
                                event.data.ptr = info->fdState;
                                event.events = EPOLLOUT | EPOLLONESHOT;

                                if(::epoll_ctl(threadInstance->epollfd, EPOLL_CTL_MOD, info->fdState->sock->get_Handle(), &event) == -1)
                                {
                                    mutex->Unlock();//
                                    //threadInstance->states_mutex->Unlock();
                                    throw Exception(String("epoll_ctl() error: ") + (const char *)_sys_errlist[errno]);
                                    //abort();
                                }

                                if(info->IsCompleted) // change the mode with EndX
                                {
                                    //threadInstance->available++;
                                    LockFree::LockFreeCounter::AtomicIncrement(&threadInstance->available);
                                    ///if(threadInstance->available - 1 == 0)
                                    ///    threadInstance->states_cv->Signal();//
                                }
                                //else printf("!IsCompleted BeginSend\n");
                                info->IsCompleted = false;
                            }
                            //threadInstance->states_mutex->Unlock();
                        }
                        mutex->Unlock();
                        return info;
                    }
                    if(isListening)
                    {
                        mutex->Unlock();//
                        throw InvalidOperationException("The operation was failed because the AsyncSocket instance is in the listening state.");
                    }
                }
                else // clientMode
                {
                    if(!isConnected)
                    {
                        mutex->Unlock();//
                        throw InvalidOperationException("The AsyncSocket instance has not been connected");
                    }

                    AcceptedConnectedInfo *info = (AcceptedConnectedInfo *)this->asyncResult;
                    //printf("BeginSend socket: %d pointer: %p last opcode: %d\n", info->fdState->sock->get_Handle(), info->ReaderWriterThreadInfo_instance_ref, info->opcode);
                    if(info->ReaderWriterThreadInfo_instance_ref == null)
                    {
                        //printf("BeginSend info->opcode: %d\n", info->opcode);
                        info->fdState->buffer = buffer;
                        info->fdState->offset = offset;
                        info->fdState->size = size;
                        info->fdState->socketFlags = socketFlags;
                        info->callback = callback;
                        info->AsyncState = state;
                        info->fdState->toRead = false;
                        info->fdState->toWrite = true;
                        info->fdState->hasClosed = false;
                        info->fdState->hasError = false;
                        info->fdState->n_read = 0;
                        info->fdState->n_written = 0;
                        info->EndXHasCalled = false;
                        info->IsCompleted = false;
                        info->opcode = BeginSend_op;
                        AsyncSocketRuntimeManager *manager = (AsyncSocketRuntimeManager *)info->AsyncSocketRuntimeManager_instance_ref;
                        ReaderWriterThreadInfo *ReaderWriterThreadInfo_instance_ref = manager->AssignSocketInstanceToReaderWriterThreadPool(manager->get_ReaderWriterThreads(), ClientModeRuntime_ThreadPool_Num, info->fdState, GuaranteeMulticoreExecution, &manager->LastCoreAssigned_ReaderWriterThreads);
                        if(ReaderWriterThreadInfo_instance_ref == null)
                        {
                            mutex->Unlock();//
                            throw OverflowException("Overloaded Resources", "The thread pool has not enough space to process this operation. Try later again.");
                        }
                        info->ReaderWriterThreadInfo_instance_ref = (Object *)ReaderWriterThreadInfo_instance_ref;
                        //ReaderWriterThreadInfo_instance_ref->states_mutex->Unlock();
                        //printf("create BeginSend socket: %d pointer: %p\n", info->fdState->sock->get_Handle(), info->ReaderWriterThreadInfo_instance_ref);
                        this->asyncResult = info;
                        //printf("\n");
                        //printf("hello world1 ReaderWriterThreadInfo_instance_ref: %p\n", ReaderWriterThreadInfo_instance_ref);//
                    }
                    else
                    {
                        ReaderWriterThreadInfo *threadInstance = (ReaderWriterThreadInfo *)info->ReaderWriterThreadInfo_instance_ref;
                        //threadInstance->states_mutex->Lock(); // Remove this lock and update toWrite by an atomic CAS.
                        {
                            //printf("hello world2 ReaderWriterThreadInfo_instance_ref: %p\n", threadInstance);//
                            info->fdState->buffer = buffer;
                            info->fdState->offset = offset;
                            info->fdState->size = size;
                            info->fdState->socketFlags = socketFlags;
                            info->callback = callback;
                            info->AsyncState = state;
                            info->fdState->toRead = false;
                            info->fdState->toWrite = true;
                            info->fdState->hasClosed = false;
                            info->fdState->hasError = false;
                            info->fdState->n_read = 0;
                            info->fdState->n_written = 0;
                            info->EndXHasCalled = false;
                            info->opcode = BeginSend_op;
                            asyncResult = info;

                            struct epoll_event event;
                            event.data.fd = info->fdState->sock->get_Handle();
                            event.data.ptr = info->fdState;
                            event.events = EPOLLOUT | EPOLLONESHOT;

                            if(::epoll_ctl(threadInstance->epollfd, EPOLL_CTL_MOD, info->fdState->sock->get_Handle(), &event) == -1)
                            {
                                //threadInstance->states_mutex->Unlock();
                                mutex->Unlock();//
                                printf("epoll_ctl() error: %s\n", (const char *)_sys_errlist[errno]);
                                throw Exception(String("epoll_ctl() error: ") + (const char *)_sys_errlist[errno]);
                                //abort();
                            }

                            if(info->IsCompleted) // change the mode with EndX
                            {
                                //threadInstance->available++;
                                LockFree::LockFreeCounter::AtomicIncrement(&threadInstance->available);
                                ///if(threadInstance->available - 1 == 0)
                                 ///   threadInstance->states_cv->Signal();//
                            }
                            //else printf("!IsCompleted BeginSend\n");
                            info->IsCompleted = false;
                        }
                        //threadInstance->states_mutex->Unlock();
                    }
                    mutex->Unlock();
                    return info;
                }
            }
            //----------------------------------------------------
            Int32 AsyncSocket::EndSend(IAsyncResult *asyncResult)
            {

                if(asyncResult == null)
                    throw ArgumentNullException("asyncResult");

                mutex->Lock();
                if(disposed)
                {
                    mutex->Unlock();//
					throw ObjectDisposedException("AsyncSocket", "The AsyncSocket has been disposed");
                }

                AcceptedConnectedInfo *info = (AcceptedConnectedInfo *)asyncResult;
                if(info->callbackCalledByRuntimeManagerDeletionPhase)
                {
                    mutex->Unlock();//
					throw ObjectDisposedException("AsyncSocket", "The AsyncSocket has been disposed");
                }

                this->asyncResult = asyncResult;

                ReaderWriterThreadInfo *threadInstance = (ReaderWriterThreadInfo *)info->ReaderWriterThreadInfo_instance_ref;
                //threadInstance->states_mutex->Lock(); // Remove this lock and update toWrite by an atomic CAS.
                info->fdState->toWrite = false; // conidser here lock (thread mutex) for connection migration.
                bool hasClosed = info->fdState->hasClosed;
                bool hasError = info->fdState->hasError;
                Int32 errorCode = info->fdState->hasError;
                Int32 n_written = info->fdState->n_written;
                AsynchMethodOpcodes opcode = info->opcode;
                bool EndXHasCalled = info->EndXHasCalled;
                info->EndXHasCalled = true;
                info->IsCompleted = true;
                //threadInstance->available--;
                if(threadInstance != null)
                    LockFree::LockFreeCounter::AtomicDecrement(&threadInstance->available);
                //threadInstance->states_mutex->Unlock();

                if(opcode != BeginSend_op)
                {
                    mutex->Unlock();//
                    throw ArgumentException("asyncResult", "asyncResult was not returned by a call to the BeginSend method.");
                }

                if(EndXHasCalled)
                {
                    mutex->Unlock();//
                    throw InvalidOperationException("EndSend was previously called for the asynchronous send.");
                }

                if(hasClosed)
                {
                    isClosedByRuntimeManager = true;
                    //Close();
                    mutex->Unlock();//
                    throw ObjectDisposedException("AsyncSocket", "The AsyncSocket has been closed");
                }
                if(hasError)
                {
                    isClosedByRuntimeManager = true;
                    //Close();
                    mutex->Unlock();//
                    throw SocketException(errorCode, (const char *)_sys_errlist[errorCode]);
                }

                mutex->Unlock();//
                return n_written;
            }
			//----------------------------------------------------
            void AsyncSocket::Bind(IPEndPoint &localEP)
            {
                if(disposed)
					throw ObjectDisposedException("AsyncSocket", "The AsyncSocket has been disposed");

                internalSocket->Bind(localEP);
            }
            //----------------------------------------------------
            void AsyncSocket::Listen(Int32 backlog)
            {
                if(disposed)
					throw ObjectDisposedException("AsyncSocket", "The AsyncSocket has been disposed");

                internalSocket->Listen(backlog);
                isListening = true;
            }
            //----------------------------------------------------
            Long AsyncSocket::get_CoreID() const
            {
                if(disposed)
					throw ObjectDisposedException("AsyncSocket", "The AsyncSocket has been disposed");

                if(asyncResult == null)
                    return -1;

                return ((AcceptedConnectedInfo *)asyncResult)->fdState->CoreID;
            }
            //----------------------------------------------------
            Int32 AsyncSocket::get_CoreCount() const
            {
                if(disposed)
					throw ObjectDisposedException("AsyncSocket", "The AsyncSocket has been disposed");

                if(serverMode)
                    return ServerModeRuntime_ThreadPool_Num;
                else
                    return ClientModeRuntime_ThreadPool_Num;
            }
            //----------------------------------------------------
            bool AsyncSocket::TryMigrateToCore(Int32 DestCoreID)
            {
                if(disposed)
					throw ObjectDisposedException("AsyncSocket", "The AsyncSocket has been disposed");

                if(DestCoreID < 0)
                    throw ArgumentOutOfRangeException("DestCoreID", "DestCoreID is less than zero");

                if(isListening)
                    throw InvalidOperationException("The operation was failed because the AsyncSocket instance is in the listening state");

                if(!isAccepted && !isConnected)
                    throw InvalidOperationException("The operation was failed because the AsyncSocket instance must be an accepted or connected socket.");

                if(DestCoreID >= get_CoreCount() - 1)
                    throw ArgumentOutOfRangeException("DestCoreID", "DestCoreID is not in the range of available CPU cores");

                AcceptedConnectedInfo *info = (AcceptedConnectedInfo *)asyncResult;
                if(info->fdState->CoreID == DestCoreID)
                    throw ArgumentException("DestCoreID", "DestCoreID is equal to the current core's ID. Try another core ID");

                AsyncSocketRuntimeManager *manager = (AsyncSocketRuntimeManager *)info->AsyncSocketRuntimeManager_instance_ref;
                if(manager == null)
                    throw ObjectDisposedException("manager","The AsyncSocket runtime manager has been disposed");

                ReaderWriterThreadInfo *targetThreadInstance = (ReaderWriterThreadInfo *)info->ReaderWriterThreadInfo_instance_ref;
                if(targetThreadInstance == null)
                    throw InvalidOperationException("Invalid operation. To be able to invoke this method, you must had been called one of the BeginSend() or BeginReceive() methods, at least one time, on this AsyncSocket instance");

                ReaderWriterThreadInfo *destThreadInstance = manager->get_ReaderWriterThreads()[DestCoreID];
                if(destThreadInstance == null)
                    throw ObjectDisposedException("destThreadInstance","The destination thread has been disposed");


                // Does migration.
                destThreadInstance->states_mutex->Lock();
                if(destThreadInstance->current_states == __epoll_event_num__)
                {
                    destThreadInstance->states_mutex->Unlock();
                    return false;
                }

                struct epoll_event event;
                event.data.fd = info->fdState->sock->get_Handle();
                event.data.ptr = info->fdState;

                if(info->fdState->toRead)
                    event.events = EPOLLIN | EPOLLONESHOT;
                else if(info->fdState->toWrite)
                    event.events = EPOLLOUT | EPOLLONESHOT;
                else
                    event.events = EPOLLIN | EPOLLOUT | EPOLLONESHOT;

                if(::epoll_ctl(destThreadInstance->epollfd, EPOLL_CTL_ADD, info->fdState->sock->get_Handle(), &event) == -1)
                {
                    if(errno == EEXIST)
                    {
                        destThreadInstance->states_mutex->Unlock();
                        targetThreadInstance->states_mutex->Unlock();
                        return true;
                    }
                    else
                    {
                        destThreadInstance->states_mutex->Unlock();
                        targetThreadInstance->states_mutex->Unlock();
                        throw Exception(String("epoll_ctl() error: ") + (const char *)_sys_errlist[errno]);
                    }
                    //abort();
                }

                destThreadInstance->current_states++;
                /*if((info->fdState->toRead || info->fdState->toWrite) && destThreadInstance->available == 0 )
                {
                    destThreadInstance->available++;
                    destThreadInstance->states_cv->Signal();
                }*/

                if(info->fdState->toRead || info->fdState->toWrite)
                    destThreadInstance->available++; //
                ///if((info->fdState->toRead || info->fdState->toWrite) && (destThreadInstance->available - 1 == 0 || destThreadInstance->current_states - 1 == 0))
                 ///   destThreadInstance->states_cv->Signal();

                info->fdState->CoreID = DestCoreID;
                info->ReaderWriterThreadInfo_instance_ref = (Object *)destThreadInstance;

                // Removes the AsyncSocket instance  from target (current) thread queue.
                event.data.fd = -1;
                event.data.ptr = null;
                if(::epoll_ctl(targetThreadInstance->epollfd, EPOLL_CTL_DEL, info->fdState->sock->get_Handle(), &event) == -1)
                {
                    if(errno != ENOENT)
                    {
                        destThreadInstance->states_mutex->Unlock();
                        targetThreadInstance->states_mutex->Unlock();
                        throw Exception(String("epoll_ctl() error: ") + (const char *)_sys_errlist[errno]);
                    }
                    //abort();
                }

                targetThreadInstance->current_states--;
                if(info->fdState->toRead || info->fdState->toWrite)
                    targetThreadInstance->available--;

                destThreadInstance->states_mutex->Unlock();
                targetThreadInstance->states_mutex->Unlock();

                manager->SignalReaderWriterThreadToUnblockEpollWait(targetThreadInstance, true);
                manager->SignalReaderWriterThreadToUnblockEpollWait(destThreadInstance, true);

                return true;
            }
            //----------------------------------------------------
            void AsyncSocket::InitClientModeRuntime(void)
            {
                const AsyncSocketRuntimeManager *manager = ClientModeRuntime_init();
                return;
            }
            //----------------------------------------------------
            void AsyncSocket::DestroyClientModeRuntime(void)
            {
                ClientModeRuntime_destroy();
                return;
            }
            //----------------------------------------------------
        };
    };
};
//**************************************************************************************************************//
