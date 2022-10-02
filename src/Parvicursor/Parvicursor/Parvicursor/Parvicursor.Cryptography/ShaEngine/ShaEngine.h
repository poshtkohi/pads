/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __Parvicursor_Cryptography_ShaEngine_h__
#define __Parvicursor_Cryptography_ShaEngine_h__

#include "../../../general.h"
#include "../../../System/BasicTypes/BasicTypes.h"
#include "../../../System/ArgumentException/ArgumentOutOfRangeException.h"
#include "../../../System/ArgumentException/ArgumentNullException.h"

#include <cryptopp/sha.h>
using namespace CryptoPP;

using namespace System;

//**************************************************************************************************************//
namespace Parvicursor
{
    namespace Cryptography
    {
		//----------------------------------------------------
		// SHA hash algoritms.
		class ShaEngine : public Object
		{
			/*---------------------fields----------------*/
			public: const static Int32 Sha1DigestSize160 = CryptoPP::SHA1::DIGESTSIZE;
			public: const static Int32 Sha2DigestSize224 = CryptoPP::SHA224::DIGESTSIZE;
			public: const static Int32 Sha2DigestSize256 = CryptoPP::SHA256::DIGESTSIZE;
			public: const static Int32 Sha2DigestSize512 = CryptoPP::SHA512::DIGESTSIZE;
			/*---------------------methods----------------*/
			// Computes the SHA hash for the input buffer and stores the hash into digest.
			public: static void Sha1ComputeHash160(In const char buffer[], In Int32 bufferLength, Out char digest[]);
			public: static void Sha2ComputeHash224(In const char buffer[], In Int32 bufferLength, Out char digest[]);
			public: static void Sha2ComputeHash256(In const char buffer[], In Int32 bufferLength, Out char digest[]);
			public: static void Sha2ComputeHash512(In const char buffer[], In Int32 bufferLength, Out char digest[]);

			// Verfies whether the diest is correct or not.
			public: static bool Sha1VerifyDigest160(In const char buffer[], In Int32 bufferLength, In const char digest[]);
			public: static bool Sha2VerifyDigest224(In const char buffer[], In Int32 bufferLength, In const char digest[]);
			public: static bool Sha2VerifyDigest256(In const char buffer[], In Int32 bufferLength, In const char digest[]);
			public: static bool Sha2VerifyDigest512(In const char buffer[], In Int32 bufferLength, In const char digest[]);
		};
		//----------------------------------------------------
    };
};
//**************************************************************************************************************//

#endif

