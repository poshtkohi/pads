All rights reserved to Alireza Poshtkohi (C) 1999-2022.
Email: arp@poshtkohi.info
Website: http://www.poshtkohi.info
_______________________________________________________________________________________________
Presented Parvicursor.NET Samples:

In Windows, run all source codes by using Microsoft Visual Studio .NET 2003 or later versions and at Linux 
by using  g++-enabled-compiler Code::Blocks.

1. ClientSocket sample shows a simple scenario in which a client connects to a HTTP server and requests the its first page on a typical domain.

2. Concurrency samples show various examples to deal with concurrency based on Parvicursor.NET framework.
   These rich samples are described throughout chapter 3. It involves 8 examples, see them in contained folder.
   
3. DiskIO samples include two advanced examples to work with concurrent file I/O and directory operations.
These two samples are covered in chapter 3.

4. FileStream sample shows a file copy scenario from target to destination file system via using the well-known and good-featured System.IO.FileStream class.

5. GraphSample works with graph algorithms to find closed loops in a graph in order to determine whether a hierarchy
	of mutexes form a deadlock situation or not. This sample is discussed in chapter 3.
6. LockFree samples provide a rich set of practical examples for lock-free data structures. We teach them in chapter 3.

7. RecursiveDirectoryTreeTraversal sample presents the full implemented features of .NET System.IO.File and System.IO.Directory class methods. This example traverses all directories under a specified path and lists all files on storage system recursively.

8. Serialization sample shows Parvicursor.NET serilazation/deserilization tools.

9. Threading sample shows use of a similar declaration of .NET Framework threads by utilizing the POSIX Threads on Linux and its port to Windows by pthreads.dll dynamic link library.

10. Timer sample shows a System.Threading.Timer class usage.
	
12. CloneThread example describe how to create threads except for POSIX threads in Linux OS with using the clone() system call.
	Also, we tell the readers how to implement an efficient mutual exclusion primitive like mutexes relied on 
	Linux futex() system call.
	
13. AsynchEchoClient/AsynchEchoServer implements two concurrent network servers based on advanced Parvicursor.NET Asynchronous Sockets (XDAS)
	for chapter 4. Parvicursor.NET sockets are .NET Asynchronous equivalent implemented atop Linux Epoll facility and Parvicursor.NET framework.
	XDAS also provide a firm library that can be easily ported to KQUEUE in UNIX and Windows Completion Ports (IOCP)
	in Windows. It is aimed to develop highly concurrent client-server applications specifically for HPC, Cloud Computing and Internet
	applications. These two sample are only compiled and run in Linux.
	
14. HttpProxyserver implements a HTTP proxy server that supports HTTP GET/PUT/CONNECT methods. It has been developed 
	based on XDAS as a practical example to show how we make use of the Parvicursor.NET framework to develop a highly concurrent
	network server.  This example is only compiled and run in Linux.
	
15.	xThreadSampleDLL, xThreadAddNumbersDispatcher, xThreadPiCalcDLL and xThreadPiClacDispatcher shows
	various examples for xThread use. These samples show various examples to distribute threads 
	over a network of computers based on Parvicursor platform. We will describe them during chapter 12.
_______________________________________________________________________________________________