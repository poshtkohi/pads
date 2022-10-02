/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "DeSerializer.h"

//**************************************************************************************************************//
namespace Parvicursor
{
   namespace Serialization
   {
	    //----------------------------------------------------
		DeSerializer::DeSerializer(char *buffer, Int32 BufferSize)
		{
			if(BufferSize <= 0)
				throw ArgumentOutOfRangeException("BufferSize", "BufferSize must be less than 10MB and greater than zero");

			this->BufferSize = BufferSize;
			this->buffer = buffer;

			this->currentIndex = 0;

		}
		//----------------------------------------------------
		void *DeSerializer::Read(void *output, Int32 size)
		{
			if(output == null)
				throw ArgumentNullException("data", "data is null reference");

			if(size <= 0)
				throw ArgumentOutOfRangeException("size", "size must be greater than zero");


			if(this->currentIndex + size > this->BufferSize)
					throw ArgumentOutOfRangeException("the buffered data size was overlapped from BufferSize");

			this->buffer += this->currentIndex;

			//void * memcpy ( void * destination, const void * source, size_t num );
			memmove(output, this->buffer, size);

			this->buffer -= this->currentIndex;

			this->currentIndex += size;

			//printf("currentIndex: %d\n", this->currentIndex);
			return output;

		}
		//----------------------------------------------------
		const String DeSerializer::Read()
		{
			Int32 len = this->Read<Int32>();
			char *str = new char[len + 1];
			str[len] = 0;
			Read(str, len);
			return String(str);
		}
		//----------------------------------------------------
		void DeSerializer::Reset(char *buffer, Int32 BufferSize)
		{
			//this->currentDataSize = 0;
			this->currentIndex = 0;
			this->buffer = buffer;
			this->BufferSize = BufferSize;
		}
		//----------------------------------------------------
   };
};
//**************************************************************************************************************//
