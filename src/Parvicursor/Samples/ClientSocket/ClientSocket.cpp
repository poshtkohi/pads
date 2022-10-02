// ClientSocket.cpp : Defines the entry point for the console application.
//

//---------------------------------------
#include "../../Parvicursor/general.h"
#include "../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../Parvicursor/System.Net.Sockets/Socket/Socket.h"
#include "../../Parvicursor/System.Net/Dns/Dns.h"
//---------------------------------------
using namespace System;
using namespace System::Net;
using namespace System::Net::Sockets;
//---------------------------------------
//#include <ws2tcpip.h>

int main(int argc, char* argv[])
{
	//IPHostEntry hostEntry1 = Dns::Resolve("www.poshtkohi.ir");
	// connect to www.example.com port 80 (http)

	/*struct addrinfo hints, *res;
	int sockfd;

	// first, load up address structs with getaddrinfo():
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;  // use IPv4 or IPv6, whichever
	//hints.ai_socktype = SOCK_STREAM;

	int retVal;
	// we could put "80" instead on "http" on the next line:
	if ((retVal = ::getaddrinfo("www.google.com", null, &hints, &res)) != 0)
	{
		printf("getaddrinfo() failed.\n");
	}

	// make a socket:

	sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	struct addrinfo *current = res;

	while(current != null)
	{
		printf("Address length: %d\n", current->ai_addrlen);

		struct sockaddr_in  *sockaddr_ipv4 = (struct sockaddr_in *) current->ai_addr;
		printf("IPv4 address %s\n", inet_ntoa(sockaddr_ipv4->sin_addr) );

		current = current->ai_next;
	}
	// connect it to the address and port we passed in to getaddrinfo():

	//connect(sockfd, res->ai_addr, res->ai_addrlen);

	::freeaddrinfo(res);

	return 0;*/

	Socket *client = null;
	IPAddress ip;
	IPHostEntry *hostEntry = null;
	try
	{
		// Finds the IP addresses associated with the domain.
		hostEntry = Dns::Resolve("localhost");
		// Prints all of the found IP addresses into console.
		for(Int32 i = 0 ; i < hostEntry->get_AddressListLength() ; i++)
		{
			ip = hostEntry->get_AddressList(i);
			printf("IP Address %d: %s\n", i , inet_ntoa(*(struct in_addr *)ip.GetAddressBytes()));
		}

		// Instantiates the Socket object for a TCP-based connection-oriented transport.
		client = new Socket(System::Net::Sockets::InterNetwork, System::Net::Sockets::Stream, System::Net::Sockets::tcp);

		// Sets up the remote endpoint to connect on port 80.
		ip = hostEntry->get_AddressList(0);
		IPEndPoint inp = IPEndPoint(ip, 80);
		// Connects to the remote web server.
		client->Connect(inp);

		const Int32 bufferSize = 256*1024;
		char buffer[bufferSize + 1];

		String request = "GET / HTTP/1.1\r\nHost: www.poshtkohi.ir\r\nConnection: close\r\n\r\n";
		printf("Client said to server:\n\n%s\n", request.get_BaseStream());
		// Sends the request to get the HTTP page from the web server.
		client->Send(request.get_BaseStream(), 0, request.get_Length(), System::Net::Sockets::None);

		// Read the entire response from the web server until the end of the connection is reached.
		Int32 n, read = 0;
		while((n = client->Receive(buffer, read, bufferSize - read, System::Net::Sockets::None)) > 0)
			read += n;

		// Prints the buffer into console if any data was received.
		if(read > 0)
		{
			// Formats the buffer to indicate the received response as an ASCII string.
			buffer[read] = '\0';
			printf("Server said to client:\n\n%s\n", buffer);
		}
	}
	catch(SocketException &e)
	{
		printf("Error occured. Exception message: %s\n", e.get_Message().get_BaseStream());
		printf("ErrorCode: %d\n", e.get_ErrorCode());
	}
	catch(ObjectDisposedException &e)
	{
		printf("Error occured. Exception message: %s\n", e.get_Message().get_BaseStream());
	}
	catch(Exception &e)
	{
		printf("Error occured. Exception message: %s\n", e.get_Message().get_BaseStream());
	}

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

	if(hostEntry != null)
		delete hostEntry;

	return 0;
}
//---------------------------------------

