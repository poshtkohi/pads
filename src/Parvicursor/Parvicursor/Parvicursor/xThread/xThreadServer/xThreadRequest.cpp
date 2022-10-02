#define null 0
#include "stdafx.h"

#include "xThreadRequest.h"

//**************************************************************************************************************//
namespace Parvicursor
{
   namespace xThread
   {
		//----------------------------------------------------
		xThreadRequest::xThreadRequest(ParvicursorSocket *socket)
		{
			this->timeout = 120*1000; // 120s timeout
			this->secure = false;

			sockets = new ArrayList();
			sockets->Add(socket);
			////pthread_mutex_unlock(&mutex);

			buffer = null;
			this->fs = null;

		}
		//----------------------------------------------------
		void xThreadRequest::AddNewClientStream(ParvicursorSocket *socket)
		{
			//printf("xThreadRequest::AddNewClientStream\n");

			//return ;
			//pthread_mutex_lock(&_xDFS_server_mutex_);
			//pthread_mutex_lock(&mutex);
			if(this->sockets != null)
			{
				try { socket->WriteNoException(); }
                catch(...) {/*pthread_mutex_unlock(&mutex);*/ return;  }
				this->sockets->Add(socket);
			}
			//pthread_mutex_unlock(&mutex);
			//pthread_mutex_unlock(&_xDFS_server_mutex_);

			return ;
		}
		//----------------------------------------------------
		int xThreadRequest::WaitForAllConnections()
		{
			//clock_t start, finish;
			int _timeout = 0;
			//start = clock();
			//double  duration;
			while(true)
			{
				//pthread_mutex_lock(&_xDFS_server_mutex_);
				//pthread_mutex_lock(&mutex);
				int count = sockets->get_Count();
				//pthread_mutex_unlock(&_xDFS_server_mutex_);
				//pthread_mutex_unlock(&mutex);
				if(count == 2)
					return 0;

				if(count > 2)
					return -1;

				if(_timeout >= this->timeout)
					return -1;
				/*finish = clock();
				duration = (double)(finish - start) / CLOCKS_PER_SEC;
				if(duration >= timeout)
					return -1;*/
				Thread::Sleep(1);
				_timeout++;
			}
		}
		//----------------------------------------------------
		void xThreadRequest::Run()
		{
			try { ((ParvicursorSocket *)sockets->get_Value(0))->WriteNoException(); }
            catch(...) {/*pthread_mutex_unlock(&mutex);*/ WorkerExit(); return ;}
			printf("---------------------------------------------------------\n");
			printf("New xThread session.\n");
			if(WaitForAllConnections() == -1) { WorkerExit(); return ; }
			//WorkerExit(); return ;

			cout << "hello xThread world!" << endl;
			Thread::Sleep(1000000);

		End:
			WorkerExit();
			return ;
		}
		//----------------------------------------------------
		void xThreadRequest::SendExceptionToOneSocketAndClose(Exception &e, ParvicursorSocket *socket)
		{
			try { socket->WriteException(e); RemoveSocketFromArrayList(socket); }
			catch(...) { RemoveSocketFromArrayList(socket); }
		}
		//----------------------------------------------------
		void xThreadRequest::MakeReadSocketArrayList()
		{
			/* FD_ZERO() clears out the fd_set called socks, so that
				it doesn't contain any file descriptors. */
			FD_ZERO(&alRead);
			/* Loops through all the possible connections and adds
				those sockets to the fd_set */

			for (int i = 0 ; i < this->sockets->get_Count() ; i++)
			{
				 int handle = ((ParvicursorSocket *)sockets->get_Value(i))->get_BaseSocket()->get_Handle();
				/*if(handle != 0)
				{*/
					FD_SET(handle, &alRead);
					if (handle > highsock)
						highsock = handle;
				/*}*/
			}

			return ;
		}
		//----------------------------------------------------
		void xThreadRequest::RemoveSocketFromArrayList(ParvicursorSocket *socket)
		{
			int i = FindSocketIndex(socket);
			if(i != -1)
			{
				ParvicursorSocket *s = (ParvicursorSocket *)sockets->get_Value(i);
				try{s->get_BaseSocket()->Shutdown(Both);}
				catch(...) { }
				s->Close();

				sockets->RemoveAt(i);
				delete s;
			}
		}
		//----------------------------------------------------
		int xThreadRequest::FindSocketIndex(ParvicursorSocket *socket)
		{
			for(int i = 0 ; i < sockets->get_Count() ; i++)
				if((ParvicursorSocket *)sockets->get_Value(i) == socket)
					return i;
			return -1;
		}
		//----------------------------------------------------
		void xThreadRequest::WorkerExit()
		{
			//pthread_mutex_lock(&mutex);
			if(sockets != null)
			{
				if(sockets->get_Count() != 0)
				{
					for(int i = 0 ; i < sockets->get_Count() ; i++)
					{
						ParvicursorSocket *s = (ParvicursorSocket *)sockets->get_Value(i);
						//s->get_BaseSocket()->Shutdown(Both);
                        //s->Close();
                        if(s != null)
                        {
                            try {s->get_BaseSocket()->Shutdown(Both);}
                            catch(...) { }
                            s->Close();
                            delete s;
							//cout << "\n\n\n\n\neeeeeeeeee\n\n\n\n" << endl;
                            s = null;
                        }
					}
				}

                this->sockets->Clear();
				delete this->sockets;
				this->sockets = null;
			}

			if(fs != null)
			{
				fs->Flush();
				fs->Close();
				delete fs;
			}


			if(buffer != null)
				delete buffer;

			return ;
		}
   		//----------------------------------------------------
   };
};
//**************************************************************************************************************//
