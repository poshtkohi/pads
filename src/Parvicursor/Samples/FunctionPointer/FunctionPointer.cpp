// FunctionPointer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>

void *test(void *i)
{
	while(true)
	{
		printf("test: %d\n", *((int *)i));
		//break;
		Sleep(1);
	}
	return i;
}

struct function_table {
   char *name;
   void *(*start)(void *);
};


int _tmain(int argc, _TCHAR* argv[])
{
	struct function_table *functions = new function_table[10];

	for(int i = 0 ; i < 10 ; i++)
	{
		functions[i].start = test;
	}
	for(int i = 0 ; i < 10 ; i++)
	{
		functions[i].start(&i);
	}

	delete functions;

	return 0;
}

