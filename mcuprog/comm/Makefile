#############################################################################
# Makefile for building: mcu burner
#############################################################################
include $(TOPDIR)/rules.mk
CFLAGS += -fpic
CXXFLAGS += -fpic

SOURCES := comm.c commport.c
OBJS=$(SOURCES:.c=.o)
ifdef CONFIG_STATIC
TARGET := libcomm.a
else
TARGET := libcomm.so
endif
all:$(TARGET)

$(TARGET):$(OBJS)
		$(LIBBUILD) $@ $(OBJS)
clean:
	rm -f $(TARGET) *.a *.so*
	rm -f *.o core *.obj *.exe

