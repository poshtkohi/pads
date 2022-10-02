/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __System_Net_NetworkCredential_h__
#define __System_Net_NetworkCredential_h__

#include "../../System/String/String.h"
#include "../../System/Object/Object.h"
#include "../../System/ArgumentException/ArgumentNullException.h"

//**************************************************************************************************************//

namespace System
{
	//----------------------------------------------------
   namespace Net
   {
	   class NetworkCredential : public Object
		{

			private: String user;
			private: String pass;
			//----------------------------------------------------
			public: NetworkCredential(const String &username, const String &password);
			public: String get_Username() const;
			public: void set_Username(const String &username);
			public: String get_Password() const;
			public: void set_Password(const String &password);
			//----------------------------------------------------
		};
	}
}
//**************************************************************************************************************//

#endif
