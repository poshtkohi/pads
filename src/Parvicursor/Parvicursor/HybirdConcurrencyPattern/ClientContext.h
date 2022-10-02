#ifndef __Parvicursor_HybirdConcurrencyPattern_ClientContext_h__
#define __Parvicursor_HybirdConcurrencyPattern_ClientContext_h__

#include "../../System/BasicTypes/BasicTypes.h"
#include "../../System/Object/Object.h"
#include "../../System/ArgumentException/ArgumentOutOfRangeException.h"
#include "../../System/ArgumentException/ArgumentNullException.h"
#include "../../System/Convert/Convert.h"
#include "../../System.Net.Sockets/Socket/Socket.h"


#if !defined ParvicursorLib_inlining
//#warning hello world
#ifndef __Parvicursor_inline__
#define __Parvicursor_inline__ inline
#endif
#else
#ifndef __Parvicursor_inline__
#define __Parvicursor_inline__
//#warning hello
#endif
#endif


//**************************************************************************************************************//

using namespace System;

namespace Parvicursor
{
	//----------------------------------------------------
   namespace HybirdConcurrencyPattern
   {
		enum ClientContextState
		{
			_Accepted = 0,
			_OnReceive = 1,
			_OnSend = 2,
			_Closed = 3,
			_OnReuse = 4
		};

		enum ProtocolState
		{
			WaitForReccive,
			AuthenticateRequested,
			Authenticated,

		};
		//----------------------------------------------------
	    // consider C++ inlining for this class
		class ClientContext : public Object
		{
			public: char *buffer;
			public: Int32 buffer_length_real;
			public: Int32 buffer_length_current;
			private: Socket *sock;
			private: ClientContextState state;
			private: ProtocolState protocolState;
			private: bool disposed;
			private: Int32 TotalBytes;
			private: Int32 TransferedBytes;
			//----------------------------------------------------
			public: ClientContext(Socket *sock);
			public: ~ClientContext();
			public: void Dispose();
			private: static const Int32 DefaultBufferSize = 1*1024; //1KB
			public: /*__Parvicursor_inline__*/ ClientContextState get_State();
			public: /*__Parvicursor_inline__*/ void set_State(ClientContextState state);
			public: /*__Parvicursor_inline__*/ Socket *get_Socket();
			public: /*__Parvicursor_inline__*/ Int32 OnSend();    // return -1 for error
			public: /*__Parvicursor_inline__*/ Int32 OnReceive(); // return -1 for error
			public: /*__Parvicursor_inline__*/ void OnAccept();
			public: /*__Parvicursor_inline__*/ char *get_Buffer();
			public: /*__Parvicursor_inline__*/ Int32 get_BufferRealSize();
			public: /*__Parvicursor_inline__*/ Int32 get_BufferCurrentSize();
			public: /*__Parvicursor_inline__*/ void set_TotalBytes(Int32 size); // set the total bytes to be sent or received.
			public: /*__Parvicursor_inline__*/ Int32 get_TotalBytes();          // get the total bytes to be sent or received.
			public: /*__Parvicursor_inline__*/ Int32 get_TransferedBytes();
			public: /*__Parvicursor_inline__*/ void set_TransferedBytes(Int32 size);
			public: /*__Parvicursor_inline__*/ void set_Buffer(char *buffer);
			public: /*__Parvicursor_inline__*/ void set_BufferRealSize(Int32 size);
			public: /*__Parvicursor_inline__*/ void set_BufferCurrentSize(Int32 size);
			public: /*__Parvicursor_inline__*/ void IncreaseTransferedBytes(Int32 size);
			public: void AllocateBuffer(Int32 size);
			public: /*__Parvicursor_inline__*/ Int32 Send(); // return -1 for error
			//----------------------------------------------------
#if defined WIN32 || WIN64
			public: /*__Parvicursor_inline__*/ OVERLAPPED *get_Overlapped();
			public: /*__Parvicursor_inline__*/ WSABUF *get_WSABUF();
			public: /*__Parvicursor_inline__*/ void set_WSABUF(char *buffer, Int32 length);
			//public: /*__Parvicursor_inline__*/ void ResetWSABUF();
			private: OVERLAPPED overlapped;
			private: WSABUF wsabuf;
			private: bool wsabuf_index_changed;
			private: public: /*__Parvicursor_inline__*/ bool get_WsabufIndexChanged();
			private: public: /*__Parvicursor_inline__*/ void set_WsabufIndexChanged(bool val);
#else
#endif
			//----------------------------------------------------
		};
		//----------------------------------------------------
	}
}
//**************************************************************************************************************//

#endif
