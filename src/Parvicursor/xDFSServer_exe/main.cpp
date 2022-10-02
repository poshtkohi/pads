// main.cpp : Defines the entry point for the console application.
//

//---------------------------------------
#include "../Parvicursor/Parvicursor/xDFS/xDFSServer/MainServer/Server.h"
//#include "../Parvicursor/Parvicursor/HybirdConcurrencyPattern/HcpManager.h"
//---------------------------------------
using namespace System;
using namespace System::Net;
using namespace System::Net::Sockets;

using namespace Parvicursor::xDFS;
using namespace Parvicursor::Serialization;
using namespace Parvicursor::Net;
//using namespace Parvicursor::HybirdConcurrencyPattern;
//---------------------------------------


// For mmap problems.
/*#if !defined WIN32 || WIN64
#include <typeinfo>
#include <signal.h> SIGBUS 
void SignalHandler(Int32 signum)
{
	return ;
}
void SignalHandler(Int32 signum)
{

    //Thread::Sleep(1000000);
    //signal(signum, SIG_DFL);
    //kill(getpid(), signum);

}

#endif*/

int main(int argc, char* argv[])
{

	//HcpManager *hcpm = new HcpManager(8080);
	/*Thread::Sleep(1000);

	delete hcpm;

	cout << "im in the exit" << endl;
	Thread::Sleep(1000);*/

	//return 10;

	int ret = 0;

    /*typedef void (*SignalHandlerPointer)(Int32);
    SignalHandlerPointer previousHandler = signal(SIGSEGV, SignalHandler);
    if(previousHandler ==  SIG_ERR)
    {
        printf("SIG_ERR: %s\n", (const char *)_sys_errlist[errno]);
    }*/

    xDFSServer *server = null;

	try
	{
		server = new xDFSServer();
		server->Start();
	}
	catch(SocketException &e)
	{
		printf("error: %s\n", e.get_Message().get_BaseStream());
		printf("ErrorCode: %d\n", e.get_ErrorCode());
		ret = -1;
	}
	catch(ObjectDisposedException &ee)
	{
		printf("error: %s\n", ee.get_Message().get_BaseStream());
		ret = -1;
	}
	catch(System::Exception &ee)
	{
		printf("error: %s\n", ee.get_Message().get_BaseStream());
		ret = -1;
	}
	catch(...)
	{
		printf("error: Unknown error\n");
		ret = -1;
	}

	if(server != null)
        delete server;

	//Parvicursor_network_cleanup();
	return ret;
}
//---------------------------------------
