/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __Parvicursor_general__
#define __Parvicursor_general__


#if defined WIN32 || WIN64
#ifndef _____Windows_____
#define _____Windows_____
#include <atlstr.h>
#include <windows.h>

static volatile bool ws2_32_is_loaded = false;
/*#if (defined _M_IX86) && (!defined _M_X64)
#pragma comment(lib, "../../ParvicursorLib/Debug/ParvicursorLib.lib")
#pragma comment(lib, "../../Parvicursor/System.Threading/Thread/pthreadVC2.lib")
#else if (defined _M_IX86) && (defined _M_X64)
#pragma comment(lib, "../../ParvicursorLib/x64/Debug/ParvicursorLib.lib")
#pragma comment(lib, "../../Parvicursor/System.Threading/Thread/pthreadVC2_x64.lib")
#endif*/

#endif


#endif

#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;


#define __xDFS_Server_Default_Port__ 2799 //21
#define Enable_TCP_Window_Scale
#define ProfilerMode

#define __Delta_Enabled__

//#define With_Error_Recovery_Support
//#define xThread_With_Error_Recovery_Support
//#define __Scaling__



#endif
