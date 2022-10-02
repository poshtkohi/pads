/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Net_Socket_AddressFamily_h__
#define __System_Net_Socket_AddressFamily_h__

#if defined WIN32 || WIN64
#include <winsock2.h>
//#include <windows.h>

#else
#define LINUX
#include <errno.h>
#include <sys/socket.h>
//#include <cygwin/socket.h>
#include <netinet/in.h>
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
			enum AddressFamily
			{
				#if defined WIN32 || WIN64
					Unknown = -1,
					Unspecified = 0,
					Unix = 1,
					InterNetwork = 2,
					ImpLink = 3,
					Pup = 4,
					Chaos = 5,
					NS = 6,
					Ipx = 6,
					Iso = 7,
					Osi = 7,
					Ecma = 8,
					DataKit = 9,
					Ccitt = 10,
					Sna = 11,
					DecNet = 12,
					DataLink = 13,
					Lat = 14,
					HyperChannel = 15,
					AppleTalk = 16,
					NetBios = 17,
					VoiceView = 18,
					FireFox = 19,
					Banyan = 21,
					Atm = 22,
					InterNetworkV6 = 23,
					Cluster = 24,
					Ieee12844 = 25,
					Irda = 26,
					NetworkDesigners = 28,
					Max = 29,

				#else

						Unknown = -1,
						Unspecified = AF_UNSPEC,
						Unix = AF_UNIX,
						InterNetwork = AF_INET,
						//ImpLink = 3,
						//Pup = 4,
						//Chaos = 5,
						//NS = 6,
						Ipx = AF_IPX,
						//Iso = 7,
						//Osi = 7,
						//Ecma = 8,
						//DataKit = 9,
						//Ccitt = 10,
						Sna = AF_SNA,
						DecNet = AF_DECnet,
						//DataLink = 13,
						//Lat = 14,
						//HyperChannel = 15,
						AppleTalk = AF_APPLETALK,
						//NetBios = 17,
						//VoiceView = 18,
						//FireFox = 19,
						//Banyan = 21,
						//Atm = 22,
						InterNetworkV6 = AF_INET6,
						//Cluster = 24,
						//Ieee12844 = 25,
						Irda = AF_IRDA,
						//NetworkDesigners = 28,
						Max = AF_MAX,
				#endif
			};
			//----------------------------------------------------
		}
	}
}
//**************************************************************************************************************//

#endif

