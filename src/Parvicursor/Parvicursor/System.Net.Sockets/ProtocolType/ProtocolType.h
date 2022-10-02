/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Net_Sockets_ProtocolType_h__
#define __System_Net_Sockets_ProtocolType_h__


#if defined WIN32 || WIN64
#include <winsock2.h>
//#include <windows.h>

#else
#define LINUX
#include <errno.h>
#include <sys/socket.h>
#endif
//**************************************************************************************************************//


namespace System
{
	//----------------------------------------------------
   namespace Net
   {
	    namespace Sockets
		{
			//----------------------------------------------------
			enum ProtocolType
			{
				#if defined WIN32 || WIN64
					iP = 0,
					icmp = 1,
					igmp = 2,
					ggp = 3,
					tcp = 6,
					pup = 12,
					udp = 17,
					idp = 22,
					ipv6 = 41,
					nd = 77,
					raw = 255,
					unspecified = 0,
					ipx = 1000,
					spx = 1256,
					spxII = 1257,
					//unknown = -1,
				#else

					iP = 0,
					icmp = 1,
					igmp = 2,
					ggp = 3,
					tcp = 0,//
					pup = 12,
					udp = 0,//
					idp = 22,
					ipv6 = 41,
					nd = 77,
					raw = 255,
					unspecified = 0,
					ipx = 1000,
					spx = 1256,
					spxII = 1257,
					//unknown = -1,
				#endif
			};
			//----------------------------------------------------
		}
	}
}
//**************************************************************************************************************//

#endif

