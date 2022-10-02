// FileTransferServer.cpp : Defines the entry point for the console application.
//

//---------------------------------------
#include "../../Parvicursor/general.h"
#include "../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../Parvicursor/System/String/String.h"
#include "../../Parvicursor/System.Net/Dns/Dns.h"
#include "../../Parvicursor/System.Net.Sockets/Socket/Socket.h"
#include "../../Parvicursor/System.IO/FileStream/FileStream.h"
#include "../../Parvicursor/System.Threading/Thread/Thread.h"
//---------------------------------------
using namespace System;
using namespace System::IO;
using namespace System::Net;
using namespace System::Net::Sockets;
using namespace System::Threading;
//---------------------------------------
// Represents a client context created by acceptor thread and passed to ProcessClient().
class ClientContext : public Object
{
	// The buffer to transfer data between the client connection, and file system and etc.
	public: char *buffer;
	// Indicates the size of the buffer in bytes.
	public: Int32 bufferSize;
	// The client Socket instance.
	public: Socket *sock;
	// The local FileStream instance to write the received data from the client.
	public: FileStream *fsWrite;
	// The local server-side file name requested by the client.
	public: String writeFilename;
	private: bool disposed;
	// The ClientContext constructor.
	public: ClientContext(Socket *acceptedSocket)
	{
		sock = acceptedSocket;
		// Default buffer size.
		bufferSize = 256 * 1024;
		// We must allocate the buffer from heap, because a concurrent server servicing a large 
		// number of clients cannot allocate the buffer from the stack due to the stack limit size.
		buffer = (char *)::malloc(bufferSize * sizeof(char));
		fsWrite = null;
		disposed = false;
	}
	// The ClientContext deconstructor.
	public: ~ClientContext()
	{
		if(disposed)
			return ;

		// Closes and releases the FileStream instance.
		if(fsWrite != null)
		{
			try { fsWrite->Close(); }
			catch (...){}
			delete fsWrite;
		}

		// Closes and releases client connection.
		if(sock != null)
		{
			try { sock->Shutdown(Both); sock->Close(); }
			catch (...){}
			delete sock;
		}

		// Deallocates the buffer.
		::free(buffer);
	}
};
//---------------------------------------
// The worker's function pointer to handle a new accepted connection.
void *ProcessClient(void *arg)
{
	// A type-casting to caste the arg into a ClientContext object.
	ClientContext *cx = (ClientContext *)arg;
	char errorOccured = false;

	try
	{
		// Reads the writeFilename from the client.
		Int32 read = cx->sock->Receive(cx->buffer, 0, 256, System::Net::Sockets::None);
		if(read <= 0)
			goto Cleanup;

		// Builds the writeFilename string from the buffer.
		cx->buffer[read] = '\0';
		cx->writeFilename = String((const char *)cx->buffer);

		// Instantiates a FileStream instance to which the write operation will be performed.
		cx->fsWrite = new FileStream(cx->writeFilename, System::IO::OpenOrCreate, System::IO::Write, 8*1024);

		// Notifies the client that it can now begin the transfer flow.
		try { cx->sock->Send(&errorOccured, 0, sizeof(char), System::Net::Sockets::None); }
		catch(...) {}

		// Receives the file blocks sent by the client and writes them into the fsWrite object.
		// The while() loop executes until the end of file represented by the client by closing the 
		// connection. In this stage, Receive() returns 0.
		read = 0;
		while( (read = cx->sock->Receive(cx->buffer, 0, cx->bufferSize, System::Net::Sockets::None)) > 0 )
			cx->fsWrite->Write(cx->buffer, 0, read);
	}
	catch(SocketException &e)
	{
		// This kind of exception indicates that the socket could not be used anymore, and then 
		// we must jump to the 'Cleanup' label.
		printf("Error occurred. SocketException message: %s\n", e.get_Message().get_BaseStream());
		goto Cleanup;
	}
	catch(IOException &e)
	{
		printf("Error occurred. IOException message: %s\n", e.get_Message().get_BaseStream());
		errorOccured = true;
	}
	catch(Exception &e)
	{
		printf("Error occurred. Exception message: %s\n", e.get_Message().get_BaseStream());
		errorOccured = true;
	}

	// Notifies the client whether there is an error during the file transfer session.
	/*try { cx->sock->Send(&errorOccured, 0, sizeof(char), System::Net::Sockets::None); }
	catch(...) {}*/

Cleanup:
	// Frees the cx instance created by acceptor thread.
	delete cx;
	return arg;
}
//---------------------------------------
int main(int argc, char* argv[])
{
	// Creates the server socket (connectioen-oriented and TCP/IP-enabled).
	Socket *server = new Socket(System::Net::Sockets::InterNetwork, System::Net::Sockets::Stream, System::Net::Sockets::tcp);
	// We will listen on port 3128 and all network interfaces.
	IPEndPoint hostEndPoint = IPEndPoint(IPAddress::get_Any(), 3128);
	// Binds the server socket to the hostEndPoint.
	server->Bind(hostEndPoint);
	// Listens on the server socket with the 'backlog; set to 100 concurrent connections.
	server->Listen(100);
	printf("The file server is listening on port 3128.\n");

	// The main acceptor thread's loop.
	while(true)
	{
		// Blocks on the server socket until an incoming connection arrives.
		Socket *s = server->Accept();
		if(s == null)
			continue;

		printf("New client connection was accepted.\n");

		// Allocates the client context, creates a worker thread, passes the cx to that worker, and
		// starts the worker to serve the client request. The ProcessClient() function will serves the client.
		ClientContext *cx = new ClientContext(s);
		Thread thread = Thread(ProcessClient, (void *)cx);
		thread.Start();
		thread.SetDetached();
	}

	return 0;
}
//---------------------------------------

