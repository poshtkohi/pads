#include "stdafx.h"

#include "OneStreamUpload.h"

//**************************************************************************************************************//
namespace Parvicursor
{
   namespace xDFS
   {
	   namespace xDFSClients
	   {
 			//----------------------------------------------------
			OneStreamUpload::OneStreamUpload(const String &readFilename, const String &remoteFilename, Int32 parallel, Int32 blockSize, Int32 tcpBufferSize, const String &xDFSServerAddress, const NetworkCredential &nc, bool secure/*, ref RijndaelEncryption rijndael*/, bool memmoryToMemoryTests, ArrayList *errors, bool IsClientSideZeroCopyEnabled, bool IsServerSideZeroCopyEnabled)
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

				if(tcpBufferSize <= 0)
					throw ArgumentOutOfRangeException("tcpBufferSize parameter can not be negative or zero.");


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
				this->written = -1;
				this->offsetSeek = 0;
				this->last_n = 0;
				this->n = 0;
				this->pageSize = 64 * 1024;


#if defined WIN32 || WIN64

				SYSTEM_INFO SysInfo;
				GetSystemInfo(&SysInfo);
				this->pageSize = SysInfo.dwAllocationGranularity;

				this->win_file_handle = null;
				this->win_map_handle = null;
				this->win_map_buffer = null;
				this->isNormalZeroCopySupported = true; // false for memory-mapped file. true for TransmiteFile.
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
                    throw Exception((const char *)_sys_errlist[errno]);

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

                        if(n1 >= 2 && n2 >= 6 && n3 >= 17) //The splice() system call first appeared in Linux 2.6.17.
                            this->isNormalZeroCopySupported = true;

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


				/*sock = new System::Net::Sockets::Socket(InterNetwork, Stream, tcp);
				//IPEndPoint hostEndPoint = new IPEndPoint(IPAddress.Any, 2799);
				IPEndPoint hostEndPoint(IPAddress::get_Any(), 2799);
				//sock.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, 1);
				sock->Bind(hostEndPoint);
				sock->Listen(1000);*/

			}
			//----------------------------------------------------
			OneStreamUpload::~OneStreamUpload()
			{
				if(!this->closed)
				{
					Close();
					//printf("hellow world\n");
				}
			}
			//----------------------------------------------------
			Int64 OneStreamUpload::get_CurrentTransferredBytes()
			{
				if(!this->closed)
				{
					return this->written;
				}
				return -1;
			}
			//----------------------------------------------------
			Int64 OneStreamUpload::get_FileSize()
			{
				if(!this->closed)
				{
					if(this->memmoryToMemoryTests)
						return 0xFFFFFFFFFFFFFFFFLL;
					else
						if(this->fs != null )
							return this->fileSize;
				}

				return -1;
			}
			//----------------------------------------------------
			/*const DateTime &OneStreamUpload::get_StartTime()
			{
				return (const DateTime &)DateTime::get_Now();
			}
			//----------------------------------------------------
			const DateTime &OneStreamUpload::get_EndTime()
			{
				return (const DateTime &)DateTime::get_Now();
			}*/
			//----------------------------------------------------
			void OneStreamUpload::WorkerProc()
			{
				//Int32 receiveTcpBufferSize = 1; //this->tcpBufferSize;
				try
				{
					IPHostEntry hostEntry = Dns::Resolve(xDFSServerAddress);
					IPAddress ip = hostEntry.get_AddressList(0);
					IPEndPoint inp(ip, __xDFS_Server_Default_Port__);

					//Serializer se(this->readFilename.get_Length() + 1 + this->guid.get_Length() + 1 + sizeof(Int64) + 2 * sizeof(Int32));
					Serializer se(512);
					se.Write(this->guid);
					se.Write(this->remoteFilename);
					se.Write<Int64>(this->fileSize);
					se.Write<Int32>(this->parallel);
					se.Write<Int32>(this->blockSize);
					se.Write<Int32>(this->tcpBufferSize);
					se.Write<bool>(this->isServerSideZeroCopyEnabled);

					if(this->parallel == 1)
					{
						Socket *sock = new Socket(ip.get_AddressFamily(), Stream, tcp);

#if !defined WIN32 || WIN64
                this->tcpBufferSize = this->tcpBufferSize / 2;
#endif

#if defined Enable_TCP_Window_Scale
						//sock->SetReceiveTcpWindowSize(this->tcpBufferSize);//sock->SetReceiveTcpWindowSize(this->tcpBufferSize);//////must delete
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
						catch(Exception &e)
						{
							if(this->errors != null)
								this->errors->Add(new Exception(e.get_Message()));
						}
						catch(...)
						{
							if(this->errors != null)
									this->errors->Add(new Exception("Unknown exception was occured"));

						}
					}
					else
					{
						for(Int32 i = 0 ; i < this->parallel ; i++)
						{

							Socket *sock = new Socket(ip.get_AddressFamily(), Stream, tcp);

#if defined Enable_TCP_Window_Scale
							//sock->SetReceiveTcpWindowSize(this->tcpBufferSize);//////must delete
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
							catch(Exception &e)
							{
								if(this->errors != null)
									this->errors->Add(new Exception(e.get_Message()));

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
										this->errors->Add(new Exception("Unknown exception was occured"));

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

					if(!this->isClientSideZeroCopyEnabled)
						this->buffer = new char[this->blockSize];
					else
						this->buffer = null;

					this->offsetSeek = 0;
					this->n = 0;
					this->written = 0;
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
								this->written = -1;
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
								this->ZeroCopyTransfer(this->socket);
								if(offsetSeek == -1)
								{
									goto End;
								}
								//std::cout << "seekValue: " << this->offsetSeek << " readValue: " << this->n << std::endl;
							}

							written += n;
							socket->CheckExceptionResponse();
							//Thread.Sleep(1);

							if(ended)
							{
								break;
							}
						}
						if(closed)
						{
							this->written = -1;
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
							Thread::Sleep(1);
							//if(!this->alWrite_enable)
								MakeReadyWriteSocketArrayList();
						//here:
							//readysocks = ::select(highsock + 1, &alRead, &alWrite, (fd_set *)null, null);
							if(this->alWrite_enable)
							{
								readysocks = ::select(highsock + 1, (fd_set *)null, &alWrite, (fd_set *)null, &timeoutWrite);
								if(readysocks == -1)
								{
									//cout << ErrorString(WSAGetLastError()) << endl << endl;
									goto End;
								}
								if(readysocks == 0)
									//continue;
									goto ReadableSockets;
								//-----Writable Sockets----------------------------------------
								for(Int32 i = 0 ; i < this->sockets->get_Count() ; i++)
								{
									sock = (ParvicursorSocket *)sockets->get_Value(i);
									if(FD_ISSET(sock->get_BaseSocket()->get_Handle(), &alWrite))
									{
										//this->alWrite_enable = false;
										try
										{
											if(!this->isClientSideZeroCopyEnabled)
											{

												ReadFromFile();

												if(offsetSeek == -1)
												{
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

											sock->set_IsCheckedExceptionResponse(NotDone);
											written += n;
											//socket->CheckExceptionResponse();
										}
										catch(Exception &e)
										{
											if(this->errors != null)
												this->errors->Add(new Exception(e.get_Message()));

											RemoveSocketFromArrayList(sock);
											continue;
										}
										catch(...)
										{
											if(this->errors != null)
												this->errors->Add(new Exception("Unknown exception"));

											RemoveSocketFromArrayList(sock);
											continue;
										}

										//continue;
									}
								}
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

								for(Int32 j = 0 ; j < this->sockets->get_Count() ; j++)
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
											continue;
										}
										catch(...)
										{
											if(this->errors != null)
												this->errors->Add(new Exception("Unknown exception"));

											RemoveSocketFromArrayList(sock);
											continue;
										}

										//continue;
									}
								}
							}
							//-------------------------------------------------------------
						}
					}
				}
				catch(Exception &e)
				{
					if(this->errors != null)
						this->errors->Add(new Exception(e.get_Message()));

					WorkerExit();
					exited = true;
					return;
				}
				catch(...)
				{
					if(this->errors != null)
							this->errors->Add(new Exception("Unknown exception was occured"));

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
							for(Int32 i = 0 ; i < this->sockets->get_Count() ; i++)
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
				catch(Exception &e)
				{
					if(this->errors != null)
						this->errors->Add(new Exception(e.get_Message()));
				}
				catch(...)
				{
					if(this->errors != null)
							this->errors->Add(new Exception("Unknown exception was occured"));
				}
				//-----------------------------------------------------------------------------

				this->written = -1;

				WorkerExit();
				exited = true;


			}
			//----------------------------------------------------
			void OneStreamUpload::ReadFromFile()
			{
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
			void OneStreamUpload::ZeroCopyTransfer(ParvicursorSocket *socket)
			{

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
					if(chunkOffset != 0)
						this->win_map_buffer += chunkOffset;

					/****enable this section after X-DotSec implementation****/
					//socket->Write(this->win_map_buffer, 0, (Int32)this->n);
					/****disable this section after X-DotSec implementation****/
					this->DoScatteredIO(socket, this->win_map_buffer);

					if(chunkOffset != 0)
						this->win_map_buffer -= chunkOffset;

					if(!UnmapViewOfFile(this->win_map_buffer))
					{
						int err = GetLastError();
						throw IOException(ErrorString(err));
					}
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
			void OneStreamUpload::SendOneExceptionToAllSocketsAndClose(Exception &e)
			{
				return ;
			}
			//----------------------------------------------------
			void OneStreamUpload::SendExceptionToOneSocketAndClose(Exception &e, ParvicursorSocket *socket)
			{
				try { socket->WriteException(e); RemoveSocketFromArrayList(socket); }
				catch(...) { RemoveSocketFromArrayList(socket); }
			}
			//----------------------------------------------------
			void OneStreamUpload::MakeReadyReadSocketArrayList()
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
			void OneStreamUpload::MakeReadyWriteSocketArrayList()
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

				this->alWrite_enable = false;
				for (Int32 i = 0; i < this->sockets->get_Count() ; i++)
				{
					s = (ParvicursorSocket *)sockets->get_Value(i);
					handle = s->get_BaseSocket()->get_Handle();
					if(handle != 0)
					{
						if(s->get_IsCheckedExceptionResponse() == Done || s->get_IsCheckedExceptionResponse() == FirstTime)
						{
							FD_SET(handle, &alWrite);
							this->alWrite_enable = true;
						}

						if (handle > highsock)
							highsock = handle;
					}
				}

				return ;
			}
			//----------------------------------------------------
			void OneStreamUpload::DoScatteredIO(ParvicursorSocket *socket, char *buffer)
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
						throw Exception("The remote endpoint was closed the connection.");

#else

					this->posix_Scattered_IO[3].iov_base = buffer;
					this->posix_Scattered_IO[3].iov_len = (UInt32)this->n;


                    Int32 sent = writev(socket->get_BaseSocket()->get_Handle(), this->posix_Scattered_IO, 4);

					if(sent == -1)
                        throw IOException((const char *)_sys_errlist[errno]);

					if(sent != 1 + 8 + 8 + (Int32)this->n)
						throw Exception("The remote endpoint was closed the connection.");

#endif
			}
			//----------------------------------------------------
			void OneStreamUpload::DoScatteredIO(ParvicursorSocket *socket)
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
						throw Exception("The remote endpoint was closed the connection.");

#else


                    Int32 sent = writev(socket->get_BaseSocket()->get_Handle(), this->posix_Scattered_IO, 3);

					if(sent == -1)
                        throw IOException((const char *)_sys_errlist[errno]);

					if(sent != 1 + 8 + 8)
						throw Exception("The remote endpoint was closed the connection.");
#endif
			}
			//----------------------------------------------------
			void OneStreamUpload::RemoveSocketFromArrayList(ParvicursorSocket *socket)
			{
				Int32 i = FindSocketIndex(socket);
				if(i != -1)
				{
					ParvicursorSocket *s = (ParvicursorSocket *)sockets->get_Value(i);
					s->get_BaseSocket()->Shutdown(Both);
					s->Close();

					sockets->RemoveAt(i);
					delete s;
				}
			}
			//----------------------------------------------------
			Int32 OneStreamUpload::FindSocketIndex(ParvicursorSocket *socket)
			{
				for(Int32 i = 0 ; i < sockets->get_Count() ; i++)
					if((ParvicursorSocket *)sockets->get_Value(i) == socket)
						return i;
				return -1;
			}
			//----------------------------------------------------
			void OneStreamUpload::Run()
			{
				worker = new Thread(this->Wrapper_To_Call_WorkerProc, (void *)this);
				worker->Start();
				worker->Join();
			}
			//----------------------------------------------------
			void OneStreamUpload::WorkerExit()
			{
				Close();
			}
			//----------------------------------------------------
			void OneStreamUpload::Close()
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
					if(this->sockets != null)
					{
						for(Int32 i = 0 ; i < sockets->get_Count() ; i++)
						{
							ParvicursorSocket *s = (ParvicursorSocket *)sockets->get_Value(i);
							if(s->get_BaseSocket()->get_Connected())
							{
								try {s->get_BaseSocket()->Shutdown(Both);}
								catch(...) { }
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
							try {this->socket->get_BaseSocket()->Shutdown(Both);}
							catch(...) { }
						}
						//s->get_BaseSocket()->Shutdown(Both);
						this->socket->Close();
						delete this->socket;
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
					/*if(this->win_map_buffer != null)
						UnmapViewOfFile(this->win_map_buffer);*/

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

					if(worker != null)
					{
						worker->Abort();
						delete worker;
					}
					//delete this->nc;

				}
			}
			//----------------------------------------------------
			void *OneStreamUpload::Wrapper_To_Call_WorkerProc(void* pt2Object)
			{
				OneStreamUpload* mySelf = (OneStreamUpload*) pt2Object;
				// call member
				mySelf->WorkerProc();
				return pt2Object;
			}
			//----------------------------------------------------
			//private void PublicKeyAuthentication(Socket sock);
			//private byte[] AuthenticationHeaderBuilder(string username, string password);
   			//----------------------------------------------------
	   };
   };
};
//**************************************************************************************************************//
