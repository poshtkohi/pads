// xDfsFileStreamClient.cpp : Defines the entry point for the console application.
//

//---------------------------------------
#include "../../Parvicursor/general.h"
#include "../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../Parvicursor/System.IO/FileStream/FileStream.h"
#include "../../Parvicursor/System/String/String.h"
#include "../../Parvicursor/Parvicursor/xDFS/xDFSClients/FileStreamClient/FileStreamClient.h"
//---------------------------------------
using namespace System;
using namespace System::IO;

using namespace Parvicursor::xDFS::xDFSClients;
//---------------------------------------
#include "../../Parvicursor/Parvicursor/Parvicursor.Cryptography/AesEngine/AesEngine.h"
using namespace Parvicursor::Cryptography;

void AES_GenerateKeyIV(void)
{
	CryptographyKey key = AesEngine::GenerateKey(AesEngine::DefaultKeyLength);
	// Generate a random IV
	Byte iv[AesEngine::DefaultBlockSize];
	AesEngine::GenerateIV(iv, AesEngine::DefaultBlockSize);

	FileStream *fsKey  = new FileStream("key.dat", System::IO::OpenOrCreate, System::IO::Write, 9);
	FileStream *fsIV  = new FileStream("iv.dat", System::IO::OpenOrCreate, System::IO::Write, 9);

	fsKey->Write((char *)key.get_Key(), 0, key.get_Length());
	fsIV->Write((char *)iv, 0, AesEngine::DefaultBlockSize);

}
//---------------------------------------
int main(int argc, char *argv[])
{
	//AES_GenerateKeyIV(); return 0;
	Int32 blockSize = 256*1024;
	char *buffer = new char[blockSize];
	bool secure = true;
	//strcpy(buffer, "Alireza Poshtkohi\n");

	//String remoteFilename = "C:/test.txt";
	//String localFilename = "C:/test1.txt";

	String remoteFilename = "C:/test.zip";
	String localFilename = "C:/test1.zip";
	String server = "localhost";
	NetworkCredential nc = NetworkCredential("user", "pass");
	FileStreamClient *fsRead = null;
	FileStream *fsWrite = null;

	try
	{
		fsRead  = new FileStreamClient(remoteFilename, System::IO::Open, System::IO::Read, blockSize, server, nc, secure);
		fsWrite  = new FileStream(localFilename, System::IO::OpenOrCreate, System::IO::Write, 9);

#if defined WIN32 || WIN64
		//while(true){
		Int64 fileLength = fsRead->get_Length();
		printf("Copying %s:%s (%I64u Bytes) to %s\n\n", server.get_BaseStream(), remoteFilename.get_BaseStream(), fileLength, localFilename.get_BaseStream());
		//::Sleep(1000);}
#else
        Int64 fileLength = fsRead->get_Length();
		printf("Copying %s:%s (%llu Bytes) to %s\n\n", server.get_BaseStream(), remoteFilename.get_BaseStream(), fileLength, localFilename.get_BaseStream());
#endif
		//return 0;
		Int32 read = 0;
		while((read = fsRead->Read(buffer, 0, blockSize)) != 0)
		{
			//printf("read: %d\n", read);
			fsWrite->Write(buffer, 0, read);
		}
	}
	catch(System::Exception &e)
	{
		printf("Exception Message: %s\n", e.get_Message().get_BaseStream());
	}
	catch(...)
	{
		printf("Unknown Exception\n");
	}

	if(fsWrite != null)
	{
		try { fsWrite->Close(); } catch(...) {}
		delete fsWrite;
		fsWrite = null;
	}

	if(fsRead != null)
	{
		try { fsRead->Close(); } catch(...){}
		delete fsRead;
		fsRead = null;
	}

	if(buffer != null)
	{
		delete buffer;
		buffer = null;
	}

	return 0;
}
//---------------------------------------
