// Cryptography.cpp : Defines the entry point for the console application.
//

//---------------------------------------
#include "../../Parvicursor/general.h"
#include "../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../Parvicursor/System/String/String.h"
#include "../../Parvicursor/Parvicursor/Parvicursor.Cryptography/AesEngine/AesEngine.h"
#include "../../Parvicursor/Parvicursor/Parvicursor.Cryptography/RsaEngine/RsaEngine.h"
#include "../../Parvicursor/Parvicursor/Parvicursor.Cryptography/ShaEngine/ShaEngine.h"
//---------------------------------------
using namespace System;
using namespace Parvicursor::Cryptography;
//---------------------------------------
void SHA_test(void);
void RSA_test(void);
void AES_test(void);
//---------------------------------------
int main(int argc, char *args[])
{
	printf("SHA test:\n");
	SHA_test();

	printf("\n\n\n\nRSA test:\n");
	RSA_test();

	printf("\n\n\n\nAES test:\n");
	AES_test();

	return 0;
}
//---------------------------------------
void SHA_test(void)
{
	const char *message = "Hello World";
	Int32 messageLen = ::strlen(message);
	char digest[ShaEngine::Sha2DigestSize256];

	ShaEngine::Sha2ComputeHash256(message, messageLen, digest);

	printf("message: %s\nDigest size: %d bytes\ndigest: %s\n", message , ShaEngine::Sha2DigestSize256, digest);

	if(ShaEngine::Sha2VerifyDigest256(message, messageLen, digest))
		printf("The message digest was verified.\n");
	else
		printf("The message digest was not verified.\n");
}
//---------------------------------------
void BufferPrint(const char *name, const char *buffer, Int32 size)
{
	printf("%s: ", name);
	for(Int32 i  = 0 ; i < size ; i++)
		printf("%d", (Byte)buffer[i]);
	printf("\nsize: %d\n", size);
}
void RSA_test(void)
{
	RsaParameters params = RsaEngine::GenerateRsaParameters(2048);

	const char *plaintext = "Hello World";
	Int32 messageLen = ::strlen(plaintext);
	char *ciphertext = null;
	char *decryptedtext = null;

	try
	{
		RsaEngine engine1 = RsaEngine(params);
		Int32 n_size = 0;
		const char *n = params.get_n(n_size);
		Int32 e_size = 0;
		const char *e = params.get_e(e_size);
		Int32 d_size = 0;
		const char *d = params.get_d(d_size);
		RsaParameters _params = RsaParameters(n, n_size, e, e_size, d, d_size);
		RsaEngine engine2 = RsaEngine(_params);
		/*Int32 size;
		const char *n = engine.get_RsaParameters().get_n(size);
		BufferPrint("n", n, size);
		const char *e = engine.get_RsaParameters().get_e(size);
		BufferPrint("e", e, size);
		return;*/
		engine1.Encrypt(plaintext, &ciphertext, messageLen);
		Int32 messageLen1 = messageLen;
		for(Int32 i  = 0 ; i < 10 ; i++)
		{
			//messageLen = ::strlen(plaintext);
			//engine.Encrypt(plaintext, &ciphertext, messageLen);
			messageLen = messageLen1;
			engine2.Decrypt(ciphertext, &decryptedtext, messageLen);
			//delete ciphertext; ciphertext = null;
			printf("messageLen: %d\n", messageLen);
			printf("plaintext: %s\n\nciphertext: %s\n\ndecryptedtext: %s\n\n", plaintext, ciphertext, decryptedtext);
			delete decryptedtext; decryptedtext = null;
		}

		//printf("plaintext: %s\n\nciphertext: %s\n\ndecryptedtext: %s\n\n", plaintext, ciphertext, decryptedtext);
	}
	catch(System::Exception &e)
	{
		printf("Exception Message: %s\n", e.get_Message().get_BaseStream());
	}

	if(ciphertext != null)
		delete ciphertext;

	if(decryptedtext != null)
		delete decryptedtext;
}
//---------------------------------------
void AES_test(void)
{
	//
	// Key and IV setup
	// AES encryption uses a secret key of a variable length (128-bit, 196-bit or 256-   
	// bit). This key is secretly exchanged between two parties before communication   
	// begins. DEFAULT_KEYLENGTH= 16 bytes
	// Generate a random key
	CryptographyKey key = AesEngine::GenerateKey(AesEngine::DefaultKeyLength);
	// Generate a random IV
	Byte iv[AesEngine::DefaultBlockSize];
	AesEngine::GenerateIV(iv, AesEngine::DefaultBlockSize);

	printf("AesEngine::DefaultKeyLength: %d AesEngine::DefaultBlockSize: %d ShaEngine::Sha2DigestSize256: %d\n", AesEngine::DefaultKeyLength, AesEngine::DefaultBlockSize, ShaEngine::Sha2DigestSize256);

	//
	// String and Sink setup
	//
	char *plaintext = "Hello World";
	int messageLen = ::strlen(plaintext);
	char *ciphertext = (char *)::malloc(sizeof(char) * (messageLen + 1) );
	char *decryptedtext = (char *)::malloc(sizeof(char) * (messageLen + 1) );
	ciphertext[messageLen] = '\0';
	decryptedtext[messageLen] = '\0';

	//
	// Dump Plain Text
	//
	cout << "Plain Text: " << plaintext << endl << endl;


	try
	{
		AesEngine aes = AesEngine(key, (const Byte *)iv, AesEngine::DefaultBlockSize);

		// Create Cipher Text
		aes.Encrypt(plaintext, ciphertext, messageLen);
		cout << "Cipher Text: " << ciphertext << endl << endl;

		// Create Plain Text
		aes.Decrypt(ciphertext, decryptedtext, messageLen);
		cout << "Plain Text: " << decryptedtext << endl << endl;
	}
	catch(System::Exception &e)
	{
		printf("Exception Message: %s\n", e.get_Message().get_BaseStream());
	}

	::free(ciphertext);
	::free(decryptedtext);
}
//---------------------------------------
