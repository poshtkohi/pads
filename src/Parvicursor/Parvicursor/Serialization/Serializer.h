/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __Parvicursor_Serialization_Serializer_h__
#define __Parvicursor_Serialization_Serializer_h__

#include "../../System/BasicTypes/BasicTypes.h"
#include "../../System/Object/Object.h"
#include "../../System/ArgumentException/ArgumentOutOfRangeException.h"
#include "../../System/ArgumentException/ArgumentNullException.h"
#include "../../System/Convert/Convert.h"

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
		class Serializer : public Object
		{
			private: char *buffer;
			private: Int32 maxCapcity;
			private: Int32 currentIndex;
			private: Int32 currentDataSize;
			private: bool fromDirectBuffer;
			private: static const Int32 MaxSupportedBufferSize = 100 * 1024 * 1024; //100MB
			//----------------------------------------------------
			public: Serializer(Int32 maxBufferCapcity);
			public: Serializer(char *buffer, Int32 bufferSize);
			//public: Serializer();
			public: ~Serializer();
			public: void Write(const void *data, Int32 size);
			public: void Write(const String &str);
			//----------------------------------------------------
			public: template <class Object> void Write(const Object &obj)
			{
				this->Write(&obj, sizeof(obj));
			}
			//----------------------------------------------------
			public: char *get_BaseBuffer() const;
			public: Int32 get_BaseBufferSize() const;
			public: void Reset();
			public: void Reset(Int32 newBufferSize);
			public: void Reset(char *buffer, Int32 bufferSize);
			//----------------------------------------------------
		};
		//----------------------------------------------------
	}
}
//**************************************************************************************************************//

#endif

