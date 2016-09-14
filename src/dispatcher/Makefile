RM=rm
CD=cd

MODULE=dispatcher

PWD=$(shell pwd)
PRODIR=$(PWD)/../..
TARGET=$(PRODIR)/bin/$(MODULE)
SRCDIR=$(PRODIR)/src
OBJDIR=$(PRODIR)/src
LIBDIR=$(PRODIR)/lib

CC       = g++
LIBS    += -lpthread -L$(LIBDIR) $(LIBDIR)/liblog4cplus-1.2.so.5
INCLUDE += -I$(PRODIR)/inc -I$(PRODIR)/inc/log4cplus -I$(PRODIR)/inc/openflow -I$(SRCDIR)/common -I$(SRCDIR)/dispatcher
CFLAGS  += -g -O0 -mcmodel=medium -Wall -Wno-reorder

# all object files.
OBJ=$(OBJDIR)/common/config.o $(OBJDIR)/common/log.o $(OBJDIR)/common/mem-pool.o $(OBJDIR)/common/mutex.o $(OBJDIR)/common/timer.o $(OBJDIR)/common/tools.o $(OBJDIR)/dispatcher/main.o $(OBJDIR)/dispatcher/tcp-server.o
all:$(OBJ)
	@echo "+----------------------------------------------------+"
	@echo "+     Start building $(MODULE) module......     +"
	@echo "+----------------------------------------------------+"
	$(CC) -g -o $(TARGET) $(OBJ) $(LIBS)
	@echo ""
	@echo "+----------------------------------------------------+"
	@echo "+     Finished building $(MODULE) program       +"
	@echo "+----------------------------------------------------+"


$(OBJDIR)/%.o:$(OBJDIR)/%.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<

clean:
	$(CD) $(OBJDIR) && $(RM) -f $(OBJ)
	$(RM) -f ./$(TARGET)
	$(RM) -f ./$(TARGET)_test
	$(RM) -f ./$(TARGET)_debug
