#include "SessionClientUploadRequest.h"

//_________________________________
Parvicursor_RESOURCE_PROFILER_INITIALIZE()
//_________________________________

//**************************************************************************************************************//
namespace Parvicursor
{
   namespace xDFS
   {
		//----------------------------------------------------
		SessionClientUploadRequest::SessionClientUploadRequest(ParvicursorSocket *socket/*, Thread* worker*/, const FileTransferInfo &transferInfo)
		{
			this->written = 0;
			this->timeout = 120*1000; // 120s timeout
			this->lastOffset = 0;
			this->lastLength = 0;
			this->j = 0;  // seek numbers
			this->k = 0;  // total writes
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

			if(info.writeFilename.ToLower().Trim().IndexOf("/dev/null", 0) >= 0) // for memroy-to-memory tests
				memmoryToMemoryTests = true;
			else
				memmoryToMemoryTests = false;
			//memmoryToMemoryTests = memmoryToMemoryTests;
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
#else
				this->isNormalZeroCopySupported = true; // false:mmap, true: splice->must to be determine the linux kernel version
				//this->pageSize = getpagesize();
				this->pageSize = (Int32)sysconf(_SC_PAGESIZE);
				if(this->pageSize == -1)
					this->pageSize = getpagesize();
				this->posix_map_buffer = null;

				this->pipes_socket_to_disk[0] = -1;
				this->pipes_socket_to_disk[1] = -1;
				this->pipes_socket_to_disk_is_defined = false;


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
		void SessionClientUploadRequest::AddNewClientStream(ParvicursorSocket *socket)
		{
			//printf("SessionClientUploadRequest::AddNewClientStream\n");

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
		int SessionClientUploadRequest::WaitForAllConnections()
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
		void SessionClientUploadRequest::Run()
		{
            //pthread_mutex_init(&mutex, null);

		    try { ((ParvicursorSocket *)sockets->get_Value(0))->WriteNoException(); }
            catch(...) {/*pthread_mutex_unlock(&mutex);*/ WorkerExit(); return ;}
			printf("---------------------------------------------------------\n");

			//_________________________________
			Parvicursor_RESOURCE_PROFILER_BEGIN()
			//_________________________________


			printf("New xDFS::xFTSM file transfer session in Upload mode.\n");
			if(this->info.parallel > 1)	if(WaitForAllConnections() == -1) { WorkerExit(); return ; }
			if(OpenFileHandle() == -1) { WorkerExit(); return ; }


			//cout << circularBufferCountClientSide << endl;
			/************************************************/

#if defined Enable_TCP_Window_Scale
							//sock->SetReceiveTcpWindowSize(1);//sock->SetReceiveTcpWindowSize(this->tcpBufferSize);//////must delete
			cout << "GetSendTcpWindowSize: " << ((ParvicursorSocket *)sockets->get_Value(0))->get_BaseSocket()->GetSendTcpWindowSize() << " GetReceiveTcpWindowSize(): " << ((ParvicursorSocket *)sockets->get_Value(0))->get_BaseSocket()->GetReceiveTcpWindowSize() << endl << endl;
#endif
			//WorkerExit(); return ;
			//if(!memmoryToMemoryTests) if(OpenFileHandle() == -1) { WorkerExit(); return ; }


#if defined WIN32 || WIN64
			if(this->info.isServerSideZeroCopyEnabled && !this->isNormalZeroCopySupported)
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
									PAGE_READWRITE,
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
			//cout << "hellllllllllllo" << endl;
			seekValue = 0;
			readValue = 0;
			ParvicursorSocket *sock = null;
			int readsocks;
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


			/********** disk thread settings ****************/
			this->disk_thread = null;
			this->circular_buffer = null;
			if(this->info.circularBufferCount > 0)
				this->circularBufferEnabled = true;
			else
			{
				this->circularBufferEnabled = false;
				this->info.circularBufferCount = -1;
			}

			if(this->info.isServerSideZeroCopyEnabled || this->memmoryToMemoryTests)
				this->circularBufferEnabled = false;


			if(this->circularBufferEnabled)
			{
				pthread_mutex_init(&this->mutex, NULL);
				this->blocks = 0;
				this->start = 0;
				this->end = 0;
				//this->_n = 0;
				//this->__n = 0;
				//this->_last_n = 0;
				//this->currentOffset = 0;
				this->nextOffset = 0;
				//this->_offset = 0;
				this->BUFFER_SIZE = (this->info.blockSize + this->xFTSM_HeaderLength) * this->info.circularBufferCount;
				this->circular_buffer = new char[this->BUFFER_SIZE];

				this->disk_thread = new Thread(this->Wrapper_To_Call_DiskThreadWorkerProc, (void *)this);
				this->disk_thread->Start();
				this->disk_thread->SetDetached();
				//Thread::Sleep(10);

				//cout << "this->info.circularBufferCount:" << this->info.circularBufferCount << endl;
			}
			/************************************************/

			Int32 rett;

			/****disable this section after X-DotSec implementation****/
			/*******for X-xDFS Scattered I/O transfers*****/
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
			if(info.parallel == 1)
			{
				while(true)
				{
//Continue_From_CircularBuffer_one_connection:
					//sock = (ParvicursorSocket *)sockets->get_Value(0);
					if(!this->info.isServerSideZeroCopyEnabled)
					{
						try
						{
							if(ReadFileBlockFTSMMode(sock))
							{
								//sock->WriteNoException();
								RemoveSocketFromArrayList(sock);
								break;
							}
						}
						catch(ObjectDisposedException &) { printf("errror1\n"); WorkerExit(); return;}
						catch(SocketException &) { RemoveSocketFromArrayList(sock); continue; }
						catch(System::Exception &e) { printf("errror2\n"); SendExceptionToOneSocketAndClose(e, sock) ; goto End; }
						//catch(...) { printf("errror3\n"); SendExceptionToOneSocketAndClose(Exception("Unknown excpetion"), sock) ; goto End; }
						if(!this->circularBufferEnabled)
						{
							/*try { WriteToFile(); }
							catch(Exception &e) { printf("errror4\n"); SendExceptionToOneSocketAndClose(e, sock) ; goto End; }*/

							try { WriteToFile(); }
							catch(System::Exception &e) { printf("errror4\n"); SendExceptionToOneSocketAndClose(e, sock) ; goto End; }
							catch(...) { System::Exception e("Unknown excpetion"); SendExceptionToOneSocketAndClose(e, sock) ; goto End; }
						}
						else
						{
							bool breakflag = false;
							while(true && !(breakflag))
							{
								rett = buffer_device();

								switch(rett)
								{
									case Parvicursor::Shared::Enums::xDFS::BufferOperationCompleted:
									{
										//goto Continue_From_CircularBuffer_one_connection;
										breakflag = true;
										break;
									}
									case Parvicursor::Shared::Enums::xDFS::BufferIsFull:
									{
										Thread::Sleep(1);
										break;
									}
									/*case Parvicursor::Shared::Enums::xDFS::BufferIsEmpty:
									{
										goto Continue_From_CircularBuffer;
									}
									case Parvicursor::Shared::Enums::xDFS::EndOfFileButBufferNotEmpty:
									{
										//printf("Parvicursor::Shared::Enums::xDFS::EndOfFileButBufferNotEmpty:Consumer\n");
										goto End;
									}
									case Parvicursor::Shared::Enums::xDFS::EndOfFileAndBufferIsEmpty:
									{
										//printf("Parvicursor::Shared::Enums::xDFS::EndOfFileAndBufferIsEmpty:Consumer:ThreadExit\n");
										goto End;
									}*/
									default:
									{
										break;
									}
								}
							}
						}
					}
					else
					{
						try
						{
							if(ZeroCopyTransfer(sock))
							{
								RemoveSocketFromArrayList(sock);
								goto End;
							}
						}
						catch(ObjectDisposedException &) { RemoveSocketFromArrayList(sock); goto End; }
						catch(SocketException &) { RemoveSocketFromArrayList(sock); goto End; }
						catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, sock) ; goto End; }
						catch(...) { System::Exception e("Unknown excpetion"); SendExceptionToOneSocketAndClose(e, sock) ; goto End; }
					}
					//catch(...) { printf("errror5\n"); SendExceptionToOneSocketAndClose(Exception("Unknown excpetion"), sock) ; goto End; }
					//WriteNoException(sock);

#if defined With_Error_Recovery_Support
						try { sock->WriteNoException(); }
						catch(...) { printf("errror6\n"); WorkerExit(); return; }
#endif
				}
			}
			else
			{
				while(true)
				{
					//here:
					if(sockets->get_Count() == 0 //|| written >= info.FileSize
					)
					break;
//				here:
					MakeReadSocketArrayList();
				//here:
					readsocks = ::select(highsock + 1, &alRead, (fd_set *)null, (fd_set *)null, null);
					if(readsocks == -1)
						goto End;

					//if (readsocks == 0)
					//{
						//goto here;
						////continue;
					//}

					//cout << "Count: "<< this->sockets->get_Count() << " readsocks:" << readsocks << endl;
					//goto End;
				here1:
					for(register Int32 i = 0 ; i < this->sockets->get_Count() ; i++)
					{
						ParvicursorSocket *sock = (ParvicursorSocket *)sockets->get_Value(i);
						//int handle = sock->get_BaseSocket()->get_Handle();
						if(FD_ISSET(sock->get_BaseSocket()->get_Handle(), &alRead))
						{
							if(!this->info.isServerSideZeroCopyEnabled)
							{
								try
								{
									if(ReadFileBlockFTSMMode(sock))
									{
										RemoveSocketFromArrayList(sock);
										continue;
									}
								}
								catch(ObjectDisposedException &) { RemoveSocketFromArrayList(sock); goto here1; }
								catch(SocketException &) { RemoveSocketFromArrayList(sock); goto here1; }
								catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, sock) ; goto here1; }
								catch(...) { System::Exception e("Unknown excpetion"); SendExceptionToOneSocketAndClose(e, sock) ; goto here1; }

								if(!this->circularBufferEnabled)
								{
									try { WriteToFile(); }
									catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, sock) ; goto here1; }
									catch(...) { System::Exception e("Unknown excpetion"); SendExceptionToOneSocketAndClose(e, sock) ; goto here1; }
								}
								else
								{
									bool breakflag = false;
									while(true && !(breakflag))
									{
										rett = buffer_device();

										switch(rett)
										{
											case Parvicursor::Shared::Enums::xDFS::BufferOperationCompleted:
											{
												//goto Continue_From_CircularBuffer_parallel_connections;
												breakflag = true;
												break;
											}
											case Parvicursor::Shared::Enums::xDFS::BufferIsFull:
											{
												Thread::Sleep(1);
												break;
											}
											/*case Parvicursor::Shared::Enums::xDFS::BufferIsEmpty:
											{
												goto Continue_From_CircularBuffer;
											}
											case Parvicursor::Shared::Enums::xDFS::EndOfFileButBufferNotEmpty:
											{
												//printf("Parvicursor::Shared::Enums::xDFS::EndOfFileButBufferNotEmpty:Consumer\n");
												goto End;
											}
											case Parvicursor::Shared::Enums::xDFS::EndOfFileAndBufferIsEmpty:
											{
												//printf("Parvicursor::Shared::Enums::xDFS::EndOfFileAndBufferIsEmpty:Consumer:ThreadExit\n");
												goto End;
											}*/
											default:
											{
												break;
											}
										}
									}
								}
							}
							else
							{
								try
								{
									if(ZeroCopyTransfer(sock))
									{
										RemoveSocketFromArrayList(sock);
										continue;
									}
								}
								catch(ObjectDisposedException &) { RemoveSocketFromArrayList(sock); goto here1; }
								catch(SocketException &) { RemoveSocketFromArrayList(sock); goto here1; }
								catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, sock) ; goto here1; }
								catch(...) { System::Exception e("Unknown excpetion"); SendExceptionToOneSocketAndClose(e, sock) ; goto here1; }
							}

#if defined With_Error_Recovery_Support
							try { sock->WriteNoException(); }
							catch(...) { RemoveSocketFromArrayList(sock); goto here1;  }
#endif
						}
					}
					//Thread::Sleep(_timeout);
				}
			}

		End:

			if(this->circularBufferEnabled)
			{
				this->nextOffset = -1;

				while(this->blocks != 0)
					Thread::Sleep(1);
			}

			printf("Seek Number: %d, Total Writes: %d\n", j , k);
			WorkerExit();
			return ;
		}
		//----------------------------------------------------
		void SessionClientUploadRequest::WriteToFile()
		{
			//Console.WriteLine("offset: {0}, length: {1}", (int)seekValue, (int)readValue);
			k++;
			/*if(memmoryToMemoryTests)
				return;*/////
			if(this->seekValue != this->lastOffset + this->lastLength)
			{
				fs->Seek(this->seekValue, Begin);
				//fs.Write(buffer, 0, (int)readValue);
				j++;
			}
			/*else
			{
				fs.Write(buffer, 0, (int)readValue);
				//for(int i = 0 ; i < (int) readValue; i++)
				//	Console.Write(buffer[i]);
			}*/
			//fs.Seek(seekValue, SeekOrigin.Begin);
			fs->Write(buffer, 0, (Int32)this->readValue);
			this->lastOffset = this->seekValue;
			this->lastLength = this->readValue;
			this->written += this->readValue;
			//Console.WriteLine("offset: {0}, seekNumber: {1}, written: {2}", seekValue , j, written);
		}
		//----------------------------------------------------
		bool SessionClientUploadRequest::ZeroCopyTransfer(ParvicursorSocket *socket)
		{
			/****enable this section after X-DotSec implementation****/
			//n = socket->ReadByte();

			/****disable this section after X-DotSec implementation****/
			/*******for X-xDFS Scattered I/O transfers*****/
			DoScatteredIO(socket);
			/**********************************************************/
			if(this->n == 0)
			{
				return true; // meaning end read.
			}
			//if(n == -1)
			//	throw ObjectDisposedException("The remote endpoint closed the connection");*/

			/****enable this section after X-DotSec implementation****/
			/*if(socket->Read(val1, 0, 8) != 8)
				throw ArgumentException("Bad format for FileTransferModeHeader.");
			if(socket->Read(val2, 0, 8) != 8)
				throw ArgumentException("Bad format for FileTransferModeHeader.");
			this->seekValue = GetLongNumberFromBytesForFTSMMode((byte *)this->val1);
			this->readValue = GetLongNumberFromBytesForFTSMMode((byte *)this->val2);*/
			//std::cout << "seekValue: " << seekValue << " readValue: " << readValue << std::endl;

			if(this->readValue > info.blockSize)
				throw ArgumentOutOfRangeException("readValue in the header can not be greater than blockSize.");

			k++;
			/*if(memmoryToMemoryTests)
				return;*/////

			//fs->Seek(seekValue, Begin);

#if defined WIN32 || WIN64

			if(this->seekValue != this->lastOffset + this->lastLength)
				j++;

			if(!this->isNormalZeroCopySupported)
			{
				Int64 chunkOffset = this->seekValue % this->pageSize;
				Int64 chunkStart = this->seekValue - chunkOffset;

				//fs->Seek(this->seekValue, Begin);

				this->win_map_buffer = (char *)MapViewOfFile(win_map_handle,
															FILE_MAP_WRITE,
															(Int32)((chunkStart & 0xFFFFFFFF00000000LL) > 32),
															(Int32)(chunkStart & 0x00000000FFFFFFFFLL),
															(Int32)(this->readValue + chunkOffset));

				if(this->win_map_buffer == null)
					throw IOException(ErrorString(GetLastError()));

				char *temp = this->win_map_buffer;

				/*if(chunkOffset != 0)
					temp += chunkOffset;*/

				n = socket->Read(temp, (Int32)chunkOffset, (Int32)this->readValue);

				/*if(chunkOffset != 0)
					temp -= chunkOffset;*/

				/*if(!FlushViewOfFile(this->win_map_buffer, (Int32)this->readValue))
					throw IOException(ErrorString(GetLastError()));*/

				/*if(chunkOffset != 0)
					this->win_map_buffer -= chunkOffset;*/

				if(!UnmapViewOfFile(this->win_map_buffer))
					throw IOException(ErrorString(GetLastError()));

				this->lastOffset = this->seekValue;
				this->lastLength = this->readValue;
				this->written += this->readValue;

                this->win_map_buffer = null;

				if(n != readValue)
					throw ArgumentException("Bad format for FileTransferModeHeader.");
			}

#else
			if(!this->isNormalZeroCopySupported)
			{
                if(this->seekValue != this->lastOffset + this->lastLength)
                {
	                //fs->Seek(this->seekValue, Begin);
                    //fs.Write(buffer, 0, (int)readValue);
                    j++;
                }

				Int64 chunkOffset = this->seekValue % this->pageSize;
				Int64 chunkStart = this->seekValue - chunkOffset;

                //this->n = this->readValue + chunkOffset;
                //fs->Seek(this->seekValue, Begin);
                this->n = this->readValue + chunkOffset;
				this->posix_map_buffer = (char *)mmap64(null, (Int32)this->n, PROT_WRITE, MAP_SHARED, this->fs->get_Handle(), chunkStart);

                if(this->posix_map_buffer == MAP_FAILED)
                    throw IOException((const char *)_sys_errlist[errno]);

			    //cout << "pageSize: " << this->pageSize << endl << endl;
				/*if(chunkOffset != 0)
					this->posix_map_buffer += chunkOffset;*/
				n = socket->Read(this->posix_map_buffer, (Int32)chunkOffset, (Int32)this->readValue);

				this->lastOffset = this->seekValue;
				this->lastLength = this->readValue;
				this->written += this->readValue;

				/*if(!UnmapViewOfFile(this->win_map_buffer))
					throw IOException(ErrorString(GetLastError()));*/

                if(n != readValue)
					throw ArgumentException("Bad format for FileTransferModeHeader.");

                this->n = this->readValue + chunkOffset;

				/*if(msync(this->posix_map_buffer, (Int32)this->n, MS_SYNC) == -1)
                    throw IOException((const char *)_sys_errlist[errno]);*/

                if(munmap(this->posix_map_buffer, (Int32)this->n) == -1)
                    throw IOException((const char *)_sys_errlist[errno]);

                this->posix_map_buffer = null;

			}
			else
			{//cout << "hjghjg" << endl;
			    if(TransferFromSocketToDisk_ZeroCopySplice(this->fs->get_Handle(), &this->seekValue, socket->get_BaseSocket()->get_Handle(), this->readValue, this->pipes_socket_to_disk, this->pageSize, this->pipes_socket_to_disk_is_defined, false) < 0)
                    throw IOException((const char *)_sys_errlist[errno]);
			}
#endif
			return false;
		}
		//----------------------------------------------------
		bool SessionClientUploadRequest::ReadFileBlockFTSMMode(ParvicursorSocket *socket)
		{
			/*if(socket.IsSecure)
			{
				if(socket.ReadByte() == 0)
					return true; // meaning end read.
				if(socket.Read(val1, val1.Length) != val1.Length)
					throw new ArgumentException("Bad format for FileTransferModeHeader.");
				if(socket.Read(val2, val2.Length) != val2.Length)
					throw new ArgumentException("Bad format for FileTransferModeHeader.");
				seekValue = (long)LongValueHeader.GetLongNumberFromBytesForFTSMMode(val1);
				readValue = (long)LongValueHeader.GetLongNumberFromBytesForFTSMMode(val2);
				if(readValue > info.tcpBufferSize)
					throw new ArgumentOutOfRangeException(string.Format("readValue in the header can not be greater than {0}.", info.tcpBufferSize));
				//byte[] buffer = new byte[readValue];
				n = socket.Read(buffer, (int)readValue);
				if(n != readValue)
					throw new ArgumentException("Bad format for FileTransferModeHeader.");
				return false;
			}
			else
			{*/
			/****enable this section after X-DotSec implementation****/
			//n = socket->ReadByte();

			/****disable this section after X-DotSec implementation****/
			/*******for X-xDFS Scattered I/O transfers*****/
			DoScatteredIO(socket);
			/**********************************************************/

			if(this->n == 0)
				return true; // meaning end read.

			//if(n == -1)
			//	throw ObjectDisposedException("The remote endpoint closed the connection");

			/****enable this section after X-DotSec implementation****/
			/*if(socket->Read(val1, 0, 8) != 8)
				throw ArgumentException("Bad format for FileTransferModeHeader.");
			if(socket->Read(val2, 0, 8) != 8)
				throw ArgumentException("Bad format for FileTransferModeHeader.");
			this->seekValue = GetLongNumberFromBytesForFTSMMode((byte *)this->val1);
			this->readValue = GetLongNumberFromBytesForFTSMMode((byte *)this->val2);*/

			//printf("seekValue: %ld readValue: %ld\n", seekValue, readValue);
			//std::cout << "seekValue: " << seekValue << " readValue: " << readValue << std::endl;
			if(this->readValue > info.blockSize)
				throw ArgumentOutOfRangeException("readValue in the header can not be greater than blockSize.");
			//byte[] buffer = new byte[readValue];
			n = socket->Read(buffer, 0, (Int32)this->readValue);
			if(n != readValue)
				throw ArgumentException("Bad format for FileTransferModeHeader.");
			return false;
			//}
		}
		//----------------------------------------------------
		/*int SessionClientUploadRequest::ReadFromOriginialSocket(ParvicursorSocket *socket, char array[], int offset, int count)
		{
			return 0;
		}*/
		//----------------------------------------------------
		void SessionClientUploadRequest::DoScatteredIO(ParvicursorSocket *socket)
		{

#if defined WIN32 || WIN64

			DWORD received, flags = MSG_PARTIAL;

			if(WSARecv(socket->get_BaseSocket()->get_Handle(), this->win_Scattered_IO, 3, &received, &flags, null, null) == SOCKET_ERROR)
			{
				int err = WSAGetLastError();
				throw SocketException(err, ErrorString(err));
			}

			if(received == 0)
				throw ObjectDisposedException("The remote endpoint closed the connection");

			if(received != 1 + 8 + 8 )
				throw ArgumentException("Bad format for FileTransferModeHeader.");

#else

            Int32 received = readv(socket->get_BaseSocket()->get_Handle(), this->posix_Scattered_IO, 3);

			if(received == -1)
                throw SocketException(errno, (const char *)_sys_errlist[errno]);

			if(received == 0)
				throw ObjectDisposedException("The remote endpoint closed the connection");

			if(received != 1 + 8 + 8)
				throw ArgumentException("Bad format for FileTransferModeHeader.");

#endif
			this->n = this->one_byte_buf_Scattered_IO[0];
			this->seekValue = GetLongNumberFromBytesForFTSMMode((byte *)this->val1);
			this->readValue = GetLongNumberFromBytesForFTSMMode((byte *)this->val2);
		}
		//----------------------------------------------------
		void SessionClientUploadRequest::WriteNoException(ParvicursorSocket *socket)
		{
			return ;
		}
		//----------------------------------------------------
		void SessionClientUploadRequest::SendOneExceptionToAllSockets(System::Exception &e)
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
		void SessionClientUploadRequest::SendExceptionToOneSocketAndClose(System::Exception &e, ParvicursorSocket *socket)
		{
			try { socket->WriteException(e); RemoveSocketFromArrayList(socket); }
			catch(...) { RemoveSocketFromArrayList(socket); }
		}
		//----------------------------------------------------
		void SessionClientUploadRequest::MakeReadSocketArrayList()
		{
			/* FD_ZERO() clears out the fd_set called socks, so that
				it doesn't contain any file descriptors. */
			FD_ZERO(&alRead);
			/* Loops through all the possible connections and adds
				those sockets to the fd_set */

			for (int i = 0 ; i < this->sockets->get_Count() ; i++)
			{
				 int handle = ((ParvicursorSocket *)sockets->get_Value(i))->get_BaseSocket()->get_Handle();
				/*if(handle != 0)
				{*/
					FD_SET(handle, &alRead);
					if (handle > highsock)
						highsock = handle;
				/*}*/
			}

			return ;
		}
		//----------------------------------------------------
		void SessionClientUploadRequest::RemoveSocketFromArrayList(ParvicursorSocket *socket)
		{
			int i = FindSocketIndex(socket);
			if(i != -1)
			{
				ParvicursorSocket *s = (ParvicursorSocket *)sockets->get_Value(i);
				//try{s->get_BaseSocket()->Shutdown(Both);}
				//catch(...) { }

				try{s->Close();}
				catch(...) { }

				sockets->RemoveAt(i);
				delete s;
			}
		}
		//----------------------------------------------------
		int SessionClientUploadRequest::OpenFileHandle()
		{
			//Console.WriteLine("OpenFileHandle ini" + memmoryToMemoryTests);
			try
			{
				int p = info.writeFilename.LastIndexOf("/");
				if(p < 0)
					p = info.writeFilename.LastIndexOf("\\");
				if(p > 0)
				{
					String directory = info.writeFilename.Substring(0, p);
					if(!Directory::Exists(directory))
						Directory::CreateDirectory(directory);
				}


#if defined WIN32 || WIN64
			if(!this->info.isServerSideZeroCopyEnabled)
				fs = new FileStream(info.writeFilename, OpenOrCreate, Write, 9);
			else
				fs = new FileStream(info.writeFilename, OpenOrCreate, ReadWrite, 9);
#else
            if(!this->info.isServerSideZeroCopyEnabled)
				fs = new FileStream(info.writeFilename, OpenOrCreate, Write, 9);
			else
				fs = new FileStream(info.writeFilename, OpenOrCreate, ReadWrite, 9);

#endif

#if defined __Scaling__

                if(!this->memmoryToMemoryTests)
                {

					fs->SetLength(0);
					fs->SetLength(this->info.fileSize);
                }

#else
                if(!this->memmoryToMemoryTests)
                {
                    /*if(this->info.isServerSideZeroCopyEnabled)
                    {
                        fs->SetLength(0);
                        fs->SetLength(this->info.fileSize);
                    }
                    else
                        fs->SetLength(0);//printf("file opened\n");*/
					fs->SetLength(0);
                    fs->SetLength(this->info.fileSize);
                }
#endif

				/*fs->SetLength(0);
				fs->SetLength(this->info.fileSize);*/
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
				cout << "error1: " << e.get_Message().get_BaseStream() << endl;
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
				cout << "error: " << e.get_Message().get_BaseStream() << endl;
				for(register Int32 i = 0 ; i < sockets->get_Count() ; i++)
				{
					try { ((ParvicursorSocket *)sockets->get_Value(i))->WriteException(e); }
					catch(...) { }
				}
				return -1;
			}
		}
		//----------------------------------------------------
		int SessionClientUploadRequest::FindSocketIndex(ParvicursorSocket *socket)
		{
			for(register Int32 i = 0 ; i < sockets->get_Count() ; i++)
				if((ParvicursorSocket *)sockets->get_Value(i) == socket)
					return i;
			return -1;
		}
		//----------------------------------------------------
		void SessionClientUploadRequest::WorkerExit()
		{

			if(this->circularBufferEnabled)
			{
				pthread_mutex_lock(&this->mutex);

				if(this->disk_thread != null)
				{
					pthread_cancel(this->disk_thread->thread);
					delete this->disk_thread;
					this->disk_thread = null;
				}
				pthread_mutex_unlock(&this->mutex);
				pthread_mutex_destroy(&this->mutex);
				if(this->circular_buffer != null)
				{
					delete this->circular_buffer;
					this->circular_buffer = null;
				}
			}
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
                            //try {s->get_BaseSocket()->Shutdown(Both);}
                            //catch(...) { }

                            try {s->Close();}
                            catch(...) { }

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
                TransferFromSocketToDisk_ZeroCopySplice(-1, null, -1, -1, this->pipes_socket_to_disk, -1, this->pipes_socket_to_disk_is_defined, true);

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
		void *SessionClientUploadRequest::Wrapper_To_Call_DiskThreadWorkerProc(void* pt2Object)
		{
			pthread_setcanceltype(PTHREAD_CANCEL_ENABLE, null);
			pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, null);

			SessionClientUploadRequest* mySelf = (SessionClientUploadRequest*) pt2Object;
			// call member
			mySelf->DiskThreadWorkerProc_Consumer();
			return pt2Object;
		}
		//----------------------------------------------------
		void SessionClientUploadRequest::DiskThreadWorkerProc_Consumer()
		{
			Int32 rett;
			while(true)
			{
				rett = unbuffer_device();

				switch(rett)
				{
					case Parvicursor::Shared::Enums::xDFS::BufferOperationCompleted:
					{
						//written += this->__n;
						break;
					}
					case Parvicursor::Shared::Enums::xDFS::BufferIsFull:
					{
						//printf("Parvicursor::Shared::Enums::xDFS::BufferIsFull:Consumer\n");
						break;
					}
					case Parvicursor::Shared::Enums::xDFS::BufferIsEmpty:
					{
						Thread::Sleep(1);
						break;
					}
					case Parvicursor::Shared::Enums::xDFS::EndOfFileButBufferNotEmpty:
					{
						//printf("Parvicursor::Shared::Enums::xDFS::EndOfFileButBufferNotEmpty:Consumer\n");
						break;
					}
					case Parvicursor::Shared::Enums::xDFS::EndOfFileAndBufferIsEmpty:
					{
						//printf("Parvicursor::Shared::Enums::xDFS::EndOfFileAndBufferIsEmpty:Consumer:ThreadExit\n");
						return ;
					}
					default:
					{
						break;
					}
				}
			}
		}
		//----------------------------------------------------
		CircularBufferState SessionClientUploadRequest::buffer_device(void)
		{
			//cout << "buffer_device" << endl;
			Int32 temp = (this->end + this->info.blockSize + this->xFTSM_HeaderLength) % this->BUFFER_SIZE;
			//Use modulo as a trick to wrap around the end of the buffer back to the beginning
			if (temp != this->start)
			{
				pthread_mutex_lock(&this->mutex);

				this->circular_buffer += end;
				memmove(this->circular_buffer, &this->seekValue, sizeof(Int64));
				this->circular_buffer += sizeof(Int64);
				memmove(this->circular_buffer, &this->readValue, sizeof(Int64));
				this->circular_buffer += sizeof(Int64);
				memmove(this->circular_buffer, this->buffer, (Int32)this->readValue);
				this->circular_buffer -= (end + sizeof(Int64) + sizeof(Int64));

				this->blocks++;

				end = temp;
				//printf("temp: %d, end: %d, offset: %d\n", temp, end, currentOffset);
				pthread_mutex_unlock(&this->mutex);
				return Parvicursor::Shared::Enums::xDFS::BufferOperationCompleted;
			}
			else
				return Parvicursor::Shared::Enums::xDFS::BufferIsFull;
			//printf("%d\n", 201%200);
			//otherwise, the buffer is full; don't do anything. you might want to return an error code to notify the writing process that the buffer is full.
		}
		//----------------------------------------------------
		CircularBufferState SessionClientUploadRequest::unbuffer_device(void)
		{
			if (this->end != this->start)
			{
				if(this->nextOffset == -1 && this->blocks == 0)
					return Parvicursor::Shared::Enums::xDFS::EndOfFileAndBufferIsEmpty;
				else
				{
					pthread_mutex_lock(&this->mutex);
					Int64 read = 0, offset = 0;
					this->circular_buffer += start;
					memmove(&offset, this->circular_buffer, sizeof(Int64));
					this->circular_buffer += sizeof(Int64);
					memmove(&read, this->circular_buffer, sizeof(Int64));
					this->circular_buffer += sizeof(Int64);

					//Console.WriteLine("offset: {0}, length: {1}", (int)seekValue, (int)readValue);
					k++;
					/*if(memmoryToMemoryTests)
						return;*/////
					if(offset != this->lastOffset + this->lastLength)
					{
						fs->Seek(offset, Begin);
						//fs.Write(buffer, 0, (int)readValue);
						j++;
					}
					/*else
					{
						fs.Write(buffer, 0, (int)readValue);
						//for(int i = 0 ; i < (int) readValue; i++)
						//	Console.Write(buffer[i]);
					}*/
					//fs.Seek(seekValue, SeekOrigin.Begin);
					this->fs->Write(this->circular_buffer, 0, (Int32)read);
					this->lastOffset = offset;
					this->lastLength = read;
					this->written += read;
					//Console.WriteLine("offset: {0}, seekNumber: {1}, written: {2}", seekValue , j, written);

					this->circular_buffer -= (start + sizeof(Int64) + sizeof(Int64));
					this->blocks--;
					pthread_mutex_unlock(&this->mutex);

					this->start = (this->start + this->info.blockSize + this->xFTSM_HeaderLength) % this->BUFFER_SIZE;
					//return(temp);
					return Parvicursor::Shared::Enums::xDFS::BufferOperationCompleted;
				}
			}
			//otherwise, the buffer is empty; return an error code
			else
			{
				if(this->nextOffset == -1)
					return Parvicursor::Shared::Enums::xDFS::EndOfFileAndBufferIsEmpty;
				else
					return Parvicursor::Shared::Enums::xDFS::BufferIsEmpty;
			}
		}
   		//----------------------------------------------------
   };
};
//**************************************************************************************************************//
