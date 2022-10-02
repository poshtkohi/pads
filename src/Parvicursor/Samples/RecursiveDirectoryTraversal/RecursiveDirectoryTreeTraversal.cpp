// RecursiveDirectoryTreeTraversal.cpp : Defines the entry point for the console application.
//
//---------------------------------------
#include "../../Parvicursor/general.h"
#include "../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../Parvicursor/System.IO/Directory/Directory.h"
#include "../../Parvicursor/System/String/String.h"
#include "../../Parvicursor/System.Collections/ArrayList/ArrayList.h"
//---------------------------------------
using namespace System;
using namespace System::IO;
using namespace System::Collections;
//---------------------------------------
void RecursiveDirectoryTreeTraversal(const String &dir);
//---------------------------------------
int main(int argc, char *argv[])
{
	String dir = "/root"; // to change

	RecursiveDirectoryTreeTraversal(dir);

	return 0;
}
//---------------------------------------
void RecursiveDirectoryTreeTraversal(const String &dir)
{
	ArrayList *dirs = Directory::GetDirectories(dir);

	if(dirs == null)
		return;

	for(Int32 i = 0 ; i < dirs->get_Count() ; i++)
	{
		String *s = (String *)dirs->get_Value(i);
		ArrayList *files = Directory::GetFiles(*s);

		if(files != null)
		{
			for(Int32 j = 0 ; j < files->get_Count() ; j++)
			{
				String *ss = (String *)files->get_Value(j);
				printf("%s\n", ss->get_BaseStream());
				delete ss;
			}
			files->Clear();
			delete files;
		}
		//printf("%s\n", s->get_BaseStream());
		RecursiveDirectoryTreeTraversal(*s);
		delete s;
	}

	dirs->Clear();
	delete dirs;
}
//---------------------------------------
