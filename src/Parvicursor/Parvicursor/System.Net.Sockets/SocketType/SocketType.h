/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Net_Sockets_SocketType_h__
#define __System_Net_Sockets_SocketType_h__

#if defined WIN32 || WIN64
//#include <winsock2.h>
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
			enum SocketType
			{
				#if defined WIN32 || WIN64
					Stream = 1,
					Dgram = 2,
					Raw = 3,
					Rdm = 4,
					Seqpacket = 5,
					//unknown = -1,
				#else
					Stream = SOCK_STREAM,
					Dgram = SOCK_DGRAM,
					Raw = SOCK_RAW,
					Rdm = SOCK_RDM,
					Seqpacket = SOCK_SEQPACKET,
					//Packet = SOCK_PACKET,
					//unknown = -1,
				#endif
			};
			//----------------------------------------------------
		}
	}
}
//**************************************************************************************************************//

#endif

