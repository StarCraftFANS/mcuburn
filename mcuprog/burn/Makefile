#
CFLAGS=-Wall
SETGID= echo 'chgrp kmem $@ ; chmod g+s $@' | su
STACK=prog84.o lp_io.o io_ports.o
#
all: prog84 lpttool dump
tar: prog84.tar.gz
#
lpttool: lpttool.o io_ports.o
	cc -o lpttool lpttool.o io_ports.o
	$(SETGID)
#
jig: jig.o $(STACK)
	cc -o jig jig.o $(STACK)
	$(SETGID)
#
dump: dump.o $(STACK)
	cc -o dump dump.o $(STACK)
	$(SETGID)
#
prog84: progmain.o $(STACK)
	cc -o prog84 progmain.o $(STACK)
	$(SETGID)
#
sampler: sampler.o io_ports.o
	cc -o sampler sampler.o io_ports.o
#
DISTRIB=README old-README prog84.man Makefile \
	prog84.h io_ports.h progmain.c prog84.c lp_io.c io_ports.c \
	jig.c lpttool.c dump.c \
	lp_cfg schematic.ps
#
prog84.tar.gz: $(DISTRIB)
	tar cf - $(DISTRIB) | gzip --best > $@
#
