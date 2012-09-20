#############################################################################
# Makefile for building: libwidgets.so.1.0.0
# Makefile for Building the mcu Burner
# Description:
# Author : Ganesh Gudigara
# <ganesh@smartpathindia.com>
#############################################################################
export TOPDIR=$(PWD)
#include the definitions.
include rules.mk
CURSLIBS := -lncurses
NEWTLIBS := -lslang -lnewt
WXLIBS   := $(shell wx-config --libs)
WXFLAGS  := $(shell wx-config --cflags)
CURSAPP := consoleburn
CONFIG_STATIC=yes
NEWTAPP := 
#codeburner
WXCBAPP := wxburn
WXSRCS  := wxburn.cpp mcupanel.cpp
WXOBJS := $(WXSRCS:.cpp=.o)
CXXFLAGS += $(WXFLAGS)
CFLAGS += $(WXFLAGS)
CURSSRCS := consoleburn.c
CURSOBJS := $(CURSSRCS:.c=.o)
NEWTSRCS := codeburner.c
NEWTOBJS := $(NEWTSRCS:.c=.o)
SUBDIRS := proto comm heximage
LIBS +=    comm/libcomm.a heximage/libheximage.a proto/libproto.a heximage/libheximage.a
.PHONY: build-libs
all: build-libs $(CURSAPP) $(NEWTAPP) $(WXCBAPP)

build-libs:
		 for i in $(SUBDIRS); do \
		 (cd $$i && $(MAKE)); \
		 done
		 
$(CURSAPP):$(CURSOBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(CURSLIBS) --rpath -fpic $(LIBS) 
$(NEWTAPP):$(NEWTOBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(NEWTLIBS) $(LIBS)
$(WXCBAPP):$(WXOBJS)
	$(CXX) -o $@ $^ $(LDFLAGS) $(WXLIBS) $(LIBS)
clean:
		 for i in $(SUBDIRS); do \
		 (cd $$i && $(MAKE) clean); \
		 done
	rm -f $(CURSAPP) $(NEWTAPP) $(WXCBAPP)
	rm -f *.o core *.obj *.exe
	
