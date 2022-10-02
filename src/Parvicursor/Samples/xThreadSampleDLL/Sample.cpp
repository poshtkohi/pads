// xThreadSampleDLL.cpp : Defines the entry point for the DLL application.
//

// Important notice:
// To run your code correctly, please carefully read the template-like code comments.

// You MUST not use global variables in this shared library (DLL or SO),
// because it causes loader-lock deadlock at the OS kernel.
// You MUST not create threads in this class and MUST not use any
// synchronization mechanism such as mutexes, semaphores and critical sections
// (if used, provide realesing the locks within the deconstructor).

#include "Sample.h"
//-----------------------------------------------------
Sample::Sample(Int32 _x, Int32 _y)
{
	this->x = _x;
	this->y = _y;
	this->z = 0;
	this->out = null;

	// Here, you MUST set the values of serializer and deserializer pointers to null,
	// otherwise your program will have to crash.
	// SURE you have allocated all memories here and provided the de-allocation of them
	// in the ~Sample() deconstructor. This means YOU must not allocate memory in Start() method
	// with new opertaor or malloc() fucntion call.
	// For example, if you need an opened file handle in Start() method,
	// you MUST provide closing the file handle in ~Sample() deconstructor.

	this->serializer = null;
	this->deserializer = null;
}
//-----------------------------------------------------
Sample::Sample()
{
	this->x = 0;
	this->y = 0;
	this->z = 0;
	this->out = null;

	// Here, you MUST set the values of serializer and deserializer pointers to null,
	// otherwise your program will have to crash.

	this->serializer = null;
	this->deserializer = null;
}
//-----------------------------------------------------
Sample::~Sample()
{
	// Here, you MUST release the allocated C++ stack memory for
	// Serializer and DeSerializer objects to avoid memory leaks for your entire multi-threaded application.

	printf("xxxxxxxxxxxxxxxxxxxxxxxxSample()xxxxxxxxxxxxxxx\n");

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
	if(this->out != null)
	{
		delete this->out;
		this->out = null;
	}
}
//-----------------------------------------------------
void Sample::Start()
{
	// NOTE: You MUST never use the Thread::Sleep() or other thread sleep APIs in this method.
	// Because it may cause loader-lock deadlock in the OS kernel.
	// Don't define any variable that allocate memory from heap in this method. Use instead
	// the class variable members to allocate memory from heap. Don't decalre local System::String
	// primitive data type in this method, since it allocates memory from heap.
	// This method is very useful in a pure compute-bound loop without Sleep() for execution
	// of scientific algoritms.

	this->z = this->x + this->y;
	printf("Start\n");
	printf("x: %d, y: %d, z: %d\n", this->x, this->y, this->z);

	/*for(register Int32 i = 0 ; i < 100 ; i++)
	{
		//this->z++;
		printf("i: %d\n", i);
		//printf("z: %d\n", this->z);
		Thread::Sleep(1); //Avoid such this fucntion call in this method.
	}*/
	//Thread::Sleep(100000000);
}
//-----------------------------------------------------
void Sample::AuxiliaryMehtod(In const void *input, Int32 inputLength, Out void **output, Out Int32 *outputLength)
{
	if(this->out == null)
		this->out = new SampleContextOutput();

	SampleContextInput *in = (SampleContextInput *)input;

	switch(in->command)
	{
		case __Add__:
			this->out->z = in->x + in->y;
			break;
		case __Minus__:
			this->out->z = in->x - in->y;
			break;
		case __Multiply__:
			this->out->z = in->x * in->y;
			break;
		case __Devide__:
			this->out->z = in->x / in->y;
			break;
		default:
			this->out->z = -1;
			break;
	}

	printf("x: %d y:%d z:%d\n", in->x, in->y, this->out->z);

	*output = out;
	*outputLength = sizeof(SampleContextOutput);

	printf("I'm AuxiliaryMehtod().\n");
}
//-----------------------------------------------------
void Sample::Deconstructor(xThreadBase *obj)
{
	// You MUST implement this method, since Parvicursor.NET Framework uses it extensively.

	if(obj != null)
	{
		delete ((Sample *)obj);
		obj = null;
	}
}
//-----------------------------------------------------
void Sample::FreeDllMemory(void *mem)
{
	// You MUST implement this method, since Parvicursor.NET Framework uses it extensively.
	// If the memory has been allocated by malloc() function, you MUST call here free() fucntion.
	// If the memory has been allocated by C++ new operator, you MUST call here C++ delete operator.

	if(mem != null)
	{
		delete mem;
		mem = null;
	}
}
//-----------------------------------------------------
void *Sample::AllocateDllMemory(UInt32 size)
{
	// You MUST implement this method, since Parvicursor.NET Framework uses it extensively.
	// If the memory has been allocated by malloc() function, you MUST call here free() fucntion.
	// If the memory has been allocated by C++ new operator, you MUST call here C++ delete operator.

	return (void *)new char[size];
}
//-----------------------------------------------------
void Sample::Serialize(char **DllBuffer, Int32 *len)
{
	// Don't change the values of len and DllBuffer variable pointers, since your program will have to crash.
	// These variables are to be used by Parvicursor.NET Framework for high-performance buffer transfers to
	// remote (or local) cross-process boundries by Parvicursor Object-Passing Interface (XDOPI).

	Int32 bufferSize = sizeof(this->x) + sizeof(this->y) + sizeof(this->z); // To avoid successive memory allocation overhead, use the pre-allocated memory and
																			// increase XDOPI peformance;
																			// you MUST consider to keep constant the bufferSize length in mind.
																			// Also, you can select a default size that you guess it is always
																			// greater than or equal to your real objects size. (e.g., a large 1MB buffer size)

	if(this->serializer == null)
		this->serializer = new Serializer(bufferSize);
	else
		this->serializer->Reset(bufferSize);

	this->serializer->Write<Int32>(this->x);
	this->serializer->Write<Int32>(this->y);
	this->serializer->Write<Int32>(this->z);

	xThreadBase::MemoryCopy(DllBuffer, len, this->serializer);	// Mapping between C++ memory and DLL C Runtime memory.
															// If this line of code is removed at the end of this method implementation,
															// your program will have to crash.

}
//-----------------------------------------------------
void Sample::DeSerialize(char **buffer, Int32 *len)
{
	// Don't change the values of len and buffer variable pointers, since your program will have to crash.
	// These variables are to be used by Parvicursor.NET Framework for high-performance buffer transfers to
	// remote (or local) cross-process boundries by Parvicursor Object-Passing Interface (XDOPI).

	if(this->deserializer == null)
		this->deserializer = new DeSerializer(*buffer, *len);
	else
		this->deserializer->Reset(*buffer, *len);

	this->x = deserializer->Read<Int32>();
	this->y = deserializer->Read<Int32>();
	this->z = deserializer->Read<Int32>();
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
		Sample *s = new Sample(); // Here, You MUST call new and instantiate your class to be run by Parvicursor.
		xThreadBase_func_pointer = (void *)s;
		return xThreadBase_func_pointer;
	}
	else
		return xThreadBase_func_pointer;*/

	Sample *s = new Sample(); // Here, You MUST call new operator and instantiate your class to be run by Parvicursor.
	return (void *)s;
}


#ifdef __cplusplus
}
#endif
//-----------------------------------------------------
// In Windows, you MUST not use the following Microsoft-specific fucntion,
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
