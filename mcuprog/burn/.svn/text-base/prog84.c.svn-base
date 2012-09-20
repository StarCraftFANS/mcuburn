/*
   prog84.c

   these routines use the parallel port to program a PIC 16C84 in serial mode
*/

#include "io_ports.h"
#include "prog84.h"
#include <stdio.h>
#include <unistd.h> /* for sleep() and usleep() */

int
  p_power   = -1,
  p_mclr    = -1,
  p_clock   = -1,
  p_data    = -1,
  p_clock_f = -1,
  p_data_f  = -1;

struct lp_io lpb;

/* if you edit this array be sure to change the array lengths in the
   various calls that reference it */
static 
struct lp_name names[] = {
  { "base",    &(lpb.base) },

  { "power",   &p_power    },
  { "mclr",    &p_mclr     },
  { "clock",   &p_clock    },
  { "data",    &p_data     },
  { "clock_f", &p_clock_f  },
  { "data_f",  &p_data_f   },

  /* alternate names */
  { "rb6",     &p_clock    },
  { "rb7",     &p_data     },
  { "rb6_f",   &p_clock_f  },
  { "rb7_f",   &p_data_f   }
};

char *cfg_file = "lp_cfg";

int p84_verbose = 0;

int testHW()
{
  int i,j;     /* utility variables */
  int ret[4];  /* array of test results */
  int m;       /* merged test results */

  SET(power, 0);
  SET(mclr, 0);
  SET(clock, 0);
  SET(data, 0);
  FLUSH;
  sleep(1);
  FLUSH;

  if(p_power == -1 || p_mclr == -1 || p_clock == -1 || p_data == -1 || 
     p_data_f == -1)
  {
    fprintf(stderr, 
	    "Error: I need control over power, MCLR, clock, and data,\n"
	    "       and I need to be able to read data back.\n");
    return 1;
  }

  if(p84_verbose)
  {
    printf("[testing loopback");
    fflush(stdout);
  }

  for(i=0; i<4; i++)
  {
    SET(clock, i&1);
    SET(data, i&2);
    FLUSH;
    j = GET(data_f)? 2 : 0;
    if(p_clock_f != -1)
      j |= GET(clock_f)? 1 : 0;
    else
      j |= i & 1;  /* fake it if we don't have it */
    ret[i] = j;
  }
  
  SET(clock, 0);
  SET(data, 0);
  FLUSH;

  if(p84_verbose)
    printf("]\n");

  /* analyze the test results.
     we put our four 2-bit responses into an int, one per nybble
     so we can use handy hex notation. */
  m = (((((ret[3] << 4) | ret[2]) << 4) | ret[1]) << 4) | ret[0];

  if(m == 0x3210)
    return 0;

  fprintf(stderr, "Bad test results!\n  [data, clock]->[data_f, clock_f]:");
  for(i = 0; i < 4; i ++)
    fprintf(stderr, " %d%d->%d%d", (i>>1)&1, i&1, (ret[i]>>1)&1, (ret[i])&1);
  fputs("\n", stderr);

  if(p_clock_f == -1)
    fprintf(stderr, "(clock_f simulated since I don't actually have it)\n");

  if(m == 0x3120)
    fputs("clock and data reversed?\n"
	  " (could be either the clock & data lines, or the sense lines)\n", 
	  stderr);

  switch(m & 0x1111)
  {
    case 0x0000: fputs("clock_f stuck low?\n", stderr);    break;
    case 0x0101: fputs("clock_f inverted?\n", stderr);     break;
    case 0x1111: fputs("clock_f stuck high?\n", stderr);   break;
  }

  switch(m & 0x2222)
  {
    case 0x0000: fputs("data_f stuck low?\n", stderr);     break;
    case 0x0022: fputs("data_f inverted?\n", stderr);      break;
    case 0x2222: fputs("data_f stuck high?\n", stderr);    break;
  }
    
  return 1;
}

/* 
   Pipe some bits through the 16C84. Returns the values read from
   the data pin at the appropriate point in the cycle for reading data mem.
   In a write, will return 'bits'.  (Unless, of course, something's wrong.)
*/
unsigned int pipeBits(bits, count)
     unsigned int bits;
     int count;
{
  unsigned int ret = 0;
  int bit;

  for(bit=0; bit<count; bit++)
  {
    SET(clock, 1);
    SET(data, bits&1);
    FLUSH;
    if(p84_verbose > 1)
      printf(". bit %2d: send %d ", bit, bits&1);
    
    /* 100 ns. delay */
    SET(clock, 0);
    FLUSH;
    ret |= (GET(data_f)?1 : 0) << bit;
    if(p84_verbose > 1)
      printf("get %d\n", GET(data_f)?1:0);
    
    /* 100ns delay */
    bits >>= 1;
  }

  SET(data, 1); /* the HI-Z state */
  FLUSH;

  usleep(1);    /* required guard time of at least 1 usec */

  return ret;
}

void xPipeBits(bits, count)
     unsigned int bits;
     int count;
{
  unsigned int ret;

  ret = pipeBits(bits, count);
  if(ret != bits)
  {
    fprintf(stderr,
	    "ERROR: sent %d bits: %d (0%o), got %d (0%o)\n"
	    "Resetting & powering down.\n",
	    count, bits, bits, ret, ret);
    SET(mclr, 0);
    FLUSH;
    SET(data, 0);
    SET(clock, 0);
    SET(power, 0);
    FLUSH;

    exit(2);
  }
}


/* A generic command-plus-14-bits-to-write call */
void genericWrite(cmd, data)
     int cmd;            /* 6 bits of command */
     unsigned int data;  /* 14 bits of data */
{
  xPipeBits(cmd, 6);		  	/* Send the command */
  xPipeBits((data << 1) & 0x7FFE, 16);	/* send the data */
}

/* A generic command-plus-14-bits-to-read call */
unsigned int genericRead(cmd)
     int cmd;
{
  unsigned int ret;

  xPipeBits(cmd, 6);			/* send the command */
  ret = pipeBits(0x7FFE, 16);		/* read the result */
  return((ret & 0x7FFE) >> 1);		/* remove guard bits */
}


void progSetup()
{
  /* read the configuration file */
  if(lpb_cfg_read(cfg_file, 11, names))
  {
    fprintf(stderr, "error reading cfg file %s\n", cfg_file);
    exit(1);
  }

  if(p84_verbose)
    lpb_dump_names(7, names);

  /* open /dev/port */
  open_io();

  /* check that the programmer's not totally dead */
  if(testHW())
  {
    fputs("\nProgrammer is not responding, aborting.\n", stderr);
    exit(1);
  }
}

void progShutdown()
{
  SET(mclr, 0);
  FLUSH;
  usleep(1000);
  SET(power, 0);
  SET(clock, 0);
  SET(data, 0);
  FLUSH;
  
  close_io();
}
