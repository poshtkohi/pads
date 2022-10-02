/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "ParvicursorSocket.h"

//**************************************************************************************************************//
namespace Parvicursor
{
   namespace Net
   {
	    //----------------------------------------------------
	    ParvicursorSocket::ParvicursorSocket(Socket *socket)
		{
			if(socket == null)
				throw ArgumentNullException("socket can not be null");

			if(socket->get_IsBound())
				throw InvalidOperationException("The socket is bound, you can not use it with ParvicursorSocket class");

			this->socket = socket;
			isSecure = false;
			IsCheckedExceptionResponse = FirstTime;
			xSecChannelSetup_has_called = false;
			MaximumAllocatedRsaBufferSize = ParvicursorSocket::DefaultSymetricEngineKeyLength + ParvicursorSocket::DefaultSymetricEngineBlockLength + ParvicursorSocket::DefaultHashEngineDigestLength + ParvicursorSocket::DefaultRsaKeySize + 128*1024;


			/*buffer_size = DefaultCryptographyBufferSize;
			// data length + data + hash
			cryptography_buffer = new char[sizeof(Int32) + buffer_size + ShaEngine::Sha2DigestSize256 + 1];
			read_buffer = new char[buffer_size];
			write_buffer = new char[buffer_size];
			read_buffer_is_dirty = false;
			write_buffer_is_dirty = false;
			write_buffer_next_index = 0;
			read_buffer_start = 0;
			read_buffer_end = 0;
			isSecure = true;//
			DummyKeySetp();*/
			//rsaEngine = null;

			disposed = false;
		}
		//----------------------------------------------------
		ParvicursorSocket::ParvicursorSocket(Socket *socket, Int32 CryptographyBufferSize)
		{
			if(socket == null)
				throw ArgumentNullException("socket can not be null");

			if(socket->get_IsBound())
				throw InvalidOperationException("The socket is bound, you can not use it with ParvicursorSocket class");

			if(socket->get_IsAccepted())
				throw InvalidOperationException("The socket is accepted in server side, this constructor can not directly be used for xSec secure data transfers. Use the constructor ParvicursorSocket(Socket *socket), the Parvicursor Framework will setup the secure channel if possibly requested by client");

			if(CryptographyBufferSize <= 0)
				throw ArgumentOutOfRangeException("CryptographyBufferSize", "CryptographyBufferSize must be greater that zero");
			this->socket = socket;

			buffer_size = CryptographyBufferSize;

			// data length + data + hash
			cryptography_buffer = new char[sizeof(Int32) + buffer_size + ShaEngine::Sha2DigestSize256 + 1];
			read_buffer = new char[buffer_size];
			write_buffer = new char[buffer_size];
			read_buffer_is_dirty = false;
			write_buffer_is_dirty = false;
			write_buffer_next_index = 0;
			read_buffer_start = 0;
			read_buffer_end = 0;
			isSecure = true;//
			IsCheckedExceptionResponse = FirstTime;
			xSecChannelSetup_has_called = false;
			MaximumAllocatedRsaBufferSize = ParvicursorSocket::DefaultSymetricEngineKeyLength + ParvicursorSocket::DefaultSymetricEngineBlockLength + ParvicursorSocket::DefaultHashEngineDigestLength + ParvicursorSocket::DefaultRsaKeySize + 128*1024;
			disposed = false;

			// Generate the shared session key in the client.
			CryptographyKey key = AesEngine::GenerateKey(ParvicursorSocket::DefaultSymetricEngineKeyLength);
			Byte *iv = new Byte[ParvicursorSocket::DefaultSymetricEngineBlockLength + 1];
			try { AesEngine::GenerateIV(iv, ParvicursorSocket::DefaultSymetricEngineBlockLength); }
			catch(System::Exception &e) { delete iv; throw System::Exception(e.get_Message()); }
			catch(...) { delete iv; throw System::Exception("Unknown execption was occured in invoking Parvicursor::Cryptography::AesEngine::GenerateIV()");  }
			cryptography_engine = new AesEngine(key, (const Byte *)iv, ParvicursorSocket::DefaultSymetricEngineBlockLength);
			delete iv;

			//rsaEngine = null;
			//DummyKeySetp();
		}
		//----------------------------------------------------
		void ParvicursorSocket::DummyKeySetp()
		{
			FileStream *fsKey  = new FileStream("C:/key/key.dat", System::IO::Open, System::IO::Read, 9);
			FileStream *fsIV  = new FileStream("C:/key/iv.dat", System::IO::Open, System::IO::Read, 9);

			Int32 keyLength = (Int32)fsKey->get_Length();
			Int32 ivLength = (Int32)fsIV->get_Length();
			Byte *key = new Byte[keyLength];
			Byte *iv = new Byte[ivLength];
			printf("keyLength: %d, ivLength: %d\n", keyLength, ivLength);

			fsKey->Read((char *)key, 0, keyLength);
			fsIV->Read((char *)iv, 0, ivLength);
			CryptographyKey ck = CryptographyKey((const Byte *)key, keyLength);
			cryptography_engine = new AesEngine(ck, (const Byte *)iv, ivLength);

			delete key; delete iv;
		}
		//----------------------------------------------------
		void ParvicursorSocket::xSecChannelSetup()
		{
			if(!xSecChannelSetup_has_called)
			{
				xSecChannelSetup_has_called = true;
				xSecSelectChannelModeNegotiation();
				if(isSecure)
				{
					xSecOneWayVerificationProtocol();
				}
			}
		}
		//----------------------------------------------------
		static void BufferPrint(const char *name, const char *buffer, Int32 size)
		{
			printf("\n%s: ", name);
			for(Int32 i  = 0 ; i < size ; i++)
				printf("%d", (Byte)buffer[i]);
			printf("\n");
		}
		void ParvicursorSocket::xSecSelectChannelModeNegotiation()
		{
			// Implements the xSec server-side xSecSelectChannelModeNegotiation protocol.
			if(socket->get_IsAccepted())
			{
				ReadAuxiliary(_buff, 0, sizeof(char));

				if(_buff[0] == 1) // Secure channel request
				{
					ParvicursorSocket::mutex->Lock();
					if(!ParvicursorSocket::rsaEngineHasBeenInitialized)
					{
						RsaParameters params = RsaEngine::GenerateRsaParameters(DefaultRsaKeySize);
						Int32 n_size = 0;
						const char *n = params.get_n(n_size);
						Int32 e_size = 0;
						const char *e = params.get_e(e_size);
						Int32 d_size = 0;
						const char *d = params.get_d(d_size);
						RsaParameters _params = RsaParameters(n, n_size, e, e_size, d, d_size);
						ParvicursorSocket::rsaEngine = RsaEngine(_params);
						ParvicursorSocket::rsaEngineHasBeenInitialized = true;
					}
					ParvicursorSocket::mutex->Unlock();

					isSecure = true;
					return ;
				}
				else if(_buff[0] == 0) // Non-secure channel request
				{
					_buff[0] = 1; // channel established
					WriteAuxiliary(_buff, 0, sizeof(char));
					isSecure = false;
					return ;
				}
				else
					throw ArgumentException("The received xSecInitialSelectChannelMode is not valid");
			}

			// Implements the xSec client-side xSecSelectChannelModeNegotiation protocol.
			else if(socket->get_Connected())
			{
				if(isSecure)
					_buff[0] = 1;
				else
					_buff[0] = 0;

				WriteAuxiliary(_buff, 0, sizeof(char));
				if(!isSecure)
				{
					ReadAuxiliary(_buff, 0, sizeof(char));

					if(_buff[0] != 1)
						throw System::Exception("The channel could not be estblished in the stage of xSecSelectChannelModeNegotiation");

				}

				return ;
			}
		}
		//----------------------------------------------------
		void ParvicursorSocket::xSecOneWayVerificationProtocol()
		{
			// Implements the xSec server-side one-way verification protocol.
			if(socket->get_IsAccepted())
			{
				// Sends public key (possibly with the server certificate)
				RsaParameters params = ParvicursorSocket::rsaEngine.get_RsaParameters();
				Int32 n_size = 0;
				const char *n = params.get_n(n_size);
				Int32 e_size = 0;
				const char *e = params.get_e(e_size);
				Parvicursor_GetBytesFromInt32Number((char *)buff, n_size);
				WriteAuxiliary((const char *)buff, 0, sizeof(Int32));
				WriteAuxiliary(n, 0, n_size);
				Parvicursor_GetBytesFromInt32Number((char *)buff, e_size);
				WriteAuxiliary((const char *)buff, 0, sizeof(Int32));
				WriteAuxiliary(e, 0, e_size);

				// Receives the shared key, iv, and hash(key + iv) encrypted with the server public key.
				ReadAuxiliary((char *)buff, 0, sizeof(Int32));
				Int32 keyIvHash_size = Parvicursor_GetInt32NumberFromBytes(buff);
				if(keyIvHash_size > MaximumAllocatedRsaBufferSize)
					throw ArgumentOutOfRangeException("The received size of the RSA packet is out of the range specified by the xSec protocol's MaximumAllocatedRsaBufferSize");
				char *keyIvHash_encrypted = new char[keyIvHash_size];
				char *keyIvHash = null;
				try
				{
					ReadAuxiliary(keyIvHash_encrypted, 0, keyIvHash_size);
					ParvicursorSocket::rsaEngine.Decrypt(keyIvHash_encrypted, &keyIvHash, keyIvHash_size);
					delete keyIvHash_encrypted; keyIvHash_encrypted = null;
					if(keyIvHash_size != ParvicursorSocket::DefaultSymetricEngineKeyLength + ParvicursorSocket::DefaultSymetricEngineBlockLength + ParvicursorSocket::DefaultHashEngineDigestLength)
						throw ArgumentOutOfRangeException("The size of the decrypted RSA ciphertext is not in the range specified by xSec protocol with the size of ParvicursorSocket::DefaultSymetricEngineKeyLength + ParvicursorSocket::DefaultSymetricEngineBlockLength + ParvicursorSocket::DefaultHashEngineDigestLength");
					keyIvHash_size = ParvicursorSocket::DefaultSymetricEngineKeyLength + ParvicursorSocket::DefaultSymetricEngineBlockLength + ParvicursorSocket::DefaultHashEngineDigestLength;
					char *key = keyIvHash;
					char *iv = key + ParvicursorSocket::DefaultSymetricEngineKeyLength;
					char *digest = iv + ParvicursorSocket::DefaultSymetricEngineBlockLength;
					if(!ShaEngine::Sha2VerifyDigest256(keyIvHash, ParvicursorSocket::DefaultSymetricEngineKeyLength + ParvicursorSocket::DefaultSymetricEngineBlockLength, digest))
						throw System::Exception("The received data via xSec protocol is not verified");
					CryptographyKey _key = CryptographyKey((const Byte *)key, ParvicursorSocket::DefaultSymetricEngineKeyLength);
					cryptography_engine = new AesEngine(_key, (const Byte *)iv, ParvicursorSocket::DefaultSymetricEngineBlockLength);
					delete keyIvHash; keyIvHash = null;
					buffer_size = ParvicursorSocket::DefaultCryptographyBufferSize;// Adjust this part for TCP Window Size support.
					// data length + data + hash
					cryptography_buffer = new char[sizeof(Int32) + buffer_size + ShaEngine::Sha2DigestSize256 + 1];
					read_buffer = new char[buffer_size];
					write_buffer = new char[buffer_size];
					read_buffer_is_dirty = false;
					write_buffer_is_dirty = false;
					write_buffer_next_index = 0;
					read_buffer_start = 0;
					read_buffer_end = 0;
					_buff[0] = 1; // channel established
					WriteAuxiliary(_buff, 0, sizeof(char));
					return ;
				}
				catch(System::Exception &ex)
				{
					if(keyIvHash_encrypted != null)
						delete keyIvHash_encrypted;
					if(keyIvHash != null)
						delete keyIvHash;

					throw System::Exception(ex.get_Message());
				}
				catch(...)
				{
					if(keyIvHash_encrypted != null)
						delete keyIvHash_encrypted;
					if(keyIvHash != null)
						delete keyIvHash;

					throw System::Exception("Unknown exception was occured within ParvicursorSocket::xSecOneWayVerificationProtocol()");
				}
			}

			// Implements the xSec client-side one-way verification protocol.
			if(socket->get_Connected())
			{
				// Receives public key (n, e), possibly with the server certificate)
				RsaEngine *rsa_engine = null;
				Int32 n_size = 0;
				Int32 e_size = 0;
				char *n = null;
				char *e = null;
				try
				{
					ReadAuxiliary((char *)buff, 0, sizeof(Int32));
					n_size = Parvicursor_GetInt32NumberFromBytes(buff);
					if(n_size <= 0 || n_size > ParvicursorSocket::DefaultRsaKeySize)
						throw ArgumentOutOfRangeException("The received size of the RSA's n parameter is out of the range specified by xSec protocol");
					n = new char[n_size];
					ReadAuxiliary(n, 0, n_size);
					ReadAuxiliary((char *)buff, 0, sizeof(Int32));
					e_size = Parvicursor_GetInt32NumberFromBytes(buff);
					if(e_size <= 0 || e_size > ParvicursorSocket::DefaultRsaKeySize)
						throw ArgumentOutOfRangeException("The received size of the RSA's e parameter is out of the range specified by xSec protocol");
					e = new char[e_size];
					ReadAuxiliary(e, 0, e_size);
					//RsaParameters params = RsaParameters((const char *)n, n_size, (const char *)e, e_size);
					params = RsaParameters((const char *)n, n_size, (const char *)e, e_size);
					rsa_engine = new RsaEngine(params);
					delete n; delete e; n = null; e = null;
				}
				catch(System::Exception &e1)
				{
					if(n != null)
						delete n;
					if(e != null)
						delete e;
					if(rsa_engine != null)
						delete rsa_engine;

					throw System::Exception(e1.get_Message());
				}
				catch(...)
				{
					if(n != null)
						delete n;
					if(e != null)
						delete e;
					if(rsa_engine != null)
						delete rsa_engine;

					throw System::Exception("Unknown exception was occured within ParvicursorSocket::xSecOneWayVerificationProtocol()");
				}
				// Send the shared key, iv, and hash(key + iv) encrypted with the server public key.
				char *keyIvHash = new char[ParvicursorSocket::DefaultSymetricEngineKeyLength + ParvicursorSocket::DefaultSymetricEngineBlockLength + ParvicursorSocket::DefaultHashEngineDigestLength];
				char *key = keyIvHash;
				char *iv = key + ParvicursorSocket::DefaultSymetricEngineKeyLength;
				char *digest = iv + ParvicursorSocket::DefaultSymetricEngineBlockLength;
				memcpy(key, cryptography_engine->get_Key().get_Key(), ParvicursorSocket::DefaultSymetricEngineKeyLength);
				memcpy(iv, cryptography_engine->get_IV(), ParvicursorSocket::DefaultSymetricEngineBlockLength);
				char *keyIvHash_encrypted = null;
				Int32 keyIvHash_size = ParvicursorSocket::DefaultSymetricEngineKeyLength + ParvicursorSocket::DefaultSymetricEngineBlockLength + ParvicursorSocket::DefaultHashEngineDigestLength;
				try
				{
					ShaEngine::Sha2ComputeHash256(keyIvHash, ParvicursorSocket::DefaultSymetricEngineKeyLength + ParvicursorSocket::DefaultSymetricEngineBlockLength, digest);
					rsa_engine->Encrypt(keyIvHash, &keyIvHash_encrypted, keyIvHash_size);
					delete rsa_engine; rsa_engine = null;
					Parvicursor_GetBytesFromInt32Number((char *)buff, keyIvHash_size);
					WriteAuxiliary((const char *)buff, 0, sizeof(Int32));
					WriteAuxiliary(keyIvHash_encrypted, 0, keyIvHash_size);
					delete keyIvHash; keyIvHash = null;
					delete keyIvHash_encrypted; keyIvHash_encrypted = null;
					ReadAuxiliary(_buff, 0, sizeof(char));
					if(_buff[0] != 1)
						throw System::Exception("The xSec channel could not be estblished in the stage of xSecOneWayVerificationProtocol");

					return;
				}
				catch(System::Exception &e2)
				{
					if(rsa_engine != null)
						delete rsa_engine;
					if(keyIvHash != null)
						delete keyIvHash;
					if(keyIvHash_encrypted != null)
						delete keyIvHash_encrypted;

					throw System::Exception(e2.get_Message());
				}
				catch(...)
				{
					if(rsa_engine != null)
						delete rsa_engine;
					if(keyIvHash != null)
						delete keyIvHash;
					if(keyIvHash_encrypted != null)
						delete keyIvHash_encrypted;

					throw System::Exception("Unknown exception was occured within ParvicursorSocket::xSecOneWayVerificationProtocol()");
				}
			}
		}
		//----------------------------------------------------
		ParvicursorSocket::~ParvicursorSocket()
		{
			if(!disposed)
				Close();

			/*if(rsaEngine != null)
			{
				delete rsaEngine;
				rsaEngine = null;
			}*/
		}
		//----------------------------------------------------
		CheckedExceptionResponseState ParvicursorSocket::get_IsCheckedExceptionResponse()
		{
			return IsCheckedExceptionResponse;
		}
		//----------------------------------------------------
		void ParvicursorSocket::set_IsCheckedExceptionResponse(CheckedExceptionResponseState value)
		{
			IsCheckedExceptionResponse = value;
		}
		//----------------------------------------------------
		bool ParvicursorSocket::get_IsSecure()
		{
			if(disposed)
				throw ObjectDisposedException("Could not access to a closed object");

			return isSecure;
		}
		//----------------------------------------------------
		Socket *ParvicursorSocket::get_BaseSocket()
		{
			if(disposed)
				throw ObjectDisposedException("Could not access to a closed object");

			return socket;
		}
		//----------------------------------------------------
		void ParvicursorSocket::WriteException(System::Exception &e)
		{
			if(disposed)
				throw ObjectDisposedException("Could not access to a closed object");

			if(!xSecChannelSetup_has_called) xSecChannelSetup();

			WriteByte((Byte)1);
			WriteString(e.get_Message());
			//cout << "WriteException: " << e.get_Message().get_BaseStream() << "\n\n" << endl;//
		}
		//----------------------------------------------------
		void ParvicursorSocket::WriteNoException()
		{
			if(disposed)
				throw ObjectDisposedException("Could not access to a closed object");

			if(!xSecChannelSetup_has_called) xSecChannelSetup();

			WriteByte((Byte)0);
		}
		//----------------------------------------------------
		void ParvicursorSocket::CheckExceptionResponse()
		{
			if(disposed)
				throw ObjectDisposedException("Could not access to a closed object");

			if(!xSecChannelSetup_has_called) xSecChannelSetup();

			if(ReadByte() == (Byte)0)
				return;
			else
			{
				String e = ReadString();
				throw System::Exception(e);
			}
		}
		//----------------------------------------------------
		void ParvicursorSocket::Close()
		{
            if(disposed)
				throw ObjectDisposedException("Could not access to a closed object");

			//if(!xSecChannelSetup_has_called) xSecChannelSetup();

			if(isSecure)
			{
				if(xSecChannelSetup_has_called)
					FlushWriteBuffer();

				delete cryptography_buffer;
				cryptography_buffer = null;

				delete read_buffer;
				read_buffer = null;

				delete write_buffer;
				write_buffer = null;

				delete cryptography_engine;
				cryptography_engine = null;
			}

			/*if(!this->disposed)
			{*/
				if(socket != null)
				{
					try { socket->Shutdown(System::Net::Sockets::Both); } catch(...) { }
					try { socket->Close(); } catch(...) {}
					delete socket;
					socket = null;
				}
				disposed = true;
			//}
		}
		//----------------------------------------------------
		void ParvicursorSocket::WriteObject(const char obj[], Int32 size)// private: char buff[4]; consider for 64 bit operating systems buffer[8] and size
		{
			if(!xSecChannelSetup_has_called) xSecChannelSetup();

			Parvicursor_GetBytesFromInt32Number((char *)buff, size);

			Write((char *)buff, 0, sizeof(Int32));
			Write(obj, 0, size);
		}
		//----------------------------------------------------
		void ParvicursorSocket::WriteString(const String &str)
		{
			if(!xSecChannelSetup_has_called) xSecChannelSetup();

			WriteObject(str.get_BaseStream(), str.get_Length() + 1);
		}
		//----------------------------------------------------
		char* ParvicursorSocket::ReadObject(Int32 &objSize) // private: char buff[4]; consider for 64 bit operating systems buffer[8] and size
		{
			if(!xSecChannelSetup_has_called) xSecChannelSetup();

			Read((char *)buff, 0, sizeof(Int32));
			objSize = Parvicursor_GetInt32NumberFromBytes(buff); // Object Length

			//cout << "objSize: " << objSize << endl;

			char *obj = new char[objSize];
			Read(obj, 0, objSize);
			return obj;
		}
		//----------------------------------------------------
		void ParvicursorSocket::ReadObject(char *buffer, Int32 currentSize, Int32 &readSize, Int32 &newSize)// private: char buff[4]; consider for 64 bit operating systems buffer[8] and size
		{
			if(!xSecChannelSetup_has_called) xSecChannelSetup();

			this->Read((char *)buff, 0, sizeof(Int32));
			readSize = Parvicursor_GetInt32NumberFromBytes(buff); // Object Length

			//cout << "objSize: " << objSize << endl;

			if(readSize > currentSize)
			{
				delete buffer;
				buffer = new char[readSize];
				newSize = readSize;
			}
			else
				newSize = currentSize;

			Read(buffer, 0, readSize);
			return ;
		}
		//----------------------------------------------------
		String ParvicursorSocket::ReadString()
		{
			if(!xSecChannelSetup_has_called) xSecChannelSetup();

			Int32 len = 0;
			char *str= ReadObject(len);
			return String(str);
		}
		//----------------------------------------------------
		Int32 ParvicursorSocket::Read(char array[], Int32 offset, Int32 count)
		{
			if(this->disposed)
				throw ObjectDisposedException("Could not access to a closed object");

			if(array == null)
				throw ArgumentNullException("array", "array can not be null");

			if(offset < 0)
				throw ArgumentOutOfRangeException("offset", "offset is less than 0");

			if(count <= 0)
				throw ArgumentOutOfRangeException("count", "count must be greater than 0");

			if(!xSecChannelSetup_has_called) xSecChannelSetup();

			if(!isSecure)
				return ReadAuxiliary(array, offset, count);
			else
			{
				Int32 total = 0;
				FlushWriteBuffer();

				char *_array = (char *)array;
				Int32 arrayIndex = offset;
				_array += arrayIndex;
				if(read_buffer_is_dirty)
				{
					Int32 available = read_buffer_end - read_buffer_start + 1;
					read_buffer += read_buffer_start;
					if(count == available)
					{
						memcpy(_array, read_buffer, available);
						_array -= arrayIndex;
						read_buffer -= read_buffer_start;
						read_buffer_start = read_buffer_end = 0;
						read_buffer_is_dirty = false;
						return count;
					}
					else if(count < available)
					{
						memcpy(_array, read_buffer, count);
						_array -= arrayIndex;
						read_buffer -= read_buffer_start;
						read_buffer_start += count;
						return count;
					}
					else if(count > available)
					{
						memcpy(_array, read_buffer, available);
						_array -= arrayIndex;
						read_buffer -= read_buffer_start;
						read_buffer_start = read_buffer_end = 0;
						read_buffer_is_dirty = false;
						total += available;
						count -= available;
						offset += available;//
					}
				}
				Int32 ret = 0;
				if((ret = RefillReadBuffer(array, offset, count)) > 0 )
					return total + ret;
				else
					return total + Read(array, offset, count);
			}
		}
		//----------------------------------------------------
		Int32 ParvicursorSocket::RefillReadBuffer(char *array, Int32 offset, Int32 len)
		{
			char *length = cryptography_buffer;
			ReadAuxiliary(length, 0, sizeof(Int32));
			Int32 count = Parvicursor_GetInt32NumberFromBytes((Byte *)length);
			if(count <= 0 || count > buffer_size)
				throw ArgumentOutOfRangeException("count", "The received data length is invalid");

			char *ciphertext = (char *)(sizeof(Int32) + cryptography_buffer);
			char *digest = (char *)(sizeof(Int32)  + count + cryptography_buffer);
			ReadAuxiliary(ciphertext, 0, count + ShaEngine::Sha2DigestSize256); // Reads ciphertext + digest

			if(len == count) // This means no buffering of the data.
			{
				char *_array = (char *)(array + offset);
				// Decrypt the received data into array.
				cryptography_engine->Decrypt(ciphertext, _array, count);
				if(!ShaEngine::Sha2VerifyDigest256(_array, count, digest))
					throw System::Exception("The received data via xSec protocol is not verified");
				read_buffer_start = read_buffer_end = 0;
				read_buffer_is_dirty = false;
				return len;
			}
			else
			{
				// Decrypt the received data into read_buffer.
				cryptography_engine->Decrypt(ciphertext, read_buffer, count);
				read_buffer_start = 0;
				read_buffer_end = count - 1;
				read_buffer_is_dirty = true;
				if(!ShaEngine::Sha2VerifyDigest256(read_buffer, count, digest))
					throw System::Exception("The received data via xSec protocol is not verified");

				return 0;
			}
		}
		//----------------------------------------------------
		Int32 ParvicursorSocket::ReadAuxiliary(char array[], Int32 offset, Int32 count)
		{
			Int32 n = 0, sum = 0;
			while(true)
			{
				n = socket->Receive(array, offset + sum, count - sum, System::Net::Sockets::None);

				/*if(n == 0)
				break;*/

				if(n == 0)
					throw ObjectDisposedException("The remote endpoint closed the connection");

				sum += n;

				if(sum == count)
					break;
			}
			return sum;
		}
		//----------------------------------------------------
		char *ParvicursorSocket::Read(Int32 size)
		{
			if(size <= 0)
				throw ArgumentOutOfRangeException("size", "size must be greater than 0");

			if(!xSecChannelSetup_has_called) xSecChannelSetup();

			char *ret = new char[size];

			try{ this->Read(ret, 0, size); }
			catch(...)
			{
				delete ret;
				return null;
			}

			return ret;
		}
		//----------------------------------------------------
		Byte ParvicursorSocket::ReadByte()
		{
			if(!xSecChannelSetup_has_called) xSecChannelSetup();

			Read((char *)buff, 0, sizeof(Byte));
			return buff[0];
		}
		//----------------------------------------------------
		void ParvicursorSocket::FlushWriteBuffer()
		{
			if(write_buffer_is_dirty)
			{
				WriteInternal(write_buffer, 0, write_buffer_next_index);
				write_buffer_next_index = 0;
				write_buffer_is_dirty = false;
			}
		}
		//----------------------------------------------------
		Int32 ParvicursorSocket::Write(const char array[], Int32 offset, Int32 count)
		{
			if(disposed)
				throw ObjectDisposedException("Could not access to a closed object");

			if(array == null)
				throw ArgumentNullException("array", "array can not be null");

			if(offset < 0)
				throw ArgumentOutOfRangeException("offset", "offset is less than 0");

			if(count <= 0)
				throw ArgumentOutOfRangeException("count", "count must be greater than 0");

			if(!xSecChannelSetup_has_called) xSecChannelSetup();

			if(!isSecure)
				return WriteAuxiliary(array, offset, count);
			else
			{
				if(count < buffer_size - write_buffer_next_index - 1) // This means the write_buffer has enough empty slots to buffer the array.
				{
					char *_array = (char *)array;
					_array += offset;
					write_buffer += write_buffer_next_index;
					memcpy(write_buffer, _array, count);
					write_buffer -= write_buffer_next_index;
					write_buffer_next_index += count;
					_array -= offset;
					write_buffer_is_dirty = true;
					return count;
				}
				else
				{
					FlushWriteBuffer();

					if(count == buffer_size)
					{
						WriteInternal(array, offset, count);
						return count;
					}
					else
					{
						Int32 i = 0;
						Int32 a = count / buffer_size;
						Int32 q = count % buffer_size;
						while(true)
						{
							WriteInternal(array, buffer_size*i, buffer_size);
							i++;
							if(q != 0 && i == a)
							{
								WriteInternal(array, buffer_size*i, q);
								break;
							}
							if(q == 0 && i == a)
								break;
						}

						return count;
					}
				}
			}
		}
		//----------------------------------------------------
		void ParvicursorSocket::WriteInternal(const char array[], Int32 offset, Int32 count)
		{
			char *_array = (char *)(array + offset);

			// Data length
			char *length = cryptography_buffer;
			Parvicursor_GetBytesFromInt32Number(length, count);

			char *ciphertext = (char *)(sizeof(Int32) + cryptography_buffer);

			// Compute the data hash.
			char *digest = (char *)(sizeof(Int32) + cryptography_buffer + count);
			ShaEngine::Sha2ComputeHash256(_array, count, digest);

			//digest[ShaEngine::Sha2DigestSize256] = '\0';

			// Encrypt the data.
			cryptography_engine->Encrypt(_array, ciphertext, count);

			//Int32 sent = socket->Send(cryptography_buffer, 0, sizeof(Int32) + count + ShaEngine::Sha2DigestSize256, System::Net::Sockets::None);

			count += (sizeof(Int32) + ShaEngine::Sha2DigestSize256);

			Int32 ret = WriteAuxiliary(cryptography_buffer, 0, count);
			count -= (sizeof(Int32) + ShaEngine::Sha2DigestSize256);
		}
		//----------------------------------------------------
		Int32 ParvicursorSocket::WriteAuxiliary(const char array[], Int32 offset, Int32 count)
		{
			Int32 n = 0, sum = 0;
			while(true)
			{
				n = socket->Send(array, offset + sum, count - sum, System::Net::Sockets::None);

				/*if(n == 0)
				break;*/

				/*if(n == 0)
				throw ObjectDisposedException("The remote endpoint closed the connection");*/

				sum += n;

				if(sum == count)
					break;
			}
			return sum;
		}
		//----------------------------------------------------
		Int32 ParvicursorSocket::Write(const char array[], Int32 count)
		{
			return Write(array, 0, count);
		}
		//----------------------------------------------------
		void ParvicursorSocket::Flush()
		{
			if(isSecure)
				FlushWriteBuffer();
		}
		//----------------------------------------------------
		void ParvicursorSocket::WriteByte(Byte buffer)
		{
			if(!xSecChannelSetup_has_called) xSecChannelSetup();
			buff[0] = buffer;
			Write((char *)buff, 0, sizeof(Byte));
			//this->Write(&buffer, 0, 1);
		}
		//----------------------------------------------------
		RsaEngine ParvicursorSocket::rsaEngine;
		Mutex *ParvicursorSocket::mutex = new Mutex();
		bool ParvicursorSocket::rsaEngineHasBeenInitialized = false;
		//----------------------------------------------------
   };
};
//**************************************************************************************************************//
