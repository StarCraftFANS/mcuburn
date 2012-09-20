#include "prog84.h"

#include <stdio.h>
#include <sys/time.h>

int verbose = 0;


void dumpProg(unsigned, unsigned);
void dumpData(unsigned, unsigned);
void dumpCfg();

main(argc, argv)
     int argc;
     char **argv;
{
  time_t now;

  progSetup();

  time(&now);

  printf("# pictools format dump file\n"
	 "# created by %s on %s",
	 argv[0], ctime(&now));


  dumpCfg();

  dumpData(0, 16);

  dumpProg(0, 30);

  progShutdown();

  return 0;
}

void dumpProg(min_addr, max_addr)
     unsigned min_addr, max_addr;
{
  int buf[8];
  int pic_addr;
  int buf_count;
  int i;

  SET(power, 1);
  SET(clock, 0);
  SET(data, 0);
  SET(mclr, 0);
  FLUSH;
  usleep(50000);
  SET(mclr, 1);
  FLUSH;
  usleep(50000);

  buf_count = -1;
  pic_addr = 0;

  printf("A%04x\n", min_addr);
 
  while(pic_addr < min_addr)
    incAddr();
  
  buf_count = 0;
  while(pic_addr < max_addr)
  {
    buf[buf_count ++] = readProg();
    if(buf_count >= 8)
    {
      printf("D%d", buf_count);
      for(i=0; i<buf_count; i++)
	printf(" %04x", buf[i]);
      putchar('\n');
      buf_count = 0;
    }
    incAddr();
    pic_addr ++;
  }
  if(buf_count)
  {
    printf("D%d", buf_count);
    for(i=0; i<buf_count; i++)
      printf(" %04x", buf[i]);
    putchar('\n');
    buf_count = 0;
  }
}

void dumpData(min_addr, max_addr)
     unsigned min_addr, max_addr;
{
  int buf[8];
  int pic_addr;
  int buf_count;
  int i;

  SET(power, 1);
  SET(clock, 0);
  SET(data, 0);
  SET(mclr, 0);
  FLUSH;
  usleep(50000);
  SET(mclr, 1);
  FLUSH;
  usleep(50000);

  buf_count = -1;
  pic_addr = 0;

  printf("B%04x\n", min_addr);
 
  while(pic_addr < min_addr)
    incAddr();
  
  buf_count = 0;
  while(pic_addr < max_addr)
  {
    buf[buf_count ++] = readData();
    if(buf_count >= 8)
    {
      printf("D%d", buf_count);
      for(i=0; i<buf_count; i++)
	printf(" %04x", buf[i]);
      putchar('\n');
      buf_count = 0;
    }
    incAddr();
    pic_addr ++;
  }
  if(buf_count)
  {
    printf("D%d", buf_count);
    for(i=0; i<buf_count; i++)
      printf(" %04x", buf[i]);
    putchar('\n');
    buf_count = 0;
  }

}

void dumpCfg()
{
  unsigned int id[4], fuses;

  SET(power, 1);
  SET(clock, 0);
  SET(data, 0);
  SET(mclr, 0);
  FLUSH;
  usleep(50000);
  SET(mclr, 1);
  FLUSH;
  usleep(50000);

  printf("T84  # I can only understand '84s\n");

  /* dummy load configuration command to get to cfg memory */
  loadConfiguration(0x3FFF);
  id[0] = readProg();
  incAddr();
  id[1] = readProg();
  incAddr();
  id[2] = readProg();
  incAddr();
  id[3] = readProg();
  incAddr();
  /* reserved 1 */
  incAddr();
  /* reserved 2 */
  incAddr();
  /* reserved 3 */
  incAddr();
  fuses = readProg();

  printf("I%04x %04x %04x %04x\n",
	 id[0], id[1], id[2], id[3]);
  
  printf("P%d\nU%d\nW%d\nC%d 0\n",
	 fuses & 0x10 ? 1 : 0,
	 fuses & 0x08 ? 1 : 0,
	 fuses & 0x04 ? 1 : 0,
	 fuses & 0x03);
}

