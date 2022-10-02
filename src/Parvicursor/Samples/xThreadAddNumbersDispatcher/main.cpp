// xThreadAddNumbersDispatcher.cpp : Defines the entry point for the console application.
//

// In linux, to run this program, use the following command in shell:
// LD_LIBRARY_PATH=/root/projects/Samples/xThreadAddNumbersDispatcher/Debug/:$LD_LIBRARY_PATH
// This makes the SO library available at runtime.

// main.cpp : Defines the entry point for the console application.
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

#define Sample_IMPORTS
#include "../xThreadSampleDLL/Sample.h"
//---------------------------------------
NetworkCredential nc = NetworkCredential("user", "pass");
ArrayList errors = ArrayList();
//---------------------------------------
int main(int argc, char *args[])
{
	/*Int32 x = 10;
	char *y = (char *)&x;
	Int32 z = (Int32)(*y);

	printf("z: %d\n", z);

	return 0;*/

	bool secure = true;
	xThreadCollection *xtc = null;
	xThreadClient *client = null;

	struct SampleContextInput input;
	input.command = __Add__;
	input.x = 1;
	input.y = 2;

	struct SampleContextOutput output;
	Int32 outputLength;

	try
	{
		Sample s1 = Sample(10, 20);
		Sample s2 = Sample(30, 40);

		xtc = new xThreadCollection();

#if defined WIN32 || WIN64
		xtc->AddNewThreadInstance(&s1, "E:/Projects/Projects/My .NET Projects/C++/Samples/xThreadAddNumbersDispatcher/Debug/xThreadSampleDLL.dll");
		xtc->AddNewThreadInstance(&s2, "E:/Projects/Projects/My .NET Projects/C++/Samples/xThreadAddNumbersDispatcher/Debug/xThreadSampleDLL.dll");
		//xtc->AddNewThreadInstance(&s1, "C:/Drives/My .NET Projects/C++/Samples/xThreadAddNumbersDispatcher/Debug/xThreadSampleDLL.dll");
		//xtc->AddNewThreadInstance(&s2, "C:/Drives/My .NET Projects/C++/Samples/xThreadAddNumbersDispatcher/Debug/xThreadSampleDLL.dll");

		// From a Windows machine we might run a remote Linux xThread thread instance  !!!!
		//xtc->AddNewThreadInstance(&s, "/root/projects/Samples/xThreadAddNumbersDispatcher/Debug/libxThreadSampleDLL.so");
#else
		xtc->AddNewThreadInstance(&s1, "/root/projects/Samples/xThreadAddNumbersDispatcher/Debug/libxThreadSampleDLL.so");
		xtc->AddNewThreadInstance(&s2, "/root/projects/Samples/xThreadAddNumbersDispatcher/Debug/libxThreadSampleDLL.so");
#endif

		client = new xThreadClient(xtc, "localhost", &nc, secure, &errors);
		client->Run();
		client->WaitForSessionInitiation();

		while(client->get_IsAlive())
		{
			if(client->get_AreAllThreadsCompleted())
			{
				for(register Int32 i = 0 ; i < xtc->get_Count() ; i++)
					client->SyncReceiveOneThread(i);

				client->TerminateSession();

				cout << "s1.x: " << s1.x << " s1.y: " << s1.y << " s1.z: " << s1.z << endl;
				cout << "s2.x: " << s2.x << " s2.y: " << s2.y << " s2.z: " << s2.z << endl;

			}
			for(register Int32 i = 0 ; i < xtc->get_Count() ; i++)
			{
				client->AuxiliaryMehtodOneThread(i, &input, sizeof(struct SampleContextInput), &output, &outputLength);
				printf("s%d x: %d y: %d z:%d\n", i + 1, input.x, input.y, output.z);
			}
			//client->TerminateSession();
			//break;

			/*for(Int32 i = 0 ; i < xtc->get_Count() ; i++)
			client->SyncReceiveOneThread(i);

			cout << "s1.x: " << s1.x << " s1.y: " << s1.y << " s1.z: " << s1.z << endl;
			cout << "s2.x: " << s2.x << " s2.y: " << s2.y << " s2.z: " << s2.z << endl;*/

			Thread::Sleep(1);
		}

		client->TerminateSession();
	}
	catch(System::Exception &e)
	{
		printf("%s\n", e.get_Message().get_BaseStream());
	}
	catch(...)
	{
		printf("An unknown error was occurred.\n");
	}

	for(register Int32 i = 0 ; i < errors.get_Count() ; i++)
	{
		System::Exception *e = (System::Exception *)errors.get_Value(i);
		//printf("%s\n", ((Exception)(*(Exception *)errors->get_Value(i))).get_Message().get_BaseStream());
		printf("Error%d: %s\n", i + 1, e->get_Message().get_BaseStream());
		delete e;
	}

	if(client != null)
		delete client;

	if(xtc != null)
		delete xtc;

	getchar();

	return 0;
}
//---------------------------------------
