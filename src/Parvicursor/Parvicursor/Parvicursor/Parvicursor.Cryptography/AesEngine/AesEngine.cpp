/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "AesEngine.h"

//**************************************************************************************************************//
namespace Parvicursor
{
   namespace Cryptography
   {
		//----------------------------------------------------
		AesEngine::AesEngine(CryptographyKey &key, const Byte iv[], Int32 ivLength)
		{
			if(iv == null)
				throw ArgumentNullException("iv", "iv can not be null");

			if(ivLength <= 0)
				throw ArgumentOutOfRangeException("ivLength", "ivLength must be greater than zero");

			this->key = key;
			//this->iv = iv;
			this->iv = new Byte[ivLength];
			::memcpy(this->iv, iv, ivLength);
			cfbEncryption = new CFB_Mode<AES>::Encryption(this->key.key, this->key.key.size(), this->iv);
			cfbDecryption = new CFB_Mode<AES>::Decryption(this->key.key, this->key.key.size(), this->iv);
			disposed = false;
		}
		//----------------------------------------------------
		AesEngine::~AesEngine()
		{
			if(!disposed)
			{
				delete cfbEncryption;
				delete cfbDecryption;
				delete iv;
				disposed = true;
			}
		}
		//----------------------------------------------------
		CryptographyKey AesEngine::GenerateKey(Int32 size)
		{
			if(size <= 0)
				throw System::ArgumentOutOfRangeException("len", "len must be greater than size");

			try
			{
				// Generate a random key
				SecByteBlock key(size);
				rnd->GenerateBlock(key, key.size());
				CryptographyKey _key;
				_key.key = key;
				return _key;
			}
			catch(CryptoPP::Exception &e)
			{
				throw System::Exception((const char *)e.GetWhat().c_str());
				//cout << "Exception was occured. Exception message: " << e.GetWhat() << endl;
			}
		}
		//----------------------------------------------------
		void AesEngine::GenerateIV(Out Byte iv[], Int32 size)
		{
			if(size <= 0)
				throw System::ArgumentOutOfRangeException("size", "size must be greater than zero");

			try
			{
				// Generate a random IV
				rnd->GenerateBlock((byte *)iv, size);
			}
			catch(CryptoPP::Exception &e)
			{
				throw System::Exception((const char *)e.GetWhat().c_str());
				//cout << "Exception was occured. Exception message: " << e.GetWhat() << endl;
			}
		}
		//----------------------------------------------------
		void AesEngine::Encrypt(In const char plaintext[], Out char ciphertext[], Int32 len)
		{
			if(disposed)
				throw ObjectDisposedException("Could not access to a closed object");

			if(len <= 0)
				throw System::ArgumentOutOfRangeException("len", "len must be greater than zero");

			try
			{
				cfbEncryption->ProcessData((byte *)ciphertext, (byte *)plaintext, len);
			}
			catch(CryptoPP::Exception &e)
			{
				throw System::Exception((const char *)e.GetWhat().c_str());
				//cout << "Exception was occured. Exception message: " << e.GetWhat() << endl;
			}
		}
		//----------------------------------------------------
		void AesEngine::Decrypt(In const char ciphertext[], Out char plaintext[], Int32 len)
		{
			if(disposed)
				throw ObjectDisposedException("Could not access to a closed object");

			if(len <= 0)
				throw System::ArgumentOutOfRangeException("len", "len must be greater than zero");

			try
			{
				cfbDecryption->ProcessData((byte *)plaintext, (byte *)ciphertext, len);
			}
			catch(CryptoPP::Exception &e)
			{
				throw System::Exception((const char *)e.GetWhat().c_str());
				//cout << "Exception was occured. Exception message: " << e.GetWhat() << endl;
			}
		}
		//----------------------------------------------------
		CryptographyKey &AesEngine::get_Key()
		{
			if(disposed)
				throw ObjectDisposedException("Could not access to a closed object");

			return key;
		}
		//----------------------------------------------------
		const Byte *AesEngine::get_IV()
		{
			if(disposed)
				throw ObjectDisposedException("Could not access to a closed object");

			return (const Byte *)iv;
		}
		//----------------------------------------------------
		AutoSeededRandomPool *AesEngine::rnd = new AutoSeededRandomPool();
		//----------------------------------------------------
   };
};
//**************************************************************************************************************//
