
#include "prog84.h"

#include <stdio.h>
#include <stdlib.h> /* for atoi() */

int verbose = 1;

void doSimple(name, bit, buf)
     const char *name;
     int bit;
     char *buf;
{
  int num = atoi(buf);
  lpb_write(&lpb, bit, num?1:0);
  printf("%s O%s\n", name, num?"N":"FF");
  lpb_flush(&lpb);
}

void jig()
{
  char buf[256];
  int bits, num;

  printf("\n*** simple jig mode. enter:\n"
         "  p0 or p1  to turn power off or on\n"
         "  m0 or m1  to set MCLR low or high\n"
	 "  c0 or c1  to set the clock input\n"
	 "  d0 or d1  to set the data input\n"
	 "  r         to read the current clock & data sense lines\n"
         "  d c       to clock c bits of d, lsb first\n");

  while(fgets(buf, 256, stdin))
  {
    if(!*buf)
      break;

    if(*buf == 'p') { doSimple("power", p_power, buf+1); continue; }
    if(*buf == 'm') { doSimple("mclr",  p_mclr,  buf+1); continue; }
    if(*buf == 'c') { doSimple("clock", p_clock, buf+1); continue; }
    if(*buf == 'd') { doSimple("data",  p_data,  buf+1); continue; }
    if(*buf == 'r')
    {
      lpb_refresh(&lpb);
      printf("clock=%d data=%d\n",
	     GET(clock_f), GET(data_f));
      continue;
    }

    if(sscanf(buf, "%d %d", &bits, &num) != 2)
    {
      printf("?\n");
      continue;
    }

    if(bits & ~((1<<num)-1))
    {
      printf("extraneous bits! fnord!\n");
      continue;
    }

    printf("%o -%d> %o\n", bits, num, pipeBits(bits, num));
  }
}

int main()
{
  progSetup();

  jig();

  progShutdown();

  return 0;
}

