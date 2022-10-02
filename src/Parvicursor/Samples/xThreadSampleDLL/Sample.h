//#include "BaseThread.h"
//#include <stdio.h>
#ifndef _Sample_h
#define _Sample_h

/*#ifndef Sample_IMPORTS
#define Sample_API __declspec (dllexport)
#else
#define Sample_API __declspec (dllimport)
#endif*/


#if defined WIN32 || WIN64
#ifndef Sample_IMPORTS
#define Sample_API __declspec (dllexport)
#else
#define Sample_API __declspec (dllimport)
#endif

#else
#define Sample_API
#endif

#include "../../Parvicursor/general.h"
#include "../../Parvicursor/System/String/String.h"
#include "../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../Parvicursor/Parvicursor/xThread/xThreadBase.h"
#include "../../Parvicursor/Parvicursor/Serialization/DeSerializer.h"
#include "../../Parvicursor/Parvicursor/Serialization/Serializer.h"
#include "../../Parvicursor/System.Threading/Thread/Thread.h"

//---------------------------------------
using namespace System;
using namespace System::Threading;

using namespace Parvicursor::xThread;
using namespace Parvicursor::Serialization;
//---------------------------------------
enum SampleCommands
{
	__Add__ = (Byte) 0,
	__Minus__ = (Byte) 1,
	__Multiply__ = (Byte) 2,
	__Devide__ = (Byte) 3
};
struct SampleContextInput
{
	SampleCommands command;
	Int32 x;
	Int32 y;
};

struct SampleContextOutput
{
	Int32 z;
};
//---------------------------------------
class Sample_API Sample : public xThreadBase, public Object
{
	public:
		Int32 x, y, z;
	private:
		SampleContextOutput *out;
	public:
		Sample(Int32 x, Int32 y);
		Sample();
		~Sample();
		void Start();
		void AuxiliaryMehtod(In const void *input, Int32 inputLength, Out void **output, Out Int32 *outputLength);
		void Serialize(char **DllBuffer, Int32 *len);
		void DeSerialize(char **buffer, Int32 *len);
		void FreeDllMemory(void *mem);
		void *AllocateDllMemory(UInt32 size);
		void Deconstructor(xThreadBase *obj);
};
//---------------------------------------
#endif
