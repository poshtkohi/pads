// FileStream.cpp : Defines the entry point for the console application.
//

//---------------------------------------
#include "../../Parvicursor/general.h"
#include "../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../Parvicursor/System.IO/FileStream/FileStream.h"
#include "../../Parvicursor/System/String/String.h"
//---------------------------------------
using namespace System;
using namespace System::IO;
//---------------------------------------
int main(int argc, char *argv[])
{
	FileStream *fsRead;
	FileStream *fsWrite;

	String readFilename = "c:/test.msi";  //to change
	String writeFilename = "c:/test1.msi";   //to change

	printf("Copying %s to %s\n", readFilename.get_BaseStream(), writeFilename.get_BaseStream());

	Int32 bufferSize = 256*1024;
	char *buffer = new char[bufferSize];
	Int32 read = 0;
	try
	{
		fsRead = new FileStream(readFilename, System::IO::Open, System::IO::Read, 8*1024);
		fsWrite = new FileStream(writeFilename, System::IO::OpenOrCreate, System::IO::Write, 8*1024);
	}
	catch(Exception &e)
	{
		printf("1. Exception Message: %s\n", e.get_Message().get_BaseStream());
        char s[12];
        scanf("%s", s);
		return 0;
	}
	while(true)
	{
		try
		{
			read = fsRead->Read(buffer, 0, bufferSize);
			if(read <= 0)
				break;
			fsWrite->Write(buffer, 0, read);
			printf("\r...");
		}
		catch(IOException &e)
		{
			printf("2. Exception Message: %s\n", e.get_Message().get_BaseStream());
			return 0;
		}
		catch(Exception &e)
		{
			printf("3. Exception Message: %s\n", e.get_Message().get_BaseStream());
			return 0;
		}
	}
	//fsWrite->Close();
	//fsRead->Close();
	delete fsRead;
	delete fsWrite;
	delete buffer;

    //char s[12];
    //scanf("%s", s);
	printf("...\r");
	return 0;
}
//---------------------------------------