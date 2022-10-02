#include "SessionClientDownloadRequest.h"

//_________________________________
Parvicursor_RESOURCE_PROFILER_INITIALIZE()
//_________________________________

//**************************************************************************************************************//
namespace Parvicursor
{
   namespace xDFS
   {
		//----------------------------------------------------
		SessionClientDownloadRequest::SessionClientDownloadRequest(ParvicursorSocket *socket/*, Thread* worker*/, const FileTransferInfo &transferInfo)
		{
			this->written = 0;
			this->timeout = 120*1000; // 120s timeout
			this->k = 0;  // total reads
			this->secure = false;
			this->memmoryToMemoryTests = false;
			this->info = transferInfo;
			//mutex = PTHREAD_MUTEX_INITIALIZER;
			//pthread_mutex_lock(&_xDFS_server_mutex_);
			//pthread_mutex_unlock(&_xDFS_server_mutex_);
			//this->alRead = null;

			//pthread_mutex_unlock(&mutex);
			sockets = new ArrayList();
			sockets->Add(socket);
			////pthread_mutex_unlock(&mutex);

			if(info.writeFilename.ToLower().Trim().IndexOf("/dev/zero", 0) >= 0) // for memroy-to-memory tests
				memmoryToMemoryTests = true;
			else memmoryToMemoryTests = false;
			memmoryToMemoryTests = memmoryToMemoryTests;
			//Console.WriteLine("memmoryToMemoryTests: "  + memmoryToMemoryTests); //
			this->secure = socket->get_IsSecure();
			buffer = null;
			val1 = null;
			val2 = null;
			//this->worker = worker;
			this->fs = null;
			this->pageSize = 64 * 1024;
            this->seekValue = 0;
			this->readValue = 0;


#if defined WIN32 || WIN64
				this->isNormalZeroCopySupported = false;

				SYSTEM_INFO SysInfo;
				GetSystemInfo(&SysInfo);
				this->pageSize = SysInfo.dwAllocationGranularity;

				this->win_file_handle = null;
				this->win_map_handle = null;
				this->win_map_buffer = null;

				/*if(info.blockSize == this->pageSize)
				{
					this->isNormalZeroCopySupported = false;
					//this->info.isServerSideZeroCopyEnabled = true;
				}
				else
				{
					this->isNormalZeroCopySupported = false;
					this->info.isServerSideZeroCopyEnabled = false;
				}*/
				this->isNormalZeroCopySupported = true; // false for memory-mapped file. true for TransmiteFile.
#else
				this->isNormalZeroCopySupported = true; // false:mmap, true: splice->must to be determine the linux kernel version
				//this->pageSize = getpagesize();
				this->pageSize = (Int32)sysconf(_SC_PAGESIZE);
				if(this->pageSize == -1)
					this->pageSize = getpagesize();
				this->posix_map_buffer = null;

				this->pipes_disk_to_socket[0] = -1;
				this->pipes_disk_to_socket[1] = -1;
				this->pipes_disk_to_socket_is_defined = false;


                struct utsname SysInfo;
				if(uname(&SysInfo) < 0)
                    throw System::Exception((const char *)_sys_errlist[errno]);

#ifdef __linux__

                this->isNormalZeroCopySupported = false;
                //cout << name.release << endl << endl; // 2.6.25.5-1.1-pae
                if(this->info.isServerSideZeroCopyEnabled)
                {
                    char *ss = new char[strlen(SysInfo.release) + 1];
                    strcpy(ss, SysInfo.release);
                    String s(ss);

                    try
                    {
                        int p1, p2, p3;

                        p1 = s.IndexOf(".");
                        if(p1 <= 0)
                            goto Continue;

                        p2 = s.IndexOf(".", p1 + 1);
                        if(p2 <= 0)
                            goto Continue;

                        p3 = s.IndexOf(".", p2 + 1);
                        if(p3 <= 0)
                            goto Continue;

                        int n1, n2, n3;

                        n1 = Convert::ToInt32(s.Substring(0, p1));
                        n2 = Convert::ToInt32(s.Substring(p1 + 1, p2 - p1));
                        n3 = Convert::ToInt32(s.Substring(p2 + 1, p3 - p2));

                        /*if(n1 >= 2 && n2 >= 6 && n3 >= 17) //The splice() system call first appeared in Linux 2.6.17.
                            this->isNormalZeroCopySupported = true;*/

                        if(n1 > 2)
                        {
                            this->isNormalZeroCopySupported = true;
                            goto Continue;
                        }
                        if(n1 < 2)
                        {
                            this->isNormalZeroCopySupported = false;
                            goto Continue;
                        }
                        if(n1 == 2)
                        {
                            if(n2 > 6)
                            {
                                this->isNormalZeroCopySupported = true;
                                goto Continue;
                            }
                            if(n2 < 6)
                            {
                                this->isNormalZeroCopySupported = false;
                                goto Continue;
                            }
                            if(n2 == 6)
                            {
                                if(n3 >= 13)
                                {
                                    this->isNormalZeroCopySupported = true;
                                    goto Continue;
                                }
                                if(n3 < 13)
                                {
                                    this->isNormalZeroCopySupported = false;
                                    goto Continue;
                                }
                            }
                        }

                        //cout << "n1: " << n1 << ", n2: "<< n2 << ", n3: " << n3 << endl << endl;

                    }
                    catch(...){ goto Continue;}

                Continue:
                    //cout << this->isNormalZeroCopySupported << endl;
                    //exit(0);
                    this->isNormalZeroCopySupported = this->isNormalZeroCopySupported;
                }
#else

                this->isNormalZeroCopySupported = false;

#endif


#endif

            this->isNormalZeroCopySupported =  this->isNormalZeroCopySupported;
			//this->alRead = null;
			/*socket.BaseSocket.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.SendBuffer, info.tcpBufferSize);*/
			//socket->get_BaseSocket()->SetReceiveTcpWindowSize(this->info.tcpBufferSize);
			//socket->get_BaseSocket()->SetSendTcpWindowSize(this->info.tcpBufferSize);
			//Console.WriteLine("max {0}", MaxQueueWorkerSize);

		}
		//----------------------------------------------------
		void SessionClientDownloadRequest::AddNewClientStream(ParvicursorSocket *socket)
		{
			//printf("SessionClientDownloadRequest::AddNewClientStream\n");

			//return ;
			//pthread_mutex_lock(&_xDFS_server_mutex_);
			//pthread_mutex_lock(&mutex);
			if(this->sockets != null)
			{
			    try { socket->WriteNoException(); }
                catch(...) {/*pthread_mutex_unlock(&mutex);*/ return;  }
				this->sockets->Add(socket);
			}
			//pthread_mutex_unlock(&mutex);
			//pthread_mutex_unlock(&_xDFS_server_mutex_);

			return ;
		}
		//----------------------------------------------------
		int SessionClientDownloadRequest::WaitForAllConnections()
		{
			//clock_t start, finish;
			int _timeout = 0;
			//start = clock();
			//double  duration;
			while(true)
			{
				//pthread_mutex_lock(&_xDFS_server_mutex_);
				//pthread_mutex_lock(&mutex);
				int count = sockets->get_Count();
				//pthread_mutex_unlock(&_xDFS_server_mutex_);
				//pthread_mutex_unlock(&mutex);
				if(count == info.parallel)
					return 0;
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
		void SessionClientDownloadRequest::Run()
		{
            //pthread_mutex_init(&mutex, null);

		    try { ((ParvicursorSocket *)sockets->get_Value(0))->WriteNoException(); }
            catch(...) {/*pthread_mutex_unlock(&mutex);*/ WorkerExit(); return ;}
			printf("---------------------------------------------------------\n");

			//_________________________________
			Parvicursor_RESOURCE_PROFILER_BEGIN()
			//_________________________________


			printf("New xDFS::xFTSM file transfer session in Download mode.\n");
			if(this->info.parallel > 1)	if(WaitForAllConnections() == -1) { WorkerExit(); return ; }
			if(OpenFileHandle() == -1) { WorkerExit(); return ; }

			try
			{
				//this->info.fileSize = this->fs->get_Length();
				Serializer se(8);
				se.Write<Int64>(this->info.fileSize);

				ParvicursorSocket *_s = (ParvicursorSocket *)sockets->get_Value(0);
				_s->WriteObject(se.get_BaseBuffer(), se.get_BaseBufferSize());
			}
            catch(...) {/*pthread_mutex_unlock(&mutex);*/ WorkerExit(); return ;}

#if defined Enable_TCP_Window_Scale
							//sock->SetReceiveTcpWindowSize(1);//sock->SetReceiveTcpWindowSize(this->tcpBufferSize);//////must delete
			cout << "GetSendTcpWindowSize: " << ((ParvicursorSocket *)sockets->get_Value(0))->get_BaseSocket()->GetSendTcpWindowSize() << " GetReceiveTcpWindowSize(): " << ((ParvicursorSocket *)sockets->get_Value(0))->get_BaseSocket()->GetReceiveTcpWindowSize() << endl << endl;
#endif
			//WorkerExit(); return ;
			//if(!memmoryToMemoryTests) if(OpenFileHandle() == -1) { WorkerExit(); return ; }


#if defined WIN32 || WIN64
			if(this->info.isServerSideZeroCopyEnabled)
			{
				this->win_file_handle = (HANDLE)_get_osfhandle(this->fs->get_Handle());

				if(this->win_file_handle == INVALID_HANDLE_VALUE)
				{
					SendOneExceptionToAllSockets(IOException(ErrorString(GetLastError()))); WorkerExit(); return ;
				}
				if(!this->isNormalZeroCopySupported)
				{
					this->win_map_handle = CreateFileMapping(this->win_file_handle,
									null,
									PAGE_READONLY,
									0,
									0,
									null);

					if(this->win_map_handle == null)
					{
						SendOneExceptionToAllSockets(IOException(ErrorString(GetLastError()))); WorkerExit(); return ;
					}
				}
			}
#endif
			seekValue = 0;
			readValue = 0;
			ParvicursorSocket *sock;
			struct timeval timeout;  // Timeout for select()
			timeout.tv_sec = 0;
			timeout.tv_usec = 0;
			//this->highsock = -1;
			////alRead = new ArrayList();
			//Console.WriteLine("sockets: "  + sockets.Count);//
			//Console.WriteLine("file opened");
			val1 = new char[8];//
			val2 = new char[8];//
			if(!this->info.isServerSideZeroCopyEnabled)
				buffer = new char[info.blockSize];//

			if(info.parallel == 1)
				sock = (ParvicursorSocket *)sockets->get_Value(0);

			/****disable this section after X-DotSec implementation****/
			/*******for X-xDFS Scattered I/O transfers*****/
			this->one_byte_buf_Scattered_IO[0] = (char)1; // signaling the client about arrival of new file block.

#if defined WIN32 || WIN64

			this->win_Scattered_IO[0].buf = this->one_byte_buf_Scattered_IO;
			this->win_Scattered_IO[0].len = 1;

			this->win_Scattered_IO[1].buf = this->val1; //seekValue buffer
			this->win_Scattered_IO[1].len = 8;

			this->win_Scattered_IO[2].buf = this->val2; //readValue buffer
			this->win_Scattered_IO[2].len = 8;

#else

			this->posix_Scattered_IO[0].iov_base = this->one_byte_buf_Scattered_IO;
			this->posix_Scattered_IO[0].iov_len = 1;

			this->posix_Scattered_IO[1].iov_base = this->val1; //seekValue buffer
			this->posix_Scattered_IO[1].iov_len = 8;

			this->posix_Scattered_IO[2].iov_base = this->val2; //readValue buffer
			this->posix_Scattered_IO[2].iov_len = 8;

#endif
			/**********************************************************/

			if(this->info.parallel == 1)
			{
				while(true)
				{
					if(!this->info.isServerSideZeroCopyEnabled)
					{
						try
						{
							ReadFromFile();
						}
						catch(System::Exception &e) { printf("errror2\n"); SendExceptionToOneSocketAndClose(e, sock) ; goto End; }
						catch(...) { printf("errror3\n"); System::Exception e("Unknown excpetion"); SendExceptionToOneSocketAndClose(e, sock) ; goto End; }

						if(this->seekValue == -1)
						{
							/****enable this section after X-DotSec implementation****/
							//socket->WriteByte(0); // signaling the client about finalization of transferring file blocks.
							/****disable this section after X-DotSec implementation****/
							this->one_byte_buf_Scattered_IO[0] = (char)0;
							this->seekValue = 0;
							this->n = 0;
							try
							{
								this->DoScatteredIO(sock);
								goto End;
								//continue;
							}
							catch(ObjectDisposedException &) { RemoveSocketFromArrayList(sock); goto End; }
							catch(SocketException &) { RemoveSocketFromArrayList(sock); goto End; }
							catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, sock) ; goto End; }
							catch(...) { System::Exception e("Unknown excpetion"); SendExceptionToOneSocketAndClose(e, sock) ; goto End; }
						}
						/****enable this section after X-DotSec implementation****/

						/*socket->WriteByte(1);// signaling the server about arrival of new file block.
						GetBytesOfLongNumberForFTSMMode(this->seekValue, this->seekValue);
						GetBytesOfLongNumberForFTSMMode(this->readValue, this->n);

						this->socket->Write(this->seekValue, 0, 8);
						this->socket->Write(this->readValue, 0, 8);
						//printf("seekValue: %ld readValue: %ld\n", this->seekValue, (long)this->n);
						//std::cout << "seekValue: " << this->seekValue << " readValue: " << this->n << std::endl;

						this->socket->Write(this->buffer, 0, (Int32)this->n);*/

						/****disable this section after X-DotSec implementation****/
						/*******for X-xDFS Scattered I/O transfers*****/
						try
						{
							this->DoScatteredIO(sock, this->buffer);//
						}
						catch(ObjectDisposedException &) { RemoveSocketFromArrayList(sock); goto End; }
						catch(SocketException &) { RemoveSocketFromArrayList(sock); goto End; }
						catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, sock) ; goto End; }
						catch(...) { System::Exception e("Unknown excpetion"); SendExceptionToOneSocketAndClose(e, sock) ; goto End; }
						/************************************************/
					}
					else
					{
						try
						{
							this->ZeroCopyTransfer(sock);//
						}
						catch(ObjectDisposedException &) { RemoveSocketFromArrayList(sock); goto End; }
						catch(SocketException &) { RemoveSocketFromArrayList(sock); goto End; }
						catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, sock) ; goto End; }
						catch(...) { System::Exception e("Unknown excpetion"); SendExceptionToOneSocketAndClose(e, sock) ; goto End; }

						if(this->seekValue == -1)
						{
							/****enable this section after X-DotSec implementation****/
							//socket->WriteByte(0); // signaling the client about finalization of transferring file blocks.
							/****disable this section after X-DotSec implementation****/
							this->one_byte_buf_Scattered_IO[0] = (char)0;
							this->seekValue = 0;
							this->n = 0;
							try
							{
								this->DoScatteredIO(sock);
								goto End;
								//continue;
							}
							catch(ObjectDisposedException &) { RemoveSocketFromArrayList(sock); goto End; }
							catch(SocketException &) { RemoveSocketFromArrayList(sock); goto End; }
							catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, sock) ; goto End; }
							catch(...) { System::Exception e("Unknown excpetion"); SendExceptionToOneSocketAndClose(e, sock) ; goto End; }
						}
						/*if(this->seekValue == -1)
						{
							goto End;
						}*/
						//std::cout << "seekValue: " << this->seekValue << " readValue: " << this->n << std::endl;
					}

					written += n;

#if defined With_Error_Recovery_Support
					try
					{
						sock->CheckExceptionResponse();
					}
					catch(ObjectDisposedException &) { RemoveSocketFromArrayList(sock); goto End; }
					catch(SocketException &) { RemoveSocketFromArrayList(sock); goto End; }
					catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, sock) ; goto End; }
					catch(...) { System::Exception e("Unknown excpetion"); SendExceptionToOneSocketAndClose(e, sock) ; goto End; }
					//Thread.Sleep(1);
#endif
				}
			}
			else
			{
				ParvicursorSocket *sock;
				Int32 readysocks;

				struct timeval timeoutRead;  // Timeout for select
				timeoutRead.tv_sec = 0;
				timeoutRead.tv_usec = 0;

				struct timeval timeoutWrite;  // Timeout for select
				timeoutWrite.tv_sec = 0;
				timeoutWrite.tv_usec = 0;

				this->alWrite_enable = false;
				this->alRead_enable = false;

				while(true)
				{
					if(sockets->get_Count() == 0 //|| written >= info.FileSize
					)
					goto End;//break;

#if defined With_Error_Recovery_Support
					Thread::Sleep(1);
#endif
					//clock();
					//if(!this->alWrite_enable)
					MakeReadyWriteSocketArrayList();
					//here:
					//readysocks = ::select(highsock + 1, &alRead, &alWrite, (fd_set *)null, null);
#if defined With_Error_Recovery_Support
					if(this->alWrite_enable)
					{
#endif

#if defined With_Error_Recovery_Support
						readysocks = ::select(highsock + 1, (fd_set *)null, &alWrite, (fd_set *)null, &timeoutWrite);
#else
						readysocks = ::select(highsock + 1, (fd_set *)null, &alWrite, (fd_set *)null, null);
#endif
						if(readysocks == -1)
						{
							//cout << ErrorString(WSAGetLastError()) << endl << endl;
							goto End;
						}
						if(readysocks == 0)
#if defined With_Error_Recovery_Support
							goto ReadableSockets;
#else
							continue;
#endif
						//-----Writable Sockets----------------------------------------
					here1:
						for(register Int32 i = 0 ; i < this->sockets->get_Count() ; i++)
						{
							sock = (ParvicursorSocket *)sockets->get_Value(i);
							if(FD_ISSET(sock->get_BaseSocket()->get_Handle(), &alWrite))
							{
								//this->alWrite_enable = false;
								if(!this->info.isServerSideZeroCopyEnabled)
								{

									try
									{
										ReadFromFile();
									}
									catch(System::Exception &e) { printf("errror2\n"); SendExceptionToOneSocketAndClose(e, sock) ; goto End; }
									catch(...) { printf("errror3\n"); System::Exception e("Unknown excpetion"); SendExceptionToOneSocketAndClose(e, sock) ; goto End; }

									if(seekValue == -1)
									{

										//goto ReadableSockets;
										goto EndTransfer;
									}
									/****enable this section after X-DotSec implementation****/
									/*sock->WriteByte(1);// signaling the server about arrival of new file block.
									GetBytesOfLongNumberForFTSMMode(this->seekValue, this->seekValue);
									GetBytesOfLongNumberForFTSMMode(this->readValue, this->n);

									sock->Write(this->seekValue, 0, 8);
									sock->Write(this->readValue, 0, 8);
									//printf("seekValue: %ld readValue: %ld\n", this->seekValue, (long)this->n);
									//std::cout << "seekValue: " << this->seekValue << " readValue: " << this->n << std::endl;


									sock->Write(this->buffer, 0, (Int32)this->n);*/

									/****disable this section after X-DotSec implementation****/
									try
									{
										this->DoScatteredIO(sock, this->buffer);
									}
									catch(ObjectDisposedException &) { RemoveSocketFromArrayList(sock); goto here1; }
									catch(SocketException &) { RemoveSocketFromArrayList(sock); goto here1; }
									catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, sock) ; goto here1; }
									catch(...) { System::Exception e("Unknown excpetion"); SendExceptionToOneSocketAndClose(e, sock) ; goto here1; }
								}
								else
								{
									//if(seekValue == -1)
									//{
									//	goto EndTransfer;
									//}

									try
									{
										this->ZeroCopyTransfer(sock);
									}
									catch(ObjectDisposedException &) { RemoveSocketFromArrayList(sock); goto here1; }
									catch(SocketException &) { RemoveSocketFromArrayList(sock); goto here1; }
									catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, sock) ; goto here1; }
									catch(...) { System::Exception e("Unknown excpetion"); SendExceptionToOneSocketAndClose(e, sock) ; goto here1; }
									if(seekValue == -1)
									{
										goto EndTransfer;
										//goto ReadableSockets;
									}
								}
#if defined With_Error_Recovery_Support
								sock->set_IsCheckedExceptionResponse(NotDone);
#endif
								written += n;
								//socket->CheckExceptionResponse();
							}
						}
#if defined With_Error_Recovery_Support
					}
					//-----Readable Sockets----------------------------------------
		ReadableSockets:
					//if(readysocks != 0)
					//if(!this->alRead_enable)
                    MakeReadyReadSocketArrayList(); //cout << "hello" << endl << endl;
					//cout << "hello" << endl << endl;
					if(alRead_enable)
					{
						if(seekValue != -1)
							readysocks = ::select(highsock + 1, &alRead, (fd_set *)null, (fd_set *)null, &timeoutRead);
						else
							readysocks = ::select(highsock + 1, &alRead, (fd_set *)null, (fd_set *)null, null);

						if(readysocks == -1)
						{
							//cout << ErrorString(WSAGetLastError()) << endl << endl;
							goto End;
						}
						if(readysocks == 0)
						{
							if(seekValue == -1)
								goto EndTransfer;
							else
								continue;
						}

				here2:
						for(register Int32 j = 0 ; j < this->sockets->get_Count() ; j++)
						{
							sock = (ParvicursorSocket *)sockets->get_Value(j);
							if(FD_ISSET(sock->get_BaseSocket()->get_Handle(), &alRead))
							{
								//this->alRead_enable = false;
								try
								{
									sock->CheckExceptionResponse();
									sock->set_IsCheckedExceptionResponse(Done);
								}
								catch(ObjectDisposedException &) { RemoveSocketFromArrayList(sock); goto here2; }
								catch(SocketException &) { RemoveSocketFromArrayList(sock); goto here2; }
								catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, sock) ; goto here2; }
								catch(...) { System::Exception e("Unknown excpetion"); SendExceptionToOneSocketAndClose(e, sock) ; goto here2; }

								//continue;
							}
						}
					}
#endif

					if(seekValue == -1)
						goto EndTransfer;
					//-------------------------------------------------------------
				}
			}


		//------signaling the client about the end of file transfer session-------------------
	EndTransfer:
		if(this->info.parallel > 1)
		{
			//MakeReadyWriteSocketArrayList();
			//Int32 readysocks = ::select(highsock + 1, (fd_set *)null, &alWrite, (fd_set *)null, null);

			//if(readysocks > 0)
			//{
			here3:
				for(register Int32 i = 0 ; i < this->sockets->get_Count() ; i++)
				{
					ParvicursorSocket *sock = (ParvicursorSocket *)sockets->get_Value(i);
					//if(FD_ISSET(sock->get_BaseSocket()->get_Handle(), &alWrite))
					//{
						//enable this section after X-DotSec implementation/
						//sock->WriteByte(0);// signaling the server about arrival of new file block.

						//disable this section after X-DotSec implementation/
						this->one_byte_buf_Scattered_IO[0] = (char)0;
						this->seekValue = 0;
						this->n = 0;

						try
						{
							this->DoScatteredIO(sock);
						}
						catch(ObjectDisposedException &) { RemoveSocketFromArrayList(sock); goto here3; }
						catch(SocketException &) { RemoveSocketFromArrayList(sock); goto here3; }
						catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, sock) ; goto here3; }
						catch(...) { System::Exception e("Unknown excpetion"); SendExceptionToOneSocketAndClose(e, sock) ; goto here3; }

						//break;
					//}
				//}
			}
		}
		//-----------------------------------------------------------------------------

		End:
			printf("Total Reads: %d\n", k);
			WorkerExit();
			return ;
		}
		//----------------------------------------------------
		void SessionClientDownloadRequest::ReadFromFile()
		{
			this->n = this->fs->Read(this->buffer, 0, this->info.blockSize);
			if(n <= 0)
			{
				this->seekValue = -1;
				return ;
			}
			this->k++;
			this->seekValue = this->fs->get_Position() - this->n;
		}
		//----------------------------------------------------
		void SessionClientDownloadRequest::ZeroCopyTransfer(ParvicursorSocket *socket)
		{

			Int64 reminding = this->info.fileSize - this->written;

			if(reminding <=0 )
			{
				this->seekValue = -1;
				return;
			}
			if(reminding > this->info.blockSize)
			{
				this->n = this->info.blockSize;
			}
			else
			{
				this->n = reminding;
				//this->offsetSeek = this->n - reminding;
			}

			/****enable this section after X-DotSec implementation****/
			/*socket->WriteByte(1);// signaling the server about arrival of new file block.
			GetBytesOfLongNumberForFTSMMode(this->seekValue, this->offsetSeek);
			GetBytesOfLongNumberForFTSMMode(this->readValue, this->n);

			socket->Write(this->seekValue, 0, 8);
			socket->Write(this->readValue, 0, 8);*/

			this->k++; // total reads

#if defined WIN32 || WIN64

			if(this->isNormalZeroCopySupported) // based on TransmiteFile Win32/64 API function.
			{

				/****disable this section after X-DotSec implementation****/
				this->DoScatteredIO(socket);

				this->fs->Seek(this->seekValue, Begin);

				if(!TransmitFile(socket->get_BaseSocket()->get_Handle(),
										this->win_file_handle,
										(DWORD)this->n,
										(DWORD)this->n,
										null,
										null,
										TF_WRITE_BEHIND))

				{
					throw SocketException(WSAGetLastError(), ErrorString(WSAGetLastError()));
					//cout << "error: hi " << ErrorString(WSAGetLastError()) << endl;
				}

				/*if(this->n != reminding)
				{
					//cout << this->last_n << endl;
					//this->fs->Seek(this->last_n, Current);
				}*/

			}
			else // based on File-based memory mapping by using CreateFileMapping Win32/64 API function.
			{
				//this->fs->Seek(this->offsetSeek, Begin);
				Int64 chunkOffset = this->seekValue % this->pageSize;
				Int64 chunkStart = this->seekValue - chunkOffset;
				this->win_map_buffer = (char *)MapViewOfFile(win_map_handle,
															FILE_MAP_READ,
															(Int32)((chunkStart & 0xFFFFFFFF00000000LL) > 32),
															(Int32)(chunkStart & 0x00000000FFFFFFFFLL),
															(Int32)(this->n + chunkOffset));
				if(this->win_map_buffer == null)
				{
					//cout << "MapViewOfFile1: " << ErrorString(GetLastError()) << endl << endl; //
					int err = GetLastError();
					throw IOException(ErrorString(err));
				}
				/*if(chunkOffset != 0)
					this->win_map_buffer += chunkOffset;*/

				char *temp = this->win_map_buffer;

				/*if(chunkOffset != 0)
					this->temp += chunkOffset;*/

				/****enable this section after X-DotSec implementation****/
				//socket->Write(this->win_map_buffer, 0, (Int32)this->n);
				/****disable this section after X-DotSec implementation****/
				this->DoScatteredIO(socket, temp);

				/*if(chunkOffset != 0)
					this->win_map_buffer -= chunkOffset;*/

				if(!UnmapViewOfFile(this->win_map_buffer))
				{
					int err = GetLastError();
					throw IOException(ErrorString(err));
				}

				this->win_map_buffer = null;
			}
#else
			if(this->isNormalZeroCopySupported) // based on POSIX Splice/sendfile API functions.
			{
                //cout << "hellooooooooooooooooooooo" << endl << endl;
                this->DoScatteredIO(socket);

                if(TransferFromDiskToSocket_ZeroCopySplice(this->fs->get_Handle(), null, socket->get_BaseSocket()->get_Handle(), (Int32)this->n, this->pipes_disk_to_socket, this->pageSize, this->pipes_disk_to_socket_is_defined, false) < 0)
                    throw IOException((const char *)_sys_errlist[errno]);

			}
			else // based on File-based memory mapping by using mmap/munmap POSIX API functions.
			{
				Int64 chunkOffset = this->seekValue % this->pageSize;
				Int64 chunkStart = this->seekValue - chunkOffset;

				this->posix_map_buffer = (char *)mmap64(null, (Int32)(this->n + chunkOffset), PROT_READ, MAP_PRIVATE, this->fs->get_Handle(), chunkStart);

                if(this->posix_map_buffer == MAP_FAILED)
                    throw IOException((const char *)_sys_errlist[errno]);

                /****enable this section after X-DotSec implementation****/
				//socket->Write(this->posix_map_buffer, 0, (Int32)this->n);
				/****disable this section after X-DotSec implementation****/
				this->DoScatteredIO(socket, this->posix_map_buffer);

				if(munmap(this->posix_map_buffer, (Int32)(this->n + chunkOffset)) == -1)
                    throw IOException((const char *)_sys_errlist[errno]);

                //this->posix_map_buffer = null;
			}
#endif

			this->last_n = this->n;

			//cout << "this->offsetSeek: " << this->offsetSeek << " this->n: " << this->n << endl << endl;
			this->seekValue = this->written + this->n;
			//cout << "this->offsetSeek: " << this->offsetSeek << " this->n: " << this->n << endl << endl;
		}
		//----------------------------------------------------
		/*int SessionClientDownloadRequest::ReadFromOriginialSocket(ParvicursorSocket *socket, char array[], int offset, int count)
		{
			return 0;
		}*/
		//----------------------------------------------------
		void SessionClientDownloadRequest::DoScatteredIO(ParvicursorSocket *socket, char *buffer)
		{
			GetBytesOfLongNumberForFTSMMode(this->val1, this->seekValue);
			GetBytesOfLongNumberForFTSMMode(this->val2, this->n);

#if defined WIN32 || WIN64

			this->win_Scattered_IO[3].buf = buffer;
			this->win_Scattered_IO[3].len = (UInt32)this->n;

			DWORD sent;

			if(WSASend(socket->get_BaseSocket()->get_Handle(), this->win_Scattered_IO, 4, &sent, 0, null, null) == SOCKET_ERROR)
			{
				int err = WSAGetLastError(); //cout << "hello11" << endl << endl;
				throw SocketException(err, ErrorString(err));
			}

			if(sent != 1 + 8 + 8 + (DWORD)this->n)
				throw SocketException(WSAECONNRESET, "The remote endpoint was closed the connection.");

#else

			this->posix_Scattered_IO[3].iov_base = buffer;
			this->posix_Scattered_IO[3].iov_len = (UInt32)this->n;


            Int32 sent = writev(socket->get_BaseSocket()->get_Handle(), this->posix_Scattered_IO, 4);

			if(sent == -1)
                throw SocketException(errno, (const char *)_sys_errlist[errno]);

			if(sent != 1 + 8 + 8 + (Int32)this->n)
				throw SocketException(ECONNRESET, "The remote endpoint was closed the connection.");

#endif
		}
		//----------------------------------------------------
		void SessionClientDownloadRequest::DoScatteredIO(ParvicursorSocket *socket)
		{

			GetBytesOfLongNumberForFTSMMode(this->val1, this->seekValue);
			GetBytesOfLongNumberForFTSMMode(this->val2, this->n);

#if defined WIN32 || WIN64


			DWORD sent;

			if(WSASend(socket->get_BaseSocket()->get_Handle(), this->win_Scattered_IO, 3, &sent, 0, null, null) == SOCKET_ERROR)
			{
				int err = WSAGetLastError(); //cout << "hello12" << endl << endl;
				throw SocketException(err, ErrorString(err));
			}

			if(sent != 1 + 8 + 8)
				throw SocketException(WSAECONNRESET, "The remote endpoint was closed the connection.");

#else


			Int32 sent = writev(socket->get_BaseSocket()->get_Handle(), this->posix_Scattered_IO, 3);

			if(sent == -1)
				throw SocketException(errno, (const char *)_sys_errlist[errno]);

			if(sent != 1 + 8 + 8)
				throw SocketException(ECONNRESET, "The remote endpoint was closed the connection.");

#endif
		}
		//----------------------------------------------------
		void SessionClientDownloadRequest::WriteNoException(ParvicursorSocket *socket)
		{
			return ;
		}
		//----------------------------------------------------
		void SessionClientDownloadRequest::SendOneExceptionToAllSockets(System::Exception &e)
		{
			ParvicursorSocket *s;

			for(register Int32 i = 0 ; i < this->sockets->get_Count() ; i++)
			{
				s = (ParvicursorSocket *)this->sockets->get_Value(i);
				try { s->WriteException(e); }
				catch(...) { }

			}
			return ;
		}
		//----------------------------------------------------
		void SessionClientDownloadRequest::SendExceptionToOneSocketAndClose(System::Exception &e, ParvicursorSocket *socket)
		{
			try { socket->WriteException(e); RemoveSocketFromArrayList(socket); }
			catch(...) { RemoveSocketFromArrayList(socket); }
		}
		//----------------------------------------------------
		void SessionClientDownloadRequest::MakeReadyReadSocketArrayList()
		{
			/*if(this->alRead_enable && this->alWrite_enable)
			{
				cout << "fff" << endl <<endl;
				return;
			}*/
			/* FD_ZERO() clears out the fd_set called socks, so that
				it doesn't contain any file descriptors. */
			FD_ZERO(&alRead);
			//FD_ZERO(&alWrite);
			ParvicursorSocket *s;
			Int32 handle;
			/* Loops through all the possible connections and adds
				those sockets to the fd_set */

			this->alRead_enable = false;

			for (Int32 i = 0; i < this->sockets->get_Count() ; i++)
			{
				s = (ParvicursorSocket *)sockets->get_Value(i);
				handle = s->get_BaseSocket()->get_Handle();
				if(handle != 0)
				{
					if(s->get_IsCheckedExceptionResponse() == NotDone)
					{
						FD_SET(handle, &alRead);
						this->alRead_enable = true;
					}

					if (handle > highsock)
						highsock = handle;
				}
			}

			return ;
		}
		//----------------------------------------------------
		void SessionClientDownloadRequest::MakeReadyWriteSocketArrayList()
		{
			/*if(this->alRead_enable && this->alWrite_enable)
			{
				cout << "fff" << endl <<endl;
				return;
			}*/
			/* FD_ZERO() clears out the fd_set called socks, so that
				it doesn't contain any file descriptors. */
			//FD_ZERO(&alRead);
			FD_ZERO(&alWrite);
			ParvicursorSocket *s;
			Int32 handle;
			/* Loops through all the possible connections and adds
				those sockets to the fd_set */

#if defined With_Error_Recovery_Support
			this->alWrite_enable = false;
#endif
			for (Int32 i = 0; i < this->sockets->get_Count() ; i++)
			{
				s = (ParvicursorSocket *)sockets->get_Value(i);
				handle = s->get_BaseSocket()->get_Handle();
				if(handle != 0)
				{
#if defined With_Error_Recovery_Support
					if(s->get_IsCheckedExceptionResponse() == Done || s->get_IsCheckedExceptionResponse() == FirstTime)
					{
#endif
						FD_SET(handle, &alWrite);
#if defined With_Error_Recovery_Support
						this->alWrite_enable = true;
					}
#endif

					if (handle > highsock)
						highsock = handle;
				}
			}

			return ;
		}
		//----------------------------------------------------
		void SessionClientDownloadRequest::RemoveSocketFromArrayList(ParvicursorSocket *socket)
		{
			int i = FindSocketIndex(socket);
			if(i != -1)
			{
				ParvicursorSocket *s = (ParvicursorSocket *)sockets->get_Value(i);
				//try{s->get_BaseSocket()->Shutdown(Both);} catch(...) { }

				try{ s->Close(); } catch(...) { }

				sockets->RemoveAt(i);
				delete s;
			}
		}
		//----------------------------------------------------
		int SessionClientDownloadRequest::OpenFileHandle()
		{
			//Console.WriteLine("OpenFileHandle ini" + memmoryToMemoryTests);
			try
			{
				int p = info.writeFilename.LastIndexOf("/");
				if(p < 0)
					p = info.writeFilename.LastIndexOf("\\");
				/*if(p > 0)
				{
					String directory = info.writeFilename.Substring(0, p);
					if(!Directory::Exists(directory))
						Directory::CreateDirectory(directory);
				}*/

				fs = new FileStream(info.writeFilename, Open, Read, 9);

				if(this->memmoryToMemoryTests)
					this->info.fileSize = 0x0FFFFFFFFFFFFFFFLL;
				else
					this->info.fileSize = fs->get_Length();

				//fs->Seek(0, Begin);
				//fs->SetLength(0);//printf("file opened\n");//
				//fs.Seek(0, SeekOrigin.Begin);
				/*if(!memmoryToMemoryTests)
				{
					if(fs.Length != info.FileSize)
					{
						fs.SetLength(info.FileSize);
						fs.Seek(0, SeekOrigin.Begin);
					}
				}*/
				//Console.WriteLine("OpenFileHandle end" + memmoryToMemoryTests);
				/*fs.SetLength(0);
				fs.Seek(0, SeekOrigin.Begin);
				fs.SetLength(info.FileSize);
				fs.Seek(0, SeekOrigin.Begin);*/
				return 0;
			}
			catch(System::Exception &e)
			{
				for(register Int32 i = 0 ; i < sockets->get_Count() ; i++)
				{
					try { ((ParvicursorSocket *)sockets->get_Value(i))->WriteException(e); }
					catch(...) { }
				}
				return -1;
			}
			catch(...)
			{
			    System::Exception e("Unknown excpetion");
				for(register Int32 i = 0 ; i < sockets->get_Count() ; i++)
				{
					try { ((ParvicursorSocket *)sockets->get_Value(i))->WriteException(e); }
					catch(...) { }
				}
				return -1;
			}
		}
		//----------------------------------------------------
		int SessionClientDownloadRequest::FindSocketIndex(ParvicursorSocket *socket)
		{
			for(register Int32 i = 0 ; i < sockets->get_Count() ; i++)
				if((ParvicursorSocket *)sockets->get_Value(i) == socket)
					return i;
			return -1;
		}
		//----------------------------------------------------
		void SessionClientDownloadRequest::WorkerExit()
		{
			//pthread_mutex_lock(&mutex);
			if(sockets != null)
			{
				if(sockets->get_Count() != 0)
				{
					for(register Int32 i = 0 ; i < sockets->get_Count() ; i++)
					{
						ParvicursorSocket *s = (ParvicursorSocket *)sockets->get_Value(i);
						//s->get_BaseSocket()->Shutdown(Both);
                        //s->Close();
                        if(s != null)
                        {
                            //try {s->get_BaseSocket()->Shutdown(Both);} catch(...) { }
                            try {s->Close();} catch(...) { }
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

			/********   X-xDFS Zero-Copy Cleanup  *********************/
#if defined WIN32 || WIN64
            if(this->win_map_buffer != null)
                UnmapViewOfFile(this->win_map_buffer);

            if(this->win_map_handle != null)
                CloseHandle(this->win_map_handle);

            this->win_map_buffer = null;
            this->win_map_handle = null;
#else
            /*if(this->posix_map_buffer != null)
                munmap(this->posix_map_buffer, (Int32)this->n);*/

            if(this->isNormalZeroCopySupported && this->info.isServerSideZeroCopyEnabled)
                TransferFromSocketToDisk_ZeroCopySplice(-1, null, -1, -1, this->pipes_disk_to_socket, -1, this->pipes_disk_to_socket_is_defined, true);

            this->posix_map_buffer = null;

#endif
			/****************************************************************/

			if(fs != null)
			{
				fs->Flush();
				fs->Close();
				delete fs;
			}

			if(val1 != null)
				delete val1;

			if(val2 != null)
				delete val2;

			if(buffer != null)
				delete buffer;
			//pthread_mutex_unlock(&mutex);
			//pthread_mutex_destroy(&mutex);
			//pthread_mutex_unlock(&_xDFS_server_mutex_);
			/*}
			catch(...) { }*/

			//_________________________________
			Parvicursor_RESOURCE_PROFILER_END()
			Parvicursor_RESOURCE_PROFILER_FINALIZE()
			//_________________________________


			return ;
		}
   		//----------------------------------------------------
   };
};
//**************************************************************************************************************//
