#ifndef __Parvicursor_Shared_Enums_xDFS_Methods_h__
#define __Parvicursor_Shared_Enums_xDFS_Methods_h__

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
				enum Methods
				{
					/// <summary>
					/// Read to file mode.
					/// </summary>
					ReadMethod = 0,
					/// <summary>
					/// Write to file mode.
					/// </summary>
					WriteMethod = 1,
					/// <summary>
					/// Flush mode.
					/// </summary>
					FlushMethod = 2,
					/// <summary>
					/// Lock mode.
					/// </summary>
					LockMethod = 3,
					/// <summary>
					/// Seek mode.
					/// </summary>
					SeekMethod = 4,
					/// <summary>
					/// Close mode.
					/// </summary>
					CloseMethod = 5,
					/// <summary>
					/// SetLength mode.
					/// </summary>
					SetLengthMethod = 6,
					/// <summary>
					/// UnLock mode.
					/// </summary>
					UnLockMethod = 7,
					/// <summary>
					/// Position mode.
					/// </summary>
					GetPositionMethod = 8,
					/// <summary>
					/// Length mode.
					/// </summary>
					GetLengthMethod = 9,
					/// <summary>
					/// CanSeek mode.
					/// </summary>
					CanSeekMethod = 10

				};
				//----------------------------------------------------
			}
		}
	}
}
//**************************************************************************************************************//

#endif
