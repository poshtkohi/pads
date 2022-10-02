/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Net_IPEndPoint_h__
#define __System_Net_IPEndPoint_h__

#include "../../System/ArgumentException/ArgumentOutOfRangeException.h"
#include "../../System.Net.Sockets/AddressFamily/AddressFamily.h"
#include "../IPAddress/IPAddress.h"

//**************************************************************************************************************//

using namespace System::Net;
using namespace System::Net::Sockets;

namespace System
{
	//----------------------------------------------------
   namespace Net
   {
		class IPEndPoint
		{
			private: long _address;
			private: IPAddress address;
			private: int port;
			public: int MaxPort;
			public: int MinPort;
			//----------------------------------------------------
			public: IPEndPoint(long address, int port);
			public: IPEndPoint(const IPAddress &address, int port);
			public: IPEndPoint();
			//public: IPEndPoint(IPAddress address, int port);
			public: int get_Port();
			public: void set_Port(int port);
			public: IPAddress get_Address();
			public: long get_LongAddress();
			public: void set_Address(IPAddress address);
			public: AddressFamily get_AddressFamily();
			//----------------------------------------------------
		};
	}
}
//**************************************************************************************************************//

#endif
