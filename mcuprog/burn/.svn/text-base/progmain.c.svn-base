#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include "prog84.h"

int verbose = 0;
int argc;
char **argv;
char **argp;

const char *usage = 
"Usage:\n"
"\t-a               verify after programming each location\n"
"\t-b               check beforehand to see if value is already correct\n"
"\t-v               be more verbose\n"
"\t-i NN NN NN NN   program ID locations\n"
"\t-p filename      read pictools format file & program\n"
"\t-h               print this help message & exit\n";

/* types o' verification */
#define VERIFY_BEFORE  0x1
#define VERIFY_AFTER   0x2

/* Argument processing functions */
long int shiftNum(char *);
FILE *shiftFile(char *);

int picPC; /* what we think the PIC's pc is right now */

typedef char BOOL;
BOOL progUp; /* has the programmer been initialized? */
int verification; /* what kinds of verification we do */
int errors = 0;  /* how many programming errors */

/* programming routines */
void programID(unsigned int *);
void programPicTools(FILE *);

int main(largc, largv)
     int largc;
     char **largv;
{
  char *cp;
  int i;
  unsigned int id[4];
  FILE *fp;

  argc = largc;
  argv = largv;
  argp = largv + 1;

  while((cp = *(argp++)))
  {
    if(*cp != '-')
    {
      fprintf(stderr, "%s: %s is not an option\n", *argv, cp);
      exit(1);
    }
    
    switch(cp[1])
    {
      case 'v':
        verbose ++;
	if(verbose > 1)
	  p84_verbose ++;
	break;
      case 'i':
	for(i=0; i<4; i++)
	  id[i] = shiftNum("id number");
	programID(id);
	break;
      case 'p':
	fp = shiftFile("pictools format file");
	programPicTools(fp);
	fclose(fp);
	break;
      case 'h':
	fputs(usage, stdout);
	exit(0);
      case 'a':
	verification |= VERIFY_AFTER;
	break;
      case 'b':
	verification |= VERIFY_BEFORE;
	break;
      default:
	fprintf(stderr, "%s: unknown option \"%s\"\n", *argv, cp);
	exit(1);
    }
  }

  if(argc < 2)
    fputs(usage, stdout);

  if(progUp)
    progShutdown();

  return 0;
}
	
/* Programming utilities */

void programInit()
{
  if(!progUp)
  {
    progSetup();
    progUp = 1;
    SET(power, 1);
  }

  SET(mclr, 0);
  SET(clock, 0);
  SET(data, 0);
  FLUSH;

  picPC = 0;

  usleep(50000);
  SET(mclr, 1);
  FLUSH;
}

void programID(id)
     unsigned int *id;
{
  int i;

  if(!progUp || picPC > 0x2000)
    programInit();

  if(verbose)
  {
    printf("writing config [");
    fflush(stdout);
  }
  loadConfiguration(id[0]);
  for(i = 0; i < 4; i++)
  {
/*    printf("readProg() == %d. id[%d] == %d.\n", readProg(), i, id[i]); */
    if((verification & VERIFY_BEFORE) && 
       (readProg() == id[i]))
    {
      if(verbose)
	putchar(' ');
    }
    else
    {
      doProgram();
      if((verification & VERIFY_AFTER) &&
	 (readProg() != id[i]))
      {
	if(verbose)
	  putchar('X');
	errors ++;
      }
      else
	if(verbose)
	  putchar('.');
    }
    if(i < 3)
    {
      if(verbose)
	fflush(stdout);
      incAddr();
      loadProg(id[i]);
    }
  }

  if(verbose)
    printf("]\n");
  
  picPC = 0x2003;
}

/* fuses */
#define WDT_FUSE 0x04  /* watchdog timer */
#define PUT_FUSE 0x08  /* power-up timer */
#define CP_FUSE  0x10  /* code protect */
#define FUSE_MASK 0x1F /* only these bits are implemented */

void programFuse(f)
     unsigned int f;
{
  if(!progUp || picPC > 0x2007)
    programInit();

  if(picPC < 0x2000)
  {
    loadConfiguration(0x3FFF);
    picPC = 0x2000;
  }
  
  while(picPC < 0x2007)
  {
    incAddr();
    picPC ++;
  }

  if(verification & VERIFY_BEFORE)
    if((readProg() & FUSE_MASK) == (f & FUSE_MASK))
    {
      if(verbose)
	printf("fuses not changed; not re-programmed\n");
      return;
    }

  if(verbose)
    printf("writing fuses\n");

  loadProg(f);
  doProgram();
  
  if(verification & VERIFY_AFTER)
  {
    int ver = readProg();
    if((ver & FUSE_MASK) != (f & FUSE_MASK))
    {
      fprintf(stderr, "writing fuses: wrote %04x, verified as %04x\n", f, ver);
      errors ++;
    }
  }
}

void programProg(addr, count, buf)
     unsigned int addr;
     int count;
     unsigned int *buf;
{
  if(!progUp || picPC > addr)
    programInit();

  if(verbose)
  {
    printf("%2d at %04x", count, addr);
    fflush(stdout);
  }

  while(picPC < addr)
  {
    incAddr();
    picPC ++;
  }

  if(verbose)
  {
    fputs(" [", stdout);
    fflush(stdout);
  }

  while(count)
  {
    if((verification & VERIFY_BEFORE) &&
       (readProg() == *buf))
    {
      if(verbose)
	putchar(' ');
    }
    else
    {
      loadProg(*buf);
      doProgram();
      if(verbose) 
	putchar('.');
    }
    fflush(stdout);
    if(verification & VERIFY_AFTER)
    {
      if(readProg() != *buf)
      {
	putchar('\b');
	putchar('X');
	errors ++;
	fflush(stdout);
      }
    }
    incAddr();
    picPC ++;
    count --;
    buf ++;
  }

  if(verbose) 
  { 
    putchar(']');
    putchar('\n'); 
  }
}

void programPicTools(fp)
     FILE *fp;
{
  BOOL have_fuses;
  BOOL have_id;
  unsigned id[4];
  unsigned fuses;
  char buf[512];
  unsigned code_addr = 0;
  int i;

  fuses = 0xFFF;

  while(fgets(buf, 512, fp))
  {
    switch(*buf)
    {
      case 0:
      case '\n':
      case '\r':
      case '#':
        break;
      case 'I':
	if(sscanf(buf+1, "%x %x %x %x",
		  &id[0], &id[1], &id[2], &id[3]) != 4)
	{
	  fputs("error reading ID numbers\n", stderr);
	  exit(2);
	}
	have_id = 1;
      case 'A':
	sscanf(buf+1, "%x", &code_addr);
	break;
      case 'D':
      {
	unsigned code_buf[32];
	int code_count;
	char *cp;

	code_count = strtol(buf+1, &cp, 10);
	if(cp == buf+1) 
	{
	  fputs("error reading data byte count\n", stderr);
	  exit(2);
	}
	for(i=0; i<code_count; i++)
	  code_buf[i] = strtol(cp, &cp, 16);
	
	programProg(code_addr, code_count, code_buf);
	code_addr += code_count;
	break;
      }
      case 'T':
	if(atoi(buf+1) != 84)
	{
	  fprintf(stderr, "I'm a 16C84 programmer, but this file as %s", buf);
	  exit(2);
	}
	break;
      case 'W':
	have_fuses = 1;
	fuses &= ~ WDT_FUSE;
	if(atoi(buf+1)) fuses |= WDT_FUSE;
	break;
      case 'U':
	have_fuses = 1;
	fuses &= ~ PUT_FUSE;
	if(atoi(buf+1)) fuses |= PUT_FUSE;
	break;
      case 'P':
	have_fuses = 1;
	fuses &= ~ CP_FUSE;
	if(atoi(buf+1)) fuses |= CP_FUSE;
	break;
      case 'C':
	i = atoi(buf+1);
	have_fuses = 1;
	fuses = ( fuses & ~3 ) | ( atoi(buf+1) & 3 );
	break;
      case 'S':
	puts("[ignoring pictools file checksum]");
	break;
      default:
	printf("warning: unrecognized line: %s", buf);
	break;
    }
  }

  if(have_id)
    programID(id);

  if(have_fuses)
    programFuse(fuses);
}


/* Arg parsing functions */

long int shiftNum(what)
     char *what;
{
  char *end;
  long int val;

  if(!*argp)
  {
    fprintf(stderr, "%s: arglist ends abruptly\n", *argv);
    exit(1);
  }

  val = strtol(*argp, &end, 0);
  if(end == *argp)
  {
    fprintf(stderr, "%s: expecting %s, got \"%s\"\n", *argv, what, *argp);
    exit(1);
  }

  argp++;

  return val;
}

FILE *shiftFile(what)
     char *what;
{
  FILE *fp;

  if(!*argp)
  {
    fprintf(stderr, "%s: arglist ends abruptly\n", *argv);
    exit(1);
  }

  fp = fopen(*argp, "r");
  if(!fp)
  {
    fprintf(stderr, "%s: can't open %s %s: %s\n",
	    *argv, what, *argp, strerror(errno));
    exit(2);
  }

  argp++;

  return fp;
}
  
