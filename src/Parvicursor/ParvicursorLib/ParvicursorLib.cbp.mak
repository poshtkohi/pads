# -*- mode: Makefile; -*-
# -----------------------------------------
# project ParvicursorLib


export PATH := /opt/wx/2.8/bin:$(PATH)
export LD_LIBRARY_PATH := /opt/wx/2.8/lib:$(LD_LIBRARY_PATH)

_WX = /home/gr/projects/gui/codeblocks/wx
_WX.LIB = $(_WX)/lib
_WX.INCLUDE = $(_WX)/include

_CB = /home/gr/projects/gui/codeblocks/cb/src
_CB.INCLUDE = $(_CB)/include
_CB.LIB = $(_CB)/devel



CFLAGS_C = $(filter-out -include "sdk.h",$(CFLAGS))

# -----------------------------------------

# MAKE_DEP = -MMD -MT $@ -MF $(@:.o=.d)

CFLAGS = -w 
INCLUDES = 
LDFLAGS =  -s
RCFLAGS = 
LDLIBS = $(T_LDLIBS)  -lstdc++

LINK_exe = gcc -o $@ $^ $(LDFLAGS) $(LDLIBS)
LINK_con = gcc -o $@ $^ $(LDFLAGS) $(LDLIBS)
LINK_dll = gcc -o $@ $^ $(LDFLAGS) $(LDLIBS) -shared
LINK_lib = rm -f $@ && ar rcs $@ $^
COMPILE_c = gcc $(CFLAGS_C) -o $@ -c $< $(MAKEDEP) $(INCLUDES)
COMPILE_cpp = g++ $(CFLAGS) -o $@ -c $< $(MAKEDEP) $(INCLUDES)
COMPILE_rc = windres $(RCFLAGS) -J rc -O coff -i $< -o $@ -I$(dir $<)

%.o : %.c ; $(COMPILE_c)
%.o : %.cpp ; $(COMPILE_cpp)
%.o : %.cxx ; $(COMPILE_cpp)
%.o : %.rc ; $(COMPILE_rc)
.SUFFIXES: .o .d .c .cpp .cxx .rc

all: all.before all.targets all.after

all.before :
	-
all.after : $(FIRST_TARGET)
	
all.targets : Debug_target 

clean :
	rm -fv $(clean.OBJ)
	rm -fv $(DEP_FILES)

.PHONY: all clean distclean

# -----------------------------------------
# Debug_target

Debug_target.BIN = bin/Debug/ParvicursorLib.lib
Debug_target.OBJ = ../Parvicursor/Parvicursor/Collections/SinglyLinkedList.o ../Parvicursor/Parvicursor/xDFS/xDFSClients/DownloadClient/DownloadClient.o ../Parvicursor/Parvicursor/xDFS/xDFSClients/UploadClient/UploadClient.o ../Parvicursor/Parvicursor/xDFS/FileTransferInfo/FileTransferInfo.o ../Parvicursor/Parvicursor/Parvicursor.Cryptography/AesEngine/AesEngine.o ../Parvicursor/Parvicursor/Parvicursor.Cryptography/RsaEngine/RsaEngine.o ../Parvicursor/Parvicursor/Parvicursor.Cryptography/ShaEngine/ShaEngine.o ../Parvicursor/Parvicursor/Parvicursor.Net/ParvicursorSocket/ParvicursorSocket.o ../Parvicursor/Parvicursor/Profiler/ResourceProfiler.o ../Parvicursor/Parvicursor/Serialization/DeSerializer.o ../Parvicursor/Parvicursor/Serialization/Serializer.o ../Parvicursor/Parvicursor/xThread/xThreadBase.o ../Parvicursor/Parvicursor/xThread/xThreadClient/xThreadClient.o ../Parvicursor/Parvicursor/xThread/xThreadCollection.o ../Parvicursor/StaticFunctions/StaticFunctions.o ../Parvicursor/System.Collections/ArrayList/ArrayList.o ../Parvicursor/System.Collections/Hashtable/Hashtable.o ../Parvicursor/System.Collections/Queue/Queue.o ../Parvicursor/System.IO/Directory/Directory.o ../Parvicursor/System.IO/DirectoryNotFoundException/DirectoryNotFoundException.o ../Parvicursor/System.IO/File/File.o ../Parvicursor/System.IO/FileStream/FileStream.o ../Parvicursor/System.IO/IOException/IOException.o ../Parvicursor/System.Net.Sockets/Socket/Socket.o ../Parvicursor/System.Net.Sockets/SocketException/SocketException.o ../Parvicursor/System.Net/Dns/Dns.o ../Parvicursor/System.Net/IPAddress/IPAddress.o ../Parvicursor/System.Net/IPEndPoint/IPEndPoint.o ../Parvicursor/System.Net/IPHostEntry/IPHostEntry.o ../Parvicursor/System.Net/NetworkCredential/NetworkCredential.o ../Parvicursor/System.Threading/Barrier/Barrier.o ../Parvicursor/System.Threading/ConditionVariable/ConditionVariable.o ../Parvicursor/System.Threading/Fiber/Fiber.o ../Parvicursor/System.Threading/Mutex/Mutex.o ../Parvicursor/System.Threading/Thread/Thread.o ../Parvicursor/System.Threading/ThreadPool/ThreadPool.o ../Parvicursor/System.Threading/ThreadStateException/ThreadStateException.o ../Parvicursor/System.Threading/Timer/Timer.o ../Parvicursor/System/ArgumentException/ArgumentException.o ../Parvicursor/System/ArgumentException/ArgumentNullException.o ../Parvicursor/System/ArgumentException/ArgumentOutOfRangeException.o ../Parvicursor/System/Convert/Convert.o ../Parvicursor/System/DateTime/DateTime.o ../Parvicursor/System/Environment/Environment.o ../Parvicursor/System/Exception/Exception.o ../Parvicursor/System/FormatException/FormatException.o ../Parvicursor/System/Guid/Guid.o ../Parvicursor/System/InvalidOperationException/InvalidOperationException.o ../Parvicursor/System/NotSupportedException/NotSupportedException.o ../Parvicursor/System/Object/Object.o ../Parvicursor/System/ObjectDisposedException/ObjectDisposedException.o ../Parvicursor/System/OverflowException/OverflowException.o ../Parvicursor/System/Random/Random.o ../Parvicursor/System/String/String.o ../Parvicursor/System/Type/Type.o 
DEP_FILES += ../Parvicursor/Parvicursor/Collections/SinglyLinkedList.d ../Parvicursor/Parvicursor/xDFS/xDFSClients/DownloadClient/DownloadClient.d ../Parvicursor/Parvicursor/xDFS/xDFSClients/UploadClient/UploadClient.d ../Parvicursor/Parvicursor/xDFS/FileTransferInfo/FileTransferInfo.d ../Parvicursor/Parvicursor/Parvicursor.Cryptography/AesEngine/AesEngine.d ../Parvicursor/Parvicursor/Parvicursor.Cryptography/RsaEngine/RsaEngine.d ../Parvicursor/Parvicursor/Parvicursor.Cryptography/ShaEngine/ShaEngine.d ../Parvicursor/Parvicursor/Parvicursor.Net/ParvicursorSocket/ParvicursorSocket.d ../Parvicursor/Parvicursor/Profiler/ResourceProfiler.d ../Parvicursor/Parvicursor/Serialization/DeSerializer.d ../Parvicursor/Parvicursor/Serialization/Serializer.d ../Parvicursor/Parvicursor/xThread/xThreadBase.d ../Parvicursor/Parvicursor/xThread/xThreadClient/xThreadClient.d ../Parvicursor/Parvicursor/xThread/xThreadCollection.d ../Parvicursor/StaticFunctions/StaticFunctions.d ../Parvicursor/System.Collections/ArrayList/ArrayList.d ../Parvicursor/System.Collections/Hashtable/Hashtable.d ../Parvicursor/System.Collections/Queue/Queue.d ../Parvicursor/System.IO/Directory/Directory.d ../Parvicursor/System.IO/DirectoryNotFoundException/DirectoryNotFoundException.d ../Parvicursor/System.IO/File/File.d ../Parvicursor/System.IO/FileStream/FileStream.d ../Parvicursor/System.IO/IOException/IOException.d ../Parvicursor/System.Net.Sockets/Socket/Socket.d ../Parvicursor/System.Net.Sockets/SocketException/SocketException.d ../Parvicursor/System.Net/Dns/Dns.d ../Parvicursor/System.Net/IPAddress/IPAddress.d ../Parvicursor/System.Net/IPEndPoint/IPEndPoint.d ../Parvicursor/System.Net/IPHostEntry/IPHostEntry.d ../Parvicursor/System.Net/NetworkCredential/NetworkCredential.d ../Parvicursor/System.Threading/Barrier/Barrier.d ../Parvicursor/System.Threading/ConditionVariable/ConditionVariable.d ../Parvicursor/System.Threading/Fiber/Fiber.d ../Parvicursor/System.Threading/Mutex/Mutex.d ../Parvicursor/System.Threading/Thread/Thread.d ../Parvicursor/System.Threading/ThreadPool/ThreadPool.d ../Parvicursor/System.Threading/ThreadStateException/ThreadStateException.d ../Parvicursor/System.Threading/Timer/Timer.d ../Parvicursor/System/ArgumentException/ArgumentException.d ../Parvicursor/System/ArgumentException/ArgumentNullException.d ../Parvicursor/System/ArgumentException/ArgumentOutOfRangeException.d ../Parvicursor/System/Convert/Convert.d ../Parvicursor/System/DateTime/DateTime.d ../Parvicursor/System/Environment/Environment.d ../Parvicursor/System/Exception/Exception.d ../Parvicursor/System/FormatException/FormatException.d ../Parvicursor/System/Guid/Guid.d ../Parvicursor/System/InvalidOperationException/InvalidOperationException.d ../Parvicursor/System/NotSupportedException/NotSupportedException.d ../Parvicursor/System/Object/Object.d ../Parvicursor/System/ObjectDisposedException/ObjectDisposedException.d ../Parvicursor/System/OverflowException/OverflowException.d ../Parvicursor/System/Random/Random.d ../Parvicursor/System/String/String.d ../Parvicursor/System/Type/Type.d 
clean.OBJ += $(Debug_target.BIN) $(Debug_target.OBJ)

Debug_target : Debug_target.before $(Debug_target.BIN) Debug_target.after_always
Debug_target : CFLAGS += -fomit-frame-pointer -O3 -w -march=native -DParvicursorLib_inlininggg  -Os
Debug_target : INCLUDES += 
Debug_target : RCFLAGS += 
Debug_target : LDFLAGS += -s -lcryptopp -lpthread   $(CREATE_DEF)
Debug_target : T_LDLIBS = 
ifdef LMAKE
Debug_target : CFLAGS -= -O1 -O2 -g -pipe
endif

Debug_target.before :
	
	
Debug_target.after_always : $(Debug_target.BIN)
	
$(Debug_target.BIN) : $(Debug_target.OBJ)
	$(LINK_lib)
	

# -----------------------------------------
ifdef MAKE_DEP
-include $(DEP_FILES)
endif
