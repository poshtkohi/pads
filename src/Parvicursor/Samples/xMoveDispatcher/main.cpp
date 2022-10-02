/**
#define meta ...
printf("%s\n", meta);
**/

//---------------------------------------
#include <general.h>
#include <System/BasicTypes/BasicTypes.h>
#include <System/String/String.h>
#include <System.Collections/ArrayList/ArrayList.h>
#include <System.Net/NetworkCredential/NetworkCredential.h>

#include <Parvicursor/xThread/xThreadClient/xThreadClient.h>
#include <Parvicursor/xThread/xThreadCollection.h>
#include <Parvicursor/xDFS/xDFSClients/FileStreamClient/FileStreamClient.h>

#include "../xMoveDLL/xMove.h"

using namespace System;
using namespace System::IO;
using namespace System::Net;

using namespace Parvicursor::xThread;
using namespace Parvicursor::xDFS::xDFSClients;

NetworkCredential nc = NetworkCredential("user", "pass");
ArrayList errors = ArrayList();
//---------------------------------------
#include <vector>
using namespace std;

struct LinkInfo
{
	const char *sourceFilename;
	const char *sourceAddress;
	const char *sourceComputeNodeAddress;
	Int64 offset;
	Int64 length;
	const char *sinkFilename;
	const char *sinkAddress;
	const char *sinkComputeNodeAddress;
	xMove *xmove;
	xThreadCollection *xtc;
	xThreadClient *client;
	ArrayList *errors;
};

vector<LinkInfo *> links;
//---------------------------------------
int main(int argc, char **argv)
{
	const char *sourceFilename = "C:/thesis.pdf";
	const char *sourceAddress = "127.0.0.1";
	const char *sinkFilename = "C:/thesis1.pdf";
	const char *sinkAddress = "127.0.0.1";

#if defined WIN32 || WIN64
	String dllFilename = "E:/Projects/Projects/My .NET Projects/C++/Parvicursor/Samples/xMoveDispatcher/bin/Debug/xMoveDLL.dll";
#else
	String dllFilename = "/root/devel/Parvicursor/Samples/xMoveDispatcher/bin/Debug/xMoveDLL.dll";
#endif

	Int32 blockSize = 128*1024;
	FileStreamClient *sourceFile = new FileStreamClient(String(sourceFilename), System::IO::Open, System::IO::Read, blockSize, String(sourceAddress), nc, true); //
	Int64 offset = 0;
	Int64 length = 0;

	try { length = sourceFile->get_Length(); }
	catch(System::Exception &e) { printf("%s\n", e.get_Message().get_BaseStream()); sourceFile->Close(); delete sourceFile; return -1;}
	catch(...) { printf("An unknown error was occurred.\n"); sourceFile->Close(); delete sourceFile; return -1; }

	sourceFile->Close();
	delete sourceFile;

	cout << "length: " << length << endl;

	//////////////////////////////////////////////////////////////////////////
	// Link information
	LinkInfo *link1 = new LinkInfo();
	link1->sourceFilename = sourceFilename;
	link1->sourceAddress = sourceAddress;
	link1->sourceComputeNodeAddress = "127.0.0.1";
	link1->sinkFilename = sinkFilename;
	link1->sinkAddress = sinkAddress;
	link1->sinkComputeNodeAddress = "127.0.0.1";
	link1->offset = 0;
	link1->offset = 0;
	link1->xmove = null;
	link1->xtc = null;
	link1->client = null;
	link1->errors = new ArrayList();

	LinkInfo *link2 = new LinkInfo();
	link2->sourceFilename = sourceFilename;
	link2->sourceAddress = sourceAddress;
	link2->sourceComputeNodeAddress = "127.0.0.1";
	link2->sinkFilename = sinkFilename;
	link2->sinkAddress = sinkAddress;
	link2->sinkComputeNodeAddress = "127.0.0.1";
	link2->offset = 0;
	link2->offset = 0;
	link2->xmove = null;
	link2->xtc = null;
	link2->client = null;
	link2->errors = new ArrayList();

	links.push_back(link1);
	links.push_back(link2);

	Int32 a = length / links.size();
	Int32 b = length % links.size();

	for(register Int32 i = 0 ; i < links.size() ; i++)
	{
		Int64 l = a;
		Int64 o = i * a;

		if(i == links.size() - 1 && b != 0)
			l += b;

		LinkInfo *info = links[i];
		info->offset = o;
		info->length = l;

		cout << "offset: " << o << " length: " << l << endl;

		info->xmove = new xMove(info->sourceFilename, info->sourceAddress, info->sourceComputeNodeAddress, info->offset, info->length, blockSize, info->sinkFilename, info->sinkAddress, info->sinkComputeNodeAddress, true);
		info->xtc = new xThreadCollection();
		info->xtc->AddNewThreadInstance(info->xmove, dllFilename);
		info->client = new xThreadClient(info->xtc, info->sourceComputeNodeAddress, &nc, true, info->errors); //
	}
	//////////////////////////////////////////////////////////////////////////
	for(register Int32 i = 0 ; i < links.size() ; i++)
	{
		LinkInfo *info = links[i];

		try
		{
			info->client->Run();
			info->client->WaitForSessionInitiation();

			while(info->client->get_IsAlive())
			{
				if(info->client->get_AreAllThreadsCompleted())
				{
					for(register Int32 i = 0 ; i < info->xtc->get_Count() ; i++)
						info->client->SyncReceiveOneThread(i);

					cout << "blockSize: " <<  info->xmove->blockSize << endl;

					for(register Int32 i = 0 ; i < info->xmove->remoteErrors->get_Count() ; i++)
					{
						System::Exception *e = (System::Exception *)info->xmove->remoteErrors->get_Value(i);
						printf("RemoteError%d: %s\n", i + 1, e->get_Message().get_BaseStream());
					}

					info->client->TerminateSession();
					break;

				}

				Thread::Sleep(1);
			}
		}
		catch(System::Exception &e)
		{
			printf("In link %d. %s\n", i + 1, e.get_Message().get_BaseStream());
			continue;
		}
		catch(...)
		{
			printf("In link %d. An unknown error was occurred.\n", i + 1);
			continue;
		}

		for(register Int32 j = 0 ; j < info->errors->get_Count() ; j++)
		{
			System::Exception *e = (System::Exception *)info->errors->get_Value(i);
			//printf("%s\n", ((Exception)(*(Exception *)errors->get_Value(i))).get_Message().get_BaseStream());
			printf("In link %d. Error%d: %s\n", i + 1, j + 1, e->get_Message().get_BaseStream());
			delete e;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	
	for(register Int32 i = 0 ; i < links.size() ; i++)
	{
		LinkInfo *info = links[i];
		delete info->xtc;
		delete info->client;
		delete info->xmove;
		delete info->errors;

		delete info;
	}
	//////////////////////////////////////////////////////////////////////////
	
/*	const char *sourceComputeNodeAddress = "127.0.0.1";
	const char *sinkComputeNodeAddress = "127.0.0.1";

	xMove *xmove = null;
	xThreadCollection *xtc = null;
	xThreadClient *client = null;

	try
	{
		xmove = new xMove(sourceFilename, sourceAddress, sourceComputeNodeAddress, offset, length, blockSize, sinkFilename, sinkAddress, sinkComputeNodeAddress, true);

		xtc = new xThreadCollection();

#if defined WIN32 || WIN64
		xtc->AddNewThreadInstance(xmove, "E:/Projects/Projects/My .NET Projects/C++/Parvicursor/Samples/xMoveDispatcher/bin/Debug/xMoveDLL.dll");
#else
		xtc->AddNewThreadInstance(&xmove, "/root/devel/Parvicursor/Samples/xMoveDispatcher/bin/Debug/xMoveDLL.dll");
#endif

		client = new xThreadClient(xtc, sourceComputeNodeAddress, &nc, true, &errors); //
		client->Run();
		client->WaitForSessionInitiation();

		printf("xtc->get_Count(): %d\n", xtc->get_Count());///

		while(client->get_IsAlive())
		{
			if(client->get_AreAllThreadsCompleted())
			{
				for(register Int32 i = 0 ; i < xtc->get_Count() ; i++)
					client->SyncReceiveOneThread(i);

				cout << "blockSize: " <<  xmove->blockSize << endl;

				for(register Int32 i = 0 ; i < xmove->remoteErrors->get_Count() ; i++)
				{
					System::Exception *e = (System::Exception *)xmove->remoteErrors->get_Value(i);
					printf("RemoteError%d: %s\n", i + 1, e->get_Message().get_BaseStream());
				}

				client->TerminateSession();
				break;

			}

			Thread::Sleep(1);
		}
	}
	catch(System::Exception &e)
	{
		printf("%s\n", e.get_Message().get_BaseStream());
		goto Cleanup;
	}
	catch(...)
	{
		printf("An unknown error was occurred.\n");
		goto Cleanup;
	}

	for(register Int32 i = 0 ; i < errors.get_Count() ; i++)
	{
		System::Exception *e = (System::Exception *)errors.get_Value(i);
		//printf("%s\n", ((Exception)(*(Exception *)errors->get_Value(i))).get_Message().get_BaseStream());
		printf("Error%d: %s\n", i + 1, e->get_Message().get_BaseStream());
		delete e;
	}

Cleanup:

	if(xtc != null)
		delete xtc;

	if(client != null)
		delete client;

	if(xmove != null) 
		delete xmove;*/

	return 0;
}
//---------------------------------------