/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __Parvicursor_Shared_Enums_xDFS_CircularBufferState_h__
#define __Parvicursor_Shared_Enums_xDFS_CircularBufferState_h__

//**************************************************************************************************************//


namespace Parvicursor
{
   namespace Shared
   {
	    namespace Enums
		{
			namespace xDFS
			{
				//----------------------------------------------------
				enum CircularBufferState
				{
					BufferOperationCompleted = 0, 
					EndOfFile = -1,
					BufferIsFull = -2,
					BufferIsEmpty = -3,
					EndOfFileButBufferNotEmpty = -4,
					EndOfFileAndBufferIsEmpty = -5
				};
				//----------------------------------------------------
			}
		}
	}
}
//**************************************************************************************************************//

#endif
