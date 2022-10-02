/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "IPHostEntry.h"

//**************************************************************************************************************//
namespace System
{
	//----------------------------------------------------
   namespace Net
   {
		//----------------------------------------------------
		IPHostEntry::IPHostEntry()
		{
			//this->size_addressList = 0;
			this->host = null;
			this->res = null;
			len = -1;
		}
		//----------------------------------------------------
		IPHostEntry::IPHostEntry(hostent *host)
		{
			//this->size_addressList = 0;
			this->host = host;
			this->res = null;
			len = -1;
		}
		//----------------------------------------------------
		IPHostEntry::IPHostEntry(addrinfo **res)
		{
			//this->size_addressList = 0;
			this->host = null;
			this->res = res;
			len = -1;
		}
		//----------------------------------------------------
		IPHostEntry::~IPHostEntry()
		{
			if(res != null)
			{
			    ////printf("~IPHostEntry()\n");
				::freeaddrinfo(*res);
				delete res;
				res = null;
			}
			//printf("hh\n");
			/*if(this->size_addressList > 0)
				delete this->addressList;*/
		}
		//----------------------------------------------------
		/*hostent *IPHostEntry::get_Hostent()
		{
			return this->host;
		}*/
		//----------------------------------------------------
		IPAddress IPHostEntry::get_AddressList(Int32 index)
		{
			if(index < 0)
				throw ArgumentOutOfRangeException("index", "index is negative");
			if(index >= get_AddressListLength())
				throw ArgumentOutOfRangeException("index");
			if(this->host != null)
				return IPAddress(this->host->h_addr_list[index], this->host->h_length);
			if(this->res != null)
			{
				struct addrinfo *current = *this->res;
				Int32 i = 0;
				while(current != null)
				{
					if(i == index)
					{
						if(current->ai_family == AF_INET)
						{
							struct sockaddr_in  *sockaddr_ipv4 = (struct sockaddr_in *) current->ai_addr;
							//printf("\tIPv4 address %s\n", inet_ntoa(sockaddr_ipv4->sin_addr) );
							return IPAddress((char *)&sockaddr_ipv4->sin_addr, 4);
							//return IPAddress((char *)current->ai_addr, current->ai_addrlen);
						}
						if(current->ai_family == AF_INET6)
						{
							//struct sockaddr_in  *sockaddr_ipv4 = (struct sockaddr_in *) current->ai_addr;
							//printf("\tIPv4 address %s\n", inet_ntoa(sockaddr_ipv4->sin_addr) );
							return IPAddress((char *)current, sizeof(struct addrinfo));
						}
					}
					current = current->ai_next;
					i++;
				}
			}
		}
		//----------------------------------------------------
		Int32 IPHostEntry::get_AddressListLength()
		{
			if(len != -1)
				return len;

			len = 0;

			if(this->host != null)
			{
				for( ; ; len++)
					if(this->host->h_addr_list[len] == null)
						break;
				return len;
			}

			if(this->res != null)
			{
				struct addrinfo *current = *this->res;

				while(current != null)
				{
				    ////printf("i: %d\n", len);
					len++;
					current = current->ai_next;
				}
				return len;
			}

			return len;
		}
		/*//----------------------------------------------------
		void IPHostEntry::set_AddressList(String *addressList, int size)
		{
			if(size > 0)
			{
				if(this->size_addressList > 0)
					delete this->addressList;
				this->size_addressList = size;
				this->addressList = addressList;
			}
		}*/
		//----------------------------------------------------
   };
};
//**************************************************************************************************************//
