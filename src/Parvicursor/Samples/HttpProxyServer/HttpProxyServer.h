#ifndef __Samples_HttpProxyServer_h__
#define __Samples_HttpProxyServer_h__


#include "../../Parvicursor/System/Object/Object.h"
#include "../../Parvicursor/System.Threading/Mutex/Mutex.h"
#include "../../AsyncSocketLib/AsyncSocket/AsyncSocket.h"

using namespace System;
using namespace System::Threading;
using namespace System::Net::Sockets;
using namespace LockFree;

//static LockFreeCounter counter = LockFreeCounter();
//extern Int32 _connection_num_ = 0;
//static Mutex *_lock_ = new Mutex();
//**************************************************************************************************************//
namespace Samples
{
    namespace AsyncSockets
    {
        //----------------------------------------------------
        #define __BadRequest__ 0
        #define __GatewayTimeout__ 1

        struct ErrorStruct
        {
            Int32 number;
            const char message[24];
            const char description[200];
        };

        static ErrorStruct errorCodes [2] =
        {
           { // 0
            400,
            "Bad Request",
            "The request could not be understood by the proxy server due to malformed syntax."
           },

           { // 1
            502,
            "Gateway Timeout",
            "The proxy did not receive a timely response from the upstream server specified by the URI or some other auxiliary server (e.g. DNS) it needed to access in attempting to complete the request."
           }

        };
        //----------------------------------------------------
        enum MethodType
        {
            __Unknown = -1, // Unknown HTTP Request
            __GET = 0,      // HTTP GET Request
            __POST = 1,     // HTTP POST Request
            __CONNECT = 2   // HTTP CONNECT Request
        };
        //----------------------------------------------------
        #define __BUFFER_SIZE__ 8192 + 1 // 8KB.
        // Session object for reading/writing client/server data asynchronously
        class Session : public Object
        {
            // Client buffer.
            public: char *clientBuffer;
            // Server buffer.
            public: char *serverBuffer;
            // Size of send/receive buffer.
            public: Int32 bufferSize;
            // Size of client rereceive buffer.
            public: Int32 clientRecv;
            // Size of server rereceive buffer.
            public: Int32 serverRecv;
            // The client socket.
            public: AsyncSocket *clientSocket;
            // The server socket.
            public: AsyncSocket *serverSocket;
            public: IPHostEntry *ipHostInfo;
            public: bool serverClosed;
            public: bool clientClosed;
            public: bool isFirstTimeHttpRequest;
            // HTTP request method type (GET/POST/CONNECT).
            public: MethodType methodType;
            private: bool disposed;
            // The hostname requested by client in the HTTP GET/PUT/CONNECT methods.
            public: char *hostname;
            // lastHostname is used to support keep-alive connections for HTTP/1.1 GET/PUT methods.
            public: char *lastHostname;
            //public: char *pathname;
            public: bool KeepAlive;
            public: Int32 port;
            // This flag is used for notification whether a large GET request teminating with '\r\n\r\n'
            // is taking palce or not. If yes, we must read the next chunk from the client socket
            // to complete the request.
            public: bool hasGetMethodCompleted;
            public: static const Int32 MaxHostnameLen = 256; // 256 bytes.
            // Stores the ContentLength with respect to the POST method request by client.
            ///public: Int64 PostContenLength;
            // A variable to store dynamic trasnsferred bytes of the content for POST request method by client.
            ///public: Int64 TransferredPostContentBytes;
            // A mutex is used to protect a session object among threads.
            public: Mutex *mutex;
            //public: static const Int32 MaxPathNameLen = 1 * 1024; // 1KB
            //---------------------------------------
            public: Session(Int32 bufferSize)
            {
                if(bufferSize <= 0)
                    bufferSize = 4 * 1024;

                clientBuffer = (char *)::malloc((bufferSize + 1) * sizeof(char)); // 1 is for '\0' that indicates the end of a string.
                if(clientBuffer == null)
                {
                    printf("malloc failure\n");
                    exit(-1);
                }
                serverBuffer = (char *)::malloc((bufferSize + 1) * sizeof(char)); // 1 is for '\0' that indicates the end of a string.
                if(serverBuffer == null)
                {
                    printf("malloc failure\n");
                    exit(-1);
                }
                hostname = (char *)::malloc((MaxHostnameLen + 1) * sizeof(char)); // 1 is for '\0' that indicates the end of a string.
                if(hostname == null)
                {
                    printf("malloc failure\n");
                    exit(-1);
                }
                lastHostname = (char *)::malloc((MaxHostnameLen + 1) * sizeof(char)); // 1 is for '\0' that indicates the end of a string.
                if(lastHostname == null)
                {
                    printf("malloc failure\n");
                    exit(-1);
                }
               /*pathname = (char *)::malloc(MaxPathNameLen * sizeof(char));
                if(pathname == null)
                {
                    printf("malloc failure\n");
                    exit(-1);
                }*/
                methodType = __Unknown;
                this->bufferSize = bufferSize;
                clientSocket = null;
                serverSocket = null;
                disposed = false;
                KeepAlive = false;
                serverClosed = true;
                clientClosed = true;
                hasGetMethodCompleted = false;
                isFirstTimeHttpRequest = true;
                mutex = new Mutex();
                ipHostInfo = null;
                //counter.Increment();
            }
            //---------------------------------------
            public: ~Session()
            {
                if(!disposed)
                {
                    /*if(clientSocket != null)
                    {
                        try { clientSocket->Close(); } catch(...) {}
                        delete clientSocket;
                        clientSocket = null;
                    }

                    if(serverSocket != null)
                    {
                        try { serverSocket->Close(); } catch(...) {}
                        delete serverSocket;
                        serverSocket = null;
                    }*/
                    ::free(clientBuffer);
                    ::free(serverBuffer);
                    ::free(hostname);
                    ::free(lastHostname);
                    if(ipHostInfo != null)
                        delete ipHostInfo;
                    if(serverSocket != null)
                        delete serverSocket;
                    if(clientSocket != null)
                        delete clientSocket;
                    if(mutex != null)
                        delete mutex;
                    //printf("\nSession deleted\n");
                    //counter.Decrement();
                    //::free(pathname);
                    disposed = true;
                }
            }
        };
        //----------------------------------------------------
        class HttpProxyServer : public Object
        {
            // The proxy listener socket.
            private: AsyncSocket *listener;
            // The port on which the proxy is listening.
            private: Int32 listenPort;
            private: bool disposed;
            // Indicates whether the proxy has already been started or not.
            private: bool hasStarted;
            // The following mutex, cv and accpted variables are used to synchronize between the main
            // thread and acceptor thread (in EndAccept).
            private: Mutex *accepted_mutex;
            private: ConditionVariable *accepted_cv;
            private: bool accepted;
            private: Long volatile StopRequested;
            //---------------------------------------
            public: HttpProxyServer(Int32 listenPort)
            {
                if(listenPort <= 0)
                    listenPort = 8080;

                this->listenPort = listenPort;
                listener = null;
                accepted_mutex = null;
                accepted_cv = null;
                hasStarted = false;
                disposed = false;
                StopRequested = false;
                accepted = false;
            }
            //---------------------------------------
            public: ~HttpProxyServer()
            {
                if(!disposed)
                {
                    if(listener != null)
                    {
                        try { listener->Close(); } catch(...) {}
                        delete listener;
                    }

                    if(hasStarted)
                    {
                        SetBoolFlagAtomic(&StopRequested, true);

                        if(accepted_cv != null)
                        {
                            accepted_cv->Signal();
                            delete accepted_cv;
                        }

                        if(accepted_mutex != null)
                            delete accepted_mutex;

                        // Disposes the asynchronous client mode runtime.
                        AsyncSocket::DestroyClientModeRuntime();
                    }

                    disposed = false;
                }
            }
            //---------------------------------------
            // Starts the proxy server.
            public: void Start()
            {
                if(!disposed && !hasStarted)
                {
                    printf("\nThe HTTP proxy service was started and is listening on port %d.\nProxy-Agent: Parvicursor.NET-AsyncProxy-v1\n\n", listenPort);

                    hasStarted = true;
                    accepted_mutex = new Mutex();
                    accepted_cv = new ConditionVariable(accepted_mutex);

                    // Sets up the asynchronous client mode runtime.
                    AsyncSocket::InitClientModeRuntime();

                    listener = new AsyncSocket(InterNetwork, Stream, tcp);
                    IPEndPoint hostEndPoint = IPEndPoint(IPAddress::get_Any(), listenPort);
                    //listener.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, 1);
                    int one = 1;
                    ::setsockopt(listener->get_BaseSocket()->get_Handle(), SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
                    listener->Bind(hostEndPoint);
                    listener->Listen(1000); //

                    //listener->BeginAccept(OnAccept, this, true);
                    //Thread::Sleep(100000000);
                    //while(true/*!GetBoolFlagAtomic(&StopRequested)*/)
                    /*{
                        accepted_mutex->Lock();
                        accepted = false;
                        accepted_mutex->Unlock();
                        // Start an asynchronous socket to listen for connections.
                        ////printf("Waiting for a connection...\n");
                        listener->BeginAccept(OnAccept, this, true);

                        // Wait until a connection is made before continuing.
                        accepted_mutex->Lock();
                        while(!accepted)
                            accepted_cv->Wait();
                        accepted_mutex->Unlock();
                    }*/

                    listener->BeginAccept(OnAccept, this, true);

                    // Wait until a connection is made before continuing.
                    accepted_mutex->Lock();
                    while(!StopRequested)
                        accepted_cv->Wait();
                    accepted_mutex->Unlock();
                }
            }
            //---------------------------------------
            ///<summary>Called when there's an incoming client connection waiting to be accepted.</summary>
            ///<param name="ar">The result of the asynchronous operation.</param>
            private: static void OnAccept(IAsyncResult *ar)
            {
                //DeletionPhaseForMarkedSockets();
                //printf("Current connections: %d\n", counter.get_Count());

                HttpProxyServer *proxy = (HttpProxyServer *)ar->AsyncState;
                //printf("OnAccept\n");

                // Signal the Start() thread to continue.
                /*proxy->accepted_mutex->Lock();
                proxy->accepted = true;
                proxy->accepted_cv->Signal();
                proxy->accepted_mutex->Unlock();*/

                // Get the socket that handles the client request.
                AsyncSocket *clientSocket = proxy->listener->EndAccept(ar);
                ////printf("accepted socket: %d\n", clientSocket->get_BaseSocket()->get_Handle());//

                ////printf("new connection accepted.\n");

                Session *session = new Session(__BUFFER_SIZE__);
                session->clientSocket = clientSocket;
                session->clientClosed = false;
                session->serverClosed = true;

                try
                {
                    session->clientSocket->BeginReceive(session->clientBuffer, 0, session->bufferSize, System::Net::Sockets::None, OnReceiveQuery, session);
                }
                catch(Exception &e)
                {
                    ////printf("Error occured in OnAccept() 1. Exception message: %s\n", e.get_Message().get_BaseStream());
                    Dispose(session, true);
                    return;
                }
                catch(...)
                {
                    ////printf("Error occured in OnAccept() 2. Unknown exception.\n");
                    Dispose(session, true);
                    return;
                }
            }
            //---------------------------------------
            ///<summary>Parses a specified query and modifies to the HTTP request.</summary>
            private: static bool ParseRequest(InOut Session *session)
            {
                if(session == null)
                    return false;
                if(session->clientBuffer == null)
                    return false;

                // Parse the HTTP METHOD request.
                char *begin = session->clientBuffer;
                char *end = null;
                Int32 methodLen = 0;
                if(strncmp(begin, "GET ", 4) == 0)
                {
                    methodLen = 4;
                    begin += 4;
                    session->methodType = __GET;
                }
                else if(strncmp(begin, "POST ", 5) == 0)
                {
                    methodLen = 5;
                    begin += 5;
                    session->methodType = __POST;
                }
                else if(strncmp(begin, "CONNECT ", 8) == 0)
                {
                    // Finds hostname, e.g., CONNECT www.example.com:443 HTTP/1.1
                    methodLen = 8;
                    begin += 8;
                    session->methodType = __CONNECT;
                    char *end;
                    if((end = strstr(begin, " HTTP/")) == null)
                        return false;

                    char *delimiter;
                    if((delimiter = strchr(begin, ':')) == null)
                        return false;
                    delimiter++; // for ':'

                    Int32 hostnameSize = (Int32)(delimiter - begin);
                    if(hostnameSize > Session::MaxHostnameLen - 1)
                        return false;
                    memmove(session->hostname, begin, hostnameSize);
                    session->hostname[hostnameSize - 1] = '\0';

                    Int32 portSize = (Int32)(end - delimiter);
                    if(portSize > 5)
                        return false;
                    char port_str[6]; // maximum port number is 65536 + 1 for '\0'
                    port_str[portSize - 1] = '\0';
                    memmove(port_str, delimiter, portSize);
                    session->port = atoi(port_str);
                    return true;
                }
                else
                    return false;

                if(session->methodType == __GET || session->methodType == __POST)
                {
                    // GET http://ut.ac.ir/Templates/Main/Images/rtl/RightCornerUnderImageItems.jpg HTTP/1.1
                    // Finds pathname and protocol version.
                    begin += 7;
                    if((end = strchr(begin, '/')) != null)
                    {
                        char *pathnameBegin = begin - 7;
                        char *pathnameEnd = end;
                        Int32 transferLen = end - begin + 7;
                        begin = end;
                        if((end = strstr(begin, " HTTP/")) != null)
                        {
                            /*Int32 len = (Int32)(end - begin);
                            if(len < Session::MaxPathNameLen)
                            {
                                memcpy(session->pathname, begin, len);
                                session->pathname[len] = '\0';
                            }
                            // Finds protocol version.
                            begin = end;
                            begin += 6; // 6 is for strlen(" HTTP/");
                            if((end = strstr(begin, "\r\n")) != null)
                            {
                                len = (Int32)(end - begin);
                                if(len <= 3) // 1.1 and 1.0
                                {
                                    char version [4];
                                    memcpy(version, begin, 3);
                                    version[len] = '\0';
                                    session->version = (float)atof(version);
                                }
                            }*/

                            //printf("pathname: %s prtocol version: %.1f\n", session->pathname, session->version);
                            // Modifies line 1.
                            memmove(pathnameBegin, pathnameEnd, session->clientRecv - transferLen);
                            pathnameBegin -= methodLen;
                            session->clientBuffer = pathnameBegin;
                            session->clientRecv -= transferLen;
                            //printf("pathnameBegin: %s\n", pathnameBegin);
                        }
                    }

                    // Removes Proxy-xxx
                    /*if((begin = strstr(session->clientBuffer, "Proxy-Connection: ")) != null)
                    {
                        if((end = strstr(begin, "\r\n")) != null)
                        {
                            Int32 len = end - begin;
                            end += 2;
                            len += 2;
                            printf("clientRecv: %d begin: %d end: %d len: %d\n", session->clientRecv , (Int32)begin, (Int32)end, len);
                            memmove(begin, end,  ((Int32)session->clientBuffer + session->clientRecv) - (Int32)end);
                            session->clientRecv -= len;
                            //printf("buffer: %s\n", session->clientBuffer);
                        }
                    }*/

                    if((begin = strstr(session->clientBuffer, "Proxy-")) != null)
                    {
                        // Proxy-Connection: Keep-Alive
                        // Proxy-Connection: Close
                        if(strncasecmp(begin, "Proxy-Connection: keep-alive", 28) == 0)
                            session->KeepAlive = true;
                        else if(strncasecmp(begin, "Proxy-Connection: Keep-Alive", 28) == 0)
                                session->KeepAlive = true;
                        else
                            session->KeepAlive = false;

                        end = begin;
                        end += 6; // strlen("Proxy-");
                        //printf("clientRecv: %d begin: %d end: %d len: %d\n", session->clientRecv , (Int32)begin, (Int32)end, len);
                        memmove(begin, end,  ((Long)session->clientBuffer + session->clientRecv) - (Long)end);
                        session->clientRecv -= 6;
                        //printf("buffer: %s\n", session->clientBuffer);
                    }
                }

                begin = session->clientBuffer;
                // Finds hostname, e.g., Host: www.example.com:8000
                if((begin = strstr(begin, "Host: ")) == null)
                    return false;
                begin += 6;

                end = null;
                if((end = strstr(begin, "\r\n")) == null) // Finds the end of the current line.
                    if((end = strstr(begin, "\n")) == null)
                        return false;


                // printf("begin: %s\n", begin);
                char *delimiter = null;
                delimiter = strchr(begin, ':');
                if(delimiter == null || delimiter > end)
                {
                    end++;
                    delimiter = end;
                    session->port = 80;
                }
                else
                {
                    //end--;
                    delimiter++; // for ':'
                    Int32 portSize = (Int32)(end - delimiter);
                    if(portSize > 5)
                        return false;
                    char port_str[6]; // maximum port number is 65536 + 1 for '\0'
                    port_str[portSize - 1] = '\0';
                    memmove(port_str, delimiter, portSize);
                    session->port = atoi(port_str);
                    if(session->port > 65536)
                        return false;
                    //printf("port: %d\n", session->port);//
                }

                Int32 hostnameSize = (Int32)(delimiter - begin);
                if(hostnameSize > Session::MaxHostnameLen - 1)
                    return false;
                memmove(session->hostname, begin, hostnameSize);
                session->hostname[hostnameSize - 1] = '\0';


                ///if(session->methodType == __POST) // for Content-Length
                ///{
                ///
                //}
                return true;
                /*Int32 lines = 0;
                char *line = req;

                while(true)
                {
                    if((line = strstr(line, "\r\n")) == null) // Carries to the next line.
                    {
                        if(lines == 0)
                            return false;
                        else
                            break;
                    }
                    line += 2; // the lenghth of \r\n
                    lines++;
                }*/
            }
            //----------------------------------------------------
            private: static void OnClientClose(IAsyncResult *ar)
            {
                if(ar == null)
                    return;

                Session *session = (Session *)ar->AsyncState;

                try { session->clientSocket->EndClose(ar); } catch(Exception &e) { } catch(...) { }

                session->mutex->Lock();
                session->clientClosed = true;
                if(session->serverClosed)
                {
                    session->mutex->Unlock();
                    delete session;
                    /*_lock_->Lock();
                    _connection_num_++;
                    _lock_->Unlock();*/
                    return;
                }
                session->mutex->Unlock();
            }
            //----------------------------------------------------
            private: static void OnServerClose(IAsyncResult *ar)
            {
                if(ar == null)
                    return;

                Session *session = (Session *)ar->AsyncState;


                try { session->serverSocket->EndClose(ar); } catch(Exception &e) { } catch(...) { }

                session->mutex->Lock();
                session->serverClosed = true;
                if(session->clientClosed)
                {
                    session->mutex->Unlock();
                    delete session;
                    /*_lock_->Lock();
                    _connection_num_++;
                    _lock_->Unlock();*/
                    return;
                }
                session->mutex->Unlock();
            }
            //----------------------------------------------------
            ///<summary>Disposes of the resources (other than memory) used by the Client.</summary>
            ///<remarks>Closes the connections with the local client and the remote host. Once Dispose() has been called, this object should not be used anymore.</remarks>
            private: static void Dispose(Session *session, bool clientMode)
            {
                if(session == null)
                    return;

                //session->clientSocket->BeginClose(OnClientClose, session);
                //session->serverSocket->BeginClose(OnServerClose, session);
                //printf("clientMode: %d\n", clientMode);
                //session->mutex->Lock();

                if(!session->clientClosed)
                {
                    /*if(!clientMode)
                    {
                        struct linger L;
                        L.l_onoff = 1;
                        L.l_linger = 0;
                        setsockopt(session->clientSocket->get_BaseSocket()->get_Handle(), SOL_SOCKET, SO_LINGER, (char *) &L, sizeof(L));
                    }*/
                    session->clientSocket->BeginClose(OnClientClose, session);
                }
                if(!session->serverClosed)
                {
                    /*if(clientMode)
                    {
                        struct linger L;
                        L.l_onoff = 1;
                        L.l_linger = 0;
                        setsockopt(session->serverSocket->get_BaseSocket()->get_Handle(), SOL_SOCKET, SO_LINGER, (char *) &L, sizeof(L));
                    }*/
                    session->serverSocket->BeginClose(OnServerClose, session);
                }

                //session->mutex->Unlock();

                return ;
            }
            //----------------------------------------------------
            ///<summary>Called when we received some data from the client connection.</summary>
            ///<param name="ar">The result of the asynchronous operation.</param>
            private: static void OnReceiveQuery(IAsyncResult *ar)
            {
                Session *session = (Session *)ar->AsyncState;

                try
                {
                    Int32 Ret = session->clientSocket->EndReceive(ar);
                    if(Ret <= 0)
                    {
                        ////printf("Error occured in OnReceiveQuery() 1. The AsyncSocket has been closed.\n");
                        Dispose(session, true);
                        return;
                    }
                    if(session->methodType == __GET && !session->hasGetMethodCompleted) // for large GET requests
                    {
                        ////printf("helllllllo1\n");
                        session->clientRecv += Ret;
                        session->clientBuffer[session->clientRecv] = '\0';
                        goto UncompletedGetMethod;
                    }
                    else
                        session->clientRecv = Ret;
                }
                catch(Exception &e)
                {
                    ////printf("Error occured in OnReceiveQuery() 2. Exception message: %s\n", e.get_Message().get_BaseStream());
                    Dispose(session, true);
                    return;
                }
                catch(...)
                {
                    ////printf("Error occured in OnReceiveQuery() 3. Unknown exception.\n");
                    Dispose(session, true);
                    return;
                }

                session->clientBuffer[session->clientRecv] = '\0';

                if(!ParseRequest(session))
                {
                    ////printf("xxxxxxhelllllllo2 client said:\n%s\n", session->clientBuffer);
                    SendErrorToClientAndDispose(session, &errorCodes[__BadRequest__]);
                    return;
                }
                session->clientBuffer[session->clientRecv] = '\0';

UncompletedGetMethod:
                if(session->methodType == __GET)
                {
                    bool found = false;
                    if(strstr(session->clientBuffer, "\r\n\r\n") != null)
                        found = true;
                    else if(strstr(session->clientBuffer, "\n\n") != null)
                        found = true;
                    if(found)
                    {
                        ////printf("helllllllo3\n");
                        session->hasGetMethodCompleted = true;
                        goto Continue;
                    }
                    else
                    {
                        ////printf("helllllllo4\n");
                        session->hasGetMethodCompleted = false;
                        // Here, we must read the remaining content of the GET request from client.
                        try
                        {
                            session->clientSocket->BeginReceive(session->clientBuffer, session->clientRecv, session->bufferSize - session->clientRecv, System::Net::Sockets::None, OnReceiveQuery, session);
                            return;
                        }
                        catch(Exception &e)
                        {
                            ////printf("Error occured in OnConnected() 5. Exception message: %s\n", e.get_Message().get_BaseStream());
                            Dispose(session, true);
                            return ;
                        }
                        catch(...)
                        {
                            ////printf("Error occured in OnConnected() 6. Unknown exception.\n");
                            Dispose(session, true);
                            return ;
                        }
                    }
                }

            Continue:

                ////printf("client said:\n%s\n\n", session->clientBuffer);
                if(session->methodType != __CONNECT && !session->isFirstTimeHttpRequest && session->KeepAlive)
                {
                    if(strcmp(session->lastHostname, session->hostname) == 0) // HTTP/1.1 Keep-Alive Support
                    {
                        ////printf("HTTP/1.1 Keep-Alive Support\n");
                        try
                        {
                            session->serverSocket->BeginSend(session->clientBuffer, 0, session->clientRecv, System::Net::Sockets::None, OnQuerySent, session);
                        }
                        catch(Exception &e)
                        {
                            ////printf("Error occured in OnReceiveQuery() 3. Exception message: %s\n", e.get_Message().get_BaseStream());
                            Dispose(session, false);
                            return;
                        }
                        catch(...)
                        {
                            ////printf("Error occured in OnReceiveQuery() 4. Unknown exception.\n");
                            Dispose(session, false);
                            return;
                        }
                        return;
                    }
                }

                //session->mutex->Lock();
                if(!session->serverClosed)
                {

                    /*struct linger L;
                    L.l_onoff = 1;
                    L.l_linger = 0;
                    setsockopt(session->serverSocket->get_BaseSocket()->get_Handle(), SOL_SOCKET, SO_LINGER, (char *) &L, sizeof(L));*/
                    session->serverSocket->Close();
                    delete session->serverSocket;
                    session->serverSocket = null;
                    session->serverClosed = true;
                }
                session->isFirstTimeHttpRequest = false;
                //session->mutex->Unlock();


                // Establish the remote endpoint for sockets.
                // The name of the remote device is "session->hostname".
                ////printf("resove1\n");
                if(session->ipHostInfo != null)
                {
                    delete session->ipHostInfo;
                    session->ipHostInfo = null;
                }
                try
                {
                    session->ipHostInfo = Dns::Resolve((const char*)session->hostname);
                }
                catch(...)
                {
                    SendErrorToClientAndDispose(session, &errorCodes[__GatewayTimeout__]);
                    return ;
                }
                ////printf("resove2\n");
                IPEndPoint remoteEP;
                try
                {
                    IPAddress ipAddress = session->ipHostInfo->get_AddressList(0);
                    ////printf("resove3\n");
                    remoteEP = IPEndPoint(ipAddress, session->port);
                    //session->mutex->Lock();
                    session->serverSocket = new AsyncSocket(InterNetwork, Stream, tcp);
                    session->serverClosed = false;
                    //session->mutex->Unlock();
                    /* Set the option active */
                    if(session->KeepAlive)
                    {
                        int one = 1;
                        ::setsockopt(session->serverSocket->get_BaseSocket()->get_Handle(), SOL_SOCKET, SO_KEEPALIVE, &one, sizeof(one));
                    }
                }
                catch(Exception &e)
                {
                    ////printf("Error occured in OnReceiveQuery() 5. Exception message: %s\n", e.get_Message().get_BaseStream());
                    //DisposeClient(session);
                    Dispose(session, true);
                    return ;
                }
                catch(...)
                {
                    ////printf("Error occured in OnReceiveQuery() 6. Unknown exception.\n");
                    //DisposeClient(session);
                    Dispose(session, true);
                    return ;
                }
                try
                {
                    /*int one = 1;
                    ::setsockopt(session->serverSocket->get_BaseSocket()->get_Handle(), SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));*/

                    session->serverSocket->BeginConnect(remoteEP, OnConnected, session, true);
                    ////printf("server socket: %d\n", session->serverSocket->get_BaseSocket()->get_Handle());//
                }
                catch(Exception &e)
                {
                    ////printf("Error occured in OnReceiveQuery() 7. Exception message: %s\n", e.get_Message().get_BaseStream());
                    SendErrorToClientAndDispose(session, &errorCodes[__GatewayTimeout__]);
                    return ;
                }
                catch(...)
                {
                    ////printf("Error occured in OnReceiveQuery() 8. Unknown exception.\n");
                    SendErrorToClientAndDispose(session, &errorCodes[__GatewayTimeout__]);
                    return ;
                }

            }
            //----------------------------------------------------
            ///<summary>Called when we're connected to the requested remote host.</summary>
            ///<param name="ar">The result of the asynchronous operation.</param>
            private: static void OnConnected(IAsyncResult *ar)
            {
                Session *session = (Session *)ar->AsyncState;

                try
                {
                    session->serverSocket->EndConnect(ar);
                    if(session->KeepAlive)
                        memmove(session->lastHostname, session->hostname, strlen(session->hostname) + 1); // Copies hostname into lastHostname.
                }
                catch(Exception &e)
                {
                    ////printf("Error occured in OnConnected() 1. Exception message: %s\n", e.get_Message().get_BaseStream());
                    SendErrorToClientAndDispose(session, &errorCodes[__GatewayTimeout__]);
                    return ;
                }
                catch(...)
                {
                    ////printf("Error occured in OnConnected() 2. Unknown exception.\n");
                    SendErrorToClientAndDispose(session, &errorCodes[__GatewayTimeout__]);
                    return ;
                }

                if(session->methodType == __CONNECT)
                { //HTTPS
                    ::sprintf(session->clientBuffer, "HTTP/1.%d  200 Connection established\r\nProxy-Agent: Parvicursor.NET-AsyncProxy-v1\r\n\r\n", 1);
                    ////printf("Successfully connected to %s on port %d.\n", session->hostname, session->port);
                    try
                    {
                        session->clientSocket->BeginSend(session->clientBuffer, 0, strlen(session->clientBuffer), System::Net::Sockets::None, OnOkSent, session);
                    }
                    catch(Exception &e)
                    {
                        ////printf("Error occured in OnConnected() 3. Exception message: %s\n", e.get_Message().get_BaseStream());
                        Dispose(session, true);
                        return ;
                    }
                    catch(...)
                    {
                        ////printf("Error occured in OnConnected() 4. Unknown exception.\n");
                        Dispose(session, true);
                        return ;
                    }

                }
                else
                { //Normal HTTP
                    ////printf("Successfully connected to %s on port %d.\n", session->hostname,session->port);
                    try
                    {
                        session->serverSocket->BeginSend(session->clientBuffer, 0, session->clientRecv, System::Net::Sockets::None, OnQuerySent, session);
                    }
                    catch(Exception &e)
                    {
                        ////printf("Error occured in OnConnected() 5. Exception message: %s\n", e.get_Message().get_BaseStream());
                        Dispose(session, false);
                        return ;
                    }
                    catch(...)
                    {
                        ////printf("Error occured in OnConnected() 6. Unknown exception.\n");
                        Dispose(session, false);
                        return ;
                    }
                }
            }
            //----------------------------------------------------
            ///<summary>Called when an OK reply has been sent to the local client.</summary>
            ///<param name="ar">The result of the asynchronous operation.</param>
            private: static void OnOkSent(IAsyncResult *ar)
            {
                Session *session = (Session *)ar->AsyncState;

                try
                {
                    Int32 Ret = session->clientSocket->EndSend(ar);
                    if(Ret == -1)
                    {
                        ////printf("Error occured in OnOkSent() 1.\n");
                        Dispose(session, true);
                        return;
                    }
                    //printf("OnOkSent() Ret: %d\n", Ret);
                }
                catch(Exception &e)
                {
                    ////printf("Error occured in OnOkSent() 2. Exception message: %s\n", e.get_Message().get_BaseStream());
                    Dispose(session, true);
                    return;
                }
                catch(...)
                {
                    ////printf("Error occured in OnOkSent() 3. Unknown exception.\n");
                    Dispose(session, true);
                    return;
                }

                StartRelay(session);
                return;
            }
            //----------------------------------------------------
            ///<summary>Called when the HTTP query has been sent to the remote host.</summary>
            ///<param name="ar">The result of the asynchronous operation.</param>
            private: static void OnQuerySent(IAsyncResult *ar)
            {
                Session *session = (Session *)ar->AsyncState;

                try
                {
                    if(session->serverSocket->EndSend(ar) == -1)
                    {
                        ////printf("Error occured in OnQuerySent() 1.\n");
                        Dispose(session, false);
                        return;
                    }
                    //printf("OnQuerySent\n");
                }
                catch(Exception &e)
                {
                    ////printf("Error occured in OnQuerySent() 2. Exception message: %s\n", e.get_Message().get_BaseStream());
                    Dispose(session, false);
                    return;
                }
                catch(...)
                {
                    ////printf("Error occured in OnQuerySent() 3. Unknown exception.\n");
                    Dispose(session, false);
                    return;
                }

                StartRelay(session);
                return;
            }
            //----------------------------------------------------
            ///<summary>Starts relaying data between the remote host and the local client.</summary>
            ///<remarks>This method should only be called after all protocol specific communication has been finished.</remarks>
            private: static void StartRelay(Session *session)
            {
                if(session == null)
                    return;

                if(session->methodType == __CONNECT)
                {
                    try
                    {
                        session->clientSocket->BeginReceive(session->clientBuffer, 0, session->bufferSize, System::Net::Sockets::None, OnClientReceive, session);
                    }
                    catch(Exception &e)
                    {
                        ////printf("Error occured in StartRelay() 1. Exception message: %s\n", e.get_Message().get_BaseStream());
                        Dispose(session, true);
                        return;
                    }
                    catch(...)
                    {
                        ////printf("Error occured in StartRelay() 2. Unknown exception.\n");
                        Dispose(session, true);
                        return;
                    }
                    try
                    {
                        session->serverSocket->BeginReceive(session->serverBuffer, 0, session->bufferSize, System::Net::Sockets::None, OnRemoteReceive, session);
                    }
                    catch(Exception &e)
                    {
                        ////printf("Error occured in StartRelay() 3. Exception message: %s\n", e.get_Message().get_BaseStream());
                        Dispose(session, false);
                        return;
                    }
                    catch(...)
                    {
                        ////printf("Error occured in StartRelay() 4. Unknown exception.\n");
                        Dispose(session, false);
                        return;
                    }
                }
                else if(session->methodType == __GET || session->methodType ==  __POST)
                {
                    try
                    {
                        session->clientSocket->BeginReceive(session->clientBuffer, 0, session->bufferSize, System::Net::Sockets::None, OnReceiveQuery, session);
                    }
                    catch(Exception &e)
                    {
                        ////printf("Error occured in StartRelay() 5. Exception message: %s\n", e.get_Message().get_BaseStream());
                        Dispose(session, true);
                        return;
                    }
                    catch(...)
                    {
                        ////printf("Error occured in StartRelay() 6. Unknown exception.\n");
                        Dispose(session, true);
                        return;
                    }
                    try
                    {
                        session->serverSocket->BeginReceive(session->serverBuffer, 0, session->bufferSize, System::Net::Sockets::None, OnRemoteReceive, session);
                    }
                    catch(Exception &e)
                    {
                        ////printf("Error occured in StartRelay() 7. Exception message: %s\n", e.get_Message().get_BaseStream());
                        Dispose(session, false);
                        return;
                    }
                    catch(...)
                    {
                        ////printf("Error occured in StartRelay() 8. Unknown exception.\n");
                        Dispose(session, false);
                        return;
                    }
                }
            }
            //----------------------------------------------------
            ///<summary>Called when we have received data from the local client. Incoming data will immediately be forwarded to the remote host.</summary>
            ///<param name="ar">The result of the asynchronous operation.</param>
            private: static void OnClientReceive(IAsyncResult *ar)
            {
                Session *session = (Session *)ar->AsyncState;

                try
                {
                    Int32 Ret = session->clientSocket->EndReceive(ar);
                    //printf("OnClientReceive Ret: %d clientBuffer: %s\n", Ret, session->clientBuffer);
                    if(Ret <= 0)
                    {
                        ////printf("Error occurred in OnClientReceive() 1. The AsyncSocket has been closed.\n");
                        Dispose(session, true);
                        return;
                    }
                    session->clientRecv = Ret;
                }
                catch(Exception &e)
                {
                    ////printf("Error occurred in OnQuerySent() 2. Exception message: %s\n", e.get_Message().get_BaseStream());
                    Dispose(session, true);
                    return;
                }
                catch(...)
                {
                    ////printf("Error occurred in OnQuerySent() 3. Unknown exception.\n");
                    Dispose(session, true);
                    return;
                }

                try
                {
                    session->serverSocket->BeginSend(session->clientBuffer, 0, session->clientRecv, System::Net::Sockets::None, OnRemoteSent, session);
                }
                catch(Exception &e)
                {
                    ////printf("Error occurred in OnClientReceive() 4. Exception message: %s\n", e.get_Message().get_BaseStream());
                    Dispose(session, false);
                    return;
                }
                catch(...)
                {
                    ////printf("Error occurred in OnClientReceive() 5. Unknown exception.\n");
                    Dispose(session, false);
                    return;
                }
            }
            //----------------------------------------------------
            ///<summary>Called when we have received data from the remote host. Incoming data will immediately be forwarded to the local client.</summary>
            ///<param name="ar">The result of the asynchronous operation.</param>
            private: static void OnRemoteReceive(IAsyncResult *ar)
            {
                Session *session = (Session *)ar->AsyncState;

                try
                {
                    Int32 Ret = session->serverSocket->EndReceive(ar);
                    //printf("OnRemoteReceive Ret: %d\n", Ret);
                    if(Ret <= 0)
                    {
                        ////printf("Error occurred in OnRemoteReceive() 1. The AsyncSocket has been closed.\n");
                        Dispose(session, false);
                        return;
                    }
                    //printf("\nServer said: %s\n", session->serverBuffer);
                    session->serverRecv = Ret;
                }
                catch(Exception &e)
                {
                    ////printf("Error occurred in OnRemoteReceive() 2. Exception message: %s\n", e.get_Message().get_BaseStream());
                    Dispose(session, false);
                    return;
                }
                catch(...)
                {
                    ////printf("Error occurred in OnRemoteReceive() 3. Unknown exception.\n");
                    Dispose(session, false);
                    return;
                }

                try
                {
                    session->clientSocket->BeginSend(session->serverBuffer, 0, session->serverRecv, System::Net::Sockets::None, OnClientSent, session);
                }
                catch(Exception &e)
                {
                    ////printf("Error occurred in OnRemoteReceive() 4. Exception message: %s\n", e.get_Message().get_BaseStream());
                    Dispose(session, true);
                    return;
                }
                catch(...)
                {
                    ////printf("Error occurred in OnRemoteReceive() 5. Unknown exception.\n");
                    Dispose(session, true);
                    return;
                }
            }
            //----------------------------------------------------
            ///<summary>Called when we have sent data to the remote host. When all the data has been sent, we will start receiving again from the local client.</summary>
            ///<param name="ar">The result of the asynchronous operation.</param>
            private: static void OnRemoteSent(IAsyncResult *ar)
            {
                Session *session = (Session *)ar->AsyncState;

                try
                {
                    Int32 Ret = session->serverSocket->EndSend(ar);
                    //printf("OnRemoteSent Ret: %d\n", Ret);
                    if(Ret <= 0)
                    {
                        ////printf("Error occurred in OnRemoteSent() 1.\n");
                        Dispose(session, false);
                        return;
                    }
                }
                catch(Exception &e)
                {
                    ////printf("Error occurred in OnRemoteSent() 2. Exception message: %s\n", e.get_Message().get_BaseStream());
                    Dispose(session, false);
                    return;
                }
                catch(...)
                {
                    ////printf("Error occurred in OnRemoteSent() 3. Unknown exception.\n");
                    Dispose(session, false);
                    return;
                }

                StartRelay(session);
                return;
            }
            //----------------------------------------------------
            ///<summary>Called when we have sent data to the local client. When all the data has been sent, we will start receiving again from the remote host.</summary>
            ///<param name="ar">The result of the asynchronous operation.</param>
            private: static void OnClientSent(IAsyncResult *ar)
            {
                Session *session = (Session *)ar->AsyncState;

                try
                {
                    Int32 Ret = session->clientSocket->EndSend(ar);
                    //printf("OnClientSent Ret: %d\n", Ret);
                    if(Ret <= 0)
                    {
                        ////printf("Error occurred in OnClientSent() 1.\n");
                        Dispose(session, true);
                        return;
                    }
                }
                catch(Exception &e)
                {
                    ////printf("Error occurred in OnClientSent() 2. Exception message: %s\n", e.get_Message().get_BaseStream());
                    Dispose(session, true);
                    return;
                }
                catch(...)
                {
                    ////printf("Error occurred in OnClientSent() 3. Unknown exception.\n");
                    Dispose(session, true);
                    return;
                }

                StartRelay(session);
                return;

            }
            //----------------------------------------------------
            ///<summary>Sends an error to the client.</summary>
            private: static void SendErrorToClientAndDispose(Session *session, ErrorStruct *error)
            {
                ::sprintf(session->clientBuffer, "HTTP/1.1 %d %s\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<html><head><title>Parvicursor.NET-AsyncProxy-v1: %d %s</title></head><body><div align=\"center\"><table border=\"0\" cellspacing=\"3\" cellpadding=\"3\" bgcolor=\"#C0C0C0\"><tr><td><table border=\"0\" width=\"500\" cellspacing=\"3\" cellpadding=\"3\"><tr><td bgcolor=\"#B2B2B2\"><p align=\"center\"><strong><font size=\"2\" face=\"Verdana\">Parvicursor.NET-AsyncProxy-v1: %d %s</font></strong></p></td></tr><tr><td bgcolor=\"#D1D1D1\"><font size=\"2\" face=\"Verdana\">%s</font></td></tr></table></center></td></tr></table></div></body></html>",
                                error->number, error->message,
                                error->number, error->message,
                                error->number, error->message,
                                error->description);

                try
                {
                    session->clientSocket->BeginSend(session->clientBuffer, 0, strlen(session->clientBuffer), System::Net::Sockets::None, OnErrorSent, session);
                }
                catch(Exception &e)
                {
                    ////printf("Error occurred in SendErrorToClientAndDispose() 1. Exception message: %s\n", e.get_Message().get_BaseStream());
                    Dispose(session, true);
                    return;
                }
                catch(...)
                {
                    ////printf("Error occurred in SendErrorToClientAndDispose() 2. Unknown exception.\n");
                    Dispose(session, true);
                }
                return;
            }
            //----------------------------------------------------
            ///<summary>Called when the Bad Request error has been sent to the client.</summary>
            ///<param name="ar">The result of the asynchronous operation.</param>
            private: static void OnErrorSent(IAsyncResult *ar)
            {
                Session *session = (Session *)ar->AsyncState;

                try
                {
                    session->clientSocket->EndSend(ar);
                }
                catch(Exception &e)
                {
                    ////printf("Error occurred in OnErrorSent() 1. Exception message: %s\n", e.get_Message().get_BaseStream());
                }
                catch(...)
                {
                    ////printf("Error occurred in OnErrorSent() 2. Unknown exception.\n");
                }

                Dispose(session, true);
            }
            //---------------------------------------
            private: static void *sock_proc(void *arg)
            {
                Int32 threadID = (Int32)(Long)arg;
                const Int32 max = 10;
                Socket **sockets = new Socket*[max];

                for(Int32 i = 0 ; i < max  ; i++)
                {
                    try{
                        sockets[i] = new Socket(InterNetwork, Stream, tcp);
                    }
                    catch(Exception &e)
                    {
                        ////printf("Exception message: %s\n", e.get_Message().get_BaseStream());
                        exit(0);
                    }
                }

                for(Int32 i = 0 ; i < max  ; i++)
                    ////printf("ThreadID: %d Socket: %d\n", threadID, sockets[i]->get_Handle());

                for(Int32 i = 0 ; i < max  ; i++)
                    delete sockets[i];

                return arg;
            }
            //---------------------------------------
            public: static int Parvicursor_main(int argc, char* argv[])
            {
                HttpProxyServer proxy = HttpProxyServer(8080);
                proxy.Start();
                //const char *request = "GET http://www.eskimo.com/~scs/cclass/notes/sx11a.html? HTTP/1.1\r\nHost: www.eskimo.com:403\r\nUser-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64; rv:14.0) Gecko/20100101 Firefox/14.0.1\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\nAccept-Language: en-us,en;q=0.5\r\nAccept-Encoding: gzip, deflate\r\nProxy-Connection: keep-alive\r\n\r\n";
                //const char *request = "CONNECT home.netscape.com:443 HTTP/1.0\r\nUser-agent: Mozilla/1.1\r\n\r\n";

                /*Session *session = new Session(64 * 1024);
                memmove(session->clientBuffer, request, strlen(request) + 1); // 1 for '\0'
                session->clientRecv = strlen(request);
                if(ParseRequest(session))
                {
                    printf("methodType: %d\n", session->methodType);
                    printf("hostname: %s port: %d\n", session->hostname, session->port);//
                    printf("buffer:\n%s", session->clientBuffer);
                }
                int getrlimit(int resource, struct rlimit *rlim);
                //printf("request:\n%s",  request);
                delete session; session = null;*/
                /*Thread **workers = new Thread*[2];
                for(Int32 i = 0 ; i < 2 ; i++)
                    workers[i] = new Thread(sock_proc, (void *)i);

                for(Int32 i = 0 ; i < 2 ; i++)
                {
                    workers[i]->Start();
                    //workers[i]->SetDetached();
                    workers[i]->Join();
                }*/

                //Thread::Sleep(10000000000);

                return 0;
            }
            //---------------------------------------
        };
        //----------------------------------------------------
    };
};
//**************************************************************************************************************//

#endif
