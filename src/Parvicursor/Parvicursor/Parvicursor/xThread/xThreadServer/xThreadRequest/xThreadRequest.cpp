#include "xThreadRequest.h"

#if defined WIN32 || WIN64
		typedef void *(__cdecl *xThreadBase_creator)();
#else
		typedef void *(*xThreadBase_creator)();
#endif
//**************************************************************************************************************//
namespace Parvicursor
{
   namespace xThread
   {
		//----------------------------------------------------
		xThreadRequest::xThreadRequest(ParvicursorSocket *socket, const String &sessionGuid)
		{
			this->timeout = 120*1000; // 120s timeout
			this->MaxThreadSupport = 1000;
			this->secure = false;

			sockets = new ArrayList();
			sockets->Add(socket);
			mutex = new Mutex();

			this->buffer = null;
			this->currentBufferSize = -1;
			this->collection = null;
			this->serializer = null;
			this->deserializer = null;

			this->threadChannel = socket;
			this->sessionGuid = sessionGuid;
		}
		//----------------------------------------------------
		void xThreadRequest::AddNewClientStream(ParvicursorSocket *socket)
		{
			//printf("xThreadRequest::AddNewClientStream\n");
			mutex->Lock();
			if(this->sockets != null)
			{
				try 
				{ 
					socket->WriteNoException(); 
					if(socket->get_IsSecure())
						socket->Flush();
				}
                catch(...) {  mutex->Unlock(); return;  }
				this->sockets->Add(socket);
				this->threadControlChannel = socket;
			}
			mutex->Unlock();

			return ;
		}
		//----------------------------------------------------
		int xThreadRequest::WaitForAllConnections()
		{
			//clock_t start, finish;
			int _timeout = 0;
			//start = clock();
			//double  duration;
			while(true)
			{
				mutex->Lock();
				int count = sockets->get_Count();
				mutex->Unlock();

				if(count == 2)
					return 0;

				if(count > 2)
					return -1;

				if(_timeout >= this->timeout)
					return -1;
				/*finish = clock();
				duration = (double)(finish - start) / CLOCKS_PER_SEC;
				if(duration >= timeout)
					return -1;*/
				Thread::Sleep(1);
				_timeout++;
			}
		}
		//----------------------------------------------------
		void xThreadRequest::Run()
		{
			mutex->Lock();
			try 
			{ 
				ParvicursorSocket * s = (ParvicursorSocket *)sockets->get_Value(0);
				s->WriteNoException();
				if(s->get_IsSecure())
					s->Flush();
			}
            catch(...) {mutex->Unlock(); WorkerExit(); return ;}
			mutex->Unlock();
			printf("---------------------------------------------------------\n");
			printf("New xThread session.\n");
			if(WaitForAllConnections() == -1) { WorkerExit(); return ; }

			cout << "hello xThread world!" << endl;

			try
			{
				Int32 objSize = -1;
				this->buffer = this->threadChannel->ReadObject(objSize); //read the client serizlized thread collection
				this->deserializer = new DeSerializer(this->buffer, objSize);

				this->collection = new xThreadCollection();
				this->collection->DeSerialize(*this->deserializer, this->MaxThreadSupport, this->sessionGuid);

				if(this->buffer != null)
				{
					delete this->buffer;
					this->buffer = null;
				}

				printf("here1\n");
				//cout << objSize << endl;
			}
			catch(ObjectDisposedException &) { RemoveSocketFromArrayList(this->threadChannel); WorkerExit(); return; }
			catch(SocketException &) { RemoveSocketFromArrayList(this->threadChannel); WorkerExit(); return; }
			catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, this->threadChannel) ; WorkerExit(); return; }
			catch(...) { System::Exception e("Unknown excpetion"); SendExceptionToOneSocketAndClose(e, this->threadChannel) ; WorkerExit(); return; }

			//cout << this->collection->get_Count() << endl;return;
			for(Int32 i = 0 ; i < this->collection->get_Count() ; i++)
			{

				xThreadCollection::ThreadInfo *info = this->collection->get_Value(i);

#if defined WIN32 || WIN64

				info->lib_handle = LoadLibrary(TEXT(info->DllFilename.get_BaseStream()));

				if(info->lib_handle == null)
				{
					try { System::Exception e(ErrorString(GetLastError())); SendExceptionToOneSocketAndClose(e, this->threadChannel) ; WorkerExit(); return; }
					catch(...) { WorkerExit(); return; }
				}

				xThreadBase_creator ProcAdd = (xThreadBase_creator)GetProcAddress(info->lib_handle, "xThreadBase_creator");

				if (ProcAdd == null)
				{
					try { System::Exception e(ErrorString(GetLastError())); SendExceptionToOneSocketAndClose(e, this->threadChannel) ; WorkerExit(); return; }
					catch(...) { WorkerExit(); return; }
				}

#else

				info->lib_handle = dlopen(info->DllFilename.get_BaseStream(), RTLD_LAZY);

				if(info->lib_handle == null)
				{
					try
					{
						char *ret = dlerror();
						if(ret != null)
						{
							char *error = new char[strlen(ret) + 1];
							strcpy(error, ret);
							System::Exception e(error);
							SendExceptionToOneSocketAndClose(e, this->threadChannel);
						}
						else
						{
							System::Exception e("Unknown error was occured.");
							SendExceptionToOneSocketAndClose(e, this->threadChannel);
						}
						WorkerExit(); return;
					}
					catch(...) { WorkerExit(); return; }
				}

				xThreadBase_creator ProcAdd = (xThreadBase_creator)dlsym(info->lib_handle, "xThreadBase_creator");

				if (ProcAdd == null)
				{
					try
					{
						char *ret = dlerror();
						if(ret != null)
						{
							char *error = new char[strlen(ret) + 1];
							strcpy(error, ret);
							System::Exception e(error);
							SendExceptionToOneSocketAndClose(e, this->threadChannel);
						}
						else
						{
							System::Exception e("Unknown error was occured.");
							SendExceptionToOneSocketAndClose(e, this->threadChannel);
						}
						WorkerExit(); return;
					}
					catch(...) { WorkerExit(); return; }
				}


#endif

				//check here for likelihood user errors
				info->objInstance = (xThreadBase *)(ProcAdd)();
				info->objInstance->DeSerialize(&info->serializedBuffer, &info->serializedBuffer_size);

				info->worker = new Thread(info->Wrapper_To_Call_StartProc, info);
				info->worker->Start();
                info->worker->SetDetached();
				//_worker.Suspend();
                //_worker.Resume();

				//info->objInstance->Serialize(&info->serializedDLLBuffer, &info->serializedBuffer_size);
				//info->obj->FreeDllMemory(info->obj);
				//info->obj->FreeDllMemory(info->serializedDLLBuffer);

			}

			try
			{
				this->threadChannel->WriteNoException();
				if(this->threadChannel->get_IsSecure())
					this->threadChannel->Flush();
			}
			catch(ObjectDisposedException &) { RemoveSocketFromArrayList(this->threadChannel); WorkerExit(); return; }
			catch(SocketException &) { RemoveSocketFromArrayList(this->threadChannel); WorkerExit(); return; }
			catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, this->threadChannel) ; WorkerExit(); return; }
			catch(...) { System::Exception e("Unknown excpetion"); SendExceptionToOneSocketAndClose(e, this->threadChannel) ; WorkerExit(); return; }

			/*xThreadCollection::ThreadInfo *_info = this->collection->get_Value(0);
			info->worker->Suspend();
			while(true)
			{
				if(info->worker->get_ThreadState() == System::Threading::Running)
					break;
				cout << "dfdfdsafasd" << endl;
				Thread::Sleep(1);
			}*/
			/*Thread::Sleep(2000);
			for(Int32 i = 0 ; i < this->collection->get_Count() ; i++)
			{
				xThreadCollection::ThreadInfo *info = this->collection->get_Value(i);
				info->worker->Suspend();
			}
			Thread::Sleep(2000);

			for(Int32 i = 0 ; i < this->collection->get_Count() ; i++)
			{
				xThreadCollection::ThreadInfo *info = this->collection->get_Value(i);
				info->worker->Resume();
			}

			Thread::Sleep(10000);*/


			int readsocks;
			byte command = 0;
			struct timeval timeoutRead;  // Timeout for select
			timeoutRead.tv_sec = 0;
			timeoutRead.tv_usec = 0; // 1000=1ms

			struct timeval timeoutWrite;  // Timeout for select
			timeoutWrite.tv_sec = 0;
			timeoutWrite.tv_usec = 0; //
			Int32 completedThreadNum = 0;
			bool _AllThreadsCompleted = false;
			bool _AllThreadsCompleted_notification_sent = false;


			this->buffer = new char[Parvicursor::xThread::xThreadRequest::DefaultBufferSize];
			this->currentBufferSize = Parvicursor::xThread::xThreadRequest::DefaultBufferSize;
			this->serializer = new Serializer(buffer, this->currentBufferSize);

			while(true)
			{
				//Thread::Sleep(1);
				//here:
				if(sockets->get_Count() == 0 //|| written >= info.FileSize
				)
					break;

				Thread::Sleep(1);

				//---------------------Thread Channel---------------------------------------
				if(!_AllThreadsCompleted_notification_sent)
				{
					if(!_AllThreadsCompleted)
					{

						if(completedThreadNum != this->collection->get_Count())
						{
							for(Int32 i = 0 ; i < this->collection->get_Count() ; i++)
							{
								xThreadCollection::ThreadInfo *info = this->collection->get_Value(i);
								if(info != null)
								{
									if(!info->numbered)
									{
										if(info->state == System::Threading::Stopped || info->state == System::Threading::Aborted)
										{
											completedThreadNum++;
											info->numbered = true;
											// Check for race.
											//cout << "System::Threading::Stopped" << endl; 
										}
									}
								}
							}
						}
						else
							_AllThreadsCompleted = true;

                        //cout << "completedThreadNum: " << completedThreadNum << endl;
					}

					MakeReadyWriteSocketArrayList();
					readsocks = ::select(highsock + 1, (fd_set *)null, &alWrite, (fd_set *)null, &timeoutWrite);
					if(readsocks == -1)
					{
						//cout << "hhhhhh 1" << endl;//
						WorkerExit(); return;
					}

					if (readsocks != 0)
					{
						if(_AllThreadsCompleted)
						{
							try
							{
								cout << "Parvicursor::xThread::xThreadEvent::AllThreadsCompleted" << endl;
								this->threadChannel->WriteByte(Parvicursor::xThread::AllThreadsCompleted);
								if(this->threadChannel->get_IsSecure())
									this->threadChannel->Flush();
								_AllThreadsCompleted_notification_sent = true;
							}
							catch(ObjectDisposedException &) { RemoveSocketFromArrayList(this->threadChannel); WorkerExit(); return; }
							catch(SocketException &) { RemoveSocketFromArrayList(this->threadChannel); WorkerExit(); return; }
							catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, this->threadChannel) ; WorkerExit(); return; }
							catch(...) { System::Exception e("Unknown exception"); SendExceptionToOneSocketAndClose(e, this->threadChannel) ; WorkerExit(); return; }
						}
						//continue;
					}
				}
				//---------------------Thread Control Channel-------------------------------
				MakeReadyReadSocketArrayList();
				readsocks = ::select(highsock + 1, &alRead, (fd_set *)null, (fd_set *)null, &timeoutRead);
				if(readsocks == -1)
				{
					//cout << "hhhhhh 2" << endl;//
					WorkerExit(); return;
				}

				if (readsocks == 0)
				{
					//goto here;
					continue;
				}

				if(FD_ISSET(this->threadControlChannel->get_BaseSocket()->get_Handle(), &alRead))
				{
					try
					{
						command = this->threadControlChannel->ReadByte();

						//------------ TerminateSession ----------------------------
						if(command == Parvicursor::xThread::TerminateSession)
						{
							cout <<  "Parvicursor::xThread::xThreadEvent::TerminateSession\n" << endl; //
							WorkerExit(); return;
						}
						//------------ SuspendAllThreads ---------------------------
						if(command == Parvicursor::xThread::SuspendAllThreads)
						{
							cout <<  "Parvicursor::xThread::xThreadEvent::SuspendAllThreads" << endl; //

							for(Int32 i = 0 ; i < this->collection->get_Count() ; i++)
							{
								xThreadCollection::ThreadInfo *info = this->collection->get_Value(i);
								if(info->state == System::Threading::Running)
								{
									info->worker->Suspend();
									info->state = System::Threading::Suspended;
								}
							}

#  		                    if defined xThread_With_Error_Recovery_Support

							try
							{
								this->threadControlChannel->WriteNoException();
								if(this->threadControlChannel->get_IsSecure())
									this->threadControlChannel->Flush();
							}
							catch(ObjectDisposedException &) { RemoveSocketFromArrayList(this->threadControlChannel); WorkerExit(); return; }
							catch(SocketException &) { RemoveSocketFromArrayList(this->threadControlChannel); WorkerExit(); return; }
							catch(Exception &e) { SendExceptionToOneSocketAndClose(e, this->threadControlChannel) ; WorkerExit(); return; }
							catch(...) { Exception e("Unknown excpetion"); SendExceptionToOneSocketAndClose(e, this->threadControlChannel) ; WorkerExit(); return; }

#						    endif

						}
						//------------ ResumeAllThreads ----------------------------
						if(command == Parvicursor::xThread::ResumeAllThreads)
						{
							cout <<  "Parvicursor::xThread::xThreadEvent::ResumeAllThreads\n" << endl; //

							for(Int32 i = 0 ; i < this->collection->get_Count() ; i++)
							{
								xThreadCollection::ThreadInfo *info = this->collection->get_Value(i);
								if(info->state == System::Threading::Suspended)
								{
									info->worker->Resume();
									info->state = System::Threading::Running;
								}
							}

#  		                    if defined xThread_With_Error_Recovery_Support

							try
							{
								this->threadControlChannel->WriteNoException();
								if(this->threadControlChannel->get_IsSecure())
									this->threadControlChannel->Flush();
							}
							catch(ObjectDisposedException &) { RemoveSocketFromArrayList(this->threadControlChannel); WorkerExit(); return; }
							catch(SocketException &) { RemoveSocketFromArrayList(this->threadControlChannel); WorkerExit(); return; }
							catch(Exception &e) { SendExceptionToOneSocketAndClose(e, this->threadControlChannel) ; WorkerExit(); return; }
							catch(...) { Exception e("Unknown exception"); SendExceptionToOneSocketAndClose(e, this->threadControlChannel) ; WorkerExit(); return; }

#						    endif

						}
						//------------ SuspendOneThread ----------------------------
						if(command == Parvicursor::xThread::SuspendOneThread)
						{
							cout <<  "Parvicursor::xThread::xThreadEvent::SuspendOneThread\n" << endl; //

							try
							{
								if(this->currentBufferSize < sizeof(Int32))
								{
									if(this->buffer != null)
										delete this->buffer;
									this->buffer = new char[Parvicursor::xThread::xThreadRequest::DefaultBufferSize];
									this->currentBufferSize = Parvicursor::xThread::xThreadRequest::DefaultBufferSize;
								}
								this->threadControlChannel->Read(this->buffer, 0, sizeof(Int32));
								this->deserializer->Reset(this->buffer, this->currentBufferSize);
								Int32 index = this->deserializer->Read<Int32>();

								if(index > this->collection->get_Count() || index < 0)
								{

#  		                            if defined xThread_With_Error_Recovery_Support
                                    Exception ee("Thread index was out of range.");
									this->threadControlChannel->WriteException(ee);
									if(this->threadControlChannel->get_IsSecure())
										this->threadControlChannel->Flush();
#                                   endif
									continue;

								}

								xThreadCollection::ThreadInfo *info = this->collection->get_Value(index);
								if(info != null)
								{
									if(info->state == System::Threading::Running)
									{
										info->worker->Suspend();
										info->state = System::Threading::Suspended;
									}
								}

#								if defined xThread_With_Error_Recovery_Support
								this->threadControlChannel->WriteNoException();
								if(this->threadControlChannel->get_IsSecure())
									this->threadControlChannel->Flush();
#								endif

							}
							catch(ObjectDisposedException &) { RemoveSocketFromArrayList(this->threadControlChannel); WorkerExit(); return; }
							catch(SocketException &) { RemoveSocketFromArrayList(this->threadControlChannel); WorkerExit(); return; }
							catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, this->threadControlChannel) ; WorkerExit(); return; }
							catch(...) { System::Exception e("Unknown excpetion"); SendExceptionToOneSocketAndClose(e, this->threadControlChannel) ; WorkerExit(); return; }


						}
						//------------ ResumeOneThread -----------------------------
						if(command == Parvicursor::xThread::ResumeOneThread)
						{
							cout <<  "Parvicursor::xThread::xThreadEvent::ResumeOneThread\n" << endl; //

							try
							{
								if(this->currentBufferSize < sizeof(Int32))
								{
									if(this->buffer != null)
										delete this->buffer;
									this->buffer = new char[Parvicursor::xThread::xThreadRequest::DefaultBufferSize];
									this->currentBufferSize = Parvicursor::xThread::xThreadRequest::DefaultBufferSize;
								}
								this->threadControlChannel->Read(this->buffer, 0, sizeof(Int32));
								this->deserializer->Reset(this->buffer, this->currentBufferSize);
								Int32 index = this->deserializer->Read<Int32>();

								if(index > this->collection->get_Count() || index < 0)
								{

#  		                            if defined xThread_With_Error_Recovery_Support
                                    Exception ee("Thread index was out of range");
									this->threadControlChannel->WriteException(ee);
									if(this->threadControlChannel->get_IsSecure())
										this->threadControlChannel->Flush();
#                                   endif
									continue;

								}

								xThreadCollection::ThreadInfo *info = this->collection->get_Value(index);
								if(info != null)
								{
									if(info->state == System::Threading::Suspended)
									{
										info->worker->Resume();
										info->state = System::Threading::Running;
									}
								}

#								if defined xThread_With_Error_Recovery_Support
								this->threadControlChannel->WriteNoException();
								if(this->threadControlChannel->get_IsSecure())
									this->threadControlChannel->Flush();
#								endif

							}
							catch(ObjectDisposedException &) { RemoveSocketFromArrayList(this->threadControlChannel); WorkerExit(); return; }
							catch(SocketException &) { RemoveSocketFromArrayList(this->threadControlChannel); WorkerExit(); return; }
							catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, this->threadControlChannel) ; WorkerExit(); return; }
							catch(...) { System::Exception e("Unknown exception"); SendExceptionToOneSocketAndClose(e, this->threadControlChannel) ; WorkerExit(); return; }

						}
						//------------ SyncReceiveOneThread -----------------------
						if(command == Parvicursor::xThread::SyncReceiveOneThread)
						{
							cout <<  "Parvicursor::xThread::xThreadEvent::SyncReceiveOneThread\n" << endl; //

							try
							{
								if(this->currentBufferSize < 1 + sizeof(Int32)) // state+len
								{
									if(this->buffer != null)
										delete this->buffer;
									this->buffer = new char[Parvicursor::xThread::xThreadRequest::DefaultBufferSize];
									this->currentBufferSize = Parvicursor::xThread::xThreadRequest::DefaultBufferSize;
								}
								this->threadControlChannel->Read(this->buffer, 0, sizeof(Int32));
								this->deserializer->Reset(this->buffer, this->currentBufferSize);
								Int32 index = this->deserializer->Read<Int32>();

								if(index > this->collection->get_Count() || index < 0)
								{

#  		                            if defined xThread_With_Error_Recovery_Support
                                    Exception ee("Thread index was out of range");
									this->threadControlChannel->WriteException(ee);
#                                   endif
									continue;

								}

								xThreadCollection::ThreadInfo *info = this->collection->get_Value(index);
								if(info != null)
								{
									if(info->state == System::Threading::Running)
									{
										info->worker->Suspend();
										info->state = System::Threading::Suspended;
									}
								}
								else
									continue;


								//cout << "index: " << index << endl;//

								info->objInstance->Serialize(&info->serializedDLLBuffer, &info->serializedBuffer_size);
								info->currentSerializedBuffer_size = info->serializedBuffer_size;

								if(info->state == System::Threading::Suspended)
									buffer[0] = (char)System::Threading::Running;
								else
									buffer[0] = (char)info->state;
								//cout << "state: " << (ThreadState)buffer[0] << endl;
								buffer += 1;
								this->serializer->Reset(this->buffer, this->currentBufferSize - 1);
								this->serializer->Write<Int32>(info->serializedBuffer_size);
								buffer -= 1;

#								if defined xThread_With_Error_Recovery_Support
								this->threadControlChannel->WriteNoException();
#								endif

								/****disable this section after xSec implementation****/
								this->threadControlChannel->Write(this->buffer, 0, 1 + sizeof(Int32));
								this->threadControlChannel->Write(info->serializedDLLBuffer, 0, info->serializedBuffer_size);
								if(this->threadControlChannel->get_IsSecure())
									this->threadControlChannel->Flush();
								/*******for xThread Scattered I/O transfers*****************/
/*#								if defined WIN32 || WIN64

								this->win_Scattered_IO[0].buf = this->buffer;
								this->win_Scattered_IO[0].len = 1 + sizeof(Int32);

								this->win_Scattered_IO[1].buf = info->serializedDLLBuffer;
								this->win_Scattered_IO[1].len = info->serializedBuffer_size;

								DWORD sent;

								if(WSASend(this->threadControlChannel->get_BaseSocket()->get_Handle(), this->win_Scattered_IO, 2, &sent, 0, null, null) == SOCKET_ERROR)
								{
									int err = WSAGetLastError(); //cout << "hello11" << endl << endl;
									throw SocketException(err, ErrorString(err));
								}

								if(sent != 1 + sizeof(Int32) + info->serializedBuffer_size)
									throw System::Exception("The remote endpoint was closed the connection.");


#								else

								this->posix_Scattered_IO[0].iov_base = this->buffer;
								this->posix_Scattered_IO[0].iov_len = 1 + sizeof(Int32);

								this->posix_Scattered_IO[1].iov_base = info->serializedDLLBuffer;
								this->posix_Scattered_IO[1].iov_len = info->serializedBuffer_size;

								Int32 sent = writev(this->threadControlChannel->get_BaseSocket()->get_Handle(), this->posix_Scattered_IO, 2);

								if(sent == -1)
									throw SocketException(errno, (const char *)_sys_errlist[errno]);

								if(sent != 1 + sizeof(Int32) + info->serializedBuffer_size)
									throw System::Exception("The remote endpoint was closed the connection.");

#								endif*/

								//cout << "sent: " << sent << endl;
								/**********************************************************/

								if(info->state == System::Threading::Suspended)
								{
									info->worker->Resume();
									info->state = System::Threading::Running;
								}


							}
							catch(ObjectDisposedException &) { RemoveSocketFromArrayList(this->threadControlChannel); WorkerExit(); return; }
							catch(SocketException &) { RemoveSocketFromArrayList(this->threadControlChannel); WorkerExit(); return; }
							catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, this->threadControlChannel) ; WorkerExit(); return; }
							catch(...) { System::Exception e("Unknown excpetion"); SendExceptionToOneSocketAndClose(e, this->threadControlChannel) ; WorkerExit(); return; }

						}
						//------------ SyncSendOneThread --------------------------
						if(command == Parvicursor::xThread::SyncSendOneThread)
						{
							cout <<  "Parvicursor::xThread::xThreadEvent::SyncSendOneThread\n" << endl; //

							try
							{
								if(this->currentBufferSize < sizeof(Int32) + sizeof(Int32))
								{
									if(this->buffer != null)
										delete this->buffer;
									this->buffer = new char[Parvicursor::xThread::xThreadRequest::DefaultBufferSize];
									this->currentBufferSize = Parvicursor::xThread::xThreadRequest::DefaultBufferSize;
								}
								this->threadControlChannel->Read(this->buffer, 0, sizeof(Int32) + sizeof(Int32));
								this->deserializer->Reset(this->buffer, this->currentBufferSize);
								Int32 index = this->deserializer->Read<Int32>();
								Int32 bufferSize = this->deserializer->Read<Int32>();

								if(index > this->collection->get_Count() || index < 0)
								{

#  		                            if defined xThread_With_Error_Recovery_Support
                                    Exception ee("Thread index was out of range");
									this->threadControlChannel->WriteException(ee);
									if(this->threadControlChannel->get_IsSecure())
										this->threadControlChannel->Flush();
#                                   endif
									continue;

								}

								xThreadCollection::ThreadInfo *info = this->collection->get_Value(index);
								if(info != null)
								{
									if(info->state == System::Threading::Running)
									{
										info->worker->Suspend();
										info->state = System::Threading::Suspended;
									}
								}
								else
									continue;


								cout << "index: " << index << endl;//
								cout << "bufferSize: " << bufferSize << endl;//
								//continue;


								if(info->serializedDLLBuffer == null)
								{
									/*if(bufferSize > Parvicursor::xThread::xThreadClient::DefaultBufferSize)
										info->serializedBuffer_size = bufferSize;
									else
										info->serializedBuffer_size = Parvicursor::xThread::xThreadClient::DefaultBufferSize;
									info->serializedDLLBuffer = new char[info->serializedBuffer_size];*/
									info->serializedBuffer_size = bufferSize;
									info->serializedDLLBuffer = (char *)info->objInstance->AllocateDllMemory(info->serializedBuffer_size);
									info->currentSerializedBuffer_size = info->serializedBuffer_size;
								}
								else
								{
									if(bufferSize > info->serializedBuffer_size)
									{
										info->objInstance->FreeDllMemory(info->serializedDLLBuffer);
										info->serializedBuffer_size = bufferSize;
										info->serializedDLLBuffer = (char *)info->objInstance->AllocateDllMemory(info->serializedBuffer_size);
										info->currentSerializedBuffer_size = info->serializedBuffer_size;
									}
									else
										info->currentSerializedBuffer_size = bufferSize;
								}

								this->threadControlChannel->Read(info->serializedDLLBuffer, 0, info->currentSerializedBuffer_size);
								info->objInstance->DeSerialize(&info->serializedDLLBuffer, &info->currentSerializedBuffer_size);

#								if defined xThread_With_Error_Recovery_Support
								this->threadControlChannel->WriteNoException();
								if(this->threadControlChannel->get_IsSecure())
									this->threadControlChannel->Flush();
#								endif

								if(info->state == System::Threading::Suspended)
								{
									info->worker->Resume();
									info->state = System::Threading::Running;
								}



							}
							catch(ObjectDisposedException &) { RemoveSocketFromArrayList(this->threadControlChannel); WorkerExit(); return; }
							catch(SocketException &) { RemoveSocketFromArrayList(this->threadControlChannel); WorkerExit(); return; }
							catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, this->threadControlChannel) ; WorkerExit(); return; }
							catch(...) { System::Exception e("Unknown excpetion"); SendExceptionToOneSocketAndClose(e, this->threadControlChannel) ; WorkerExit(); return; }

						}
						//------------ SyncReceiveAllThreads ----------------------
						// This can be implemented by client via calling successive SyncReceiveOneThread(Int32 index) method.
						//------------ SyncSendAllThreads -------------------------
						// This can be implemented by client via calling successive SyncSendOneThread(Int32 index) method.
						//------------ AbortOneThread ------------------------------
						if(command == Parvicursor::xThread::AbortOneThread)
						{
							cout <<  "Parvicursor::xThread::xThreadEvent::AbortOneThread\n" << endl; //

							try
							{
								if(this->currentBufferSize < sizeof(Int32))
								{
									if(this->buffer != null)
										delete this->buffer;
									this->buffer = new char[Parvicursor::xThread::xThreadRequest::DefaultBufferSize];
									this->currentBufferSize = Parvicursor::xThread::xThreadRequest::DefaultBufferSize;
								}
								this->threadControlChannel->Read(this->buffer, 0, sizeof(Int32));
								this->deserializer->Reset(this->buffer, this->currentBufferSize);
								Int32 index = this->deserializer->Read<Int32>();

								if(index > this->collection->get_Count() || index < 0)
								{

#  		                            if defined xThread_With_Error_Recovery_Support
                                    Exception ee("Thread index was out of range");
									this->threadControlChannel->WriteException(ee);
									if(this->threadControlChannel->get_IsSecure())
										this->threadControlChannel->Flush();
#                                   endif
									continue;

								}

								xThreadCollection::ThreadInfo *info = this->collection->get_Value(index);
								if(info == null)
									continue;

								if(info->worker != null)
								{
									info->state = System::Threading::AbortRequested;
									printf("pthread_cancel: %d\n" , pthread_cancel(info->worker->thread)); //			exit(0);
									info->state = System::Threading::Aborted;
								}

								//completedThreadNum++;

/*								if(info->serializedBuffer != null)
								{
									delete info->serializedBuffer;
									info->serializedBuffer = null;
								}

								if(info->serializedDLLBuffer != null)
								{
									info->objInstance->FreeDllMemory(info->serializedDLLBuffer);
									info->serializedDLLBuffer = null;
								}

								if(info->objInstance != null)
								{
									info->objInstance->FreeDllMemory(info->objInstance);
									info->objInstance = null;
								}

								if(info->lib_handle != null)
								{

#									if defined WIN32 || WIN64

									//cout << "He: "<< endl;
									//cout << "FreeLibrary: "<<
									FreeLibrary(info->lib_handle); // << endl;
									//FreeLibraryAndExitThread(temp->lib_handle, -1);

#									else

									dlclose(info->lib_handle);

#									endif
									info->lib_handle = null;

								}

								this->collection->set->Remove(info);

								if(info != null)
								{
									delete info;
									info = null;
								}*/

#								if defined xThread_With_Error_Recovery_Support
								this->threadControlChannel->WriteNoException();
								if(this->threadControlChannel->get_IsSecure())
									this->threadControlChannel->Flush();
#								endif

							}
							catch(ObjectDisposedException &) { RemoveSocketFromArrayList(this->threadControlChannel); WorkerExit(); return; }
							catch(SocketException &) { RemoveSocketFromArrayList(this->threadControlChannel); WorkerExit(); return; }
							catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, this->threadControlChannel) ; WorkerExit(); return; }
							catch(...) { System::Exception e("Unknown exception"); SendExceptionToOneSocketAndClose(e, this->threadControlChannel) ; WorkerExit(); return; }

						}
						//----------------------------------------------------------
						if(command == Parvicursor::xThread::AuxiliaryMehtodOneThread)
						{
							cout <<  "Parvicursor::xThread::xThreadEvent::AuxiliaryMehtodOneThread\n" << endl; //

							try
							{
								Int32 index = -1;
								//char *_index = (char *)&index;
								Byte _index[4];

								//printf("xThreadRequest::AuxiliaryMehtodOneThread 1\n");///

								this->threadControlChannel->Read((char *)_index, 0, sizeof(Int32));
								//index = (Int32)(*_index);
								index = Parvicursor_GetInt32NumberFromBytes(_index);

								//printf("index: %d\n", index);//

								//printf("xThreadRequest::AuxiliaryMehtodOneThread 2 index: %d \n", index);///

								if(index > this->collection->get_Count() || index < 0)
								{

#  		                            if defined xThread_With_Error_Recovery_Support
									Exception ee("Thread index was out of range");
									this->threadControlChannel->WriteException(ee);
#                                   endif
									continue;

								}

								Int32 inputLength = -1;
								//char *_inputLength = (char *)&inputLength;
								Byte _inputLength[4];
								this->threadControlChannel->Read((char *)_inputLength, 0, sizeof(Int32));
								//inputLength = (Int32)(*_inputLength);
								inputLength = Parvicursor_GetInt32NumberFromBytes(_inputLength);
								//printf("xThreadRequest::AuxiliaryMehtodOneThread 3 inputLength: %d \n", inputLength);///
								//printf("inputLength: %d\n", inputLength);//
								if(inputLength <= 0)
									throw System::ArgumentOutOfRangeException("inputLength", "inputLength must be greater than zero.");
								if(this->currentBufferSize < inputLength)
								{
									if(this->buffer != null)
										delete this->buffer;
									this->buffer = new char[inputLength];
									this->currentBufferSize = inputLength;
								}

								this->threadControlChannel->Read((char *)buffer, 0, inputLength);

								xThreadCollection::ThreadInfo *info = this->collection->get_Value(index);
								if(info == null)
									continue;

								Int32 outputLength = 0;
								//char *_outputLength;
								char _outputLength[4];
								void *output = null;

								//printf("xThreadRequest::AuxiliaryMehtodOneThread 4\n");///

								info->objInstance->AuxiliaryMehtod((const void *)buffer, inputLength, &output, &outputLength);
								//printf("xThreadRequest::AuxiliaryMehtodOneThread 5\n");///
								//printf("inputLength: %d outputLength: %d output: %p\n", inputLength, outputLength, output);

								if(outputLength <= 0)
									throw ArgumentOutOfRangeException("outputLength", "outputLength must be greater than zero.");
								if(output == null)
									throw ArgumentNullException("output", "output can not be null.");

								//_outputLength = (char *)&outputLength;
								Parvicursor_GetBytesFromInt32Number(_outputLength, outputLength);


								this->threadControlChannel->Write(_outputLength, 0, sizeof(Int32));
								this->threadControlChannel->Write((char *)output, 0, outputLength);
/*#if defined WIN32 || WIN64

								this->win_Scattered_IO[0].buf = _outputLength;
								this->win_Scattered_IO[0].len = sizeof(Int32);

								this->win_Scattered_IO[1].buf = (char *)output;
								this->win_Scattered_IO[1].len = outputLength;
								DWORD sent;

								if(WSASend(this->threadControlChannel->get_BaseSocket()->get_Handle(), this->win_Scattered_IO, 2, &sent, 0, null, null) == SOCKET_ERROR)
								{
									int err = WSAGetLastError();
									printf("err: %s\n", ErrorString(err));
									throw SocketException(err, ErrorString(err));
								}
								//printf("sent: %d\n", sent);
								if(sent != sizeof(Int32) + outputLength)
									throw System::Exception("The remote endpoint was closed the connection.");

#else

								this->posix_Scattered_IO[0].iov_base = _outputLength;
								this->posix_Scattered_IO[0].iov_base.len = sizeof(Int32);

								this->posix_Scattered_IO[1].iov_base = (char *)output;
								this->posix_Scattered_IO[1].iov_len = outputLength;

								Int32 sent = writev(this->threadControlChannel->get_BaseSocket()->get_Handle(), this->posix_Scattered_IO, 2);

								if(sent == -1)
									throw SocketException(errno, (const char *)_sys_errlist[errno]);

								if(sent != sizeof(Int32) + outputLength)
									throw Exception("The remote endpoint was closed the connection.");

#endif*/

#								if defined xThread_With_Error_Recovery_Support
								this->threadControlChannel->WriteNoException();
#								endif

								if(this->threadControlChannel->get_IsSecure())
									this->threadControlChannel->Flush();

							}
							catch(ObjectDisposedException &) { RemoveSocketFromArrayList(this->threadControlChannel); WorkerExit(); return; }
							catch(SocketException &) { RemoveSocketFromArrayList(this->threadControlChannel); WorkerExit(); return; }
							catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, this->threadControlChannel) ; WorkerExit(); return; }
							catch(...) { System::Exception e("Unknown exception"); SendExceptionToOneSocketAndClose(e, this->threadControlChannel) ; WorkerExit(); return; }

						}
						//----------------------------------------------------------
					}
					catch(ObjectDisposedException &) { RemoveSocketFromArrayList(this->threadControlChannel); WorkerExit(); return; }
					catch(SocketException &) { RemoveSocketFromArrayList(this->threadControlChannel); WorkerExit(); return; }
					catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, this->threadControlChannel) ; WorkerExit(); return; }
					catch(...) { System::Exception e("Unknown exception"); SendExceptionToOneSocketAndClose(e, this->threadControlChannel) ; WorkerExit(); return; }
				}
				//--------------------------------------------------------------------------
				//Thread::Sleep(_timeout);
			}
		//End:
			WorkerExit();
			//Thread::Sleep(100000);
			return ;
		}
		//----------------------------------------------------
		void xThreadRequest::SendExceptionToOneSocketAndClose(System::Exception &e, ParvicursorSocket *socket)
		{
			try { socket->WriteException(e); RemoveSocketFromArrayList(socket); }
			catch(...) { RemoveSocketFromArrayList(socket); }
		}
		//----------------------------------------------------
		void xThreadRequest::MakeReadyReadSocketArrayList()
		{
			/* FD_ZERO() clears out the fd_set called socks, so that
				it doesn't contain any file descriptors. */
			FD_ZERO(&alRead);
			Int32 handle = this->threadControlChannel->get_BaseSocket()->get_Handle();

			FD_SET(handle, &alRead);

			if (handle > highsock)
				highsock = handle;


			return ;
		}
		//----------------------------------------------------
		void xThreadRequest::MakeReadyWriteSocketArrayList()
		{
			/* FD_ZERO() clears out the fd_set called socks, so that
				it doesn't contain any file descriptors. */
			FD_ZERO(&alWrite);
			Int32 handle = this->threadChannel->get_BaseSocket()->get_Handle();

			FD_SET(handle, &alWrite);

			if (handle > highsock)
				highsock = handle;


			return ;
		}
		//----------------------------------------------------
		void xThreadRequest::RemoveSocketFromArrayList(ParvicursorSocket *socket)
		{
			int i = FindSocketIndex(socket);
			if(i != -1)
			{
				ParvicursorSocket *s = (ParvicursorSocket *)sockets->get_Value(i);
				//try{s->get_BaseSocket()->Shutdown(Both);} catch(...) { }
				s->Close();

				sockets->RemoveAt(i);
				delete s;
			}
		}
		//----------------------------------------------------
		int xThreadRequest::FindSocketIndex(ParvicursorSocket *socket)
		{
			for(int i = 0 ; i < sockets->get_Count() ; i++)
				if((ParvicursorSocket *)sockets->get_Value(i) == socket)
					return i;
			return -1;
		}
		//----------------------------------------------------
		void xThreadRequest::WorkerExit()
		{
			mutex->Lock();
			if(this->sockets != null)
			{
				if(this->sockets->get_Count() != 0)
				{
					for(int i = 0 ; i < this->sockets->get_Count() ; i++)
					{
						ParvicursorSocket *s = (ParvicursorSocket *)this->sockets->get_Value(i);
						//s->get_BaseSocket()->Shutdown(Both);
                        //s->Close();
                        if(s != null)
                        {
                            //try {s->get_BaseSocket()->Shutdown(Both);} catch(...) { }
                            s->Close();
                            delete s;
							//cout << "\n\n\n\n\neeeeeeeeee\n\n\n\n" << endl;
                            s = null;
                        }
					}
				}

                this->sockets->Clear();
				delete this->sockets;
				this->sockets = null;
			}
			mutex->Unlock();

			if(buffer != null)
			{
				delete buffer;
				this->buffer = null;
			}

			if(this->serializer != null)
			{
				delete this->serializer;
				this->serializer = null;
			}
			if(this->deserializer != null)
			{
				delete this->deserializer;
				this->deserializer = null;
			}
			// here delete the created threads
			if(this->collection != null)
			{
				delete this->collection;
				this->collection = null;
			}

			if(mutex != null)
			{
				delete mutex;
				mutex = null;
			}

			return ;
		}
   		//----------------------------------------------------
   };
};
//**************************************************************************************************************//
