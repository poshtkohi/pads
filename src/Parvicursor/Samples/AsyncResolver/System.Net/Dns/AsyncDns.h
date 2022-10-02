#ifndef __Samples_AsnycDns_h__
#define __Samples_AsnycDns_h__

#include "../../../../Parvicursor/general.h"
#include "../../../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../../../Parvicursor/System/Object/Object.h"
#include "../../../../Parvicursor/System/Convert/Convert.h"
#include "../../../../Parvicursor/System/Exception/Exception.h"
#include "../../../../Parvicursor/System/ArgumentException/ArgumentNullException.h"
#include "../../../../Parvicursor/System/Environment/Environment.h"
#include "../../../../Parvicursor/System/Async/Async.h"
#include "../../../../Parvicursor/System/String/String.h"
#include "../../../../Parvicursor/System.Net/Dns/Dns.h"
#include "../../../../Parvicursor/System.Net/IPHostEntry/IPHostEntry.h"
#include "../../../../Parvicursor/System.Threading/ThreadPool/ThreadPool.h"

using namespace System;
using namespace System::Net;
using namespace System::Threading;
//**************************************************************************************************************//

namespace System
{
	namespace Net
	{
		//----------------------------------------------------
		// A thread pool instance is used to simulate an asynchronous DNS resolver with calling
		// the synchronous Dns::Resovle() method. This sample project is just a simple learning
		// example. In general, we should consider a systsem-wide thread pool for all asynchronous
		// method invocations across Parvicursor.NET framework instead of a thread pool per class.
		static ThreadPool dnsThreadPool = ThreadPool(System::Environment::get_ProcessorCount()*2, INT_MAX);
		//----------------------------------------------------
		// An internal class that provides to encapsulate the necessary information allocated by
		// the BeginResolve() method and used by the EndResolve() method.
		class AsyncDnsInfo : public IAsyncResult
		{
			// Stores the hostName passed to the BeginResolve() method.
			public: String hostName;
			// Stores the callback passed to the BeginResolve() method.
			public: AsyncCallback requestCallback;
			// Stores the return of the synchronous Dns::Resovle() method
			public: IPHostEntry hostEntry;
			// Indicates whether there is an error during the calling synchronous Dns::Resovle() method.
			public: bool HasException;
			// Stores the message of the thrown exception by synchronous Dns::Resovle() method.
			public: String exceptionMessage;
		};
		//----------------------------------------------------
		// Provides simple asynchronous domain name resolution functionality.
		class AsyncDns : public Object
		{
			// Begins an asynchronous request to resolve a DNS host name or IP address to an IPAddress instance.
			public: static IAsyncResult *BeginResolve(const String &hostName, AsyncCallback requestCallback, Object *stateObject);
			// Ends an asynchronous request for DNS information.
			public: static IPHostEntry EndResolve(IAsyncResult *asyncResult);
		};
		//----------------------------------------------------
	};
};
//**************************************************************************************************************//

#endif
