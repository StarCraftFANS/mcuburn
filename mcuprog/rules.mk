#############################################################################
# Makefile for building: mcu Burner
# Description : 
# Author : Ganesh Gudigara
#############################################################################
export CROSS=
export CC=$(CROSS)gcc
export CXX=$(CROSS)g++
export LD=$(CROSS)ld
export AR=$(CROSS)ar
export RANLIB=$(CROSS)ranlib
export PREFIX = /usr/local
export CXXFLAGS += -I$(PREFIX)/include -I. -I$(TOPDIR)/include
export CFLAGS  +=  -I$(PREFIX)/include -I. -I$(TOPDIR)/include
export LDFLAGS := -L$(PREFIX)/lib -L.
export CONFIG_STATIC=yes

ifdef CONFIG_SHARED
export CFLAGS += -shared
export CXXFLAGS += -shared
export LIBBUILD := $(AR) scru
else
export CFLAGS += -static
export CXXFLAGS += -static
export LIBBUILD := $(AR) crus
endif

%.o:%.cpp
	  $(CXX) -c $^ $(CXXFLAGS) -o $@
%.o:%.c
	$(CC) -c $^ $(CFLAGS) -o $@

