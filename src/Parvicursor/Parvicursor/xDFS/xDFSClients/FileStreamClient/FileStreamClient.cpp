/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "FileStreamClient.h"

//Whenever the xDFS is conveyed to ParvicursorLib, similar to ParvicursorSocket, apply the inlining for FileStream class.
//**************************************************************************************************************//
namespace Parvicursor
{
   namespace xDFS
   {
	   namespace xDFSClients
	   {
			//----------------------------------------------------
			FileStreamClient::FileStreamClient(const String &remoteFilename, FileMode mode, FileAccess access, /*FileShare share,*/ Int32 blockSize, const String &xDFSServerAddress, const NetworkCredential &nc, bool secure)
			{
				if(remoteFilename.get_BaseStream() == null)
					throw ArgumentNullException("remoteFilename is a null reference.");

				if(xDFSServerAddress.get_BaseStream() == null)
					throw ArgumentNullException("xDFSServerAddress is a null reference.");

				if(&nc == null)
					throw ArgumentNullException("nc is a null reference.");

				if(blockSize <= 0)
					throw ArgumentOutOfRangeException("blockSize parameter can not be negative or zero.");

				this->remoteFilename = remoteFilename;
				this->mode = mode;
				this->access = access;
				this->blockSize = blockSize;
				this->xDFSServerAddress = xDFSServerAddress;
				this->nc = (NetworkCredential *)&nc;
				this->secure = secure;
				socket = null;
				paramBuffer = new char[sizeof(Int64)*3]; // This buffer is used as the container for method parameter except for orginal data. We here assume maximim 3 parametrs.
				//hostEntry = null;
				guid = Guid::NewGuid();
				isConnected = false;
				disposed = false;
			}
			//----------------------------------------------------
			FileStreamClient::~FileStreamClient()
			{
				if(!disposed)
				{
					Close();
					//disposed = true;
				}
			}
			//----------------------------------------------------
			void FileStreamClient::ConnectToServer()
			{
				if(!isConnected)
				{
					IPHostEntry  hostEntry = Dns::Resolve2(xDFSServerAddress);
					//IPAddress ip = hostEntry->get_AddressList(0);
					IPAddress ip = hostEntry.get_AddressList(0);
					IPEndPoint inp(ip, __xDFS_Server_Default_Port__);
					Socket *sock = new Socket(ip.get_AddressFamily(), Stream, tcp);

					//Serializer se(this->readFilename.get_Length() + 1 + this->guid.get_Length() + 1 + sizeof(Int64) + 2 * sizeof(Int32));
					Serializer se(512);
					se.Write(guid);
					se.Write(remoteFilename);
					se.Write<Int32>(blockSize);
					se.Write<Byte>(mode);
					se.Write<Byte>(access);

					if(secure)
						socket = new ParvicursorSocket(sock, ParvicursorSocket::DefaultCryptographyBufferSize);//
					else
						socket = new ParvicursorSocket(sock);//

					socket->get_BaseSocket()->Connect(inp);
					socket->WriteByte((char)FileStreamFromClient);
					//printf("se.get_BaseBufferSize(): %d\n", se.get_BaseBufferSize());//
					socket->WriteObject(se.get_BaseBuffer(), se.get_BaseBufferSize());
					socket->CheckExceptionResponse();
					isConnected = true;
				}
			}
			//----------------------------------------------------
			void FileStreamClient::Write(const char array[], Int32 offset, Int32 count)
			{
				if(disposed)
					throw ObjectDisposedException ("FileStreamClient", "FileStreamClient has been disposed");

				if(!isConnected)
					ConnectToServer();

				if(offset < 0 || count < 0) 
					throw ArgumentOutOfRangeException("offset or count can not be negative."); 

				if(count <= blockSize)
				{
					WriteInternal(array, offset, count);
					return ;
				}
				else
				{
					Int32 i = 0;
					Int32 a = count / blockSize;
					Int32 q = count % blockSize;
					while(true)
					{
						WriteInternal(array, blockSize*i, blockSize);
						i++;
						if(q != 0 && i == a)
						{
							WriteInternal(array, blockSize*i, q);
							break;
						}
						if(q == 0 && i == a)
							break;
					}
				}

				return;
			}
			//----------------------------------------------------
			void FileStreamClient::WriteInternal(const char array[], Int32 offset, Int32 count)
			{
				paramBuffer[0] = (Byte)WriteMethod;
				paramBuffer += sizeof(Byte);
				Parvicursor_GetBytesFromInt32Number(paramBuffer, count);
				paramBuffer -= sizeof(Byte);

				socket->Write(paramBuffer, 0, sizeof(Int32) + sizeof(Byte));
				socket->Write(array, offset, count);
				socket->CheckExceptionResponse();
			}
			//----------------------------------------------------
			Int32 FileStreamClient::Read(char array[], Int32 offset, Int32 count)
			{
				if(disposed)
					throw ObjectDisposedException ("FileStreamClient", "FileStreamClient has been disposed");

				//printf("hellox1\n");
				if(!isConnected)
					ConnectToServer();
				//printf("hellox2\n");

				if(offset < 0 || count < 0) 
					throw ArgumentOutOfRangeException("offset or count can not be negative."); 

				if(count <= blockSize)
				{
					//printf("hellox3\n");
					return ReadInternal(array, offset, count);
				}
				else
				{
					Int32 temp = 0;
					Int32 sum = 0;
					Int32 i = 0;
					Int32 a = count / blockSize;
					Int32 q = count % blockSize;
					while(true)
					{
						if((temp = ReadInternal(array, blockSize*i, blockSize)) == 0)
							break;
						sum += temp;
						i++;
						if(q != 0 && i == a)
						{
							if((temp = ReadInternal(array, blockSize*i, q)) == 0)
								break;
							sum += temp;
							break;
						}
						if(q == 0 && i == a)
							break;
					}
					return sum;
				}
			}
			//----------------------------------------------------
			Int32 FileStreamClient::ReadInternal(char array[], Int32 offset, Int32 count)
			{
				paramBuffer[0] = (Byte)ReadMethod;
				paramBuffer += sizeof(Byte);
				Parvicursor_GetBytesFromInt32Number(paramBuffer, count);
				paramBuffer -= sizeof(Byte);

				
				socket->Write(paramBuffer, 0, sizeof(Byte) + sizeof(Int32));

				socket->CheckExceptionResponse();
				//printf("hellox4\n");//
				socket->Read(paramBuffer, 0, sizeof(Int32) );

				Int32 newCount = Parvicursor_GetInt32NumberFromBytes((Byte *)paramBuffer);
				if(newCount == 0)
					return 0;

				if(newCount > count)
					throw ArgumentOutOfRangeException("The xDFS server replied with data greater than count");

				socket->Read(array, offset, newCount);
				return newCount;
			}
			//----------------------------------------------------
			void FileStreamClient::Flush()
			{
				if(disposed)
					throw ObjectDisposedException ("FileStreamClient", "FileStreamClient has been disposed");

				if(!isConnected)
					ConnectToServer();

				paramBuffer[0] = (Byte)FlushMethod;
				socket->Write(paramBuffer, 0, sizeof(Byte));
				socket->CheckExceptionResponse();
			}
			//----------------------------------------------------
			void FileStreamClient::Close()
			{
				if(disposed)
					throw ObjectDisposedException ("FileStreamClient", "FileStreamClient has been disposed");

				paramBuffer[0] = (Byte)CloseMethod;

				if(isConnected)
				{
					try { socket->Write(paramBuffer, 0, sizeof(Byte)); }
					catch(...) {}
				}

				if(socket != null)
				{
					//try { socket->get_BaseSocket()->Shutdown(Both); } catch (...){ }
					try { socket->Close(); } catch (...){ }
					delete socket;
					socket = null;
				}


				if(paramBuffer != null)
				{
					delete paramBuffer;
					paramBuffer = null;
				}

				/*if(hostEntry != null)
				{
					delete hostEntry;
					hostEntry = null;
				}*/

				disposed = true;
			}
			//----------------------------------------------------
			void FileStreamClient::SetLength(Int64 value)
			{
				if(disposed)
					throw ObjectDisposedException ("FileStreamClient", "FileStreamClient has been disposed");

				if(!isConnected)
					ConnectToServer();

				if(value < 0) 
					throw ArgumentOutOfRangeException("value can not be negative");

				paramBuffer[0] = (Byte)SetLengthMethod;
				paramBuffer += sizeof(Byte);
				Parvicursor_GetBytesFromInt64Number(paramBuffer, value);
				paramBuffer -= sizeof(Byte);
				socket->Write(paramBuffer, 0, sizeof(Byte) + sizeof(Int64));
				socket->CheckExceptionResponse();
			}
			//----------------------------------------------------
			void FileStreamClient::Lock(Int64 position, Int64 length)
			{
				if(disposed)
					throw ObjectDisposedException ("FileStreamClient", "FileStreamClient has been disposed");

				if(!isConnected)
					ConnectToServer();

				if(position < 0) 
					throw ArgumentOutOfRangeException("position can not be negative");

				if(length <= 0) 
					throw ArgumentOutOfRangeException("length can not be negative or zero");

				paramBuffer[0] = (Byte)LockMethod;
				paramBuffer += sizeof(Byte);
				Parvicursor_GetBytesFromInt64Number(paramBuffer, position);
				paramBuffer += sizeof(Int64);
				Parvicursor_GetBytesFromInt64Number(paramBuffer, length);
				paramBuffer -= ( sizeof(Byte) + sizeof(Int64) );
				socket->Write(paramBuffer, 0, sizeof(Byte) + sizeof(Int64) + sizeof(Int64));
				socket->CheckExceptionResponse();
			}
			//----------------------------------------------------
			void FileStreamClient::UnLock(Int64 position, Int64 length)
			{
				if(disposed)
					throw ObjectDisposedException ("FileStreamClient", "FileStreamClient has been disposed");

				if(!isConnected)
					ConnectToServer();

				if(position < 0) 
					throw ArgumentOutOfRangeException("position can not be negative");

				if(length <= 0) 
					throw ArgumentOutOfRangeException("length can not be negative or zero");

				paramBuffer[0] = (Byte)UnLockMethod;
				paramBuffer += sizeof(Byte);
				Parvicursor_GetBytesFromInt64Number(paramBuffer, position);
				paramBuffer += sizeof(Int64);
				Parvicursor_GetBytesFromInt64Number(paramBuffer, length);
				paramBuffer -= ( sizeof(Byte) + sizeof(Int64) );
				socket->Write(paramBuffer, 0, sizeof(Byte) + sizeof(Int64) + sizeof(Int64));
				socket->CheckExceptionResponse();
			}
			//----------------------------------------------------
			Int64 FileStreamClient::Seek(Int64 offset, SeekOrigin origin)
			{
				if(disposed)
					throw ObjectDisposedException ("FileStreamClient", "FileStreamClient has been disposed");

				if(!isConnected)
					ConnectToServer();

				if(offset < 0) 
					throw ArgumentOutOfRangeException("offset can not be negative");

				paramBuffer[0] = (Byte)SeekMethod;
				paramBuffer += sizeof(Byte);
				Parvicursor_GetBytesFromInt64Number(paramBuffer, offset);
				paramBuffer += sizeof(Int64);
				*paramBuffer = (Byte)origin;
				paramBuffer -= ( sizeof(Byte) + sizeof(Int64) );
				socket->Write(paramBuffer, 0, sizeof(Byte) + sizeof(Int64) + sizeof(Byte));
				socket->CheckExceptionResponse();

				socket->Read(paramBuffer, 0, sizeof(Int64));
				return Parvicursor_GetInt64NumberFromBytes((Byte *)paramBuffer);
			}
			//----------------------------------------------------
			Int64 FileStreamClient::get_Position()
			{
				if(disposed)
					throw ObjectDisposedException ("FileStreamClient", "FileStreamClient has been disposed");

				if(!isConnected)
					ConnectToServer();

				paramBuffer[0] = (Byte)GetPositionMethod;
				socket->Write(paramBuffer, 0, sizeof(Byte));
				socket->CheckExceptionResponse();

				socket->Read(paramBuffer, 0, sizeof(Int64));
				return Parvicursor_GetInt64NumberFromBytes((Byte *)paramBuffer);
			}
			//----------------------------------------------------
			Int64 FileStreamClient::get_Length()
			{
				if(disposed)
					throw ObjectDisposedException ("FileStreamClient", "FileStreamClient has been disposed");

				if(!isConnected)
					ConnectToServer();

				paramBuffer[0] = (Byte)GetLengthMethod;
				socket->Write(paramBuffer, 0, sizeof(Byte));
				socket->CheckExceptionResponse();

				socket->Read(paramBuffer, 0, sizeof(Int64));
				return Parvicursor_GetInt64NumberFromBytes((Byte *)paramBuffer);
			}
			//----------------------------------------------------
			bool FileStreamClient::get_CanRead()
			{
				if(disposed)
					throw ObjectDisposedException ("FileStreamClient", "FileStreamClient has been disposed");

				if(access == System::IO::Read)
					return true;
				else
					return false;
			}
			//----------------------------------------------------
			bool FileStreamClient::get_CanWrite()
			{
				if(disposed)
					throw ObjectDisposedException ("FileStreamClient", "FileStreamClient has been disposed");

				if(access == System::IO::Write)
					return true;
				else
					return false;
			}
			//----------------------------------------------------
			bool FileStreamClient::get_CanSeek()
			{
				if(disposed)
					throw ObjectDisposedException ("FileStreamClient", "FileStreamClient has been disposed");

				return true;
			}
			//----------------------------------------------------
			const String &FileStreamClient::get_Name()
			{
				if(disposed)
					throw ObjectDisposedException ("FileStreamClient", "FileStreamClient has been disposed");

				return remoteFilename;
			}
			//----------------------------------------------------
			//private void PublicKeyAuthentication(Socket sock);
			//private Byte[] AuthenticationHeaderBuilder(string username, string password);
   			//----------------------------------------------------
	   };
   };
};
//**************************************************************************************************************//
