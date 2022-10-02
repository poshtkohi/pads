/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __Parvicursor_xDFS_FileTransferInfo_h__
#define __Parvicursor_xDFS_FileTransferInfo_h__

#include "../../../general.h"
#include "../../../System/BasicTypes/BasicTypes.h"
#include "../../../System/Object/Object.h"
#include "../../../System/String/String.h"
#include "../../../System/ArgumentException/ArgumentNullException.h"

//**************************************************************************************************************//

using namespace System;

namespace Parvicursor
{
	//----------------------------------------------------
   namespace xDFS
   {
		//----------------------------------------------------
		class FileTransferInfo : public Object
		{
			public: String guid;
			public: String writeFilename;
			public: Int32 parallel;
			public: Int32 tcpBufferSize;
			public: Int32 blockSize;
			//public: int streamBufferSize;
			public: Int64 fileSize;
			public: bool isServerSideZeroCopyEnabled;
			public: Int32 circularBufferCount;
			//public: bool circularBufferEnabled;
			//----------------------------------------------------
			public: FileTransferInfo(const String &guid, const String &writeFilename, Int64 fileSize, Int32  parallel, Int32 blockSize, Int32 tcpBufferSize, bool isServerSideZeroCopyEnabled, Int32 circularBufferCount);
			public: FileTransferInfo();
			public: ~FileTransferInfo();
			//----------------------------------------------------
		};
		//----------------------------------------------------
	};
};
//**************************************************************************************************************//

#endif

