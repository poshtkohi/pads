/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __Parvicursor_xDFS_xDFSClients_FileStreamClient_h__
#define __Parvicursor_xDFS_xDFSClients_FileStreamClient_h__


#include "../../../../general.h"
#include "../../../../System/BasicTypes/BasicTypes.h"
#include "../../../../System/Object/Object.h"
#include "../../../../StaticFunctions/StaticFunctions.h"
#include "../../../../../Parvicursor/System/String/String.h"
#include "../../../../Parvicursor/Parvicursor.Net/ParvicursorSocket/ParvicursorSocket.h"
#include "../../../../../Parvicursor/System/ObjectDisposedException/ObjectDisposedException.h"
#include "../../../../../Parvicursor/System.IO/IOException/IOException.h"
#include "../../../../Parvicursor/Shared/Enums/xDFS/TransferChannelMode.h"
#include "../../../../Parvicursor/Shared/Enums/xDFS/Methods.h"
#include "../../../../Parvicursor/Serialization/Serializer.h"
#include "../../../../Parvicursor/Serialization/DeSerializer.h"
#include "../../../../System/ArgumentException/ArgumentNullException.h"
#include "../../../../System.Collections/ArrayList/ArrayList.h"
#include "../../../../System.IO/FileStream/FileStream.h"
#include "../../../../System.Net.Sockets/Socket/Socket.h"
#include "../../../../System.Net/Dns/Dns.h"
#include "../../../../System.Net/NetworkCredential/NetworkCredential.h"
#include "../../../../System/Guid/Guid.h"

//**************************************************************************************************************//
using namespace System;
using namespace System::Collections;
using namespace System::IO;
using namespace System::Net;
using namespace System::Net::Sockets;

using namespace Parvicursor::Net;
using namespace Parvicursor::Serialization;
using namespace Parvicursor::Shared::Enums::xDFS;


namespace Parvicursor
{
   namespace xDFS
   {
	   namespace xDFSClients
	   {
			class FileStreamClient : public Object
			{
				/*---------------------fields----------------*/
				private: String remoteFilename;
				private: FileMode mode;
				private: FileAccess access;
				private: Int32 blockSize;
				private: String xDFSServerAddress;
				private: NetworkCredential *nc;
				private: bool secure;
				private: String guid;
				//private: IPHostEntry *hostEntry;
				private: ParvicursorSocket *socket;
				private: char *paramBuffer;
				private: bool isConnected;
				private: bool disposed;
				/*---------------------methods----------------*/
				/// <summary>
				/// Initializes a new instance of the FileStream class with the specified path, creation mode, read/write permission, and sharing permission.
				/// </summary>
				/// <param name="remoteFilename">A remote relative or absolute path for the file that the current FileStream object will encapsulate.</param>
				/// <param name="mode">A FileMode constant that determines how to open or create the file.</param>
				/// <param name="access">A FileAccess constant that determines how the file can be accessed by the FileStream object.</param>
				/// <param name="share">A FileShare constant that determines how the file will be shared by processes.</param>
				/// <param name="encoding">A PathEncoding constant that determines how the path file will be encoded by processes.</param>
				/// <param name="xDFSServerAddress">xDFSServer Address.</param>
				/// <param name="nc">Provides credentials for password-based authentication schemes to destination xDFS server.</param>
				/// <param name="Secure">Determines secure or secureless connection based on Parvicursor.DotSec transfer layer security.</param>
				public: FileStreamClient(const String &remoteFilename, FileMode mode, FileAccess access, /*FileShare share,*/ Int32 blockSize, const String &xDFSServerAddress, const NetworkCredential &nc, bool secure);
				public: ~FileStreamClient();
				/// <summary>
				/// Reads a block of bytes from the stream and writes the data in a given buffer.
				/// </summary>
				/// <param name="array">When this method returns, contains the specified byte array with the values between offset and (offset + count - 1) replaced by the bytes read from the current source.</param>
				/// <param name="offset">The byte offset in array at which to begin reading.</param>
				/// <param name="count">The maximum number of bytes to read.</param>
				/// <returns>The total number of bytes read into the buffer. This might be less than the number of bytes requested if that number of bytes are not currently available, or zero if the end of the stream is reached.</returns>
				public: Int32 Read(char array[], Int32 offset, Int32 count);
				private: inline Int32 ReadInternal(char array[], Int32 offset, Int32 count);
				/// <summary>
				/// Writes a block of bytes to this stream using data from a buffer.
				/// </summary>
				/// <param name="array">The array to which bytes are written.</param>
				/// <param name="offset">The byte offset in array at which to begin writing.</param>
				/// <param name="count">The maximum number of bytes to write.</param>
				/// <returns></returns>
				public: void Write(const char array[], Int32 offset, Int32 count);
				private: inline void WriteInternal(const char array[], Int32 offset, Int32 count);
				/// <summary>
				/// Clears all buffers for this stream and causes any buffered data to be written to the underlying device.
				/// </summary>
				public: void Flush();
				/// <summary>
				/// Closes the file and releases any resources associated with the current file stream.
				/// </summary>
				public: void Close();
				/// <summary>
				/// Sets the length of this stream to the given value.
				/// </summary>
				/// <param name="value">The new length of the stream.</param>
				public: void SetLength(Int64 value);
				/// <summary>
				/// Prevents other processes from changing the FileStream while permitting read access.
				/// </summary>
				/// <param name="position">The beginning of the range to unlock.</param>
				/// <param name="length">The range to be unlocked.</param>
				public: void Lock(Int64 position, Int64 length);
				/// <summary>
				/// Allows access by other processes to all or part of a file that was previously locked.
				/// </summary>
				/// <param name="position">The beginning of the range to lock. The value of this parameter must be equal to or greater than zero (0).</param>
				/// <param name="length">The range to be locked.</param>
				public: void UnLock(Int64 position, Int64 length);
				/// <summary>
				/// Sets the current position of this stream to the given value.
				/// </summary>
				/// <param name="offset">The point relative to origin from which to begin seeking.</param>
				/// <param name="origin">Specifies the beginning, the end, or the current position as a reference point for origin, using a value of type SeekOrigin.</param>
				public: Int64 Seek(Int64 offset, SeekOrigin origin);
				/// <summary>
				/// Gets or sets the current position of this stream.
				/// </summary>
				public: Int64 get_Position();
				/// <summary>
				/// Gets the length in bytes of the stream.
				/// </summary>
				public: Int64 get_Length();
				/// <summary>
				/// Gets a value indicating whether the current stream supports reading.
				/// </summary>
				public: bool get_CanRead();
				/// <summary>
				/// Gets a value indicating whether the current stream supports writing.
				/// </summary>
				public: bool get_CanWrite();
				/// <summary>
				/// Gets a value indicating whether the current stream supports seeking.
				/// </summary>
				public: bool get_CanSeek();
				/// <summary>
				/// Gets the name of the FileStream that was passed to the constructor.
				/// </summary>
				public: const String &get_Name();
				private: void ConnectToServer();
			};
	   };
   };
};
//**************************************************************************************************************//

#endif
