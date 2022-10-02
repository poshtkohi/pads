/**
    #define meta ...
    printf("%s\n", meta);
**/


/*
	All rights reserved to Alireza Poshtkohi (c) 1999-2022.
	Email: arp@poshtkohi.info
	Website: http://www.poshtkohi.info
*/

#include "xThreadCollection.h"

//**************************************************************************************************************//
/*void cleanupHandler(void *arg)
{
	//pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, null);
	printf("In the cleanup handler\n");
	CleanupInfo *info = (CleanupInfo *)arg;
	//info->obj->ReleaseCriticalSesctions();
	//info->obj->Deconstructor(info->obj);//printf("mmmmmm\n");
	info->barrier->SignalAndWait();
}*/
//----------------------------------------------------
namespace Parvicursor
{
   namespace xThread
   {
	   	//----------------------------------------------------
		xThreadCollection::ThreadInfo::ThreadInfo()
		{
			this->lib_handle = null;
			this->serializedBuffer = null;
			this->serializedDLLBuffer = null;
			this->objInstance = null;
			this->state = System::Threading::Unknown;
			this->worker = null;
			this->serializedBuffer_size = -1; // the length allocated by new[] operator.
			this->currentSerializedBuffer_size = -1;
			this->numbered = false;
		}
		//----------------------------------------------------
		xThreadCollection::ThreadInfo::~ThreadInfo()
		{
			/*if(this->serializer != null)
			{
				delete this->serializer;
				this->objInstance = null;
			}*/
		}
		//----------------------------------------------------
		void *xThreadCollection::ThreadInfo::Wrapper_To_Call_StartProc(void *pt2Object)
		{

/*
			Setting the cancelability type to PTHREAD_CANCEL_ASYNCHRONOUS is rarely
			useful.  Since the thread could be canceled at any time, it cannot safely
			reserve resources (e.g., allocating memory with malloc(3)), acquire mutexes,
			semaphores, or locks, and so on.  Reserving resources is unsafe because the
			application has no way of knowing what the state of these resources is when
			the thread is canceled; that is, did cancellation occur before the resources
			were reserved, while they were reserved, or after they were released?
			Furthermore, some internal data structures (e.g., the linked list of free
			blocks managed by the malloc(3) family of functions) may be left in an
			inconsistent state if cancellation occurs in the middle of the function call.
			Consequently, clean-up handlers cease to be useful.  Functions that can be
			safely asynchronously canceled are called async-cancel-safe functions.
			POSIX.1-2001 only requires that pthread_cancel(3), pthread_setcancelstate(),
			and pthread_setcanceltype() be async-cancel-safe.  In general, other library
			functions can't be safely called from an asynchronously cancelable thread.
			One of the few circumstances in which asynchronous cancelability is useful is
			for cancellation of a thread that is in a pure compute-bound loop.
*/

			if(pt2Object != null)
			{
				::pthread_setcanceltype(PTHREAD_CANCEL_ENABLE, null);
				::pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, null);
				//xThreadBase *mySelf = (xThreadBase *)pt2Object;
				xThreadCollection::ThreadInfo *mySelf = (xThreadCollection::ThreadInfo *)pt2Object;
				if(mySelf->objInstance != null)
				{
					//pthread_cleanup_push(cleanupHandler, (void *)mySelf->objInstance);
					/*CleanupInfo *info = new CleanupInfo();
					info->obj = mySelf->objInstance;
					info->barrier = new Barrier(2);
					mySelf->cleanupInfo = info;
					ptw32_cleanup_t _cleanup;
					ptw32_push_cleanup(&_cleanup, (ptw32_cleanup_callback_t)cleanupHandler, (void *)mySelf->objInstance);*/

					mySelf->state = System::Threading::Running;
					((xThreadBase *)(mySelf->objInstance))->Start();
					//((xThreadBase *)(mySelf->objInstance))->InternalStart();
					mySelf->state = System::Threading::Stopped;

					//ptw32_pop_cleanup(0);
				}
				// call member
				//mySelf->Start();
			}

			return pt2Object;
			//return null;
		}
		//----------------------------------------------------
		xThreadCollection::xThreadCollection()
		{
			//this->set = null;
			this->set = new ArrayList();
			this->fromXThreadClientClass = false;
		}
		//----------------------------------------------------
		xThreadCollection::~xThreadCollection()
		{
			if(this->set != null)
			{
				String dir;
				for(register Int32 i = 0 ; i < this->set->get_Count() ; i++)
				{
					ThreadInfo *temp = (ThreadInfo *)this->set->get_Value(i);

					/*if(temp->serializedBuffer != null)
					{
						if(this->fromXThreadClientClass)
							temp->obj->FreeDllMemory(temp->serializedBuffer);
						else
							delete temp->serializedBuffer;
					}*/
					/*if(temp->worker != null)
					{
						//temp->worker->Suspend();
						//temp->worker->Abort();
						temp->worker->Abort2();
						temp->state = System::Threading::Aborted;
						delete temp->worker;
						temp->worker = null;
					}*/

					//exit(-1);
					if(temp->state == System::Threading::Running || temp->state == System::Threading::Suspended)
					{
						//HANDLE handle = pthread_getw32threadhandle_np(temp->thread);
						//int err1 = TerminateThread(handle, 0);
						//printf("pthread_cancel: %d\n" , pthread_cancel(temp->thread)); //			exit(0);

						if(temp->worker != null)
						{
							temp->state = System::Threading::AbortRequested;
							printf("pthread_cancel: %d\n" , pthread_cancel(temp->worker->thread)); //			exit(0);
							temp->state = System::Threading::Aborted;

							/*temp->cleanupInfo->barrier->SignalAndWait();
							delete temp->cleanupInfo->barrier;
							delete temp->cleanupInfo;*/
						}
						//while(pthread_cancel(temp->thread) != 0){}
						//pthread_cancel(temp->thread);
						//pthread_yield();


						//pthread_kill(temp->thread, 22);
						/*HANDLE handle = pthread_getw32threadhandle_np(temp->thread);
						//SuspendThread(handle);
					if(handle == null)
							goto Here;
						int err1 = TerminateThread(handle, 0);
						if(err1 == 0)
							cout << ErrorString(GetLastError()) << endl;
						int err2 = CloseHandle(handle);
						if(err2 == 0)
							cout << ErrorString(GetLastError()) << endl;
						cout << "TerminateThread: " <<  err1 << endl;
						cout << "CloseHandle: " <<  err2 << endl;
						//temp->objInstance->Exit();*/
						//_endthread();
					}

					if(temp->worker != null)
					{
						delete temp->worker;
						temp->worker = null;
					}

					if(temp->serializedBuffer != null)
					{
						delete temp->serializedBuffer;
						temp->serializedBuffer = null;
					}

					if(temp->serializedDLLBuffer != null)
					{
						temp->objInstance->FreeDllMemory(temp->serializedDLLBuffer);
						temp->serializedDLLBuffer = null;
					}

					if(!this->fromXThreadClientClass)
					{
						if(temp->objInstance != null)
						{
							//temp->objInstance->FreeDllMemory(temp->objInstance);printf("mmmmmm\n");
							//temp->objInstance = null;

							temp->objInstance->ReleaseCriticalSesctions();

							temp->objInstance->Deconstructor(temp->objInstance);//printf("mmmmmm\n");
							temp->objInstance = null;
						}
					}

					/*printf("helloxxxxxxxxxxxx\n");
					Thread::Sleep(10000000000000);*/

					if(temp->lib_handle != null)
					{

#if defined WIN32 || WIN64

						//cout << "He: "<< endl;
						//cout << "FreeLibrary: "<<
						::FreeLibrary(temp->lib_handle); // << endl;
						//FreeLibraryAndExitThread(temp->lib_handle, -1);

#else

						::dlclose(temp->lib_handle);

#endif
						temp->lib_handle = null;

					}

					dir = temp->workingDirectory;

					if(temp != null)
					{
						delete temp;
						temp = null;
					}
				}

				if(dir.get_BaseStream() != null)
					if(Directory::Exists(dir))
						Directory::Delete(dir, true);

				this->set->Clear();
				delete this->set;
				this->set = null;

			}
		}
		//----------------------------------------------------
		void xThreadCollection::AddNewThreadInstance(const xThreadBase *objInstance, const String &DllFilename)
		{
			if(DllFilename.get_BaseStream() == null)
				throw ArgumentNullException("DllFilename is a null reference.");


			if(objInstance == null)
				throw ArgumentNullException("objInstance is a null reference.");

			//file not foud exception

			if(!File::Exists(DllFilename))
				throw IOException(DllFilename + " could not be found.");

			this->fromXThreadClientClass = true;
			xThreadBase *obj = (xThreadBase *)objInstance;

			char *buffer = null;
			int len = -1;
			obj->Serialize(&buffer, &len);
			/*DeSerializer de(buffer, len);
			cout << de.Read<Int32>() << endl; */
			//delete serializer;
			//obj->FreeDllMemory(buffer);

			ThreadInfo *info = new ThreadInfo();
			info->DllFilename = DllFilename;
			info->objInstance = obj;
			info->guid = Guid::NewGuid();
			info->serializedDLLBuffer = buffer;
			info->serializedBuffer_size = len;
			info->currentSerializedBuffer_size = len;
			//info->obj = obj;

			//info->objInstance = reinterpret_cast<xThreadBase *>((xThreadBase *)objInstance);


			this->set->Add(info);

			return ;
		}
		//----------------------------------------------------
		Serializer *xThreadCollection::Serialize() // for client-use
		{

			if(this->set == null)
				return null;

			if(this->set->get_Count() == 0)
				return null;

			this->fromXThreadClientClass = true;

			Int32 bufferSize = 0;
			bufferSize += sizeof(Int32); //for threads count
			FileStream *fs = null;

			for(register Int32 i = 0 ; i < this->set->get_Count() ; i++)
			{
				ThreadInfo *temp = (ThreadInfo *)this->set->get_Value(i);
				fs = new FileStream(temp->DllFilename, Open, Read, 20);
				bufferSize += (sizeof(Int32) + (Int32)fs->get_Length());
				fs->Close();

				bufferSize += (sizeof(Int32) + temp->serializedBuffer_size);
				bufferSize += (sizeof(Int32) + temp->guid.get_Length());

				delete fs;
			}

			Serializer *serializer = new Serializer(bufferSize);
			serializer->Write<Int32>(this->set->get_Count());

			//cout << "ooo   1" << bufferSize << endl;

			for(register Int32 i = 0 ; i < this->set->get_Count() ; i++)
			{
				ThreadInfo *temp = (ThreadInfo *)this->set->get_Value(i);
				fs = new FileStream(temp->DllFilename, Open, Read, 10);
				Int32 size = (Int32)fs->get_Length();
				char *buffer = new char[size];
				try
				{
					fs->Read(buffer, 0, size);
					fs->Close();
					delete fs;

					serializer->Write(temp->guid);
					serializer->Write<Int32>(temp->currentSerializedBuffer_size); serializer->Write(temp->serializedDLLBuffer, temp->currentSerializedBuffer_size);
					serializer->Write<Int32>(size); serializer->Write(buffer, size);

					delete buffer;
				}

				catch(System::Exception &e)
				{
					delete buffer;
					delete fs;
					delete serializer;
					throw Exception(e.get_Message());
				}
				catch(...)
				{
					delete buffer;
					delete fs;
					delete serializer;
					throw Exception("Unknown exception was occured from Parvicursor::xThread::xThreadCollection::Serialize()");

				}
			}
			//cout << bufferSize << endl;
			return serializer;
		}
		//----------------------------------------------------
		void xThreadCollection::DeSerialize(DeSerializer &deserializer, Int32 MaxThreadSupport, const String &sessionGuid)//for server-use
		{
			Int32 threadCollectionCount = 0;
			this->fromXThreadClientClass = false;
			threadCollectionCount = deserializer.Read<Int32>();

			if(threadCollectionCount <= 0 || threadCollectionCount > MaxThreadSupport)
				throw ArgumentOutOfRangeException("Thread count must be greater than zero and less than " + Convert::ToString(MaxThreadSupport) + ".");

			register Int32 currentThreadCollectionCount = 1;
			for(register Int32 i = 0 ; i < threadCollectionCount ; i++, currentThreadCollectionCount++)
			{
				ThreadInfo *info = null;
				char *buff = null;
				char *buffer = null;
				FileStream *fs = null;
				try
				{
					info = new ThreadInfo();

					info->guid = deserializer.Read();
					info->serializedBuffer_size = deserializer.Read<Int32>();  buffer = new char[info->serializedBuffer_size];  info->currentSerializedBuffer_size = info->serializedBuffer_size; deserializer.Read(buffer, info->serializedBuffer_size); info->serializedBuffer = buffer;

					//------------------Write DllFilename-------------------------------------------
					Int32 size = deserializer.Read<Int32>();
					buff = new char[size]; deserializer.Read(buff, size);

					String dir = Directory::GetCurrentDirectory().Replace("\\", "/") + "/xThreadSessions/" + sessionGuid + "/";
					String file = dir + info->guid + ".dll";
					info->DllFilename = file;
					info->workingDirectory = dir;
					//cout << dir.get_BaseStream() << endl;//
					Directory::CreateDirectory(dir);

					fs = new FileStream(file, Create, Write, 9);
					fs->Write(buff, 0, size);
					//cout << "file size: " << size << endl;//
					//cout << info->guid.get_BaseStream() << endl;//
					//cout << "serialized thread size: " << info->serializedBuffer_size << endl;//
					fs->Close();
					delete fs;
					delete buff;
					//------------------------------------------------------------------------------

					this->set->Add(info);
				}

				catch(System::Exception &e)
				{
					if(buff != null)
						delete buff;

					if(buffer != null)
						delete buffer;

					if(fs != null)
						delete fs;

					if(info != null)
						delete info;

					//cout << e.get_Message().get_BaseStream() << endl;//
					throw Exception(e.get_Message());
				}
				catch(...)
				{
					if(buff != null)
						delete buff;

					if(buffer != null)
						delete buffer;

					if(fs != null)
						delete fs;

					if(info != null)
						delete info;

					throw Exception("Unknown exception was occured from Parvicursor::xThread::xThreadCollection::DeSerialize()");

				}
			}

		}
		//----------------------------------------------------
		/*ArrayList &xThreadCollection::get_Collection() const
		{
			return (ArrayList &)this->set;
		}*/
		//----------------------------------------------------
		Int32 xThreadCollection::get_Count()
		{
			if(this->set != null)
				return this->set->get_Count();
			else
				return -1;
		}
		//----------------------------------------------------
		xThreadCollection::ThreadInfo *xThreadCollection::get_Value(Int32 index)
		{
			if(this->set != null)
				return (ThreadInfo *)this->set->get_Value(index);
			else
				return null;
		}
   		//----------------------------------------------------
   };
};
//**************************************************************************************************************//
