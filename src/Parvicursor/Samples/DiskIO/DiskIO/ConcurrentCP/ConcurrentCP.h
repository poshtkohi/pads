#ifndef __DiskIO_ConcurrentCP_h__
#define __DiskIO_ConcurrentCP_h__

#include "../../../../Parvicursor/general.h"
#include "../../../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../../../Parvicursor/System/Environment/Environment.h"
#include "../../../../Parvicursor/System.IO/FileStream/FileStream.h"
#include "../../../../Parvicursor/System.Threading/Thread/Thread.h"
#include "../../../../Parvicursor/System.Threading/ThreadPool/ThreadPool.h"
#include "../../../../Parvicursor/System.Threading/Mutex/Mutex.h"
#include "../../../../Parvicursor/System.Threading/ConditionVariable/ConditionVariable.h"
#include "../../../../Parvicursor/System.Threading/Barrier/Barrier.h"
#include "../../../../Parvicursor/System.IO/Directory/Directory.h"
#include "../../../../Parvicursor/System/String/String.h"
#include "../../../../Parvicursor/System.Collections/ArrayList/ArrayList.h"

using namespace System;
using namespace System::IO;
using namespace System::Threading;
using namespace System::Collections;
//**************************************************************************************************************//

namespace DiskIO
{
	//----------------------------------------------------
	class ConcurrentCP : public Object
	{
		/*---------------------fields----------------*/
		private: struct MethodInfo : public Object
		{
			String *readFilename;
			String *writeDir;
			Int32 bufferSize;
			public: MethodInfo(String *readFilename, String *writeDir, Int32 bufferSize)
			{
				this->readFilename = readFilename;
				this->writeDir = writeDir;
				this->bufferSize = bufferSize;
			}
			public: ~MethodInfo()
			{
				if(readFilename != null) // allocated in FindRootDirectoryFiles or RecursiveDirectoryTreeTraversal
					delete readFilename;
				if(writeDir != null) // allocated in FindRootDirectoryFiles or RecursiveDirectoryTreeTraversal
					delete writeDir;
			}
		};
		/*---------------------methods----------------*/
		private: static void MethodCallback(Object *state)
		{
			MethodInfo *info = (MethodInfo *)state;

			//printf("%s\n", info->readFilename->get_BaseStream());//
			//printf("methodID: %d %s\n", info->methodID, info->writeDir->get_BaseStream());//

			String writeFilename = String(*info->writeDir) + "/" + info->readFilename->Substring(info->readFilename->LastIndexOf("/") + 1);

			FileStream *fsRead = null;
			FileStream *fsWrite = null;

			printf("Copying '%s' to '%s'\n", info->readFilename->get_BaseStream(), writeFilename.get_BaseStream());

			try
			{
				fsRead = new FileStream(*info->readFilename, System::IO::Open, System::IO::Read, 8*1024);
				fsWrite = new FileStream(writeFilename, System::IO::OpenOrCreate, System::IO::Write, 8*1024);
			}
			catch(Exception &e)
			{
				printf("1. Exception Message: %s\n", e.get_Message().get_BaseStream());
				if(fsRead != null)
					delete fsRead;
				if(fsWrite != null)
					delete fsWrite;
				delete info; // allocated in FindRootDirectoryFiles or RecursiveDirectoryTreeTraversal
				return ;
			}
			catch(...)
			{
				printf("2. Unknown Exception\n");
				if(fsRead != null)
					delete fsRead;
				if(fsWrite != null)
					delete fsWrite;
				delete info; // allocated in FindRootDirectoryFiles or RecursiveDirectoryTreeTraversal
				return ;
			}

			Int32 read;
			if(info->bufferSize <= 0)
				info->bufferSize = 256*1024;
			char *buffer = new char[info->bufferSize];
			while(true)
			{
				try
				{
					read = fsRead->Read(buffer, 0, info->bufferSize);
					if(read <= 0)
						break;
					fsWrite->Write(buffer, 0, read);
				}
				catch(IOException &e)
				{
					printf("3. Exception Message: %s\n", e.get_Message().get_BaseStream());
					break;
				}
				catch(Exception &e)
				{
					printf("4. Exception Message: %s\n", e.get_Message().get_BaseStream());
					break;
				}
				catch(...)
				{
					printf("5. Unknown Exception\n");
					break;
				}
			}
			//fsWrite->Close();
			//fsRead->Close();
			delete fsRead;
			delete fsWrite;
			delete buffer;

			delete info; // allocated in FindRootDirectoryFiles or RecursiveDirectoryTreeTraversal
			return;
		}
		/*-------------------------------------------*/
		private: static void RecursiveDirectoryTreeTraversal(const String &dir, const String &newdir, const String &root, ThreadPool *pool, Int32 bufferSize)
		{
			ArrayList *dirs = Directory::GetDirectories(dir);
			if(dirs == null)
				return;

			for(Int32 i = 0 ; i < dirs->get_Count() ; i++)
			{
				String *s = (String *)dirs->get_Value(i);
				String newstr = newdir + s->Substring(root.get_Length());
				if(!Directory::Exists(newstr))
					Directory::CreateDirectory(newstr);
				ArrayList *files = Directory::GetFiles(*s);

				if(files != null)
				{
					for(Int32 j = 0 ; j < files->get_Count() ; j++)
					{
						String *ss = (String *)files->get_Value(j);
						MethodInfo *info = new MethodInfo(ss, new String(newstr), bufferSize);
						pool->QueueUserWorkItem(MethodCallback, info);
					}
					files->Clear();
					delete files;
				}
				RecursiveDirectoryTreeTraversal(*s, newdir, root, pool, bufferSize);
				delete s;
			}

			dirs->Clear();
			delete dirs;
		}
		/*-------------------------------------------*/
		private: static void FindRootDirectoryFiles(const String &newdir, const String &root, ThreadPool *pool, Int32 bufferSize)
		{

			ArrayList *files = Directory::GetFiles(root);

			if(files != null)
			{
				for(Int32 j = 0 ; j < files->get_Count() ; j++)
				{
					String *ss = (String *)files->get_Value(j);
					//printf("%s\n", ss->get_BaseStream());
					MethodInfo *info = new MethodInfo(ss, new String(newdir), bufferSize);
					pool->QueueUserWorkItem(MethodCallback, info);
				}
				files->Clear();
				delete files;
			}
		}
		/*-------------------------------------------*/
		// Creates a recursive directory pattern, e.g., /a/b/c/d/e.
		private: static void CreateDirectoryEx(const String &path)
		{
			String str = path;
			String temp;
			Int32 n = 0;
			while(true)
			{
				n = str.IndexOf("/", n + 1);
				if(n <= 0)
					break;

				temp = str.Substring(0, n);
				if(!Directory::Exists(temp))
					Directory::CreateDirectory(temp);
			}

			if(!Directory::Exists(path))
				Directory::CreateDirectory(path);
		}
		/*-------------------------------------------*/
		public: static int Parvicursor_main(int argc, char *argv[])
		{
			Int32 bufferSize = 256*1024;
			Int32 threadNum = Environment::get_ProcessorCount()*4;
			String src = "C:/Users/Administrator/Desktop/Matrix Multiplication"; // to change
			String dest = "C:/test/c"; // to change
			if(!Directory::Exists(src))
			{
				printf("src does not exist.\n");
				return 0;
			}
			// Initilizes the thread pool instance.
			ThreadPool pool = ThreadPool(threadNum, 10000);
			try
			{
				if(!Directory::Exists(dest))
					CreateDirectoryEx(dest);

				FindRootDirectoryFiles(dest, src, &pool, bufferSize);
				RecursiveDirectoryTreeTraversal(src, dest, src, &pool, bufferSize);
			}
			catch(Exception &e)
			{
				printf("6. Exception Message: %s\n", e.get_Message().get_BaseStream());
			}
			catch(...)
			{
				printf("7. Unknown Exception\n");
			}

			// Waits until all methods (or threads within the ThreadPool) complete.
			pool.WaitOnTaskQueue();
			//while(pool.get_Count() != 0)
			//Thread::Sleep(1000);
			//pool.JoinAltogether();

			//getchar();
			return 0;
		}
		/*-------------------------------------------*/
	};
	//----------------------------------------------------
};
//**************************************************************************************************************//

#endif
