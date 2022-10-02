#include "SessionFileStreamRequest.h"

//**************************************************************************************************************//
namespace Parvicursor
{
   namespace xDFS
   {
		//----------------------------------------------------
		SessionFileStreamRequest::SessionFileStreamRequest(ParvicursorSocket *socket, const FileStreamInfo &info)
		{
			// Here, check the sent paramters from client.
			this->socket = socket;
			this->info = (FileStreamInfo *)&info;
			this->buffer = new char[info.blockSize];
			paramBuffer = new char[sizeof(Int64)*10]; // This buffer is used as the container for method parameter except for orginal data. We here assume maximim 10 parametrs.
		}
		//----------------------------------------------------
		void SessionFileStreamRequest::Run()
		{
			try { socket->WriteNoException(); }
            catch(...) {/*pthread_mutex_unlock(&mutex);*/ WorkerExit(); return ;}
			printf("---------------------------------------------------------\n");


			printf("New xDFS::xDFSM file transfer session.\n");
			//printf("blockSize: %d filename: %s\n", info->blockSize, info->remoteFilename.get_BaseStream());
			if(!OpenFileHandle()) { WorkerExit(); return ; }

			while(true)
			{
				Methods method;
				try { method = (Methods)socket->ReadByte(); }
				catch(System::Exception &e)
				{
					//printf("Exception occured: %s\n", e.get_Message().get_BaseStream());
					WorkerExit(); return ;
				}
				catch(...) { WorkerExit(); return ; }

				switch(method)
				{
					case WriteMethod:
						if(!Write()) { WorkerExit(); return ; }
						//fs.Flush();
						//printf("I'm WriteMethod\n");
						break;
					case ReadMethod:
						if(!Read()) { WorkerExit(); return ; }
						//fs.Flush();
						//printf("I'm ReadMethod\n");
						break;
					case CloseMethod:
						Close();
						//printf("I'm CloseMethod\n");
						WorkerExit();
						return;
					case SetLengthMethod:
						if(!SetLength()) { WorkerExit(); return ; }
						//printf("I'm SetLengthMethod\n");
						break;
					case LockMethod:
						if(!Lock()) { WorkerExit(); return ; }
						//printf("I'm LockMethod\n");
						break;
					case UnLockMethod:
						if(!UnLock()) { WorkerExit(); return ; }
						//printf("I'm UnLockMethod\n");
						break;
					case GetLengthMethod:
						if(!get_Length()) { WorkerExit(); return ; }
						//printf("I'm GetLengthMethod\n");
						break;
					case GetPositionMethod:
						if(!get_Position()) { WorkerExit(); return ; }
						//printf("I'm GetPositionMethod\n");
						break;
					case SeekMethod:
						if(!Seek()) { WorkerExit(); return ; }
						//printf("I'm SeekMethod\n");
						break;
					case FlushMethod:
						if(!Flush()) { WorkerExit(); return ; }
						//printf("I'm FlushMethod\n");
						break;
					default:
						if(!SendUnsupportedMethod()) { WorkerExit(); return ; }
						//printf("I'm UnsupportedMethod\n");
						break;
				}
			}
		}
		//----------------------------------------------------
		bool SessionFileStreamRequest::SendUnsupportedMethod()
		{
			try
			{
				System::Exception e = System::Exception("Unsupported Parvicursor::Shared::Enums::xDFS::Methods");
				try { socket->WriteException(e); } catch(...) { }
				return true;
			}
			catch(SocketException &)
			{
				return false;
			}
			catch(System::Exception &e)
			{
				try { socket->WriteException(e); } catch(...) { }
				return false;
			}
			catch(...)
			{
				System::Exception e = System::Exception("Unknown excpetion");
				try { socket->WriteException(e); } catch(...) { }
				return false;
			}
		}
		//----------------------------------------------------
		bool SessionFileStreamRequest::Write()
		{
			try
			{
				socket->Read(paramBuffer, 0, sizeof(Int32));
				Int32 count = Parvicursor_GetInt32NumberFromBytes((Byte *)paramBuffer);
				if(count > info->blockSize || count <= 0)
				{
					ArgumentOutOfRangeException e = ArgumentOutOfRangeException("count must be less than blockSize and greater than zero");
					try { socket->WriteException(e); } catch(...) { }
					return false;
				}
				socket->Read(buffer, 0, count);
				fs->Write(buffer, 0, count);
				socket->WriteNoException();
				return true;
			}
			catch(SocketException &)
			{
				return false;
			}
			catch(System::Exception &e)
			{
				try { socket->WriteException(e); } catch(...) { }
				return false;
			}
			catch(...)
			{
				System::Exception e = System::Exception("Unknown excpetion");
				try { socket->WriteException(e); } catch(...) { }
				return false;
			}
		}
		//----------------------------------------------------
		bool SessionFileStreamRequest::Read()
		{
			//printf("hellox1\n");
			try
			{
				socket->Read(paramBuffer, 0, sizeof(Int32));
				//printf("hellox2\n");
				Int32 count = Parvicursor_GetInt32NumberFromBytes((Byte *)paramBuffer);
				if(count > info->blockSize || count <= 0)
				{
					ArgumentOutOfRangeException e = ArgumentOutOfRangeException("count must be less than blockSize and greater than zero");
					try { socket->WriteException(e); } catch(...) { }
					return false;
				}
				Int32 n = fs->Read(buffer, 0, count);
				socket->WriteNoException();
				Parvicursor_GetBytesFromInt32Number(paramBuffer, n);
				socket->Write(paramBuffer, 0, sizeof(Int32));
				if(n != 0)
					socket->Write(buffer, 0, n);

				return true;
			}
			catch(SocketException &)
			{
				printf("SocketException occured\n");
				return false;
			}
			catch(System::Exception &e)
			{
				printf("Exception occured: %s\n", e.get_Message().get_BaseStream());
				try { socket->WriteException(e); } catch(...) { }
				return false;
			}
			catch(...)
			{
				printf("Unknown excpetion\n");
				System::Exception e = System::Exception("Unknown excpetion");
				try { socket->WriteException(e); } catch(...) { }
				return false;
			}
		}
		//----------------------------------------------------
		bool SessionFileStreamRequest::get_Length()
		{
			try
			{
				Int64 length = fs->get_Length();
				socket->WriteNoException();

				Parvicursor_GetBytesFromInt64Number(paramBuffer, length);
				socket->Write(paramBuffer, 0, sizeof(Int64));
				return true;
			}
			catch(SocketException &)
			{
				return false;
			}
			catch(System::Exception &e)
			{
				try { socket->WriteException(e); } catch(...) { }
				return false;
			}
			catch(...)
			{
				System::Exception e = System::Exception("Unknown excpetion");
				try { socket->WriteException(e); } catch(...) { }
				return false;
			}
		}
		//----------------------------------------------------
		bool SessionFileStreamRequest::get_Position()
		{
			try
			{
				Int64 position = fs->get_Position();
				socket->WriteNoException();

				Parvicursor_GetBytesFromInt64Number(paramBuffer, position);
				socket->Write(paramBuffer, 0, sizeof(Int64));
				return true;
			}
			catch(SocketException &)
			{
				return false;
			}
			catch(System::Exception &e)
			{
				try { socket->WriteException(e); } catch(...) { }
				return false;
			}
			catch(...)
			{
				System::Exception e = System::Exception("Unknown excpetion");
				try { socket->WriteException(e); } catch(...) { }
				return false;
			}
		}
		//----------------------------------------------------
		bool SessionFileStreamRequest::SetLength()
		{
			try
			{
				socket->Read(paramBuffer, 0, sizeof(Int64));
				Int64 value = Parvicursor_GetInt64NumberFromBytes((Byte *)paramBuffer);
				fs->SetLength(value);
				socket->WriteNoException();
				return true;
			}
			catch(SocketException &)
			{
				return false;
			}
			catch(System::Exception &e)
			{
				try { socket->WriteException(e); } catch(...) { }
				return false;
			}
			catch(...)
			{
				System::Exception e = System::Exception("Unknown excpetion");
				try { socket->WriteException(e); } catch(...) { }
				return false;
			}
		}
		//----------------------------------------------------
		bool SessionFileStreamRequest::Lock()
		{
			try
			{
				socket->Read(paramBuffer, 0, sizeof(Int64) + sizeof(Int64));
				Int64 position = Parvicursor_GetInt64NumberFromBytes((Byte *)paramBuffer);
				paramBuffer += sizeof(Int64);
				Int64 length = Parvicursor_GetInt64NumberFromBytes((Byte *)paramBuffer);
				paramBuffer -= sizeof(Int64);
				fs->Lock(position, length);
				socket->WriteNoException();
				return true;
			}
			catch(SocketException &)
			{
				return false;
			}
			catch(System::Exception &e)
			{
				try { socket->WriteException(e); } catch(...) { }
				return false;
			}
			catch(...)
			{
				System::Exception e = System::Exception("Unknown excpetion");
				try { socket->WriteException(e); } catch(...) { }
				return false;
			}
		}
		//----------------------------------------------------
		bool SessionFileStreamRequest::UnLock()
		{
			try
			{
				socket->Read(paramBuffer, 0, sizeof(Int64) + sizeof(Int64));
				Int64 position = Parvicursor_GetInt64NumberFromBytes((Byte *)paramBuffer);
				paramBuffer += sizeof(Int64);
				Int64 length = Parvicursor_GetInt64NumberFromBytes((Byte *)paramBuffer);
				paramBuffer -= sizeof(Int64);
				fs->UnLock(position, length);
				socket->WriteNoException();
				return true;
			}
			catch(SocketException &)
			{
				return false;
			}
			catch(System::Exception &e)
			{
				try { socket->WriteException(e); } catch(...) { }
				return false;
			}
			catch(...)
			{
				System::Exception e = System::Exception("Unknown excpetion");
				try { socket->WriteException(e); } catch(...) { }
				return false;
			}
		}
		//----------------------------------------------------
		bool SessionFileStreamRequest::Seek()
		{
			try
			{
				socket->Read(paramBuffer, 0, sizeof(Int64) + sizeof(Byte));
				Int64 offset = Parvicursor_GetInt64NumberFromBytes((Byte *)paramBuffer);
				paramBuffer += sizeof(Int64);
				SeekOrigin origin = (SeekOrigin)(*paramBuffer);
				paramBuffer -= sizeof(Int64);
				Int64 newOffset = fs->Seek(offset, origin);
				socket->WriteNoException();

				Parvicursor_GetBytesFromInt64Number(paramBuffer, newOffset);
				socket->Write(paramBuffer, 0, sizeof(Int64));
				return true;
			}
			catch(SocketException &)
			{
				return false;
			}
			catch(System::Exception &e)
			{
				try { socket->WriteException(e); } catch(...) { }
				return false;
			}
			catch(...)
			{
				System::Exception e = System::Exception("Unknown excpetion");
				try { socket->WriteException(e); } catch(...) { }
				return false;
			}
		}
		//----------------------------------------------------
		bool SessionFileStreamRequest::Flush()
		{
			try
			{
				fs->Flush();
				socket->WriteNoException();
				return true;
			}
			catch(SocketException &)
			{
				return false;
			}
			catch(System::Exception &e)
			{
				try { socket->WriteException(e); } catch(...) { }
				return false;
			}
			catch(...)
			{
				System::Exception e = System::Exception("Unknown excpetion");
				try { socket->WriteException(e); } catch(...) { }
				return false;
			}
		}
		//----------------------------------------------------
		void SessionFileStreamRequest::Close()
		{
			try { fs->Flush(); fs->Close(); }
			catch(...) { }

			delete fs;
			fs = null;
		}
		//----------------------------------------------------
		bool SessionFileStreamRequest::OpenFileHandle()
		{
			try
			{
				fs = new FileStream(info->remoteFilename, info->mode, info->access, 9);
				return true;
			}
			catch(System::Exception &e)
			{
				try { socket->WriteException(e); } catch(...) { }
				return false;
			}
			catch(...)
			{
			    System::Exception e = System::Exception("Unknown excpetion");
				try { socket->WriteException(e); } catch(...) { }
				return false;
			}
		}
		//----------------------------------------------------
		void SessionFileStreamRequest::WorkerExit()
		{
			if(socket != null)
			{
                // try { socket->get_BaseSocket()->Shutdown(Both); } catch(...) { }

                try { socket->Close(); } catch(...) { }
				delete socket;
				socket = null;
			}

			if(fs != null)
			{
				try 
				{
					fs->Flush();
					fs->Close();
				}
				catch(...) {}
				delete fs;
			}

			if(buffer != null)
			{
				delete buffer;
				buffer = null;
			}

			if(paramBuffer != null)
			{
				delete paramBuffer;
				paramBuffer = null;
			}

			return ;
		}
   		//----------------------------------------------------
   };
};
//**************************************************************************************************************//
