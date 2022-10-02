#define null 0
#include "stdafx.h"

#include "EndPoint.h"

//**************************************************************************************************************//
namespace System
{
	//----------------------------------------------------
   namespace Net 
   {
	   //----------------------------------------------------
	   EndPoint::EndPoint()
	   {
	   }
	   //----------------------------------------------------
	   AddressFamily EndPoint::get_AddressFamily()
	   {
			throw NotSupportedException();
	   }
	   //----------------------------------------------------
	   /*EndPoint EndPoint::Create(SocketAddress address)
	   {
			throw new NotSupportedException();
	   }
	   //----------------------------------------------------
	   SocketAddress EndPoint::Serialize()
	   {
			throw new NotSupportedException();
	   }*/
	   //----------------------------------------------------
   };
};
//**************************************************************************************************************//
