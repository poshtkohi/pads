/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "Socket.h"


//**************************************************************************************************************//
namespace System
{
	//----------------------------------------------------
   namespace Net
   {
	   namespace Sockets
	   {
		   //----------------------------------------------------
		   Socket::Socket(SOCKET s)
		   {
			   	//this->address_family = addressFamily;
				//this->socket_type = socketType;
				//this->protocol_type = protocolType;
				this->disposed = false;
				this->isListening = false;
				this->isConnected = false;
				this->isBound = false;
				this->isAccepted = true;
				this->sock = (Int32)s;
				//this->isConnected = false;
		   }
		   //----------------------------------------------------
		   Socket::Socket(SOCKET s, AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType)
		   {
			   	this->address_family = addressFamily;
				this->socket_type = socketType;
				this->protocol_type = protocolType;
				this->disposed = false;
				this->isListening = false;
				this->sock = (Int32)s;
		   }
		   //must consider exceptions
			//----------------------------------------------------
			Socket::Socket(AddressFamily addressFamily, SocketType socketType, ProtocolType protocolType)
			{
				this->address_family = addressFamily;
				this->socket_type = socketType;
				this->protocol_type = protocolType;
				this->disposed = false;
				this->isListening = false;
				this->isConnected = false;
				this->isAccepted = false;
				this->isBound = false;

#if defined WIN32 || WIN64

					if(!ws2_32_is_loaded)
					{
						WORD wVersionRequested;
						WSADATA wsaData;
						Int32 errorIni;
						wVersionRequested = MAKEWORD(2, 2);
						errorIni = WSAStartup(wVersionRequested, &wsaData);
						if(errorIni != 0)
						{
							Int32 err = WSAGetLastError();
							throw SocketException(err, ErrorString(err));
						}

						ws2_32_is_loaded = true;
					}
						//throw SocketException(errorIni);
					/*
						WSASYSNOTREADY Indicates that the underlying network subsystem is not ready for network communication.
						WSAVERNOTSUPPORTED The version of Windows Sockets support requested is not provided by this particular Windows Sockets implementation.
						WSAEINPROGRESS A blocking Windows Sockets 1.1 operation is in progress.
						WSAEPROCLIM Limit on the number of tasks supported by the Windows Sockets implementation has been reached.
						WSAEFAULT The lpWSAData is not a valid pointer.
					*/
					this->sock = (Int32)socket(this->address_family, this->socket_type, this->protocol_type);
					if(this->sock < 0)
					{
						//if(this->sock == INVALID_SOCKET)
						Int32 err = WSAGetLastError();
						throw SocketException(err, ErrorString(err));
					}

					//printf("socket handle: %d\n", this->sock);//

#else
                    //printf("hello www");
					this->sock = socket(this->address_family, this->socket_type, this->protocol_type);
					if(this->sock < 0)
						//throw SocketException(errno, (const char *)_sys_errlist[errno]);
						throw SocketException(errno, (const char *)_sys_errlist[errno]);
					/*
						EPROTONOSUPPORT The protocol or style is not supported by the namespace specified.
						EMFILE The process already has too many file descriptors open.
						ENFILE The system already has too many file descriptors open.
						EACCES The process does not have the privilege to create a socket of the specified style or protocol.
						ENOBUFS The system ran out of internal buffer space.
					*/

#endif
			}
			//----------------------------------------------------
			Socket::~Socket()
			{
				//printf("vv\n");
				/*#if defined WIN32 || WIN64
				ws2_32_is_loaded = false;
				WSACleanup();
				#endif*/
				if(!disposed)
					this->Close();
				/*if(!disposed)
				//WSACleanup();
					this->Close();*/
			}
			//----------------------------------------------------
			AddressFamily Socket::get_AddressFamily() const
			{
				return this->address_family;
			}
			//----------------------------------------------------
			SocketType Socket::get_SocketType() const
			{
				return this->socket_type;
			}
			//----------------------------------------------------
			ProtocolType Socket::get_ProtocolType() const
			{
				return this->protocol_type;
			}
			//----------------------------------------------------
			bool Socket::get_Connected() const
			{
				return this->isConnected;
			}
			//----------------------------------------------------
			bool Socket::get_IsAccepted() const
			{
				return this->isAccepted;
			}
			//----------------------------------------------------
			bool Socket::get_IsBound() const
			{
				return this->isBound;
			}
			//----------------------------------------------------
			// get port, IPv4 or IPv6:
			/*in_port_t get_in_port(struct sockaddr *sa)
			{
				if (sa->sa_family == AF_INET) {
					return (((struct sockaddr_in*)sa)->sin_port);
				}

				return (((struct sockaddr_in6*)sa)->sin6_port);
			}*/
			void Socket::Connect(IPEndPoint &remoteEP)
			{
				if(&remoteEP == NULL)
						throw ArgumentNullException("remoteEP", "remoteEP is null");

				if(this->disposed)
						throw ObjectDisposedException("Socket", "The Socket has been closed");

				if(this->isListening)
						throw InvalidOperationException("The Socket is Listening");

				if(get_AddressFamily() == System::Net::Sockets::InterNetwork)
				{
					struct sockaddr_in clientService;
					struct in_addr addr;
					if(remoteEP.get_LongAddress() >= 0)
						addr.s_addr = remoteEP.get_LongAddress();
					else
					{
						addr.s_addr = *(u_long *) remoteEP.get_Address().GetAddressBytes();
					}
					clientService.sin_family = this->address_family;
					clientService.sin_port = htons(remoteEP.get_Port());
					clientService.sin_addr = addr;

					//printf("connect\n");
					//this->ret = connect((SOCKET)this->sock, (struct sockaddr *)remoteEP.get_Address().GetAddressBytes(), remoteEP.get_Address().GetAddressBytesLength());
					this->ret = connect((SOCKET)this->sock, (SOCKADDR*) &clientService, sizeof(clientService) );
				}
				else //if(get_AddressFamily() == System::Net::Sockets::InterNetworkV6)
				{
					struct addrinfo *res = (struct addrinfo *)remoteEP.get_Address().GetAddressBytes();
					((struct sockaddr_in6 *)(res->ai_addr))->sin6_port = htons(remoteEP.get_Port());
					this->ret = connect((SOCKET)this->sock, res->ai_addr, res->ai_addrlen);
					//printf("connect\n");
					//this->ret = connect((SOCKET)this->sock, (struct sockaddr *)remoteEP.get_Address().GetAddressBytes(), remoteEP.get_Address().GetAddressBytesLength());
					//this->ret = connect((SOCKET)this->sock, (SOCKADDR*) &clientService, sizeof(clientService) );
				}

                if(this->ret < 0)
                {
#if defined WIN32 || WIN64

						Int32 err = WSAGetLastError();
						throw SocketException(err, ErrorString(err));
                        /*switch(WSAGetLastError())
                        {
                            case WSANOTINITIALISED:
                                throw SocketException(WSAGetLastError(), "A successful WSAStartup call must occur before using this function.");
                                break;
                            case WSAENETDOWN:
                                throw SocketException(WSAGetLastError(), "The network subsystem has failed.");
                                break;
                            case WSAEADDRINUSE:
                                throw SocketException(WSAGetLastError(), "The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR. This error usually occurs when executing bind, but could be delayed until this function if the bind was to a partially wildcard address (involving ADDR_ANY) and if a specific address needs to be committed at the time of this function.");
                                break;
                            case WSAEINTR:
                                throw SocketException(WSAGetLastError(), "The blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall");
                                break;
                            case WSAEINPROGRESS:
                                throw SocketException(WSAGetLastError(), "A nonblocking connect call is in progress on the specified socket.");
                                break;
                            case WSAEADDRNOTAVAIL:
                                throw SocketException(WSAGetLastError(), "The remote address is not a valid address (such as ADDR_ANY).");
                                break;
                            case WSAEAFNOSUPPORT:
                                throw SocketException(WSAGetLastError(), "Addresses in the specified family cannot be used with this socket.");
                                break;
                            case WSAECONNREFUSED:
                                throw SocketException(WSAGetLastError(), "The attempt to connect was forcefully rejected.");
                                break;
                            case WSAEFAULT:
                                throw SocketException(WSAGetLastError(), "The name or the namelen parameter is not a valid part of the user address space, the namelen parameter is too small, or the name parameter contains incorrect address format for the associated address family.");
                                break;
                            case WSAEINVAL:
                                throw SocketException(WSAGetLastError(), "The parameter s is a listening socket.");
                                break;
                            case WSAEISCONN:
                                throw SocketException(WSAGetLastError(), "The socket is already connected (connection-oriented sockets only).");
                                break;
                            case WSAENETUNREACH:
                                throw SocketException(WSAGetLastError(), "The network cannot be reached from this host at this time.");
                                break;
                            case WSAEHOSTUNREACH:
                                throw SocketException(WSAGetLastError(), "A socket operation was attempted to an unreachable host.");
                                break;
                            case WSAENOBUFS:
                                throw SocketException(WSAGetLastError(), "No buffer space is available. The socket cannot be connected.");
                                break;
                            case WSAENOTSOCK:
                                throw SocketException(WSAGetLastError(), "The descriptor is not a socket.");
                                break;
                            case WSAETIMEDOUT:
                                throw SocketException(WSAGetLastError(), "Attempt to connect timed out without establishing a connection.");
                                break;
                            case WSAEWOULDBLOCK:
                                throw SocketException(WSAGetLastError(), "The socket is marked as nonblocking and the connection cannot be completed immediately.");
                                break;
                            case WSAEACCES:
                                throw SocketException(WSAGetLastError(), "Attempt to connect datagram socket to broadcast address failed because setsockopt option SO_BROADCAST is not enabled.");
                                break;
                            default:
                                throw SocketException(WSAGetLastError());
                        }*/

#else

                            switch(errno)
                            {

                                case EACCES:
                                    throw SocketException(errno, "For Unix domain sockets, which are identified by pathname: Write permission is denied on the socket file, or search permission is denied for one of the directories in the path prefix.");
                                    break;
                                case EPERM:
                                    throw SocketException(errno, "The user tried to connect to a broadcast address without having the socket broadcast flag enabled or the connection request failed because of a local firewall rule.");
                                    break;
                                case EADDRINUSE:
                                    throw SocketException(errno, "Local address is already in use.");
                                    break;
                                case EAFNOSUPPORT:
                                    throw SocketException(errno, "The passed address didn't have the correct address family in its sa_family field.");
                                    break;
                                case EAGAIN:
                                    throw SocketException(errno, "No more free local ports or insufficient entries in the routing cache. For PF_INET see the net.ipv4.ip_local_port_range sysctl in ip(7) on how to increase the number of local ports.");
                                    break;
                                case EALREADY:
                                    throw SocketException(errno, "The socket is non-blocking and a previous connection attempt has not yet been completed.");
                                    break;
                                case EBADF:
                                    throw SocketException(errno, "The file descriptor is not a valid index in the descriptor table.");
                                    break;
                                case ECONNREFUSED:
                                    throw SocketException(errno, "No one listening on the remote address.");
                                    break;
                                case EFAULT:
                                    throw SocketException(errno, "The socket structure address is outside the user's address space. ");
                                    break;
                                case EINPROGRESS:
                                    throw SocketException(errno, "The socket is non-blocking and the connection cannot be completed immediately. It is possible to select(2) or poll(2) for completion by selecting the socket for writing. After select(2) indicates writability, use getsockopt(2) to read the SO_ERROR option at level SOL_SOCKET to determine whether connect() completed successfully (SO_ERROR is zero) or unsuccessfully (SO_ERROR is one of the usual error codes listed here, explaining the reason for the failure).");
                                    break;
                                case EINTR:
                                    throw SocketException(errno, "The system call was interrupted by a signal that was caught.");
                                    break;
                                case EISCONN:
                                    throw SocketException(errno, "The socket is already connected.");
                                    break;
                                case ENETUNREACH:
                                    throw SocketException(errno, "Network is unreachable.");
                                    break;
                                case ENOTSOCK:
                                    throw SocketException(errno, "The file descriptor is not associated with a socket.");
                                    break;
                                case ETIMEDOUT:
                                    throw SocketException(errno, "Timeout while attempting connection. The server may be too busy to accept new connections. Note that for IP sockets the timeout may be very long when syncookies are enabled on the server.");
                                    break;
                                default:
                                    throw SocketException(errno, (const char *)_sys_errlist[errno]);
                                    //throw SocketException(errno, (const char *)_sys_errlist[errno]);
                            }

#endif
                }

				this->isConnected = true;

				return;
			}
			//----------------------------------------------------
			void Socket::Bind(IPEndPoint &localEP)
			{
				if(&localEP == NULL)
						throw ArgumentNullException("localEP", "localEP is null");

				if(this->disposed)
						throw ObjectDisposedException("Socket", "The Socket has been closed");

				if(this->isListening)
						throw InvalidOperationException("The Socket is Listening");

                sockaddr_in clientService;
                struct in_addr addr;
				if(localEP.get_LongAddress() >= 0)
					addr.s_addr = localEP.get_LongAddress();
				else
					addr.s_addr = *(u_long *) localEP.get_Address().GetAddressBytes();
                clientService.sin_family = this->address_family;
                clientService.sin_port = htons(localEP.get_Port());
                clientService.sin_addr = addr;

                this->ret = bind((SOCKET)this->sock, (SOCKADDR*) &clientService, sizeof(clientService) );

                if(this->ret < 0)
                {
#if defined WIN32 || WIN64

						Int32 err = WSAGetLastError();
						throw SocketException(err, ErrorString(err));
                        /*switch(WSAGetLastError())
                        {
                            case WSANOTINITIALISED:
                                throw SocketException(WSAGetLastError(), "A successful WSAStartup call must occur before using this function.");
                                break;
                            case WSAENETDOWN:
                                throw SocketException(WSAGetLastError(), "The network subsystem has failed.");
                                break;
                            case WSAEACCES:
                                throw SocketException(WSAGetLastError(), "Attempt to connect datagram socket to broadcast address failed because setsockopt option SO_BROADCAST is not enabled.");
                                break;
                            case WSAEADDRINUSE:
                                throw SocketException(WSAGetLastError(), "A process on the computer is already bound to the same fully-qualified address and the socket has not been marked to allow address reuse with SO_REUSEADDR. For example, the IP address and port are bound in the af_inet case). (See the SO_REUSEADDR socket option under setsockopt.)");
                                break;
                            case WSAEADDRNOTAVAIL:
                                throw SocketException(WSAGetLastError(), "The specified address is not a valid address for this computer.");
                                break;
                            case WSAEFAULT:
                                throw SocketException(WSAGetLastError(), "The name or namelen parameter is not a valid part of the user address space, the namelen parameter is too small, the name parameter contains an incorrect address format for the associated address family, or the first two bytes of the memory block specified by name does not match the address family associated with the socket descriptor s.");
                                break;
                            case WSAEINPROGRESS:
                                throw SocketException(WSAGetLastError(), "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
                                break;
                            case WSAEINVAL:
                                throw SocketException(WSAGetLastError(), "The socket is already bound to an address.");
                                break;
                            case WSAENOBUFS:
                                throw SocketException(WSAGetLastError(), "Not enough buffers available, too many connections.");
                                break;
                            case WSAENOTSOCK:
                                throw SocketException(WSAGetLastError(), "The descriptor is not a socket.");
                                break;
                            default:
                                throw SocketException(WSAGetLastError());
                        }*/

#else

                            switch(errno)
                            {
                                case EACCES:
                                    throw SocketException(errno, "The address is protected, and the user is not the superuser.");
                                    break;
								//case EADDRINUSE:
                                //    throw SocketException(errno, "The given address is already in use.");
                                //    break;
								case EBADF:
                                    throw SocketException(errno, "sockfd is not a valid descriptor.");
                                    break;
								//case EINVAL:
                                //   throw SocketException(errno, "The socket is already bound to an address.");
                                //    break;
								case ENOTSOCK:
                                    throw SocketException(errno, "sockfd is a descriptor for a file, not a socket.");
                                    break;
								//case EACCES:
                                //    throw SocketException(errno, "Search permission is denied on a component of the path prefix.");
                                //    break;
								case EADDRNOTAVAIL:
                                    throw SocketException(errno, "A non-existent interface was requested or the requested address was not local.");
                                    break;
								case EFAULT:
                                    throw SocketException(errno, "my_addr points outside the user's accessible address space.");
                                    break;
								//case EINVAL:
                                //    throw SocketException(errno, "The addrlen is wrong, or the socket was not in the AF_UNIX family.");
                                //    break;
								case ELOOP:
                                    throw SocketException(errno, "Too many symbolic links were encountered in resolving my_addr.");
                                    break;
								case ENAMETOOLONG:
                                    throw SocketException(errno, "my_addr is too long.");
                                    break;
								case ENOENT:
                                    throw SocketException(errno, "The file does not exist.");
                                    break;
								case ENOMEM:
                                    throw SocketException(errno, "Insufficient kernel memory was available.");
                                    break;
								case ENOTDIR:
                                    throw SocketException(errno, "A component of the path prefix is not a directory.");
                                    break;
								case EROFS:
                                    throw SocketException(errno, "The socket inode would reside on a read-only file system.");
                                    break;

                                default:
                                    throw SocketException(errno, (const char *)_sys_errlist[errno]);
                            }

#endif
                }

				this->isBound = true;
				return;
			}
			//----------------------------------------------------
			void Socket::Select(ArrayList &checkRead, ArrayList &checkWrite, ArrayList &checkError, Int32 microSeconds)
			{
				if(&checkRead == NULL && &checkWrite == NULL && &checkError == NULL)
					throw ArgumentNullException("checkRead/checkWrite/checkError", "checkRead,checkWrite and checkError are null");;

				struct timeval *timeout;  /* Timeout for select */
				if(microSeconds < 0)
					timeout = null;
				else
					timeout->tv_usec = microSeconds;

				//static fd_set socks_checkRead;
				//static fd_set socks_checkWrite;
				//static fd_set socks_checkError;
                fd_set socks_checkRead;
				fd_set socks_checkWrite;
				fd_set socks_checkError;

				Int32 i;
				Int32 s;
				Int32 highsock = -1;
				Int32 ret;
				//=======================
				if(&checkRead != NULL)
				{
					if(checkRead.get_Count() > 0)
					{
						FD_ZERO(&socks_checkRead);
						for(i = 0 ; checkRead.get_Count() ; i++)
						{
							s = (Int32)(Long)checkRead[i];
							FD_SET(s, &socks_checkRead);
							if (s > highsock)
								highsock = s;
						}

						ret = select(highsock + 1, &socks_checkRead, (fd_set *) 0, (fd_set *) 0, timeout);
						if(ret < 0)
						{
							goto ReportError;
						}
						else
						{
							//checkRead.Clear();

							/*for(i = 0 ; socks_checkRead.fd_count ; i++)
								checkRead.Add((Object *)socks_checkRead.fd_array[i]);*/

							if(ret > 0)
							{
								ArrayList al;//();
								for(i = 0 ; checkRead.get_Count() ; i++)
								{
									s = (Int32)(Long)checkRead[i];
									if (FD_ISSET(s, &socks_checkRead))
										al.Add((Object*)s);
								}
								checkRead.Clear();
								checkRead = al;

							}
							else
								checkRead.Clear();
						}
					}
				}
				//=======================
				if(&checkWrite != NULL)
				{
					if(checkWrite.get_Count() > 0)
					{
						FD_ZERO(&socks_checkWrite);
						for(i = 0 ; checkWrite.get_Count() ; i++)
						{
							s = (Int32)(Long)checkWrite[i];
							FD_SET(s, &socks_checkWrite);
							if (s > highsock)
								highsock = s;
						}

						ret = select(highsock + 1, (fd_set *) 0, &socks_checkWrite, (fd_set *) 0, timeout);

						if(ret < 0)
						{
							goto ReportError;
						}
						else
						{

							//for(i = 0 ; socks_checkWrite.fd_count ; i++)
							//	checkWrite.Add((Object *)socks_checkWrite.fd_array[i]);

							if(ret > 0)
							{
								ArrayList al;//();
								for(i = 0 ; checkWrite.get_Count() ; i++)
								{
									s = (Int32)(Long)checkWrite[i];
									if (FD_ISSET(s, &socks_checkWrite))
										al.Add((Object*)s);
								}
								checkWrite.Clear();
								checkWrite = al;

							}
							else
								checkWrite.Clear();
						}
					}
				}
				//=======================
				if(&checkError != NULL)
				{
					if(checkError.get_Count() > 0)
					{
						FD_ZERO(&socks_checkError);
						for(i = 0 ; checkError.get_Count() ; i++)
						{
							s = (Int32)(Long)checkError[i];
							FD_SET(s, &socks_checkError);
							if (s > highsock)
								highsock = s;
						}

						ret = select(highsock + 1, (fd_set *) 0, (fd_set *) 0, &socks_checkError, timeout);

						if(ret < 0)
						{
							goto ReportError;
						}
						else
						{

							//for(i = 0 ; socks_checkError.fd_count ; i++)
							//	checkRead.Add((Object *)socks_checkError.fd_array[i]);

							if(ret > 0)
							{
								ArrayList al;//();
								for(i = 0 ; checkError.get_Count() ; i++)
								{
									s = (Int32)(Long)checkError[i];
									if (FD_ISSET(s, &socks_checkError))
										al.Add((Object*)s);
								}
								checkError.Clear();
								checkError = al;

							}
							else
								checkError.Clear();

						}
					}
				}
				return ;
				//=======================
				ReportError:

#if defined WIN32 || WIN64

						Int32 err = WSAGetLastError();
						throw SocketException(err, ErrorString(err));
                        /*switch(WSAGetLastError())
                        {
                            case WSANOTINITIALISED:
                                throw SocketException(WSAGetLastError(), "A successful WSAStartup call must occur before using this function.");
                                break;
                            case WSAEFAULT:
                                throw SocketException(WSAGetLastError(), "The Windows Sockets implementation was unable to allocate needed resources for its internal operations, or the readfds, writefds, exceptfds, or timeval parameters are not part of the user address space.");
                                break;
                            case WSAENETDOWN:
                                throw SocketException(WSAGetLastError(), "The network subsystem has failed.");
                                break;
                            case WSAEINVAL:
                                throw SocketException(WSAGetLastError(), "The time-out value is not valid, or all three descriptor parameters were null.");
                                break;
                            case WSAEINTR:
                                throw SocketException(WSAGetLastError(), "A blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall.");
                                break;
                            case WSAEINPROGRESS:
                                throw SocketException(WSAGetLastError(), "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
                                break;
                            case WSAENOTSOCK:
                                throw SocketException(WSAGetLastError(), "One of the descriptor sets contains an entry that is not a socket.");
                                break;
                            default:
                                throw SocketException(WSAGetLastError());
                        }*/

#else

                            switch(errno)
							{
                                case EBADF:
                                    throw SocketException(errno, "An invalid file descriptor was given in one of the sets. (Perhaps a file descriptor that was already closed, or one on which an error has occurred.)");
                                    break;
                                case EINTR:
                                    throw SocketException(errno, "A signal was caught.");
                                    break;
                                case EINVAL:
                                    throw SocketException(errno, "nfds is negative or the value contained within timeout is invalid.");
                                    break;
                                case ENOMEM:
                                    throw SocketException(errno, "unable to allocate memory for internal tables.");
                                    break;
                                default:
                                    throw SocketException(errno, (const char *)_sys_errlist[errno]);
                            }

#endif
			}
			//----------------------------------------------------
			Int32 Socket::Receive(char buffer[], Int32 offset, Int32 size, SocketFlags socketFlags)
			{
				if(this->disposed)
					throw ObjectDisposedException("Socket", "The Socket has been closed");


				if(offset < 0)
					throw ArgumentOutOfRangeException("offset", "offset is less than 0");

				if(size <= 0)
					throw ArgumentOutOfRangeException("size", "size must be greater than 0");

				//buffer += offset;

				this->ret = recv((SOCKET)this->sock, buffer + offset, size, socketFlags);

				//buffer -= offset;

				if(this->ret < 0)
                {
#if defined WIN32 || WIN64

						Int32 err = WSAGetLastError();
						throw SocketException(err, ErrorString(err));
                        /*switch(WSAGetLastError())
                        {
                            case WSANOTINITIALISED:
                                throw SocketException(WSAGetLastError(), "A successful WSAStartup call must occur before using this function.");
                                break;
                            case WSAENETDOWN:
                                throw SocketException(WSAGetLastError(), "The network subsystem has failed.");
                                break;
                            case WSAEFAULT:
                                throw SocketException(WSAGetLastError(), "The buf parameter is not completely contained in a valid part of the user address space.");
                                break;
                            case WSAENOTCONN:
                                throw SocketException(WSAGetLastError(), "The socket is not connected.");
                                break;
							case WSAEINTR:
                                throw SocketException(WSAGetLastError(), "The (blocking) call was canceled through WSACancelBlockingCall.");
                                break;
                            case WSAEINPROGRESS:
                                throw SocketException(WSAGetLastError(), "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
                                break;
                            case WSAENETRESET:
                                throw SocketException(WSAGetLastError(), "The connection has been broken due to the keep-alive activity detecting a failure while the operation was in progress.");
                                break;
                            case WSAENOTSOCK:
                                throw SocketException(WSAGetLastError(), "The descriptor is not a socket.");
                                break;
							case WSAEOPNOTSUPP:
                                throw SocketException(WSAGetLastError(), "MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only send operations.");
                                break;
                            case WSAESHUTDOWN:
                                throw SocketException(WSAGetLastError(), "The socket has been shut down; it is not possible to receive on a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH.");
                                break;
                            case WSAEWOULDBLOCK:
                                throw SocketException(WSAGetLastError(), "The socket is marked as nonblocking and the receive operation would block.");
                                break;
                            case WSAEMSGSIZE:
                                throw SocketException(WSAGetLastError(), "The message was too large to fit into the specified buffer and was truncated.");
                                break;
							case WSAEINVAL:
                                throw SocketException(WSAGetLastError(), "The socket has not been bound with bind, or an unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled or (for byte stream sockets only) len was zero or negative.");
                                break;
                            case WSAECONNABORTED:
                                throw SocketException(WSAGetLastError(), "The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable.");
                                break;
                            case WSAETIMEDOUT:
                                throw SocketException(WSAGetLastError(), "The connection has been dropped because of a network failure or because the peer system failed to respond.");
                                break;
                            case WSAECONNRESET:
                                throw SocketException(WSAGetLastError(), "The virtual circuit was reset by the remote side executing a hard or abortive close. The application should close the socket as it is no longer usable. On a UPD-datagram socket this error would indicate that a previous send operation resulted in an ICMP \"Port Unreachable\" message.");
                                break;
                            default:
                                throw SocketException(WSAGetLastError());
                        }*/

#else

                            switch(errno)
                            {

                                case EAGAIN:
                                    throw SocketException(errno, "The socket's file descriptor is marked O_NONBLOCK and no data is waiting to be received; or MSG_OOB is set and no out-of-band data is available and either the socket's file descriptor is marked O_NONBLOCK or the socket does not support blocking to await out-of-band data.");
                                    break;
								case EBADF:
                                    throw SocketException(errno, "The socket argument is not a valid file descriptor.");
                                    break;
                                case ECONNRESET:
                                    throw SocketException(errno, "A connection was forcibly closed by a peer. ");
                                    break;
								case EINTR:
                                    throw SocketException(errno, "The recv() function was interrupted by a signal that was caught, before any data was available.");
                                    break;
                                case EINVAL:
                                    throw SocketException(errno, "The MSG_OOB flag is set and no out-of-band data is available.");
                                    break;
								case ENOTCONN:
                                    throw SocketException(errno, "A receive is attempted on a connection-mode socket that is not connected.");
                                    break;
                                case ENOTSOCK:
                                    throw SocketException(errno, "The socket argument does not refer to a socket.");
                                    break;
								case EOPNOTSUPP:
                                    throw SocketException(errno, "The specified flags are not supported for this socket type or protocol.");
                                    break;
                                case ETIMEDOUT:
                                    throw SocketException(errno, "The connection timed out during connection establishment, or due to a transmission timeout on active connection.");
                                    break;
								case EIO:
                                    throw SocketException(errno, "An I/O error occurred while reading from or writing to the file system.");
                                    break;
                                case ENOBUFS:
                                    throw SocketException(errno, "Insufficient resources were available in the system to perform the operation.");
                                    break;
                                default:
                                    throw SocketException(errno, (const char *)_sys_errlist[errno]);
                            }

#endif
                }
				else
					return this->ret;
			}
			//----------------------------------------------------
			Int32 Socket::Send(const char buffer[], Int32 offset, Int32 size, SocketFlags socketFlags)
			{
				if(this->disposed)
					throw ObjectDisposedException("Socket", "The Socket has been closed");

				if(offset < 0)
					throw ArgumentOutOfRangeException("offset", "offset is less than 0");

				if(size <= 0)
					throw ArgumentOutOfRangeException("size", "size must be greater than 0");

				//buffer += offset;

				this->ret = send((SOCKET)this->sock, buffer + offset, size, socketFlags);

				//buffer -= offset;

				if(this->ret < 0)
                {
#if defined WIN32 || WIN64

						Int32 err = WSAGetLastError();
						throw SocketException(err, ErrorString(err));
                        /*switch(WSAGetLastError())
                        {
                            case WSANOTINITIALISED:
                                throw SocketException(WSAGetLastError(), "A successful WSAStartup call must occur before using this function.");
                                break;
                            case WSAENETDOWN:
                                throw SocketException(WSAGetLastError(), "The network subsystem has failed.");
                                break;
                            case WSAEACCES:
                                throw SocketException(WSAGetLastError(), "The requested address is a broadcast address, but the appropriate flag was not set. Call setsockopt with the SO_BROADCAST socket option to enable use of the broadcast address.");
                                break;
                            case WSAEINTR:
                                throw SocketException(WSAGetLastError(), "A blocking Windows Sockets 1.1 call was canceled through WSACancelBlockingCall.");
                                break;
                            case WSAEINPROGRESS:
                                throw SocketException(WSAGetLastError(), "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
                                break;
                            case WSAEFAULT:
                                throw SocketException(WSAGetLastError(), "The buf parameter is not completely contained in a valid part of the user address space.");
                                break;
                            case WSAENETRESET:
                                throw SocketException(WSAGetLastError(), "The connection has been broken due to the keep-alive activity detecting a failure while the operation was in progress.");
                                break;
                            case WSAENOBUFS:
                                throw SocketException(WSAGetLastError(), "No buffer space is available.");
                                break;
                            case WSAENOTCONN:
                                throw SocketException(WSAGetLastError(), "The socket is not connected.");
                                break;
                            case WSAENOTSOCK:
                                throw SocketException(WSAGetLastError(), "The descriptor is not a socket.");
                                break;
                            case WSAEOPNOTSUPP:
                                throw SocketException(WSAGetLastError(), "MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only receive operations.");
                                break;
                            case WSAESHUTDOWN:
                                throw SocketException(WSAGetLastError(), "The socket has been shut down; it is not possible to send on a socket after shutdown has been invoked with how set to SD_SEND or SD_BOTH.");
                                break;
                            case WSAEWOULDBLOCK:
                                throw SocketException(WSAGetLastError(), "The socket is marked as nonblocking and the requested operation would block.");
                                break;
                            case WSAEMSGSIZE:
                                throw SocketException(WSAGetLastError(), "The socket is message oriented, and the message is larger than the maximum supported by the underlying transport.");
                                break;
                            case WSAEHOSTUNREACH:
                                throw SocketException(WSAGetLastError(), "The remote host cannot be reached from this host at this time.");
                                break;
                            case WSAEINVAL:
                                throw SocketException(WSAGetLastError(), "The socket has not been bound with bind, or an unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled.");
                                break;
                            case WSAECONNABORTED:
                                throw SocketException(WSAGetLastError(), "The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable.");
                                break;
                            case WSAECONNRESET:
                                throw SocketException(WSAGetLastError(), "The virtual circuit was reset by the remote side executing a hard or abortive close. For UDP sockets, the remote host was unable to deliver a previously sent UDP datagram and responded with a \"Port Unreachable\" ICMP packet. The application should close the socket as it is no longer usable.");
                                break;
                            case WSAETIMEDOUT:
                                throw SocketException(WSAGetLastError(), "The connection has been dropped, because of a network failure or because the system on the other end went down without notice.");
                                break;

                            default:
                                throw SocketException(WSAGetLastError());
                        }*/

#else

                            switch(errno)
                            {
                                case EACCES:
                                    throw SocketException(errno, "(For Unix domain sockets, which are identified by pathname) Write permission is denied on the destination socket file, or search permission is denied for one of the directories the path prefix.");
                                    break;
                                case EAGAIN:
                                    throw SocketException(errno, "The socket is marked non-blocking and the requested operation would block.");
                                    break;
                                case EBADF:
                                    throw SocketException(errno, "An invalid descriptor was specified.");
                                    break;
                                case ECONNRESET:
                                    throw SocketException(errno, "Connection reset by peer.");
                                    break;
                                case EDESTADDRREQ:
                                    throw SocketException(errno, "The socket is not connection-mode, and no peer address is set.");
                                    break;
                                case EFAULT:
                                    throw SocketException(errno, "An invalid user space address was specified for a parameter.");
                                    break;
                                case EINTR:
                                    throw SocketException(errno, "A signal occurred before any data was transmitted.");
                                    break;
                                case EINVAL:
                                    throw SocketException(errno, "Invalid argument passed.");
                                    break;
                                case EISCONN:
                                    throw SocketException(errno, "The connection-mode socket was connected already but a recipient was specified. (Now either this error is returned, or the recipient specification is ignored.)");
                                    break;
                                case EMSGSIZE:
                                    throw SocketException(errno, "The socket type requires that message be sent atomically, and the size of the message to be sent made this impossible.");
                                    break;
                                case ENOBUFS:
                                    throw SocketException(errno, "The output queue for a network interface was full. This generally indicates that the interface has stopped sending, but may be caused by transient congestion. (Normally, this does not occur in Linux. Packets are just silently dropped when a device queue overflows.)");
                                    break;
                                case ENOMEM:
                                    throw SocketException(errno, "No memory available.");
                                    break;
                                case ENOTCONN:
                                    throw SocketException(errno, "The socket is not connected, and no target has been given.");
                                    break;
                                case ENOTSOCK:
                                    throw SocketException(errno, "The argument s is not a socket.");
                                    break;
                                case EOPNOTSUPP:
                                    throw SocketException(errno, "Some bit in the flags argument is inappropriate for the socket type.");
                                    break;
                                case EPIPE:
                                    throw SocketException(errno, "The local end has been shut down on a connection oriented socket. In this case the process will also receive a SIGPIPE unless MSG_NOSIGNAL is set.");
                                    break;
                                default:
                                    throw SocketException(errno, (const char *)_sys_errlist[errno]);
                            }

#endif
                }
				else
					return this->ret;
			}
			//----------------------------------------------------
			void Socket::Close()
			{

				if(this->disposed)
					throw ObjectDisposedException("Socket", "The Socket has been closed");

				this->isListening = false;
				this->isConnected = false;
				this->isAccepted = false;
				this->disposed = true;

				#if defined WIN32 || WIN64
					this->ret = closesocket((SOCKET)this->sock);
				#else
					this->ret = close(this->sock);
				#endif

				if(this->ret < 0)
                {
#if defined WIN32 || WIN64

						Int32 err = WSAGetLastError();
						throw SocketException(err, ErrorString(err));
                        /*switch(WSAGetLastError())
                        {
                            case WSANOTINITIALISED:
                                throw SocketException(WSAGetLastError(), "A successful WSAStartup call must occur before using this function.");
                                break;
                            case WSAENETDOWN:
                                throw SocketException(WSAGetLastError(), "The network subsystem has failed.");
                                break;
                            case WSAENOTSOCK:
                                throw SocketException(WSAGetLastError(), "The descriptor is not a socket.");
                                break;
                            case WSAEINPROGRESS:
                                throw SocketException(WSAGetLastError(), "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
                                break;
                            case WSAEINTR:
                                throw SocketException(WSAGetLastError(), "The (blocking) Windows Socket 1.1 call was canceled through WSACancelBlockingCall.");
                                break;
                            case WSAEWOULDBLOCK:
                                throw SocketException(WSAGetLastError(), "The socket is marked as nonblocking, but the l_onoff member of the linger structure is set to non-zcero and the l_linger member of the linger structure is set to a nonzero timeout value.");
                                break;
                            default:
                                throw SocketException(WSAGetLastError());
                        }*/

#else

                            switch(errno)
                            {
                                case EBADF:
                                    throw SocketException(errno, "fd isn't a valid open file descriptor.");
                                    break;
								case EINTR:
                                    throw SocketException(errno, "The close() call was interrupted by a signal.");
                                    break;
								case EIO:
                                    throw SocketException(errno, "An I/O error occurred.");
                                    break;
                                default:
                                    throw SocketException(errno, (const char *)_sys_errlist[errno]);
                            }

#endif
                }

/*#if defined WIN32 || WIN64
				WSACleanup();
#endif*///printf("closing\n");

				return ;
			}
			//----------------------------------------------------
			void Socket::Shutdown(SocketShutdown how)
			{

				if(this->disposed)
					throw ObjectDisposedException("Socket", "The Socket has been closed");

				this->ret = shutdown((SOCKET)this->sock, how);

				if(this->ret < 0)
                {
#if defined WIN32 || WIN64

						Int32 err = WSAGetLastError();
						throw SocketException(err, ErrorString(err));
                        /*switch(WSAGetLastError())
                        {
                            case WSANOTINITIALISED:
                                throw SocketException(WSAGetLastError(), "A successful WSAStartup call must occur before using this function.");
                                break;
                            case WSAENETDOWN:
                                throw SocketException(WSAGetLastError(), "The network subsystem has failed.");
                                break;
                            case WSAEINVAL:
                                throw SocketException(WSAGetLastError(), "The how parameter is not valid, or is not consistent with the socket type. For example, SD_SEND is used with a UNI_RECV socket type.");
                                break;
                            case WSAEINPROGRESS:
                                throw SocketException(WSAGetLastError(), "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.");
                                break;
                            case WSAENOTCONN:
                                throw SocketException(WSAGetLastError(), "The socket is not connected (connection-oriented sockets only).");
                                break;
                            case WSAENOTSOCK:
                                throw SocketException(WSAGetLastError(), "The descriptor is not a socket.");
                                break;
                            default:
                                throw SocketException(WSAGetLastError());
                        }*/

#else

                            switch(errno)
                            {
                                case EBADF:
                                    throw SocketException(errno, "The socket is not a valid descriptor.");
                                    break;
                                case ENOTCONN:
                                    throw SocketException(errno, "The specified socket is not connected.");
                                    break;
                                case ENOTSOCK:
                                    throw SocketException(errno, "The s is a file, not a socket.");
                                    break;
                                default:
                                    throw SocketException(errno, (const char *)_sys_errlist[errno]);
                            }

#endif
                }

				return ;
			}
			//----------------------------------------------------
			void Socket::Listen(Int32 backlog)
			{

				if(this->disposed)
					throw ObjectDisposedException("Socket", "The Socket has been closed");

				this->ret = listen((SOCKET)this->sock, backlog);

				if(this->ret < 0)
                {
#if defined WIN32 || WIN64

					Int32 err = WSAGetLastError();
					throw SocketException(err, ErrorString(err));

#else

                    switch(errno)
                    {
						case EADDRINUSE:
							throw SocketException(errno, "Another socket is already listening on the same port.");
							break;
						case EBADF:
							throw SocketException(errno, "The argument sockfd is not a valid descriptor.");
							break;
						case ENOTSOCK:
							throw SocketException(errno, "The argument sockfd is not a socket.");
							break;
						case EOPNOTSUPP:
							throw SocketException(errno, "The socket is not of a type that supports the listen() operation.");
							break;
						default:
							throw SocketException(errno, (const char *)_sys_errlist[errno]);
                    }

#endif
                }

				this->isListening = true;
				return ;
			}
			//----------------------------------------------------
			Socket *Socket::Accept() const
			{

				if(this->disposed)
					throw ObjectDisposedException("Socket", "The Socket has been closed");

				//this->ret = listen((SOCKET)this->sock, backlog);

				SOCKET _AcceptSocket = accept((SOCKET)this->sock, null, null);


#if defined WIN32 || WIN64

				if(_AcceptSocket == INVALID_SOCKET)
				{
					Int32 err = WSAGetLastError();
					throw SocketException(err, ErrorString(err));
				}

#else
				if(_AcceptSocket < 0)
				{
                    switch(errno)
                    {
						case EAGAIN:
							throw SocketException(errno, "The socket is marked non-blocking and no connections are present to be accepted.");
							break;
						case EBADF:
							throw SocketException(errno, "The descriptor is invalid.");
							break;
						case ECONNABORTED:
							throw SocketException(errno, "A connection has been aborted.");
							break;
						case EINTR:
							throw SocketException(errno, "The system call was interrupted by a signal that was caught before a valid connection arrived.");
							break;
						case EINVAL:
							throw SocketException(errno, "Socket is not listening for connections, or addrlen is invalid (e.g., is negative).");
							break;
						case EMFILE:
							throw SocketException(errno, "The per-process limit of open file descriptors has been reached.");
							break;
						case ENFILE:
							throw SocketException(errno, "The system limit on the total number of open files has been reached.");
							break;
						case ENOTSOCK:
							throw SocketException(errno, "The descriptor references a file, not a socket.");
							break;
						case EOPNOTSUPP:
							throw SocketException(errno, "The referenced socket is not of type SOCK_STREAM.");
							break;
						case EFAULT:
							throw SocketException(errno, "The addr argument is not in a writable part of the user address space.");
							break;
						case ENOBUFS:
							throw SocketException(errno, "Not enough free memory. This often means that the memory allocation is limited by the socket buffer limits, not by the system memory.");
							break;
						case EPROTO:
							throw SocketException(errno, "Protocol error.");
							break;
						case EPERM :
							throw SocketException(errno, "Firewall rules forbid connection.");
							break;
						default:
							throw SocketException(errno, (const char *)_sys_errlist[errno]);
                    }
				}

#endif

				return new Socket(_AcceptSocket);
			}
			//----------------------------------------------------
			void Socket::SetReceiveTcpWindowSize(Int32 size)
			{
				if(this->disposed)
					throw ObjectDisposedException("Socket", "The Socket has been closed");

				/*if(size <= 0)
					throw ArgumentOutOfRangeException("size", "size must be greater than 0");*/

				if(size < 0)
					throw ArgumentOutOfRangeException("size", "size can not be negative");

				this->ret = setsockopt(this->get_Handle(), SOL_SOCKET, SO_RCVBUF, (char *)&size, sizeof(size));

				if(this->ret < 0)
                {
#if defined WIN32 || WIN64

					Int32 err = WSAGetLastError();
					throw SocketException(err, ErrorString(err));

#else

					throw SocketException(errno, (const char *)_sys_errlist[errno]);

#endif
                }

				return ;
			}
			//----------------------------------------------------
			void Socket::SetSendTcpWindowSize(Int32 size)
			{
				if(this->disposed)
					throw ObjectDisposedException("Socket", "The Socket has been closed");

				/*if(size <= 0)
					throw ArgumentOutOfRangeException("size", "size must be greater than 0");*/

				if(size < 0)
					throw ArgumentOutOfRangeException("size", "size can not be negative");

				this->ret = setsockopt(this->get_Handle(), SOL_SOCKET, SO_SNDBUF, (char *)&size, sizeof(size));

				if(this->ret < 0)
                {
#if defined WIN32 || WIN64

					Int32 err = WSAGetLastError();
					throw SocketException(err, ErrorString(err));

#else

					throw SocketException(errno, (const char *)_sys_errlist[errno]);

#endif
                }

				return ;
			}
			//----------------------------------------------------
			Int32 Socket::GetReceiveTcpWindowSize()
			{
				if(this->disposed)
					throw ObjectDisposedException("Socket", "The Socket has been closed");

				Int32 size;
#if defined WIN32 || WIN64
				Int32 optLen = sizeof(Int32);
#else
				socklen_t optLen = sizeof(Int32);
#endif

				this->ret = getsockopt(this->get_Handle(), SOL_SOCKET, SO_RCVBUF, (char *)&size, &optLen);

				if(this->ret < 0)
                {
#if defined WIN32 || WIN64

					Int32 err = WSAGetLastError();
					throw SocketException(err, ErrorString(err));

#else

					throw SocketException(errno, (const char *)_sys_errlist[errno]);

#endif
                }

				return size;
			}
			//----------------------------------------------------
			Int32 Socket::GetSendTcpWindowSize()
			{
				if(this->disposed)
					throw ObjectDisposedException("Socket", "The Socket has been closed");

				Int32 size;
#if defined WIN32 || WIN64
				Int32 optLen = sizeof(Int32);
#else
				socklen_t optLen = sizeof(Int32);
#endif

				this->ret = getsockopt(this->get_Handle(), SOL_SOCKET, SO_SNDBUF, (char *)&size, &optLen);

				if(this->ret < 0)
                {
#if defined WIN32 || WIN64

					Int32 err = WSAGetLastError();
					throw SocketException(err, ErrorString(err));

#else

					throw SocketException(errno, (const char *)_sys_errlist[errno]);

#endif
                }

				return size;
			}
			//----------------------------------------------------

	   };
	};
};
//**************************************************************************************************************//
