# -*- mode: Makefile; -*-
# -----------------------------------------
# workspace Workspace

all :

% :
	$(MAKE) -B -C ParvicursorLib.cbp -f ParvicursorLib.cbp.mak $@
	$(MAKE) -B -C ../xSim2Lib -f xSim2Lib.cbp.mak $@
	$(MAKE) -B -C ../Samples/xSim2/ClusteredQueuingNetworkSimulation -f ClusteredQueuingNetworkSimulation.cbp.mak $@
	$(MAKE) -B -C ../Samples/xSim2/PipelineTandemQueueSimulation -f PipelineTandemQueueSimulation.cbp.mak $@
