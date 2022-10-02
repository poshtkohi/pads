/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __Parvicursor_Serialization_DeSerializer_h__
#define __Parvicursor_Serialization_DeSerializer_h__


#include "../../System/BasicTypes/BasicTypes.h"
#include "../../System/Object/Object.h"
#include "../../System/ArgumentException/ArgumentOutOfRangeException.h"
#include "../../System/ArgumentException/ArgumentNullException.h"

#include <memory.h>
#include <string.h>
#include <stdio.h>
//**************************************************************************************************************//

using namespace System;

namespace Parvicursor
{
	//----------------------------------------------------
   namespace Serialization
   {
		//----------------------------------------------------
		class DeSerializer : public Object
		{
			private: char *buffer;
			private: Int32 BufferSize;
			private: Int32 currentIndex;
			public: DeSerializer(char *buffer, Int32 BufferSize);
			//public: DeSerializer();
			//public: ~DeSerializer();
			public: void *Read(void *output, Int32 size);
			public: template <class _Object> const _Object Read()
			{
				_Object obj;
				this->Read(&obj, sizeof(obj));
				return obj;
			}
			public: const String Read();
			public: void Reset(char *buffer, Int32 BufferSize);
		};
		//----------------------------------------------------
	}
}
//**************************************************************************************************************//

#endif

