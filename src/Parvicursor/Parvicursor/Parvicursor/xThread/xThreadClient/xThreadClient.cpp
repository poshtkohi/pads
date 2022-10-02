/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "xThreadClient.h"

//**************************************************************************************************************//
namespace Parvicursor
{
   namespace xThread
   {
 		//----------------------------------------------------
		xThreadClient::xThreadClient(xThreadCollection *collection, const String &xThreadServerAddress, NetworkCredential *nc, bool secure/*, ref RijndaelEncryption rijndael*/, ArrayList *errors)
		{
			if(xThreadServerAddress.get_BaseStream() == null)
				throw ArgumentNullException("xThreadServerAddress is a null reference.");

			if(&nc == null)
				throw ArgumentNullException("nc is a null reference.");

			if(collection->get_Count() == 0)
				throw ArgumentNullException("collection is empty.");

			this->xThreadServerAddress = xThreadServerAddress;
			this->nc = nc;
			this->collection = collection;
			this->secure = secure;
			this->errors = errors;
			//this->worker = null;
			this->closed = false;
			this->ended = false;
			this->disposed = false;
			this->are_SuspendedAllThreads = false;
			this->_AreAllThreadsCompleted = false;
			this->buffer = null;
			this->socket = null;
			this->fs = null;
			this->sockets = null;
			this->serializer = null;
			this->deserializer = null;

			this->sockets = new ArrayList();

			this->guid = Guid::NewGuid();

			mutex = new Mutex();
			cv = new ConditionVariable(mutex);
			sessionInitiated = false;
			isClosedByRuntimeManager = false;
		}
		//----------------------------------------------------
		void xThreadClient::WaitForSessionInitiation()
		{
			mutex->Lock();
			if(closed)
			{
				mutex->Unlock();
				throw ObjectDisposedException("xThreadClient", "The xThreadClient has been closed");
			}
			if(sessionInitiated)
			{
				mutex->Unlock();
				return;
			}
			while(!sessionInitiated && !closed)
				cv->Wait();
			mutex->Unlock();
			return ;
		}
		//----------------------------------------------------
		bool xThreadClient::get_IsAlive()
		{
			mutex->Lock();
			if(isClosedByRuntimeManager || closed)
			{
				mutex->Unlock();
				return false;
			}
			bool ret = false;
			if(sessionInitiated)
				ret = true;

			mutex->Unlock();
			return ret;
		}
		//----------------------------------------------------
		xThreadClient::~xThreadClient()
		{
			if(!this->disposed)
			{
				Close();
				disposed = true;
				delete mutex;
				delete cv;
			}
		}
		//----------------------------------------------------
		void xThreadClient::WorkerProc() // sock[0]:thread channel, sock[1]: thread control channel
		{
			pthread_setcanceltype(PTHREAD_CANCEL_ENABLE, null);
			pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, null);

			IPHostEntry hostEntry;
			try
			{
				hostEntry = Dns::Resolve2(this->xThreadServerAddress);
			}
			catch(System::Exception &e)
			{
				if(this->errors != null)
					this->errors->Add(new System::Exception(e.get_Message()));

				WorkerExit(); return;
			}
			catch(...)
			{
				if(this->errors != null)
						this->errors->Add(new System::Exception("Unknown exception was occured"));

				WorkerExit(); return;

			}

			IPAddress ip = hostEntry.get_AddressList(0);
			IPEndPoint inp = IPEndPoint(ip, __xDFS_Server_Default_Port__);

			Serializer se(512);
			se.Write(this->guid);// cout << this->guid.get_BaseStream()  << endl;

			for(register Int32 i = 0 ; i < 2 ; i++)
			{
				Socket *sock = new Socket(ip.get_AddressFamily(), Stream, tcp);

				if(sock == null)
				{
					//pthread_mutex_unlock(&this->mutex);
					throw ObjectDisposedException("Could not instantiate from Socket class.");
				}

				try
				{
					ParvicursorSocket *s;
					if(secure)
					{
						//printf("I'm secure1: i: %d\n", i);
						s = new ParvicursorSocket(sock, ParvicursorSocket::DefaultCryptographyBufferSize);
					}
					else
						s = new ParvicursorSocket(sock);

					this->sockets->Add(s);
					s->get_BaseSocket()->Connect(inp);

					s->WriteByte((char)xThreadRequestFromClient);
					s->WriteObject(se.get_BaseBuffer(), se.get_BaseBufferSize());
					s->CheckExceptionResponse();

					//printf("I'm secure2: i: %d\n", i);//

					if(i == 0)
						this->threadChannel = s;

					if(i == 1)
						this->threadControlChannel= s;

				}
				catch(System::Exception &e)
				{
					if(this->errors != null)
						this->errors->Add(new System::Exception(e.get_Message()));

					//throw Exception("See errors list for error details.");
					WorkerExit(); return;
				}
				catch(...)
				{
					if(this->errors != null)
							this->errors->Add(new System::Exception("Unknown exception was occured"));

					//throw Exception("See errors list for error details.");
					WorkerExit(); return;
				}
			}

			try
			{
				this->serializer = this->collection->Serialize();

				this->threadChannel->WriteObject(this->serializer->get_BaseBuffer(), this->serializer->get_BaseBufferSize());

				//cout << "hello2" << endl; Thread::Sleep(100000); return;

				this->threadChannel->CheckExceptionResponse();

				delete this->serializer;
				this->serializer = null;
			}
			catch(System::Exception &e)
			{
				if(this->errors != null)
					this->errors->Add(new System::Exception(e.get_Message()));

				//throw Exception("See errors list for error details.");
				if(this->serializer != null)
				{
					delete this->serializer;
					this->serializer = null;
				}

				WorkerExit(); return;
			}
			catch(...)
			{
				if(this->errors != null)
						this->errors->Add(new System::Exception("Unknown exception was occured"));

				//throw Exception("See errors list for error details.");
				if(this->serializer != null)
				{
					delete this->serializer;
					this->serializer = null;
				}

				mutex->Unlock();
				WorkerExit(); return;
			}

			for(Int32 i = 0 ; i < this->collection->get_Count() ; i++)
			{
				xThreadCollection::ThreadInfo *info = this->collection->get_Value(i);
				if(info != null)
					info->state = System::Threading::Running;
			}

			mutex->Lock();
			sessionInitiated = true;
			cv->Signal();
			mutex->Unlock();

			this->buffer = new char[Parvicursor::xThread::xThreadClient::DefaultBufferSize];
			this->currentBufferSize = Parvicursor::xThread::xThreadClient::DefaultBufferSize;
			this->serializer = new Serializer(buffer, this->currentBufferSize);
			this->deserializer = new DeSerializer(buffer, this->currentBufferSize);

			//pthread_mutex_unlock(&this->mutex);

			//printf("I'm secure2\n");

			//----------------------------- Thread Channel ---------------------------
			//printf("I'm secure3\n");//
			int readsocks;
			Byte command = 0;
			struct timeval timeoutRead;  // Timeout for select
			timeoutRead.tv_sec = 0;
			timeoutRead.tv_usec = 1000; // 1ms
			// do manage error handling
			while(true)
			{
				if(sockets->get_Count() == 0 //|| written >= info.FileSize
				)
				break;

				MakeReadyReadSocketArrayList();

				readsocks = ::select(highsock + 1, &alRead, (fd_set *)null, (fd_set *)null, null);
				if(readsocks == -1)
				{
					WorkerExit(); return;
				}

				if (readsocks == 0)
				{
					//goto here;
					continue;
				}
				if(FD_ISSET(this->threadChannel->get_BaseSocket()->get_Handle(), &alRead))
				{
					try
					{
						command = this->threadChannel->ReadByte();
						if(command == Parvicursor::xThread::AllThreadsCompleted)
						{
							//cout << "AllThreadsCompleted" << endl;
							mutex->Lock();
							this->_AreAllThreadsCompleted = true;
							mutex->Unlock();
							continue;
						}
						if(command == Parvicursor::xThread::TerminateSession)
						{
							if(this->errors != null)
								this->errors->Add(new System::Exception("The remote xThread server abnormaly terminated the session over the thread channel"));

							isClosedByRuntimeManager = true;
							WorkerExit(); return;
						}
						else
						{
							if(this->errors != null)
								this->errors->Add(new System::Exception("Unknown received Parvicursor::xThread::xThreadEvent from remote xThread server over the thread channel"));

							WorkerExit(); return;
						}
					}
					catch(System::Exception &e)
					{
						if(this->errors != null)
							this->errors->Add(new System::Exception(e.get_Message() + " over the thread channel"));

						WorkerExit(); return;
					}
					catch(...)
					{
						if(this->errors != null)
								this->errors->Add(new System::Exception("Unknown exception was occured over the thread channel"));

						WorkerExit(); return;

					}
				}
				//Thread::Sleep(_timeout);
			}
			//------------------------------------------------------------------------

			WorkerExit();
			exited = true;
		}
		//----------------------------------------------------
		void xThreadClient::SendOneExceptionToAllSocketsAndClose(System::Exception &e)
		{
			return ;
		}
		//----------------------------------------------------
		void xThreadClient::SendExceptionToOneSocketAndClose(System::Exception &e, ParvicursorSocket *socket)
		{
			try { socket->WriteException(e); RemoveSocketFromArrayList(socket); }
			catch(...) { RemoveSocketFromArrayList(socket); }
		}
		//----------------------------------------------------
		void xThreadClient::MakeReadyReadSocketArrayList()
		{
			/* FD_ZERO() clears out the fd_set called socks, so that
				it doesn't contain any file descriptors. */
			FD_ZERO(&alRead);
			Int32 handle = this->threadChannel->get_BaseSocket()->get_Handle();

			FD_SET(handle, &alRead);

            highsock = -1;
			if (handle > highsock)
				highsock = handle;


			return ;
		}
		//----------------------------------------------------
		void xThreadClient::RemoveSocketFromArrayList(ParvicursorSocket *socket)
		{
			Int32 i = FindSocketIndex(socket);
			if(i != -1)
			{
				ParvicursorSocket *s = (ParvicursorSocket *)sockets->get_Value(i);
				// s->get_BaseSocket()->Shutdown(Both);
				s->Close();

				sockets->RemoveAt(i);
				delete s;
			}
		}
		//----------------------------------------------------
		Int32 xThreadClient::FindSocketIndex(ParvicursorSocket *socket)
		{
			for(register Int32 i = 0 ; i < sockets->get_Count() ; i++)
				if((ParvicursorSocket *)sockets->get_Value(i) == socket)
					return i;
			return -1;
		}
		//----------------------------------------------------
		void xThreadClient::TerminateSession()
		{
			mutex->Lock();

			if(isClosedByRuntimeManager)
			{
				mutex->Unlock();
				return ;
			}
			if(!this->closed)
			{
				//cout << "pthread_cancel: " << pthread_cancel(this->thread) << endl; on success 0 value will be returned.
				pthread_cancel(this->thread);

				try
				{
					this->threadControlChannel->WriteByte(Parvicursor::xThread::TerminateSession);
					if(this->threadControlChannel->get_IsSecure())
						this->threadControlChannel->Flush();

					for(register Int32 i = 0 ; i < this->collection->get_Count() ; i++)
					{
						xThreadCollection::ThreadInfo *info = this->collection->get_Value(i);
						if(info != null)
							info->state = System::Threading::Aborted;
					}
				}
				catch(System::Exception &e)
				{
					if(this->errors != null)
						this->errors->Add(new System::Exception(e.get_Message()));

				}
				catch(...)
				{
					if(this->errors != null)
							this->errors->Add(new System::Exception("Unknown exception was occured"));

				}

				mutex->Unlock();
				this->Close();
				return;
			}
			else
			{
				mutex->Unlock();
				throw ObjectDisposedException("xThreadClient", "The xThreadClient has been closed");
			}
		}
		//----------------------------------------------------
		void xThreadClient::SuspendAllThreads()
		{
			mutex->Lock();

			if(!this->closed)
			{
				if(this->are_SuspendedAllThreads)
				{
					mutex->Unlock();
					return;
				}

				try
				{
					this->threadControlChannel->WriteByte(Parvicursor::xThread::SuspendAllThreads);
					if(this->threadControlChannel->get_IsSecure())
						this->threadControlChannel->Flush();

#                   if defined xThread_With_Error_Recovery_Support
					this->threadControlChannel->CheckExceptionResponse(); //
#					endif

					this->are_SuspendedAllThreads = true;
					for(register Int32 i = 0 ; i < this->collection->get_Count() ; i++)
					{
						xThreadCollection::ThreadInfo *info = this->collection->get_Value(i);
						if(info != null)
							if(info->state == System::Threading::Running)
								info->state = System::Threading::Suspended;
					}
					mutex->Unlock();
					return;
				}
				catch(System::Exception &e)
				{
					if(this->errors != null)
						this->errors->Add(new System::Exception(e.get_Message()));

					mutex->Unlock();
					this->Close();
					return;
				}
				catch(...)
				{
					if(this->errors != null)
							this->errors->Add(new System::Exception("Unknown exception was occured"));

					mutex->Unlock();
					this->Close();
					return;
				}
			}
			else
			{
				mutex->Unlock();
				throw ObjectDisposedException("xThreadClient", "The xThreadClient has been closed");
			}
		}
		//----------------------------------------------------
		void xThreadClient::ResumeAllThreads()
		{
			mutex->Lock();

			if(!this->closed)
			{
				if(!this->are_SuspendedAllThreads)
				{
					mutex->Unlock();
					return;
				}

				try
				{
					this->threadControlChannel->WriteByte(Parvicursor::xThread::ResumeAllThreads);
					if(this->threadControlChannel->get_IsSecure())
						this->threadControlChannel->Flush();

#                   if defined xThread_With_Error_Recovery_Support
					this->threadControlChannel->CheckExceptionResponse(); //
#					endif

					this->are_SuspendedAllThreads = false;
					for(register Int32 i = 0 ; i < this->collection->get_Count() ; i++)
					{
						xThreadCollection::ThreadInfo *info = this->collection->get_Value(i);
						if(info != null)
							if(info->state == System::Threading::Suspended)
								info->state = System::Threading::Running;
					}
					mutex->Unlock();
					return;
				}
				catch(System::Exception &e)
				{
					if(this->errors != null)
						this->errors->Add(new System::Exception(e.get_Message()));

					mutex->Unlock();
					this->Close();
					return;
				}
				catch(...)
				{
					if(this->errors != null)
							this->errors->Add(new System::Exception("Unknown exception was occurred"));

					mutex->Unlock();
					this->Close();
					return;
				}
			}
			else
			{
				mutex->Unlock();
				throw ObjectDisposedException("xThreadClient", "The xThreadClient has been closed");
			}
		}
		//----------------------------------------------------
		void xThreadClient::SuspendOneThread(Int32 index)
		{
			mutex->Lock();

			if(!this->closed)
			{

				if(index < 0)
				{
					mutex->Unlock();
					throw ArgumentOutOfRangeException("index", "index must be greater than or equal to zero");
				}

				if(this->are_SuspendedAllThreads)
				{
					mutex->Unlock();
					return;
				}

				if(index > this->collection->get_Count())
				{
					mutex->Unlock();
					throw ArgumentOutOfRangeException("index", "index is out of range");
				}

				xThreadCollection::ThreadInfo *info = this->collection->get_Value(index);
				if(info != null)
				{
					if(info->state != System::Threading::Running || info->state == System::Threading::Aborted)
					{
						mutex->Unlock();
						throw ThreadStateException("The thread has not been started or is dead.");
					}
				}
				else
				{
					mutex->Unlock();
					return ;
				}

				try
				{
					if(this->currentBufferSize < 1 + sizeof(Int32))
					{
						if(this->buffer != null)
							delete this->buffer;
						this->buffer = new char[Parvicursor::xThread::xThreadClient::DefaultBufferSize];
						this->currentBufferSize = Parvicursor::xThread::xThreadClient::DefaultBufferSize;
					}
					buffer[0] = Parvicursor::xThread::SuspendOneThread;
					buffer++;
					this->serializer->Reset(this->buffer, this->currentBufferSize - 1);
					this->serializer->Write<Int32>(index);
					buffer--;
					this->threadControlChannel->Write(this->buffer, 0, 1 + sizeof(Int32));
					if(this->threadControlChannel->get_IsSecure())
						this->threadControlChannel->Flush();

#                   if defined xThread_With_Error_Recovery_Support
					this->threadControlChannel->CheckExceptionResponse(); //
#					endif

					info->state = System::Threading::Suspended;

					mutex->Unlock();
					return;
				}
				catch(System::Exception &e)
				{
					if(this->errors != null)
						this->errors->Add(new System::Exception(e.get_Message()));

					mutex->Unlock();
					this->Close();
					return;
				}
				catch(...)
				{
					if(this->errors != null)
							this->errors->Add(new System::Exception("Unknown exception was occured"));

					mutex->Unlock();
					this->Close();
					return;
				}
			}
			else
			{
				mutex->Unlock();
				throw ObjectDisposedException("xThreadClient", "The xThreadClient has been closed");
			}
		}
		//----------------------------------------------------
		void xThreadClient::ResumeOneThread(Int32 index)
		{
			mutex->Lock();

			if(!this->closed)
			{
				if(index < 0)
				{
					mutex->Unlock();
					throw ArgumentOutOfRangeException("index", "index must be greater than or equal to zero");
				}

				//pthread_mutex_lock(&this->mutex);

				if(index > this->collection->get_Count())
				{
					mutex->Unlock();
					throw ArgumentOutOfRangeException("index", "index is out of range");
				}

				xThreadCollection::ThreadInfo *info = this->collection->get_Value(index);
				if(info != null)
				{
					if(info->state != System::Threading::Running || info->state == System::Threading::Aborted || info->state == System::Threading::Suspended)
					{
						mutex->Unlock();
						throw ThreadStateException("The thread has not been started, is dead, or is not in the suspended state.");
					}
				}
				else
				{
					mutex->Unlock();
					return ;
				}

				try
				{
					if(this->currentBufferSize < 1 + sizeof(Int32))
					{
						if(this->buffer != null)
							delete this->buffer;
						this->buffer = new char[Parvicursor::xThread::xThreadClient::DefaultBufferSize];
						this->currentBufferSize = Parvicursor::xThread::xThreadClient::DefaultBufferSize;
					}
					buffer[0] = Parvicursor::xThread::ResumeOneThread;
					buffer++;
					this->serializer->Reset(this->buffer, this->currentBufferSize - 1);
					this->serializer->Write<Int32>(index);
					buffer--;
					this->threadControlChannel->Write(this->buffer, 0, 1 + sizeof(Int32));
					if(this->threadControlChannel->get_IsSecure())
						this->threadControlChannel->Flush();

#                   if defined xThread_With_Error_Recovery_Support
					this->threadControlChannel->CheckExceptionResponse(); //
#					endif

					info->state = System::Threading::Running;

					if(this->are_SuspendedAllThreads)
						this->are_SuspendedAllThreads = false;

					mutex->Unlock();
					return;
				}
				catch(System::Exception &e)
				{
					if(this->errors != null)
						this->errors->Add(new System::Exception(e.get_Message()));

					mutex->Unlock();
					this->Close();
					return;
				}
				catch(...)
				{
					if(this->errors != null)
							this->errors->Add(new System::Exception("Unknown exception was occured"));

					mutex->Unlock();
					this->Close();
					return;

				}
			}
			else
			{
				mutex->Unlock();
				throw ObjectDisposedException("xThreadClient", "The xThreadClient has been closed");
			}
		}
		//----------------------------------------------------
		void xThreadClient::SyncReceiveOneThread(Int32 index)
		{
			mutex->Lock();

			if(!this->closed)
			{
				if(index < 0)
				{
					mutex->Unlock();
					throw ArgumentOutOfRangeException("index", "index must be greater than or equal to zero");
				}

				if(index > this->collection->get_Count())
				{
					mutex->Unlock();
					throw ArgumentOutOfRangeException("index", "index is out of range");
				}

				xThreadCollection::ThreadInfo *info = this->collection->get_Value(index);
				if(info != null)
				{
					if(info->state == System::Threading::Aborted || info->state == System::Threading::Stopped || info->state == System::Threading::AbortRequested || info->state == System::Threading::SuspendRequested)
					{
						mutex->Unlock();
						return ;
					}
				}
				else
				{
					mutex->Unlock();
					return ;
				}

				try
				{
					if(this->currentBufferSize < 1 + sizeof(Int32)) // state+len
					{
						if(this->buffer != null)
							delete this->buffer;
						this->buffer = new char[Parvicursor::xThread::xThreadClient::DefaultBufferSize];
						this->currentBufferSize = Parvicursor::xThread::xThreadClient::DefaultBufferSize;
					}

					buffer[0] = Parvicursor::xThread::SyncReceiveOneThread;
					buffer++;
					this->serializer->Reset(this->buffer, this->currentBufferSize - 1);
					this->serializer->Write<Int32>(index);
					buffer--;

					this->threadControlChannel->Write(this->buffer, 0, 1 + sizeof(Int32));
					if(this->threadControlChannel->get_IsSecure())
						this->threadControlChannel->Flush();

#                   if defined xThread_With_Error_Recovery_Support
					this->threadControlChannel->CheckExceptionResponse(); //
#					endif

					this->threadControlChannel->Read(this->buffer, 0, 1 + sizeof(Int32));
					ThreadState state = (ThreadState)this->buffer[0];// check error here for incorrect status
					this->buffer++;
					this->deserializer->Reset(this->buffer, this->currentBufferSize - 1);
					Int32 bufferSize = this->deserializer->Read<Int32>();
					this->buffer--;
					if(bufferSize == 0)
					{
						mutex->Unlock();
						return;
					}
					if(info->serializedBuffer == null)
					{
						/*if(bufferSize > Parvicursor::xThread::xThreadClient::DefaultBufferSize)
							info->serializedBuffer_size = bufferSize;
						else
							info->serializedBuffer_size = Parvicursor::xThread::xThreadClient::DefaultBufferSize;
						info->serializedBuffer = new char[info->serializedBuffer_size];*/
						info->serializedBuffer = new char[bufferSize];
						info->serializedBuffer_size = bufferSize;
						info->currentSerializedBuffer_size = bufferSize;
					}
					else
					{
						if(bufferSize > info->serializedBuffer_size)
						{
							delete info->serializedBuffer;
							info->serializedBuffer_size = bufferSize;
							info->serializedBuffer = new char[info->serializedBuffer_size];
							info->currentSerializedBuffer_size = info->serializedBuffer_size;
						}
						else
							info->currentSerializedBuffer_size = bufferSize;
					}

					//cout << "info->currentSerializedBuffer_size: " << info->currentSerializedBuffer_size << endl;
					this->threadControlChannel->Read(info->serializedBuffer, 0, info->currentSerializedBuffer_size);
					info->objInstance->DeSerialize(&info->serializedBuffer, &info->currentSerializedBuffer_size);
					//if(info->)
					//	info->objInstance->DeSerialize(&info->serializedBuffer, &info->serializedBuffer_size);

					//exit(0);
					mutex->Unlock();
					return ;
				}
				catch(System::Exception &e)
				{
					if(this->errors != null)
						this->errors->Add(new System::Exception(e.get_Message()));

					mutex->Unlock();
					this->Close();
					return ;
				}
				catch(...)
				{
					if(this->errors != null)
							this->errors->Add(new System::Exception("Unknown exception was occured"));

					mutex->Unlock();
					this->Close();
					return ;
				}
			}
			else
			{
				mutex->Unlock();
				throw ObjectDisposedException("xThreadClient", "The xThreadClient has been closed");
			}
		}
		//----------------------------------------------------
		void xThreadClient::SyncSendOneThread(Int32 index)
		{
			mutex->Lock();

			if(!this->closed)
			{
				if(index < 0)
				{
					mutex->Unlock();
					throw ArgumentOutOfRangeException("index", "index must be greater than or equal to zero");
				}

				if(index > this->collection->get_Count())
				{
					mutex->Unlock();
					throw ArgumentOutOfRangeException("index", "index is out of range");
				}

				xThreadCollection::ThreadInfo *info = this->collection->get_Value(index);
				if(info != null)
				{
					if(info->state == System::Threading::Aborted || info->state == System::Threading::Stopped || info->state == System::Threading::AbortRequested || info->state == System::Threading::SuspendRequested)
					{
						mutex->Unlock();
						return ;
					}
				}
				else
				{
					mutex->Unlock();
					return ;
				}

				try
				{
					if(this->currentBufferSize < 1 + sizeof(Int32) + sizeof(Int32))
					{
						if(this->buffer != null)
							delete this->buffer;
						this->buffer = new char[Parvicursor::xThread::xThreadClient::DefaultBufferSize];
						this->currentBufferSize = Parvicursor::xThread::xThreadClient::DefaultBufferSize;
					}
					buffer[0] = Parvicursor::xThread::SyncSendOneThread;
					buffer++;
					info->objInstance->Serialize(&info->serializedDLLBuffer, &info->serializedBuffer_size);
					if(info->serializedBuffer_size == 0)
					{
						buffer--;
						mutex->Unlock();
						return;
					}
					this->serializer->Reset(this->buffer, this->currentBufferSize - 1);
					this->serializer->Write<Int32>(index);
					this->serializer->Write<Int32>(info->serializedBuffer_size);
					buffer--;
					//this->threadControlChannel->Write(this->buffer, 0, 1 + sizeof(Int32) + sizeof(Int32));


					/****disable this section after xSec implementation****/
					this->threadControlChannel->Write(this->buffer, 0, 1 + sizeof(Int32) + sizeof(Int32));
					this->threadControlChannel->Write(info->serializedDLLBuffer, 0, info->serializedBuffer_size);
					if(this->threadControlChannel->get_IsSecure())
						this->threadControlChannel->Flush();

					/*******for xThread Scattered I/O transfers*****************/
/*#					if defined WIN32 || WIN64

					this->win_Scattered_IO[0].buf = this->buffer;
					this->win_Scattered_IO[0].len = 1 + sizeof(Int32) + sizeof(Int32);

					this->win_Scattered_IO[1].buf = info->serializedDLLBuffer;
					this->win_Scattered_IO[1].len = info->serializedBuffer_size;

					DWORD sent;

					if(WSASend(this->threadControlChannel->get_BaseSocket()->get_Handle(), this->win_Scattered_IO, 2, &sent, 0, null, null) == SOCKET_ERROR)
					{
						int err = WSAGetLastError(); //cout << "hello11" << endl << endl;
						mutex->Unlock();
						throw SocketException(err, ErrorString(err));
					}

					if(sent != 1 + sizeof(Int32) + sizeof(Int32) + info->serializedBuffer_size)
					{
						mutex->Unlock();
						throw System::Exception("The remote endpoint was closed the connection.");
					}


#					else

					this->posix_Scattered_IO[0].iov_base = this->buffer;
					this->posix_Scattered_IO[0].iov_len = 1 + sizeof(Int32) + sizeof(Int32);

					this->posix_Scattered_IO[1].iov_base = info->serializedDLLBuffer;
					this->posix_Scattered_IO[1].iov_len = info->serializedBuffer_size;

					Int32 sent = writev(this->threadControlChannel->get_BaseSocket()->get_Handle(), this->posix_Scattered_IO, 2);

					if(sent == -1)
					{
						mutex->Unlock();
						throw SocketException(errno, (const char *)_sys_errlist[errno]);
					}

					if(sent != 1 + sizeof(Int32) + sizeof(Int32) + info->serializedBuffer_size)
					{
						mutex->Unlock();
						throw Exception("The remote endpoint was closed the connection.");
					}

#					endif*/

					//cout << "sent: " << sent << endl;
					/**********************************************************/

#                   if defined xThread_With_Error_Recovery_Support
					this->threadControlChannel->CheckExceptionResponse(); //
#					endif

					mutex->Unlock();
					return;

				}
				catch(System::Exception &e)
				{
					if(this->errors != null)
						this->errors->Add(new System::Exception(e.get_Message()));

					mutex->Unlock();
					this->Close();
					return ;
				}
				catch(...)
				{
					if(this->errors != null)
							this->errors->Add(new System::Exception("Unknown exception was occured"));

					mutex->Unlock();
					this->Close();
					return ;
				}
			}
			else
			{
				mutex->Unlock();
				throw ObjectDisposedException("xThreadClient", "The xThreadClient has been closed");
			}
		}
		//----------------------------------------------------
		void xThreadClient::AbortOneThread(Int32 index)
		{
			mutex->Lock();

			if(!this->closed)
			{
				if(index < 0)
				{
					mutex->Unlock();
					throw ArgumentOutOfRangeException("index", "index must be greater than or equal to zero");
				}

				if(index > this->collection->get_Count())
				{
					mutex->Unlock();
					throw ArgumentOutOfRangeException("index", "index is out of range");
				}

				xThreadCollection::ThreadInfo *info = this->collection->get_Value(index);
				if(info != null)
				{
					if(info->state == System::Threading::Suspended)
					{
						mutex->Unlock();
						throw ThreadStateException("The thread that is being aborted is currently suspended.");
					}
					if(info->state == System::Threading::Aborted || info->state == System::Threading::AbortRequested)
					{
						mutex->Unlock();
						return;
					}
				}
				else
				{
					mutex->Unlock();
					return ;
				}

				try
				{
					if(this->currentBufferSize < 1 + sizeof(Int32))
					{
						if(this->buffer != null)
							delete this->buffer;
						this->buffer = new char[Parvicursor::xThread::xThreadClient::DefaultBufferSize];
						this->currentBufferSize = Parvicursor::xThread::xThreadClient::DefaultBufferSize;
					}
					buffer[0] = Parvicursor::xThread::AbortOneThread;
					buffer++;
					this->serializer->Reset(this->buffer, this->currentBufferSize - 1);
					this->serializer->Write<Int32>(index);
					buffer--;
					this->threadControlChannel->Write(this->buffer, 0, 1 + sizeof(Int32));
					if(this->threadControlChannel->get_IsSecure())
						this->threadControlChannel->Flush();

#                   if defined xThread_With_Error_Recovery_Support
					this->threadControlChannel->CheckExceptionResponse(); //
#					endif

					info->state = System::Threading::Aborted;

					mutex->Unlock();
					return;
				}
				catch(System::Exception &e)
				{
					if(this->errors != null)
						this->errors->Add(new System::Exception(e.get_Message()));

					mutex->Unlock();
					this->Close();
					return;
				}
				catch(...)
				{
					if(this->errors != null)
							this->errors->Add(new System::Exception("Unknown exception was occurred"));

					mutex->Unlock();
					this->Close();
					return;
				}
			}
			else
			{
				mutex->Unlock();
				throw ObjectDisposedException("xThreadClient", "The xThreadClient has been closed");
			}
		}
		//----------------------------------------------------
		void xThreadClient::AuxiliaryMehtodOneThread(Int32 index, In const void *input, Int32 inputLength, Out void *output, Out Int32 *outputLength)
		{
			if(index < 0)
				throw ArgumentOutOfRangeException("index", "index must be greater than or equal to zero");

			if(inputLength <= 0)
				throw ArgumentOutOfRangeException("inputLength", "inputLength must be greater than zero");

			if(input == null)
				throw ArgumentNullException("input", "input can not be null");

			if(output == null)
				throw ArgumentNullException("output", "output can not be null");

			mutex->Lock();

			if(!this->closed)
			{

				if(index > this->collection->get_Count())
				{
					mutex->Unlock();
					throw ArgumentOutOfRangeException("index", "index is out of range");
				}

				xThreadCollection::ThreadInfo *info = this->collection->get_Value(index);
				if(info == null)
				{
					mutex->Unlock();
					return ;
				}

				Byte xev = (Byte)Parvicursor::xThread::AuxiliaryMehtodOneThread;
				//char *_index = (char *)&index;
				//char *_inputLength = (char *)&inputLength;

				//char *_index = (char *)&index;
				//char *_inputLength = (char *)&inputLength;
				char _index[4];
				Parvicursor_GetBytesFromInt32Number(_index, index);

				char _inputLength[4];
				Parvicursor_GetBytesFromInt32Number(_inputLength, inputLength);

				try
				{
					//printf("xThreadClient::AuxiliaryMehtodOneThread: 1 inputLength: %d\n", inputLength);///
					//printf("xThreadClient::AuxiliaryMehtodOneThread: 1 _inputLength: %d\n", (Int32)(*_inputLength));///
					this->threadControlChannel->Write((char *)&xev, 0, sizeof(Byte));
					this->threadControlChannel->Write(_index, 0, sizeof(Int32));
					this->threadControlChannel->Write(_inputLength, 0, sizeof(Int32));
					this->threadControlChannel->Write((char *)input, 0, inputLength);
					if(this->threadControlChannel->get_IsSecure())
						this->threadControlChannel->Flush();
					//printf("xThreadClient::AuxiliaryMehtodOneThread: 2 inputLength: %d\n", inputLength);///
/*#if defined WIN32 || WIN64

					static WSABUF	scattered_io [4];

					scattered_io[0].buf = (char *)&xev;
					scattered_io[0].len = sizeof(Byte);

					scattered_io[1].buf = _index;
					scattered_io[1].len = sizeof(Int32);

					scattered_io[2].buf = _inputLength;
					scattered_io[2].len = sizeof(Int32);

					scattered_io[3].buf = (char *)input;
					scattered_io[3].len = inputLength;

					DWORD sent;

					if(WSASend(this->threadControlChannel->get_BaseSocket()->get_Handle(), scattered_io, 4, &sent, 0, null, null) == SOCKET_ERROR)
					{
						int err = WSAGetLastError(); //cout << "hello11" << endl << endl;
						mutex->Unlock();
						throw SocketException(err, ErrorString(err));
					}

					if(sent != sizeof(Byte) + sizeof(Int32) + sizeof(Int32) + inputLength)
					{
						mutex->Unlock();
						throw System::Exception("The remote endpoint was closed the connection.");
					}
					//printf("hello1 sent: %d xThreadEvent: %d Int32: %d inputLength: %d\n", sent, sizeof(Byte), sizeof(Int32), inputLength);//

#else
					static iovec scattered_io [4];

					scattered_io[0].iov_base = (char *)&xev;
					scattered_io[0].iov_len = sizeof(Byte);

					scattered_io[1].iov_base = _index;
					scattered_io[1].iov_len = sizeof(Int32);

					scattered_io[2].iov_base = _inputLength;
					scattered_io[2].iov_len = sizeof(Int32);

					scattered_io[3].iov_base = (char *)input;
					scattered_io[3].iov_len = inputLength;

					Int32 sent = writev(this->threadControlChannel->get_BaseSocket()->get_Handle(), scattered_io, 4);

					if(sent == -1)
					{
						mutex->Unlock();
						throw SocketException(errno, (const char *)_sys_errlist[errno]);
					}

					if(sent != sizeof(Byte) + sizeof(Int32) + sizeof(Int32) + inputLength)
					{
						mutex->Unlock();
						throw Exception("The remote endpoint was closed the connection.");
					}
#endif*/


#if defined xThread_With_Error_Recovery_Support
						this->threadControlChannel->CheckExceptionResponse(); //
#endif
					//char *_outputLength = (char *)outputLength;
					Byte _outputLength[4];
					this->threadControlChannel->Read((char *)_outputLength, 0, sizeof(Int32));
					//*outputLength = (Int32)(*_outputLength);
					*outputLength = Parvicursor_GetInt32NumberFromBytes(_outputLength);
					//printf("xThreadClient::AuxiliaryMehtodOneThread: 3 outputLength: %d\n", *outputLength);///
					if(*outputLength <= 0)
						throw ArgumentOutOfRangeException("outputLength", "outputLength must be greater than zero issued by the xThread server.");
					this->threadControlChannel->Read((char *)output, 0, *outputLength);
					//printf("xThreadClient::AuxiliaryMehtodOneThread: 4\n");///

					mutex->Unlock();
					return ;
				}
				catch(System::Exception &e)
				{
					if(this->errors != null)
						this->errors->Add(new System::Exception(e.get_Message()));

					mutex->Unlock();
					this->Close();
					return;
				}
				catch(...)
				{
					if(this->errors != null)
						this->errors->Add(new System::Exception("Unknown exception was occurred"));

					mutex->Unlock();
					this->Close();
					return;
				}
			}
			else
			{
				mutex->Unlock();
				throw ObjectDisposedException("xThreadClient", "The xThreadClient has been closed");
			}
		}
		//----------------------------------------------------
		bool xThreadClient::get_AreAllThreadsCompleted()
		{
			if(this->closed)
				throw ObjectDisposedException();

			return this->_AreAllThreadsCompleted;
		}
		//----------------------------------------------------
		void xThreadClient::Run()
		{
			Thread worker = Thread(this->Wrapper_To_Call_WorkerProc, (void *)this);
			worker.Start();
			//worker->Join();
			this->thread = worker.thread;
			worker.SetDetached();
			return;
		}
		//----------------------------------------------------
		void xThreadClient::WorkerExit()
		{
			Close();
		}
		//----------------------------------------------------
		void xThreadClient::Close()
		{
			mutex->Lock();
			cv->Signal();

			if(!this->closed)
			{
				//pthread_mutex_unlock(&this->mutex);
				//pthread_mutex_lock(&this->mutex);

				if(this->sockets != null)
				{
					for(register Int32 i = 0 ; i < sockets->get_Count() ; i++)
					{
						ParvicursorSocket *s = (ParvicursorSocket *)sockets->get_Value(i);
						if(s->get_BaseSocket()->get_Connected())
						{
							// try {s->get_BaseSocket()->Shutdown(Both);} catch(...) { }
						}
						//s->get_BaseSocket()->Shutdown(Both);
						s->Close();
						delete s;
					}
					this->sockets->Clear();
					delete this->sockets;
					this->sockets = null;
				}

				if(this->socket != null)
				{

					if(this->socket->get_BaseSocket()->get_Connected())
					{
						//try {this->socket->get_BaseSocket()->Shutdown(Both);} catch(...) { }
					}
					//s->get_BaseSocket()->Shutdown(Both);
					this->socket->Close();
					delete this->socket;
					this->socket = null;
				}

				if(this->buffer != null)
				{
					delete this->buffer;
					this->buffer = null;
				}

				if(this->fs != null)
				{
					fs->Close();
					delete this->fs;
					this->fs = null;
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
				/*if(worker != null)
				{
				    ////pthread_exit(null);

                    //pthread_mutex_lock(&this->mutex);
					//worker->Abort2();
					//cout << "exit" << endl;
                    //this->closed = true;
					pthread_cancel(this->worker->thread);
                    delete worker;
					worker = null;cout << "error" << endl;
					//pthread_mutex_unlock(&this->mutex);
				}*/

				this->closed = true;
				//pthread_mutex_unlock(&this->mutex);
				//pthread_mutex_destroy(&this->mutex);
				//pthread_exit(null); // here: consider pthread_cancel() instead of pthread_exit(0
				//pthread_mutex_destroy(&this->mutex);
				//delete this->nc;
			}
			mutex->Unlock();
		}
		//----------------------------------------------------
		void *xThreadClient::Wrapper_To_Call_WorkerProc(void* pt2Object)
		{
			xThreadClient* mySelf = (xThreadClient*) pt2Object;
			// call member
			mySelf->WorkerProc();
			return pt2Object;
		}
		//----------------------------------------------------
		//private void PublicKeyAuthentication(Socket sock);
		//private Byte[] AuthenticationHeaderBuilder(string username, string password);
   		//----------------------------------------------------
   };
};
//**************************************************************************************************************//
