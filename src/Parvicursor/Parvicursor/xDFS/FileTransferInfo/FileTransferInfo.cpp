/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "FileTransferInfo.h"

//**************************************************************************************************************//
namespace Parvicursor
{
   namespace xDFS
   {
		//----------------------------------------------------
	    FileTransferInfo::FileTransferInfo(const String &guid, const String &writeFilename, Int64 fileSize, Int32  parallel, Int32 blockSize, Int32 tcpBufferSize, bool isServerSideZeroCopyEnabled, Int32 circularBufferCount)
		{
			if(guid.get_BaseStream() == null)
				throw ArgumentNullException("guid");

			if(writeFilename.get_BaseStream() == null)
				throw ArgumentNullException("writeFilename");

			if(blockSize <= 0)
				blockSize = 256 * 1204;

			if(tcpBufferSize <= 0)
				tcpBufferSize = 64 * 1204;

			if(parallel <= 0)
				parallel = 1;

			this->guid = guid;
			this->writeFilename = writeFilename;
			this->fileSize = fileSize;
			this->parallel = parallel;
			this->blockSize = blockSize;
			this->tcpBufferSize = tcpBufferSize;
			this->isServerSideZeroCopyEnabled = isServerSideZeroCopyEnabled;
			this->circularBufferCount = circularBufferCount;
		}
		//----------------------------------------------------
	    FileTransferInfo::FileTransferInfo()
		{
		}
		//----------------------------------------------------
	    FileTransferInfo::~FileTransferInfo()
		{
		}
		//----------------------------------------------------
   };
};
//**************************************************************************************************************//
