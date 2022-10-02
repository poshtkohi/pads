#include "stdafx.h"

#include "HcpManager.h"

//**************************************************************************************************************//

ArrayList *Parvicursor::HybirdConcurrencyPattern::HcpManager::contexts_lifo_queue = new ArrayList();
pthread_mutex_t Parvicursor::HybirdConcurrencyPattern::HcpManager::_contexts_lifo_mutex_lock_ = PTHREAD_MUTEX_INITIALIZER;


namespace Parvicursor
{
   namespace HybirdConcurrencyPattern
   {
	   //----------------------------------------------------
		HcpManager::HcpManager(UInt16 listenPort)
		{
				Socket *acceptSocket = null;

#if defined WIN32 || WIN64

				WORD wVersionRequested;
				WSADATA wsaData;
				Int32 errorIni;
				wVersionRequested = MAKEWORD(2, 2);
				errorIni = WSAStartup(wVersionRequested, &wsaData);
				if(errorIni != 0)
				{
					Int32 err = WSAGetLastError();
					throw SocketException(err, ErrorString(err));
				}

				SOCKET ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

				if (INVALID_SOCKET == ListenSocket) 
				{
					Int32 err = WSAGetLastError();
					throw SocketException(err, ErrorString(err));
				}
				this->AcceptSocket = new Socket(ListenSocket, InterNetwork, Stream, tcp);
#else
				this->AcceptSocket = new Socket(InterNetwork, Stream, tcp);
#endif

				this->listenPort = listenPort;
				//IPEndPoint hostEndPoint = new IPEndPoint(IPAddress.Any, 2799);
				IPEndPoint hostEndPoint(IPAddress::get_Any(), listenPort);
				//sock.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, 1);
				this->AcceptSocket->Bind(hostEndPoint);
				//catch(Exception &e){cout << e.get_Message().get_BaseStream() << endl; return -1;}
				this->AcceptSocket->Listen(1000); 



#if defined WIN32 || WIN64

				g_hShutdownEvent = NULL;

				InitializeIOCP();

				
				g_hAcceptEvent = WSACreateEvent();

				if (WSA_INVALID_EVENT == g_hAcceptEvent)
				{
					Int32 err = WSAGetLastError();
					throw SocketException(err, ErrorString(err));
				}

				if (SOCKET_ERROR == WSAEventSelect(this->AcceptSocket->get_Handle(), g_hAcceptEvent, FD_ACCEPT))
				{
					WSACloseEvent(g_hAcceptEvent);
					Int32 err = WSAGetLastError();
					throw SocketException(err, ErrorString(err));
					//goto error;
				}

				g_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
#else
#endif




				this->worker_master = null;
				this->worker_NetworkIoDispatcher = null;


				/*this->contexts_lifo_queue = null;
				HcpManager::InitilizeLifoQueue(&this->contexts_lifo_queue, InitialQueueSize);
				this->contexts_lifo_queue_index = 0;*/
				/*this->contexts_lifo_queue = new ArrayList();

				this->_contexts_lifo_mutex_lock_ = PTHREAD_MUTEX_INITIALIZER;*/

				this->worker_master = new Thread(this->Wrapper_To_Call_WorkerProc_master, (void *)this);
				this->worker_master->Start();
				this->worker_master->Join();
				//this->worker_NetworkIoDispatcher->Join();
		}
		//----------------------------------------------------
		HcpManager::~HcpManager()
		{

			if(this->worker_NetworkIoDispatcher != null)
			{
				pthread_cancel(this->worker_NetworkIoDispatcher->thread);
				delete this->worker_NetworkIoDispatcher;
				this->worker_NetworkIoDispatcher = null;
				//pthread_exit(null);
			}
		}
		//----------------------------------------------------
		void HcpManager::WorkerProc_NetworkIoDispatcher()
		{
			pthread_setcanceltype(PTHREAD_CANCEL_ENABLE, null);
			pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, null);

			/*while(true)
			{
				printf("hello world1\n");
				Thread::Sleep(100000000);
			}*/

			void *nativeContext = null;
			ClientContext *context = null;
			Int32 bytesTransfered;
			Int32 nBytesRecv = 0;
			Int32 nBytesSent = 0;

#if defined WIN32 || WIN64
			DWORD dwBytes = 0, dwFlags = 0;
			OVERLAPPED  *pOverlapped = NULL;
			Int32 error = 0;
#else
#endif


			//while(true){Thread::Sleep(1);}
			while(true)
			{
//printf("yyy\n");
#if defined WIN32 || WIN64
				//BOOL bReturn = GetQueuedCompletionStatus(g_hIOCompletionPort, &bytesTransfered, (LPDWORD)&nativeContext, &pOverlapped, INFINITE);
				BOOL bReturn = GetQueuedCompletionStatus(this->g_hIOCompletionPort, (LPDWORD)&bytesTransfered, (LPDWORD)&nativeContext, &pOverlapped, INFINITE);

printf("yyy\n");
				if (NULL == nativeContext)
				{
					//We are shutting down
					break;
				}

				//Get the client context
				context = (ClientContext *)nativeContext;

				if ((FALSE == bReturn) || ((TRUE == bReturn) && (0 == bytesTransfered)))
				{
					//Client connection gone, remove it.
					CloseConnection(context);
					continue;
				}

				WSABUF *wsabuf = context->get_WSABUF();
				OVERLAPPED *overlapped = context->get_Overlapped();

				switch (context->get_State())
				{
					/*case _Accepted:
					{
						try { context->get_Socket()->Receive(&c, 0, 1, None);
						cout << c << endl;}
						catch(Exception &e){cout << e.get_Message().get_BaseStream() << endl; }

						context->OnAccept();
						break;
					}*/

					case _OnReceive:
					{
						dwFlags = 0;

						//Get the data.
						nBytesRecv = WSARecv(context->get_Socket()->get_Handle(), wsabuf, 1, 
											 &dwBytes, &dwFlags, overlapped, NULL);

						error = WSAGetLastError();
						if (nBytesRecv == SOCKET_ERROR && error != WSA_IO_PENDING)
						{
							CloseConnection(context);
							break;
						}


						context->IncreaseTransferedBytes(bytesTransfered);
						if(context->get_TransferedBytes() < context->get_TotalBytes())
						{

							context->set_WsabufIndexChanged(true);
							wsabuf->buf += context->get_TransferedBytes();
							wsabuf->len = context->get_TotalBytes() - context->get_TransferedBytes();

							nBytesRecv = WSARecv(context->get_Socket()->get_Handle(), wsabuf, 1, 
											 &dwBytes, &dwFlags, overlapped, NULL);

							error = WSAGetLastError();
							if (nBytesRecv == SOCKET_ERROR && error != WSA_IO_PENDING)
							{
								CloseConnection(context);
								break;
							}
							break;
						}
						else
						{
							if(context->get_WsabufIndexChanged())
							{
								wsabuf->buf -= context->get_TransferedBytes();
								context->set_WsabufIndexChanged(false);
							}
							if(context->OnReceive() < 0)
							{
							}
						}

						break;
					}

					case _OnSend:
					{
						dwFlags = 0;
						context->IncreaseTransferedBytes(bytesTransfered);
						if(context->get_TransferedBytes() < context->get_TotalBytes())
						{

							context->set_WsabufIndexChanged(true);
							wsabuf->buf += context->get_TransferedBytes();
							wsabuf->len = context->get_TotalBytes() - context->get_TransferedBytes();

							nBytesRecv = WSASend(context->get_Socket()->get_Handle(), wsabuf, 1, 
											 &dwBytes, dwFlags, overlapped, NULL);

							error = WSAGetLastError();
							if (nBytesRecv == SOCKET_ERROR && error != WSA_IO_PENDING)
							{
								CloseConnection(context);
								break;
							}
							break;
						}
						else
						{
							if(context->get_WsabufIndexChanged())
							{
								wsabuf->buf -= context->get_TransferedBytes();
								context->set_WsabufIndexChanged(false);
							}
							context->OnSend();
						}

						break;
					}

					default:
					{
						//We should never be reaching here, under normal circumstances.
						break;
					}
				}
#else
#endif

			}
		}
		//----------------------------------------------------
		void HcpManager::WorkerProc_master()
		{
			this->worker_NetworkIoDispatcher = new Thread(this->Wrapper_To_Call_WorkerProc_NetworkIoDispatcher, (void *)this);
			this->worker_NetworkIoDispatcher->Start();
			this->worker_NetworkIoDispatcher->SetDetached();

#if defined WIN32 || WIN64

			WSANETWORKEVENTS WSAEvents;

			while(WAIT_OBJECT_0 != WaitForSingleObject(g_hShutdownEvent, 0))
			{
				if (WSA_WAIT_TIMEOUT != WSAWaitForMultipleEvents(1, &g_hAcceptEvent, FALSE, WAIT_TIMEOUT_INTERVAL, FALSE))
				{
					WSAEnumNetworkEvents(this->AcceptSocket->get_Handle(), g_hAcceptEvent, &WSAEvents);
					if ((WSAEvents.lNetworkEvents & FD_ACCEPT) && (0 == WSAEvents.iErrorCode[FD_ACCEPT_BIT]))
					{
						Socket *s = this->AcceptSocket->Accept(); printf("hellow: %d\n", s->get_Handle());
						if(s!= null)
						{
							ClientContext *context = new ClientContext(s);

							if(AssociateWithIOCP(context))
							{
								pthread_mutex_lock(&_contexts_lifo_mutex_lock_);
								this->contexts_lifo_queue ->Add(context);
								pthread_mutex_unlock(&_contexts_lifo_mutex_lock_);

								//context->get_Socket()->Receive(context->buffer_small, 0, 1, None);
								//Once the data is successfully received, we will print it.
								//context->SetOpCode(OP_WRITE);

								WSABUF *wsbuf = context->get_WSABUF();
								//wsbuf.buf = context->get_SmallBuffer();
								//wsbuf.len = context->get_SmallBufferSize();
								OVERLAPPED *overlapped = context->get_Overlapped();

								//Get data.
								DWORD dwFlags = 0;
								DWORD dwBytes = 0;

								//Post initial Recv
								//This is a right place to post a initial Recv
								//Posting a initial Recv in WorkerThread will create scalability issues.
								int nBytesRecv = WSARecv(context->get_Socket()->get_Handle(), wsbuf, 1, 
									&dwBytes, &dwFlags, overlapped, NULL);

								if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
								{
									printf("\nError in Initial Post. %s.\n", ErrorString(WSAGetLastError()));
									CloseConnection(context);
									continue;
								}

								/*BOOL bSuccess = PostQueuedCompletionStatus(this->g_hIOCompletionPort, 1, (DWORD)context, context->get_Overlapped());
								//HANDLE hTemp = CreateIoCompletionPort((HANDLE)context->get_Socket()->get_Handle(), this->g_hIOCompletionPort, (DWORD)context, 0);
								
								if ( (!bSuccess && GetLastError( ) != ERROR_IO_PENDING))
								{            
									pthread_mutex_lock(&_contexts_lifo_mutex_lock_);

									contexts_lifo_queue->Remove(context);

									pthread_mutex_unlock(&_contexts_lifo_mutex_lock_);printf("error\n");

									delete context;
								}*/

							}
						}
						//AcceptConnection(ListenSocket);
					}
				}
			}
#else
#endif

		}
		//----------------------------------------------------
		void *HcpManager::Wrapper_To_Call_WorkerProc_NetworkIoDispatcher(void* pt2Object)
		{
			HcpManager *mySelf = (HcpManager *) pt2Object;
			// call member
			mySelf->WorkerProc_NetworkIoDispatcher();
			return pt2Object;
		}
		//----------------------------------------------------
		void *HcpManager::Wrapper_To_Call_WorkerProc_master(void* pt2Object)
		{
			HcpManager *mySelf = (HcpManager *) pt2Object;
			// call member
			mySelf->WorkerProc_master();
			return pt2Object;
		}
		//----------------------------------------------------
		void HcpManager::InitilizeLifoQueue(Out ArrayList **al, UInt32 initialCapacity)
		{
			if(initialCapacity <= 0)
				throw ArgumentOutOfRangeException("initialCapacity", "initialCapacity must greater than zero");

			if((*al) == null)
				(*al) = new ArrayList();

			(*al)->Clear();

			for(UInt32 i = 0 ; i < initialCapacity ; i++)
				(*al)->Add(null);

			return ;
		}
		//----------------------------------------------------
		void HcpManager::CloseConnection(In ClientContext *context)
		{
			//printf("\n%d %s.\n", context->get_Socket()->get_Handle(), ErrorString(WSAGetLastError()));
			pthread_mutex_lock(&this->_contexts_lifo_mutex_lock_);

			this->contexts_lifo_queue->Remove(context); //close clinet connection

			pthread_mutex_unlock(&this->_contexts_lifo_mutex_lock_);

			delete context;
			cout << "CloseConnection" << endl;

			return ;
		}
		//----------------------------------------------------
#if defined WIN32 || WIN64
		//----------------------------------------------------
		void HcpManager::InitializeIOCP()
		{
			//Create I/O completion port
			this->g_hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

			if(this->g_hIOCompletionPort == null)
			{
				Int32 err = WSAGetLastError();
				throw SocketException(err, ErrorString(err));
			}
		}
		//----------------------------------------------------
		bool HcpManager::AssociateWithIOCP(In ClientContext *context)
		{
			//Associate the socket with IOCP
			HANDLE hTemp = CreateIoCompletionPort((HANDLE)context->get_Socket()->get_Handle(), this->g_hIOCompletionPort, (DWORD)context, 0);

			if (NULL == hTemp)
			{
				printf("\nError occurred while executing CreateIoCompletionPort().");
				delete context;
				return false;
			}
			return true;
		}
		//----------------------------------------------------
#else
		//----------------------------------------------------
		//----------------------------------------------------
#endif
		//----------------------------------------------------
   };
};
//**************************************************************************************************************//