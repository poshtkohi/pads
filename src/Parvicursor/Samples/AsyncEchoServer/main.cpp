// AsyncEchoServer.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include "../../AsyncSocketLib/AsyncSocket/AsyncSocket.h"
#include "../../Parvicursor/System.Threading/Mutex/Mutex.h"
#include "../../Parvicursor/System.Threading/ConditionVariable/ConditionVariable.h"

using namespace System;
using namespace System::Threading;
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
	public: char buffer[BufferSize];
	// Size of received buffer.
	public: Int32 n_read;
	public: Int32 n_written;
};
// Asynchronous callbaks.
static void OnAccept(IAsyncResult *ar);
static void OnReceive(IAsyncResult *ar);

static void OnSent(IAsyncResult *ar);
// The following parameters are used to synchronize between the main thread and the execution of asynchronous requests.
static Mutex mutex = Mutex();
static ConditionVariable cv = ConditionVariable(&mutex);
static bool conditionMet = false;
//---------------------------------------
static Long volatile __StopRequested__ = false;
static void StopRequested_signal_handler(int sig)
{

    printf("\nCTRL+C was pressed.\n");
    printf("The server is terminating.\nPlease wait ...\n");
    //SetBoolFlagAtomic(&__StopRequested__, true); // Atomic operations are allowed to execute within interrupt handlers.
    //cv.Signal(); // This operation is valid becuase we used the 'cv' without any accompanying mutex. Thus, there will not be any deadlock point.
    // my_signal(SIGINT, StopRequested_signal_handler);
    exit(0);
}
//---------------------------------------
int main(int argc, char* argv[])
{
    my_signal(SIGINT, StopRequested_signal_handler);

    printf("Service was started.\n");

    AsyncSocket *listener = new AsyncSocket(InterNetwork, Stream, tcp);
    IPEndPoint hostEndPoint = IPEndPoint(IPAddress::get_Any(), 9000);
    //listener.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, 1);
    //int one = 1;
    //setsockopt(listener.get_Handle(), SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    listener->Bind(hostEndPoint);
    listener->Listen(1000);

	while(!GetBoolFlagAtomic(&__StopRequested__))
	{
	    conditionMet = false;
		// Start an asynchronous socket to listen for connections.
		printf("Waiting for a connection...\n");
		listener->BeginAccept(OnAccept, listener, true);

		// Wait until a connection is made before continuing.
        mutex.Lock();
        while(!conditionMet)
            cv.Wait();
        mutex.Unlock();
	}

    printf("The server is terminating.\nPlease wait ...\n");
	listener->Close();
	delete listener;

	// Here, we should also close all accepted conncetions. For example, you could
	// store a reference to every client connection into an array list
	// and close them altogheter in a loop.

	return 0;
}
//---------------------------------------
void OnAccept(IAsyncResult *ar)
{
    // Signals the main thread to continue.
	mutex.Lock();
	conditionMet = true;
	cv.Signal();
	mutex.Unlock();

	// Gets the socket that handles the client request.
	AsyncSocket *listener = (AsyncSocket *)ar->AsyncState;
	AsyncSocket *handler = listener->EndAccept(ar);

	printf("new connection accepted.\n");

	// Creates the state object.
	StateObject *state = new StateObject();
	state->workSocket = handler;
	state->n_read = 0;
	state->n_written = 0;
	handler->BeginReceive(state->buffer, 0, state->BufferSize, System::Net::Sockets::None, OnReceive, state);

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
            printf("Error occured in OnReceive. The AsyncSocket has been closed\n");
            handler->Close();
            delete handler; // This has been already allocated via invoking EndAccept();
            delete state;
            return ;
        }
    }
    catch(Exception &e)
    {
        printf("Error occured in OnReceive. Exception message: %s\n", e.get_Message().get_BaseStream());
        handler->Close();
        delete handler; // This has been already allocated via invoking EndAccept();
        delete state;
        return ;
    }
    catch(...)
    {
        printf("Error occured in OnReceive. Unknown exception.\n");
        handler->Close();
        delete handler; // This has been already allocated via invoking EndAccept();
        delete state;
        return ;
    }

    if(bytesRead > 0)
    {

        printf("CoreID: %d bytesRead: %d\n", handler->get_CoreID(), bytesRead);
        state->n_read = bytesRead;
        // Echos the data back to the client.
        handler->BeginSend(state->buffer, 0, bytesRead, System::Net::Sockets::None, OnSent, state);
    }
    else
        handler->BeginReceive(state->buffer, 0, state->BufferSize, System::Net::Sockets::None, OnReceive, state);

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
        printf("Error occured in OnSent. Exception message: %s\n", e.get_Message().get_BaseStream());
        handler->Close();
        delete handler; // This has been already allocated via invoking EndAccept();
        delete state;
        return ;
    }
    catch(...)
    {
        printf("Error occurred in OnSent. Unknown exception.\n");
        handler->Close();
        delete handler; // This has been already allocated via invoking EndAccept();
        delete state;
        return ;
    }

    if(bytesSent > 0)
    {
        printf("CoreID: %d bytesSent: %d\n", handler->get_CoreID(), bytesSent);
        state->n_written += bytesSent;
        Int32 remaining = state->n_read - state->n_written;
        if(remaining > 0)
            handler->BeginSend(state->buffer, state->n_written, remaining, System::Net::Sockets::None, OnSent, state);
        else
        {
            state->n_read = 0;
            state->n_written = 0;
            handler->BeginReceive(state->buffer, 0, state->BufferSize, System::Net::Sockets::None, OnReceive, state);
        }
    }
    else
        handler->BeginSend(state->buffer, 0, state->n_read, System::Net::Sockets::None, OnSent, state);

    return;
}
//---------------------------------------
