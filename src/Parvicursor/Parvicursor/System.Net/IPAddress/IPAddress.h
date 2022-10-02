/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Net_IPAddress_h__
#define __System_Net_IPAddress_h__

#include "../../System/NotSupportedException/NotSupportedException.h"
#include "../../System.Net.Sockets/AddressFamily/AddressFamily.h"
//**************************************************************************************************************//

using namespace System::Net::Sockets;

namespace System
{
	//----------------------------------------------------
   namespace Net
   {
		class IPAddress
		{
		    private: char *address;
			private: int len;
			//----------------------------------------------------
			public: IPAddress();
		    /* len is length of address. */
			public: IPAddress(char address[], int len);
			public: AddressFamily get_AddressFamily();
			public: static long get_Any();
			public: static long get_Broadcast();
			public: static long get_Loopback();
			public: static long get_None();
			/*public: static long get_IPv6Any();
			public: static long get_IPv6Loopback();
			public: static long get_IPv6None();*/
			public: char *GetAddressBytes();
			public: int GetAddressBytesLength();
			//----------------------------------------------------
		};
	}
}
//**************************************************************************************************************//

#endif
