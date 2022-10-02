/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "Dns.h"

//**************************************************************************************************************//

//using namespace System::Net::Sockets;

namespace System
{
	//----------------------------------------------------
   namespace Net
   {
	   //----------------------------------------------------
	   IPHostEntry Dns::Resolve2(const String &hostName)
	   {
		   if(hostName.get_BaseStream() == null)
			   throw ArgumentNullException("hostName");
#if defined WIN32 || WIN64
			if(!ws2_32_is_loaded)
			{
		       WORD wVersionRequested;
			   WSADATA wsaData;
			   int errorIni;
			   wVersionRequested = MAKEWORD(2, 2);
			   errorIni = WSAStartup(wVersionRequested,&wsaData);
			   if(errorIni != 0)
			   {
                  WSACleanup();
				  throw SocketException(errorIni);
			   }
			   ws2_32_is_loaded = true;
			}
#endif
		   hostent *host;
		   host = gethostbyname(hostName.get_BaseStream());
		   if(host == null)
		   {
#if defined WIN32 || WIN64

                int err = WSAGetLastError();
                throw SocketException(err, ErrorString(err));
                /*switch(WSAGetLastError())
                {
                    case WSANOTINITIALISED:
                        throw SocketException(WSAGetLastError(), "A successful WSAStartup call must occur before using this function.");
                        break;
                    case WSAENETDOWN:
                        throw SocketException(WSAGetLastError(), "The network subsystem has failed.");
                        break;
                    case WSAHOST_NOT_FOUND:
                        throw SocketException(WSAGetLastError(), "Authoritative answer host not found.");
                        break;
                    case WSATRY_AGAIN:
                        throw SocketException(WSAGetLastError(), "Nonauthoritative host not found, or server failure.");
                        break;
                    case WSANO_RECOVERY:
                        throw SocketException(WSAGetLastError(), "A nonrecoverable error occurred.");
                        break;
                    case WSANO_DATA:
                        throw SocketException(WSAGetLastError(), "Valid name, no data record of requested type.");
                        break;
                    case WSAEINPROGRESS:
                        throw SocketException(WSAGetLastError(), "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
                        break;
                    case WSAEFAULT:
                        throw SocketException(WSAGetLastError(), "The name parameter is not a valid part of the user address space.");
                        break;
                    case WSAEINTR:
                        throw SocketException(WSAGetLastError(), "locking Windows Socket 1.1 call was canceled through WSACancelBlockingCall.");
                        break;
                    default:
                        throw SocketException(WSAGetLastError());
                }*/
    #else
                switch(h_errno)
                {
                    case HOST_NOT_FOUND:
                        throw SocketException(h_errno, "The specified host is unknown.");
                        break;
                    case NO_ADDRESS:
                        throw SocketException(h_errno, "The requested name is valid but does not have an IP address.");
                        break;
                    case NO_RECOVERY:
                        throw SocketException(h_errno, "A non-recoverable name server error occurred.");
                        break;
                    case TRY_AGAIN:
                        throw SocketException(h_errno, "A temporary error occurred on an authoritative name server. Try again later.");
                        break;
                    default:
                        throw SocketException(h_errno);
                }
    #endif

		   }
		   //printf("hostlen:%d\n", host->h_length);
		   //for(int i = 0 ; host->h_length ; i++)
		   /*printf("addrlist:%s\n", host->h_name);
		   printf("addrlist:%s\n", host->h_addr_list[0]);
		   struct in_addr addr;
           addr.s_addr = *(u_long *)host->h_addr_list[0];
           printf("\nFirst IP Address: %s\n", inet_ntoa(addr));*/

		   return IPHostEntry(host);
		   //return null;
	   }
	   //----------------------------------------------------
	   IPHostEntry *Dns::Resolve(const String &hostName)
	   {
		   if(hostName.get_BaseStream() == null)
			   throw ArgumentNullException("hostName");
#if defined WIN32 || WIN64
		   if(!ws2_32_is_loaded)
		   {
			   WORD wVersionRequested;
			   WSADATA wsaData;
			   int errorIni;
			   wVersionRequested = MAKEWORD(2, 2);
			   errorIni = WSAStartup(wVersionRequested,&wsaData);
			   if(errorIni != 0)
			   {
				   WSACleanup();
				   throw SocketException(errorIni);
			   }
			   ws2_32_is_loaded = true;
		   }
#endif

		   struct addrinfo hints;
		   struct addrinfo **res = new struct addrinfo *();
		   // first, load up address structs with getaddrinfo():
		   memset(&hints, 0, sizeof hints);
		   hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
		   //hints.ai_socktype = SOCK_STREAM;
		   int retVal = ::getaddrinfo(hostName.get_BaseStream(), null, &hints, res);

		   if(retVal != 0)
		   {
#if defined WIN32 || WIN64

			   int err = WSAGetLastError();
			   throw SocketException(err, ErrorString(err));
#else

			   throw SocketException(h_errno, (const char *)_sys_errlist[h_errno]);
#endif

		   }

		   return new IPHostEntry(res);
		   //return null;
	   }
	   //----------------------------------------------------
   };
};
//**************************************************************************************************************//
