/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "xThreadBase.h"

//**************************************************************************************************************//
namespace Parvicursor
{
   namespace xThread
   {
	   	//----------------------------------------------------
		void xThreadBase::MemoryCopy(char **DllBuffer, int *len, Serializer *serializer)
		{
			if(serializer == null)
				throw ArgumentNullException("serializer", "serializer is a null reference.");

			char *buff = serializer->get_BaseBuffer();

			if(*len < serializer->get_BaseBufferSize())
			{
				if(*DllBuffer != null)
					delete *DllBuffer;

				*len = serializer->get_BaseBufferSize();
				*DllBuffer = new char[*len]; //cout << "hello 1" <<endl;
			}

			if(*DllBuffer == null)
			{
				*len = serializer->get_BaseBufferSize();
				*DllBuffer = new char[*len]; //cout << "hello 2" <<endl;
			}

			for(register Int32 i = 0 ; i < *len ; i++)
				*(*DllBuffer + i) = buff[i];
		}
		//----------------------------------------------------
		void xThreadBase::AuxiliaryMehtod(In const void *input, Int32 inputLength, Out void **output, Out Int32 *outputLength)
		{
			*outputLength = -1;
		}
		//----------------------------------------------------
		void xThreadBase::ReleaseCriticalSesctions()
		{
			return;
		}
   		//----------------------------------------------------
   };
};
//**************************************************************************************************************//
