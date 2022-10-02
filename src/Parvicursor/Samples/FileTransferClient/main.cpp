// FileTransferClient.cpp : Defines the entry point for the console application.
//

//---------------------------------------
#include "../../Parvicursor/general.h"
#include "../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../Parvicursor/System.Net.Sockets/Socket/Socket.h"
#include "../../Parvicursor/System.Net/Dns/Dns.h"
#include "../../Parvicursor/System.IO/FileStream/FileStream.h"
#include "../../Parvicursor/System/String/String.h"
//---------------------------------------
using namespace System;
using namespace System::IO;
using namespace System::Net;
using namespace System::Net::Sockets;
//---------------------------------------
bool CheckServerResponseForError(Socket *client)
{
	// Checks whether there was a server-side error during the file transfer session.
	char errorOccured = false;
	if(client->Receive(&errorOccured, 0, sizeof(char), System::Net::Sockets::None) > 0)
	{
		if(errorOccured)
			printf("There was a server-side error during the file transfer session.\n");
	}
	else
	{
		printf("The server has been closed the connection, and thus we could not determine if there is a server-side error during the file transfer session.\n");
		return true;
	}

	return (bool)errorOccured;
}
//---------------------------------------
int main(int argc, char* argv[])
{
	Socket *client = null;
	FileStream *fsRead = null;
	//String localFilename = "c:/test.pdf";
	//String remoteFilename = "c:/test1.pdf";
    String localFilename = "/root/projects/test.pdf";
	String remoteFilename = "/root/projects/test1.pdf";
	const Int32 bufferSize = 256 * 1024;
	char buffer[bufferSize];
	try
	{
		// Opens the local file to read.
		fsRead = new FileStream(localFilename, System::IO::Open, System::IO::Read, 8*1024);

		// Finds the IP address associated with the domain.
		IPHostEntry hostEntry = Dns::Resolve2("localhost");
		// Instantiates the Socket object for a TCP-based connection-oriented transport.
		client = new Socket(System::Net::Sockets::InterNetwork, System::Net::Sockets::Stream, System::Net::Sockets::tcp);

		// Sets up the remote endpoint to connect on port 3128.
		IPAddress ip = hostEntry.get_AddressList(0);
		IPEndPoint inp = IPEndPoint(ip, 3128);
		// Connects to the remote file server.
		client->Connect(inp);

		// Sends the remote file name to be created by server.
		client->Send(remoteFilename.get_BaseStream(), 0, remoteFilename.get_Length(), System::Net::Sockets::None);

		// Checks whether there was a server-side error during the file transfer initiation.
		if(CheckServerResponseForError(client))
			goto Cleanup;
		printf("The file transfer session was started.\n");

		// Reads a file block from fsRead.
		Int32 read, sent, total;
		while( (read = fsRead->Read(buffer, 0, bufferSize)) > 0 )
		{
			total = 0;
			// Tries to send all read content within the buffer until all bytes are sent.
			while( (read - total) > 0 )
			{
				sent = client->Send(buffer, total, read - total, System::Net::Sockets::None);
				total += sent;
			}
		}

		printf("The file transfer session was successfully completed.\n");

		// Checks whether there was a server-side error during the file transfer session.
		/*if(!CheckServerResponseForError(client))
			printf("The file '%s' was successfully uploaded to the remote server.\n", localFilename.get_BaseStream());
		else
			goto Cleanup;*/
	}
	catch(SocketException &e)
	{
		printf("Error occurred. Exception message: %s\n", e.get_Message().get_BaseStream());
		printf("ErrorCode: %d\n", e.get_ErrorCode());
	}
	catch(ObjectDisposedException &e)
	{
		printf("Error occurred. Exception message: %s\n", e.get_Message().get_BaseStream());
	}
	catch(Exception &e)
	{
		printf("Error occurred. Exception message: %s\n", e.get_Message().get_BaseStream());
	}

Cleanup:
	// Closes and releases the Socket instance.
	if(client != null)
	{
		try
		{
			client->Shutdown(Both);
			client->Close();
		}
		catch(...){}
		delete client;
	}

	// Closes and releases the FileStream instance.
	if(fsRead != null)
	{
		try {fsRead->Close();} catch(...){}
		delete fsRead;
	}

	return 0;
}
//---------------------------------------
