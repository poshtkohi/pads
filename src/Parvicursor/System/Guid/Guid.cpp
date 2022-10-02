/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "Guid.h"

//**************************************************************************************************************//

namespace System
{
		//----------------------------------------------------
		String Guid::NewGuid()
		{
#if defined WIN32 || WIN64

			unsigned char *guidStr = 0x00;
			GUID *pguid = 0x00;
			pguid = new GUID();
			CoCreateGuid(pguid);
			// Convert the GUID to a string
			UuidToString(pguid, &guidStr);

			delete pguid;
			//printf("%s\n", guidStr);

			String s((const char *)guidStr);

			RpcStringFree(&guidStr);

			return s;

#else

            uuid_t id;
            uuid_generate(id);

            char *buffer = new char[33];

            buffer[32] = 0;

            for(register Int32 i = 0 ; i < 16; i++, buffer+=2)
                sprintf(buffer, "%x%x", (id[i]&240)>>4, id[i]&15);

            buffer-=32;

            return String(buffer);
#endif
		}
	    //----------------------------------------------------

}
//**************************************************************************************************************//
