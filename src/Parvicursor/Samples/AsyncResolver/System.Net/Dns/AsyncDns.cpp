//#include "stdafx.h"
#include "AsyncDns.h"

//**************************************************************************************************************//


namespace System
{
	namespace Net
	{
		//----------------------------------------------------
		// This internal callback is executed by the ThreadPool instance.
		static void ResolveCallback(Object *asyncDnsInfo)
		{
			// Casts asyncDnsInfo into the AsyncDnsInfo class.
			AsyncDnsInfo *info = (AsyncDnsInfo *)asyncDnsInfo;

			try
			{
				// The Resolve() method is a synchronous method.
				info->hostEntry = System::Net::Dns::Resolve2(info->hostName);
			}
			catch(Exception &e)
			{
				info->HasException = true;
				info->exceptionMessage = e.get_Message();
			}
			catch(...)
			{
				info->HasException = true;
				info->exceptionMessage = "Unknown exception";
			}

			// Invokes the callback passed to the BeginResolve() method.
			info->requestCallback(info);
		}
		//----------------------------------------------------
		IAsyncResult *AsyncDns::BeginResolve(const String &hostName, AsyncCallback requestCallback, Object *stateObject)
		{
			if(hostName.get_BaseStream() == null)
				throw ArgumentNullException("hostName");

			if(requestCallback == null)
				throw ArgumentNullException("requestCallback");

			// Allocates a placeholder to keep the asynchronous request.
			AsyncDnsInfo *info = new AsyncDnsInfo();
			info->hostName = hostName;
			info->requestCallback = requestCallback;
			info->AsyncState = stateObject;
			info->IsCompleted = false;
			info->CompletedSynchronously = false;
			info->HasException = false;

			// Enqueues the request into the ThreadPool instance.
			dnsThreadPool.QueueUserWorkItem(ResolveCallback, info);
			return info;
		}
		//----------------------------------------------------
		IPHostEntry AsyncDns::EndResolve(IAsyncResult *asyncResult)
		{
			if(asyncResult == null)
				throw ArgumentNullException("asyncResult");

			// Casts asyncResult into the AsyncDnsInfo class.
			AsyncDnsInfo *info = (AsyncDnsInfo *)asyncResult;

			// Was there an error while the asynchronous execution?
			if(info->HasException)
			{
				String exceptionMessage = info->exceptionMessage;
				delete info;
				// Re-throws the caught exception within the ResolveCallback() function.
				throw Exception(exceptionMessage);
			}

			IPHostEntry hostEntry = info->hostEntry;

			// Releases the allocated memory for AsyncDnsInfo instance within the BeginResolve() method.
			delete info;

			return hostEntry;
		}
		//----------------------------------------------------
	};
};
//**************************************************************************************************************//
