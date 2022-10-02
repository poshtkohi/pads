// main.cpp : Defines the entry point for the console application.
// In linux, to run this program, use the following command in shell:
// LD_LIBRARY_PATH=/root/projects/Samples/xThreadThirdPartyTransferDispatcher/Debug/:$LD_LIBRARY_PATH
// This makes the SO library available at runtime.
//

//---------------------------------------
#include "../../Parvicursor/general.h"
#include "../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../Parvicursor/System/String/String.h"
#include "../../Parvicursor/System.Collections/ArrayList/ArrayList.h"
#include "../../Parvicursor/System.Net/NetworkCredential/NetworkCredential.h"
#include "../../Parvicursor/Parvicursor/xThread/xThreadClient/xThreadClient.h"
#include "../../Parvicursor/Parvicursor/xThread/xThreadCollection.h"
#include "../../Parvicursor/Parvicursor/xThread/xThreadBase.h"
//---------------------------------------
using namespace System;
using namespace System::Collections;
using namespace System::Net;
using namespace System::Net::Sockets;
using namespace System::Threading;

using namespace Parvicursor::xThread;
//---------------------------------------
#define xThreadThirdPartyTransfer_IMPORTS
#include "../xThreadThirdPartyTransferDLL/xThreadThirdPartyTransferDLL.h"
//---------------------------------------
NetworkCredential nc = NetworkCredential("user", "pass");
ArrayList errors = ArrayList();
//---------------------------------------
#include <signal.h>
xThreadCollection *xtc = null;
xThreadClient *client = null;
static void StopRequested_signal_handler(int sig)
{
	printf("\nCTRL+C was pressed.\n");
	printf("The client program is terminating.\nPlease wait ...\n");

	if(client != null)
		client->TerminateSession();

	if(client != null)
		delete client;

	if(xtc != null)
		delete xtc;

	exit(0);
	return;
}
//---------------------------------------
int main(int argc, char *args[])
{
	signal(SIGINT, StopRequested_signal_handler);

	const char *dest = "localhost";
	const char *localFilename = "C:/test/test.pdf";
	const char *remoteFilename = "C:/test/test1.pdf";
	Int32 parallel = 10;
	Int32 blockSize = 256*1024;
	Int32 tcpBufferSize = 256*1024;

	struct xThreadThirdPartyTransferContextInput input;
	input.command = __GetTransferredBytes__;

	struct xThreadThirdPartyTransferContextOutput output;
	Int32 outputLength;

	try
	{
		xThreadThirdPartyTransfer s = xThreadThirdPartyTransfer(dest, localFilename, remoteFilename, parallel, blockSize, tcpBufferSize);

		xtc = new xThreadCollection();

#if defined WIN32 || WIN64
		xtc->AddNewThreadInstance(&s, "E:/Projects/Projects/My .NET Projects/C++/Samples/xThreadThirdPartyTransferDispatcher/Debug/xThreadThirdPartyTransferDLL.dll");
		//xtc->AddNewThreadInstance(&s, "C:/Drives/My .NET Projects/C++/Samples/xThreadThirdPartyTransferDispatcher/Debug/xThreadThirdPartyTransferDLL.dll");
#else
		xtc->AddNewThreadInstance(&s, "/root/Samples/xThreadThirdPartyTransferDispatcher/Debug/libxThreadThirdPartyTransferDLL.dll");
#endif

		client = new xThreadClient(xtc, "localhost", &nc, false, &errors);
		client->Run();
		client->WaitForSessionInitiation();

		Int64 last = 0, current = 0;

		while(client->get_IsAlive())
		{
			/*if(client->get_AreAllThreadsCompleted())
			{
				for(Int32 i = 0 ; i < xtc->get_Count() ; i++)
					client->SyncReceiveOneThread(i);


				cout << "dest: " << s.dest << " localFilename: " << s.localFilename << " remoteFilename: " << s.remoteFilename << endl;
				break;
			}
			printf("1\n");
			Thread::Sleep(1);*/
			client->AuxiliaryMehtodOneThread(0, &input, sizeof(struct xThreadThirdPartyTransferContextInput), &output, &outputLength);

			if(output.transferredBytes == (Int64)-1)
			{
				//printf("goto Terminate\n");
				goto Terminate;
			}


			current = output.transferredBytes;

//#if defined WIN32 || WIN64
//				printf("Session: %d transferredBytes: %I64u\n", i + 1, output.transferredBytes);
//#else
//				printf("Session: %d transferredBytes: %llu\n", i + 1, output.transferredBytes);
//#endif

			Thread::Sleep(5000);
			printf("\rspeed: %.2f MB/s\r", ((Float)(current - last))/(5 * 1024.0 * 1024.0));
			last = current;
		}
Terminate:
		client->TerminateSession();
		for(register Int32 i = 0 ; i < s.remoteErrors->get_Count() ; i++)
		{
			System::Exception *e = (System::Exception *)s.remoteErrors->get_Value(i);
			printf("RemoteError%d: %s\n", i + 1, e->get_Message().get_BaseStream());
		}

	}
	catch(System::Exception &ee)
	{
		printf("%s\n", ee.get_Message().get_BaseStream());
	}
	catch(...)
	{
		printf("An unknown error was occurred.\n");
	}

    for(register Int32 i = 0 ; i < errors.get_Count() ; i++)
    {
        System::Exception *e = (System::Exception *)errors.get_Value(i);
        printf("Error%d: %s\n", i + 1, e->get_Message().get_BaseStream());
        delete e;
    }

	if(client != null)
		delete client;

	if(xtc != null)
		delete xtc;
	return 0;
}
//---------------------------------------
