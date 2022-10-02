/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __Parvicursor_Cryptography_RsaEngine_h__
#define __Parvicursor_Cryptography_RsaEngine_h__

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
#include <cryptopp/rsa.h>
using namespace CryptoPP;

using namespace System;

//**************************************************************************************************************//
namespace Parvicursor
{
    namespace Cryptography
    {
		//----------------------------------------------------
		// RSA Parameters and Operations
		// n: Modulus
		// p: Prime number 1
		// q: Prime number 2
		// d: Private exponent
		// e: Public exponent
		// M: Message (plaintext)
		// C: Ciphertext
		// Private key: (d, n)
		// Public key: (e, n)
		// Encryption: C = M^e mod n
		// Decryption: M = C^d mod n
		class RsaParameters : public Object
		{
			/*---------------------fields----------------*/
			private: char *n; // mode, n = p.q
			//private: char *p; // prime number 1
			//private: char *q; // prime number 2
			private: char *d; // Kprivate = (d, n)
			private: char *e; // Kpublic = (e, n)
			private: Int32 n_size;
			//private: Int32 p_size;
			//private: Int32 q_size;
			private: Int32 d_size;
			private: Int32 e_size;
			private: InvertibleRSAFunction params;
			private: bool disposed;
			/*---------------------methods----------------*/
			public: RsaParameters();
			public: RsaParameters(InvertibleRSAFunction &params);
			public: RsaParameters(const char *n, Int32 n_size, const char *e, Int32 e_size);
			public: RsaParameters(const char *n, Int32 n_size, const char *e, Int32 e_size, const char *d, Int32 d_size);
			public: ~RsaParameters();
			// Gets the RSA parameter's n (modulus or mode).
			public: const char *get_n(Out Int32 &size);
			// Gets the RSA parameter's p (p is a prime number in n = p*q).
			//public: char *get_p(Out Int32 &size);
			// Gets the RSA parameter's q (q is a prime number in n = p*q).
			//public: char *get_q(Out Int32 &size);
			// Gets the RSA parameter's d (private exponent). Private key is = (d, n).
			public: const char *get_d(Out Int32 &size);
			// Gets the RSA parameter's e (public exponent). Public key is = (e, n).
			public: const char *get_e(Out Int32 &size);
			// This method is internally used. Please don't invoke this method directly from your code.
			public: InvertibleRSAFunction &get_Params();
		};

		class RsaEngine : public Object
		{
			/*---------------------fields----------------*/
			private: static AutoSeededRandomPool *rnd;
			private: RSA::PrivateKey privateKey;
			private: RSA::PublicKey publicKey;
			private: bool privateKey_initialized;
			private: bool publicKey_initialized;
			private: RsaParameters params;
			private: bool disposed;
			/*---------------------methods----------------*/
			public: RsaEngine();
			public: RsaEngine(RsaParameters &params);
			public: ~RsaEngine();
			// Generates the RSA parameters.
			public: static RsaParameters GenerateRsaParameters(Int32 size);
			// Encrypts the plaintext and stores it into the location of ciphertext.
			// len specifies the length of plaintext, also it will be involved the length of ciphertext
			// as the output parameter. Note: You must free the ciphertext after using this method because
			// this method allocates its memory.
			public: void Encrypt(In const char plaintext[], Out char **ciphertext, InOut Int32 &len);
			// Decrypts the ciphertext and stores it into the location of plaintext.
			// len specifies the length of ciphertext, also it will be involved the length of plaintext
			// as the output parameter. Note: You must free the plaintext after using this method because
			// this method allocates its memory.
			public: void Decrypt(In const char ciphertext[], Out char **plaintext, InOut Int32 &len);
			// Gets the RSA algoirtm's parameters such as public and private keys.
			public: RsaParameters &get_RsaParameters();
		};
		//----------------------------------------------------
    };
};
//**************************************************************************************************************//

#endif

