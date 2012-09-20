/*
   io_ports.h - defs & decls for lower level bit I/O routines

   written and copyright October 1994 Wim Lewis, wiml@{netcom,omnigroup}.com
   distribution & re-use ok as long as you use this power wisely
   and only for good, never for evil
*/

#ifndef IO_PORTS_H
#define IO_PORTS_H

/*************************************************************************
   Stuff in io_ports.c
 *************************************************************************/

void open_io(void);                    /* open /dev/port */
void close_io(void);                   /* close /dev/port */
unsigned char in_byte(int);            /* read one byte */
void out_byte(int, unsigned char);     /* write one byte */

extern int dev_port_fd;                /* the fd used to talk to the kernel */

/* Note: Bit masks for using the status (base+1) and ctl (base+2) ports can 
   be found in /usr/include/linux/lp.h under Linux. Be sure to use 
   the LP_Pwhatever constants and *not* the LP_whatever constants 
   (which are for the device driver's status word, not the hardware's). */

/*************************************************************************
   Stuff in lp_io.c
 *************************************************************************/

/* This allows simple buffering of the bit changes to a line printer port */
struct lp_io
{
  unsigned base;  /* base I/O address */

  /* Most recently written/read values */
  unsigned last[3];

  /* Desired values */
  unsigned wanted[3];
};

void lpb_flush(struct lp_io *);  	/* write out all buffered changes */
void lpb_refresh(struct lp_io *);	/* read in current values */

/* These two functions use "bit indicators" encoded into an int;
   see the bitmasks below. */
int  lpb_test(struct lp_io *, int);
void lpb_write(struct lp_io *, int, int);

/* Bit indicators are encoded into ints and are passed to the
   set, reset, & test macros. Note that BI_TYPE is the offset from
   the parallel port base I/O address. */
#define BI_OFFSET    000007 /* Bit offset within the word         */
#define BI_DATA      000000 /* Data bit?                          */
#define BI_STAT      000010 /* Status bit?                        */
#define BI_CTL       000020 /* Control bit?                       */
#define BI_TYPE      000030 /* Mask for DATA/CTL/STAT             */
#define BI_TYPE_SHIFT 3
#define BI_INVERSE   000040 /* Negative logic                     */

/* Associats a name with a bit indicator. */
struct lp_name { char *name; int *value; };

/* read a fileful of name-indicator pairs */
int lpb_cfg_read(const char *, int, struct lp_name *);
void lpb_dump_names(int, struct lp_name *);

#endif /* IO_PORTS_H */
