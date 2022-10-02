// Serialization.cpp : Defines the entry point for the console application.
//

//---------------------------------------
#include "../../Parvicursor/general.h"
#include "../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../Parvicursor/System/String/String.h"
#include "../../Parvicursor/Parvicursor/Serialization/Serializer.h"
#include "../../Parvicursor/Parvicursor/Serialization/DeSerializer.h"
//---------------------------------------
using namespace System;

using namespace Parvicursor::Serialization;
//---------------------------------------

int main(int argc, char* argv[])
{
	/////// Serializer
	int a = 10;
	int b = 30;
	String str = "Hello World!";
	int c = 40;
	Serializer se(1024);
	se.Write<int>(a);
	se.Write<int>(b);
	se.Write(str);
	se.Write<int>(c);
	printf("a: %d, b: %d, str: %s, c: %d\n", a, b, str.get_BaseStream(), c);


	/////// DeSerializer
	int _a = -1;
	int _b = -1;
	String _str;
	int _c = -1;
	DeSerializer de(se.get_BaseBuffer(), se.get_BaseBufferSize());
	_a = de.Read<int>();
	_b = de.Read<int>();
	_str = de.Read();
	_c = de.Read<int>();
	printf("_a: %d, _b: %d, _str: %s, _c: %d\n", _a, _b, _str.get_BaseStream(), _c);


	//se.Reset();
	//de.Reset();

	return 0;
}
//---------------------------------------
