
// DiskIO.cpp : Defines the entry point for the console application.
//

//------------------------------------------------------------------------------
#include "DiskIO/ConcurrentFileCopy/ConcurrentFileCopy.h"
#include "DiskIO/ConcurrentCP/ConcurrentCP.h"
//------------------------------------------------------------------------------
using namespace DiskIO;
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	//return DiskIO::ConcurrentFileCopy::Parvicursor_main(argc, argv);
	return DiskIO::ConcurrentCP::Parvicursor_main(argc, argv);
}
//------------------------------------------------------------------------------
