#include "Server.h"

//**************************************************************************************************************//
namespace Parvicursor
{
   namespace xDFS
   {
 		//----------------------------------------------------
	    Server::Server(Socket *sock)
		{
			socket = new ParvicursorSocket(sock);//
		}
		//----------------------------------------------------
		void Server::Run()
		{
			Thread thread(this->Wrapper_To_Call_ProtocolInterpreter, (void *)this);
			thread.Start();
			thread.SetDetached();
		}
		//----------------------------------------------------
		void Server::ProtocolInterpreter()
		{
			printf("ProtocolInterpreter\n");//

			TransferChannelMode channelMode;

			try { channelMode = (TransferChannelMode)socket->ReadByte(); }
			catch(System::Exception &e)
			{
				printf("System::Exception occured: %s\n", e.get_Message().get_BaseStream());
				ThreadExit();
				return ;
			}
			catch(CryptoPP::Exception &ee)
			{
				//printf("CryptoPP::Exception occured: %s\n", ee.GetWhat());
				ThreadExit();
				return ;
			}
			catch(...) { ThreadExit(); return ;}

			//printf("channelMode: %d\n", channelMode);

			//_______________________________________________________

			if(channelMode == SingleFileTransferUploadFromClient)
			{
				printf("mode: SingleFileTransferUploadFromClient\n");

				char *buffer = null;
				FileTransferInfo info;

				try
				{
					int objSize = 0;
					buffer = this->socket->ReadObject(objSize);
					DeSerializer de(buffer, objSize);
					info.guid = de.Read();
					info.writeFilename = de.Read();
					info.fileSize = de.Read<Int64>();
					info.parallel = de.Read<Int32>();
					info.blockSize = de.Read<Int32>();
					info.tcpBufferSize = de.Read<Int32>();
					info.isServerSideZeroCopyEnabled = de.Read<bool>();
					info.circularBufferCount = de.Read<Int32>();

					//Int32 receiveTcpBufferSize = 1; //this->tcpBufferSize;

#if !defined WIN32 || WIN64
                    info.tcpBufferSize = info.tcpBufferSize / 2;
#endif

#if defined Enable_TCP_Window_Scale

					socket->get_BaseSocket()->SetReceiveTcpWindowSize(info.tcpBufferSize);
					//socket->get_BaseSocket()->SetSendTcpWindowSize(info.tcpBufferSize);//////must delete
					//socket->get_BaseSocket()->SetSendTcpWindowSize(0);//////must delete
#endif

					//pthread_mutex_lock(&_xDFS_server_mutex_);//
					//cout << "guid: " << info.guid.get_BaseStream() << "\nremoteFilename: " << info.writeFilename.get_BaseStream() << "\nfileSize: " << info.fileSize << "\nparallel: " << info.parallel << "\nblockSize: " << info.blockSize << "\ntcpBufferSize: " << info.tcpBufferSize << endl;
					//pthread_mutex_unlock(&_xDFS_server_mutex_);//

					if(buffer != null)
						delete buffer;

				}
				catch(...) { if(buffer != null) delete buffer; ThreadExit(); return;  }

				/*try { socket->WriteNoException(); }
                catch(...) {ThreadExit(); return;  }*/
				//ThreadExit(); return;

				pthread_mutex_lock(&_xDFS_server_mutex_);//
				bool _is = sessions->Contains(info.guid);
				pthread_mutex_unlock(&_xDFS_server_mutex_);//
				if(_is)
				{
					try
					{
						pthread_mutex_lock(&_xDFS_server_mutex_);//
						SessionClientUploadRequest *session = (SessionClientUploadRequest *)sessions->get_Item(info.guid);
						pthread_mutex_unlock(&_xDFS_server_mutex_);//

                        if(session != null)
							session->AddNewClientStream(socket);

						return;
					}
					catch(System::Exception &e)
					{
   						cout << "hello catch1: " << e.get_Message().get_BaseStream() << endl;
						//exit(-1);
						ThreadExit();
						return;
                    }
					catch(...)
					{
					    cout << "hello catch2: " << (const char *)_sys_errlist[errno] << endl;
						//exit(-1);
						ThreadExit();
						return;
					}
				}
				else
				{
					////pthread_mutex_lock(&_xDFS_server_mutex_);//
					//printf("hello2\n");
					////pthread_mutex_unlock(&_xDFS_server_mutex_);//
                    SessionClientUploadRequest *session = new SessionClientUploadRequest(socket/*, thread*/, info);
					try
					{
						////pthread_mutex_lock(&_xDFS_server_mutex_);//
						pthread_mutex_lock(&_xDFS_server_mutex_);//
						sessions->Add(info.guid, session);
						pthread_mutex_unlock(&_xDFS_server_mutex_);//

						session->Run();
						//pthread_mutex_lock(&_xDFS_server_mutex_);//
						//pthread_mutex_unlock(&_xDFS_server_mutex_);//
                        pthread_mutex_lock(&_xDFS_server_mutex_);
                        //cout << "last sessions->get_Count(): " << sessions->get_Count() << endl;
                        sessions->Remove(info.guid);
                        ////pthread_mutex_unlock(&_xDFS_server_mutex_);//
                        //cout << "next sessions->get_Count(): " << sessions->get_Count() << endl;
                        //cout << "\nhello world !!!\n" << endl;
                        pthread_mutex_unlock(&_xDFS_server_mutex_);

                        if(session != null)
                            delete session;

                        //free( thread);
                        //pthread_exit(null);

						return;
					}
					catch(System::Exception &e)
					{
                        if(session != null)
                            delete session;

   						cout << "hello catch3: " << e.get_Message().get_BaseStream() << endl;
						//exit(-1);
						ThreadExit();
						return;
                    }
					catch(...)
					{
                        if(session != null)
                            delete session;

						cout << "hello catch4: " << (const char *)_sys_errlist[errno] << endl;
						//exit(-1);
						ThreadExit();
						return;
					}
				}
				return ;
			}

			//_______________________________________________________

			if(channelMode == SingleFileTransferDownloadFromClient)
			{
				printf("mode: SingleFileTransferDownloadFromClient\n");

				char *buffer = null;
				FileTransferInfo info;

				try
				{
					int objSize = 0;
					buffer = this->socket->ReadObject(objSize);
					DeSerializer de(buffer, objSize);
					info.guid = de.Read();
					info.writeFilename = de.Read();
					//info.fileSize = de.Read<Int64>();
					info.parallel = de.Read<Int32>();
					info.blockSize = de.Read<Int32>();
					info.tcpBufferSize = de.Read<Int32>();
					info.isServerSideZeroCopyEnabled = de.Read<bool>();

					//Int32 receiveTcpBufferSize = 1; //this->tcpBufferSize;

#if !defined WIN32 || WIN64
                    info.tcpBufferSize = info.tcpBufferSize / 2;
#endif

#if defined Enable_TCP_Window_Scale

					//socket->get_BaseSocket()->SetReceiveTcpWindowSize(0);//////must delete
					//socket->get_BaseSocket()->SetSendTcpWindowSize(info.tcpBufferSize);//////must delete
					socket->get_BaseSocket()->SetSendTcpWindowSize(info.tcpBufferSize);
#endif

					//pthread_mutex_lock(&_xDFS_server_mutex_);//
					//cout << "guid: " << info.guid.get_BaseStream() << "\nremoteFilename: " << info.writeFilename.get_BaseStream() << "\nfileSize: " << info.fileSize << "\nparallel: " << info.parallel << "\nblockSize: " << info.blockSize << "\ntcpBufferSize: " << info.tcpBufferSize << endl;
					//pthread_mutex_unlock(&_xDFS_server_mutex_);//

					if(buffer != null)
						delete buffer;

				}
				catch(...) { if(buffer != null) delete buffer; ThreadExit(); return;  }

				/*try { socket->WriteNoException(); }
                catch(...) {ThreadExit(); return;  }*/
				//ThreadExit(); return;

				pthread_mutex_lock(&_xDFS_server_mutex_);//
				bool _is = sessions->Contains(info.guid);
				pthread_mutex_unlock(&_xDFS_server_mutex_);//
				if(_is)
				{
					try
					{
						pthread_mutex_lock(&_xDFS_server_mutex_);//
						SessionClientDownloadRequest *session = (SessionClientDownloadRequest *)sessions->get_Item(info.guid);
						pthread_mutex_unlock(&_xDFS_server_mutex_);//

                        if(session != null)
							session->AddNewClientStream(socket);

						return;
					}
					catch(System::Exception &e)
					{
   						cout << "hello catch1: " << e.get_Message().get_BaseStream() << endl;
						//exit(-1);
						ThreadExit();
						return;
                    }
					catch(...)
					{
					    cout << "hello catch2: " << (const char *)_sys_errlist[errno] << endl;
						//exit(-1);
						ThreadExit();
						return;
					}
				}
				else
				{
					////pthread_mutex_lock(&_xDFS_server_mutex_);//
					//printf("hello2\n");
					////pthread_mutex_unlock(&_xDFS_server_mutex_);//
                    SessionClientDownloadRequest *session = new SessionClientDownloadRequest(socket/*, thread*/, info);
					try
					{
						////pthread_mutex_lock(&_xDFS_server_mutex_);//
						pthread_mutex_lock(&_xDFS_server_mutex_);//
						sessions->Add(info.guid, session);
						pthread_mutex_unlock(&_xDFS_server_mutex_);//

						session->Run();
						//pthread_mutex_lock(&_xDFS_server_mutex_);//
						//pthread_mutex_unlock(&_xDFS_server_mutex_);//
                        pthread_mutex_lock(&_xDFS_server_mutex_);
                        //cout << "last sessions->get_Count(): " << sessions->get_Count() << endl;
                        sessions->Remove(info.guid);
                        ////pthread_mutex_unlock(&_xDFS_server_mutex_);//
                        //cout << "next sessions->get_Count(): " << sessions->get_Count() << endl;
                        //cout << "\nhello world !!!\n" << endl;
                        pthread_mutex_unlock(&_xDFS_server_mutex_);

                        if(session != null)
                            delete session;

                        //free( thread);
                        //pthread_exit(null);

						return;
					}
					catch(System::Exception &e)
					{
                        if(session != null)
                            delete session;

   						cout << "hello catch3: " << e.get_Message().get_BaseStream() << endl;
						//exit(-1);
						ThreadExit();
						return;
                    }
					catch(...)
					{
                        if(session != null)
                            delete session;

						cout << "hello catch4: " << (const char *)_sys_errlist[errno] << endl;
						//exit(-1);
						ThreadExit();
						return;
					}
				}
				return ;
			}

			//_______________________________________________________

			if(channelMode == xThreadRequestFromClient)
			{
				printf("mode: xThreadRequestFromClient\n");

				char *buffer = null;

				String guid;

				try
				{
					int objSize = 0;
					buffer = this->socket->ReadObject(objSize);
					DeSerializer de(buffer, objSize);
					guid = de.Read();
					printf("guid: %s\n", guid.get_BaseStream());//

					//Int32 receiveTcpBufferSize = 1; //this->tcpBufferSize;

					//pthread_mutex_lock(&_xDFS_server_mutex_);//
					//cout << "guid: " << info.guid.get_BaseStream() << "\nremoteFilename: " << info.writeFilename.get_BaseStream() << "\nfileSize: " << info.fileSize << "\nparallel: " << info.parallel << "\nblockSize: " << info.blockSize << "\ntcpBufferSize: " << info.tcpBufferSize << endl;
					//pthread_mutex_unlock(&_xDFS_server_mutex_);//

					if(buffer != null)
						delete buffer;

				}
				catch(...) { if(buffer != null) delete buffer; ThreadExit(); return;  }

				/*try { socket->WriteNoException(); }
                catch(...) {ThreadExit(); return;  }*/
				//ThreadExit(); return;

				pthread_mutex_lock(&_xDFS_server_mutex_);//
				bool _is = sessions->Contains(guid);
				pthread_mutex_unlock(&_xDFS_server_mutex_);//
				if(_is)
				{
					try
					{
						pthread_mutex_lock(&_xDFS_server_mutex_);//
						xThreadRequest *session = (xThreadRequest *)sessions->get_Item(guid);
						pthread_mutex_unlock(&_xDFS_server_mutex_);//

                        if(session != null)
							session->AddNewClientStream(socket);

						return;
					}
					catch(System::Exception &e)
					{
   						cout << "hello catch1: " << e.get_Message().get_BaseStream() << endl;
						//exit(-1);
						ThreadExit();
						return;
                    }
					catch(...)
					{
					    cout << "hello catch2: " << (const char *)_sys_errlist[errno] << endl;
						//exit(-1);
						ThreadExit();
						return;
					}
				}
				else
				{
                    xThreadRequest *session = new xThreadRequest(socket/*, thread*/, guid);
					try
					{
						pthread_mutex_lock(&_xDFS_server_mutex_);//
						sessions->Add(guid, session);
						pthread_mutex_unlock(&_xDFS_server_mutex_);//

						session->Run();
                        pthread_mutex_lock(&_xDFS_server_mutex_);
                        //cout << "last sessions->get_Count(): " << sessions->get_Count() << endl;
                        sessions->Remove(guid);
                        pthread_mutex_unlock(&_xDFS_server_mutex_);//
                        //cout << "next sessions->get_Count(): " << sessions->get_Count() << endl;
                        //cout << "\nhello world !!!\n" << endl;

                        if(session != null)
                            delete session;

						cout << "here" << endl;

						//Thread::Sleep(1000000);
						//Thread::Sleep(100000);
						//pthread_exit(null);
						return;

					}
					catch(System::Exception &e)
					{
                        if(session != null)
                            delete session;

   						cout << "hello catch3: " << e.get_Message().get_BaseStream() << endl;
						//exit(-1);
						ThreadExit();
						return;
                    }
					catch(...)
					{
                        if(session != null)
                            delete session;

						cout << "hello catch4: " << (const char *)_sys_errlist[errno] << endl;
						//exit(-1);
						ThreadExit();
						return;
					}
				}
				return ;
			}

			//_______________________________________________________

			if(channelMode == FileStreamFromClient)
			{
				printf("mode: FileStreamFromClient\n");

				char *buffer = null;

				FileStreamInfo info = FileStreamInfo();

				try
				{
					Int32 objSize = 0;
					buffer = this->socket->ReadObject(objSize);
					DeSerializer de(buffer, objSize);
					info.guid = de.Read();
					info.remoteFilename = de.Read();
					info.blockSize = de.Read<Int32>();
					info.mode = (FileMode)de.Read<byte>();
					info.access = (FileAccess)de.Read<byte>();

					//Int32 receiveTcpBufferSize = 1; //this->tcpBufferSize;

					//pthread_mutex_lock(&_xDFS_server_mutex_);//
					//cout << "guid: " << info.guid.get_BaseStream() << "\nremoteFilename: " << info.writeFilename.get_BaseStream() << "\nfileSize: " << info.fileSize << "\nparallel: " << info.parallel << "\nblockSize: " << info.blockSize << "\ntcpBufferSize: " << info.tcpBufferSize << endl;
					//pthread_mutex_unlock(&_xDFS_server_mutex_);//

					if(buffer != null)
						delete buffer;

				}
				catch(...) { if(buffer != null) delete buffer; ThreadExit(); return;  }

				/*try { socket->WriteNoException(); }
				catch(...) {ThreadExit(); return;  }*/
				//ThreadExit(); return;

				pthread_mutex_lock(&_xDFS_server_mutex_);//
				bool _is = sessions->Contains(info.guid);
				pthread_mutex_unlock(&_xDFS_server_mutex_);//
				if(_is)
				{
					try
					{
					    System::Exception e = System::Exception("The session with guid " + info.guid + " already exists");
						socket->WriteException(e);
						ThreadExit();
						return;
					}
					catch(System::Exception &e)
					{
						cout << "hello catch1: " << e.get_Message().get_BaseStream() << endl;
						//exit(-1);
						ThreadExit();
						return;
					}
					catch(...)
					{
						cout << "hello catch2: " << (const char *)_sys_errlist[errno] << endl;
						//exit(-1);
						ThreadExit();
						return;
					}
				}
				else
				{
					SessionFileStreamRequest *session = new SessionFileStreamRequest(socket, info);
					try
					{
						pthread_mutex_lock(&_xDFS_server_mutex_);//
						sessions->Add(info.guid, session);
						pthread_mutex_unlock(&_xDFS_server_mutex_);//

						session->Run();
						pthread_mutex_lock(&_xDFS_server_mutex_);
						sessions->Remove(info.guid);
						pthread_mutex_unlock(&_xDFS_server_mutex_);//

						if(session != null)
							delete session;

						return;

					}
					catch(System::Exception &e)
					{
						if(session != null)
							delete session;

						cout << "hello catch3: " << e.get_Message().get_BaseStream() << endl;
						//exit(-1);
						ThreadExit();
						return;
					}
					catch(...)
					{
						if(session != null)
							delete session;

						cout << "hello catch4: " << (const char *)_sys_errlist[errno] << endl;
						//exit(-1);
						ThreadExit();
						return;
					}
				}
				return ;
			}
			//_______________________________________________________

			//_______________________________________________________


			//_______________________________________________________

		}
 		//----------------------------------------------------
		// must implement
		/*FileTransferInfo Server::GetFileTransferInfo()
		{
		}*/
 		//----------------------------------------------------
		// must implement
		bool Server::PublicKeyAuthentication(Socket sock)
		{
			return false;
		}
 		//----------------------------------------------------
		void Server::ThreadExit()
		{
		    //return ;
			if(socket != null)
			{
				try
				{
					socket->Close();
					delete socket;
				}
				catch(...){}
				socket = null;
			}
			if(serverInstance != null)
			{
				delete ((Server *)serverInstance);
				serverInstance = null;
			}
			//pthread_exit(null);//
			//return ;
			//pthread_mutex_unlock(&_xDFS_server_mutex_);//
		}
		//----------------------------------------------------
		void *Server::Wrapper_To_Call_ProtocolInterpreter(void* pt2Object)
		{
			Server* mySelf = (Server*) pt2Object;
			// call member
			mySelf->ProtocolInterpreter();
			return pt2Object;
		}
   		//----------------------------------------------------
   };
};
//**************************************************************************************************************//
