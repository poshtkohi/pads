/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "IPEndPoint.h"

//**************************************************************************************************************//
namespace System
{
	//----------------------------------------------------
   namespace Net
   {
	   //----------------------------------------------------
	   IPEndPoint::IPEndPoint()
	   {
		   this->MaxPort = 65535;
		   this->MinPort = 0;

		   this->_address = -1;
	   }
	   //----------------------------------------------------
	   IPEndPoint::IPEndPoint(long address, int port)
	   {
		   this->MaxPort = 65535;
		   this->MinPort = 0;

		   if(port < MinPort || port > MaxPort)
				throw new ArgumentOutOfRangeException("Invalid port");

		   this->_address = address;
		   this->port = port;
	   }
	   //----------------------------------------------------
	   IPEndPoint::IPEndPoint(const IPAddress &address, int port)
	   {
		   this->MaxPort = 65535;
		   this->MinPort = 0;
		   if(port < MinPort || port > MaxPort)
				throw new ArgumentOutOfRangeException("Invalid port");
		   this->address = address;
		   this->port = port;
		   this->_address = -1;
	   }
	   //----------------------------------------------------
	   void IPEndPoint::set_Port(int port)
	   {
		   this->port = port;
		   if(port < MinPort || port > MaxPort)
				throw new ArgumentOutOfRangeException("Invalid port");
	   }
	   //----------------------------------------------------
	   int IPEndPoint::get_Port()
	   {
		   return this->port;
	   }
	   //----------------------------------------------------
	   void IPEndPoint::set_Address(IPAddress address)
	   {
		   this->address = address;
	   }
	   //----------------------------------------------------
	   IPAddress IPEndPoint::get_Address()
	   {
		   return this->address;
	   }
	   //----------------------------------------------------
	   long IPEndPoint::get_LongAddress()
	   {
		   return this->_address;
	   }
	   //----------------------------------------------------
	   AddressFamily IPEndPoint::get_AddressFamily()
	   {
		   return this->address.get_AddressFamily();
	   }
	   //----------------------------------------------------
   };
};
//**************************************************************************************************************//
