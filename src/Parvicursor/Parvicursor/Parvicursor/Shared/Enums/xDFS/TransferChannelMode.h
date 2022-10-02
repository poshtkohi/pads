/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __Parvicursor_Shared_Enums_xDFS_TransferChannelMode_h__
#define __Parvicursor_Shared_Enums_xDFS_TransferChannelMode_h__

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
				enum TransferChannelMode
				{
					/// <summary>
					/// Meaning transferring a single file on the xDFS session from client to server.
					/// </summary>
					SingleFileTransferUploadFromClient = 1,
					/// <summary>
					/// Meaning transferring a single file on the xDFS session from server to client.
					/// </summary>
					SingleFileTransferDownloadFromClient = 2,
					/// <summary>
					/// Meaning moving a directory tree on the xDFS session from client to server.
					/// </summary>
					DirectoryMovementUploadFromClient = 3,
					/// <summary>
					/// Meaning moving a FileStream request on the xDFS session from client to server.
					/// </summary>
					FileStreamFromClient = 4,

					xThreadRequestFromClient = 5

				};
				//----------------------------------------------------
			}
		}
	}
}
//**************************************************************************************************************//

#endif
