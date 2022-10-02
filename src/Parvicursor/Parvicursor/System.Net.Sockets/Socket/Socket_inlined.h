#ifndef __System_Net_Sockets_Socket_h__
#define __System_Net_Sockets_Socket_h__


#include "../../System/Exception/Exception.h"
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
#include "../../StaticFunctions/StaticFunctions.h"

#include <stdio.h>

#ifndef FD_SETSIZE
#define FD_SETSIZE      200
#endif /* FD_SETSIZE */

#if defined WIN32 || WIN64

#include <winsock2.h>
#ifndef __Windows_h__
#define __Windows_h__

#ifndef MSG_PARTIAL
#define MSG_PARTIAL	0x8000
#endif
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
#define SOCKET int
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
				private: int ret;
				private: bool isListening;
				private: bool isConnected;
				private: int sock;
				private: AddressFamily address_family;
				private: SocketType socket_type;
				private: ProtocolType protocol_type;
				private: bool blocking;// = true;
				private: bool disposed;// = false;
				//----------------------------------------------------
				public: Socket(AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType);
				private: Socket(SOCKET s);
				public: ~Socket();
				public: inline AddressFamily get_AddressFamily() const;
				public: inline SocketType get_SocketType() const;
				public: inline ProtocolType get_ProtocolType() const;
				public: inline int get_Handle() const;
				public: inline bool get_Connected() const;
                //----------------------------------------------------
				public: inline void Connect(IPEndPoint &remoteEP);
				public: inline void Bind(IPEndPoint &localEP);
				public: inline void Listen(int backlog);
				public: inline static void Select(ArrayList &checkRead, ArrayList &checkWrite, ArrayList &checkError, int microSeconds);
				public: inline int Send(const char buffer[], int offset, int size, SocketFlags socketFlags);
				public: inline int Receive(char buffer[], int offset, int size, SocketFlags socketFlags);
				public: inline void Close();
				public: inline void Shutdown(SocketShutdown how);
				public: inline Socket *Accept() const;
				//public:  void SetSocketOption(SocketOptionLevel optionLevel, SocketOptionName optionName,byte[] optionValue) // msu to be implemented
				public: inline void SetReceiveTcpWindowSize(int size);
				public: inline void SetSendTcpWindowSize(int size);
				public: inline int GetReceiveTcpWindowSize();
				public: inline int GetSendTcpWindowSize();

				//----------------------------------------------------
			};
			//----------------------------------------------------
		}
	}
}
//**************************************************************************************************************//

#endif
