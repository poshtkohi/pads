/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "IPAddress.h"

//**************************************************************************************************************//
namespace System
{
	//----------------------------------------------------
   namespace Net
   {
	   //----------------------------------------------------
	   IPAddress::IPAddress()
	   {
	   }
	   //----------------------------------------------------
	   IPAddress::IPAddress(char address[], int len)
	   {
		   this->address = address;
		   this->len = len;
	   }
	   //----------------------------------------------------
	   /*IPAddress::~IPAddress()
	   {
		   //delete address;
	   }*/
	   //----------------------------------------------------
	   AddressFamily IPAddress::get_AddressFamily()
	   {
		   if(this->len == 4)
			   return System::Net::Sockets::InterNetwork;
		   else
			   return System::Net::Sockets::InterNetworkV6;
	   }
	   //----------------------------------------------------
	   long IPAddress::get_Any()
	   {
		   return INADDR_ANY;
	   }
	   //----------------------------------------------------
	   long IPAddress::get_Broadcast()
	   {
		   return INADDR_BROADCAST;
	   }
	   //----------------------------------------------------
	   long IPAddress::get_Loopback()
	   {
		   return INADDR_LOOPBACK;
	   }
	   //----------------------------------------------------
	   long IPAddress::get_None()
	   {
		   return INADDR_NONE;
	   }
	   /*//----------------------------------------------------
	   long IPAddress::get_IPv6Any()
	   {
		   return INADDR_ANY;
	   }
	   //----------------------------------------------------
	   long IPAddress::get_IPv6Loopback()
	   {
		   return INADDR_ANY;
	   }
	   //----------------------------------------------------
	   long IPAddress::get_IPv6None()
	   {
		   return INADDR_ANY;
	   }*/
	   //----------------------------------------------------
	   char *IPAddress::GetAddressBytes()
	   {
		   return this->address;
	   }
	   //----------------------------------------------------
	   int IPAddress::GetAddressBytesLength()
	   {
		   return this->len;
	   }
	   //----------------------------------------------------
   };
};
//**************************************************************************************************************//
