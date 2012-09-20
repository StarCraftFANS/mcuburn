/*

*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "io_ports.h"
#include <linux/lp.h>

#define IBUFLEN 128
#define LPT1_BASE  (0x378)             /* where LPT1: is on most h/w */

/* These assume that 'base' == LPT1_BASE or equiv. */
#define lp_data   (base)               /* write to this to change D0-D7 */
#define lp_status (base+1)             /* read from this to get status */
#define lp_ctl    (base+2)             /* write to change the ctl lines */


int base = LPT1_BASE;

int main(argc, argv)
     int argc;
     char **argv;
{
  char buf[IBUFLEN];
  unsigned char ch;

  if(argc == 2)
    base = atoi(argv[1]);
  if((argc != 1) || (!base))
  {
    fputs("Bad usage! No biscuit!\n", stderr);
    exit(8);
  }
    
  open_io();  /* opens /dev/port */

  printf("base = %d (0x%x)\n", base, base);

  while(fgets(buf, IBUFLEN, stdin))
  {
    if(buf[0] == '?')
    {
      ch = in_byte(lp_data);
      printf("<-- %d (0x%02x)\n", (int)ch, (int)ch);
    }
    else if (!buf[0] || buf[0] == '\n')
    {
      ch = in_byte(lp_status);
      printf("<+1 %d (0x%02x):", (int)ch, (int)ch);
      if(!(ch & LP_PBUSY))
	fputs(" BUSY", stdout);
      if(!(ch & LP_PACK))
	fputs(" ACK", stdout);
      if(ch & LP_POUTPA)
	fputs(" OUT-OF-PAPER", stdout);
      if(ch & LP_PSELECD)
	fputs(" ONLINE", stdout);
      if(!(ch & LP_PERRORP))
	fputs(" ERROR", stdout);
      putchar('\n');
    }
    else
    {
      ch = (unsigned char)atoi(buf);
      out_byte(lp_data, ch);
      printf("--> %d (0x%02x)\n", (int)ch, (int)ch);
    }
  }

  close_io();  /* release /dev/port */
  
  return(0);
}


