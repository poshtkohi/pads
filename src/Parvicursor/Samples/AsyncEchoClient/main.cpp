
// AsyncEchoClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../AsyncSocketLib/AsyncSocket/AsyncSocket.h"

using namespace System::Net::Sockets;
//---------------------------------------
// State object for reading/writing client data asynchronously
class StateObject : public Object
{
	// Client  socket.
	public: AsyncSocket *workSocket;
	// Size of send/receive buffer.
	public: const static Int32 BufferSize = 1024;
	// Send buffer.
	public: char sendBuffer[BufferSize];
	// Receive buffer.
	public: char receiveBuffer[BufferSize];
	public: Int32 len;
	public: bool hasException;
	public: StateObject()
	{
	    hasException = false;
    }
};
// Asynchronous callbaks.
static void OnConnected(IAsyncResult *ar);
static void OnReceive(IAsyncResult *ar);
static void OnSent(IAsyncResult *ar);
// Indicate whether the clients have any error occurred.
static bool hasExceptionClient1 = false;
static bool hasExceptionClient2 = false;
// The following parameters are used to synchronize between the main thread and the execution of asynchronous requests.
static Mutex mutex = Mutex();
static ConditionVariable cv = ConditionVariable(&mutex);
static Int32 conditionNum = 0;
//---------------------------------------
//static Long volatile __StopRequested__ = false;
static void StopRequested_signal_handler(int sig)
{
    printf("\nCTRL+C was pressed.\n");
    printf("The client program is terminating.\nPlease wait ...\n");
    //SetBoolFlagAtomic(&__StopRequested__, true); // Atomic operations are allowed to execute within interrupt handlers.
    //cv.Signal(); // This operation is valid becuase we used the 'cv' without any accompanying mutex. Thus, there will not be any deadlock point.
    exit(0);
    // my_signal(SIGINT, StopRequested_signal_handler);
}
//---------------------------------------
int main(int argc, char* argv[])
{
    my_signal(SIGINT, StopRequested_signal_handler);

    printf("The client program was started 1.\n");

    // Sets up the asynchronous client mode runtime.
    AsyncSocket::InitClientModeRuntime();

    // Establish the remote endpoint for sockets.
    // The name of the remote device is "localhost".
    IPHostEntry ipHostInfo = Dns::Resolve2("localhost");
    IPAddress ipAddress = ipHostInfo.get_AddressList(0);
    IPEndPoint remoteEP = IPEndPoint(ipAddress, 9000);

while(true)
{
    AsyncSocket *client1 = new AsyncSocket(InterNetwork, Stream, tcp);
    AsyncSocket *client2 = new AsyncSocket(InterNetwork, Stream, tcp);
    Int32 one = 1;
    setsockopt(client1->get_BaseSocket()->get_Handle(), SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    setsockopt(client2->get_BaseSocket()->get_Handle(), SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

	// Create the state object.
	StateObject *state1 = new StateObject();
	state1->workSocket = client1;
	StateObject *state2 = new StateObject();
	state2->workSocket = client2;

    conditionNum = 0;

    // Connect to the remote endpoints.
    try
    {
        client1->BeginConnect(remoteEP, OnConnected, state1, true);
        client2->BeginConnect(remoteEP, OnConnected, state2, true);
    }
    catch(SocketException &e)
    {
        printf("Error occurred in OnConnected. Exception message: %s\n", e.get_Message().get_BaseStream());
        return -2;
    }
    catch(...)
    {
        printf("Error occurred in OnConnected. Unknown exception.\n");
        return -3;
    }

	// Waits until the two client connect.
    mutex.Lock();
    while(conditionNum != 2)
        cv.Wait();
    mutex.Unlock();

    /*if(GetBoolFlagAtomic(&__StopRequested__))
        goto lable;*/

    const char *str = "Alireza Poshtkohi";
	Int32 len = strlen(str) + 1; // 1 for '\0' (the end of string)
	memmove(state1->sendBuffer, str, len);
	state1->len = len;
    memmove(state2->sendBuffer, str, len);
	state2->len = len;

    if(state1->hasException || state2->hasException)
        goto Cleanup;

    //for(register Int32 i = 1 ; i <= 1 ; i++)
    //{

        printf("conditionNum: %d\n", conditionNum);//
        conditionNum = 0;
        //printf("I: %d\n", i);

        client1->BeginSend(state1->sendBuffer, 0, state1->len, System::Net::Sockets::None, OnSent, state1);
        client2->BeginSend(state2->sendBuffer, 0, state2->len, System::Net::Sockets::None, OnSent, state2);

		// Waits until the two send operations complete.
        mutex.Lock();
        while(conditionNum != 2)
            cv.Wait();
        mutex.Unlock();


        if(state1->hasException || state2->hasException)
           goto Cleanup;

        printf("Client1 Cores: %d CoreID: %d sendBuffer: %s receiveBuffer: %s\n", client1->get_CoreCount(), client1->get_CoreID(), state1->sendBuffer, state1->receiveBuffer);
        printf("Client2 Cores: %d CoreID: %d sendBuffer: %s receiveBuffer: %s\n", client2->get_CoreCount(), client2->get_CoreID(), state2->sendBuffer, state2->receiveBuffer);

Cleanup:
        //client1->print();
        //client2->print();

        client1->Close();
        delete client1;
        delete state1;
        client2->Close();
        delete client2;
        delete state2;
    }
    return 0;
        /*printf("Client1 Cores: %d CoreID: %d sendBuffer: %s receiveBuffer: %s\n", client1->get_CoreCount(), client1->get_CoreID(), state1->sendBuffer, state1->receiveBuffer);
        printf("Client2 Cores: %d CoreID: %d sendBuffer: %s receiveBuffer: %s\n", client2->get_CoreCount(), client2->get_CoreID(), state2->sendBuffer, state2->receiveBuffer);

        // Swaps cores.
        Int32 client1_CurrentCore = client1->get_CoreID();
        Int32 client2_CurrentCore = client2->get_CoreID();
        if(client1_CurrentCore != client2_CurrentCore)
        {
            printf("Tries to swap client cores ...\n");

            bool success = client1->TryMigrateToCore(client2_CurrentCore);
            if(success)
                printf("client1 was swapped.\n");
            else
                printf("client1 was not swapped.\n");

            success = client2->TryMigrateToCore(client1_CurrentCore);
            if(success)
                printf("client2 was swapped.\n");
            else
                printf("client2 was not swapped.\n");
        }
    }*/

//Cleanup:

    printf("The client program is terminating.\nPlease wait ...\n");

	// Here, we should also close all accepted conncetions. For example, you could
	// store a reference to every client connection into an array list
	// and close them altogheter in a loop.The client program was started.
    /*client1->Close();
	delete client1;
	delete state1;
    client2->Close();
	delete client2;
	delete state2;*/

    // Disposes the asynchronous client mode runtime.
	AsyncSocket::DestroyClientModeRuntime();

	return 0;
}
//---------------------------------------
void OnConnected(IAsyncResult *ar)
{
    // Retrieve the state object and the handler socket
    // from the asynchronous state object.
    StateObject *state = (StateObject *) ar->AsyncState;
    AsyncSocket *handler = state->workSocket;

    //printf("OnConnected() 1.\n");
    printf("OnConnected()\n");
    try
    {
        //printf("OnConnected() 2.\n");
        // Completes the connection.
        handler->EndConnect(ar);
        printf("Socket connected.\n");
    }
    catch(Exception &e)
    {
        state->hasException = true;
        printf("Error occurred in OnConnected. Exception message: %s\n", e.get_Message().get_BaseStream());
    }
    catch(...)
    {
        state->hasException = true;
        printf("Error occurred in OnConnected. Unknown exception.\n");
    }

    // Signals the main thread to continue.
	mutex.Lock();
	conditionNum++;
	cv.Signal();
	mutex.Unlock();
	return;
}
//---------------------------------------
void OnReceive(IAsyncResult *ar)
{
    // Retrieves the state object and the handler socket from the asynchronous state object.
    StateObject *state = (StateObject *) ar->AsyncState;
    AsyncSocket *handler = state->workSocket;

    // Reads data from the client socket.
    Int32 bytesRead;
    try
    {
        bytesRead = handler->EndReceive(ar);
        if(bytesRead <= 0)
        {
            printf("Error occurred in OnReceive. The AsyncSocket has been closed.\n");
            mutex.Lock();
            conditionNum++;
            cv.Signal();
            mutex.Unlock();
            return ;
        }
    }
    catch(Exception &e)
    {
        state->hasException = true;
        printf("Error occurred in OnReceive. Exception message: %s\n", e.get_Message().get_BaseStream());
        mutex.Lock();
        conditionNum++;
        cv.Signal();
        mutex.Unlock();
        return ;
    }
    catch(...)
    {
        state->hasException = true;
        printf("Error occurred in OnReceive. Unknown exception.\n");
        mutex.Lock();
        conditionNum++;
        cv.Signal();
        mutex.Unlock();
        return ;
    }

    if(bytesRead > 0)
    {
        printf("bytesRead: %d\n", bytesRead);
        mutex.Lock();
        conditionNum++;
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
    // Retrieves the state object and the handler socket from the asynchronous state object.
    StateObject *state = (StateObject *) ar->AsyncState;
    AsyncSocket *handler = state->workSocket;

    // Completes sending the data to the remote device.
    Int32 bytesSent;
    try
    {
        bytesSent = handler->EndSend(ar);
    }
    catch(Exception &e)
    {
        state->hasException = true;
        printf("Error occurred in OnSent. Exception message: %s\n", e.get_Message().get_BaseStream());
        mutex.Lock();
        conditionNum++;
        cv.Signal();
        mutex.Unlock();
        return ;
    }
    catch(...)
    {
        state->hasException = true;
        printf("Error occurred in OnSent. Unknown exception.\n");
        mutex.Lock();
        conditionNum++;
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
