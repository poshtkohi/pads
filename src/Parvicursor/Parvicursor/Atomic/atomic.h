/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#ifndef __Parvicursor_Atomic__
#define __Parvicursor_Atomic__

#include "../general.h"
#include "../System/BasicTypes/BasicTypes.h"

using namespace System;


extern "C" inline int SimpleISR_x86();


static inline int Parvicursor_SimpleISR_x86()
{
	return SimpleISR_x86();
}
//----------------------------------------------------
/*
	MOVL $1,%eax is AT&T syntax for Unices and
	MOV eax,1 is Intel syntax for Windows.
*/

/*
		CMPXCHG - Compare and Exchange

        Usage:  CMPXCHG dest,src  (486+)
        Modifies flags: AF CF OF PF SF ZF

        Compares the accumulator (8-32 bits) with dest.  If equal the
        dest is loaded with src, otherwise the accumulator is loaded
        with dest.

                                 Clocks                 Size
        Operands         808x  286   386   486          Bytes

        reg,reg           -     -     -     6             2
        mem,reg           -     -     -     7             2

        - add 3 clocks if the mem,reg comparison fails

*/

/*CMPXCHG compares its destination (first) operand to the value in AL,
AX or EAX (depending on the size of the instruction). If they are
equal, it copies its source (second) operand into the destination and
sets the zero flag. Otherwise, it clears the zero flag and leaves the
destination alone.

CMPXCHG is intended to be used for atomic operations in multitasking
or multiprocessor environments. To safely update a value in shared
memory, for example, you might load the value into EAX, load the
updated value into EBX, and then execute the instruction lock cmpxchg
[value],ebx. If value has not changed since being loaded, it is
updated with your desired new value, and the zero flag is set to let
you know it has worked. (The LOCK prefix prevents another processor
doing anything in the middle of this operation: it guarantees
atomicity.) However, if another processor has modified the value in
between your load and your attempted store, the store does not happen,
and you are notified of the failure by a cleared zero flag, so you can
go round and try again.*/

/*The CMPXCHG (compare and exchange) and CMPXCHG8B (compare and exchange
8 bytes) instructions are used to synchronize operations in systems that use
multiple processors. The CMPXCHG instruction requires three operands: a source
operand in a register, another source operand in the EAX register, and a destination
operand. If the values contained in the destination operand and the EAX register are
equal, the destination operand is replaced with the value of the other source
operand (the value not in the EAX register). Otherwise, the original value of the
destination operand is loaded in the EAX register. The status flags in the EFLAGS
register reflect the result that would have been obtained by subtracting the destination
operand from the value in the EAX register.
The CMPXCHG instruction is commonly used for testing and modifying semaphores.
It checks to see if a semaphore is free. If the semaphore is free, it is marked allocated;
otherwise it gets the ID of the current owner. This is all done in one uninterruptible
operation. In a single-processor system, the CMPXCHG instruction
eliminates the need to switch to protection level 0 (to disable interrupts) before
executing multiple instructions to test and modify a semaphore.
For multiple processor systems, CMPXCHG can be combined with the LOCK prefix to
perform the compare and exchange operation atomically. (See "Locked Atomic Operations"
in Chapter 8, "Multiple-Processor Management," of the Intel® 64 and IA-32
Architectures Software Developer’s Manual, Volume 3A, for more information on
atomic operations.)*/


// If the value of accum is equal to dest, the value of dest is changed to newval.
// If this function fails due to the atomic operation or non-equality between accum and dest,
// false is returned and accum is filled with dest.

#if defined WIN32 || WIN64
/*
	The C's prototype of the Parvicursor_AtomicCompareAndSwap function for x64 architectures.
	In x64 architectures, use the assembly file atomic_x64.asm for the following CAS function in Windows OS.
	I did this because Microsoft C++ Compiler for x64 architectures does not support assembly inlining.
*/
extern "C" inline bool Parvicursor_AtomicCompareAndSwap_x64(void volatile *accum, void volatile *dest, Long newval);
/*
	The Microsoft x64 calling convention (for long mode on x86-64) takes advantage of additional register
	space in the AMD64/Intel 64 platform. The registers RCX, RDX, R8, R9 are used for integer and pointer
	arguments (in that order left to right), and XMM0, XMM1, XMM2, XMM3 are used for floating point arguments.
	Additional arguments are pushed onto the stack (right to left). Integer return values (similar to x86) are
	returned in RAX if 64 bits or less. Floating point return values are returned in XMM0. Parameters less than 64
	bits long are not zero extended; the high bits contain garbage.
 */
#endif

inline static bool Parvicursor_AtomicCompareAndSwap(void volatile *accum, void volatile*dest, Long newval)
{
	//CMPXCHG dest,src for Intel syntax
	//CMPXCHG src,dest for AT&T syntax

	//bool success = true;
#if defined WIN32 || WIN64
#	ifdef _M_IX86
	__asm {
		mov edx,accum		// Load the address of the accum variable into edx.
		mov eax,[edx]		// Load the value of the accum variable into eax.
		mov ebx,dest		// Load the address of the dest variable into ebx.
		mov ecx,newval		// Load the value of the newval variable into ecx.
		lock cmpxchg [ebx],ecx // Atomic compare exchange between [ebx] and ecx.
		jne Unsuccessful	// If the values are not equal or the opration was failed
							//	due to concurrent access of threads, then jump if ZF=0.
		mov eax,1			// Here we must fill eax with the true values due to the successful
							// operationn of the cmpxchg instruction.
		jmp End
		// Don't call the ret instruction from the inline assembly, otherwise you'll skip the
		// epilog code that the compiler puts at the end of the function to clean the stack.

Unsuccessful:
		mov [edx],eax		// Copy the eax content (the value of the location pointed by the dest)
							// within the address of the pointer accum.
		mov eax,0			// Here, we must fill eax with the value false due to the unsuccessful
							// operation of the cmpxchg instruction and return with result in eax.
End:
	}
#	elif defined _M_X64
	return Parvicursor_AtomicCompareAndSwap_x64(accum, dest, newval);
#	else
#	error Parvicursor_AtomicCompareAndSwap function is only compiled for x86/x64 CPU architectures.
#	endif

#else

#	if defined __i386__ || __i486__ || __i686__
	bool success;
    asm volatile(
		"movl (%%edx),%%eax\n\t" //accumulator
		"lock cmpxchg %%ecx,(%%ebx)\n\t"
		"jne 0f\n\t" //Jump if ZF=0
		"movl $1,%%eax\n\t" //success = true;
		"jmp 1f\n\t"
"0:\n\t"
		"movl %%eax,(%%edx)\n\t" // copy the eax content within the accum
		"movl $0,%%eax\n\t" //success = false;
"1:\n\t"
        : "=a"(success)
        : "d"(accum), "b"(dest), "c"(newval)
        : /* no changed register */
    );
	return success;
#	elif defined __x86_64__
	Long success;
	/*
	The calling convention of the System V AMD64 application binary interface[9] is followed on Linux and other non-Microsoft operating systems. The registers RDI, RSI, RDX, RCX, R8 and R9 are used for integer and pointer arguments while XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6 and XMM7 are used for floating point arguments. For system calls, R10 is used instead of RCX.[9] As in the Microsoft x64 calling convention, additional arguments are pushed onto the stack and the return value is stored in RAX.
	*/
	asm volatile(
		"movq %1,%%rax\n\t" //accumulator
		"lock cmpxchgq %3,%2\n\t" //rsi=dest, rdx=newval
		"jne 0f\n\t" //Jump if ZF=0
		"movq $1,%%rax\n\t" //success = true;
		"jmp 1f\n\t"
"0:\n\t"
		"movq %%rax,%1\n\t" // copy the rax content within the accum
		"movq $0,%%rax\n\t" //success = false;
"1:\n\t"
		: "=a"(success)
		: "m"(*(Long volatile *)(accum)), "m"(*(Long volatile *)(dest)), "r"(newval)//"d"(accum), "b"(dest), "c"(newval)
		: "memory"//no changed register
	);

	return (bool)success;
#	else
#	error Parvicursor_AtomicCompareAndSwap function is only compiled for x86/x64 CPU archirectures.
#	endif
#endif
}
//----------------------------------------------------
/*
	Tagged pointers
	Because of pointer alignment, the lowest significant bits of each pointer acts as free storage:
	we can tag each pointer with a boolean, or a bit more, depending on the context.
	We just need to remember to mask out the lowest bits when dereferencing the modified pointer.
	This trick should be used sparingly; recall premature optimization is a sin.

	While reading up on The ABA Problem I came across a fantastic hack.  The ABA problem, in a
	nutshell, results from the inability to atomically access both a pointer and a "marked" bit at
	the same time (read the wikipedia page).  One fun, but very hackish solution is to "smuggle" data
	in a pointer.  Example:


	#include "stdio.h"
	void * smuggle(void * ptr, int value){
		return (void *)( (long long)ptr | (value & 3) );
	}

	int recoverData(void * ptr){
		return (long long)ptr & 3;
	}

	void * recoverPointer(void * ptr){
		return (void *)( (long long)ptr & (~3) );
	}
	int main()
	{
		int a = 90;
		int b = 2;
		int * c = &a;
		void * d = smuggle((void *)c, b);
		printf("The value of a is %d\n", *( (int *) recoverPointer(d) ) );
		printf("The value of b is %d\n", recoverData(d) );
	}


	The above code outputs:

	The value of a is 90
	The value of b is 2

	So what happened? On a Unix system like OS X or Linux (as well as almost every operating system),
	memory is aligned to specific addresses. Since a pointer is nothing more than an address, the alignment
	has an interesting effect on the value of the pointer.

	If I run the above code, the integer a has an address of 0x68537518.
	This isn't set in stone, and will be different each time I run the program.
	If we look at the addresses in binary it looks like this:

	0110 1000 0101 0011 0111 0101 0001 1000

	Run the code again and we get 0x66601518:

	0110 0110 0110 0000 0001 0101 0001 1000

	If we look at the address of integer b in the same run we get 0x66601514:

	0110 0110 0110 0000 0001 0101 0001 0100

	There is a lot of similarity in those addresses, but I want to point out the last two binary bits.
	In all addresses, they are zero.  This is because of alignment.  The compiler will
	align the bits so they always start on a value that is a multiple of four, since integers are 4 bytes (32 bits) long.  This means all integer addresses will always end in 00.

	So the smuggle method above takes advantage of that fact.
	Since we know the values are always going to be 00, we can replace them with an arbitrary value
	between 0 and 3.  When we want to retrieve the smuggled data, we only look at the lowest two bits.
	When we want to dereference the pointer, we zero those bits out and voila, we have our original address.

	In almost all applications, this would be a pretty useless trick, but in the case of the ABA problem,
	one can provide a couple of flags in addition to the pointer to better describe the value it is pointing to.
	This allows instructions such as compare-and-set to atomically access both pieces of data.


	Advantages

	The major advantage of tagged pointers is that they take up less space
	than a pointer along with a separate tag field. This can be especially
	important when a pointer is a return value from a function. It can also
	be important in large tables of pointers.

	A more subtle advantage is that by storing a tag in the same place as the
	pointer, it is often possible to guarantee the atomicity of an operation
	that updates both the pointer and its tag without external synchronization
	mechanisms. This can be an extremely large performance gain, especially in
	operating systems.

	Disadvantages

	Tagged pointers have some of the same difficulties as xor linked lists,
	although to a lesser extent. For example, not all debuggers will be able
	to properly follow tagged pointers; however, this is not an issue for a
	debugger that is designed with tagged pointers in mind.

	The use of 0 to represent a null pointer does not suffer from these disadvantages:
	it is pervasive, and most programming languages treat 0 as a special null value.
	It has thoroughly proven its robustness. Usually when "tagged pointers" are spoken
	of, this common use is excluded.

	In 32-bit machines, most datums are aligned on 4-byte boundaries (including virtually all objects),
	so the two least significant bits of pointers are always zero (and can be used for other purposes).
	Of course, SW may need to mask off those bits before dereferencing any such pointer. (11)bin=(3)oct
	In 64-bit machines, this range expands to the maximum of 16 bits. (1111111111111111)bin=(177777)oct
*/

/*
	The following functions can be efficiently with assembly instructions to improve the speed.
	For example:
		tst R1, 0 ;; test least significant bit (LSB)
*/
static inline void *TaggedPointer_Smuggle(void *ptr, Int32 value)
{
	//return (void *)( (long long)ptr | (value & 3) ); // The value 177777 is for 64-bit architectures
	return (void *)( (Long)ptr | (value & 3) ); // The value 3 is for 32-bit architectures
}
static inline Int32 TaggedPointer_RecoverData(void *ptr)
{
	//return (long long)ptr & 177777; //the value 177777 is for 64-bit architectures
	return (Long)ptr & 3; // The value 3 is for 32-bit architectures
}
static inline void *TaggedPointer_RecoverPointer(void *ptr)
{
	//return (void *)( (long long)ptr & (~177777) ); //the value 177777 is for 64-bit architectures
	return (void *)( (Long)ptr & (~3) ); // The value 3 is for 32-bit architectures
}
/* Get the last bit of the LSB */
static inline bool is_marked_reference(void *ptr)
{
	return TaggedPointer_RecoverData(ptr) & 1;
}
/* Set the last bit of the LSB */
static inline void *get_marked_reference(void *ptr)
{
	return TaggedPointer_Smuggle(ptr, 1);
}
/* Clear the last bit of the LSB */
static inline void *get_unmarked_reference(void *ptr)
{
	return TaggedPointer_RecoverPointer(ptr);
}
//----------------------------------------------------
// Get the string representation of the v with respect to the base in the range of either 2, 8 or 16.
static string convBase(ULong v, Long base)
{
	static const string digits = "0123456789abcdef";
	string result;
	if((base < 2) || (base > 16)) {
		result = "Error: base out of range.";
	}
	else {
		do {
			result = digits[v % base] + result;
			v /= base;
		}
		while(v);
	}
	return result;
}
//----------------------------------------------------

#endif
