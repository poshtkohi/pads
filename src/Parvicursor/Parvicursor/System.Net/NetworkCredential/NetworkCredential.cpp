/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "NetworkCredential.h"

//**************************************************************************************************************//
namespace System
{
	//----------------------------------------------------
   namespace Net
   {
	    //----------------------------------------------------
	    NetworkCredential::NetworkCredential(const String &username, const String &password)
		{
			this->user = username;
			this->pass = password;
		}
		//----------------------------------------------------
		String NetworkCredential::get_Username() const
		{
			return this->user;
		}
		//----------------------------------------------------
		void NetworkCredential::set_Username(const String &username)
		{
			this->user = username;
		}
		//----------------------------------------------------
		String NetworkCredential::get_Password() const
		{
			return this->pass;
		}
		//----------------------------------------------------
		void NetworkCredential::set_Password(const String &password)
		{
			this->pass = password;
		}
	    //----------------------------------------------------
   };
};
//**************************************************************************************************************//
