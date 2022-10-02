#include "stdafx.h"

#include "ClientContext.h"

//**************************************************************************************************************//
namespace Parvicursor
{
   namespace HybirdConcurrencyPattern
   {
	   //----------------------------------------------------
		ClientContext::ClientContext(Socket *sock)
		{
			if(sock == null)
				throw ArgumentNullException("sock");

			//this->state = _Accepted;
			this->state = _OnReceive;
			this->sock = sock;
			this->buffer = null;
//			this->TotalBytes = 0;
			this->TotalBytes = 3;
			this->TransferedBytes = 0;
			this->wsabuf_index_changed = false;

			AllocateBuffer(ClientContext::DefaultBufferSize);

#if defined WIN32 || WIN64

			ZeroMemory(&this->overlapped, sizeof(OVERLAPPED));
#else
#endif

		}
		//----------------------------------------------------
		ClientContext::~ClientContext()
		{
			if(!this->disposed)
				Dispose();
		}
		//----------------------------------------------------
		void ClientContext::Dispose()
		{
			if(this->disposed)
				return ;

			this->disposed = true;

			this->state = _Closed;

			if(this->sock != null)
			{
				try{this->sock->Close();}
				catch(...) {}

				delete this->sock;
			}

			if(this->buffer != null)
			{
				delete this->buffer;
				this->buffer = null;
			}
		}
		//----------------------------------------------------
		ClientContextState ClientContext::get_State()
		{
			return this->state;
		}
		//----------------------------------------------------
		void ClientContext::set_State(ClientContextState state)
		{
			//this->TotalBytes = 0;
			this->TransferedBytes = 0;
			this->buffer_length_current = this->buffer_length_real;
			this->state = state;
		}
		//----------------------------------------------------
		Socket *ClientContext::get_Socket()
		{
			return this->sock;
		}
		//----------------------------------------------------
		Int32 ClientContext::OnReceive() // return -1 for error
		{
			printf("OnReceive\n");
			//this->buffer[1] = '\0';
			printf("buffer:%s, size: %d\n", this->buffer, this->get_TransferedBytes());

			this->buffer[0] = 'C';
			this->buffer[1] = 'D';
			this->buffer[2] = '\0';
			this->set_TotalBytes(3);
			this->set_State(_OnSend);
			return this->Send();
			//this->set_State(_OnSend);

			return 0;
		}
		//----------------------------------------------------
		Int32 ClientContext::OnSend() // return -1 for error
		{
			printf("OnSend\n");
			this->set_TotalBytes(3);
			this->set_State(_OnReceive);
			//this->Send();
			return 0;
		}
		//----------------------------------------------------
		void ClientContext::OnAccept()
		{
			printf("OnAccept\n");
		}
		//----------------------------------------------------
		char *ClientContext::get_Buffer()
		{
			return this->buffer;
		}
		//----------------------------------------------------
		void ClientContext::AllocateBuffer(Int32 size)
		{
			if(size < 0)
					throw ArgumentOutOfRangeException("size", "size is less than 0");

			if(this->buffer != null)
				delete this->buffer;

			this->buffer = new char[size];
			this->buffer_length_real = size;
			this->buffer_length_current = size;


#if defined WIN32 || WIN64
			this->wsabuf.buf = this->buffer;
			this->wsabuf.len = this->buffer_length_current;
#else

#endif
		}
		//----------------------------------------------------
		void ClientContext::set_Buffer(char *buffer)
		{
			this->buffer = buffer;
		}
		//----------------------------------------------------
		Int32 ClientContext::get_BufferRealSize()
		{
			return this->buffer_length_real;
		}
		//----------------------------------------------------
		Int32 ClientContext::get_BufferCurrentSize()
		{
			return this->buffer_length_current;
		}
		//----------------------------------------------------
		void ClientContext::set_BufferRealSize(Int32 size)
		{
			this->buffer_length_real = size;
		}
		//----------------------------------------------------
		void ClientContext::set_BufferCurrentSize(Int32 size)
		{
			this->buffer_length_current = size;
		}
		//----------------------------------------------------
		void ClientContext::set_TotalBytes(Int32 size)
		{
			this->TotalBytes = size;

#if defined WIN32 || WIN64
			this->wsabuf.len = this->TotalBytes;
#else

#endif
		}
		//----------------------------------------------------
		Int32 ClientContext::get_TotalBytes()
		{
			return this->TotalBytes;
		}
		//----------------------------------------------------
		Int32 ClientContext::get_TransferedBytes()
		{
			return this->TransferedBytes;
		}
		//----------------------------------------------------
		void ClientContext::set_TransferedBytes(Int32 size)
		{
			this->TransferedBytes = size;
		}
		//----------------------------------------------------
		void ClientContext::IncreaseTransferedBytes(Int32 size)
		{
			this->TransferedBytes += size;
		}
		//----------------------------------------------------
		Int32 ClientContext::Send()
		{
#if defined WIN32 || WIN64
			DWORD dwBytes, dwFlags;
			dwFlags = 0;

			Int32 nBytesRecv = WSASend(this->sock->get_Handle(), &this->wsabuf, 1, 
							&dwBytes, dwFlags, &this->overlapped, NULL);

			Int32 error = WSAGetLastError();
			if (nBytesRecv == SOCKET_ERROR && error != WSA_IO_PENDING)
			{
				//throw SocketException(error, ErrorString(error));
				return -1;
			}

			return 0;
#else
#endif

		}
		//----------------------------------------------------
#if defined WIN32 || WIN64
		OVERLAPPED *ClientContext::get_Overlapped()
		{
			return &this->overlapped;
		}
		//----------------------------------------------------
		WSABUF *ClientContext::get_WSABUF()
		{
			return &this->wsabuf;
		}
		//----------------------------------------------------
		bool ClientContext::get_WsabufIndexChanged()
		{
			return this->wsabuf_index_changed;
		}
		//----------------------------------------------------
		void ClientContext::set_WsabufIndexChanged(bool val)
		{
			this->wsabuf_index_changed = val;
		}
		//----------------------------------------------------
		/*void ClientContext::set_WSABUF(char *buffer, Int32 length)
		{
			this->wsabuf.buf = buffer;
			this->wsabuf.len = length;
		}*/
		//----------------------------------------------------
#else
		//----------------------------------------------------
		//----------------------------------------------------
		//----------------------------------------------------
#endif
		//----------------------------------------------------

   };
};
//**************************************************************************************************************//
