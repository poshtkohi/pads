/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "Serializer.h"

//**************************************************************************************************************//
namespace Parvicursor
{
   namespace Serialization
   {
	   //----------------------------------------------------
		Serializer::Serializer(Int32 maxBufferCapcity)
		{
			if(maxBufferCapcity > Parvicursor::Serialization::Serializer::MaxSupportedBufferSize || maxBufferCapcity <= 0)
				throw ArgumentOutOfRangeException("maxBufferCapcity", "maxBufferCapcity must be less than " + Convert::ToString(MaxSupportedBufferSize) + " bytes and greater than zero");

			this->maxCapcity = maxBufferCapcity;
			this->buffer = new char[this->maxCapcity];

			this->currentIndex = 0;
			this->currentDataSize = 0;
			this->fromDirectBuffer = false;
		}
		//----------------------------------------------------
		Serializer::Serializer(char *buffer, Int32 bufferSize)
		{
			if(bufferSize <= 0)
				throw ArgumentOutOfRangeException("bufferSize", "bufferSize must be greater than zero");

			if(buffer == null)
				throw ArgumentNullException("buffer", "buffer is null reference");

			this->maxCapcity = bufferSize;
			this->buffer = buffer;
			this->fromDirectBuffer = true;

			this->currentIndex = 0;
			this->currentDataSize = 0;
		}
		//----------------------------------------------------
		/*Serializer::Serializer()
		{
				//this->buffer = null;
		}*/
		//----------------------------------------------------
		/*Serializer::Serializer()
		{
			::Serializer(4*1024);
		}*/
		//----------------------------------------------------
		Serializer::~Serializer()
		{
			if(this->fromDirectBuffer)
				return ;

			if(buffer != null)
			{
				delete buffer;
				buffer = null;
			}

		}
		//----------------------------------------------------
		void Serializer::Write(const void *data, Int32 size)
		{
			if(this->buffer == null)
				return ;

			if(data == null)
				throw ArgumentNullException("data", "data is null reference");

			if(size <= 0)
				throw ArgumentOutOfRangeException("size", "size must be greater than zero");

			if(this->currentIndex + size > this->maxCapcity)
						throw ArgumentOutOfRangeException("the buffered data size was overlapped from maxBufferCapcity");

			this->buffer += this->currentDataSize;
			//void * memcpy ( void * destination, const void * source, size_t num );
			memmove(this->buffer, data, size);

			this->buffer -= this->currentDataSize;

			this->currentIndex += size;
			this->currentDataSize += size;

			//printf("currentDataSize: %d\n", currentDataSize);

		}
		//----------------------------------------------------
		void Serializer::Write(const String &str)
		{
			if(str.get_BaseStream() == null)
				throw ArgumentNullException("str", "str is null reference");

			this->Write<Int32>(str.get_Length());
			this->Write(str.get_BaseStream(), str.get_Length());
		}
		//----------------------------------------------------
		char *Serializer::get_BaseBuffer() const
		{
			return this->buffer;
		}
		//----------------------------------------------------
		Int32 Serializer::get_BaseBufferSize() const
		{
			return this->currentDataSize;
		}
		//----------------------------------------------------
		void Serializer::Reset()
		{
			this->currentDataSize = 0;
			this->currentIndex = 0;
		}
		//----------------------------------------------------
		void Serializer::Reset(Int32 newBufferSize)
		{

			if(newBufferSize <= 0)
			{
				Reset();
				return ;
			}

			if(newBufferSize > MaxSupportedBufferSize)
				throw ArgumentOutOfRangeException("newBufferSize", "newBufferSize must be less than " + Convert::ToString(MaxSupportedBufferSize) + " bytes and greater than zero");

			if(newBufferSize > this->maxCapcity)
			{
				delete this->buffer;
				this->buffer = new char[newBufferSize];
				this->maxCapcity = newBufferSize;
			}

			Reset();
		}
		//----------------------------------------------------
		void Serializer::Reset(char *buffer, Int32 bufferSize)
		{
			if(bufferSize <= 0)
				throw ArgumentOutOfRangeException("bufferSize", "bufferSize must be greater than zero");

			if(buffer == null)
				throw ArgumentNullException("buffer", "buffer is null reference");

			if(!this->fromDirectBuffer)
				delete this->buffer;

			this->maxCapcity = bufferSize;
			this->buffer = buffer;
			this->fromDirectBuffer = true;

			this->currentIndex = 0;
			this->currentDataSize = 0;
		}
		//----------------------------------------------------
   };
};
//**************************************************************************************************************//
