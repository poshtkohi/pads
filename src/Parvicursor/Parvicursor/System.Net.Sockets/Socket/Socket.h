/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Net_Sockets_Socket_h__
#define __System_Net_Sockets_Socket_h__


#include "../../general.h"
#include "../../StaticFunctions/StaticFunctions.h"
#include "../../System/Exception/Exception.h"
#include "../../System/BasicTypes/BasicTypes.h"
#include "../../System/String/String.h"
#include "../../System.Collections/ArrayList/ArrayList.h"
#include "../../System/ObjectDisposedException/ObjectDisposedException.h"
#include "../../System/InvalidOperationException/InvalidOperationException.h"
#include "../../System/ArgumentException/ArgumentNullException.h"
#include "../SocketException/SocketException.h"
#include "../AddressFamily/AddressFamily.h"
#include "../ProtocolType/ProtocolType.h"
#include "../SocketType/SocketType.h"
#include "../SocketFlags/SocketFlags.h"
#include "../SocketShutdown/SocketShutdown.h"
#include "../../System.Net/IPEndPoint/IPEndPoint.h"

#include <stdio.h>

//#undef FD_SETSIZE
//#define FD_SETSIZE s200

#if defined WIN32 || WIN64
//#endif /* FD_SETSIZE */


#include <winsock2.h>
#include <ws2tcpip.h>

#ifndef MSG_PARTIAL
#define MSG_PARTIAL	0x8000
//#include <windows.h>
#endif

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")

#else
#define LINUX
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#ifndef SOCKADDR
#define SOCKADDR sockaddr
#endif

#ifndef SOCKET
#define SOCKET Int32
#endif

#endif




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
using namespace System::Collections;
using namespace System::Net;

namespace System
{
	//----------------------------------------------------
   namespace Net
   {
	    namespace Sockets
		{
			//----------------------------------------------------
			class Socket : public Object
			{
				private: Int32 ret;
				private: bool isListening;
				private: bool isConnected;
				private: bool isAccepted;
				private: bool isBound;
				private: Int32 sock;
				private: AddressFamily address_family;
				private: SocketType socket_type;
				private: ProtocolType protocol_type;
				private: bool blocking;
				private: bool disposed;
				//----------------------------------------------------
				//public: Socket(){}
				public: Socket(AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType);
				private: Socket(SOCKET s);
				public: Socket(SOCKET s, AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType);
				public: ~Socket();
				public: __Parvicursor_inline__ AddressFamily get_AddressFamily() const;
				public: __Parvicursor_inline__ SocketType get_SocketType() const;
				public: __Parvicursor_inline__ ProtocolType get_ProtocolType() const;
				public: __Parvicursor_inline__ bool get_Connected() const;
				public: __Parvicursor_inline__ bool get_IsAccepted() const;
				public: __Parvicursor_inline__ bool get_IsBound() const;
                /*//----------------------------------------------------
                public: inline AddressFamily get_AddressFamily() const
                {
                    return this->address_family;
                }
                //----------------------------------------------------
                public: inline SocketType get_SocketType() const
                {
                    return this->socket_type;
                }
                //----------------------------------------------------
                public: inline ProtocolType get_ProtocolType() const
                {
                    return this->protocol_type;
                }
                //----------------------------------------------------
                public: inline Int32 get_Handle() const
                {
                    return this->sock;
                }
                //----------------------------------------------------
                public: inline bool get_Connected() const
                {
                    return this->isConnected;
                }*/
                //----------------------------------------------------
				public: void Connect(IPEndPoint &remoteEP);
				public: void Bind(IPEndPoint &localEP);
				public: void Listen(Int32 backlog);
				public: static void Select(ArrayList &checkRead, ArrayList &checkWrite, ArrayList &checkError, Int32 microSeconds);
				public: __Parvicursor_inline__ Int32 Send(const char buffer[], Int32 offset, Int32 size, SocketFlags socketFlags);
				public: __Parvicursor_inline__ Int32 Receive(char buffer[], Int32 offset, Int32 size, SocketFlags socketFlags);
				public: void Close();
				public: void Shutdown(SocketShutdown how);
				public: Socket *Accept() const;
				//public:  void SetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName,Byte[] optionValue) // msu to be implemented
				public: void SetReceiveTcpWindowSize(Int32 size);
				public: void SetSendTcpWindowSize(Int32 size);
				public: Int32 GetReceiveTcpWindowSize();
				public: Int32 GetSendTcpWindowSize();
                public: inline Int32 get_Handle() const
                {
                    return sock;
                }
				//----------------------------------------------------
			};
			//----------------------------------------------------
		}
	}
}
//**************************************************************************************************************//

#endif
