RM=rm
MV=mv
CD=cd
CP=cp

PRODUCT=bin/DCFabric

ifeq (,$(PRO_DIR))
PRO_DIR = $(shell pwd)/src
endif

#Out file path
OPath = $(PRO_DIR)


CC       = g++
TARGET   = ./$(PRODUCT)
LIBS    += -lpthread -L./lib -L./lib $(shell pwd)/lib/libjemalloc.so.2  $(shell pwd)/lib/liblog4cplus-1.2.so.5 $(shell pwd)/lib/libtbb.so $(shell pwd)/lib/libtbb.so.2
INCLUDE += -I./inc -I./inc/openflow -I./inc/log4cplus -I./inc/jemalloc -I./inc/tbb -I./inc/rapidjson -I$(OPath)/core -I$(OPath)/topo -I$(OPath)/handler -I$(OPath)/restful -I$(OPath)/flow -I$(OPath)/host -I$(OPath)/restful -I$(OPath)/utils 
CFLAGS  += -g -O0 -mcmodel=medium -Wall -Wno-reorder -ltbb

# all object files.
OBJ=$(OPath)/core/CServer.o $(OPath)/topo/CTopoMgr.o $(OPath)/core/CTCPServer.o $(OPath)/restful/CHttpRecvWorker.o $(OPath)/core/COfpRecvWorker.o $(OPath)/core/CConf.o $(OPath)/core/CCond.o $(OPath)/core/CMsg.o $(OPath)/core/CMutex.o $(OPath)/core/CBuffer.o $(OPath)/core/CTimer.o $(OPath)/core/dcfabric.o $(OPath)/core/log.o $(OPath)/core/CSemaphore.o $(OPath)/core/CRefObj.o $(OPath)/core/CRecvWorker.o $(OPath)/core/CException.o $(OPath)/core/CMsgTreeNode.o $(OPath)/core/COfMsgTreeNode.o $(OPath)/core/CEtherMsgTreeNode.o $(OPath)/core/CIPMsgTreeNode.o $(OPath)/core/CMsgTreeNodeMgr.o $(OPath)/utils/comm-util.o $(OPath)/utils/COfMsgUtil.o $(OPath)/handler/CMsgHandler.o $(OPath)/handler/CMsgHandlerMgr.o $(OPath)/core/CSwitch.o $(OPath)/core/CSwitchMgr.o $(OPath)/handler/CEtherArpHandler.o $(OPath)/handler/COfBarrierReplyHandler.o $(OPath)/handler/COfBarrierRequestHandler.o $(OPath)/handler/COfEchoReplyHandler.o $(OPath)/handler/COfEchoRequestHandler.o $(OPath)/handler/COfErrorHandler.o $(OPath)/handler/COfExperimenterHandler.o $(OPath)/handler/COfFeaturesReplyHandler.o $(OPath)/handler/COfFeaturesRequestHandler.o $(OPath)/handler/COfFlowModHandler.o $(OPath)/handler/COfFlowRemovedHandler.o $(OPath)/handler/COfGetAsyncReplyHandler.o $(OPath)/handler/COfGetAsyncRequestHandler.o $(OPath)/handler/COfGetConfigReplyHandler.o $(OPath)/handler/COfGetConfigRequestHandler.o $(OPath)/handler/COfGroupModHandler.o $(OPath)/handler/COfHelloHandler.o $(OPath)/handler/CIpIcmpHandler.o $(OPath)/handler/CEtherIPHandler.o $(OPath)/handler/CEtherIPv6Handler.o $(OPath)/handler/CEtherLldpHandler.o $(OPath)/handler/COfMeterModHandler.o $(OPath)/handler/COfMultipartReplyHandler.o $(OPath)/handler/COfMultipartRequestHandler.o $(OPath)/handler/COfPacketInHandler.o $(OPath)/handler/COfPacketOutHandler.o $(OPath)/handler/COfPortModHandler.o $(OPath)/handler/COfPortStatusHandler.o $(OPath)/handler/COfQueueGetConfigReplyHandler.o $(OPath)/handler/COfQueueGetConfigRequestHandler.o $(OPath)/handler/COfRoleReplyHandler.o $(OPath)/handler/COfRoleRequestHandler.o $(OPath)/handler/COfSetAsyncHandler.o $(OPath)/handler/COfSetConfigHandler.o $(OPath)/handler/COfStatsReplyHandler.o $(OPath)/handler/COfStatsRequestHandler.o $(OPath)/handler/COfTableModHandler.o $(OPath)/handler/CIpTcpHandler.o $(OPath)/handler/CIpUdpHandler.o $(OPath)/handler/COfVendorHandler.o $(OPath)/handler/CEtherVlanHandler.o $(OPath)/restful/CRestManager.o $(OPath)/restful/CRestRequest.o $(OPath)/restful/CRestResponce.o $(OPath)/restful/CRestHttp.o $(OPath)/restful/CRestApi.o $(OPath)/restful/CRestApiMgr.o $(OPath)/restful/CRestDefine.o  $(OPath)/host/CHost.o $(OPath)/host/COpenStackHost.o $(OPath)/host/CHostMgr.o $(OPath)/flow/CFlowMgr.o $(OPath)/flow/CFlow.o
all:$(OBJ)
	@echo "+----------------------------------------------------+"
	@echo "+     Start building $(PRODUCT) module......     +"
	@echo "+----------------------------------------------------+"
	$(CC) -g -o $(TARGET) $(OBJ) $(LIBS)
	@echo ""
	@echo "+----------------------------------------------------+"
	@echo "+     Finished building $(PRODUCT) program       +"
	@echo "+----------------------------------------------------+"


$(OPath)/%.o:$(OPath)/%.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<

clean:
	$(CD) $(OPath) && $(RM) -f $(OBJ)
	$(RM) -f ./$(PRODUCT)
	$(RM) -f ./$(PRODUCT)_test
	$(RM) -f ./$(PRODUCT)_debug
