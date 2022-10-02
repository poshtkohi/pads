/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Net_Sockets_SocketShutdown_h__
#define __System_Net_Sockets_SocketShutdown_h__

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
			enum SocketShutdown
			{
				#if defined WIN32 || WIN64
					Receive = SD_RECEIVE,
					Send = SD_SEND,
					Both = SD_BOTH,
				#else
					Receive =  SHUT_RD,
					Send =  SHUT_WR,
					Both = SHUT_RDWR,
				#endif
			};
			//----------------------------------------------------
		}
	}
}
//**************************************************************************************************************//

#endif

