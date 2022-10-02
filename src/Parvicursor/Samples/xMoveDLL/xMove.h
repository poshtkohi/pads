#ifndef __xMove_h__
#define __xMove_h__


#if defined WIN32 || WIN64
#ifndef xMove_IMPORTS
#define xMove_API __declspec (dllexport)
#else
#define xMove_API __declspec (dllimport)
#endif

#else
#define xMove_API
#endif

#include <general.h>
#include <System/String/String.h>
#include <System.Collections/ArrayList/ArrayList.h>
#include <System/BasicTypes/BasicTypes.h>
#include <System.IO/Directory/Directory.h>
#include <Parvicursor/xThread/xThreadBase.h>
#include <Parvicursor/Serialization/DeSerializer.h>
#include <Parvicursor/Serialization/Serializer.h>


#include <Parvicursor/xThread/xThreadClient/xThreadClient.h>
#include <Parvicursor/xThread/xThreadCollection.h>
#include <Parvicursor/xThread/xThreadBase.h>
#include <Parvicursor/xDFS/xDFSClients/FileStreamClient/FileStreamClient.h>

//----------------------------------------------------
using namespace System;
using namespace System::Collections;
using namespace System::IO;

using namespace Parvicursor::xThread;
using namespace Parvicursor::Serialization;
using namespace Parvicursor::xDFS::xDFSClients;
//----------------------------------------------------
class xMove_API xMove : public xThreadBase, public Object
{
	private: struct xMoveContextOutput
	{
		// Indicates the number of bytes sent to the sink node.
		public: Int32 length;

		public: xMoveContextOutput()
		{
			length = 0;
		}
		public: ~xMoveContextOutput()
		{
			length = 0;
		}
	};
	/*---------------------fields-----------------*/
	private: char *sourceFilename;
	private: char *sourceAddress;
	private: char *sourceComputeNodeAddress;
	private: Int64 offset;
	private: Int64 length;
	public: Int32 blockSize;
	private: char *sinkFilename;
	private: char *sinkAddress;
	private: char *sinkComputeNodeAddress;
	private: FileStreamClient *sourceFile;
	private: bool isFirstTimeInitilization;
	public: ArrayList *remoteErrors;
	private: NetworkCredential *nc;
	private: char *buffer;
	private: bool hasRemoteErrors;
	private: bool isSourceSide;
	private: bool disposed;
	/// The following variables used at sink side.
	private: xMove *xmove_sink;
	private: xThreadCollection *xtc_sink;
	private: xThreadClient *client_sink;
	private: FileStreamClient *sinkFile;
	/// The following variables are used for xRMI channel
	private: xMoveContextOutput *output;
	private: Int32 outputLength;
	/*---------------------methods----------------*/
	public: xMove(const char *sourceFilename, const char *sourceAddress, const char *sourceComputeNodeAddress, Int64 offset, Int64 length, Int32 blockSize, const char *sinkFilename, const char *sinkAddress, const char *sinkComputeNodeAddress, bool isSourceSide);
	public: xMove();
	public: ~xMove();
	/// Overridden methods
	public: void Start();
	public: void AuxiliaryMehtod(In const void *input, Int32 inputLength, Out void **output, Out Int32 *outputLength);
	public: void Serialize(char **DllBuffer, Int32 *len);
	public: void DeSerialize(char **buffer, Int32 *len);
	public: void FreeDllMemory(void *mem);
	public: void *AllocateDllMemory(UInt32 size);
	public: void Deconstructor(xThreadBase *obj);
	public: void ReleaseCriticalSesctions();
	/// Other methods
	/*--------------------------------------------*/
};
//---------------------------------------

#endif
