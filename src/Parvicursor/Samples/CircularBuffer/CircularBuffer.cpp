// CircularBuffer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <string.h>

//---------------------------------------
#pragma comment(lib, "../../ParvicursorLib/Debug/ParvicursorLib.lib")
#include "../../Parvicursor/general.h"
#include "../../Parvicursor/System.Threading/Thread/Thread.h"
#include "../../Parvicursor/System/BasicTypes/BasicTypes.h"
#include "../../Parvicursor/System.IO/FileStream/FileStream.h"
#include "../../Parvicursor/System/String/String.h"
//---------------------------------------
using namespace System;
using namespace System::IO;
using namespace System::Threading;

void *Wrapper_To_Call_Producer(void *pt2Object);
void *Wrapper_To_Call_Consumer(void *pt2Object);
Int32 buffer_device(void);
Int32 unbuffer_device(void);

FileStream *fsProducer;
FileStream *fsConsumer;
String readFilename = "E:/dl/Software/openSUSE-11.0-DVD-i386.iso";  //to change
String writeFilename = "G:/openSUSE-11.0-DVD-i386.iso";   //to change
Int32 n = 0;
Int32 last_n = 0;
Int64 currentOffset = 0;
Int64 nextOffset = 0;
Int32 blocks = 0;
//integers to index circular_buffer
Int32 start = 0, end = 0;
//---------------------------------------

#define BLOCK_SIZE 256*1024 // 64KB
//#define BUFFER_SIZE 10*BLOCK_SIZE//1MB // 256KB
#define BUFFER_SIZE 10*BLOCK_SIZE//1MB
#define END_OF_FILE -1
#define BUFFER_IS_FULL -2
#define BUFFER_IS_EMPTY -3
#define END_OF_FILE_BUT_BUFFER_NOT_EMPTY -4
#define END_OF_FILE_AND_BUFFER_IS_EMPTY -5

 
//a buffer with BUFFER_SIZE slots
char circular_buffer[BUFFER_SIZE];
static pthread_mutex_t _mutex_ = PTHREAD_MUTEX_INITIALIZER;
 //--------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	fsProducer = new FileStream(readFilename, System::IO::Open, System::IO::Read, 9);
	fsConsumer = new FileStream(writeFilename, System::IO::Create, System::IO::Write, 9);
	fsConsumer->SetLength(0);

	Thread *producer = new Thread(Wrapper_To_Call_Producer, null);
	Thread *consumer = new Thread(Wrapper_To_Call_Consumer, null);

	producer->Start();
	producer->SetDetached();

	consumer->Start();
	consumer->SetDetached();
	

	while(true)
		Thread::Sleep(10000000);

    return 0;
}
//--------------------------------------------------------------------------------
void *Wrapper_To_Call_Producer(void *pt2Object)
{
	pthread_setcanceltype(PTHREAD_CANCEL_ENABLE, null);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, null);

	Int32 ret;
	while(true)
	{
		ret = buffer_device();

		switch(ret)
		{
			case END_OF_FILE:
			{
				printf("BUFFER_IS_FULL\n");
				return pt2Object;
			}
			case BUFFER_IS_FULL:
			{
				printf("BUFFER_IS_FULL\n");
				Thread::Sleep(1);
				break;
			}
			case BUFFER_IS_EMPTY:
			{
				printf("BUFFER_IS_EMPTY\n");
				break;
			}
			case END_OF_FILE_BUT_BUFFER_NOT_EMPTY:
			{
				printf("END_OF_FILE_BUT_BUFFER_NOT_EMPTY:Producer:ThreadExit\n");
				fsProducer->Flush();
				fsProducer->Close();
				delete fsProducer;
				fsProducer = null;
				return pt2Object;
			}
			case END_OF_FILE_AND_BUFFER_IS_EMPTY:
			{
				printf("END_OF_FILE_AND_BUFFER_IS_EMPTY:Producer:ThreadExit\n");
				fsProducer->Flush();
				fsProducer->Close();
				delete fsProducer;
				fsProducer = null;
				return pt2Object;
			}
			default:
			{
				break;
			}
		}
		//Thread::Sleep(1);
	}
	return pt2Object;
}
//--------------------------------------------------------------------------------
void *Wrapper_To_Call_Consumer(void *pt2Object)
{
	//pthread_setcanceltype(PTHREAD_CANCEL_ENABLE, null);
	//pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, null);

	Int32 ret;
	while(true)
	{
		ret = unbuffer_device();

		switch(ret)
		{
			case END_OF_FILE:
			{
				printf("BUFFER_IS_FULL\n");
				fsConsumer->Flush();
				fsConsumer->Close();
				delete fsConsumer;
				fsConsumer = null;
				return pt2Object;
			}
			case BUFFER_IS_FULL:
			{
				printf("BUFFER_IS_FULL\n");
				//Thread::Sleep(1);
				break;
			}
			case BUFFER_IS_EMPTY:
			{
				printf("BUFFER_IS_EMPTY\n");
				Thread::Sleep(1);
				break;
			}
			case END_OF_FILE_BUT_BUFFER_NOT_EMPTY:
			{
				printf("END_OF_FILE_BUT_BUFFER_NOT_EMPTY:Consumer\n");
				Thread::Sleep(1);
				break;
			}
			case END_OF_FILE_AND_BUFFER_IS_EMPTY:
			{
				printf("END_OF_FILE_AND_BUFFER_IS_EMPTY:Consumer:ThreadExit\n");
				fsConsumer->Flush();
				fsConsumer->Close();
				delete fsConsumer;
				fsConsumer = null;
				return pt2Object;
			}
			default:
			{
				break;
			}
		}
		//Thread::Sleep(1);
	}
	return pt2Object;
	//return null;
}
//--------------------------------------------------------------------------------
Int32 buffer_device(void)
{
	int temp = (end + BLOCK_SIZE) % BUFFER_SIZE;
    //Use modulo as a trick to wrap around the end of the buffer back to the beginning
    if (temp != start)
	{
		pthread_mutex_lock(&_mutex_);

		n = fsProducer->Read(circular_buffer, end, BLOCK_SIZE);
		if(n <= 0)
		{
			currentOffset = -1;
			nextOffset = -1;
			if (end != start)
			{
				pthread_mutex_unlock(&_mutex_);
				return END_OF_FILE_BUT_BUFFER_NOT_EMPTY;
			}
			else
			{
				pthread_mutex_unlock(&_mutex_);
				return END_OF_FILE_AND_BUFFER_IS_EMPTY;
			}
		}
		/*if(n != BLOCK_SIZE)
			printf("n: %d\n", n);*/

		blocks++;
		currentOffset = nextOffset;
		nextOffset += n;
		last_n = n;
        //circular_buffer[end] = c;
        end = temp;
		//printf("temp: %d, end: %d, offset: %d\n", temp, end, currentOffset);
		pthread_mutex_unlock(&_mutex_);
		return 0;
    }
	else
		return BUFFER_IS_FULL;
	//printf("%d\n", 201%200);
    //otherwise, the buffer is full; don't do anything. you might want to return an error code to notify the writing process that the buffer is full.
}
//-------------------------------------------------------------------------------- 
Int32 unbuffer_device(void)
{
    if (end != start)
	{        
		//char temp = circular_buffer[start];
		if(currentOffset == -1 && blocks == 1)
		{
			pthread_mutex_lock(&_mutex_);
			fsConsumer->Write(circular_buffer, start, last_n); //printf("\n\n\n\kljbkldsfjgk\n\n\n\n");
			blocks--;
			pthread_mutex_unlock(&_mutex_);
			return END_OF_FILE_AND_BUFFER_IS_EMPTY;
		}
		else
		{
			pthread_mutex_lock(&_mutex_);
			fsConsumer->Write(circular_buffer, start, BLOCK_SIZE);
			start = (start + BLOCK_SIZE) % BUFFER_SIZE;
			//return(temp);
			blocks--;
			pthread_mutex_unlock(&_mutex_);
			return 0;
		}
    }
    //otherwise, the buffer is empty; return an error code
    else
	{
		if(currentOffset == -1)
			return END_OF_FILE_AND_BUFFER_IS_EMPTY;
		else
			return BUFFER_IS_EMPTY;
	}
}
//--------------------------------------------------------------------------------