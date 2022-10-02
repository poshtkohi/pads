// AsyncResolver.cpp : Defines the entry point for the console application.
//

//---------------------------------------
#include "../../Parvicursor/general.h"
#include "../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../Parvicursor/System/String/String.h"
#include "../../Parvicursor/System.Threading/Mutex/Mutex.h"
#include "../../Parvicursor/System.Threading/ConditionVariable/ConditionVariable.h"
#include "System.Net/Dns/AsyncDns.h"
//---------------------------------------
using namespace System;
using namespace System::Net;
//---------------------------------------
// State object for resolving the domain asynchronously
class ResolveState : public Object
{
	public: String domain;
};
// The following parameters are used to synchronize between the main thread and the execution of asynchronous requests.
static Mutex mutex = Mutex();
static ConditionVariable cv = ConditionVariable(&mutex);
static Int32 completedNum = 0;
//---------------------------------------
static void OnResolve(IAsyncResult *ar)
{
	// Convert the ResolveState object to a ResolveState object.
	ResolveState *state = (ResolveState *)ar->AsyncState;
	try
	{
		// End the asynchronous request.
		IPHostEntry hostEntry = AsyncDns::EndResolve(ar);

		// Prints all of the found IP addresses into console.
		for(Int32 i = 0 ; i < hostEntry.get_AddressListLength() ; i++)
		{
			IPAddress ip = hostEntry.get_AddressList(i);
			printf("Domain: %s IP Address %d: %s\n", state->domain.get_BaseStream(), i , inet_ntoa(*(struct in_addr *)ip.GetAddressBytes()));
		}
	}
	catch(Exception &e)
	{
		printf("Error occurred in OnResolve)() for %s. Exception message: %s\n", state->domain.get_BaseStream(), e.get_Message().get_BaseStream());
	}
	catch(...)
	{
		printf("Error occurred in OnResolve() for %s. Unknown exception.\n", state->domain.get_BaseStream());
	}

	// Signals the main thread that the current asynchronous method was finished.
	mutex.Lock();
	completedNum++;
	cv.Signal();
	mutex.Unlock();
}
//---------------------------------------
int main(int argc, char *argv[])
{
	// Allocates the domain set to be resolved.
	const Int32 DomainNum = 10;
	String *domains = new String[DomainNum];
	domains[0] = "www.poshtkohi.ir";
	domains[1] = "www.microsoft.com";
	domains[2] = "www.kernel.org";
	domains[3] = "www.yahoo.com";
	domains[4] = "www.google.com";
	domains[5] = "www.msn.com";
	domains[6] = "mail.google.com";
	domains[7] = "mail.yahoo.com";
	domains[8] = "www.facebook.com";
	domains[9] = "www.nytimes.com";

	// Allocates the asynchronous states and issues the BeginResolve() method for each resolve request.
	ResolveState **states = new ResolveState*[DomainNum];
	for(register Int32 i  = 0 ; i < DomainNum ; i++)
	{
		// Create an instance of the ResolveState class.
		states[i] = new ResolveState();
		states[i]->domain = domains[i];
		// Begin an asynchronous request for information like host name, IP addresses, or
		// aliases for specified the specified URI.
		IAsyncResult *asyncResult = AsyncDns::BeginResolve(domains[i], OnResolve, states[i]);
	}

	// Waits until all asynchronous request complete.
	mutex.Lock();
	while(completedNum != DomainNum)
		cv.Wait();
	mutex.Unlock();

	// Releases the allocated objects.
	for(register Int32 i  = 0 ; i < DomainNum ; i++)
		delete states[i];

	delete []states;
	delete []domains;

	return 0;
}
//---------------------------------------
