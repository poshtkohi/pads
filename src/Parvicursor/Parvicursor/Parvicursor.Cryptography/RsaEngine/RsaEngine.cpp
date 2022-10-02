/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "RsaEngine.h"

//**************************************************************************************************************//
namespace Parvicursor
{
   namespace Cryptography
   {
		//----------------------------------------------------
		RsaParameters::RsaParameters(InvertibleRSAFunction &params)
		{
			this->params = params;
			n = null;
			//p = null;
			//q = null;
			d = null;
			e = null;
			disposed = false;
		}
		//----------------------------------------------------
		RsaParameters::RsaParameters()
		{
			n = null;
			//p = null;
			//q = null;
			d = null;
			e = null;
			disposed = true;
		}
		//----------------------------------------------------
		RsaParameters::RsaParameters(const char *n, Int32 n_size, const char *e, Int32 e_size)
		{
			if(n == null)
				throw ArgumentNullException("n", "n can not be null");

			if(e == null)
				throw ArgumentNullException("e", "e can not be null");

			if(n_size <= 0)
				throw ArgumentOutOfRangeException("n_size", "n_size must be greater than 0");

			if(e_size <= 0)
				throw ArgumentOutOfRangeException("e_size", "e_size must be greater than 0");

			params = InvertibleRSAFunction();
			Integer _n = Integer((const byte *)n, n_size);
			params.SetModulus(_n);
			Integer _e = Integer((const byte *)e, e_size);
			params.SetPublicExponent(_e);
			this->n = null;
			//p = null;
			//q = null;
			this->d = null;
			this->e = null;
			disposed = false;
		}
		//----------------------------------------------------
		RsaParameters::RsaParameters(const char *n, Int32 n_size, const char *e, Int32 e_size, const char *d, Int32 d_size)
		{
			if(n == null)
				throw ArgumentNullException("n", "n can not be null");

			if(e == null)
				throw ArgumentNullException("e", "e can not be null");

			if(d == null)
				throw ArgumentNullException("d", "d can not be null");

			if(n_size <= 0)
				throw ArgumentOutOfRangeException("n_size", "n_size must be greater than 0");

			if(e_size <= 0)
				throw ArgumentOutOfRangeException("e_size", "e_size must be greater than 0");

			if(d_size <= 0)
				throw ArgumentOutOfRangeException("d_size", "d_size must be greater than 0");

			params = InvertibleRSAFunction();
			Integer _n = Integer((const byte *)n, n_size);
			params.SetModulus(_n);
			Integer _e = Integer((const byte *)e, e_size);
			params.SetPublicExponent(_e);
			Integer _d = Integer((const byte *)d, d_size);
			params.SetPrivateExponent(_d);
			this->n = null;
			//p = null;
			//q = null;
			this->d = null;
			this->e = null;
			disposed = false;
		}
		//----------------------------------------------------
		RsaParameters::~RsaParameters()
		{
			if(!disposed)
			{
				if(n != null)
					delete n;

				/*if(p != null)
					delete p;

				if(q != null)
					delete q;*/

				if(d != null)
					delete d;

				if(e != null)
					delete e;

				disposed = true;
			}
		}
		//----------------------------------------------------
		const char *RsaParameters::get_n(Out Int32 &size)
		{
			if(disposed)
				throw ObjectDisposedException("Could not access to a closed object");

			if(n != null)
			{
				size = n_size;
				return n;
			}

			const Integer &_n = params.GetModulus();
			n_size = _n.MinEncodedSize();
			n = new char[n_size];
			_n.Encode((byte *)n, n_size);
			//n[n_size] = '\0';
			size = n_size;

			/*printf("nnnn: ");
			for(Int32 i = 0 ; i < size ; i++)
				printf("%d", (Byte)n[i]);

			printf("\n");*/

			return n;
		}
		//----------------------------------------------------
		/* const char *RsaParameters::get_p(Out Int32 &size)
		{
			if(disposed)
				throw ObjectDisposedException("Could not access to a closed object");

			if(p != null)
			{
				size = p_size;
				return p;
			}

			const Integer &_p = params.GetPrime1();
			p_size = _p.MinEncodedSize();
			p = new char[p_size];
			_p.Encode((byte *)p, p_size);
			size = p_size;
			return p;
		}
		//----------------------------------------------------
		const char *RsaParameters::get_q(Out Int32 &size)
		{
			if(disposed)
				throw ObjectDisposedException("Could not access to a closed object");
			if(q != null)
			{
				size = q_size;
				return q;
			}

			const Integer &_q = params.GetPrime2();
			q_size = _q.MinEncodedSize();
			q = new char[q_size];
			_q.Encode((byte *)q, q_size);
			size = q_size;
			return q;
		}*/
		//----------------------------------------------------
		const char *RsaParameters::get_d(Out Int32 &size)
		{
			if(disposed)
				throw ObjectDisposedException("Could not access to a closed object");

			if(d != null)
			{
				size = d_size;
				return d;
			}

			try
			{
				const Integer &_d = params.GetPrivateExponent();
				d_size = _d.MinEncodedSize();
				d = new char[d_size];
				_d.Encode((byte *)d, d_size);
				size = d_size;
				return d;
			}
			catch(CryptoPP::Exception &e)
			{
				throw System::Exception((const char *)e.GetWhat().c_str());
				//cout << "Exception was occured. Exception message: " << e.GetWhat() << endl;
			}
		}
		//----------------------------------------------------
		const char *RsaParameters::get_e(Out Int32 &size)
		{
			if(disposed)
				throw ObjectDisposedException("Could not access to a closed object");

			if(e != null)
			{
				size = e_size;
				return e;
			}

			try
			{
				const Integer &_e = params.GetPublicExponent();
				e_size = _e.MinEncodedSize();
				e = new char[e_size];
				_e.Encode((byte *)e, e_size);
				size = e_size;
				return e;
			}
			catch(CryptoPP::Exception &e)
			{
				throw System::Exception((const char *)e.GetWhat().c_str());
				//cout << "Exception was occured. Exception message: " << e.GetWhat() << endl;
			}
		}
		//----------------------------------------------------
		InvertibleRSAFunction &RsaParameters::get_Params()
		{
			if(disposed)
				throw ObjectDisposedException("Could not access to a closed object");

			return params;
		}
		//----------------------------------------------------
		RsaEngine::RsaEngine()
		{
			disposed = true;
		}
		//----------------------------------------------------
		RsaEngine::RsaEngine(RsaParameters &params)
		{
			// Create keys.
			//privateKey = RSA::PrivateKey(params.get_Params());
			//publicKey = RSA::PublicKey(params.get_Params());
			privateKey_initialized = false;
			publicKey_initialized = false;
			this->params = params;
			disposed = false;
		}
		//----------------------------------------------------
		RsaEngine::~RsaEngine()
		{
			if(!disposed)
			{
				disposed = true;
			}
		}
		//----------------------------------------------------
		RsaParameters RsaEngine::GenerateRsaParameters(Int32 size)
		{
			if(size <= 0)
				throw System::ArgumentOutOfRangeException("len", "len must be greater than size");

			try
			{
				InvertibleRSAFunction params;
				params.GenerateRandomWithKeySize(*rnd, size);

				RsaParameters parameters = RsaParameters(params);
				return parameters;
			}
			catch(CryptoPP::Exception &e)
			{
				throw System::Exception((const char *)e.GetWhat().c_str());
				//cout << "Exception was occured. Exception message: " << e.GetWhat() << endl;
			}
		}
		//----------------------------------------------------
		void RsaEngine::Encrypt(In const char plaintext[], Out char **ciphertext, InOut Int32 &len)
		{
			if(disposed)
				throw ObjectDisposedException("Could not access to a closed object");

			if(len <= 0)
				throw System::ArgumentOutOfRangeException("len", "len must be greater than zero");

			if(*ciphertext != null)
				delete *ciphertext;

			if(!publicKey_initialized)
			{
				publicKey.Initialize(params.get_Params().GetModulus(), params.get_Params().GetPublicExponent());
				publicKey_initialized = true;
			}

			// Encryption
			// At this point, we have n, e and m. To encrypt m, we perform the following.
			Integer m = Integer((const byte *)plaintext, len);
			try
			{
				Integer c = publicKey.ApplyFunction(m);
				len = c.MinEncodedSize();
				*ciphertext = new char[len + 1];
				(*ciphertext)[len] = '\0';
				c.Encode((byte *)*ciphertext, len);
			}
			catch(CryptoPP::Exception &e)
			{
				if(*ciphertext != null)
					delete *ciphertext;
				*ciphertext = null;
				cout << "Exception was occured. Exception message: " << e.GetWhat() << endl;
				throw System::Exception((const char *)e.GetWhat().c_str());
			}

		}
		//----------------------------------------------------
		void RsaEngine::Decrypt(In const char ciphertext[], Out char **plaintext, InOut Int32 &len)
		{
			if(disposed)
				throw ObjectDisposedException("Could not access to a closed object");

			if(len <= 0)
				throw System::ArgumentOutOfRangeException("len", "len must be greater than zero");

			/*if(*plaintext != null)
				delete *plaintext;*/

			if(!privateKey_initialized)
			{
				privateKey.Initialize(params.get_Params().GetModulus(), params.get_Params().GetPublicExponent(), params.get_Params().GetPrivateExponent());
				privateKey_initialized = true;
			}

			try
			{
				// Decryption
				Integer c = Integer((const byte *)ciphertext, len);
				Integer m = privateKey.CalculateInverse(*rnd, c);
				len = m.MinEncodedSize();
				*plaintext = new char[len + 1];
				(*plaintext)[len] = '\0';
				m.Encode((byte *)*plaintext, len);
			}
			catch(CryptoPP::Exception &e)
			{
				if(*plaintext != null)
					delete *plaintext;
				*plaintext = null;
				cout << "Exception was occurred. Exception message: " << e.GetWhat() << endl;
				throw System::Exception((const char *)e.GetWhat().c_str());
			}
		}
		//----------------------------------------------------
		RsaParameters &RsaEngine::get_RsaParameters()
		{
			if(disposed)
				throw ObjectDisposedException("Could not access to a closed object");

			return params;
		}
		//----------------------------------------------------
		AutoSeededRandomPool *RsaEngine::rnd = new AutoSeededRandomPool();
		//----------------------------------------------------
   };
};
//**************************************************************************************************************//
