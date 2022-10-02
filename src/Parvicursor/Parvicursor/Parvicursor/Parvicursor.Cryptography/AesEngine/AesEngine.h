/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __Parvicursor_Cryptography_AesEngine_h__
#define __Parvicursor_Cryptography_AesEngine_h__

#include "../../../general.h"
#include "../../../System/BasicTypes/BasicTypes.h"
#include "../../../System/ArgumentException/ArgumentNullException.h"
#include "../../../System/ArgumentException/ArgumentOutOfRangeException.h"
#include "../../../System/ObjectDisposedException/ObjectDisposedException.h"

#include <cryptopp/modes.h>
#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/rng.h>
#include <cryptopp/osrng.h>
using namespace CryptoPP;

using namespace System;

//**************************************************************************************************************//
namespace Parvicursor
{
    namespace Cryptography
    {
		//----------------------------------------------------
		class CryptographyKey : public Object
		{
			public: SecByteBlock key;
			public: CryptographyKey() {}
			public: CryptographyKey(const Byte *key, Int32 length)
			{
				if(key == null)
					throw ArgumentNullException("key can not be null");

				if(length <= 0)
					throw ArgumentOutOfRangeException("length", "length must be greater than zero");

				this->key = SecByteBlock((const byte *)key, length);
			}
			public: inline const Byte *get_Key() { return (const Byte *)key.data(); }
			public: inline Int32 get_Length() { return key.size(); }
		};

		class AesEngine : public Object
		{
			/*---------------------fields----------------*/
			private: CryptographyKey key;
			private: Byte *iv;
			private: static AutoSeededRandomPool *rnd;
			public: static const Int32 DefaultBlockSize = AES::BLOCKSIZE;
			public: static const Int32 DefaultKeyLength = AES::DEFAULT_KEYLENGTH;
			private: CFB_Mode<AES>::Encryption *cfbEncryption;
			private: CFB_Mode<AES>::Decryption *cfbDecryption;
			private: bool disposed;
			/*---------------------methods----------------*/
			public: AesEngine(CryptographyKey &key, const Byte iv[], Int32 ivLength);
			public: ~AesEngine();
			public: static CryptographyKey GenerateKey(Int32 size);
			public: static void GenerateIV(Out Byte iv[], Int32 size);
			public: void Encrypt(In const char plaintext[], Out char ciphertext[], Int32 len);
			public: void Decrypt(In const char ciphertext[], Out char plaintext[], Int32 len);
			public: CryptographyKey &get_Key();
			public: const Byte *get_IV();
		};
		//----------------------------------------------------
    };
};
//**************************************************************************************************************//

#endif

