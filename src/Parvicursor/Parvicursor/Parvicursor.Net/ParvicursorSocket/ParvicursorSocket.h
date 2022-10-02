/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __Parvicursor_Net_ParvicursorSocket_h__
#define __Parvicursor_Net_ParvicursorSocket_h__

#include "../../../System/Object/Object.h"
#include "../../../System/BasicTypes/BasicTypes.h"

#ifdef ParvicursorLib_inlining
//#warning from
//#include "../../../System.Net.Sockets/Socket/Socket_inlined.h"
#define __Parvicursor_inline__ inline
#endif

#include "../../../System.Net.Sockets/Socket/Socket.h"
#include "../../../System/Exception/Exception.h"
#include "../../../System.IO/FileStream/FileStream.h"
#include "../../../System.Threading/Mutex/Mutex.h"

#ifdef ParvicursorLib_inlining
#undef __Parvicursor_inline__
#define __Parvicursor_inline__
#endif


#if !defined ParvicursorLib_inlining
//#warning hello world
#ifndef __Parvicursor_inline__
#define __Parvicursor_inline__ inline
#endif
#else
#ifndef __Parvicursor_inline__
#define __Parvicursor_inline__
//#warning hello
#endif
#endif

#include "../../../Parvicursor/Parvicursor.Cryptography/AesEngine/AesEngine.h"
#include "../../../Parvicursor/Parvicursor.Cryptography/ShaEngine/ShaEngine.h"
#include "../../../Parvicursor/Parvicursor.Cryptography/RsaEngine/RsaEngine.h"


//**************************************************************************************************************//

using namespace System;
using namespace System::IO;
using namespace System::Net::Sockets;
using namespace System::Threading;

using namespace Parvicursor::Cryptography;

namespace Parvicursor
{
	//----------------------------------------------------
   namespace Net
   {
	    enum CheckedExceptionResponseState
		{
			Done = 0,
			NotDone = 1,
			FirstTime = 2
		};
		//----------------------------------------------------
		class ParvicursorSocket : public Object
		{
			private: AesEngine *cryptography_engine;
			private: RsaParameters params;
			private: char *cryptography_buffer;
			private: char *read_buffer;
			private: char *write_buffer;
			private: bool read_buffer_is_dirty;
			private: bool write_buffer_is_dirty;
			private: Int32 write_buffer_next_index;
			private: Int32 read_buffer_start;
			private: Int32 read_buffer_end;
			private: Int32 buffer_size;
			//private: char *key, *iv;
			public: static const Int32 DefaultCryptographyBufferSize = 64 * 1024;
			public: static const Int32 DefaultRsaKeySize = 1024;
			private: Int32 MaximumAllocatedRsaBufferSize;
			public: static const Int32 DefaultSymetricEngineKeyLength = AesEngine::DefaultKeyLength;
			public: static const Int32 DefaultSymetricEngineBlockLength = AesEngine::DefaultBlockSize;
			public: static const Int32 DefaultHashEngineDigestLength = ShaEngine::Sha2DigestSize256;
			private: bool isSecure;
			private: static Mutex *mutex;
			private: static RsaEngine rsaEngine;
			private: static bool rsaEngineHasBeenInitialized;
			private: bool xSecChannelSetup_has_called;
			private: Socket *socket;// = null;
			private: Byte buff[4]; //consider for 64 bit operating systems buffer[8]
			private: char _buff[4];
			protected: private: bool disposed;
			protected: private: CheckedExceptionResponseState IsCheckedExceptionResponse;
			//private: SecureBinaryReader reader;
			//private: SecureBinaryWriter writer;
			//private: Int32 tcpBufferSize;// = 256 * 1024;// 64KB  for none secure and 32KB for secure connections based on DotSec model.
			//----------------------------------------------------
			// Use this constrctor for non-secure data trasnfers in client side. In server side, the Parvicursor Platform will decide about whether the channel must be secured or not, this will be requested by client.
			public: ParvicursorSocket(Socket *socket);
			// Use this constrctor to force the undelaying channels to be secured over xSec protocol. This constructore can only be used on connected sockets, otherwise an excpetion will be thrown.
			public: ParvicursorSocket(Socket *socket, Int32 CryptographyBufferSize/* = ParvicursorSocket::DefaultCryptographyBufferSize*/);
			public: ~ParvicursorSocket();
			public: __Parvicursor_inline__ bool get_IsSecure();
			public: __Parvicursor_inline__ Socket *get_BaseSocket();
			public: __Parvicursor_inline__ void WriteException(System::Exception &e);
			public: __Parvicursor_inline__ void WriteNoException();
			public: __Parvicursor_inline__ void CheckExceptionResponse();
			public: void Close();
			public: __Parvicursor_inline__ char *ReadObject(Int32 &objSize);
			public: __Parvicursor_inline__ void ReadObject(char *buffer, Int32 currentSize, Int32 &readSize, Int32 &newSize);
			public: __Parvicursor_inline__ String ReadString();
			//public: void WriteObject(object obj)
			public: __Parvicursor_inline__ void WriteObject(const char obj[], Int32 size);// private: char buff[4]; consider for 64 bit operating systems buffer[8] and size
			public: __Parvicursor_inline__ void WriteString(const String &str);
			public: __Parvicursor_inline__ Int32 Read(char array[], Int32 offset, Int32 count);
			public: __Parvicursor_inline__ char *Read(Int32 size);
			public: __Parvicursor_inline__ Byte ReadByte();
			public: __Parvicursor_inline__ Int32 Write(const char array[], Int32 offset, Int32 count);
			public: __Parvicursor_inline__ Int32 Write(const char array[], Int32 count);
			public: __Parvicursor_inline__ void WriteByte(Byte buffer);
			public: __Parvicursor_inline__ CheckedExceptionResponseState get_IsCheckedExceptionResponse();
			public: __Parvicursor_inline__ void set_IsCheckedExceptionResponse(CheckedExceptionResponseState value);
			// Use this method in secure mode via xSec enabled to enusre flushing the internal write buffer after Write() operations.
			public: __Parvicursor_inline__ void Flush();

			private: inline Int32 RefillReadBuffer(char *array, Int32 offset, Int32 len);
			private: inline void WriteInternal(const char array[], Int32 offset, Int32 count);
			private: inline Int32 ReadAuxiliary(char array[], Int32 offset, Int32 count);
			private: inline Int32 WriteAuxiliary(const char array[], Int32 offset, Int32 count);
			private: inline void FlushWriteBuffer();

			private: void xSecChannelSetup();
			private: inline void xSecSelectChannelModeNegotiation();
			private: inline void xSecOneWayVerificationProtocol();
			private: inline void DummyKeySetp();
			//----------------------------------------------------
		};
		//----------------------------------------------------
	}
}
//**************************************************************************************************************//


#endif

