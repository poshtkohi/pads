#ifndef __System_Net_EndPoint_h__
#define __System_Net_EndPoint_h__

#ifndef __System_NotSupportedException_h__
#include "../../System/NotSupportedException/NotSupportedException.h"
#endif

#ifndef __System_Net_Socket_h__
#include "../../System.Net.Socket/Socket/Socket.h"
#endif
//**************************************************************************************************************//

using namespace System::Net::Socket;

namespace System
{
	//----------------------------------------------------
   namespace Net 
   {
		class EndPoint
		{
			//----------------------------------------------------
			protected: EndPoint();
			public: virtual AddressFamily get_AddressFamily();
			//public: virtual EndPoint Create (SocketAddress address);
			//public: virtual SocketAddress Serialize();
			//----------------------------------------------------
		};
	}
}
//**************************************************************************************************************//

#endif
