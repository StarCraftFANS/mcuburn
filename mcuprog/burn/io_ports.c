/*
   convenience routines for doing simple I/O with linux' /dev/port

   note that you have to have uid=root or gid=kmem to open /dev/port

   written and copyright October 1994 by Wim Lewis, wiml@{netcom,omnigroup}.com
   distribute, modify, re-use freely!
*/

#include "io_ports.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

int dev_port_fd = -1;

void open_io()
{
  dev_port_fd = open("/dev/port", O_RDWR);
  if(dev_port_fd < 0)
  {
    perror("/dev/port");
    exit(errno);
  }
}

void close_io()
{
  close(dev_port_fd);
  dev_port_fd = -1;
}

void out_byte(port, byte)
     int port;
     unsigned char byte;
{
  off_t s;
  int r;

  s = lseek(dev_port_fd, port, 0);
  if(s < 0)
    perror("lseek");
  else if(s != port)
    fprintf(stderr, "hmmm: seeking to %d, went to %ld.\n", port, (long)s);

  r = write(dev_port_fd, &byte, 1);
  if(r != 1)
  {
    fprintf(stderr, "hmmm: write returned %d\n", r);
    if(r < 0) perror("write");
  }
}

unsigned char in_byte(port)
     int port;
{
  off_t s;
  int r;
  unsigned char ch = 0;

  s = lseek(dev_port_fd, port, 0);
  if(s < 0)
    perror("lseek");
  else if(s != port)
    fprintf(stderr, "hmmm: seeking to %d, went to %ld.\n", port, (long)s);

  r = read(dev_port_fd, &ch, 1);
  if(r != 1)
  {
    fprintf(stderr, "hmmm: read returned %d\n", r);
    if(r < 0) perror("read");
  }

  return ch;
}


