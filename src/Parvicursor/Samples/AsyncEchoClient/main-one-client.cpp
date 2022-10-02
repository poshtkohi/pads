 
// AsynchIOTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../AsynchIOTest/AsyncSocket/AsyncSocket.h"

using namespace System::Net::Sockets;
//---------------------------------------
// State object for reading/writing client data asynchronously
class StateObject : public Object
{
	// Client  socket.
	public: AsyncSocket *workSocket;
	// Size of receive buffer.
	public: const static Int32 BufferSize = 1024;
	// Receive buffer.
	public: char sendBuffer[BufferSize];
	public: char receiveBuffer[BufferSize];
	public: Int32 len;
};
static void OnConnected(IAsyncResult *ar);
static void OnReceive(IAsyncResult *ar);
static void OnSent(IAsyncResult *ar);
static Mutex mutex = Mutex();
static ConditionVariable cv = ConditionVariable(&mutex);
static bool volatile hasException = false;
static bool conditionMet = false;
//---------------------------------------
static Long volatile __StopRequested__ = false;
static void StopRequested_signal_handler(int sig)
{
    printf("\nCTRL+C was pressed.\n");
    SetBoolFlagAtomic(&__StopRequested__, true); // Atomic operations are allowed to execute within interrupt handlers.
    cv.Signal(); // This operation is valid becuase we used the 'cv' without any accompanying mutex. Thus, there will not be any deadlock point.
    // my_signal(SIGINT, StopRequested_signal_handler);
    return;
}
//---------------------------------------
int main(int argc, char* argv[])
{
    my_signal(SIGINT, StopRequested_signal_handler);

    printf("The client program was started.\n");


    // Establish the remote endpoint for the socket.
    // The name of the
    // remote device is "localhost".
/*for(int i = 0 ; i < 40 ; i++)
{
    printf("try: %d\n", i);*/

    AsyncSocket::InitClientModeRuntime(); // Sets up the asynchronous client mode runtime.

    IPHostEntry ipHostInfo = Dns::Resolve("localhost");
    IPAddress ipAddress = ipHostInfo.get_AddressList(0);
    IPEndPoint remoteEP = IPEndPoint(ipAddress, 9000);

    AsyncSocket *client = new AsyncSocket(InterNetwork, Stream, tcp);
    int one = 1;
    setsockopt(client->get_BaseSocket()->get_Handle(), SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

	// Create the state object.
	StateObject *state = new StateObject();
	state->workSocket = client;

    conditionMet = false;

    // Connect to the remote endpoint.
    client->BeginConnect(remoteEP, OnConnected, state);

    mutex.Lock();
    while(!conditionMet)
        cv.Wait();
    mutex.Unlock();

    /*if(GetBoolFlagAtomic(&__StopRequested__))
        goto lable;*/

    if(hasException)
        return -1;

    const char *str = "Alireza Poshtkohi";
	Int32 len = strlen(str) + 1; // 1 for '\0' (the end of string)
	memmove(state->sendBuffer, str, len);
	state->len = len;

for(Int32 i = 1 ; i <= 100 ; i++)
{
    conditionMet = false;
    printf("I: %d\n", i);
	client->BeginSend(state->sendBuffer, 0, state->len, System::Net::Sockets::None, OnSent, state);

    mutex.Lock();
    while(!conditionMet)
        cv.Wait();
    mutex.Unlock();


    if(!hasException)
        printf("CoreID: %d sendBuffer: %s\nreceiveBuffer: %s\n", client->get_CoreID(), state->sendBuffer, state->receiveBuffer);
    else
        break;
}

    printf("The client program is terminating.\nPlease wait ...\n");

    client->Close();
	delete client;
	delete state;

	AsyncSocket::DestroyClientModeRuntime(); // Disposes the asynchronous client mode runtime.
//}
	// Here, we should also close all accepted conncetions. For example, you could
	// store a reference to every client connection into an array list
	// and close them altogheter in a loop.
	return 0;
}
//---------------------------------------
void OnConnected(IAsyncResult *ar)
{
    // Retrieve the state object and the handler socket
    // from the asynchronous state object.
    StateObject *state = (StateObject *) ar->AsyncState;
    AsyncSocket *handler = state->workSocket;

    // Signal the main thread to continue.
	/*mutex.Lock();
	cv.Signal();
	mutex.Unlock();*/

    try
    {
        // Complete the connection.
        handler->EndConnect(ar);
        printf("Socket connected.\n");
    }
    catch(Exception &e)
    {
        hasException = true;
        printf("Error occurred in OnConnected. Exception message: %s\n", e.get_Message().get_BaseStream());
        mutex.Lock();
        conditionMet = true;
        cv.Signal();
        mutex.Unlock();
        return;
    }
    catch(...)
    {
        hasException = true;
        printf("Error occurred in OnConnected. Unknown exception.\n");
        mutex.Lock();
        conditionMet = true;
        cv.Signal();
        mutex.Unlock();
        return;
    }

    // Signal the main thread to continue.
	mutex.Lock();
	conditionMet = true;
	cv.Signal();
	mutex.Unlock();
	return;
	/*if(ar !=  null)
	{
        7 ((AcceptedInfo *)ar); // deletes handler and internal allocated ar data structures.
	}*/
}
//---------------------------------------
void OnReceive(IAsyncResult *ar)
{
    // Retrieve the state object and the handler socket
    // from the asynchronous state object.
    StateObject *state = (StateObject *) ar->AsyncState;
    AsyncSocket *handler = state->workSocket;

    // Read data from the client socket.
    Int32 bytesRead;
    try
    {
        bytesRead = handler->EndReceive(ar);
    }
    catch(Exception &e)
    {
        hasException = true;
        printf("Error occurred in OnReceive. Exception message: %s\n", e.get_Message().get_BaseStream());
        mutex.Lock();
        conditionMet = true;
        cv.Signal();
        mutex.Unlock();
        return ;
    }
    catch(...)
    {
        hasException = true;
        printf("Error occurred in OnReceive. Unknown exception.\n");
        mutex.Lock();
        conditionMet = true;
        cv.Signal();
        mutex.Unlock();
        return ;
    }

    if(bytesRead > 0)
    {
        printf("bytesRead: %d\n", bytesRead);
        mutex.Lock();
        conditionMet = true;
        cv.Signal();
        mutex.Unlock();
        return;
    }
    else
        handler->BeginReceive(state->receiveBuffer, 0,  state->len, System::Net::Sockets::None, OnReceive, state);

    return;
}
//---------------------------------------
void OnSent(IAsyncResult *ar)
{
    // Retrieve the state object and the handler socket
    // from the asynchronous state object.
    StateObject *state = (StateObject *) ar->AsyncState;
    AsyncSocket *handler = state->workSocket;

    // Complete sending the data to the remote device.
    Int32 bytesSent;
    try
    {
        bytesSent = handler->EndSend(ar);
    }
    catch(Exception &e)
    {
        hasException = true;
        printf("Error occurred in OnSent. Exception message: %s\n", e.get_Message().get_BaseStream());
        mutex.Lock();
        conditionMet = true;
        cv.Signal();
        mutex.Unlock();
        return ;
    }
    catch(...)
    {
        hasException = true;
        printf("Error occurred in OnSent. Unknown exception.\n");
        mutex.Lock();
        conditionMet = true;
        cv.Signal();
        mutex.Unlock();
        return ;
    }

    printf("bytesSent: %d\n", bytesSent);

    if(bytesSent > 0)
        handler->BeginReceive(state->receiveBuffer, 0, state->len, System::Net::Sockets::None, OnReceive, state);
    else
        handler->BeginSend(state->sendBuffer, 0, state->len, System::Net::Sockets::None, OnSent, state);

    return;
}
//---------------------------------------
