#ifndef _xThreadPiCalc_h
#define _xThreadPiCalc_h


#if defined WIN32 || WIN64
#ifndef xThreadPiCalc_IMPORTS
#define xThreadPiCalc_API __declspec (dllexport)
#else
#define xThreadPiCalc_API __declspec (dllimport)
#endif

#else
#define xThreadPiCalc_API
#endif

#include "../../Parvicursor/general.h"
#include "../../Parvicursor/System/String/String.h"
#include "../../Parvicursor/System/Math/Math.h"
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

class xThreadPiCalc_API xThreadPiCalc : public xThreadBase, public Object
{
	public:
		Int32 startDigitNum;
		Int32 numDigits;
		Int32 strLen;
		char *result;
	
	public:
		xThreadPiCalc(Int32 startDigitNum, Int32 numDigits);
		xThreadPiCalc();
		~xThreadPiCalc();
		void Start();
		void Serialize(char **DllBuffer, Int32 *len);
		void DeSerialize(char **buffer, Int32 *len);
		void FreeDllMemory(void *mem);
		void *AllocateDllMemory(UInt32 size);
		void Deconstructor(xThreadBase *obj);
};

#endif
