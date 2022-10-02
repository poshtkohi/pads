// FileCreator.cpp : Defines the entry point for the console application.
//

#include "../Parvicursor/general.h"
#include "../Parvicursor/System/String/String.h"
#include "../Parvicursor/System/Convert/Convert.h"
#include "../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../Parvicursor/System.IO/FileStream/FileStream.h"
#include "../Parvicursor/System.IO/Directory/Directory.h"
#include "../Parvicursor/System.IO/File/File.h"
#include "../Parvicursor/Parvicursor/Profiler/ResourceProfiler.h"

#include <iostream>
#include <iomanip>
//---------------------------------------
using namespace System;
using namespace System::IO;

//_________________________________
Parvicursor_RESOURCE_PROFILER_INITIALIZE()
//_________________________________

//---------------------------------------
FileStream *fsMain = null;
FileStream *fs = null;
char *buffer = null;
inline int FileCopy(const String &inputFile, const String &outputDirectory, int megs);
inline void FreeMemory();

int main(int argc, char* argv[])
{
	cout << "This program produces files with sizes of 1, 10, 20, 40, 50, 100, 200, 300, 400, 500, 1000, 2000 and 4000 Meg for tests." << endl;
	cout << "Commands:\n" << endl;
	cout << "quit: exit from this program." << endl;
	cout << "create: creates the above file lists on your hard drive.\nUsage:" << endl;
	cout << "\tcreate inputFile,outputDirectory\nExample:" << endl;
	cout << "\t" << "create c:/windows/myfile.dll,e:/test" << endl << endl;

	int p1;
	printf("**: ");
	char ss[256];
	buffer = new char[1024*1024];
	while(true)
	{
		ss[0] = '\0';
		while(true)
		{
			gets(ss);
			if(strlen(ss) != 0)
				break;
		}

		char *sss = new char[strlen(ss) + 1];
		strcpy(sss, ss);
		String command(sss);
		command = command.Trim();
		printf("\n");
		if(command == "quit")
			break;
		if((p1 = command.IndexOf("create")) >= 0)
		{
			//cout << command.get_BaseStream() << endl; exit(0);
			int p2 = command.IndexOf(",");
			if(p2 <= 0)
			{
				printf("Bad format.(Usage: create inputFile,outputDirectory)\n");
				printf("**: ");
				continue;
			}
			p1 += String("create ").get_Length();
			String inputFile = command.Substring(p1, p2 - p1).Replace("\\", "/").Trim();
			String outputDirectory = command.Substring(p2 + 1).Replace("\\", "/").Trim();
			try
			{
				if(Directory::Exists(outputDirectory))
					Directory::CreateDirectory(outputDirectory);

				if(!File::Exists(inputFile))
				{
					printf("inputFile does not exists.\n");
					printf("**: ");
					continue;
				}
			}
			catch(Exception &e) { printf("%s", e.get_Message().get_BaseStream()); printf("**: "); continue;}
			catch(...) { printf("Unknow expetion was occured"); printf("**: "); continue;}
			/*FileInfo info = new FileInfo(inputFile);
			if(info.Length < 1024 * 1024)
			{
				printf("inputFile size must be greater than 1Meg.\n");
				printf("**: ");
				continue;
			}*/
			try
			{
				int ret = 0;
				ret = FileCopy(inputFile, outputDirectory, 100);
				//ret = FileCopy(inputFile, outputDirectory, 1);
				/*ret = FileCopy(inputFile, outputDirectory, 10);
				ret = FileCopy(inputFile, outputDirectory, 20);
				ret = FileCopy(inputFile, outputDirectory, 40);
				ret = FileCopy(inputFile, outputDirectory, 50);
				ret = FileCopy(inputFile, outputDirectory, 100);
				ret = FileCopy(inputFile, outputDirectory, 200);
				ret = FileCopy(inputFile, outputDirectory, 300);
				ret = FileCopy(inputFile, outputDirectory, 400);
				ret = FileCopy(inputFile, outputDirectory, 500);
				ret = FileCopy(inputFile, outputDirectory, 1000);
				ret = FileCopy(inputFile, outputDirectory, 2000);*/
				//ret = FileCopy(inputFile, outputDirectory, 4000);

				if(ret < 0)
				{
					printf("inputFile size must be greater than 1Meg.\n");
					printf("**: ");
					continue;
				}
			}
			catch(Exception &e) { printf("%s", e.get_Message().get_BaseStream()); break; }
			catch(...) { printf("Unknow expetion was occured"); break; }
		}
		else
			printf("Unrecognized command.\n");
		printf("**: ");
	}

	FreeMemory();

	return 0;

}
//---------------------------------------
int FileCopy(const String &inputFile, const String &outputDirectory, int megs)
{

	//_________________________________
	Parvicursor_RESOURCE_PROFILER_BEGIN()
	//_________________________________

	String path = outputDirectory + "/" + Convert::ToString(megs) + "meg.dat";
	fsMain = new FileStream(inputFile, System::IO::Open, System::IO::Read, 9);
	if(fsMain->get_Length() < 1024 * 1024)
	{
		FreeMemory();
		return -1;
	}
	fs = new FileStream(path, System::IO::Create, System::IO::Write, 9);
	buffer = new char[1024*1024];
	int read = fsMain->Read(buffer, 0, 1024*1024);
	printf ("Creating %s\n" ,path.get_BaseStream());
	register Int32 _megs = megs;

	for(register int i = 0 ; i < _megs ; i++)
	{
		fs->Write(buffer, 0, read);
		printf("\rcompleted %d%% ...\r", (int)(((float)(i+1)/(float)megs) * 100.0));//
	}

	//_________________________________
	Parvicursor_RESOURCE_PROFILER_END()
	//_________________________________

	fsMain->Close();
	fs->Close();
	printf("\n");
	FreeMemory();
	return 0;
}
//---------------------------------------
void FreeMemory()
{
	if(fsMain != null)
	{
		fsMain->Close();
		delete fsMain;
		fsMain = null;
	}
	if(fs != null)
	{
		fs->Close();
		delete fs;
		fs = null;
	}
	if(buffer != null)
	{
		delete buffer;
		buffer = null;
	}
}
//---------------------------------------
