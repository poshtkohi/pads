// xThreadPiCalcDLL.cpp : Defines the entry point for the DLL application.

// Important notice:
// To run your code correctly, please carefully read the template-like code comments.

// You MUST not use global variables in this shared library (DLL or SO),
// because it causes loader-lock deadlock at the OS kernel.
// You MUST not create threads in this class and MUST not use any
// synchronization mechanism such as mutexes, semaphores and critical sections
// (if used, provide releasing the locks within the deconstructor).

#include "xThreadPiCalcDLL.h"
//-----------------------------------------------------
xThreadPiCalc::xThreadPiCalc(Int32 startDigitNum, Int32 numDigits)
{
	this->startDigitNum = startDigitNum;
	this->numDigits = numDigits;
	this->result = new char[numDigits + 1];
	this->strLen = 0;

	// Here, you MUST set the values of serializer and deserializer pointers to null,
	// otherwise your program will have to crash.
	// SURE you have allocated all memories here and provided the de-allocation of them
	// in the ~xThreadPiCalc() deconstructor. This means YOU must not allocate memory in Start() method
	// with new operator or malloc() function call.
	// For example, if you need an opened file handle in Start() method,
	// you MUST provide closing the file handle in ~xThreadPiCalc() deconstructor.

	this->serializer = null;
	this->deserializer = null;
}
//-----------------------------------------------------
xThreadPiCalc::xThreadPiCalc()
{
	this->startDigitNum = 0;
	this->numDigits = 0;
	this->result = null;
	this->strLen = 0;

	// Here, you MUST set the values of serializer and deserializer pointers to null,
	// otherwise your program will have to crash.

	this->serializer = null;
	this->deserializer = null;
}
//-----------------------------------------------------
xThreadPiCalc::~xThreadPiCalc()
{
	// Here, you MUST release the allocated C++ stack memory for
	// Serializer and DeSerializer objects to avoid memory leaks for your entire multi-threaded application.

	if(this->serializer != null)
	{
		delete this->serializer;
		this->serializer = null;
	}

	if(this->deserializer != null)
	{
		delete this->deserializer;
		this->deserializer = null;
	}

	if(this->result != null)
		delete this->result;
}
//-----------------------------------------------------
inline static Int32 mul_mod(Int32 a, Int32 b, Int32 m) 
{
    return (Int32) (((long) a * (long) b) % m);
}

/* return the inverse of x mod y */
inline static Int32 inv_mod(Int32 x, Int32 y) 
{
    Int32 q,u,v,a,c,t;
    u = x;
    v = y;
    c = 1;
    a = 0;
    while(u != 0) 
    {
        q = v/u;

        t = c;
        c = a - q*c;
        a = t;

        t = u;
        u = v - q*u;
        v = t;
    } 

    a = a%y;

    if(a < 0) 
    {
        a = y + a;
    }

    return a;
}

/* return (a^b) mod m */
inline static Int32 pow_mod(Int32 a, Int32 b, Int32 m)
{
    Int32 r, aa;

    r = 1;
    aa = a;

    while(true) 
    {
        if((b & 1) != 0)
        {
            r = mul_mod(r, aa, m);
        }

        b = b >> 1;

        if(b == 0)
        {
            break;
        }

        aa = mul_mod(aa, aa, m);
    }

    return r;
}

/* return true if n is prime */
inline static bool is_prime(Int32 n)
{
    if((n % 2) == 0) 
    {
        return false;
    }

	Int32 r = (Int32)System::Math::Sqrt(n);

    for(register Int32 i = 3 ; i <= r ; i+=2)
    {
        if((n % i) == 0) 
        {
            return false;
        }
    }

    return true;
}

/* return the prime number immediately after n */
inline static Int32 next_prime(Int32 n)
{
    do 
    {
        n++;
    } while (!is_prime(n));

    return n;
}

inline static Double double_mod(Double a, Double b)
{
	//Double a = 10.5, b = 1.0, c;
	Double c;
	Int32 q;
	//a = bq + c;
	q = (Int32)(a/b);
	c = a - b*q;
	return c;
}
static void CalculatePiDigits(Int32 n, char *retStr)
{
    Int32 av, vmax, num, den, s, t;

	Int32 N = (Int32) ((n + 20) * Math::Log(10) / Math::Log(2));

    Double sum = 0;

    for(register Int32 a = 3 ; a <= (2 * N) ; a = next_prime(a)) 
    {
		vmax = (Int32) (Math::Log(2 * N) / Math::Log(a));

        av = 1;

        for(register Int32 i = 0; i < vmax ; i++)
        {
            av = av * a;
        }

        s = 0;
        num = 1;
        den = 1;
        Int32 v = 0;
        Int32 kq = 1;
        Int32 kq2 = 1;

        for(register Int32 k = 1 ; k <= N ; k++) 
        {

            t = k;

            if (kq >= a) 
            {
                do 
                {
                    t = t / a;
                    v--;
                } while((t % a) == 0);

                kq = 0;
            }
            kq++;
            num = mul_mod(num, t, av);

            t = 2 * k - 1;

            if (kq2 >= a) 
            {
                if (kq2 == a) 
                {
                    do 
                    {
                        t = t / a;
                        v++;
                    } while ((t % a) == 0);
                }
                kq2 -= a;
            }
            den = mul_mod(den, t, av);
            kq2 += 2;

            if (v > 0) 
            {
                t = inv_mod(den, av);
                t = mul_mod(t, num, av);
                t = mul_mod(t, k, av);

                for(register Int32 i = v ; i < vmax ; i++)
                {
                    t = mul_mod(t, a, av);
                }

                s += t;

                if (s >= av)
                {
                    s -= av;
                }
            }

        }

        t = pow_mod(10, n - 1, av);
        s = mul_mod(s, t, av);
		sum = double_mod(sum + (Double) s / (Double) av, 1.0);
    }
	
    Int32 Result = (Int32) (sum * 1e9);

    //String StringResult = String.Format("{0:D9}", Result);

    //return StringResult;
	//return Convert::ToString(Result);
	//return Result;

#if defined WIN32 || WIN64
			sprintf(retStr, "%I32u", Result);    // Convert int to string
#else
			sprintf(retStr, "%lu", Result);    // Convert int to string
#endif
}
//-----------------------------------------------------
void xThreadPiCalc::Start()
{
	// NOTE: You MUST never use the Thread::Sleep() or other thread sleep APIs in this method.
	// Because it may cause loader-lock deadlock in the OS kernel.
	// Don't define any variable that allocate memory from heap in this method. Use instead
	// the class variable members to allocate memory from heap. Don't declare local System::String
	// primitive data type in this method, since it allocates memory from heap.
	// This method is very useful in a pure compute-bound loop without Sleep() for execution
	// of scientific algorithms.

	char retStr[1024];
	this->result[0] = '\0';

	for(register Int32 i = 0; i <= Math::Ceiling(this->numDigits / 9) ; i++)
    {
		CalculatePiDigits(this->startDigitNum + (i * 9), retStr);
		strcat(this->result, retStr);
    }

	result[this->numDigits] = '\0';
	this->strLen = this->numDigits;
}
//-----------------------------------------------------
void xThreadPiCalc::Deconstructor(xThreadBase *obj)
{
	// You MUST implement this method, since Parvicursor.NET Framework uses it extensively.

	if(obj != null)
	{
		delete ((xThreadPiCalc *)obj);
		obj = null;
	}
}
//-----------------------------------------------------
void xThreadPiCalc::FreeDllMemory(void *mem)
{
	// You MUST implement this method, since Parvicursor.NET Framework uses it extensively.
	// If the memory has been allocated by malloc() function, you MUST call here free() function.
	// If the memory has been allocated by C++ new operator, you MUST call here C++ delete operator.

	if(mem != null)
	{
		delete mem;
		mem = null;
	}
}
//-----------------------------------------------------
void *xThreadPiCalc::AllocateDllMemory(UInt32 size)
{
	// You MUST implement this method, since Parvicursor.NET Framework uses it extensively.
	// If the memory has been allocated by malloc() function, you MUST call here free() function.
	// If the memory has been allocated by C++ new operator, you MUST call here C++ delete operator.

	return (void *)new char[size];
}
//-----------------------------------------------------
void xThreadPiCalc::Serialize(char **DllBuffer, Int32 *len)
{
	// Don't change the values of len and DllBuffer variable pointers, since your program will have to crash.
	// These variables are to be used by Parvicursor.NET Framework for high-performance buffer transfers to
	// remote (or local) cross-process boundaries by Parvicursor Object-Passing Interface (XDOPI).

	Int32 bufferSize = sizeof(this->startDigitNum) + sizeof(this->numDigits) + sizeof(this->strLen) 
						+ this->numDigits * sizeof(char);				
																			// To avoid successive memory allocation overhead, use the pre-allocated memory and
																			// increase XDOPI peformance;
																			// you MUST consider to keep constant the bufferSize length in mind.
																			// Also, you can select a default size that you guess it is always
																			// greater than or equal to your real objects size. (e.g., a large 1MB buffer size)

	if(this->serializer == null)
		this->serializer = new Serializer(bufferSize);
	else
		this->serializer->Reset(bufferSize);

	this->serializer->Write<Int32>(this->startDigitNum);
	this->serializer->Write<Int32>(this->numDigits);
	this->serializer->Write<Int32>(this->strLen);

	if(this->strLen > 0)
		this->serializer->Write(this->result, this->strLen);

	xThreadBase::MemoryCopy(DllBuffer, len, this->serializer);	// Mapping between C++ memory and DLL C Runtime memory.
															// If this line of code is removed at the end of this method implementation,
															// your program will have to crash.
}
//-----------------------------------------------------
void xThreadPiCalc::DeSerialize(char **buffer, Int32 *len)
{
	// Don't change the values of len and buffer variable pointers, since your program will have to crash.
	// These variables are to be used by Parvicursor.NET Framework for high-performance buffer transfers to
	// remote (or local) cross-process boundaries by Parvicursor Object-Passing Interface (XDOPI).

	if(this->deserializer == null)
		this->deserializer = new DeSerializer(*buffer, *len);
	else
		this->deserializer->Reset(*buffer, *len);

	this->startDigitNum = deserializer->Read<Int32>();
	this->numDigits = deserializer->Read<Int32>();
	this->strLen = deserializer->Read<Int32>();

	if(this->result == null)
	{
		this->result = new char[numDigits + 1];
	}

	if(this->strLen > 0)
	{
		deserializer->Read(this->result, this->strLen);
		this->result[this->strLen] = '\0';
	}
}
//-----------------------------------------------------
#ifdef __cplusplus    // If used by C++ code,
extern "C" {          // we need to export the C interface
#endif


// You MUST not use global variables in this shared library(DLL or SO),
// because it causes loader-lock deadlock at the OS kernel.

//static void *xThreadBase_func_pointer = null;

xThreadBase_DLL_Export void *xThreadBase_creator()	// Don't change this function name and its declaration (prototype),
{													// since Parvicursor uses from this native function
													// to locate your exported DLL function at the runtime.

	// You MUST not use global variables in this shared library(DLL or SO),
	// because it causes loader-lock deadlock at the OS kernel.
	/*if(xThreadBase_func_pointer == null)
	{
		xThreadPiCalc *s = new xThreadPiCalc(); // Here, You MUST call new and instantiate your class to be run by Parvicursor.
		xThreadBase_func_pointer = (void *)s;
		return xThreadBase_func_pointer;
	}
	else
		return xThreadBase_func_pointer;*/

	xThreadPiCalc *s = new xThreadPiCalc(); // Here, You MUST call new operator and instantiate your class to be run by Parvicursor.
	return (void *)s;
}


#ifdef __cplusplus
}
#endif
//-----------------------------------------------------
// In Windows, you MUST not use the following Microsoft-specific function,
// because it causes loader-lock deadlock at the OS kernel.

/*	The below code snippet can be removed in Win platforms.
	It has been intended for advanced programmers in
	much more better coding the DLL's.
*/
/*#if defined WIN32 || WIN64
#ifndef __Win_DllMain__
#define __Win_DllMain__
//#define xThreadBase_API xThreadBase_DLL_Export
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}
#endif
#endif*/
//-----------------------------------------------------
