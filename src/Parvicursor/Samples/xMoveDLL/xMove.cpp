// xMoveDLL.cpp : Defines the entry point for the DLL application.

// Important notice:
// To run your code correctly, please carefully read the template-like code comments.

// You MUST not use global variables in this shared library (DLL or SO),
// because it causes loader-lock deadlock at the OS kernel.
// You MUST not create threads in this class and MUST not use any
// synchronization mechanism such as mutexes, semaphores and critical sections
// (if used, provide releasing the locks within the deconstructor).


extern char dllFilename[256];

#include "xMove.h"
//-----------------------------------------------------
xMove::xMove(const char *sourceFilename, const char *sourceAddress, const char *sourceComputeNodeAddress, Int64 offset, Int64 length, Int32 blockSize, const char *sinkFilename, const char *sinkAddress, const char *sinkComputeNodeAddress, bool isSourceSide)
{
	this->sourceFilename = new char[(Int32)strlen(sourceFilename) + 1];
	this->sourceAddress = new char[(Int32)strlen(sourceAddress) + 1];
	this->sourceComputeNodeAddress = new char[(Int32)strlen(sourceComputeNodeAddress) + 1];
	::strcpy(this->sourceFilename, sourceFilename);
	::strcpy(this->sourceAddress, sourceAddress);
	::strcpy(this->sourceComputeNodeAddress, sourceComputeNodeAddress);

	this->offset = offset;
	this->length = length;
	this->blockSize = blockSize;

	this->sinkFilename = new char[(Int32)strlen(sinkFilename) + 1];
	this->sinkAddress = new char[(Int32)strlen(sinkAddress) + 1];
	this->sinkComputeNodeAddress = new char[(Int32)strlen(sinkComputeNodeAddress) + 1];
	::strcpy(this->sinkFilename, sinkFilename);
	::strcpy(this->sinkAddress, sinkAddress);
	::strcpy(this->sinkComputeNodeAddress, sinkComputeNodeAddress);

	isFirstTimeInitilization = true;
	this->isSourceSide = isSourceSide;

	sourceFile = null;
	remoteErrors = new ArrayList();
	hasRemoteErrors = false;
	nc = new NetworkCredential("user", "pass");
	buffer = null;
	output = null;

	xmove_sink = null;
	xtc_sink = null;
	client_sink = null;


	// Here, you MUST set the values of serializer and deserializer pointers to null,
	// otherwise your program will have to crash.
	// SURE you have allocated all memories here and provided the de-allocation of them
	// in the ~xMove() deconstructor. This means YOU must not allocate memory in Start() method
	// with new operator or malloc() function call.
	// For example, if you need an opened file handle in Start() method,
	// you MUST provide closing the file handle in ~xMove() deconstructor.

	serializer = null;
	deserializer = null;
	disposed = false;
}
//-----------------------------------------------------
xMove::xMove()
{
	sourceFilename = null;
	sourceAddress = null;
	sourceComputeNodeAddress = null;

	offset = 0;
	length = 0;
	blockSize = 0;

	sinkFilename = null;
	sinkAddress = null;
	sinkComputeNodeAddress = null;

	isFirstTimeInitilization = true;
	sourceFile = null;
	remoteErrors = new ArrayList();
	hasRemoteErrors = false;
	isSourceSide = true;
	nc = new NetworkCredential("user", "pass");
	buffer = null;
	output = null;

	xmove_sink = null;
	xtc_sink = null;
	client_sink = null;
	sinkFile = null;

	// Here, you MUST set the values of serializer and deserializer pointers to null,
	// otherwise your program will have to crash.

	serializer = null;
	deserializer = null;
	disposed = false;
}
//-----------------------------------------------------
xMove::~xMove()
{
	// Here, you MUST release the allocated C++ stack memory for
	// Serializer and DeSerializer objects to avoid memory leaks for your entire multi-threaded application.

	if(!disposed)
	{
		if(serializer != null)
		{
			delete serializer;
			serializer = null;
		}

		if(deserializer != null)
		{
			delete deserializer;
			deserializer = null;
		}

		if(sourceFilename != null)
			delete sourceFilename;

		if(this->sourceAddress != null)
		{
			delete sourceAddress;
			sourceAddress = null;
		}

		if(sourceComputeNodeAddress != null)
		{
			delete sourceComputeNodeAddress;
			sourceComputeNodeAddress = null;
		}

		if(this->sinkFilename != null)
		{
			delete sinkFilename;
			sinkFilename = null;
		}

		if(this->sinkAddress != null)
		{
			delete sinkFilename;
			sinkFilename = null;
		}

		if(sinkComputeNodeAddress != null)
		{
			delete sinkComputeNodeAddress;
			sinkComputeNodeAddress = null;
		}

		if(remoteErrors != null)
		{
			for(register Int32 i = 0 ; i < remoteErrors->get_Count() ; i++)
			{
				System::Exception *e = (System::Exception *)remoteErrors->get_Value(i);
				delete e;
			}
			delete remoteErrors;
			remoteErrors = null;
		}

		if(nc != null)
		{
			delete nc;
			nc = null;
		}

		if(isSourceSide)
		{
			if(sourceFile != null)
			{
				try { sourceFile->Close(); } catch(...) { }

				delete sourceFile;
				sourceFile = null;
			}

			if(buffer != null)
			{
				delete buffer;
				buffer = null;
			}
		}
		else
		{
			if(xtc_sink != null)
			{
				delete xtc_sink;
				xtc_sink = null;
			}

			if(client_sink != null)
			{
				//client_sink->TerminateSession();
				delete client_sink;
				client_sink = null;
			}

			if(xmove_sink != null) 
			{
				delete xmove_sink;
				xmove_sink = null;
			}

			if(sinkFile != null)
			{
				try { sinkFile->Close(); } catch(...) { }

				delete sinkFile;
				sinkFile = null;
			}
		}

		if(output != null)
		{
			delete output;
			output = null;
		}

		disposed = true;
	}
}
//-----------------------------------------------------
void xMove::Start()
{
	// NOTE: You MUST never use the Thread::Sleep() or other thread sleep APIs in this method.
	// Because it may cause loader-lock deadlock in the OS kernel.
	// Don't define any variable that allocate memory from heap in this method. Use instead
	// the class variable members to allocate memory from heap. Don't declare local System::String
	// primitive data type in this method, since it allocates memory from heap.
	// This method is very useful in a pure compute-bound loop without Sleep() for execution
	// of scientific algorithms.

	if(isSourceSide)
	{
		printf("xMove::Start() - Source Side\n");

		sourceFile = new FileStreamClient(String((const char *)sourceFilename), System::IO::Open, System::IO::Read, blockSize, String((const char *)sourceAddress), *nc, false);

		try
		{
			sourceFile->Seek(offset, System::IO::Begin);

			xmove_sink = new xMove(sourceFilename, sourceAddress, sourceComputeNodeAddress, offset, length, blockSize, sinkFilename, sinkAddress, sinkComputeNodeAddress, false);

			xtc_sink = new xThreadCollection();
			//printf("xxx: %d\n", strlen(dllFilename));
			xtc_sink->AddNewThreadInstance(xmove_sink, String(dllFilename));

			client_sink = new xThreadClient(xtc_sink, sinkComputeNodeAddress, nc, true, remoteErrors); ///
			client_sink->Run();
			client_sink->WaitForSessionInitiation();

			// Establishes an xThread session for staring the xRMI channel.
			while( client_sink->get_IsAlive() )
			{
				if(client_sink->get_AreAllThreadsCompleted())
					break;

				Thread::Sleep(1);
			}

			if(remoteErrors->get_Count() > 0)
				goto Cleanup1;

			buffer = new char[sizeof(Int32) + blockSize];
			output = new xMoveContextOutput();
			Int32 n;

			// Reads from sourceFile and sends to sink compute node through xRMI channel.
			while( (n = sourceFile->Read(buffer + sizeof(Int32), 0, blockSize)) > 0 )
			{
				Parvicursor_GetBytesFromInt32Number(buffer, n);
				//printf("%d\n", n);
				//printf("client_sink->AuxiliaryMehtodOneThread: 1\n");
				client_sink->AuxiliaryMehtodOneThread(0, (const void *)buffer, sizeof(Int32) + n, output, &outputLength);
				//printf("client_sink->AuxiliaryMehtodOneThread: 2 outputLength: %d\n", outputLength);

				if(outputLength > 0)
				{
					//printf("client_sink->AuxiliaryMehtodOneThread: 3\n");
					if(output->length < 0)
					{
						//printf("client_sink->AuxiliaryMehtodOneThread: 4\n");
						remoteErrors->Add(new System::Exception("An error was occurred within while sending file blocks to the sink compute node through xRMI."));
						break;
					}
				}
			}

			client_sink->TerminateSession();

		}
		catch(System::Exception &e)
		{
			printf("eeeeeeeeeeeeeeeeeee: %s", e.get_Message().get_BaseStream());
			remoteErrors->Add(new System::Exception(e.get_Message()));
		}
		catch(...)
		{
			remoteErrors->Add(new System::Exception("An unknown error was occurred within xThreadThirdPartyTransfer::Start()."));
		}

Cleanup1:
		if(remoteErrors->get_Count() > 0)
		{
			printf("hasRemoteErrors\n");
			hasRemoteErrors = true;
			if(client_sink != null)
			{
				try { client_sink->TerminateSession(); } catch(...) { }
			}
			return;
		}
	}
	else
	{
		printf("xMove::Start() - Sink Side\n");
		sinkFile = new FileStreamClient(String((const char *)sinkFilename), System::IO::OpenOrCreate, System::IO::Write, blockSize, String((const char *)sinkAddress), *nc, false);

		try
		{
			sinkFile->Seek(offset, System::IO::Begin);
		}
		catch(System::Exception &e)
		{
			printf("eeeeeeeeeeeeeeeeeee: %s", e.get_Message().get_BaseStream());
			remoteErrors->Add(new System::Exception(e.get_Message()));
		}
		catch(...)
		{
			remoteErrors->Add(new System::Exception("An unknown error was occurred within xThreadThirdPartyTransfer::Start()."));
		}

Cleanup2:
		if(remoteErrors->get_Count() > 0)
		{
			printf("hasRemoteErrors\n");
			hasRemoteErrors = true;
			if(client_sink != null)
			{
				try { client_sink->TerminateSession(); } catch(...) { }
			}
			return;
		}

		return ;
	}


	return;

}
//-----------------------------------------------------
void xMove::ReleaseCriticalSesctions()
{

}
//-----------------------------------------------------
// The following method is executed at sink-side compute node.
void xMove::AuxiliaryMehtod(In const void *input, Int32 inputLength, Out void **output, Out Int32 *outputLength)
{
	if(this->output == null)
	{
		this->output = new xMoveContextOutput();
		this->output->length = 0;
	}

	char *in = (char *)input;
	Int32 length = Parvicursor_GetInt32NumberFromBytes((Byte *)in);

	try
	{
		sinkFile->Write(in + sizeof(Int32), 0, length);
		this->output->length = length;
	}
	catch(System::Exception &e)
	{
		this->output->length = -1;
	}
	catch(...)
	{
		this->output->length = -1;
	}

	*output = this->output;
	*outputLength = sizeof(xMoveContextOutput);

	/*if(isSourceSide)
		return;*/

	/*printf("xMove::AuxiliaryMehtod\n");

	if(this->output == null)
	{
		this->output = new xMoveContextOutput();
		this->output->length = 0;
	}

	xMoveContextInput *in = (xMoveContextInput *)input;

	try
	{
		sinkFile->Write(in->buffer, 0, in->length);
		this->output->length = in->length;
	}
	catch(System::Exception &e)
	{
		this->output->length = -1;
	}
	catch(...)
	{
		this->output->length = -1;
	}

	*output = this->output;
	*outputLength = sizeof(xMoveContextOutput);*/
}
//-----------------------------------------------------
void xMove::Deconstructor(xThreadBase *obj)
{
	// You MUST implement this method, since Parvicursor.NET Framework uses it extensively.

	if(obj != null)
	{
		delete ((xMove *)obj);
		obj = null;
	}
}
//-----------------------------------------------------
void xMove::FreeDllMemory(void *mem)
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
void *xMove::AllocateDllMemory(UInt32 size)
{
	// You MUST implement this method, since Parvicursor.NET Framework uses it extensively.
	// If the memory has been allocated by malloc() function, you MUST call here free() function.
	// If the memory has been allocated by C++ new operator, you MUST call here C++ delete operator.

	return (void *)new char[size];
}
//-----------------------------------------------------
void xMove::Serialize(char **DllBuffer, Int32 *len)
{
	// Don't change the values of len and DllBuffer variable pointers, since your program will have to crash.
	// These variables are to be used by Parvicursor.NET Framework for high-performance buffer transfers to
	// remote (or local) cross-process boundaries by Parvicursor Object-Passing Interface (XDOPI).

	Int32 bufferSize = 4*1024; // 4KB 				
	// To avoid successive memory allocation overhead, use the pre-allocated memory and
	// increase XDOPI performance;
	// you MUST consider to keep constant the bufferSize length in mind.
	// Also, you can select a default size that you guess it is always
	// greater than or equal to your real objects size. (e.g., a large 1MB buffer size)
	if(serializer == null)
		serializer = new Serializer(bufferSize);
	else
		this->serializer->Reset(bufferSize);

	cout << "xMove::Serialize()" << endl;

	if(isFirstTimeInitilization)
	{

		Int32 size;

		size = (Int32)strlen(sourceFilename);
		serializer->Write<Int32>(size);
		serializer->Write(sourceFilename, size);

		size = (Int32)strlen(sourceAddress);
		serializer->Write<Int32>(size);
		serializer->Write(sourceAddress, size);

		size = (Int32)strlen(sourceComputeNodeAddress);
		serializer->Write<Int32>(size);
		serializer->Write(sourceComputeNodeAddress, size);

		serializer->Write<Int64>(offset);
		serializer->Write<Int64>(length);
		serializer->Write<Int32>(blockSize);

		size = (Int32)strlen(sinkFilename);
		serializer->Write<Int32>(size);
		serializer->Write(sinkFilename, size);

		size = (Int32)strlen(sinkAddress);
		serializer->Write<Int32>(size);
		serializer->Write(sinkAddress, size);

		size = (Int32)strlen(sinkComputeNodeAddress);
		serializer->Write<Int32>(size);
		serializer->Write(sinkComputeNodeAddress, size);

		serializer->Write<bool>(isSourceSide);

		isFirstTimeInitilization = false;
	}
	else
	{
		serializer->Write<Int32>(blockSize);
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
void xMove::DeSerialize(char **buffer, Int32 *len)
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

		//////////////////////////////////////////////////////////////////////////
		size = deserializer->Read<Int32>();
		if(sourceFilename == null)
			sourceFilename = new char[size + 1];
		else if((Int32)strlen(sourceFilename) > size)
		{
			delete sourceFilename;
			sourceFilename = new char[size + 1];
		}
		deserializer->Read(sourceFilename, size);
		sourceFilename[size] = '\0';

		size = deserializer->Read<Int32>();
		if(sourceAddress == null)
			sourceAddress = new char[size + 1];
		else if((Int32)strlen(sourceAddress) > size)
		{
			delete sourceAddress;
			sourceAddress = new char[size + 1];
		}
		deserializer->Read(sourceAddress, size);
		sourceAddress[size] = '\0';

		size = deserializer->Read<Int32>();
		if(sourceComputeNodeAddress == null)
			sourceComputeNodeAddress = new char[size + 1];
		else if((Int32)strlen(sourceComputeNodeAddress) > size)
		{
			delete sourceComputeNodeAddress;
			sourceComputeNodeAddress = new char[size + 1];
		}
		deserializer->Read(sourceComputeNodeAddress, size);
		sourceComputeNodeAddress[size] = '\0';
		//////////////////////////////////////////////////////////////////////////
		 offset = deserializer->Read<Int64>();
		 length = deserializer->Read<Int64>();
		 blockSize = deserializer->Read<Int32>();
		//////////////////////////////////////////////////////////////////////////
		size = deserializer->Read<Int32>();
		if(sinkFilename == null)
			sinkFilename = new char[size + 1];
		else if((Int32)strlen(sinkFilename) > size)
		{
			delete sinkFilename;
			sinkFilename = new char[size + 1];
		}
		deserializer->Read(sinkFilename, size);
		sinkFilename[size] = '\0';

		size = deserializer->Read<Int32>();
		if(sinkAddress == null)
			sinkAddress = new char[size + 1];
		else if((Int32)strlen(sinkAddress) > size)
		{
			delete sinkAddress;
			sinkAddress = new char[size + 1];
		}
		deserializer->Read(sinkAddress, size);
		sinkAddress[size] = '\0';

		size = deserializer->Read<Int32>();
		if(sinkComputeNodeAddress == null)
			sinkComputeNodeAddress = new char[size + 1];
		else if((Int32)strlen(sinkComputeNodeAddress) > size)
		{
			delete sinkComputeNodeAddress;
			sinkComputeNodeAddress = new char[size + 1];
		}
		deserializer->Read(sinkComputeNodeAddress, size);
		sinkComputeNodeAddress[size] = '\0';
		//////////////////////////////////////////////////////////////////////////
		isSourceSide = deserializer->Read<bool>();
		//////////////////////////////////////////////////////////////////////////

		isFirstTimeInitilization = false;
	}
	else
	{
		blockSize = deserializer->Read<Int32>();
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
		xMove *s = new xMove(); // Here, You MUST call new and instantiate your class to be run by Parvicursor.
		xThreadBase_func_pointer = (void *)s;
		return xThreadBase_func_pointer;
	}
	else
		return xThreadBase_func_pointer;*/

	xMove *s = new xMove(); // Here, You MUST call new operator and instantiate your class to be run by Parvicursor.
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
