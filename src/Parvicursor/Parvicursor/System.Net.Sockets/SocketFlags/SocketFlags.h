/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Net_Sockets_SocketFlags_h__
#define __System_Net_Sockets_SocketFlags_h__

#if defined WIN32 || WIN64
//#include <winsock2.h>
//#include <winsock.h>
//#include <windows.h>
#include <mswsock.h>

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
			enum SocketFlags
			{
				#if defined WIN32 || WIN64

					// Summary:
					//     Use no flags for this call.
					None = 0,
					//
					// Summary:
					//     Process out-of-band data.
					OutOfBand = MSG_OOB,
					//
					// Summary:
					//     Peek at the incoming message.
					Peek = MSG_PEEK,
					//
					// Summary:
					//     Send without using routing tables.
					DontRoute = MSG_DONTROUTE,
					//
					// Summary:
					//     Provides a standard value for the number of WSABUF structures that are used
					//     to send and receive data.
					MaxIOVectorLength = 16,
					//
					// Summary:
					//     The message was too large to fit into the specified buffer and was truncated.
					Truncated = MSG_TRUNC,
					//
					// Summary:
					//     Indicates that the control data did not fit into an internal 64-KB buffer
					//     and was truncated.
					ControlDataTruncated = MSG_CTRUNC,
					//
					// Summary:
					//     Indicates a broadcast packet.
					Broadcast = MSG_BCAST,
					//
					// Summary:
					//     Indicates a multicast packet.
					Multicast = MSG_MCAST,
					//
					// Summary:
					//     Partial send or receive for message.
					Partial = MSG_PARTIAL,


				#else
					// Summary:
					//     Use no flags for this call.
					None = 0,
					//
					// Summary:
					//     Process out-of-band data.
					OutOfBand = MSG_OOB,
					//
					// Summary:
					//     Peek at the incoming message.
					Peek = MSG_PEEK,
					//
					// Summary:
					//     Send without using routing tables.
					DontRoute = MSG_DONTROUTE,
					//
					// Summary:
					//     Provides a standard value for the number of WSABUF structures that are used
					//     to send and receive data.
					//MaxIOVectorLength = 16,
					//
					// Summary:
					//     The message was too large to fit into the specified buffer and was truncated.
					Truncated = MSG_TRUNC,
					//
					// Summary:
					//     Indicates that the control data did not fit into an internal 64-KB buffer
					//     and was truncated.
					ControlDataTruncated = MSG_CTRUNC,
					//
					// Summary:
					//     Indicates a broadcast packet.
					//Broadcast = MSG_BCAST,
					//
					// Summary:
					//     Indicates a multicast packet.
					//Multicast = MSG_MCAST,
					//
					// Summary:
					//     Partial send or receive for message.
					//Partial = MSG_PARTIAL,

				#endif
			};
			//----------------------------------------------------
		}
	}
}
//**************************************************************************************************************//

#endif
