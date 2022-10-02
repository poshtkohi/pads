/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Net_Dns_h__
#define __System_Net_Dns_h__

#include "../../System/String/String.h"
#include "../../System/ArgumentException/ArgumentNullException.h"
#include "../../System.Net.Sockets/SocketException/SocketException.h"
#include "../IPHostEntry/IPHostEntry.h"
#include "../../StaticFunctions/StaticFunctions.h"

#if defined WIN32 || WIN64
#ifndef __Windows_h__
#define __Windows_h__
//#include <winsock2.h>
//#include <windows.h>
#define WIN
#pragma comment(lib,"ws2_32.lib")
#include <ws2tcpip.h>
#endif
#else
#define LINUX
#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <stdio.h>
#include <string.h>

//**************************************************************************************************************//

namespace System
{
	//----------------------------------------------------
   namespace Net
   {
	    class Dns : public Object
		{
			//----------------------------------------------------
			public: static IPHostEntry *Resolve(const String &hostName);
			public: static IPHostEntry Resolve2(const String &hostName);
			//----------------------------------------------------
		};
	}
}
//**************************************************************************************************************//

#endif
