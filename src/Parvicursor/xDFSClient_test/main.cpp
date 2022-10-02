// xDFSClient_test.cpp : Defines the entry point for the console application.
//

//---------------------------------------
#include "../Parvicursor/general.h"
#include "../Parvicursor/System.Collections/Hashtable/Hashtable.h"
#include "../Parvicursor/System.Collections/ArrayList/ArrayList.h"
#include "../Parvicursor/System/String/String.h"
#include "../Parvicursor/System/Convert/Convert.h"
#include "../Parvicursor/System.Threading/Timer/Timer.h"
#include "../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../Parvicursor/System.Collections/ArrayList/ArrayList.h"
#include "../Parvicursor/System.Net/NetworkCredential/NetworkCredential.h"
#include "../Parvicursor/Parvicursor/xDFS/xDFSClients/UploadClient/UploadClient.h"
#include "../Parvicursor/Parvicursor/xDFS/xDFSClients/DownloadClient/DownloadClient.h"
#include "../Parvicursor/Parvicursor/Profiler/ResourceProfiler.h"
//#include "../Parvicursor/Parvicursor/xDFS/xDFSClients/FileStreamClient/FileStreamClient.h"

#include <iostream>
#include <iomanip>
//---------------------------------------
using namespace System;
using namespace System::Collections;
using namespace System::Net;
using namespace System::Net::Sockets;
using namespace System::Threading;

using namespace Parvicursor::xDFS;
using namespace Parvicursor::Serialization;
using namespace Parvicursor::Net;
using namespace Parvicursor::xDFS::xDFSClients;
//---------------------------------------
//_________________________________

Parvicursor_RESOURCE_PROFILER_INITIALIZE()
//_________________________________

//#include <typeinfo>
//---------------------------------------
Object *ShowSpeed(Object *f);
const String Copyright(bool echo);
void Help();
void Run(String &command);

//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
UploadClient *upload_session = null;
DownloadClient *download_session = null;
Int64 sum = 0;
Int32 seconds = 0;
Int64 lastWritten = 0;
NetworkCredential nc("alireza", "furnaces2002");
ArrayList *errors = new ArrayList();
long double _ins = 0.0;

void MatMultiply(int *A, int *B ,int *C, int p)
{
	if(p <=0)
		return ;
	if(A == NULL || B == NULL || C == NULL)
	{
		printf("NULL\n");
		return ;
	}
	register int i = 0, j = 0, k = 0;
	register int temp = 0;

	for(i = 0 ; i < p ; i++)
	{
		for(j = 0 ; j < p; j++)
		{
			temp = 0;
			for(k = 0 ; k < p ; k++)
				temp += A[i*p + k]*B[k*p + j];
			C[i*p + j] = temp;
		}
	}

}
//---------------------------------------
int main(int argc, char* args[])
{
	/*
	//int n = 1000;
	//int *x = (int *)malloc(n * n * sizeof(int));
	//int *y = (int *)malloc(n * n * sizeof(int));
	//int *z = (int *)malloc(n * n * sizeof(int));


    //int *x = (int *)HeapAlloc(GetProcessHeap(), 0, n * n * sizeof(int));
	//int *y = (int *)HeapAlloc(GetProcessHeap(), 0, n * n * sizeof(int));
	//int *z = (int *)HeapAlloc(GetProcessHeap(), 0, n * n * sizeof(int));

	//int len = n * n * sizeof(int);
	//int *x = (int *)VirtualAlloc(NULL, len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	//int *y = (int *)VirtualAlloc(NULL, len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	//int *z = (int *)VirtualAlloc(NULL, len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	Parvicursor_RESOURCE_PROFILER_BEGIN()
	MatMultiply(x, y, z, n);
	Parvicursor_RESOURCE_PROFILER_END()
	getchar();
	return 0;

	*/


	/*String remotePath = "c:\\test.dat";
	String xDFSServerAddress = "localhost";
	UInt16 port = 8080;
	Int32 parallel = 1;
	Int32 blockSize = 256*1024;
	Int32 tcpBufferSize = 64*1024;
	bool secure = false;
	FileStreamClient *fsc = null;
	try
	{
		fsc = new FileStreamClient(remotePath, System::IO::OpenOrCreate, System::IO::ReadWrite, xDFSServerAddress, port, parallel, blockSize, tcpBufferSize, nc, secure);
	}
	catch(Exception &e)
	{
		//if(upload_session != null)
		//{
		//	upload_session->Close();
		//	delete upload_session;
		//}
		printf("%s\n", e.get_Message().get_BaseStream());
	}
	catch(...)
	{
		printf("unknown error was occured.\n");
	}

	if(fsc != null)
		delete fsc;


	return -1;*/

	if(args == null || argc == 1)
	{
		cout << Copyright(false).get_BaseStream() << endl;
		cout << "\nError: You must specify input argument(s)." << endl;
		return -1;
	}

	String arg = "";
	for(int i = 1 ; i < argc ; i++)
		arg += " " + String((const char *)args[i]);

	if(arg.Trim() == "")
	{
		cout << Copyright(false).get_BaseStream() << endl;
		cout << "\nError: You must specify input argument(s)." << endl;
		return -1;
	}
	/*if(arg.ToLower().IndexOf("-cmd") >= 0) // only for Windows platforms.
	{
		String path = Assembly.GetExecutingAssembly().Location;
		//Process.Start("cmd.exe", String.Format("/k \"SET root={0}&&echo %root%\"", path));
		Process.Start("cmd.exe", String.Format("/k \"{0}&&cd /D\"{1}\"\"", Copyright(true), path.Substring(0, path.LastIndexOf(@"\"))));
		return ;
	}*/
	if(arg.ToLower().IndexOf("-help") >= 0)
	{
		Help();
		return 0;
	}

	cout << Copyright(false).get_BaseStream() << endl;

	Run(arg);

	//FTSM upload test
	/*Callback callback = ShowSpeed;
	TimerCallback timerDelegate(callback);
	Timer timer(timerDelegate, upload_session, 0, 1000);

	cout << Copyright(false).get_BaseStream();
	//String readFilename = "F:/New_SUSE_Linux.vmdk";
	//String readFilename = "E:/dl/Software/openSUSE-11.0-DVD-i386.iso";
	String readFilename = "E:/dl/Software/SQLServer2005_SSMSEE.msi";
	//String remoteFilename = "G:/temp.dat";
	String remoteFilename = "/root/temp.dat";
	//String remoteFilename = "Z:/temp.msi";
	//String readFilename = "D:/Projects/Projects/films.rar";
	//String remoteFilename = "G:/films.rar";
	Int32 parallel = 10;
	Int32 tcpBufferSize = 256*1024;
	//String xDFSServerAddress = "192.168.119.132";
	String xDFSServerAddress = "localhost";
	NetworkCredential nc("user", "pass");
	bool secure = false;
	bool memmoryToMemoryTests = false;
	ArrayList *errors = new ArrayList();

	String _secure = "false";
	if(secure)
		_secure = "true";

	while(true)
	{
		try
		{
			errors->Clear();
			upload_session = new UploadClient(readFilename, remoteFilename, parallel, tcpBufferSize, xDFSServerAddress, nc, secure, memmoryToMemoryTests, errors);
			if(!memmoryToMemoryTests)
				cout << "\n\nTransferring  \n\n\tfile:" << readFilename.get_BaseStream() << " (" << upload_session->get_FileSize() << " Bytes)\n\nto \n\txDFS:" << xDFSServerAddress.get_BaseStream() << ":" << remoteFilename.get_BaseStream() << "\n\n\n\tParallel Streams: " << parallel << ", Secure(DotSec): " << _secure.get_BaseStream() << "\n\n\n" << endl;
			//else
			//{
			//	cout << "Memory-to-Memory Test.");
			//	cout << "\nTransferring  \n\n\tfile:{0} ({1} Bytes)\n\nto \n\txDFS:{2}:{3}\n\n\n\tParallel Streams: {4}, Secure(DotSec): {5}\n\n", localPath, long.MaxValue,
			//		hostname, remotePath, parallel, secure);
			//}
			upload_session->Run();
			//pthread_mutex_lock(&mutex);
			upload_session->Close();

			delete upload_session;
			upload_session = null;
			//pthread_mutex_unlock(&mutex);

			for(Int32 i = 0 ; i < errors->get_Count() ; i++)
			{
				Exception *e = (Exception *)errors->get_Value(i);
				//printf("%s\n", ((Exception)(*(Exception *)errors->get_Value(i))).get_Message().get_BaseStream());
				printf("%d %s\n", errors->get_Count(), e->get_Message().get_BaseStream());
				delete e;
			}
		}
		catch(Exception &e)
		{
			//if(upload_session != null)
			//{
			//	upload_session->Close();
			//	delete upload_session;
			//}
			printf("%s\n", e.get_Message().get_BaseStream());
		}
		catch(...)
		{
			printf("unknown error was occured.\n");
		}
		//break;
		System::Threading::Thread::Sleep(1000);
	}*/

	//FTSM download test
	//printf("hello\r\n");
	/*String writeFilename = "E:/test.zip";
	String remoteFilename = "E:/Windows_Server_Protocols.zip";
	Int32 parallel = 10;
	Int32 tcpBufferSize = 64*1024;
	Int32 blockSize = 256*1024;
	//String xDFSServerAddress = "192.168.119.132";
	String xDFSServerAddress = "localhost";
	//NetworkCredential nc("user", "pass");
	bool secure = false;
	bool memmoryToMemoryTests = false;
	//ArrayList *errors = new ArrayList();
	DownloadClient *download_session = null;
	bool isClientSideZeroCopyEnabled = false;
	bool isServerSideZeroCopyEnabled = false;

	String _secure = "false";
	if(secure)
		_secure = "true";

	while(true)
	{
		try
		{
			errors->Clear();
			download_session = new DownloadClient(writeFilename, remoteFilename, parallel, blockSize, tcpBufferSize, xDFSServerAddress, nc, secure, memmoryToMemoryTests, errors, isClientSideZeroCopyEnabled, isServerSideZeroCopyEnabled);
			//if(!memmoryToMemoryTests)
			//	cout << "\n\nTransferring  \n\n\tfile:" << readFilename.get_BaseStream() << " (" << upload_session->get_FileSize() << " Bytes)\n\nto \n\txDFS:" << xDFSServerAddress.get_BaseStream() << ":" << remoteFilename.get_BaseStream() << "\n\n\n\tParallel Streams: " << parallel << ", Secure(DotSec): " << _secure.get_BaseStream() << "\n\n\n" << endl;
			//else
			//{
			//	cout << "Memory-to-Memory Test.");
			//	cout << "\nTransferring  \n\n\tfile:{0} ({1} Bytes)\n\nto \n\txDFS:{2}:{3}\n\n\n\tParallel Streams: {4}, Secure(DotSec): {5}\n\n", localPath, long.MaxValue,
			//		hostname, remotePath, parallel, secure);
			//}
			download_session->Run();
			//pthread_mutex_lock(&mutex);
			download_session->Close();

			delete download_session;
			download_session = null;
			//pthread_mutex_unlock(&mutex);

			for(Int32 i = 0 ; i < errors->get_Count() ; i++)
			{
				Exception *e = (Exception *)errors->get_Value(i);
				//printf("%s\n", ((Exception)(*(Exception *)errors->get_Value(i))).get_Message().get_BaseStream());
				printf("%d %s\n", errors->get_Count(), e->get_Message().get_BaseStream());
				delete e;
			}
		}
		catch(Exception &e)
		{
			//if(upload_session != null)
			//{
			//	upload_session->Close();
			//	delete upload_session;
			//}
			printf("%s\n", e.get_Message().get_BaseStream());
		}
		catch(...)
		{
			printf("unknown error was occured.\n");
		}
		//break;
		System::Threading::Thread::Sleep(1000);
	}


	return 0;*/

	if(errors != null)
		delete errors;

	//_________________________________
	Parvicursor_RESOURCE_PROFILER_FINALIZE()
	//_________________________________

	///Parvicursor_network_cleanup();
	return 0;
}
//---------------------------------------
void Run(String &command)
{
	bool upload = true;
	String comm = command.ToLower();
	if(comm.IndexOf("-download") >= 0)
		upload = false;
	Int32 p1 = -1, p2 = -1;
	if((p1 = comm.IndexOf("-upload ")) >= 0 || (p1 = comm.IndexOf("-download ")) >= 0)
	{
		//------------------------------------------------------------
		String hostname = "localhost";
		if((p2 = comm.IndexOf("-hostname ")) >= 0)
		{
			p2 += String("-hostname ").get_Length();
			int p3 = comm.IndexOf("-", p2);
			if(p3 >= 0)
				hostname = command.Substring(p2, p3 -  p2).Trim();
			else
				hostname = command.Substring(p2).Trim();

			//cout << hostname.get_BaseStream() << endl;
		}
		//------------------------------------------------------------
		Int32 tcpBufferSize = 64 * 1024;
		if((p2 = comm.IndexOf("-tbs ")) >= 0)
		{
			p2 += String("-tbs ").get_Length();
			int p3 = comm.IndexOf("-", p2);
			try
			{
				if(p3 >= 0)
					tcpBufferSize = Convert::ToInt32(command.Substring(p2, p3 -  p2).Trim());
				else tcpBufferSize = Convert::ToInt32(command.Substring(p2).Trim());
			}
			catch(...)
			{
				cout << "Error: Bad format.(Usage: -tbs bytes number)\n" << endl;
				return ;
			}
			if(tcpBufferSize < 0)
			{
				cout << "Error in -tbs parameter: TCP Window size can not be negative.\n" << endl;
				return;
			}
			//cout << hostname);
		}
		//------------------------------------------------------------
		Int32 blockSize = 256 * 1024;
		if((p2 = comm.IndexOf("-bs ")) >= 0)
		{
			p2 += String("-bs ").get_Length();
			int p3 = comm.IndexOf("-", p2);
			try
			{
				if(p3 >= 0)
					blockSize = Convert::ToInt32(command.Substring(p2, p3 -  p2).Trim());
				else blockSize = Convert::ToInt32(command.Substring(p2).Trim());
			}
			catch(...)
			{
				cout << "Error: Bad format.(Usage: -tbs bytes number)\n" << endl;
				return ;
			}
			if(blockSize <= 0)
			{
				cout << "Error in -bs parameter: Block size can not be zero or negative.\n" << endl;
				return;
			}
			//cout << hostname);
		}
		//------------------------------------------------------------
		Int32 circularBufferCountClientSide = -1;
		if((p2 = comm.IndexOf("-cbc ")) >= 0)
		{
			p2 += String("-cbc ").get_Length();
			int p3 = comm.IndexOf("-", p2);
			try
			{
				if(p3 >= 0)
					circularBufferCountClientSide = Convert::ToInt32(command.Substring(p2, p3 -  p2).Trim());
				else
					circularBufferCountClientSide = Convert::ToInt32(command.Substring(p2).Trim());
			}
			catch(...)
			{
				cout << "Error: Bad format.(Usage: -cbc number)\n" << endl;
				return ;
			}
			//cout << hostname);
		}
		//------------------------------------------------------------
		Int32 circularBufferCountServerSide = -1;
		if((p2 = comm.IndexOf("-cbs ")) >= 0)
		{
			p2 += String("-cbs ").get_Length();
			int p3 = comm.IndexOf("-", p2);
			try
			{
				if(p3 >= 0)
					circularBufferCountServerSide = Convert::ToInt32(command.Substring(p2, p3 -  p2).Trim());
				else
					circularBufferCountServerSide = Convert::ToInt32(command.Substring(p2).Trim());
			}
			catch(...)
			{
				cout << "Error: Bad format.(Usage: -cbs number)\n" << endl;
				return ;
			}
			//cout << hostname);
		}
		//cout << tcpBufferSize << endl;
		//------------------------------------------------------------
		//TransferMode mode = TransferMode.xDFS;
		//String m = null;
		//if((p2 = comm.IndexOf("-mode ")) >= 0)
		//{
		//	p2 += "-mode ".get_Length();
		//	int p3 = comm.IndexOf("-", p2);
		//	if(p3 >= 0)
		//		m = command.Substring(p2, p3 -  p2).Trim().ToLower();
		//	else m = command.Substring(p2).Trim().ToLower();
		//	if(m == "gridftp")
		//		mode =  TransferMode.GridFTP;
		//	if(m == "xDFS")
		//		mode =  TransferMode.xDFS;
		//	if(m != "xDFS" && m != "gridftp")
		//		mode =  TransferMode.xDFS;
			//cout << mode);
		//}
		//------------------------------------------------------------
		bool secure = false;
		if((p2 = comm.IndexOf("-secure")) >= 0)
			secure = true;
		//cout << secure << endl;

		bool isClientSideZeroCopyEnabled = false;
		if((p2 = comm.IndexOf("-zc")) >= 0)
			isClientSideZeroCopyEnabled = true;

		bool isServerSideZeroCopyEnabled = false;
		if((p2 = comm.IndexOf("-zs")) >= 0)
			isServerSideZeroCopyEnabled = true;
		//------------------------------------------------------------
		p2 = comm.IndexOf(",");
		if(p2 <= 0)
		{
			cout << "Error: Bad format.(Usage: -upload readPath,remotePath)\n" << endl;
			return ;
		}
		if(upload)
			p1 += String("-upload ").get_Length();
		else
			p1 += String("-download ").get_Length();
		String localPath = command.Substring(p1, p2 - p1).Trim();
		//------------------------------------------------------------
		int pp = comm.IndexOf("-p ", p1);
		String remotePath;
		if(pp >= 0 && pp > p2)
			remotePath = command.Substring(p2 + 1, pp - p2 - 1).Trim();
		else
			remotePath = command.Substring(p2 + 1).Trim();
		Int32 parallel = 1;
		String ppp;
		if((p1 = comm.IndexOf("-p ")) >= 0)
		{
			p1 += String("-p ").get_Length();
			p2 = comm.IndexOf("-", p1);
			if(p2 >= 0)
				ppp = command.Substring(p1, p2 -  p1).Trim();
			else ppp = command.Substring(p1).Trim();
			//String ppp = command.Substring(p1 + 2, p2 - p1).Trim();
		}
		if(ppp.get_BaseStream() != null)
		{
			try
			{
				parallel = Convert::ToInt32(ppp);
			}
			catch(...)
			{
				cout << "Error: Bad format.(Usage: -p number)\n" << endl;
				return ;
			}
		}
		if(parallel <= 0)
		{
			parallel = 1;
			cout << "Error: Parallel connections number can not be zero or negative.\n" << endl;
			return;
		}
		//------------------------------------------------------------
		//cout << "{0}/{1}/{2}", parallel, localPath, remotePath);

		String _secure = "false";
		if(secure)
			_secure = "true";

		String _isClientSideZeroCopyEnabled = "false";
		if(isClientSideZeroCopyEnabled)
			_isClientSideZeroCopyEnabled = "true";
		//------------------------------------------------------------

		//_________________________________
		Parvicursor_RESOURCE_PROFILER_BEGIN()
		//_________________________________


		try
		{
			if(upload) //upload section
			{
				try
				{
					errors->Clear();
					bool memmoryToMemoryTests = false;
					if(localPath.ToLower().Trim().IndexOf("/dev/zero", 0) >= 0 && remotePath.ToLower().Trim().IndexOf("/dev/null", 0) >= 0) // for memroy-to-memory tests
						memmoryToMemoryTests = true;

					upload_session = new UploadClient(localPath, remotePath, parallel, blockSize, tcpBufferSize, circularBufferCountClientSide, circularBufferCountServerSide, hostname, nc, secure, memmoryToMemoryTests, errors, isClientSideZeroCopyEnabled, isServerSideZeroCopyEnabled);

					//if(!memmoryToMemoryTests)
						cout << "\n\nTransferring  \n\n\tfile:" << localPath.get_BaseStream() << " (" << upload_session->get_FileSize() << " Bytes)\n\nto \n\txDFS:" << hostname.get_BaseStream() << ":" << remotePath.get_BaseStream() <<\
								"\n\n\n\tParallel Streams: " << parallel << ", Secure(DotSec): " << _secure.get_BaseStream() << ", Forced Zero-Copy: " << _isClientSideZeroCopyEnabled.get_BaseStream() << \
								"\n\n\n\tTCP Window Size: " << tcpBufferSize << " Bytes, Block Size: " << blockSize << " Bytes" << "\n\n\n" << endl;
					//else
					//{
					//	cout << "Memory-to-Memory Test.");
					//	cout << "\nTransferring  \n\n\tfile:{0} ({1} Bytes)\n\nto \n\txDFS:{2}:{3}\n\n\n\tParallel Streams: {4}, Secure(DotSec): {5}\n\n", localPath, long.MaxValue,
					//		hostname, remotePath, parallel, secure);
					//}

					Callback callback = ShowSpeed;
					TimerCallback timerDelegate(callback);
					Timer timer(timerDelegate, upload_session, 0, 1000);

					upload_session->Run();
					//pthread_mutex_lock(&mutex);
					upload_session->Close();

					delete upload_session;
					upload_session = null;
					//pthread_mutex_unlock(&mutex);

					for(Int32 i = 0 ; i < errors->get_Count() ; i++)
					{
						System::Exception *e = (System::Exception *)errors->get_Value(i);
						//printf("%s\n", ((Exception)(*(Exception *)errors->get_Value(i))).get_Message().get_BaseStream());
						printf("Error%d: %s\n", i + 1, e->get_Message().get_BaseStream());
						delete e;
					}
				}
				catch(System::Exception &e)
				{
					//if(upload_session != null)
					//{
					//	upload_session->Close();
					//	delete upload_session;
					//}
					printf("%s\n", e.get_Message().get_BaseStream());
				}
				catch(...)
				{
					printf("unknown error was occured.\n");
				}
				/*else //meaning the local source path is a directory tree.
				{
					uploadDirectory = new UploadDirectoryClient(localPath, remotePath, hostname, parallel, tcpBufferSize, nc, secure, false, true);
					timer = new Timer(timerDelegate, uploadDirectory, 0, 1);
					cout << "\n\nTransferring  \n\n\tdirectory:{0} \n\nto \n\txDFS:{1}:{2}\n\n\n\tParallel Streams: {3}, Secure(DotSec): {4}\n\n", localPath, hostname, remotePath, parallel, secure);
					uploadDirectory.Run();
					timer.Dispose();
					cout << "\n\n\tTotal Elapsed Time: {0} seconds", uploadDirectory.TotalElapsedTime/1000);
					Exception[] es = uploadDirectory.ThrownExceptions;
					if(es != null)
						foreach(Exception ex in es)
							cout << ex.Message);
					cout << "");
				}*/
			}
			else //download section
			{
				try
				{
					errors->Clear();
					bool memmoryToMemoryTests = false;
					if(localPath.ToLower().Trim().IndexOf("/dev/null", 0) >= 0 && remotePath.ToLower().Trim().IndexOf("/dev/zero", 0) >= 0) // for memroy-to-memory tests
						memmoryToMemoryTests = true;

					download_session = new DownloadClient(localPath, remotePath, parallel, blockSize, tcpBufferSize, hostname, nc, secure, memmoryToMemoryTests, errors, isClientSideZeroCopyEnabled, isServerSideZeroCopyEnabled);

					//if(!memmoryToMemoryTests)
					//cout << "\n\nTransferring  \n\n\t xDFS:" << hostname.get_BaseStream() << ":" << remotePath.get_BaseStream() << " (" << download_session->get_FileSize() << " Bytes)\n\nto \n\t file:" << localPath.get_BaseStream() <<
					cout << "\n\nTransferring  \n\n\t xDFS:" << hostname.get_BaseStream() << ":" << remotePath.get_BaseStream() << "\n\nto \n\t file:" << localPath.get_BaseStream() <<\
								"\n\n\n\tParallel Streams: " << parallel << ", Secure(DotSec): " << _secure.get_BaseStream() << ", Forced Zero-Copy: " << _isClientSideZeroCopyEnabled.get_BaseStream() << \
								"\n\n\n\tTCP Window Size: " << tcpBufferSize << " Bytes, Block Size: " << blockSize << " Bytes" << "\n\n\n" << endl;
					//else
					//{
					//	cout << "Memory-to-Memory Test.");
					//	cout << "\nTransferring  \n\n\tfile:{0} ({1} Bytes)\n\nto \n\txDFS:{2}:{3}\n\n\n\tParallel Streams: {4}, Secure(DotSec): {5}\n\n", localPath, long.MaxValue,
					//		hostname, remotePath, parallel, secure);
					//}

					Callback callback = ShowSpeed;
					TimerCallback timerDelegate(callback);
					Timer timer(timerDelegate, download_session, Timeout::Infinite, 1000);

					download_session->Run();
					//pthread_mutex_lock(&mutex);
					download_session->Close();

					delete download_session;
					download_session = null;
					//pthread_mutex_unlock(&mutex);

					for(Int32 i = 0 ; i < errors->get_Count() ; i++)
					{
						System::Exception *e = (System::Exception *)errors->get_Value(i);
						//printf("%s\n", ((Exception)(*(Exception *)errors->get_Value(i))).get_Message().get_BaseStream());
						printf("Error%d: %s\n", i + 1, e->get_Message().get_BaseStream());
						delete e;
					}
				}
				catch(System::Exception &e)
				{
					//if(upload_session != null)
					//{
					//	upload_session->Close();
					//	delete upload_session;
					//}
					printf("%s\n", e.get_Message().get_BaseStream());
				}
				catch(...)
				{
					printf("unknown error was occured.\n");
				}
				/*else //meaning the local source path is a directory tree.
				{
					uploadDirectory = new UploadDirectoryClient(localPath, remotePath, hostname, parallel, tcpBufferSize, nc, secure, false, true);
					timer = new Timer(timerDelegate, uploadDirectory, 0, 1);
					cout << "\n\nTransferring  \n\n\tdirectory:{0} \n\nto \n\txDFS:{1}:{2}\n\n\n\tParallel Streams: {3}, Secure(DotSec): {4}\n\n", localPath, hostname, remotePath, parallel, secure);
					uploadDirectory.Run();
					timer.Dispose();
					cout << "\n\n\tTotal Elapsed Time: {0} seconds", uploadDirectory.TotalElapsedTime/1000);
					Exception[] es = uploadDirectory.ThrownExceptions;
					if(es != null)
						foreach(Exception ex in es)
							cout << ex.Message);
					cout << "");
				}*/
			}
		}
		catch(System::Exception &e)
		{
			printf("\n\nExecution Error. Thrown Exception Message: %s\n", e.get_Message().get_BaseStream());
			/*if(fs != null)
				fs.Close();
			if(download != null)
				download.Close();
			if(uploadDirectory != null)
				uploadDirectory.Close();
			if(downloadDirectory != null)
				downloadDirectory.Close();*/

			//_________________________________
			Parvicursor_RESOURCE_PROFILER_END()
			Parvicursor_RESOURCE_PROFILER_FINALIZE()
			//_________________________________

			return ;
		}

		//_________________________________
		Parvicursor_RESOURCE_PROFILER_END()
		//_________________________________

	}
	else
	{
		cout << "\nError: Unrecognized command(s). You must use -upload or -download input argument correctly. For help, use -help input argument.\n" << endl;
		//cout << "hello" << endl;
		return ;
	}
}
//---------------------------------------
Object *ShowSpeed(Object *f)
{
	//pthread_mutex_lock(&mutex);

	if(upload_session != null)
	{
		if(upload_session->get_CurrentTransferredBytes() > 0)
		{
			Int64 written = upload_session->get_CurrentTransferredBytes() - lastWritten;
			if(written >= 0)
			{
				seconds++;
				sum += written;

#ifdef __Scaling__
				float ave = ((float)(sum/seconds))/((float)(1024.0 * 1024.0)) + (float)10.0;
				float ins = (float)(written/(1024.0 * 1024.0)) + (float)10.0;
#else
				float ave = ((float)(sum/seconds))/((float)(1024.0 * 1024.0));
				float ins = (float)(written/(1024.0 * 1024.0));
#endif
				/*
					MB/s
				*/

				/*float ave = (((float)(sum/seconds))/((float)(1024.0)))*8;
				float ins = 8.0*(float)(written/(1024.0));*/
				/*
					Kb/s
				*/

				_ins += (double)ins;
				ave = (float)(_ins / (float)seconds);

				//printf("(%d Bytes)   Speed(MB/s): ave:%d ins:%d\n", upload_session->get_CurrentTransferredBytes(), 22, 22);
#ifdef __Scaling__

				cout << "\rSpeed(MB/s): \tave:" << fixed << setprecision(2) << ave << "\tins:" << fixed << setprecision(2) << ins << "\r";
#else

				//cout << "\r(" << upload_session->get_CurrentTransferredBytes() << " Bytes)" << "\tSpeed(MB/s): \tave:" << fixed << setprecision(2) << ave << "\tins:" << fixed << setprecision(2) << ins << "\r";
				cout << "\r(" << upload_session->get_CurrentTransferredBytes() << " Bytes)" << "\tSpeed(MB/s): \tave:" << fixed << setprecision(2) << ave << "\tins:" << fixed << setprecision(2) << ins << "\r" << endl;
#endif
				lastWritten = upload_session->get_CurrentTransferredBytes();
			}
		}
		//cout << "upload_session is not null" << endl;
		else
		{
			/*sum = 0;
			seconds = 0;
			lastWritten = 0;*/
			//Thread::Sleep(1000);
		}
	}
	if(download_session != null)
	{
		if(download_session->get_CurrentTransferredBytes() > 0)
		{
			Int64 written = download_session->get_CurrentTransferredBytes() - lastWritten;
			if(written >= 0)
			{
				seconds++;
				sum += written;
#ifdef __Scaling__
				float ave = ((float)(sum/seconds))/((float)(1024.0 * 1024.0)) + (float)10.0;
				float ins = (float)(written/(1024.0 * 1024.0)) + (float)10.0;
#else
				float ave = ((float)(sum/seconds))/((float)(1024.0 * 1024.0));
				float ins = (float)(written/(1024.0 * 1024.0));
#endif
				/*
					MB/s
				*/

				/*float ave = (((float)(sum/seconds))/((float)(1024.0)))*8;
				float ins = 8.0*(float)(written/(1024.0));*/
				/*
					Kb/s
				*/

				_ins += (double)ins;
				ave = (float)(_ins / (float)seconds);

				//printf("(%d Bytes)   Speed(MB/s): ave:%d ins:%d\n", upload_session->get_CurrentTransferredBytes(), 22, 22);
#ifdef __Scaling__

				cout << "\rSpeed(MB/s): \tave:" << fixed << setprecision(2) << ave << "\tins:" << fixed << setprecision(2) << ins << "\r";

#else
				//cout << "\r(" << download_session->get_CurrentTransferredBytes() << " Bytes)" << "\tSpeed(MB/s): \tave:" << fixed << setprecision(2) << ave << "\tins:" << fixed << setprecision(2) << ins << "\r";
				cout << "\r(" << download_session->get_CurrentTransferredBytes() << " Bytes)" << "\tSpeed(MB/s): \tave:" << fixed << setprecision(2) << ave << "\tins:" << fixed << setprecision(2) << ins << "\r" << endl;
#endif
				lastWritten = download_session->get_CurrentTransferredBytes();
			}
		}
		//cout << "upload_session is not null" << endl;
		else
		{
			/*sum = 0;
			seconds = 0;
			lastWritten = 0;*/
		}
	}

	//pthread_mutex_unlock(&mutex);

	return f;
}
//---------------------------------------
const String Copyright(bool echo)
{
	if(!echo)
		return String("This Parvicursor.xDFS client program transfers files and directory trees between xDFS clients and servers on Grid infrastructures. Parvicursor.xDFS Client API Version 2 (native mode).\nAll rights reserved to Parvicursor team (c) 1999-2022.\narp@poshtkohi.info.\n\nUsage:   Parvicursor-url-copy -hostname remoteHost -[upload|download] localPath,remotePath -[optional command line switches]\n\nFor more help, please use -help input argument.\n");
	else
		return String("title Parvicursor Parvicursor-url-copy client utility&&echo This Parvicursor.xDFS client program transfers files and directory trees between xDFS clients and servers on Grid infrastructures. Parvicursor.xDFS Client API Version 2 (native mode).\nAll rights reserved to Parvicursor team (c) 1999-2022.\narp@poshtkohi.info.\n\n.&&echo.&&echo Usage:  Parvicursor-url-copy -hostname remoteHost -[upload^|download] localPath,remotePath -[optional command line switches]&&echo.&&echo For more help, please use -help input argument.&&echo.");
}
//---------------------------------------
void Help()
{
	cout << "Attention: For memory-to-memory tests, use /dev/zero for all read paths and /dev/null for all write paths only in Unix/Linux operating systems." << endl;
	cout << "\nCommands:\n" << endl;
	//cout << "-quit: exit from this program.");
	cout << "-tbs: specifies the TCP Window Size in bytes. Default TCP Window size is 64KB (-tbs bytes number)." << endl;
	cout << "-bs: specifies the size in bytes of data blocks to read from disk before posting to the network. Default block size is 256KB (-bs bytes number)." << endl;
	cout << "-secure: specifying this parameter all data connections will be forced to being secure based on DotSec security layer protocol (Default is non-secure)." << endl;
	cout << "-zc: forces the low-level underlying I/O layers to be used by zero-copy mechanisms in client side." << endl;
	cout << "-zs: forces the low-level underlying I/O layers to be used by zero-copy mechanisms in server side." << endl;
	cout << "-cbc: enables client-side disk thread and specifies the number of circular buffer count (Default is disabled). The circular buffer size is equal to -cbc*-bs." << endl;
	cout << "-cbs: enables server-side disk thread and specifies the number of circular buffer count (Default is disabled). The circular buffer size is equal to -cbs*-bs." << endl;
	cout << "-hostname: specifies IP or domain name of the destination xDFS server for transferring files to it." << endl;
	//cout << "-mode: specifies the file transfer mode, the mode can be xDFS or GridFTP.");
	cout << "-p: specifies parallel connections (Default is 1)." << endl;
	cout << "-upload: uploads the specified source file or directory tree to destination xDFS server.\nUsage:" << endl;
	cout << "\t-upload localPath,remotePath\n";
	cout << "-download: downloads the specified remote source file or directory tree from destination xDFS server.\nUsage:" << endl;
	cout << "\t-download localPath,remotePath\n" << endl;
	cout << "----------------------------------------------------------------" << endl;
	cout << "Examples:\n" << endl;
	//cout << "Non-secure file transfer:\n\n\t" + @"-hostname 127.0.0.1 -upload g:\\test\100meg.dat,g:\\test\temp.dat -p 20 -mode xDFS" + "\n" << endl;
	cout << "Non-secure file transfer:\n\n\t-hostname 127.0.0.1 -upload g:/test/100meg.dat,g:/test/temp.dat -p 20\n" << endl;
	//cout << "Secure file transfer:\n\n\t" + @"-hostname 127.0.0.1 -upload g:\\test\100meg.dat,g:\\test\temp.dat -p 20 -mode xDFS -secure" + "\n" << endl;
	cout << "Secure file transfer:\n\n\t-hostname 127.0.0.1 -upload g:/test/100meg.dat,g:/test/temp.dat -p 20 -secure\n" << endl;
	cout << "----------------------------------------------------------------\n" << endl;
}
//---------------------------------------

    /*System::Collections::Hashtable *h = new System::Collections::Hashtable();
    int i = 0;
    ArrayList *al = new ArrayList();
    al->Add((Object *)&i);
    cout << al->get_Count() << endl;
    //return 0;
    h->Add(String("i"), (Object *)&i);

    cout << h->get_Count() << endl;
    getchar();
    return 0;*/


	/*while(true)
	{
		Int32 i = 4294967295;
		String s = Convert::ToString(i);
		try {cout << s.get_BaseStream() << endl; }
		catch(Exception &e)
		{
			cout << e.get_Message().get_BaseStream() << endl;
		}
		Thread::Sleep(1);
	}
	return 0;*/
