/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "SocketException.h"

//**************************************************************************************************************//
namespace System
{
	//----------------------------------------------------
   namespace Net
   {
		namespace Sockets
		{
			//----------------------------------------------------
			SocketException::SocketException(Int32 error) : Exception("Unhandled Exception: System.Net.Socket.SocketException: For more information see the ErrorCode and refer to Win or GNU Socket API documentations.")
			{
				/*#if defined WIN32 || WIN64
					WSACleanup();
				#endif*/
				this->error = error;
			}
			//----------------------------------------------------
			SocketException::SocketException(Int32 error, const String &message) : Exception(message)
			{
				/*#if defined WIN32 || WIN64
					WSACleanup();
				#endif*/
				this->error = error;
			}
			//----------------------------------------------------
			Int32 SocketException::get_ErrorCode()
			{
				return error;
			}
            //----------------------------------------------------
            String SocketException::get_Message()
            {
                String s = "Unhandled Exception: System.Net.Sockets.SocketException: " + Exception::get_Message() + ".";
                return s;//String::Copy(s);
            }
			//----------------------------------------------------
		};

	};
   //----------------------------------------------------
};
//**************************************************************************************************************//
