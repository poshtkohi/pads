#ifndef _xThreadThirdPartyTransfer_h
#define _xThreadThirdPartyTransfer_h


#if defined WIN32 || WIN64
#ifndef xThreadThirdPartyTransfer_IMPORTS
#define xThreadThirdPartyTransfer_API __declspec (dllexport)
#else
#define xThreadThirdPartyTransfer_API __declspec (dllimport)
#endif

#else
#define xThreadThirdPartyTransfer_API
#endif

#include "../../Parvicursor/general.h"
#include "../../Parvicursor/System/String/String.h"
#include "../../Parvicursor/System.Collections/ArrayList/ArrayList.h"
#include "../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../Parvicursor/Parvicursor/xThread/xThreadBase.h"
#include "../../Parvicursor/Parvicursor/Serialization/DeSerializer.h"
#include "../../Parvicursor/Parvicursor/Serialization/Serializer.h"
#include "../../Parvicursor/System.Threading/Thread/Thread.h"
#include "../../Parvicursor/System.Net.Sockets/Socket/Socket.h"
#include "../../Parvicursor/System.Net/NetworkCredential/NetworkCredential.h"
#include "../../Parvicursor/Parvicursor/Parvicursor.Net/ParvicursorSocket/ParvicursorSocket.h"
#include "../../Parvicursor/Parvicursor/xDFS/xDFSClients/UploadClient/UploadClient.h"

//---------------------------------------
using namespace System;
using namespace System::Threading;
using namespace System::Collections;

using namespace Parvicursor::Net;
using namespace Parvicursor::xThread;
using namespace Parvicursor::Serialization;
using namespace Parvicursor::xDFS::xDFSClients;

//---------------------------------------
enum xThreadThirdPartyTransferCommands
{
	__GetTransferredBytes__ = (Byte)5,
};
struct xThreadThirdPartyTransferContextInput
{
	xThreadThirdPartyTransferCommands command;
	Int32 pad1;
	Int32 pad2;
};

struct xThreadThirdPartyTransferContextOutput
{
	Int64 transferredBytes;
};
//---------------------------------------
class xThreadThirdPartyTransfer_API xThreadThirdPartyTransfer : public xThreadBase, public Object
{
	public:
		char *dest;
		char *localFilename;
		char *remoteFilename;
		ArrayList *remoteErrors;

	private:
		bool isFirstTimeInitilization;
		bool hasRemoteErrors;
		UploadClient *upc;
		NetworkCredential *nc;
		Int32 parallel;
		Int32 blockSize;
		Int32 tcpBufferSize;
		xThreadThirdPartyTransferContextOutput *out;
		Mutex *mutex;

	public:
		xThreadThirdPartyTransfer(const char *Dest, const char *LocalFilename, const char *RemoteFilename, Int32 parallel, Int32 blockSize, Int32 tcpBufferSize);
		xThreadThirdPartyTransfer();
		~xThreadThirdPartyTransfer();
		void Start();
		void AuxiliaryMehtod(In const void *input, Int32 inputLength, Out void **output, Out Int32 *outputLength);
		void Serialize(char **DllBuffer, Int32 *len);
		void DeSerialize(char **buffer, Int32 *len);
		void FreeDllMemory(void *mem);
		void *AllocateDllMemory(UInt32 size);
		void Deconstructor(xThreadBase *obj);
		void ReleaseCriticalSesctions();
};
//---------------------------------------

#endif
