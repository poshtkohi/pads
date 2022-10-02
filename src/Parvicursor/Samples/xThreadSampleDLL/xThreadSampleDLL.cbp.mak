# -*- mode: Makefile; -*-
# -----------------------------------------
# project xThreadSampleDLL


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

CFLAGS = -fPIC 
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
	
all.targets : Debug_Win32_target 

clean :
	rm -fv $(clean.OBJ)
	rm -fv $(DEP_FILES)

.PHONY: all clean distclean

# -----------------------------------------
# Debug_Win32_target

Debug_Win32_target.BIN = Debug/libxThreadSampleDLL.so
Debug_Win32_target.OBJ = Sample.o stdafx.o 
DEP_FILES += Sample.d stdafx.d 
clean.OBJ += $(Debug_Win32_target.BIN) $(Debug_Win32_target.OBJ)

Debug_Win32_target : Debug_Win32_target.before $(Debug_Win32_target.BIN) Debug_Win32_target.after_always
Debug_Win32_target : CFLAGS += -W -g -O0 -D_DEBUG -D_WINDOWS -D_USRDLL -DXTHREADSAMPLEDLL_EXPORTS  -Os
Debug_Win32_target : INCLUDES += 
Debug_Win32_target : RCFLAGS += 
Debug_Win32_target : LDFLAGS += -shared  $(CREATE_LIB) $(CREATE_DEF)
Debug_Win32_target : T_LDLIBS = ../../ParvicursorLib/Debug/ParvicursorLib.lib 
ifdef LMAKE
Debug_Win32_target : CFLAGS -= -O1 -O2 -g -pipe
endif

Debug_Win32_target.before :
	
	
Debug_Win32_target.after_always : $(Debug_Win32_target.BIN)
	
$(Debug_Win32_target.BIN) : $(Debug_Win32_target.OBJ)
	$(LINK_dll)
	

# -----------------------------------------
ifdef MAKE_DEP
-include $(DEP_FILES)
endif