#include "xDFSServer.h"

//**************************************************************************************************************//
namespace Parvicursor
{
   namespace xDFS
   {
 		//----------------------------------------------------
		xDFSServer::xDFSServer()
		{
		    sessions = new System::Collections::Hashtable();
			//pthread_mutex_init(&_xDFS_server_mutex_, NULL);
			printf("Integrated Parvicursor Servers (xDFS + xThread) Version 2 (native mode) with xSec Protocol v1.\nAll rights reserved to Parvicursor team (c) 2000-%d.\nEmail: arp@poshtkohi.info\nWebsite: http://www.poshtkohi.info\n\n\n\n", DateTime::get_Now().get_Year());
			//sessions = new Hashtable();
			//connections = new ArrayList();
//			printf("connections->get_Count(): %d\n", connections->get_Count());
			//ServerRSA = new RSA(); // generates a random server RSA public-private key
			//worker = new Thread(new ThreadStart(this.WorkerProc));
			//TestThread *test = new TestThread();
			worker = new Thread(this->Wrapper_To_Call_WorkerProc, (void *)this);
			closed = false;
		}
		//----------------------------------------------------
		void xDFSServer::Start()
		{
			if(!worker->get_IsAlive())
			{
				worker->Start();
				worker->Join();
			}
		}
		//----------------------------------------------------
		void xDFSServer::WorkerProc()
		{
			sock = new Socket(InterNetwork, Stream, tcp);
			//IPEndPoint hostEndPoint = new IPEndPoint(IPAddress.Any, 2799);
			IPEndPoint hostEndPoint(IPAddress::get_Any(), __xDFS_Server_Default_Port__);
			//sock.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, 1);
			sock->Bind(hostEndPoint);
			sock->Listen(500);
			//pthread_mutex_init(&_xDFS_server_mutex_, NULL);


			Socket *s;
			while(true)
			{
				if(closed)
					break;

				s = sock->Accept();

				//s.Blocking = false; // only for MONO .NET 1.1.6 on Windows
				//pthread_mutex_lock(&_xDFS_server_mutex_);//
				if(s != null)
				{
					//__i++;
					//Server info(s, sessions, /*ref ServerRSA,*/ connections);
					try
					{ 
						Server *info = new Server(s); 
						info->serverInstance = info;
						info->Run();
					}
					catch(System::Exception &e){ printf("Here: %s\n", e.get_Message().get_BaseStream()); }
					catch(...){ cout << "Server info(s)" << endl;}
					printf("new connection\n");
					//Thread::Sleep(1);
				}
				else
                    Thread::Sleep(1);
				//pthread_mutex_unlock(&_xDFS_server_mutex_);//
				//GC.Collect();
			}
		}
		//----------------------------------------------------
		void *xDFSServer::Wrapper_To_Call_WorkerProc(void* pt2Object)
		{
			xDFSServer* mySelf = (xDFSServer*) pt2Object;
			// call member
			mySelf->WorkerProc();
			return pt2Object;
		}
   		//----------------------------------------------------
   };
};
//**************************************************************************************************************//
