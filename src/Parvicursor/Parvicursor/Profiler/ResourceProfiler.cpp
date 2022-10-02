/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "ResourceProfiler.h"



#if defined WIN32 || WIN64
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#else
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>
#endif


#if defined WIN32 || WIN64
#include <stdio.h>
#include <tchar.h>
#include <Ntsecapi.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef ::LONG NTSTATUS;
//typedef ::ULONG KAFFINITY;
typedef KAFFINITY *PKAFFINITY;
typedef ::LONG KPRIORITY;

typedef struct _CLIENT_ID {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID;
typedef CLIENT_ID *PCLIENT_ID;

typedef enum _KWAIT_REASON {
  Executive = 0,
  FreePage = 1,
  PageIn = 2,
  PoolAllocation = 3,
  DelayExecution = 4,
  Suspended = 5,
  UserRequest = 6,
  WrExecutive = 7,
  WrFreePage = 8,
  WrPageIn = 9,
  WrPoolAllocation = 10,
  WrDelayExecution = 11,
  WrSuspended = 12,
  WrUserRequest = 13,
  WrEventPair = 14,
  WrQueue = 15,
  WrLpcReceive = 16,
  WrLpcReply = 17,
  WrVirtualMemory = 18,
  WrPageOut = 19,
  WrRendezvous = 20,
  Spare2 = 21,
  Spare3 = 22,
  Spare4 = 23,
  Spare5 = 24,
  WrCalloutStack = 25,
  WrKernel = 26,
  WrResource = 27,
  WrPushLock = 28,
  WrMutex = 29,
  WrQuantumEnd = 30,
  WrDispatchInt = 31,
  WrPreempted = 32,
  WrYieldExecution = 33,
  WrFastMutex = 34,
  WrGuardedMutex = 35,
  WrRundown = 36,
  MaximumWaitReason = 37
} KWAIT_REASON;


typedef struct _VM_COUNTERS {
	::ULONG PeakVirtualSize;
    ::ULONG VirtualSize;
    ::ULONG PageFaultCount;
    ::ULONG PeakWorkingSetSize;
    ::ULONG WorkingSetSize;
    ::ULONG QuotaPeakPagedPoolUsage;
    ::ULONG QuotaPagedPoolUsage;
    ::ULONG QuotaPeakNonPagedPoolUsage;
    ::ULONG QuotaNonPagedPoolUsage;
    ::ULONG PagefileUsage;
    ::ULONG PeakPagefileUsage;
} VM_COUNTERS;
typedef VM_COUNTERS *PVM_COUNTERS;

typedef enum _PROCESSINFOCLASS {
    ProcessBasicInformation,
    ProcessQuotaLimits,
    ProcessIoCounters,
    ProcessVmCounters,
    ProcessTimes,
    ProcessBasePriority,
    ProcessRaisePriority,
    ProcessDebugPort,
    ProcessExceptionPort,
    ProcessAccessToken,
    ProcessLdtInformation,
    ProcessLdtSize,
    ProcessDefaultHardErrorMode,
    ProcessIoPortHandlers,          // Note: this is kernel mode only
    ProcessPooledUsageAndLimits,
    ProcessWorkingSetWatch,
    ProcessUserModeIOPL,
    ProcessEnableAlignmentFaultFixup,
    ProcessPriorityClass,
    ProcessWx86Information,
    ProcessHandleCount,
    ProcessAffinityMask,
    ProcessPriorityBoost,
    MaxProcessInfoClass
    } PROCESSINFOCLASS;

typedef struct _SYSTEM_THREAD {
  ::LARGE_INTEGER KernelTime;
  ::LARGE_INTEGER UserTime;
  ::LARGE_INTEGER CreateTime;
  ::ULONG WaitTime;
  ::PVOID StartAddress;
  CLIENT_ID ClientId;
  KPRIORITY Priority;
  ::LONG BasePriority;
  ::ULONG ContextSwitchCount;
  ::ULONG State;
  KWAIT_REASON WaitReason;
} SYSTEM_THREAD, *PSYSTEM_THREAD;


typedef struct _SYSTEM_PROCESS_INFORMATION {
  ::ULONG NextEntryOffset;
  ::ULONG NumberOfThreads;
  ::LARGE_INTEGER Reserved[3];
  ::LARGE_INTEGER CreateTime;
  ::LARGE_INTEGER UserTime;
  ::LARGE_INTEGER KernelTime;
  ::UNICODE_STRING ImageName;
  KPRIORITY BasePriority;
  ::HANDLE ProcessId;
  ::HANDLE InheritedFromProcessId;
  ::ULONG HandleCount;
  ::ULONG Reserved2[2];
  ::ULONG PrivatePageCount;
  VM_COUNTERS VirtualMemoryCounters;
  IO_COUNTERS IoCounters;
  SYSTEM_THREAD Threads[0];
} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;

typedef enum _SYSTEM_INFORMATION_CLASS {
  SystemBasicInformation,
  SystemProcessorInformation,
  SystemPerformanceInformation,
  SystemTimeOfDayInformation,
  SystemPathInformation,
  SystemProcessInformation,
  SystemCallCountInformation,
  SystemDeviceInformation,
  SystemProcessorPerformanceInformation,
  SystemFlagsInformation,
  SystemCallTimeInformation,
  SystemModuleInformation,
  SystemLocksInformation,
  SystemStackTraceInformation,
  SystemPagedPoolInformation,
  SystemNonPagedPoolInformation,
  SystemHandleInformation,
  SystemObjectInformation,
  SystemPageFileInformation,
  SystemVdmInstemulInformation,
  SystemVdmBopInformation,
  SystemFileCacheInformation,
  SystemPoolTagInformation,
  SystemInterruptInformation,
  SystemDpcBehaviorInformation,
  SystemFullMemoryInformation,
  SystemLoadGdiDriverInformation,
  SystemUnloadGdiDriverInformation,
  SystemTimeAdjustmentInformation,
  SystemSummaryMemoryInformation,
  SystemNextEventIdInformation,
  SystemEventIdsInformation,
  SystemCrashDumpInformation,
  SystemExceptionInformation,
  SystemCrashDumpStateInformation,
  SystemKernelDebuggerInformation,
  SystemContextSwitchInformation,
  SystemRegistryQuotaInformation,
  SystemExtendServiceTableInformation,
  SystemPrioritySeperation,
  SystemPlugPlayBusInformation,
  SystemDockInformation,
  SystemPowerInformation_,
  SystemProcessorSpeedInformation,
  SystemCurrentTimeZoneInformation,
  SystemLookasideInformation
} SYSTEM_INFORMATION_CLASS, *PSYSTEM_INFORMATION_CLASS;

// NtQuerysInformationOProcess
typedef NTSTATUS (WINAPI * PFN_NTQUERYINFORMATIONPROCESS)(::HANDLE, PROCESSINFOCLASS, ::PVOID, ULONG, ::PULONG);

// NtQuerySystemInformation
typedef NTSTATUS (WINAPI * PFN_NTQUERYSYSTEMINFORMATION)(::SYSTEM_INFORMATION_CLASS, ::PVOID, ::ULONG, ::PULONG);

#ifdef __cplusplus
}
#endif


static inline PSYSTEM_PROCESS_INFORMATION getProcessInfo();
static inline PSYSTEM_PROCESS_INFORMATION getByPID(PSYSTEM_PROCESS_INFORMATION head, ::DWORD pid);

PSYSTEM_PROCESS_INFORMATION getProcessInfo()
{
	// Load Library
	::HINSTANCE hNtDll = ::LoadLibraryW(L"ntdll.dll");
	if(hNtDll == NULL)
		return false;

	PFN_NTQUERYSYSTEMINFORMATION fnProcInfo = PFN_NTQUERYSYSTEMINFORMATION(::GetProcAddress(hNtDll, "NtQuerySystemInformation"));

	if(fnProcInfo == NULL)
	{
		::FreeLibrary(hNtDll);
			return false;
	}

	::ULONG ulSize;
	::LONG status;
	::LPBYTE pBuffer;
	::ULONG bufferSize;

	bufferSize = 0;
	do{
		bufferSize += 0x10000;
		pBuffer = (::LPBYTE)malloc(bufferSize);

	status = fnProcInfo(SystemProcessInformation, pBuffer, bufferSize, &ulSize);
	if(status == 0xC0000004 /*STATUS_INFO_LENGTH_MISMATCH*/) {
		free(pBuffer);
		}
	} while (status == 0xC0000004 /*STATUS_INFO_LENGTH_MISMATCH*/);

	::FreeLibrary(hNtDll);

	if(status == 0x00) {
		return (PSYSTEM_PROCESS_INFORMATION)pBuffer;
	}

	return NULL;
}

PSYSTEM_PROCESS_INFORMATION getByPID(PSYSTEM_PROCESS_INFORMATION head, ::DWORD pid)
{
	PSYSTEM_PROCESS_INFORMATION current = head;

	while (current != NULL) {
		if (current->ProcessId == (HANDLE)pid) {
			return current;
		}

		if(current->NextEntryOffset == 0) {
			current = NULL;
		} else {
			current = (PSYSTEM_PROCESS_INFORMATION)((::LPBYTE)current + current->NextEntryOffset);
		}
	}

	return NULL;
}


#endif

//**************************************************************************************************************//
namespace Parvicursor
{
   namespace Profiler
   {
	    //----------------------------------------------------
	    usage usage::operator -(usage &u)
	    {
			usage uu;

			uu.ru_idrss = abs(this->ru_idrss - u.ru_idrss);
			uu.ru_inblock = abs(this->ru_inblock - u.ru_inblock);
			uu.ru_isrss = abs(this->ru_isrss - u.ru_isrss);
			uu.ru_ixrss = abs(this->ru_ixrss - u.ru_ixrss);
			uu.ru_majflt = abs(this->ru_majflt - u.ru_majflt);
			uu.ru_maxrss = abs(this->ru_maxrss - u.ru_maxrss);
			uu.ru_minflt = abs(this->ru_minflt - u.ru_minflt);
			uu.ru_msgrcv = abs(this->ru_msgrcv - u.ru_msgrcv);
			uu.ru_msgsnd = abs(this->ru_msgsnd - u.ru_msgsnd);
			uu.ru_nivcsw = abs(this->ru_nivcsw - u.ru_nivcsw);
			uu.ru_nsignals = abs(this->ru_nsignals - u.ru_nsignals);
			uu.ru_nswap = abs(this->ru_nswap - u.ru_nswap);
			uu.ru_nvcsw = abs(this->ru_nvcsw - u.ru_nvcsw);
			uu.ru_oublock = abs(this->ru_oublock - u.ru_oublock);

			uu.ru_stime.tv_sec = abs(this->ru_stime.tv_sec - u.ru_stime.tv_sec);
			uu.ru_stime.tv_usec = abs(this->ru_stime.tv_usec - u.ru_stime.tv_usec);

			uu.ru_utime.tv_sec = abs(this->ru_utime.tv_sec - u.ru_utime.tv_sec);
			uu.ru_utime.tv_usec = abs(this->ru_utime.tv_usec - u.ru_utime.tv_usec);

			return uu;
	    }
		//----------------------------------------------------
		void ResourceProfiler::GetResourceUsage(Out usage *u)
		{
#if defined WIN32 || WIN64

			::HANDLE who = ::GetCurrentProcess();
			if(who == null)
				throw Exception(ErrorString(::GetLastError()));

			::FILETIME lpCreationTime, lpExitTime, lpKernelTime, lpUserTime;
			if(!::GetProcessTimes(who, &lpCreationTime, &lpExitTime, &lpKernelTime, &lpUserTime))
				throw Exception(ErrorString(::GetLastError()));

			::SYSTEMTIME lpSystemTime;
			if(!::FileTimeToSystemTime(&lpUserTime, &lpSystemTime))
				throw Exception(ErrorString(::GetLastError()));
			u->ru_utime.tv_sec = lpSystemTime.wSecond;
			u->ru_utime.tv_usec = lpSystemTime.wMilliseconds * 1000;

			if(!::FileTimeToSystemTime(&lpKernelTime, &lpSystemTime))
				throw Exception(ErrorString(GetLastError()));
			u->ru_stime.tv_sec = lpSystemTime.wSecond;
			u->ru_stime.tv_usec = lpSystemTime.wMilliseconds * 1000;

			::PROCESS_MEMORY_COUNTERS pmc;
			if(!::GetProcessMemoryInfo(who, &pmc, sizeof(pmc)))
				throw Exception(ErrorString(::GetLastError()));
			u->ru_maxrss = pmc.WorkingSetSize;
			u->ru_majflt = pmc.PageFaultCount;
			u->ru_nswap = pmc.PagefileUsage;
			/*
				PageFaultCount
				The number of page faults.

				PeakWorkingSetSize
				The peak working set size, in bytes.

				WorkingSetSize
				The current working set size, in bytes.

				QuotaPeakPagedPoolUsage
				The peak paged pool usage, in bytes.

				QuotaPagedPoolUsage
				The current paged pool usage, in bytes.

				QuotaPeakNonPagedPoolUsage
				The peak nonpaged pool usage, in bytes.

				QuotaNonPagedPoolUsage
				The current nonpaged pool usage, in bytes.

				PagefileUsage
				The current space allocated for the pagefile, in bytes. Those pages may or may not be in memory.

				PeakPagefileUsage
				The peak space allocated for the pagefile, in bytes.
			*/


			::IO_COUNTERS lpIoCounters;
			if(!::GetProcessIoCounters(who, &lpIoCounters))
				throw Exception(ErrorString(::GetLastError()));
			u->ru_inblock = lpIoCounters.ReadOperationCount;
			u->ru_oublock = lpIoCounters.WriteOperationCount;


			::PSYSTEM_PROCESS_INFORMATION head = getProcessInfo();
			::PSYSTEM_PROCESS_INFORMATION myProc = getByPID(head, ::GetCurrentProcessId());
			if(myProc != null)
			{
				u->ru_nvcsw = 0;
				u->ru_nivcsw = myProc->Threads->ContextSwitchCount;
				/*u->ru_inblock = myProc->IoCounters.ReadOperationCount;
				u->ru_oublock = myProc->IoCounters.WriteOperationCount;
				u->ru_maxrss= myProc->VirtualMemoryCounters.WorkingSetSize;
				u->ru_majflt = myProc->VirtualMemoryCounters.PageFaultCount;
				u->ru_nswap = myProc->VirtualMemoryCounters.PagefileUsage;*/
				/*printf("Image name %s\n", myProc->ImageName.Buffer);
				printf("Handle count %u\n", myProc->HandleCount);
				printf("Thread count %u\n", myProc->NumberOfThreads);
				printf("Virtual size %u\n", myProc->VirtualMemoryCounters.VirtualSize);*/
			}
			else
			{
				u->ru_nvcsw = 0;
				u->ru_nivcsw = 0;
				printf("Not found :-/\n");
			}

			if(head != null)
				free(head);


			/*POSIX compatibility*/
			u->ru_nsignals = 0;
			u->ru_msgsnd = 0;
			u->ru_msgrcv = 0;
			u->ru_minflt = 0;
			u->ru_isrss = 0;
			u->ru_ixrss = 0;
			u->ru_idrss = 0;

#else

			if(::getrusage(RUSAGE_SELF, (rusage *)u) < 0)
			{
				throw Exception((const char *)_sys_errlist[errno]);
			}



#endif

		}
		//----------------------------------------------------
		void ResourceProfiler::PrintResourceUsage(In usage *u)
		{
			cout << "user CPU time used - second: " << u->ru_utime.tv_sec <<  " microseconds: " << u->ru_utime.tv_usec << endl;
			cout << "system CPU time used - second: " << u->ru_stime.tv_sec <<  " microseconds: " << u->ru_stime.tv_usec << endl;
			cout << "maximum resident set size: " << u->ru_maxrss << endl;
			cout << "integral shared memory size: " << u->ru_ixrss << endl;
			cout << "integral unshared data size: " << u->ru_idrss << endl;
			cout << "integral unshared stack size: " << u->ru_isrss << endl;
			cout << "page reclaims (soft page faults): " << u->ru_minflt << endl;
			cout << "page faults (hard page faults): " << u->ru_majflt << endl;
			cout << "swaps: " << u->ru_nswap << endl;
			cout << "block input operations: " << u->ru_inblock << endl;
			cout << "block output operations: " << u->ru_oublock << endl;
			cout << "IPC messages sent: " << u->ru_msgsnd << endl;
			cout << "IPC messages received: " << u->ru_msgrcv << endl;
			cout << "signals received: " << u->ru_nsignals << endl;
			cout << "voluntary context switches: " << u->ru_nvcsw << endl;
			cout << "involuntary context switches: " << u->ru_nivcsw << endl;
		}
		//----------------------------------------------------
		Int64 ResourceProfiler::GetClock()
		{
			return clock();
		}
		//----------------------------------------------------
   };
};
//**************************************************************************************************************//


/*
		Not all fields are completed; unmaintained fields are set to zero by the
		kernel.  (The unmaintained fields are provided for compatibility with other
		systems, and because they may one day be supported on Linux.)  The fields are
		interpreted as follows:

		ru_utime
				This is the total amount of time spent executing in user mode,
				expressed in a timeval structure (seconds plus microseconds).

		ru_stime
				This is the total amount of time spent executing in kernel mode,
				expressed in a timeval structure (seconds plus microseconds).

		ru_maxrss (since Linux 2.6.32)
				This is the maximum resident set size used (in kilobytes). For
				RUSAGE_CHILDREN, this is the resident set size of the largest child,
				not the maximum resident set size of the process tree.

		ru_ixrss (unmaintained)
				This field is currently unused on Linux.

		ru_idrss (unmaintained)
				This field is currently unused on Linux.

		ru_isrss (unmaintained)
				This field is currently unused on Linux.

		ru_minflt
				The number of page faults serviced without any I/O activity; here I/O
				activity is avoided by "reclaiming" a page frame from the list of pages
				awaiting reallocation.

		ru_majflt
				The number of page faults serviced that required I/O activity.

		ru_nswap (unmaintained)
				This field is currently unused on Linux.

		ru_inblock (since Linux 2.6.22)
				The number of times the file system had to perform input.

		ru_oublock (since Linux 2.6.22)
				The number of times the file system had to perform output.

		ru_msgsnd (unmaintained)
				This field is currently unused on Linux.

		ru_msgrcv (unmaintained)
				This field is currently unused on Linux.

		ru_nsignals (unmaintained)
				This field is currently unused on Linux.

		ru_nvcsw (since Linux 2.6)
				The number of times a context switch resulted due to a process
				voluntarily giving up the processor before its time slice was completed
				(usually to await availability of a resource).

		ru_nivcsw (since Linux 2.6)
				The number of times a context switch resulted due to a higher priority
				process becoming runnable or because the current process exceeded its
				time slice.
*/
