/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Net_Sockets_SocketException_h__
#define __System_Net_Sockets_SocketException_h__

#include "../../System/Exception/Exception.h"
#include "../../System/String/String.h"


#if defined WIN32 || WIN64
#include <winsock2.h>
//#include <windows.h>
#pragma comment(lib,"ws2_32.lib")

#else
#define LINUX
#include <errno.h>
#include <sys/socket.h>
#endif

//**************************************************************************************************************//

namespace System
{
	namespace Net
   {
		namespace Sockets
		{
			class SocketException : public Exception
			{
				private: int error;
				//----------------------------------------------------
				public: SocketException(Int32 error);
				public: SocketException(Int32 error, const String &message);
			    public: Int32 get_ErrorCode();
			    public: String get_Message();
				//----------------------------------------------------
			};
		}
	}
}
//**************************************************************************************************************//

#endif

