// main.cpp : Defines the entry point for the console application.
// In linux, to run this program, use the following command in shell:
// LD_LIBRARY_PATH=/root/projects/Samples/xThreadPiClacDispatcher/Debug/:$LD_LIBRARY_PATH
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
#define xThreadPiCalc_IMPORTS
//#define xThreadBase_IMPORTS
#include "../xThreadPiCalcDLL/xThreadPiCalcDLL.h"

//#define Sample_IMPORTS
//#include "../xThreadSampleDLL/Sample.h"
//---------------------------------------
NetworkCredential nc = NetworkCredential("user", "pass");
//ArrayList errors0 = ArrayList();
//ArrayList errors1 = ArrayList();
//---------------------------------------
int main(int argc, char *args[])
{
	ArrayList *xthreads = new ArrayList();
	const Int32 nodeNum = 1;
	const String nodes [nodeNum] = {"localhost"/*, "localhost"*/};
	const Int32 xThreadPerNodeNum = 2;
	const Int32 from = 1;
	const Int32 to = 1000;
	Int32 physicalxThreads = nodeNum * xThreadPerNodeNum;
	//--- Parallel Algorithm -----------
	Int32 total = to - from;
	Int32 totalPerThread = total / physicalxThreads;
	Int32 totalPerThread_mod = total % physicalxThreads;
	Int32 i = 0, one = 0;
	while(from + i * totalPerThread < total)
	{
		if(i != 0)
			one = 1;
		xthreads->Add(new xThreadPiCalc(from + i * totalPerThread + one, totalPerThread));
		i++;
	}
	//if(totalPerThread_mod != 0)
	//	xthreads->Add(new xThreadPiCalc(from + i * totalPerThread + one, totalPerThread_mod));

	for(register Int32 i = 0 ; i < xthreads->get_Count(); i++)
	{
		xThreadPiCalc *temp = (xThreadPiCalc *)xthreads->get_Value(i);
		printf("startDigitNum: %d numDigits: %d\n", temp->startDigitNum, temp->numDigits);
	}
	//--- A small master-slave middleware implementation ----
	printf("xThreadNum: %d\n", xthreads->get_Count());
	ArrayList *collections = new ArrayList();
	register Int32 j = 0;
	register Int32 current = 0;
	while(j < xthreads->get_Count() - 1)
	{
		 xThreadCollection *xtc = new xThreadCollection();
		for(register Int32 k = 0 ; k < xThreadPerNodeNum && current < xthreads->get_Count() ; k++)
		{
			xThreadPiCalc *temp = (xThreadPiCalc *)xthreads->get_Value(j + k);
#if defined WIN32 || WIN64
			xtc->AddNewThreadInstance(temp, "E:/Projects/Projects/My .NET Projects/C++/Samples/xThreadPiClacDispatcher/Debug/xThreadPiCalcDLL.dll");
#else
			xtc->AddNewThreadInstance(temp, "/root/projects/Samples/xThreadPiClacDispatcher/Debug/libxThreadPiCalcDLL.so");
#endif
			current++;
		}
		collections->Add(xtc);
		j += xThreadPerNodeNum;
	}

	Int32 remainder = xthreads->get_Count() % physicalxThreads;
	if(remainder != 0)
	{
		for(register Int32 i = remainder - 1 ; i >= 0 ; i--)
		{
			xThreadPiCalc *temp = (xThreadPiCalc *)xthreads->get_Value(xthreads->get_Count() - 1 - i);
#if defined WIN32 || WIN64
			((xThreadCollection *)collections->get_Value(collections->get_Count() - 1))->AddNewThreadInstance(temp, "E:/Projects/Projects/My .NET Projects/C++/Samples/xThreadPiClacDispatcher/Debug/xThreadPiCalcDLL.dll");
#else
			((xThreadCollection *)collections->get_Value(collections->get_Count() - 1))->AddNewThreadInstance(temp, "/root/projects/Samples/xThreadPiClacDispatcher/Debug/libxThreadPiCalcDLL.so");
#endif
		}
	}
	printf("collections: %d\n", collections->get_Count());
	xThreadClient **clients = new xThreadClient *[collections->get_Count()];
	ArrayList **errors = new ArrayList *[collections->get_Count()];
	for(register Int32 i = 0 ; i < collections->get_Count() ; i++)
	{
		xThreadCollection *xtc = (xThreadCollection *)collections->get_Value(i);
		errors[i] = new ArrayList();
		clients[i] = new xThreadClient(xtc, nodes[i], &nc, false, errors[i]);
	}
	//--- Execute our implemented small middleware ---
	try
	{
		for(register Int32 i = 0 ; i < collections->get_Count() ; i++)
			clients[i]->Run();

		for(register Int32 i = 0 ; i < collections->get_Count() ; i++)
			clients[i]->WaitForSessionInitiation();

		for(register Int32 i = 0 ; i < collections->get_Count() ; i++)
		{
			while(clients[i]->get_IsAlive())
			{
				if(clients[i]->get_AreAllThreadsCompleted())
				{
					xThreadCollection *xtc = (xThreadCollection *)collections->get_Value(i);
					for(Int32 j = 0 ; j < xtc->get_Count() ; j++)
					{
						clients[i]->SyncReceiveOneThread(j);
						xThreadPiCalc *calc = (xThreadPiCalc *)xtc->get_Value(j)->objInstance;
						cout << "from: " << calc->startDigitNum << " to: " << calc->startDigitNum + calc->numDigits << " result: " << calc->result << endl;
					}

					clients[i]->TerminateSession();
				}

				Thread::Sleep(1);
			}
		}
	}
	catch(System::Exception &e)
	{
		printf("%s\n", e.get_Message().get_BaseStream());
	}
	catch(...)
	{
		printf("An unknown error was occurred.\n");
	}
	//--- Report likely remote errors ---------
	for(register Int32 i = 0 ; i < collections->get_Count() ; i++)
	{
		for(register Int32 j = 0 ; j < errors[i]->get_Count() ; j++)
		{
			System::Exception *e = (System::Exception *)errors[i]->get_Value(j);
			printf("Error%d for client%d: %s\n", j + 1, i + 1, e->get_Message().get_BaseStream());
			delete e;
		}
	}
	//--- Dispose allocated objects -----------
	for(register Int32 i = 0 ; i < collections->get_Count() ; i++)
		delete clients[i];
	delete clients;

	for(register Int32 i = 0 ; i < collections->get_Count() ; i++)
		delete errors[i];
	delete errors;

	for(register Int32 i = 0 ; i < collections->get_Count() ; i++)
		delete ((xThreadCollection *)collections->get_Value(i));
	delete collections;

	for(register Int32 i = 0 ; i < xthreads->get_Count() ; i++)
		delete ((xThreadPiCalc *)xthreads->get_Value(i));
	delete xthreads;
	//-----------------------------------------
	/*xThreadPiCalc s1 = xThreadPiCalc(1, 200);
	Sample s2 = Sample(10, 20);

	xThreadCollection **collections = new xThreadCollection *[2];
	for(register Int32 i = 0 ; i < 2 ; i++)
		collections[i] = new xThreadCollection();

	collections[0]->AddNewThreadInstance(&s1, "E:/Projects/Projects/My .NET Projects/C++/Samples/xThreadPiClacDispatcher/Debug/xThreadPiCalcDLL.dll");
	collections[1]->AddNewThreadInstance(&s2, "E:/Projects/Projects/My .NET Projects/C++/Samples/xThreadSampleDLL/Debug/xThreadSampleDLL.dll");

	xThreadClient **clients = new xThreadClient *[2];
	clients[0] = new xThreadClient(collections[0], "localhost", &nc, false, &errors0);
	clients[1] = new xThreadClient(collections[1], "localhost", &nc, false, &errors1);

	for(register Int32 i = 0 ; i < 2 ; i++)
	{
		clients[i]->Run();
		printf("Run %d\n", i);
		//clients[i]->WaitForSessionInitiation();
	}Thread::Sleep(1000);

	for(register Int32 i = 0 ; i < 2 ; i++)
	{
		clients[i]->WaitForSessionInitiation();
		printf("WaitForSessionInitiation %d\n", i);
	}

	for(register Int32 i = 0 ; i < 2 ; i++)
	{
		while(clients[i]->get_IsAlive())
		{
			if(clients[i]->get_AreAllThreadsCompleted())
			{
				xThreadCollection *xtc = (xThreadCollection *)collections[i];
				for(Int32 j = 0 ; j < xtc->get_Count() ; j++)
				{
					clients[i]->SyncReceiveOneThread(j);
					if(i == 0)
					{
						xThreadPiCalc *calc = (xThreadPiCalc *)xtc->get_Value(j)->objInstance;
						cout << "from: " << calc->startDigitNum << " to: " << calc->startDigitNum + calc->numDigits << " result: " << calc->result << endl;
					}
					if(i == 1)
					{
						Sample *s = (Sample *)xtc->get_Value(j)->objInstance;
						cout << "s->x: " << s->x << " s->y: " << s->y << " s->z: " << s->z << endl;
					}
				}

				clients[i]->TerminateSession();
			}

			Thread::Sleep(1);
		}
	}*/

/*	xThreadCollection *xtc = null;
	xThreadClient *client = null;
	try
	{
		xThreadPiCalc s = xThreadPiCalc(1, 200);
		//xThreadPiCalc s2 = xThreadPiCalc(4);

		xtc = new xThreadCollection();

#if defined WIN32 || WIN64
		xtc->AddNewThreadInstance(&s, "E:/Projects/Projects/My .NET Projects/C++/Samples/xThreadPiClacDispatcher/Debug/xThreadPiCalcDLL.dll");
		//xtc->AddNewThreadInstance(&s, "C:/Drives/My .NET Projects/C++/Samples/xThreadPiClacDispatcher/Debug/xThreadPiCalcDLL.dll");
		//xtc->AddNewThreadInstance(&s2, "C:/Drives/My .NET Projects/C++/Samples/xThreadPiClacDispatcher/Debug/xThreadPiCalcDLL.dll");

		//xtc->AddNewThreadInstance(&s, "/root/Samples/xThreadPiClacDispatcher/Debug/libxThreadPiCalcDLL.so"); //!!!! from Windows machine to run remote Linux xThread thread !!!!
#else
		xtc->AddNewThreadInstance(&s, "/root/Samples/xThreadPiClacDispatcher/Debug/libxThreadPiCalcDLL.so");
		//xtc->AddNewThreadInstance(&s2, "/root/Samples/xThreadPiClacDispatcher/Debug/libxThreadPiCalcDLL.so");
#endif

		client = new xThreadClient(xtc, "localhost", &nc, false, &errors);
		client->Run();
		client->WaitForSessionInitiation();

		while(client->get_IsAlive())
		{
			if(client->get_AreAllThreadsCompleted())
			{
				for(Int32 i = 0 ; i < xtc->get_Count() ; i++)
					client->SyncReceiveOneThread(i);

				client->TerminateSession();

				cout << "from: " << s.startDigitNum << " to: " << s.startDigitNum + s.numDigits << " result: " << s.result << endl;
			}

			Thread::Sleep(1);
		}

	}
	catch(Exception &e)
	{
		printf("%s\n", e.get_Message().get_BaseStream());
	}
	catch(...)
	{
		printf("An unknown error was occurred.\n");
	}

    for(register Int32 i = 0 ; i < errors.get_Count() ; i++)
    {
        Exception *e = (Exception *)errors.get_Value(i);
        //printf("%s\n", ((Exception)(*(Exception *)errors->get_Value(i))).get_Message().get_BaseStream());
        printf("Error%d: %s\n", i + 1, e->get_Message().get_BaseStream());
        delete e;
    }

	if(client != null)
		delete client;

	if(xtc != null)
		delete xtc;*/

	getchar();

	return 0;
}
//---------------------------------------
