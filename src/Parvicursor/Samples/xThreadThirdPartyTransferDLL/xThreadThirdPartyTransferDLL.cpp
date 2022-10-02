// xThreadThirdPartyTransferDLL.cpp : Defines the entry point for the DLL application.

// Important notice:
// To run your code correctly, please carefully read the template-like code comments.

// You MUST not use global variables in this shared library (DLL or SO),
// because it causes loader-lock deadlock at the OS kernel.
// You MUST not create threads in this class and MUST not use any
// synchronization mechanism such as mutexes, semaphores and critical sections
// (if used, provide releasing the locks within the deconstructor).

#include "xThreadThirdPartyTransferDLL.h"
//-----------------------------------------------------
xThreadThirdPartyTransfer::xThreadThirdPartyTransfer(const char *Dest, const char *LocalFilename, const char *RemoteFilename, Int32 parallel, Int32 blockSize, Int32 tcpBufferSize)
{
	this->dest = new char[(Int32)strlen(Dest) + 1];
	this->localFilename = new char[(Int32)strlen(LocalFilename) + 1];
	this->remoteFilename = new char[(Int32)strlen(RemoteFilename) + 1];
	strcpy(this->dest, Dest);
	strcpy(this->localFilename, LocalFilename);
	strcpy(this->remoteFilename, RemoteFilename);
	this->isFirstTimeInitilization = true;
	this->upc = null;
	this->parallel = parallel;
	this->blockSize = blockSize;
	this->tcpBufferSize = tcpBufferSize;
	this->nc = new NetworkCredential("user", "pass");
	this->remoteErrors = new ArrayList();
	this->hasRemoteErrors = false;
	this->out = null;
	this->mutex = null;

	// Here, you MUST set the values of serializer and deserializer pointers to null,
	// otherwise your program will have to crash.
	// SURE you have allocated all memories here and provided the de-allocation of them
	// in the ~xThreadThirdPartyTransfer() deconstructor. This means YOU must not allocate memory in Start() method
	// with new operator or malloc() function call.
	// For example, if you need an opened file handle in Start() method,
	// you MUST provide closing the file handle in ~xThreadThirdPartyTransfer() deconstructor.

	this->serializer = null;
	this->deserializer = null;
}
//-----------------------------------------------------
xThreadThirdPartyTransfer::xThreadThirdPartyTransfer()
{
	this->dest = null;
	this->localFilename = null;
	this->remoteFilename = null;
	this->isFirstTimeInitilization = true;
	this->parallel = 0;
	this->blockSize = 0;
	this->tcpBufferSize = 0;
	this->upc = null;
	this->nc = new NetworkCredential("user", "pass");
	this->remoteErrors = new ArrayList();
	this->hasRemoteErrors = false;
	this->out = null;

	this->mutex = null;
	//this->mutex = new Mutex();

	// Here, you MUST set the values of serializer and deserializer pointers to null,
	// otherwise your program will have to crash.

	this->serializer = null;
	this->deserializer = null;
}
//-----------------------------------------------------
xThreadThirdPartyTransfer::~xThreadThirdPartyTransfer()
{
	// Here, you MUST release the allocated C++ stack memory for
	// Serializer and DeSerializer objects to avoid memory leaks for your entire multi-threaded application.

	printf("xxxxxxxxxxxxxxxxxxxThreadThirdPartyTransfer()xxxxxxxxxxxxxxxx\n");

	if(this->mutex != null)
	{
		//this->mutex->Unlock();
		delete this->mutex;
		this->mutex = null;
	}
	
	if(this->upc != null)
	{
		this->upc->Close();
		delete this->upc;
		this->upc = null;
	}

	if(this->serializer != null)
	{
		delete this->serializer;
		this->serializer = null;
	}

	if(this->deserializer != null)
	{
		delete this->deserializer;
		this->deserializer = null;
	}

	if(this->dest != null)
	{
		delete this->dest;
		this->dest = null;
	}

	if(this->localFilename != null)
	{
		delete this->localFilename;
		this->localFilename = null;
	}

	if(this->remoteFilename != null)
	{
		delete this->remoteFilename;
		this->remoteFilename = null;
	}

	if(this->nc != null)
	{
		delete this->nc;
		this->nc = null;
	}

	if(this->remoteErrors != null)
	{
		for(register Int32 i = 0 ; i < remoteErrors->get_Count() ; i++)
		{
			System::Exception *e = (System::Exception *)remoteErrors->get_Value(i);
			delete e;
		}
		delete this->remoteErrors;
		this->remoteErrors = null;
	}

	if(this->out != null)
	{
		delete this->out;
		this->out = null;
	}
}
//-----------------------------------------------------
void xThreadThirdPartyTransfer::Start()
{
	// NOTE: You MUST never use the Thread::Sleep() or other thread sleep APIs in this method.
	// Because it may cause loader-lock deadlock in the OS kernel.
	// Don't define any variable that allocate memory from heap in this method. Use instead
	// the class variable members to allocate memory from heap. Don't declare local System::String
	// primitive data type in this method, since it allocates memory from heap.
	// This method is very useful in a pure compute-bound loop without Sleep() for execution
	// of scientific algorithms.

	try
	{
		this->upc = new UploadClient(String((const char *)this->localFilename), String((const char *)this->remoteFilename), this->parallel, this->blockSize, this->tcpBufferSize, 100, 100, String((const char *)this->dest), *this->nc, false, false, this->remoteErrors, true, true);
		this->upc->Run();
	}
	catch(System::Exception &e)
	{
		this->remoteErrors->Add(new System::Exception(e.get_Message()));
	}
	catch(...)
	{
		this->remoteErrors->Add(new System::Exception("An unknown error was occurred within xThreadThirdPartyTransfer::Start()."));
	}

	if(this->remoteErrors->get_Count() > 0)
	{
		printf("hasRemoteErrors\n");
		this->hasRemoteErrors = true;
	}

	/*int i = 0;
	while(true)
	{
		this->mutex->Lock();
		Thread::Sleep(10);
		i++;
		this->mutex->Unlock();
		printf("i: %d\n", i);
		Thread::Sleep(1);
	}*/
	//this->transferredBytes = this->upc->get_CurrentTransferredBytes();
	//upload_session->Close();
	return;

}
//-----------------------------------------------------
void xThreadThirdPartyTransfer::ReleaseCriticalSesctions()
{
	/*if(this->mutex != null)
		this->mutex->Unlock();*/
	printf("\nI'm ReleaseCriticalSesctions().\n");
	//Thread::Sleep(1);
}
//-----------------------------------------------------
void xThreadThirdPartyTransfer::AuxiliaryMehtod(In const void *input, Int32 inputLength, Out void **output, Out Int32 *outputLength)
{
	if(this->out == null)
	{
		this->out = new xThreadThirdPartyTransferContextOutput();
		this->out->transferredBytes = 0;
	}

	xThreadThirdPartyTransferContextInput *in = (xThreadThirdPartyTransferContextInput *)input;

	switch(in->command)
	{
		case __GetTransferredBytes__:
			if(this->upc != null)
			{
				this->out->transferredBytes = this->upc->get_CurrentTransferredBytes();
				printf("get_CurrentTransferredBytes: %d\n", this->out->transferredBytes);
			}
			else
				this->out->transferredBytes = 0;

			*output = this->out;
			*outputLength = sizeof(xThreadThirdPartyTransferContextOutput);
			break;
		default:
			printf("Default\n");
			*output = null;
			*outputLength = 0;
			break;
	}

	printf("\nI'm AuxiliaryMehtod().\n");
}
//-----------------------------------------------------
void xThreadThirdPartyTransfer::Deconstructor(xThreadBase *obj)
{
	// You MUST implement this method, since Parvicursor.NET Framework uses it extensively.

	if(obj != null)
	{
		delete ((xThreadThirdPartyTransfer *)obj);
		obj = null;
	}
}
//-----------------------------------------------------
void xThreadThirdPartyTransfer::FreeDllMemory(void *mem)
{
	// You MUST implement this method, since Parvicursor.NET Framework uses it extensively.
	// If the memory has been allocated by malloc() function, you MUST call here free() function.
	// If the memory has been allocated by C++ new operator, you MUST call here C++ delete operator.

	if(mem != null)
	{
		delete mem;
		mem = null;
	}
}
//-----------------------------------------------------
void *xThreadThirdPartyTransfer::AllocateDllMemory(UInt32 size)
{
	// You MUST implement this method, since Parvicursor.NET Framework uses it extensively.
	// If the memory has been allocated by malloc() function, you MUST call here free() function.
	// If the memory has been allocated by C++ new operator, you MUST call here C++ delete operator.

	return (void *)new char[size];
}
//-----------------------------------------------------
void xThreadThirdPartyTransfer::Serialize(char **DllBuffer, Int32 *len)
{
	// Don't change the values of len and DllBuffer variable pointers, since your program will have to crash.
	// These variables are to be used by Parvicursor.NET Framework for high-performance buffer transfers to
	// remote (or local) cross-process boundaries by Parvicursor Object-Passing Interface (XDOPI).

	Int32 bufferSize = 4*1024; // 4KB 				
																			// To avoid successive memory allocation overhead, use the pre-allocated memory and
																			// increase XDOPI peformance;
																			// you MUST consider to keep constant the bufferSize length in mind.
																			// Also, you can select a default size that you guess it is always
																			// greater than or equal to your real objects size. (e.g., a large 1MB buffer size)
	if(this->serializer == null)
		this->serializer = new Serializer(bufferSize);
	else
		this->serializer->Reset(bufferSize);

	if(isFirstTimeInitilization)
	{
		Int32 size;

		size = (Int32)strlen(this->dest);
		this->serializer->Write<Int32>(size);
		this->serializer->Write(this->dest, size);

		size = (Int32)strlen(this->localFilename);
		this->serializer->Write<Int32>(size);
		this->serializer->Write(this->localFilename, size);

		size =  (Int32)strlen(this->remoteFilename);
		this->serializer->Write<Int32>(size);
		this->serializer->Write(this->remoteFilename, size);

		this->serializer->Write<Int32>(this->parallel);
		this->serializer->Write<Int32>(this->blockSize);
		this->serializer->Write<Int32>(this->tcpBufferSize);

		isFirstTimeInitilization = false;
	}

	if(this->hasRemoteErrors)
	{
		this->serializer->Write<bool>(true);
		this->serializer->Write<Int32>(this->remoteErrors->get_Count());
		for(register Int32 i = 0 ; i < this->remoteErrors->get_Count() ; i++)
		{
			System::Exception *e = (System::Exception *)this->remoteErrors->get_Value(i);
			this->serializer->Write(e->get_Message());
		}
	}
	else
		this->serializer->Write<bool>(false);
	//this->serializer->Write<Int64>(this->upc->get_CurrentTransferredBytes);

	xThreadBase::MemoryCopy(DllBuffer, len, this->serializer);	// Mapping between C++ memory and DLL C Runtime memory.
															// If this line of code is removed at the end of this method implementation,
															// your program will have to crash.
}
//-----------------------------------------------------
void xThreadThirdPartyTransfer::DeSerialize(char **buffer, Int32 *len)
{
	// Don't change the values of len and buffer variable pointers, since your program will have to crash.
	// These variables are to be used by Parvicursor.NET Framework for high-performance buffer transfers to
	// remote (or local) cross-process boundaries by Parvicursor Object-Passing Interface (XDOPI).

	if(this->deserializer == null)
		this->deserializer = new DeSerializer(*buffer, *len);
	else
		this->deserializer->Reset(*buffer, *len);

	if(isFirstTimeInitilization)
	{
		Int32 size;

		size = this->deserializer->Read<Int32>();
		if(this->dest == null)
			this->dest = new char[size + 1];
		else if((Int32)strlen(this->dest) > size)
		{
			delete this->dest;
			this->dest = new char[size + 1];
		}
		this->deserializer->Read(this->dest, size);
		this->dest[size] = '\0';

		size = this->deserializer->Read<Int32>();
		if(this->localFilename == null)
			this->localFilename = new char[size + 1];
		else if((Int32)strlen(this->localFilename) > size)
		{
			delete this->localFilename;
			this->localFilename = new char[size + 1];
		}
		this->deserializer->Read(this->localFilename, size);
		this->localFilename[size] = '\0';

		size = this->deserializer->Read<Int32>();
		if(this->remoteFilename == null)
			this->remoteFilename = new char[size + 1];
		else if((Int32)strlen(this->remoteFilename) > size)
		{
			delete this->remoteFilename;
			this->remoteFilename = new char[size + 1];
		}
		this->deserializer->Read(this->remoteFilename, size);
		this->remoteFilename[size] = '\0';


		this->parallel = this->deserializer->Read<Int32>();
		this->blockSize = this->deserializer->Read<Int32>();
		this->tcpBufferSize = this->deserializer->Read<Int32>();

		isFirstTimeInitilization = false;
	}

	if(this->deserializer->Read<bool>())
	{
		Int32 n = this->deserializer->Read<Int32>();
		for(register Int32 i = 0 ; i < n ; i++)
		{
			String message = this->deserializer->Read();
			System::Exception *e = new System::Exception(message);
			this->remoteErrors->Add(e);
		}
	}

	//this->transferredBytes = this->deserializer->Read<Int64>();
}
//-----------------------------------------------------
#ifdef __cplusplus    // If used by C++ code,
extern "C" {          // we need to export the C interface
#endif


// You MUST not use global variables in this shared library(DLL or SO),
// because it causes loader-lock deadlock at the OS kernel.

//static void *xThreadBase_func_pointer = null;

xThreadBase_DLL_Export void *xThreadBase_creator()	// Don't change this function name and its declaration (prototype),
{													// since Parvicursor uses from this native function
													// to locate your exported DLL function at the runtime.

	// You MUST not use global variables in this shared library(DLL or SO),
	// because it causes loader-lock deadlock at the OS kernel.
	/*if(xThreadBase_func_pointer == null)
	{
		xThreadThirdPartyTransfer *s = new xThreadThirdPartyTransfer(); // Here, You MUST call new and instantiate your class to be run by Parvicursor.
		xThreadBase_func_pointer = (void *)s;
		return xThreadBase_func_pointer;
	}
	else
		return xThreadBase_func_pointer;*/

	xThreadThirdPartyTransfer *s = new xThreadThirdPartyTransfer(); // Here, You MUST call new operator and instantiate your class to be run by Parvicursor.
	return (void *)s;
}


#ifdef __cplusplus
}
#endif
//-----------------------------------------------------
// In Windows, you MUST not use the following Microsoft-specific function,
// because it causes loader-lock deadlock at the OS kernel.

/*	The below code snippet can be removed in Win platforms.
	It has been intended for advanced programmers in
	much more better coding the DLL's.
*/
/*#if defined WIN32 || WIN64
#ifndef __Win_DllMain__
#define __Win_DllMain__
//#define xThreadBase_API xThreadBase_DLL_Export
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
#endif
#endif*/
//-----------------------------------------------------
