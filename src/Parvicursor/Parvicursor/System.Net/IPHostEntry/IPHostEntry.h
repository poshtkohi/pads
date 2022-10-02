/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Net_IPHostEntry_h__
#define __System_Net_IPHostEntry_h__

#include "../../System/String/String.h"
#include "../../System/ArgumentException/ArgumentOutOfRangeException.h"
#include "../IPAddress/IPAddress.h"

#if defined WIN32 || WIN64
#ifndef __Windows_h__
#define __Windows_h__
//#include <winsock2.h>
//#include <windows.h>
#define WIN
#include <ws2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
#endif
#else
#define LINUX
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#endif

//**************************************************************************************************************//

namespace System
{
	//----------------------------------------------------
   namespace Net
   {
		class IPHostEntry
		{
			private: struct hostent *host;
			private: struct addrinfo **res;
			private: Int32 len;
			//----------------------------------------------------
			public: IPHostEntry();
			public: IPHostEntry(struct hostent *host);
			public: IPHostEntry(struct addrinfo **res);
			public: ~IPHostEntry();
			public: IPAddress get_AddressList(Int32 index); //IPAddress[]
			public: Int32 get_AddressListLength();
			/*public: void set_AddressList(String *addressList, int size);
			//public: String* get_Aliases(int &size); //String[]
			//public: void set_Aliases(String *aliases, int &size); //String[]
			public: String get_HostName();
			public: void set_HostName(const String &hostName);*/
			//----------------------------------------------------
		};
	}
}
//**************************************************************************************************************//

#endif
