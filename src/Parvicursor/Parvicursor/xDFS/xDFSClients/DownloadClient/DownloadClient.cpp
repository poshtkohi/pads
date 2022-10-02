#include "DownloadClient.h"

//Whenever the xDFS is conveied to ParvicursorLib, similiar to ParvicursorSocket, apply the inlining for FileStream class.
//**************************************************************************************************************//
namespace Parvicursor
{
   namespace xDFS
   {
	   namespace xDFSClients
	   {
 			//----------------------------------------------------
			DownloadClient::DownloadClient(const String &writeFilename, const String &remoteFilename, Int32 parallel, Int32 blockSize, Int32 tcpBufferSize, const String &xDFSServerAddress, const NetworkCredential &nc, bool secure/*, ref RijndaelEncryption rijndael*/, bool memmoryToMemoryTests, ArrayList *errors, bool IsClientSideZeroCopyEnabled, bool IsServerSideZeroCopyEnabled)
			{

				if(writeFilename.get_BaseStream() == null)
					throw ArgumentNullException("writeFilename is a null reference.");

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
				this->writeFilename = writeFilename;
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

				this->fs = new FileStream(this->writeFilename, OpenOrCreate, ReadWrite, 9); // FileShare
				//this->fileSize = -1;//this->fs->get_Length();

				this->guid = Guid::NewGuid();

				this->isServerSideZeroCopyEnabled = IsServerSideZeroCopyEnabled;
				this->isClientSideZeroCopyEnabled = IsClientSideZeroCopyEnabled;

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
			DownloadClient::~DownloadClient()
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
			Int64 DownloadClient::get_CurrentTransferredBytes()
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
			Int64 DownloadClient::get_FileSize()
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
			/*const DateTime &DownloadClient::get_StartTime()
			{
				return (const DateTime &)DateTime::get_Now();
			}
			//----------------------------------------------------
			const DateTime &DownloadClient::get_EndTime()
			{
				return (const DateTime &)DateTime::get_Now();
			}*/
			//----------------------------------------------------
			void DownloadClient::WorkerProc()
			{
				//Int32 receiveTcpBufferSize = 1; //this->tcpBufferSize;
				try
				{
					IPHostEntry hostEntry = Dns::Resolve2(xDFSServerAddress);
					IPAddress ip = hostEntry.get_AddressList(0);
					IPEndPoint inp(ip, __xDFS_Server_Default_Port__);

					//Serializer se(this->writeFilename.get_Length() + 1 + this->guid.get_Length() + 1 + sizeof(Int64) + 2 * sizeof(Int32));
					Serializer se(512);
					se.Write(this->guid);
					se.Write(this->remoteFilename);
					//se.Write<Int64>(this->fileSize);
					se.Write<Int32>(this->parallel);
					se.Write<Int32>(this->blockSize);
					se.Write<Int32>(this->tcpBufferSize);
					se.Write<bool>(this->isServerSideZeroCopyEnabled);

					if(this->parallel == 1)
					{
						Socket *sock = new Socket(ip.get_AddressFamily(), Stream, tcp);

						if(sock == null)
							throw ObjectDisposedException("Could not instantiate from Socket class.");

#if !defined WIN32 || WIN64
                this->tcpBufferSize = this->tcpBufferSize / 2;
#endif

#if defined Enable_TCP_Window_Scale
						//sock->SetReceiveTcpWindowSize(this->tcpBufferSize);//sock->SetReceiveTcpWindowSize(this->tcpBufferSize);//////must delete
						//sock->SetReceiveTcpWindowSize(0);//////must delete
						sock->SetSendTcpWindowSize(this->tcpBufferSize);
#endif

						socket = new ParvicursorSocket(sock);

						char *buff = null;

						try
						{
							socket->get_BaseSocket()->Connect(inp);
							socket->WriteByte((char)SingleFileTransferDownloadFromClient);
							socket->WriteObject(se.get_BaseBuffer(), se.get_BaseBufferSize());
							socket->CheckExceptionResponse();

							int objSize = 0;
							buff = socket->ReadObject(objSize);
							DeSerializer de(buff, objSize);
							this->fileSize = de.Read<Int64>();

#if defined Enable_TCP_Window_Scale
							//sock->SetReceiveTcpWindowSize(this->tcpBufferSize);//sock->SetReceiveTcpWindowSize(this->tcpBufferSize);//////must delete
							cout << "GetSendTcpWindowSize: " << sock->GetSendTcpWindowSize() << " GetReceiveTcpWindowSize(): " << sock->GetReceiveTcpWindowSize() << endl << endl;
#endif
						}
						catch(System::Exception &e)
						{
							if(buff != null)
							{
								delete buff;
								buff = null;
							}
							if(this->errors != null)
								this->errors->Add(new System::Exception(e.get_Message()));
						}
						catch(...)
						{
							if(buff != null)
							{
								delete buff;
								buff = null;
							}
							if(this->errors != null)
									this->errors->Add(new System::Exception("An unknown exception was occured"));

						}
					}
					else
					{
						for(register Int32 i = 0 ; i < this->parallel ; i++)
						{

							Socket *sock = new Socket(ip.get_AddressFamily(), Stream, tcp);

#if defined Enable_TCP_Window_Scale
							//sock->SetReceiveTcpWindowSize(this->tcpBufferSize);//////must delete
							sock->SetReceiveTcpWindowSize(this->tcpBufferSize);
							//sock->SetSendTcpWindowSize(0);//////must delete
#endif

							if(sock == null)
								throw ObjectDisposedException("Could not instantiate from Socket class.");

							ParvicursorSocket *s = new ParvicursorSocket(sock);
							this->sockets->Add(s);
							try
							{
								s->get_BaseSocket()->Connect(inp);

								s->WriteByte((char)SingleFileTransferDownloadFromClient);
								s->WriteObject(se.get_BaseBuffer(), se.get_BaseBufferSize());
								s->CheckExceptionResponse();

							}
							catch(System::Exception &e)
							{
								if(this->errors != null)
									this->errors->Add(new System::Exception(e.get_Message()));

								if(i == 0)
									goto End;
								else
									continue;
							}
							catch(...)
							{
								if(this->errors != null)
										this->errors->Add(new System::Exception("An unknown exception was occured"));

								if(i == 0)
									goto End;
								else
									continue;
							}
						}

						if(this->sockets != null)
						{
							if(this->sockets->get_Count() != 0)
							{
								char *buff = null;
								try
								{
									ParvicursorSocket *_s = (ParvicursorSocket *)sockets->get_Value(0);
									int objSize = 0;
									buff = _s->ReadObject(objSize);
									DeSerializer de(buff, objSize);
									this->fileSize = de.Read<Int64>();
								}
								catch(System::Exception &e)
								{
									if(buff != null)
									{
										delete buff;
										buff = null;
									}
									if(this->errors != null)
										this->errors->Add(new System::Exception(e.get_Message()));
								}
								catch(...)
								{
									if(buff != null)
									{
										delete buff;
										buff = null;
									}
									if(this->errors != null)
											this->errors->Add(new System::Exception("An unknown exception was occured"));

								}
							}
						}
					}
					//WorkerExit();
					//return;
					if(this->errors != null)
						if(this->errors->get_Count() != 0)
							goto End;

					//cout << this->fileSize << endl ;

					/*this->fs->SetLength(0);
					this->fs->SetLength(this->fileSize);*/

#if defined __Scaling__

					if(!this->memmoryToMemoryTests)
					{
						this->fs->SetLength(0);
						this->fs->SetLength(this->fileSize);
					}

#else
					if(!this->memmoryToMemoryTests)
					{
						if(this->isClientSideZeroCopyEnabled)
						{
							this->fs->SetLength(0);
							this->fs->SetLength(this->fileSize);
						}
						else
							fs->SetLength(0);//printf("file opened\n");
					}
#endif


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
										PAGE_READWRITE,
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
					//ParvicursorSocket socket = new ParvicursorSocket(reader, writer);
					//t1 = DateTime.Now;

					this->seekValue = new char[8];
					this->readValue = new char[8];

					int readsocks;
					struct timeval timeout;  // Timeout for select()
					timeout.tv_sec = 0;
					timeout.tv_usec = 0;

					if(!this->isClientSideZeroCopyEnabled)
						this->buffer = new char[this->blockSize];
					else
						this->buffer = null;

					this->offsetSeek = 0;
					this->n = 0;
					this->written_mutex->Lock();
					this->written = 0;
					this->written_mutex->Unlock();

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
					//-----------------------------------------------------------------------------
					//cout << "hello world !" << endl;
					//return ;
					if(this->parallel == 1)
					{
						while(true)
						{
							//sock = (ParvicursorSocket *)sockets->get_Value(0);
							if(!this->isClientSideZeroCopyEnabled)
							{
								try
								{
									if(ReadFileBlockFTSMMode(this->socket))
									{
										//sock->WriteNoException();
										//RemoveSocketFromArrayList(this->socket);
										goto End;
										//break;
									}
								}

								catch(System::Exception &e)
								{
									if(this->errors != null)
										this->errors->Add(new System::Exception(e.get_Message())); cout << "error is here" << endl;

									goto End;
								}
								catch(...)
								{
									if(this->errors != null)
											this->errors->Add(new System::Exception("An unknown exception was occured"));

									goto End;
								}

								try { WriteToFile(); }
								catch(System::Exception &e)
								{
									if(this->errors != null)
										this->errors->Add(new System::Exception(e.get_Message()));

									goto End;
								}
								catch(...)
								{
									if(this->errors != null)
											this->errors->Add(new System::Exception("An unknown exception was occured"));

									goto End;
								}
							}
							else
							{
								try
								{
									if(ZeroCopyTransfer(this->socket))
									{
										goto End;
									}
								}
								catch(System::Exception &e)
								{
									if(this->errors != null)
										this->errors->Add(new System::Exception(e.get_Message()));

									goto End;
								}
								catch(...)
								{
									if(this->errors != null)
											this->errors->Add(new System::Exception("An unknown exception was occured"));

									goto End;
								}
							}


#if defined With_Error_Recovery_Support

							try { this->socket->WriteNoException(); }
							catch(System::Exception &e)
							{
								if(this->errors != null)
									this->errors->Add(new System::Exception(e.get_Message()));

								goto End;
							}
							catch(...)
							{
								if(this->errors != null)
										this->errors->Add(new Exception("An unknown exception was occured"));

								goto End;
							}

#endif

						}
					}
					else
					{
						while(true)
						{
							if(sockets->get_Count() == 0 //|| written >= info.FileSize
							)
							goto End; //break;
	//				here:
						MakeReadSocketArrayList();
					//here:
						readsocks = ::select(highsock + 1, &alRead, (fd_set *)null, (fd_set *)null, null);
						if(readsocks == -1)
						{
#if defined WIN32 || WIN64

							int err = WSAGetLastError();
							throw SocketException(err, ErrorString(err));

#else
							throw SocketException(errno, (const char *)_sys_errlist[errno]);

#endif
						}

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
								if(!this->isClientSideZeroCopyEnabled)
								{
									try
									{
										if(ReadFileBlockFTSMMode(sock))
										{
											RemoveSocketFromArrayList(sock);
											goto here1;
										}
									}
									catch(ObjectDisposedException &e) { RemoveSocketFromArrayList(sock); if(this->errors != null){this->errors->Add(new System::Exception(e.get_Message()));} goto here1; }
									catch(SocketException &e) { RemoveSocketFromArrayList(sock); if(this->errors != null){this->errors->Add(new System::Exception(e.get_Message()));} goto here1; }
									catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, sock) ; if(this->errors != null){this->errors->Add(new System::Exception(e.get_Message()));} goto here1; }
									catch(...) { System::Exception e("An unknown exception was occured"); SendExceptionToOneSocketAndClose(e, sock) ; if(this->errors != null){this->errors->Add(new System::Exception(e.get_Message()));} goto here1; }
									try { WriteToFile(); }
									catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, sock) ; if(this->errors != null){this->errors->Add(new System::Exception(e.get_Message()));} goto here1; }
									catch(...) { System::Exception e("An unknown exception was occured"); SendExceptionToOneSocketAndClose(e, sock) ; goto here1; }
								}
								else
								{
									try
									{
										if(ZeroCopyTransfer(sock))
										{
											RemoveSocketFromArrayList(sock);
											goto here1;
											//continue;
										}
									}
									catch(ObjectDisposedException &e) { RemoveSocketFromArrayList(sock); if(this->errors != null){this->errors->Add(new System::Exception(e.get_Message()));} goto here1; }
									catch(SocketException &e) { RemoveSocketFromArrayList(sock); if(this->errors != null){this->errors->Add(new System::Exception(e.get_Message()));} goto here1; }
									catch(System::Exception &e) { SendExceptionToOneSocketAndClose(e, sock) ; if(this->errors != null){this->errors->Add(new System::Exception(e.get_Message()));} goto here1; }
									catch(...) { System::Exception e("An unknown exception was occured"); SendExceptionToOneSocketAndClose(e, sock) ; if(this->errors != null){this->errors->Add(new System::Exception(e.get_Message()));} goto here1; }
								}
#if defined With_Error_Recovery_Support
								try { sock->WriteNoException(); }
								catch(System::Exception &e) { RemoveSocketFromArrayList(sock); if(this->errors != null){this->errors->Add(new Exception(e.get_Message()));} goto here1; }
								catch(...) { RemoveSocketFromArrayList(sock); if(this->errors != null){this->errors->Add(new Exception("Unknown excpetion"));} goto here1;  }
#endif
							}
						}
						//Thread::Sleep(_timeout);
					}
					}


					this->written_mutex->Lock();
					this->written = -1;
					this->written_mutex->Unlock();

					goto End;

				}
				catch(System::Exception &e)
				{
					if(this->errors != null)
						this->errors->Add(new System::Exception(e.get_Message()));

					goto End;
				}
				catch(...)
				{
					if(this->errors != null)
							this->errors->Add(new System::Exception("An unknown exception was occured"));

					goto End;
				}

			End:

				WorkerExit();
				exited = true;
				return;
			}
			//----------------------------------------------------
			void DownloadClient::MakeReadSocketArrayList()
			{
				/* FD_ZERO() clears out the fd_set called socks, so that
					it doesn't contain any file descriptors. */
				FD_ZERO(&alRead);
				/* Loops through all the possible connections and adds
					those sockets to the fd_set */

				for (register Int32 i = 0 ; i < this->sockets->get_Count() ; i++)
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
			bool DownloadClient::ZeroCopyTransfer(ParvicursorSocket *socket)
			{
				/****enable this section after X-DotSec implementation****/
				//n = socket->ReadByte();

				/****disable this section after X-DotSec implementation****/
				/*******for X-xDFS Scattered I/O transfers*****/
				DoScatteredIO(socket);
				/**********************************************************/

				if(this->n == 0)
					return true; // meaning end read.
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

				if(this->read_val > this->blockSize)
					throw ArgumentOutOfRangeException("readValue in the header can not be greater than blockSize.");

				this->k++;
				/*if(memmoryToMemoryTests)
					return;*/////

				//fs->Seek(seekValue, Begin);

#if defined WIN32 || WIN64

				if(this->offsetSeek != this->lastOffset + this->lastLength)
					j++;

				if(!this->isNormalZeroCopySupported)
				{
					Int64 chunkOffset = this->offsetSeek % this->pageSize;
					Int64 chunkStart = this->offsetSeek - chunkOffset;

					//fs->Seek(this->seekValue, Begin);

					this->win_map_buffer = (char *)MapViewOfFile(win_map_handle,
																FILE_MAP_WRITE,
																(Int32)((chunkStart & 0xFFFFFFFF00000000LL) > 32),
																(Int32)(chunkStart & 0x00000000FFFFFFFFLL),
																(Int32)(this->read_val + chunkOffset));

					if(this->win_map_buffer == null)
						throw IOException(ErrorString(GetLastError()));

					char *temp = this->win_map_buffer;

					/*if(chunkOffset != 0)
						temp += chunkOffset;*/

					n = socket->Read(temp, (Int32)chunkOffset, (Int32)this->read_val);

					/*if(chunkOffset != 0)
						temp -= chunkOffset;*/

					/*if(!FlushViewOfFile(this->win_map_buffer, (Int32)this->readValue))
						throw IOException(ErrorString(GetLastError()));*/

					/*if(chunkOffset != 0)
						this->win_map_buffer -= chunkOffset;*/

					if(!UnmapViewOfFile(this->win_map_buffer))
						throw IOException(ErrorString(GetLastError()));

					this->lastOffset = this->offsetSeek;
					this->lastLength = this->read_val;
					this->written_mutex->Lock();
					this->written += this->read_val;
					this->written_mutex->Unlock();

					this->win_map_buffer = null;

					if(n != this->read_val)
						throw ArgumentException("Bad format for FileTransferModeHeader.");
				}

#else
				if(!this->isNormalZeroCopySupported)
				{
					if(this->offsetSeek != this->lastOffset + this->lastLength)
					{
						//fs->Seek(this->seekValue, Begin);
						//fs.Write(buffer, 0, (int)readValue);
						j++;
					}

					Int64 chunkOffset = this->offsetSeek % this->pageSize;
					Int64 chunkStart = this->offsetSeek - chunkOffset;

					//this->n = this->readValue + chunkOffset;
					//fs->Seek(this->seekValue, Begin);
					this->n = this->read_val + chunkOffset;
					this->posix_map_buffer = (char *)mmap64(null, (Int32)this->n, PROT_WRITE, MAP_SHARED, this->fs->get_Handle(), chunkStart);

					if(this->posix_map_buffer == MAP_FAILED)
						throw IOException((const char *)_sys_errlist[errno]);

					//cout << "pageSize: " << this->pageSize << endl << endl;
					/*if(chunkOffset != 0)
						this->posix_map_buffer += chunkOffset;*/
					n = socket->Read(this->posix_map_buffer, (Int32)chunkOffset, (Int32)this->read_val);

					this->lastOffset = this->offsetSeek;
					this->lastLength = this->read_val;
					this->written_mutex->Lock();
					this->written += this->read_val;
					this->written_mutex->Unlock();

					/*if(!UnmapViewOfFile(this->win_map_buffer))
						throw IOException(ErrorString(GetLastError()));*/

					if(n != this->read_val)
						throw ArgumentException("Bad format for FileTransferModeHeader.");

					this->n = this->read_val + chunkOffset;

					if(munmap(this->posix_map_buffer, (Int32)this->n) == -1)
						throw IOException((const char *)_sys_errlist[errno]);

					this->posix_map_buffer = null;

				}
				else
				{//cout << "hjghjg" << endl;
					if(TransferFromSocketToDisk_ZeroCopySplice(this->fs->get_Handle(), &this->offsetSeek, socket->get_BaseSocket()->get_Handle(), this->read_val, this->pipes_socket_to_disk, this->pageSize, this->pipes_socket_to_disk_is_defined, false) < 0)
						throw IOException((const char *)_sys_errlist[errno]);
				}
#endif

				return false;
			}
			//----------------------------------------------------
			bool DownloadClient::ReadFileBlockFTSMMode(ParvicursorSocket *socket)
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

				if(this->n == 0)/* || this->read_val == 0)*/
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
				if(this->read_val > this->blockSize)
					throw ArgumentOutOfRangeException("readValue in the header can not be greater than blockSize.");
				//byte[] buffer = new byte[readValue];
				//cout << this->read_val << endl;
				n = socket->Read(this->buffer, 0, (Int32)this->read_val);
				if(n != this->read_val)
					throw ArgumentException("Bad format for FileTransferModeHeader.");
				return false;
				//}
			}
			//----------------------------------------------------
			/*int DownloadClient::ReadFromOriginialSocket(ParvicursorSocket *socket, char array[], int offset, int count)
			{
				return 0;
			}*/
			//----------------------------------------------------
			void DownloadClient::DoScatteredIO(ParvicursorSocket *socket)
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
				this->offsetSeek = GetLongNumberFromBytesForFTSMMode((byte *)this->seekValue);
				this->read_val = GetLongNumberFromBytesForFTSMMode((byte *)this->readValue);
			}
			//----------------------------------------------------
			void DownloadClient::WriteToFile()
			{
				//Console.WriteLine("offset: {0}, length: {1}", (int)seekValue, (int)readValue);
				k++;
				/*if(memmoryToMemoryTests)
					return;*/////
				if(this->offsetSeek != this->lastOffset + this->lastLength)
				{
					fs->Seek(this->offsetSeek, Begin);
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
				fs->Write(this->buffer, 0, (Int32)this->read_val);
				this->lastOffset = this->offsetSeek;
				this->lastLength = this->read_val;
				this->written_mutex->Lock();
				this->written += this->read_val;
				this->written_mutex->Unlock();
				//Console.WriteLine("offset: {0}, seekNumber: {1}, written: {2}", seekValue , j, written);
			}
			//----------------------------------------------------
			void DownloadClient::RemoveSocketFromArrayList(ParvicursorSocket *socket)
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
			void DownloadClient::SendOneExceptionToAllSockets(System::Exception &e)
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
			void DownloadClient::SendExceptionToOneSocketAndClose(System::Exception &e, ParvicursorSocket *socket)
			{
				try { socket->WriteException(e); RemoveSocketFromArrayList(socket); }
				catch(...) { RemoveSocketFromArrayList(socket); }
			}
			//----------------------------------------------------
			Int32 DownloadClient::FindSocketIndex(ParvicursorSocket *socket)
			{
				for(register Int32 i = 0 ; i < sockets->get_Count() ; i++)
					if((ParvicursorSocket *)sockets->get_Value(i) == socket)
						return i;
				return -1;
			}
			//----------------------------------------------------
			void DownloadClient::Run()
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
			void DownloadClient::WorkerExit()
			{
				Close();
			}
			//----------------------------------------------------
			void DownloadClient::Close()
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
						this->fs->Close();
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
			void *DownloadClient::Wrapper_To_Call_WorkerProc(void* pt2Object)
			{
				DownloadClient* mySelf = (DownloadClient*) pt2Object;
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
