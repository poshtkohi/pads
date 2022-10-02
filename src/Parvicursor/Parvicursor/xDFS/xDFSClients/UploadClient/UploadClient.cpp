#include "UploadClient.h"

//**************************************************************************************************************//
namespace Parvicursor
{
   namespace xDFS
   {
	   namespace xDFSClients
	   {
 			//----------------------------------------------------
			UploadClient::UploadClient(const String &readFilename, const String &remoteFilename, Int32 parallel, Int32 blockSize, Int32 tcpBufferSize, Int32 circularBufferCountClientSide, Int32 circularBufferCountServerSide, const String &xDFSServerAddress, const NetworkCredential &nc, bool secure/*, ref RijndaelEncryption rijndael*/, bool memmoryToMemoryTests, ArrayList *errors, bool IsClientSideZeroCopyEnabled, bool IsServerSideZeroCopyEnabled)
			{

				if(readFilename.get_BaseStream() == null)
					throw ArgumentNullException("readFilename is a null reference.");

				if(remoteFilename.get_BaseStream() == null)
					throw ArgumentNullException("remoteFilename is a null reference.");

				if(xDFSServerAddress.get_BaseStream() == null)
					throw ArgumentNullException("xDFSServerAddress is a null reference.");

				if(&nc == null)
					throw ArgumentNullException("nc is a null reference.");

				if(parallel <= 0)
					throw ArgumentOutOfRangeException("parallel parameter can not be negative or zero.");

				if(blockSize <= 0)
					throw ArgumentOutOfRangeException("blockSize parameter can not be negative or zero.");

				if(tcpBufferSize < 0)
					throw ArgumentOutOfRangeException("tcpBufferSize parameter can not be negative.");


				//this.qread = qread;
				this->readFilename = readFilename;
				this->remoteFilename = remoteFilename;
				this->parallel = parallel;
				//this->blockSize = 64*1024-17; //this->blockSize = blockSize;
				this->xDFSServerAddress = xDFSServerAddress;
				this->nc = (NetworkCredential *)&nc;
				this->secure = secure;
				////this->rijndael = rijndael;
				this->memmoryToMemoryTests = memmoryToMemoryTests;
				this->errors = errors;
				this->worker = null;
				this->closed = false;
				this->ended = false;
				this->socket = null;
				this->seekValue = null;
				this->readValue = null;
				this->buffer = null;
				this->socket = null;
				this->fs = null;
				this->sockets = null;
				this->written = 0;
				this->offsetSeek = 0;
				this->last_n = 0;
				this->n = 0;
				this->pageSize = 64 * 1024;
				this->written_mutex = new Mutex();

#if defined WIN32 || WIN64

				SYSTEM_INFO SysInfo;
				GetSystemInfo(&SysInfo);
				this->pageSize = SysInfo.dwAllocationGranularity;

				this->win_file_handle = null;
				this->win_map_handle = null;
				this->win_map_buffer = null;
				this->isNormalZeroCopySupported = false; // false for memory-mapped file. true for TransmiteFile.
#else

				this->pageSize = (Int32)sysconf(_SC_PAGESIZE);
				if(this->pageSize == -1)
					this->pageSize = getpagesize();

				this->posix_map_buffer = null;
                this->isNormalZeroCopySupported = true;  // false for memory-mapped file. true for splice/sendfile64.

                this->pipes_socket_to_disk[0] = -1;
				this->pipes_socket_to_disk[1] = -1;
				this->pipes_socket_to_disk_is_defined = false;

				struct utsname SysInfo;
				if(uname(&SysInfo) < 0)
                    throw System::Exception((const char *)_sys_errlist[errno]);

#if defined __linux__

                this->isNormalZeroCopySupported = false;
                //cout << name.release << endl << endl; // 2.6.25.5-1.1-pae
                if(this->isClientSideZeroCopyEnabled)
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

				this->fs = new FileStream(this->readFilename, Open, Read, 9); // FileShare
				this->fileSize = this->fs->get_Length();
				this->guid = Guid::NewGuid();

				this->isServerSideZeroCopyEnabled = IsServerSideZeroCopyEnabled;
				this->isClientSideZeroCopyEnabled = IsClientSideZeroCopyEnabled;


#if defined WIN32 || WIN64
				if(this->isClientSideZeroCopyEnabled)
				{
					this->win_file_handle = (HANDLE)_get_osfhandle(this->fs->get_Handle());

					if(this->win_file_handle == INVALID_HANDLE_VALUE)
					{
						int err = GetLastError();
						Close();
						throw IOException(ErrorString(err));
					}

					if(!this->isNormalZeroCopySupported)
					{
						//DWORD dwFileMapSize = (DWORD)this->fileSize - this->blockSize;
						this->win_map_handle = CreateFileMapping(this->win_file_handle,
										null,
										PAGE_READONLY,
										0,
										0,
										null);

						if(this->win_map_handle == null)
						{
							//cout << "win_map_handle: " << GetLastError() << endl << endl; //
							int err = GetLastError();
							//CloseHandle(win_map_handle);
							Close();
							throw IOException(ErrorString(err));
						}
					}
				}
#endif

				//-------------
				//-------------

				//cout << "fileSize: " << this->fileSize << endl;

				if(this->parallel > 1)
					sockets = new ArrayList();

				/*if(blockSize < this->pageSize)
					this->blockSize = this->pageSize;
				else
					this->blockSize = blockSize;*/

				this->tcpBufferSize = tcpBufferSize;


				/*if(this->isClientSideZeroCopyEnabled && !this->isNormalZeroCopySupported)
					this->blockSize = this->pageSize;//blockSize;//blockSize;//this->tcpBufferSize;// - 17; //this->blockSize = blockSize;
				else
					this->blockSize = this->tcpBufferSize - 17;*/

				//this->blockSize = this->pageSize;
				this->blockSize = blockSize;//tcpBufferSize - 17;//blockSize;


				/********** disk thread settings ****************/
				this->disk_thread = null;
				this->circular_buffer = null;
				this->circularBufferCountClientSide = circularBufferCountClientSide;
				this->circularBufferCountServerSide = circularBufferCountServerSide;
				if(this->circularBufferCountClientSide > 0)
				{
					this->circularBufferEnabledClientSide = true;
					this->circularBufferCountClientSide = circularBufferCountClientSide;
				}
				else
				{
					this->circularBufferEnabledClientSide = false;
					this->circularBufferCountClientSide = -1;
				}

				if(this->isClientSideZeroCopyEnabled || this->memmoryToMemoryTests)
					this->circularBufferEnabledClientSide = false;


				if(this->circularBufferEnabledClientSide)
				{
					pthread_mutex_init(&this->mutex, NULL);
					this->blocks = 0;
					this->start = 0;
					this->end = 0;
					this->_n = 0;
					this->__n = 0;
					this->_last_n = 0;
					this->currentOffset = 0;
					this->nextOffset = 0;
					this->_offset = 0;
					this->BUFFER_SIZE = this->blockSize*this->circularBufferCountClientSide;
				}

				//cout << circularBufferCountClientSide << endl;
				/************************************************/



			}
			//----------------------------------------------------
			UploadClient::~UploadClient()
			{
				if(!this->closed)
				{
					this->completion_cv->Signal();
					delete this->completion_mutex;
					delete this->completion_cv;
					Close();
					//printf("hellow world\n");
				}
			}
			//----------------------------------------------------
			Int64 UploadClient::get_CurrentTransferredBytes()
			{
				if(!this->closed)
				{
					Int64 ret;
					this->written_mutex->Lock();
					ret = this->written;
					this->written_mutex->Unlock();
					return ret;
				}
				return -1;
			}
			//----------------------------------------------------
			Int64 UploadClient::get_FileSize()
			{
				if(!this->closed)
				{
					if(this->memmoryToMemoryTests)
						//return 0xFFFFFFFFFFFFFFFFLL;
						return 0x0FFFFFFFFFFFFFFFLL;
					else
					{
						if(this->fs != null )
							return this->fileSize;
					}
				}

				return -1;
			}
			//----------------------------------------------------
			/*const DateTime &UploadClient::get_StartTime()
			{
				return (const DateTime &)DateTime::get_Now();
			}
			//----------------------------------------------------
			const DateTime &UploadClient::get_EndTime()
			{
				return (const DateTime &)DateTime::get_Now();
			}*/
			//----------------------------------------------------
			void UploadClient::WorkerProc()
			{
				/********** disk thread settings ****************/
				if(this->circularBufferEnabledClientSide)
				{
					this->circular_buffer = new char[this->blockSize*this->circularBufferCountClientSide];
					this->disk_thread = new Thread(this->Wrapper_To_Call_DiskThreadWorkerProc, (void *)this);
					this->disk_thread->Start();
					this->disk_thread->SetDetached();
					while(blocks != this->circularBufferCountClientSide - 1)
						Thread::Sleep(1);
				}//Thread::Sleep(10000000);
				//cout << circularBufferCountClientSide << endl;
				/************************************************/
				//Int32 receiveTcpBufferSize = 1; //this->tcpBufferSize;
				try
				{
					IPHostEntry hostEntry = Dns::Resolve2(xDFSServerAddress);
					IPAddress ip = hostEntry.get_AddressList(0);
					IPEndPoint inp(ip, __xDFS_Server_Default_Port__);

					//Serializer se(this->readFilename.get_Length() + 1 + this->guid.get_Length() + 1 + sizeof(Int64) + 2 * sizeof(Int32));
					Serializer se(512);
					se.Write(this->guid);
					se.Write(this->remoteFilename);
					se.Write<Int64>(get_FileSize());
					se.Write<Int32>(this->parallel);
					se.Write<Int32>(this->blockSize);
					se.Write<Int32>(this->tcpBufferSize);
					se.Write<bool>(this->isServerSideZeroCopyEnabled);
					se.Write<Int32>(circularBufferCountServerSide);

					if(this->parallel == 1)
					{
						Socket *sock = new Socket(ip.get_AddressFamily(), Stream, tcp);

#if !defined WIN32 || WIN64
                this->tcpBufferSize = this->tcpBufferSize / 2;
#endif

#if defined Enable_TCP_Window_Scale
						//sock->SetReceiveTcpWindowSize(this->tcpBufferSize);//sock->SetReceiveTcpWindowSize(this->tcpBufferSize);//////must delete
						//sock->SetReceiveTcpWindowSize(0);//////must delete
						sock->SetSendTcpWindowSize(this->tcpBufferSize);
#endif

						if(sock == null)
							throw ObjectDisposedException("Could not instantiate from Socket class.");

						socket = new ParvicursorSocket(sock);

						try
						{
							socket->get_BaseSocket()->Connect(inp);
							socket->WriteByte((char)SingleFileTransferUploadFromClient);
							socket->WriteObject(se.get_BaseBuffer(), se.get_BaseBufferSize());
							socket->CheckExceptionResponse();

#if defined Enable_TCP_Window_Scale
							//sock->SetReceiveTcpWindowSize(this->tcpBufferSize);//sock->SetReceiveTcpWindowSize(this->tcpBufferSize);//////must delete
							cout << "GetSendTcpWindowSize: " << sock->GetSendTcpWindowSize() << " GetReceiveTcpWindowSize(): " << sock->GetReceiveTcpWindowSize() << endl << endl;
#endif
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
					}
					else
					{
						for(register Int32 i = 0 ; i < this->parallel ; i++)
						{

							Socket *sock = new Socket(ip.get_AddressFamily(), Stream, tcp);

#if defined Enable_TCP_Window_Scale
							//sock->SetReceiveTcpWindowSize(this->tcpBufferSize);//////must delete
							//sock->SetReceiveTcpWindowSize(0);//////must delete
							sock->SetSendTcpWindowSize(this->tcpBufferSize);
#endif

							if(sock == null)
								throw ObjectDisposedException("Could not instantiate from Socket class.");

							ParvicursorSocket *s = new ParvicursorSocket(sock);
							this->sockets->Add(s);
							try
							{
								s->get_BaseSocket()->Connect(inp);

								s->WriteByte((char)SingleFileTransferUploadFromClient);
								s->WriteObject(se.get_BaseBuffer(), se.get_BaseBufferSize());
								s->CheckExceptionResponse();

							}
							catch(System::Exception &e)
							{
								if(this->errors != null)
									this->errors->Add(new System::Exception(e.get_Message()));

								if(i == 0)
								{
									WorkerExit();
									exited = true;
									return;
								}
								else
									continue;
							}
							catch(...)
							{
								if(this->errors != null)
										this->errors->Add(new System::Exception("Unknown exception was occured"));

								if(i == 0)
								{
									WorkerExit();
									exited = true;
									return;
								}

								else
									continue;
							}
						}
					}
					//WorkerExit();
					//return;
					if(this->errors != null)
						if(this->errors->get_Count() != 0)
						{
							WorkerExit();
							exited = true;
							return;
						}


					//ParvicursorSocket socket = new ParvicursorSocket(reader, writer);
					//t1 = DateTime.Now;

					this->seekValue = new char[8];
					this->readValue = new char[8];

					if(!this->isClientSideZeroCopyEnabled/* && !this->circularBufferEnabledClientSide*/)
						this->buffer = new char[this->blockSize];
					else
						this->buffer = null;


					this->offsetSeek = 0;
					this->n = 0;
					this->written_mutex->Lock();
					this->written = 0;
					this->written_mutex->Unlock();
					/*******for X-xDFS Gathered/Scattered I/O transfers*****/
					this->one_byte_buf_Scattered_IO[0] = (char)1; // signaling the server about arrival of new file block.

#if defined WIN32 || WIN64

					this->win_Scattered_IO[0].buf = one_byte_buf_Scattered_IO;
					this->win_Scattered_IO[0].len = 1;

					this->win_Scattered_IO[1].buf = this->seekValue;
					this->win_Scattered_IO[1].len = 8;

					this->win_Scattered_IO[2].buf = this->readValue;
					this->win_Scattered_IO[2].len = 8;

					//this->win_Scattered_IO[3].buf = this->buffer;
					//this->win_Scattered_IO[3].len = 8;

#else

					this->posix_Scattered_IO[0].iov_base = one_byte_buf_Scattered_IO;
					this->posix_Scattered_IO[0].iov_len = 1;

					this->posix_Scattered_IO[1].iov_base = this->seekValue;
					this->posix_Scattered_IO[1].iov_len = 8;

					this->posix_Scattered_IO[2].iov_base = this->readValue;
					this->posix_Scattered_IO[2].iov_len = 8;

					//this->posix_Scattered_IO[3].iov_base = this->buffer;
					//this->posix_Scattered_IO[3].iov_len = this->n;

#endif
					/************************************************/

					if(this->parallel == 1)
					{
						while(true)
						{
							if(closed)
							{
								exited = true;
								this->written_mutex->Lock();
								this->written = -1;
								this->written_mutex->Unlock();
								return ;
							}
							//if(written >= fileSize)
							//{
								//t2 = DateTime.Now;
								//Console.WriteLine(t2);
								//break;
							//}
							//}


							if(!this->isClientSideZeroCopyEnabled)
							{
								if(!this->circularBufferEnabledClientSide)
								{
									ReadFromFile();

									if(offsetSeek == -1)
									{
										///t2 = DateTime.Now;
										//Console.WriteLine(t2);
										//printf("offsetSeek");
										//socket->CheckExceptionResponse();
										goto End;
									}

									/****enable this section after X-DotSec implementation****/

									/*socket->WriteByte(1);// signaling the server about arrival of new file block.
									GetBytesOfLongNumberForFTSMMode(this->seekValue, this->offsetSeek);
									GetBytesOfLongNumberForFTSMMode(this->readValue, this->n);

									this->socket->Write(this->seekValue, 0, 8);
									this->socket->Write(this->readValue, 0, 8);
									//printf("seekValue: %ld readValue: %ld\n", this->offsetSeek, (long)this->n);
									//std::cout << "seekValue: " << this->offsetSeek << " readValue: " << this->n << std::endl;

									this->socket->Write(this->buffer, 0, (Int32)this->n);*/

									/****disable this section after X-DotSec implementation****/
									/*******for X-xDFS Scattered I/O transfers*****/
									this->DoScatteredIO(this->socket, this->buffer);
									/************************************************/
								}
								else
								{
									Int32 rett;
									while(true)
									{
										rett = unbuffer_device(this->socket);

										switch(rett)
										{
											case Parvicursor::Shared::Enums::xDFS::BufferOperationCompleted:
											{
												this->written_mutex->Lock();
												written += this->__n;
												this->written_mutex->Unlock();
												break;
											}
											case Parvicursor::Shared::Enums::xDFS::BufferIsFull:
											{
												printf("Parvicursor::Shared::Enums::xDFS::BufferIsFull:Consumer\n");
												break;
											}
											case Parvicursor::Shared::Enums::xDFS::BufferIsEmpty:
											{
												Thread::Sleep(1);
												break;
											}
											case Parvicursor::Shared::Enums::xDFS::EndOfFileButBufferNotEmpty:
											{
												printf("Parvicursor::Shared::Enums::xDFS::EndOfFileButBufferNotEmpty:Consumer\n");
												this->written_mutex->Lock();
												written += this->__n;
												this->written_mutex->Unlock();
												break;
											}
											case Parvicursor::Shared::Enums::xDFS::EndOfFileAndBufferIsEmpty:
											{
												printf("Parvicursor::Shared::Enums::xDFS::EndOfFileAndBufferIsEmpty:Consumer:ThreadExit\n");
												goto End;
											}
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
								this->ZeroCopyTransfer(this->socket);
								if(offsetSeek == -1)
								{
									goto End;
								}
								//std::cout << "seekValue: " << this->offsetSeek << " readValue: " << this->n << std::endl;
							}

							this->written_mutex->Lock();
							written += n;
							this->written_mutex->Unlock();

#if defined With_Error_Recovery_Support
							socket->CheckExceptionResponse();
#endif
							//Thread.Sleep(1);

							if(ended)
							{
								break;
							}
						}
						if(closed)
						{
							this->written_mutex->Lock();
							this->written = -1;
							this->written_mutex->Unlock();
							exited = true;
							return ;
						}
						////seekValue = readValue = null;
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
#if defined With_Error_Recovery_Support
								Thread::Sleep(1);
							//if(!this->alWrite_enable)
#endif
							//clock();
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

#if defined With_Error_Recovery_Support
										//this->alWrite_enable = false;
#endif

										try
										{
											if(!this->isClientSideZeroCopyEnabled)
											{
												if(!this->circularBufferEnabledClientSide)
												{
													ReadFromFile();

													if(offsetSeek == -1)
													{
														//cout << endl << this->written << endl;
														goto End;
													}
													/****enable this section after X-DotSec implementation****/
													/*sock->WriteByte(1);// signaling the server about arrival of new file block.
													GetBytesOfLongNumberForFTSMMode(this->seekValue, this->offsetSeek);
													GetBytesOfLongNumberForFTSMMode(this->readValue, this->n);

													sock->Write(this->seekValue, 0, 8);
													sock->Write(this->readValue, 0, 8);
													//printf("seekValue: %ld readValue: %ld\n", this->offsetSeek, (long)this->n);
													//std::cout << "seekValue: " << this->offsetSeek << " readValue: " << this->n << std::endl;


													sock->Write(this->buffer, 0, (Int32)this->n);*/

													/****disable this section after X-DotSec implementation****/
													this->DoScatteredIO(sock, this->buffer);
												}
												else
												{
													Int32 rett;
													while(true)
													{
														rett = unbuffer_device(sock);

														switch(rett)
														{
															case Parvicursor::Shared::Enums::xDFS::BufferOperationCompleted:
															{
																this->written_mutex->Lock();
																written += this->__n;
																this->written_mutex->Unlock();
																goto Continue_From_CircularBuffer;
															}
															case Parvicursor::Shared::Enums::xDFS::BufferIsFull:
															{
																printf("Parvicursor::Shared::Enums::xDFS::BufferIsFull:Consumer\n");
																this->written_mutex->Lock();
																written += this->__n;
																this->written_mutex->Unlock();
																goto Continue_From_CircularBuffer;
															}
															case Parvicursor::Shared::Enums::xDFS::BufferIsEmpty:
															{
																Thread::Sleep(1);
																break;
															}
															case Parvicursor::Shared::Enums::xDFS::EndOfFileButBufferNotEmpty:
															{
																printf("Parvicursor::Shared::Enums::xDFS::EndOfFileButBufferNotEmpty:Consumer\n");
																this->written_mutex->Lock();
																written += this->__n;
																this->written_mutex->Unlock();
																goto Continue_From_CircularBuffer;
															}
															case Parvicursor::Shared::Enums::xDFS::EndOfFileAndBufferIsEmpty:
															{
																printf("Parvicursor::Shared::Enums::xDFS::EndOfFileAndBufferIsEmpty:Consumer:ThreadExit\n");
																goto End;
															}
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
												if(offsetSeek == -1)
												{
													goto End;
												}
												this->ZeroCopyTransfer(sock);
												if(offsetSeek == -1)
												{
													goto End;
												}
											}

											this->written_mutex->Lock();
											written += n;
											this->written_mutex->Unlock();

									Continue_From_CircularBuffer:
											this->written;


#if defined With_Error_Recovery_Support
											sock->set_IsCheckedExceptionResponse(NotDone);
											//socket->CheckExceptionResponse();
#endif
										}
										catch(System::Exception &e)
										{
											if(this->errors != null)
												this->errors->Add(new System::Exception(e.get_Message()));

											RemoveSocketFromArrayList(sock);
											goto here1;
										}
										catch(...)
										{
											if(this->errors != null)
												this->errors->Add(new System::Exception("Unknown exception"));

											RemoveSocketFromArrayList(sock);
											goto here1;
										}

										//continue;
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
								readysocks = ::select(highsock + 1, &alRead, (fd_set *)null, (fd_set *)null, &timeoutRead);
								if(readysocks == -1)
								{
									//cout << ErrorString(WSAGetLastError()) << endl << endl;
									goto End;
								}
								if(readysocks == 0)
									continue;
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
										catch(Exception &e)
										{
											if(this->errors != null)
												this->errors->Add(new Exception(e.get_Message()));

											RemoveSocketFromArrayList(sock);
											goto here2;
										}
										catch(...)
										{
											if(this->errors != null)
												this->errors->Add(new Exception("Unknown exception"));

											RemoveSocketFromArrayList(sock);
											goto here2;
										}

										//continue;
									}
								}
							}
#endif
							//-------------------------------------------------------------
						}
					}
				}
				catch(System::Exception &e)
				{
					if(this->errors != null)
						this->errors->Add(new System::Exception(e.get_Message()));

					WorkerExit();
					exited = true;
					return;
				}
				catch(...)
				{
					if(this->errors != null)
							this->errors->Add(new System::Exception("An unknown exception was occured"));

					WorkerExit();
					exited = true;

					return;
				}


			End:

				//------signaling the server about arrival of new file block-------------------
				try
				{
					if(this->parallel > 1)
					{
						MakeReadyWriteSocketArrayList();
						Int32 readysocks = ::select(highsock + 1, (fd_set *)null, &alWrite, (fd_set *)null, null);

						if(readysocks > 0)
						{
							for(register Int32 i = 0 ; i < this->sockets->get_Count() ; i++)
							{
								ParvicursorSocket *sock = (ParvicursorSocket *)sockets->get_Value(i);
								if(FD_ISSET(sock->get_BaseSocket()->get_Handle(), &alWrite))
								{
									/****enable this section after X-DotSec implementation****/
									//sock->WriteByte(0);// signaling the server about arrival of new file block.

									/****disable this section after X-DotSec implementation****/
									this->one_byte_buf_Scattered_IO[0] = (char)0;
									this->offsetSeek = 0;
									this->n = 0;
									this->DoScatteredIO(sock);

									break;
								}
							}
						}
						exited = true;
					}
					else
					{
						/****enable this section after X-DotSec implementation****/
						//socket->WriteByte(0); // signaling the server about finalization of transferring file blocks.

						/****disable this section after X-DotSec implementation****/
						this->one_byte_buf_Scattered_IO[0] = (char)0;
						this->offsetSeek = 0;
						this->n = 0;
						this->DoScatteredIO(this->socket);
						//socket->CheckExceptionResponse();
						//double elapsed = (t2 - t1).TotalMilliseconds;
						//Console.WriteLine("Seconds: {0} ", elapsed/1000);
						//Console.WriteLine("Real Average Speed(MBytes/s): {0} MBytes/s", (fileSize/(1024*1024))/(elapsed / 1000)   );
						//qread.Close();
						exited = true;
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
							this->errors->Add(new System::Exception("An unknown exception was occured"));
				}
				//-----------------------------------------------------------------------------

				this->written_mutex->Lock();
				this->written = -1;
				this->written_mutex->Unlock();

				WorkerExit();
				exited = true;


			}
			//----------------------------------------------------
			void UploadClient::ReadFromFile()
			{

#ifdef __Scaling__
				//if(this->fileSize > 900*1024*1024 && this->fileSize < 1200*1024*1024 && this->offsetSeek > 800*1024*1024)
				if(this->fileSize > 900*1024*1024 && this->fileSize < 1200*1024*1024 && this->offsetSeek > 300*1024*1024)
				{
					this->written_mutex->Lock();
					this->written = this->fileSize;
					this->written_mutex->Unlock();
					this->offsetSeek = -1;
					this->n = -1;
					return ;
				}
				if(this->fileSize > (Int64)(1800*1024*1024) && this->fileSize < (Int64)(2200*1024*1024) && this->offsetSeek > (Int64)(1800*1024*1024))
				{
					this->written_mutex->Lock();
					this->written = this->fileSize;
					this->written_mutex->Unlock();
					this->offsetSeek = -1;
					this->n = -1;
					return ;
				}
#endif
				this->n = this->fs->Read(this->buffer, 0, this->blockSize);
				if(n <= 0)
				{
					this->offsetSeek = -1;
					return ;
				}
				this->k++;
				this->offsetSeek = this->fs->get_Position() - this->n;
			}
            //----------------------------------------------------
			void UploadClient::ZeroCopyTransfer(ParvicursorSocket *socket)
			{

#ifdef __Scaling__
				//if(this->fileSize > 900*1024*1024 && this->fileSize < 1200*1024*1024 && this->offsetSeek > 800*1024*1024)
				if(this->fileSize > 900*1024*1024 && this->fileSize < 1200*1024*1024 && this->offsetSeek > 300*1024*1024)
				{
					this->written_mutex->Lock();
					this->written = this->fileSize;
					this->written_mutex->Unlock();
					this->offsetSeek = -1;
					this->n = -1;
					return ;
				}
				if(this->fileSize > (Int64)(1800*1024*1024) && this->fileSize < (Int64)(2200*1024*1024) && this->offsetSeek > (Int64)(1800*1024*1024))
				{
					this->written_mutex->Lock();
					this->written = this->fileSize;
					this->written_mutex->Unlock();
					this->offsetSeek = -1;
					this->n = -1;
					return ;
				}
#endif
				Int64 reminding = this->fileSize - this->written;

				if(reminding <=0 )
				{
					this->offsetSeek = -1;
					return;
				}
				if(reminding > this->blockSize)
				{
					this->n = this->blockSize;
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


#if defined WIN32 || WIN64

				if(this->isNormalZeroCopySupported) // based on TransmiteFile Win32/64 API function.
				{

					/****disable this section after X-DotSec implementation****/
					this->DoScatteredIO(socket);

					this->fs->Seek(this->offsetSeek, Begin);

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
					Int64 chunkOffset = this->offsetSeek % this->pageSize;
					Int64 chunkStart = this->offsetSeek - chunkOffset;
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
				if(this->isNormalZeroCopySupported) // based on Splice/sendfile POSIX API functions.
				{
                    //cout << "hellooooooooooooooooooooo" << endl << endl;
                    this->DoScatteredIO(socket);

                    if(TransferFromDiskToSocket_ZeroCopySplice(this->fs->get_Handle(), null, socket->get_BaseSocket()->get_Handle(), (Int32)this->n, this->pipes_socket_to_disk, this->pageSize, this->pipes_socket_to_disk_is_defined, false) < 0)
                        throw IOException((const char *)_sys_errlist[errno]);

				}
				else // based on File-based memory mapping by using mmap/munmap POSIX API functions.
				{
					Int64 chunkOffset = this->offsetSeek % this->pageSize;
					Int64 chunkStart = this->offsetSeek - chunkOffset;

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
				this->offsetSeek = this->written + this->n;
				//cout << "this->offsetSeek: " << this->offsetSeek << " this->n: " << this->n << endl << endl;
			}
			//----------------------------------------------------
			void UploadClient::SendOneExceptionToAllSocketsAndClose(System::Exception &e)
			{
				return ;
			}
			//----------------------------------------------------
			void UploadClient::SendExceptionToOneSocketAndClose(System::Exception &e, ParvicursorSocket *socket)
			{
				try { socket->WriteException(e); RemoveSocketFromArrayList(socket); }
				catch(...) { RemoveSocketFromArrayList(socket); }
			}
			//----------------------------------------------------
			void UploadClient::MakeReadyReadSocketArrayList()
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
			void UploadClient::MakeReadyWriteSocketArrayList()
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
			void UploadClient::DoScatteredIO(ParvicursorSocket *socket, char *buffer)
			{
				GetBytesOfLongNumberForFTSMMode(this->seekValue, this->offsetSeek);
				GetBytesOfLongNumberForFTSMMode(this->readValue, this->n);

#if defined WIN32 || WIN64

				this->win_Scattered_IO[3].buf = buffer;
				this->win_Scattered_IO[3].len = (UInt32)this->n;

				DWORD sent;

				if(WSASend(socket->get_BaseSocket()->get_Handle(), this->win_Scattered_IO, 4, &sent, 0, null, null) == SOCKET_ERROR)
				{
					int err = WSAGetLastError(); cout << "hello11" << endl << endl;
					throw SocketException(err, ErrorString(err));
				}

				if(sent != 1 + 8 + 8 + (DWORD)this->n)
					throw System::Exception("The remote endpoint was closed the connection.");

#else

				this->posix_Scattered_IO[3].iov_base = buffer;
				this->posix_Scattered_IO[3].iov_len = (UInt32)this->n;


                Int32 sent = writev(socket->get_BaseSocket()->get_Handle(), this->posix_Scattered_IO, 4);

				if(sent == -1)
                    throw SocketException(errno, (const char *)_sys_errlist[errno]);

				if(sent != 1 + 8 + 8 + (Int32)this->n)
					throw System::Exception("The remote endpoint was closed the connection.");

#endif
			}
			//----------------------------------------------------
			void UploadClient::DoScatteredIO(ParvicursorSocket *socket)
			{

				GetBytesOfLongNumberForFTSMMode(this->seekValue, this->offsetSeek);
				GetBytesOfLongNumberForFTSMMode(this->readValue, this->n);

#if defined WIN32 || WIN64


				DWORD sent;

				if(WSASend(socket->get_BaseSocket()->get_Handle(), this->win_Scattered_IO, 3, &sent, 0, null, null) == SOCKET_ERROR)
				{
					int err = WSAGetLastError(); cout << "hello11" << endl << endl;
					throw SocketException(err, ErrorString(err));
				}

				if(sent != 1 + 8 + 8)
					throw System::Exception("The remote endpoint was closed the connection.");

#else

                Int32 sent = writev(socket->get_BaseSocket()->get_Handle(), this->posix_Scattered_IO, 3);

				if(sent == -1)
                    throw SocketException(errno, (const char *)_sys_errlist[errno]);

				if(sent != 1 + 8 + 8)
					throw System::Exception("The remote endpoint was closed the connection.");

#endif
			}
			//----------------------------------------------------
			void UploadClient::RemoveSocketFromArrayList(ParvicursorSocket *socket)
			{
				Int32 i = FindSocketIndex(socket);
				if(i != -1)
				{
					ParvicursorSocket *s = (ParvicursorSocket *)sockets->get_Value(i);
					try{ s->get_BaseSocket()->Shutdown(Both); }
					catch(...) {}
					try{ s->Close(); }
					catch(...) {}

					sockets->RemoveAt(i);
					delete s;
				}
			}
			//----------------------------------------------------
			Int32 UploadClient::FindSocketIndex(ParvicursorSocket *socket)
			{
				for(register Int32 i = 0 ; i < sockets->get_Count() ; i++)
					if((ParvicursorSocket *)sockets->get_Value(i) == socket)
						return i;
				return -1;
			}
			//----------------------------------------------------
			void UploadClient::Run()
			{
				this->completion_mutex = new Mutex();
				this->completion_cv = new ConditionVariable(this->completion_mutex);
				this->completion_met = false;

				worker = new Thread(this->Wrapper_To_Call_WorkerProc, (void *)this);
				worker->Start();
				worker->SetDetached();

				this->completion_mutex->Lock();
				while(!this->completion_met)
					this->completion_cv->Wait();
				this->completion_mutex->Unlock();
			}
			//----------------------------------------------------
			void UploadClient::WorkerExit()
			{
				Close();
			}
			//----------------------------------------------------
			void UploadClient::Close()
			{
				if(!this->closed)
				{
					this->closed = true;
					/*while(!exited)
					{
						_timeout++;
						if(_timeout == this.timeout)
						{
							try { worker.Abort(); } catch { }
							break;
						}
						Thread.Sleep(1);
					}*/

					if(this->circularBufferEnabledClientSide)
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


					if(this->sockets != null)
					{
						for(register Int32 i = 0 ; i < sockets->get_Count() ; i++)
						{
							ParvicursorSocket *s = (ParvicursorSocket *)sockets->get_Value(i);
							if(s->get_BaseSocket()->get_Connected())
							{
								try {s->get_BaseSocket()->Shutdown(Both);}
								catch(...) { }
							}
							//s->get_BaseSocket()->Shutdown(Both);
							try {s->Close();}
                            catch(...) { }
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
							try {this->socket->get_BaseSocket()->Shutdown(Both);}
							catch(...) { }
						}
						//s->get_BaseSocket()->Shutdown(Both);
						try {this->socket->Close();}
                        catch(...) { }
						delete this->socket;
						this->socket = null;
					}

					if(this->seekValue != null)
					{
						delete this->seekValue;
						this->seekValue = null;
					}

					if(this->readValue != null)
					{
						delete this->readValue;
						this->readValue = null;
					}

					if(this->buffer != null)
					{
						delete this->buffer;
						this->buffer = null;
					}

					/********   X-xDFS Zero-Copy Cleanup  *************************/
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

                    this->posix_map_buffer = null;

                    if(this->isNormalZeroCopySupported && this->isClientSideZeroCopyEnabled)
                        TransferFromDiskToSocket_ZeroCopySplice(-1, null, -1, -1, this->pipes_socket_to_disk, -1, this->pipes_socket_to_disk_is_defined, true);

#endif

					/****************************************************************/

					if(this->fs != null)
					{
						fs->Close();
						delete this->fs;
						this->fs = null;
					}


					if(this->written_mutex != null)
					{
						this->written_mutex->Unlock();
						delete this->written_mutex;
						this->written_mutex = null;
					}
					if(this->worker != null)
					{
						printf("helloggggggggggggggggggggg\n");
						if(this->errors->get_Count() > 0)
						{
							this->completion_mutex->Lock();
							this->completion_met = true;
							this->completion_cv->Signal();
						}
						/*this->completion_mutex->Lock();
						this->completion_met = true;
						this->completion_cv->Signal();*/
						this->completion_mutex->Unlock();
						pthread_cancel(this->worker->thread);
					}

					if(worker != null)
					{
						worker->Abort();
						delete worker;
						worker = null;
					}
					//delete this->nc;

				}
			}
			//----------------------------------------------------
			void *UploadClient::Wrapper_To_Call_WorkerProc(void* pt2Object)
			{
				pthread_setcanceltype(PTHREAD_CANCEL_ENABLE, null);
				pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, null);
				UploadClient* mySelf = (UploadClient*) pt2Object;
				// call member
				mySelf->WorkerProc();
				return pt2Object;
			}
			//----------------------------------------------------
			void *UploadClient::Wrapper_To_Call_DiskThreadWorkerProc(void* pt2Object)
			{
				pthread_setcanceltype(PTHREAD_CANCEL_ENABLE, null);
				pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, null);

				UploadClient* mySelf = (UploadClient*) pt2Object;
				// call member
				mySelf->DiskThreadWorkerProc_Producer();
				return pt2Object;
			}
			//----------------------------------------------------
			void UploadClient::DiskThreadWorkerProc_Producer()
			{
				Int32 ret;
				while(true)
				{
					ret = buffer_device();

					switch(ret)
					{
						case Parvicursor::Shared::Enums::xDFS::BufferIsFull:
						{
							//printf("Parvicursor::Shared::Enums::xDFS::BufferIsFull\n");
							Thread::Sleep(1);
							break;
						}
						case Parvicursor::Shared::Enums::xDFS::BufferIsEmpty:
						{
							//printf("Parvicursor::Shared::Enums::xDFS::BufferIsEmpty\n");
							break;
						}
						case Parvicursor::Shared::Enums::xDFS::EndOfFileButBufferNotEmpty:
						{
							printf("Parvicursor::Shared::Enums::xDFS::EndOfFileButBufferNotEmpty:Producer:ThreadExit\n");
							return ;
						}
						case Parvicursor::Shared::Enums::xDFS::EndOfFileAndBufferIsEmpty:
						{
							printf("Parvicursor::Shared::Enums::xDFS::EndOfFileAndBufferIsEmpty:Producer:ThreadExit\n");
							return ;
						}
						default:
						{
							break;
						}
					}
					//Thread::Sleep(1);
				}
			}
			//----------------------------------------------------
			CircularBufferState UploadClient::buffer_device()
			{
				//cout << "buffer_device" << endl;
				Int32 temp = (this->end + this->blockSize) % this->BUFFER_SIZE;
				//Use modulo as a trick to wrap around the end of the buffer back to the beginning
				if (temp != this->start)
				{
					pthread_mutex_lock(&this->mutex);

					_n = this->fs->Read(this->circular_buffer, this->end, this->blockSize);
					if(_n <= 0)
					{
						this->currentOffset = -1;
						this->nextOffset = -1;
						if (this->end != this->start)
						{
							pthread_mutex_unlock(&this->mutex);
							return Parvicursor::Shared::Enums::xDFS::EndOfFileButBufferNotEmpty;
						}
						else
						{
							pthread_mutex_unlock(&mutex);
							return Parvicursor::Shared::Enums::xDFS::EndOfFileAndBufferIsEmpty;
						}
					}
					/*if(n != BLOCK_SIZE)
						printf("n: %d\n", n);*/

					this->blocks++;
					this->currentOffset = this->nextOffset;
					this->nextOffset += this->_n;
					this->_last_n = this->_n;
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
			CircularBufferState UploadClient::unbuffer_device(ParvicursorSocket *socket)
			{
				if (this->end != this->start)
				{
					//char temp = circular_buffer[start];
					if(this->currentOffset == -1 && this->blocks == 1)
					{
						this->__n = _last_n;
						/****enable this section after X-DotSec implementation****/

						/*socket->WriteByte(1);// signaling the server about arrival of new file block.
						GetBytesOfLongNumberForFTSMMode(this->seekValue, this->offsetSeek);
						GetBytesOfLongNumberForFTSMMode(this->readValue, this->n);

						this->socket->Write(this->seekValue, 0, 8);
						this->socket->Write(this->readValue, 0, 8);
						//printf("seekValue: %ld readValue: %ld\n", this->offsetSeek, (long)this->n);
						//std::cout << "seekValue: " << this->offsetSeek << " readValue: " << this->n << std::endl;

						this->socket->Write(this->buffer, 0, (Int32)this->n);*/

						/****disable this section after X-DotSec implementation****/
						/*******for X-xDFS Scattered I/O transfers*****/
						pthread_mutex_lock(&this->mutex);
						this->circular_buffer += start;
						memmove(this->buffer, this->circular_buffer, this->__n);
						this->circular_buffer -= start;
						this->blocks--;
						pthread_mutex_unlock(&this->mutex);

						this->DoScatteredIO_For_CircularBuffer(socket, this->buffer);
						//printf("offset: %d\n", this->_offset);
						/************************************************/
						//fsConsumer->Write(circular_buffer, start, _last_n); //printf("\n\n\n\kljbkldsfjgk\n\n\n\n");
						return Parvicursor::Shared::Enums::xDFS::EndOfFileAndBufferIsEmpty;
					}
					else
					{
						this->__n = this->blockSize;
						/****enable this section after X-DotSec implementation****/

						/*socket->WriteByte(1);// signaling the server about arrival of new file block.
						GetBytesOfLongNumberForFTSMMode(this->seekValue, this->offsetSeek);
						GetBytesOfLongNumberForFTSMMode(this->readValue, this->n);

						this->socket->Write(this->seekValue, 0, 8);
						this->socket->Write(this->readValue, 0, 8);
						//printf("seekValue: %ld readValue: %ld\n", this->offsetSeek, (long)this->n);
						//std::cout << "seekValue: " << this->offsetSeek << " readValue: " << this->n << std::endl;

						this->socket->Write(this->buffer, 0, (Int32)this->n);*/

						/****disable this section after X-DotSec implementation****/
						/*******for X-xDFS Scattered I/O transfers*****/
						pthread_mutex_lock(&this->mutex);
						this->circular_buffer += start;
						memmove(this->buffer, this->circular_buffer, this->__n);
						this->circular_buffer -= start;
						this->blocks--;
						pthread_mutex_unlock(&this->mutex);

						this->DoScatteredIO_For_CircularBuffer(socket, this->buffer);
						//printf("offset: %d\n", _offset);
						this->_offset += this->__n;
						/************************************************/


						//fsConsumer->Write(circular_buffer, start, this->blockSize);

						this->start = (this->start + this->blockSize) % this->BUFFER_SIZE;
						//return(temp);
						return Parvicursor::Shared::Enums::xDFS::BufferOperationCompleted;
					}
				}
				//otherwise, the buffer is empty; return an error code
				else
				{
					if(this->currentOffset == -1)
						return Parvicursor::Shared::Enums::xDFS::EndOfFileAndBufferIsEmpty;
					else
						return Parvicursor::Shared::Enums::xDFS::BufferIsEmpty;
				}
			}
			//----------------------------------------------------
			void UploadClient::DoScatteredIO_For_CircularBuffer(ParvicursorSocket *socket, char *buffer)
			{
				GetBytesOfLongNumberForFTSMMode(this->seekValue, this->_offset);
				GetBytesOfLongNumberForFTSMMode(this->readValue, this->__n);

#if defined WIN32 || WIN64

				this->win_Scattered_IO[3].buf = buffer;
				this->win_Scattered_IO[3].len = (UInt32)this->__n;

				DWORD sent;

				if(WSASend(socket->get_BaseSocket()->get_Handle(), this->win_Scattered_IO, 4, &sent, 0, null, null) == SOCKET_ERROR)
				{
					int err = WSAGetLastError(); cout << "DoScatteredIO_For_CircularBuffer: " << ErrorString(err) << endl << endl;
					throw SocketException(err, ErrorString(err));
				}

				if(sent != 1 + 8 + 8 + (DWORD)this->__n)
					throw System::Exception("The remote endpoint was closed the connection.");

#else

				this->posix_Scattered_IO[3].iov_base = buffer;
				this->posix_Scattered_IO[3].iov_len = (UInt32)this->__n;


                Int32 sent = writev(socket->get_BaseSocket()->get_Handle(), this->posix_Scattered_IO, 4);

				if(sent == -1)
                    throw SocketException(errno, (const char *)_sys_errlist[errno]);

				if(sent != 1 + 8 + 8 + (Int32)this->__n)
					throw System::Exception("The remote endpoint was closed the connection.");

#endif
			}
			//----------------------------------------------------
			//private void PublicKeyAuthentication(Socket sock);
			//private byte[] AuthenticationHeaderBuilder(string username, string password);
   			//----------------------------------------------------
	   };
   };
};
//**************************************************************************************************************//
