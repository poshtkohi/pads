// xDFSServer_exe.cpp : Defines the entry point for the console application.
//


//---------------------------------------
#include "../Parvicursor/Parvicursor/xDFS/xDFSServer/MainServer/Server.h"
//---------------------------------------
using namespace System;
using namespace System::Net;
using namespace System::Net::Sockets;

using namespace Parvicursor::xDFS;
using namespace Parvicursor::Serialization;
using namespace Parvicursor::Net;
//---------------------------------------

//#include <typeinfo>
//#define NDEBUG
/*#include <signal.h>
void SignalHandler(Int32 signum)
{

    //Thread::Sleep(1000000);
    //signal(signum, SIG_DFL);
    //kill(getpid(), signum);

}*/

int main(int argc, char* argv[])
{
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

	///Parvicursor_network_cleanup();
	return ret;
}
//---------------------------------------
