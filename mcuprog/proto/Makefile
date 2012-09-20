#############################################################################
# Makefile for building: mcu burner
#############################################################################
CFLAGS += -fpic
CXXFLAGS += -fpic

SOURCES := ezatmel.c
OBJS=$(SOURCES:.c=.o)
ifdef CONFIG_STATIC
TARGET := libproto.a
else
TARGET := libproto.so
endif
.PHONY: $(TARGET)
all:$(TARGET)

$(TARGET):$(OBJS)
		$(LIBBUILD) $@ $(OBJS)
clean:
	rm -f $(TARGET) *.a *.so*
	rm -f *.o core *.obj *.exe

