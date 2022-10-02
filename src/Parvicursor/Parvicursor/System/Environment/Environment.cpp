/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "Environment.h"

//**************************************************************************************************************//

namespace System
{
	//----------------------------------------------------
	Int32 Environment::get_ProcessorCount()
	{
#if defined WIN32 || WIN64
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		return sysinfo.dwNumberOfProcessors;
#elif MACOS
		int nm[2];
		size_t len = 4;
		uint32_t count;

		nm[0] = CTL_HW; nm[1] = HW_AVAILCPU;
		sysctl(nm, 2, &count, &len, NULL, 0);

		if(count < 1) {
			nm[1] = HW_NCPU;
			sysctl(nm, 2, &count, &len, NULL, 0);
			if(count < 1) { count = 1; }
		}
		return count;
#else
		return sysconf(_SC_NPROCESSORS_ONLN);
#endif
	}
	//----------------------------------------------------
};
//**************************************************************************************************************//
