/*
   lp_io.c

   buffered parallel-port I/O routines. These lie on top of the
   "asm IN/OUT" analogues in io_ports.c. For convenience, they define
   "bit indicators" which pack the register of a bit, its offset
   in the byte, and whether it's inverted or not into an int.

   There are two pieces of code in this file. The first part does simple
   buffered bitwise I/O:

       lpb_write(lpb, bit, val)
       val = lpb_test(lpb, bit)
         write to and read the bit indicated by 'bit' in the buffer pointed
         to by lpb.

       lpb_flush(lpb)         writes out the data and control bytes
       lpb_refresh(lpb)       reads in the status byte

   The second part parses a (very simple) configuration file format
   to fill in an array of bit indicators. This is lpb_cfg_read(),
   which takes a filename, a pointer to an array of (struct lp_name)s,
   and the length of the array, and tries to fill in the array from the
   data file. There is a companion function, lpb_dump_names, which
   takes an array or structs and its length, and dumps them to stdout
   for debugging.
*/

#include "io_ports.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h> /* for strtoul() */

/* This turns on debugging output from lpb_cfg_read */
/* #define LPBDEBUG */

void lpb_write(lpb, bit, val)
     struct lp_io *lpb;
     int bit, val;
{
  int mask = 1 << (bit & BI_OFFSET);
  if(bit & BI_INVERSE)
    val = !val;
  if(val)
    lpb->wanted[ (bit & BI_TYPE) >> BI_TYPE_SHIFT ] |= mask;
  else
    lpb->wanted[ (bit & BI_TYPE) >> BI_TYPE_SHIFT ] &= ~mask;
}

int lpb_test(lpb, bit)
     struct lp_io *lpb;
     int bit;
{
  int mask = 1 << (bit & BI_OFFSET);
  if(lpb->last[ (bit & BI_TYPE) >> BI_TYPE_SHIFT ] & mask)
    return (bit & BI_INVERSE)? 0 : 1;
  else
    return (bit & BI_INVERSE)? 1 : 0;
}

void lpb_flush(lpb)
     struct lp_io *lpb;
{
  int i;
  for(i=0; i<3; i+=2) /* gratuitous loop construct */
    if(lpb->wanted[i] != lpb->last[i])
    {
      out_byte(lpb->base + i, (unsigned char)(lpb->wanted[i]));
      lpb->last[i] = lpb->wanted[i];
    }
}

void lpb_refresh(lpb)
     struct lp_io *lpb;
{
  lpb->last[1] = in_byte(lpb->base + 1);
}

/* ************************************************************************ */

/* lpb_cfg_read() and helpers */

/* NOTE that any entry in the names vector with with the name "base"
   is treated specially; it is read as an integer (with C syntax for
   octal & hex) instead of being parsed as a bit spec

   Also note that the parseBit() routine is very simple minded right
   now and should probably be extended a little bit */

#define MAXLINE 512

static int parseBit(char *);

int lpb_cfg_read(filename, namec, namev)
     const char *filename;
     int namec;
     struct lp_name *namev;
{
  FILE *fp;
  char buf[MAXLINE];
  char *line, *cp;
  int wordlen;
  enum { wtNone, wtNumber, wtName } wordtype;
  int word;

  /* init */
  fp = fopen(filename, "r");
  if(!fp)
  {
    perror(filename);
    return -1;
  }

  while(fgets(buf, MAXLINE, fp))
  {
#ifdef LPBDEBUG
    printf("<%s>\n", buf); 
#endif

    /* Trim leading whitespace. */
    for(cp = buf; *cp && isspace(*cp); cp++)
      ;
    line = cp;

    /* Remove comments. */
    if((cp = index(line, '#')))
      *cp = (char)0;

    /* Skip blank lines. */
    if(!*line)
      continue;

    /* Extract the first word of the line. */
    for(cp = line; *cp; cp++)
      if(isspace(*cp) || (*cp == '=') || (*cp == ':'))
	break;

    wordlen = cp - line;

#ifdef LPBDEBUG
    printf("[key <%.*s>]\n", wordlen, line); 
#endif

    /* Skip past the separator, if any */
    while(*cp && isspace(*cp))
      cp++;
    if((*cp == '=') || (*cp == ':'))
      cp++;

    /* Find out what kind of word this is. */
    wordtype = wtNone;

    if(!wordtype)
      for(word = 0; word < namec; word ++)
	if(wordlen == strlen(namev[word].name) &&
	   !strncmp(line, namev[word].name, wordlen))
	{
	  wordtype = wtName;
#ifdef LPBDEBUG
	  printf("[index %d]\n", word); 
#endif
	  break;
	}

    if(wordtype == wtName && wordlen == 4 && !strncmp(line, "base", wordlen))
      wordtype = wtNumber;

#ifdef LPBDEBUG
    printf("[wordtype = %d]\n", wordtype); 
#endif

    /* Parse the word. */
    switch(wordtype)
    {
      case wtNone:
        fprintf(stderr, "%s: don't know what to do with \"%.*s\"\n",
		filename, wordlen, line);
	break;
      case wtName:
	*(namev[word].value) = parseBit(cp);
	break;
      case wtNumber:
	*(namev[word].value) = strtoul(cp, (char **)NULL, 0);
	break;
    }
  } /* end of for-each-line loop */

  fclose(fp);

  return(0);
}


static struct { char *name; int nameLen, value; } symBits[] = {
  { "busy",  4, BI_STAT | 7 },
  { "ack",   3, BI_STAT | 6 },
  { "error", 5, BI_STAT | 3 },
  { NULL,    0, 0 }
};

static int parseBit(spec)
     char *spec;
{
  char *cp = spec;
  int val = 0;
  int valid = 0;
  int i;

#ifdef LPBDEBUG
  printf("[parsing bit <%s>]\n", spec); 
#endif

  if(!*cp)
    return -1;

  do 
  {
#ifdef LPBDEBUG
    printf("@%s,%d\n", cp, val);  
#endif
    switch(*cp)
    {
      case ' ':
      case '\t':
      case '\n':
	break;     /* ignore whitespace */
	
      case '!':
      case '~':
        val ^= BI_INVERSE;  /* inversion */
	break;

      case 'd':
      case 'D':
	val &= ~(BI_TYPE | BI_OFFSET);
	val |= strtol(cp+1, &cp, 10) | BI_DATA;
	cp--;
	valid = 1;
	break;

      case 'c':
      case 'C':
	val &= ~(BI_TYPE | BI_OFFSET);
	val |= strtol(cp+1, &cp, 10) | BI_CTL;
	cp--;
	valid = 1;
	break;

      case 's':
      case 'S':
	val &= ~(BI_TYPE | BI_OFFSET);
	val |= strtol(cp+1, &cp, 10) | BI_STAT;
	cp--;
	valid = 1;
	break;

      default:
	for(i = 0; symBits[i].name; i++)
	  if(!strncmp(cp, symBits[i].name, symBits[i].nameLen))
	  {
	    cp += symBits[i].nameLen - 1;
	    val &= ~(BI_TYPE | BI_OFFSET);
	    val |= symBits[i].value;
	    valid = 1;
	    break;
	  }
	if(symBits[i].name)
	  break;
	
	fprintf(stderr, "unrecognized pin name: \"%s\"\n", cp);
	return -1;
    }
  }
  while(*++cp);

  if(!valid) 
    return -1;
  return val;
}

static const char * const typeNames[4] =
  { "data", "status", "control", "<bad type>" };

void lpb_dump_names(namec, namev)
     int namec;
     struct lp_name *namev;
{
  int v, i;

  for(i=0; i<namec; i++)
  {
    printf("%s = ", namev[i].name);
    if(!strcmp(namev[i].name, "base"))
    {
      printf("0x%03x\n", *(namev[i].value));
      continue;
    }
    v = *(namev[i].value);
    if(v == -1)
      printf("undefined");
    else
    {
      printf("%s bit %d",
	     typeNames[ (v & BI_TYPE) >> BI_TYPE_SHIFT ],
	     v & BI_OFFSET);
      if(v & BI_INVERSE)
	printf(" inverted");
    }
    putchar('\n');
  }
}

