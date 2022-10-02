# -*- mode: Makefile; -*-
# -----------------------------------------
# workspace Workspace

all :

% :
	#$(MAKE) -B -f ParvicursorLib.cbp.mak $@
	$(MAKE) -B -C ../xDFSClient_test -f xDFSClient_test.cbp.mak $@
	$(MAKE) -B -C ../xDFSServer_exe -f xDFSServer_exe.cbp.mak $@
	$(MAKE) -B -C ../FileCreator -f FileCreator.cbp.mak $@
