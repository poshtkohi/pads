/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "ShaEngine.h"

//**************************************************************************************************************//
namespace Parvicursor
{
   namespace Cryptography
   {
	   //----------------------------------------------------
	   void ShaEngine::Sha1ComputeHash160(In const char buffer[], In Int32 bufferLength, Out char digest[])
	   {
		   if(buffer == null)
			   throw ArgumentNullException("buffer", "buffer can not be null");

		   if(digest == null)
			   throw ArgumentNullException("digest", "digest can not be null");

		   if(bufferLength <= 0)
			   throw ArgumentOutOfRangeException("bufferLength", "bufferLength must be greater than 0");

		   CryptoPP::SHA1().CalculateDigest((byte *)digest, (const byte *)buffer, bufferLength);
	   }
	   //----------------------------------------------------
		void ShaEngine::Sha2ComputeHash224(In const char buffer[], In Int32 bufferLength, Out char digest[])
		{
			if(buffer == null)
				throw ArgumentNullException("buffer", "buffer can not be null");

			if(digest == null)
				throw ArgumentNullException("digest", "digest can not be null");

			if(bufferLength <= 0)
				throw ArgumentOutOfRangeException("bufferLength", "bufferLength must be greater than 0");

			CryptoPP::SHA224().CalculateDigest((byte *)digest, (const byte *)buffer, bufferLength);
		}
		//----------------------------------------------------
		void ShaEngine::Sha2ComputeHash256(In const char buffer[], In Int32 bufferLength, Out char digest[])
		{
			if(buffer == null)
				throw ArgumentNullException("buffer", "buffer can not be null");

			if(digest == null)
				throw ArgumentNullException("digest", "digest can not be null");

			if(bufferLength <= 0)
				throw ArgumentOutOfRangeException("bufferLength", "bufferLength must be greater than 0");

			CryptoPP::SHA256().CalculateDigest((byte *)digest, (const byte *)buffer, bufferLength);
		}
		//----------------------------------------------------
		void ShaEngine::Sha2ComputeHash512(In const char buffer[], In Int32 bufferLength, Out char digest[])
		{
			if(buffer == null)
				throw ArgumentNullException("buffer", "buffer can not be null");

			if(digest == null)
				throw ArgumentNullException("digest", "digest can not be null");

			if(bufferLength <= 0)
				throw ArgumentOutOfRangeException("bufferLength", "bufferLength must be greater than 0");

			CryptoPP::SHA512().CalculateDigest((byte *)digest, (const byte *)buffer, bufferLength);
		}
		//----------------------------------------------------
		bool ShaEngine::Sha1VerifyDigest160(In const char buffer[], In Int32 bufferLength, In const char digest[])
		{
			if(buffer == null)
				throw ArgumentNullException("buffer", "buffer can not be null");

			if(digest == null)
				throw ArgumentNullException("digest", "digest can not be null");

			if(bufferLength <= 0)
				throw ArgumentOutOfRangeException("bufferLength", "bufferLength must be greater than 0");

			return CryptoPP::SHA1().VerifyDigest((byte *)digest, (const byte *)buffer, bufferLength);
		}
		//----------------------------------------------------
		bool ShaEngine::Sha2VerifyDigest224(In const char buffer[], In Int32 bufferLength, In const char digest[])
		{
			if(buffer == null)
				throw ArgumentNullException("buffer", "buffer can not be null");

			if(digest == null)
				throw ArgumentNullException("digest", "digest can not be null");

			if(bufferLength <= 0)
				throw ArgumentOutOfRangeException("bufferLength", "bufferLength must be greater than 0");

			return CryptoPP::SHA224().VerifyDigest((byte *)digest, (const byte *)buffer, bufferLength);
		}
		//----------------------------------------------------
		bool ShaEngine::Sha2VerifyDigest256(In const char buffer[], In Int32 bufferLength, In const char digest[])
		{
			if(buffer == null)
				throw ArgumentNullException("buffer", "buffer can not be null");

			if(digest == null)
				throw ArgumentNullException("digest", "digest can not be null");

			if(bufferLength <= 0)
				throw ArgumentOutOfRangeException("bufferLength", "bufferLength must be greater than 0");

			return CryptoPP::SHA256().VerifyDigest((byte *)digest, (const byte *)buffer, bufferLength);
		}
		//----------------------------------------------------
		bool ShaEngine::Sha2VerifyDigest512(In const char buffer[], In Int32 bufferLength, In const char digest[])
		{
			if(buffer == null)
				throw ArgumentNullException("buffer", "buffer can not be null");

			if(digest == null)
				throw ArgumentNullException("digest", "digest can not be null");

			if(bufferLength <= 0)
				throw ArgumentOutOfRangeException("bufferLength", "bufferLength must be greater than 0");

			return CryptoPP::SHA512().VerifyDigest((byte *)digest, (const byte *)buffer, bufferLength);
		}
		//----------------------------------------------------
   };
};
//**************************************************************************************************************//
