// xMoveDLL.cpp : Defines the entry point for the DLL application.
//


char dllFilename[256];

#include <stdio.h>

#if defined WIN32 || WIN64
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{

	size_t len;

	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			TCHAR _dllFilename[256];
			::GetModuleFileName((HMODULE)hModule, _dllFilename, 256);
			len = strlen(_dllFilename);

			for(register size_t i = 0 ; i < len ; i++)
				dllFilename[i] = (char)_dllFilename[i];

			dllFilename[len] = '\0';
			//printf("%s\n", dllFilename);
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			break;
		default:
			break;
	}

    return TRUE;
}
#else
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <dlfcn.h>
__attribute__((constructor))
void on_load(void)
{
	Dl_info dl_info;
	dladdr(on_load, &dl_info);
	//fprintf(stderr, "module %s loaded\n", dl_info.dli_fname);
	size_t len = strlen(dl_info.dli_fname);

	for(register size_t i = 0 ; i < len ; i++)
		dllFilename[i] = (char)dl_info.dli_fname[i];

	dllFilename[len] = '\0';
	//printf("%s\n", dllFilename);
}
#endif
