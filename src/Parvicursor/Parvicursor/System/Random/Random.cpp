/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "Random.h"

//**************************************************************************************************************//

namespace System
{
		//----------------------------------------------------
		Random::Random()
		{
			srand(time(null));
		}
		//----------------------------------------------------
		Random::Random(Int32 seed)
		{
			srand(seed);
		}
		//----------------------------------------------------
		Int32 Random::Next()
		{
			return rand();
		}
		//----------------------------------------------------
		Int32 Random::Next(Int32 maxValue)
		{
			return rand()%maxValue;
		}
		//----------------------------------------------------
		Int32 Random::Next(Int32 minValue, Int32 maxValue)
		{
			return (rand()%(maxValue - minValue)) + minValue;
		}
	    //----------------------------------------------------
}
//**************************************************************************************************************//
