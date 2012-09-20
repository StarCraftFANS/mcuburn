/* header for anything that wants to use the 16C84's serial programming mode */
#ifndef PROG84_H
#define PROG84_H

#include "io_ports.h"

/* ************************************************************************ */
/* Externally visible variables & functions                                 */

/* VARIABLES */

/* Bit indicators for the various control & feedback signals.
   See lp_io for what a bit indicator is. */
extern int p_power, p_mclr, p_clock, p_data, p_clock_f, p_data_f;

/* lp_io control structure */
extern struct lp_io lpb;

/* name of file to read pin assignments from; defaults to lp_cfg */
extern char *cfg_file;

/* set this to nonzero for copious output */
extern int p84_verbose;

/* FUNCTIONS */

/* set up and shut down the programmer, as well as the software layers
   under this (lpb_cfg_read, open_io(), etc.)                               */
void progSetup();
void progShutdown(); /* powers down */
int testHW(); /* simple hardware test sequence. called by progSetup().
	         returns non-zero on problem */

/* Routines to clock some bits through the chip serially. */
unsigned int pipeBits(unsigned int, int); /* send bits through the '84 */
void xPipeBits(unsigned int, int); /* send bits, confirm, exit on error */
void genericWrite(int, unsigned int); /* send command & 14 data bits */
unsigned int genericRead(int); /* send command, then read 14 data bits */

/* ************************************************************************ */
/* Defines for the descriptive programmer.                                  */

/* MICROCHIP'S SERIAL COMMANDS */
#define SC_LOADCFG	 0
#define SC_LOADPROG	 2
#define SC_READPROG	 4
#define SC_INCREMENT	 6
#define SC_PROGRAM	 8
#define SC_PARALLEL	10  /* not used */
#define SC_LOADDATA	 3
#define SC_READDATA	 5

/* Bit-twiddling macros, for convenience */
#define SET(bitname, value) lpb_write(&lpb, p_ ## bitname, value)
#define GET(bitname) lpb_test(&lpb, p_ ## bitname)
#define FLUSH do{ lpb_flush(&lpb); lpb_refresh(&lpb); } while(0)

/* Commands that write */
#define loadConfiguration(cfg) genericWrite(SC_LOADCFG, cfg)
#define loadProg(data)         genericWrite(SC_LOADPROG, data)
#define loadData(data)         genericWrite(SC_LOADDATA, data)

/* Commands that read */
#define readProg()	genericRead(SC_READPROG)
#define readData()	genericRead(SC_READDATA)

/* Commands that neither read nor write */
#define incAddr()	xPipeBits(SC_INCREMENT, 6)
#define doProgram()	do{ xPipeBits(SC_PROGRAM, 6); usleep(10000); }while(0)

#endif /* PROG84_H */
