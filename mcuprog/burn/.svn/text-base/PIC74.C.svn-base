/*****************************************************************************
 *
 * NOTE: o NEVER program security fuse's if your using a ceramic windowed
 *         EPROM version of the PIC 16C74 microcontroller !. The fuse's can't
 *         be erased by UV light, so it's an expensive windowed OTP version.
 *
 *       o In-circuit programming requires the OSC1/CLKIN pin be grounded
 *         this in order to avoid problems with the internal program counter.
 *
 ******************************************************************************
 *
 * pic74.c : A serial mode PIC16C74 programmer
 *
 * Features:
 *
 *      o  Auto detects the hex object file formats, INHX16 or INHX8M.
 *      o  Skip unused memory locations and 0x3FFF data bytes during
 *          Programming mode.
 *      o  Features true PC speed independent 100uS programming puls.
 *      o  Embedded fuse information in object file is supported.
 *      o  Fuse information is saved in object file, if PIC16C74 is read.
 *      o  Auto programming mode, first the PIC is blankchecked then programmed
 *          and finally a verify is performed.
 *      o  Features Read/Dump mode.
 *
 * Notes/restrictions/bugs:
 *
 *      o  Software needs a IBM compliant computer.
 *      o  This program needs external hardware to be attached
 *          to a parallel port (see pp.asc for an ASCII schematic).
 *      o  Don't use from Windows; another program could grab the LPT port.
 *      o  Run pic74, pic74 -s -2 or pic74 -s -3 to initialise the LPT port.
 *      o  Run pic74, pic74 -t -2 or pic74 -t -3 to test computer and hardware.
 *      o  This is NOT a production quality programmer. (No surprise!).
 *
 * Revision history:
 *
 * ??-Aug-1996: V-0.0;  Wrote the main code in just one day !.
 * ??-Aug-1996: V-0.1b; First code to successfully program a 16C74.
 * 10-Aug-1996: V-0.1b; Removed a few bugs, thanks to Ed's help !.
 * 18-Aug-1996: V-0.1b; Modified the 100uS delay, added timeout.
 * 30-Aug-1996: V-0.1b; Added PIT hardware detect and timing correction.
 * 20-Sep-1996: V-0.1b: Added auto detect object file format INHX8M or INHX16.
 * 30-Sep-1996: V-1.0b; Software and source code released to the internet.
 *
 * ??-???-19??: V-1.0 ; if no bug reports are received by the end of this year.
 *
 *
 * This version (V-1.0b) is a public beta release. If you receive this program
 * consider yourself a beta tester (of course, you are under no
 * obligation).  I will remove beta status when 10 (arbitrary
 * I know) testers tell me they have used it to successfully program a
 * PIC using the hardware in pp.asc.
 *
 *
 * Usage:
 *
 *      pic74  [-rpbvadst] [ -lxhrwpc238 ]  objfile
 *
 * Where objfile is the program data in Intel hex format.
 * Various things, including the fuse details, can be set on the
 * command line using the switches described below. Superfluous
 * command line arguments are silently ignored and later switches
 * take precedence, even if contradictory. The hex object file format
 * is automatically detected by software, the detected file format
 * is displayed on the screen. The object file is checked for validity
 * by checking the file type, address range, checksum and lost of bytes.
 *
 * Switches (options) are introduced by - (or /) and are encoded as
 * a sequence of one or more characters from the sets {rpbvadst} and/or
 * {lxhrwpc238} Switches can be given individually (like -r /2 -p) or
 * in groups (like -r2p or /rp2 or /2/r/p which are all synonyms).
 * Switches are recognized anywhere in the command line
 * (but don't use the DOS-like pic74/r/p/2  form - this should be written
 * pic74 /r/p/2;  space around switch groups is important).
 *
 * There are six different mode's of operation:
 *
 * Modes:
 *    o    d           -  read memory and dump to screen.
 *    o    r           -  read memory and store data in objfile.
 *    o    p           -  program memory and read data from objfile.
 *    o    v           -  verify program memory and data from objfile.
 *    o    b           -  blankcheck program and configuration memory.
 *    o    a           -  blankcheck, program and verify data from objfile.
 *
 * There are three logical groups of switches: fuses, ports, hex-types.
 * They can be mixed freely.
 *
 * Fuses:
 *    o    l, x, h, r  -  LP, XT, HS or RC oscillator (default LP).
 *    o    w           -  enable watchdog timer.
 *    o    p           -  enable power-up timer.
 *    o    c           -  enable code protection.
 * Ports:
 *    o   2, 3         -  hardware on LPT2 or LPT3 (default is LPT1).
 * Hex-types:
 *    o   8            -  objfiles are Intel Hex-8, i.e. INHX8M
 *                        (default is Intel Hex-16, i.e. INHX16)
 *                        (The software auto detects the Intel hex type)
 *
 * E.g., -a -rp28 means use autoprogram mode (blankcheck, program and verify)
 * set the fuses to use the RC oscillator, no watchdog, use power-up timer,
 * and don't protect code; assume the hardware is on LPT2 and the objfiles
 * are in INHX8M format. The software will detect and correct if a different
 * file type is detected, specifying the file type makes sense if the data is
 * being saved on disk using one of the two hex formats. The INHX16 hex format
 * is however prefered and advised above the INHX8M hex format.
 *
 *
 * Improvements :
 *
 *      o Support the 16C74A and other members of the Microchip family (easy),
 *         if I receive a request or have to kill some time,
 *
 *      o Write a programmer for the 12C5XX family of chips (easy),
 *         ASAP, when I receive the windowed 12C5XX version.
 *
 *      o IýC support using the same hardware (easy),
 *         when I need it.
 *
 *      o Porting the sofware to Window NT (porting to W95 is a waste of time).
 *         ??.
 *
 *      o Making the programmer a true production quality programmer.
 *        VCC switching (4.5v-5.5v). (hardware and software modification)
 *
 *      o Add a GUI (waste of time?).
 *      o Lots of other stuff I just don't have time for.
 *
 * Acknowledgements:
 *
 * Many thanks to Edwin Gouweloos for trusting my programming skills
 * and testing beta release 0.1b. You must be really MAD or BLIND if
 * you trust my programming skills Ed, Especially the first release.
 * I hope the SYNC-MATE will be a success, Ed !. The SYNC-MATE is worlds
 * smallest RISC based sync/async wonder box, with super-computer power.
 *
 * Thanks to David Tait for his idea of the LPT programmer and his
 * pp.exe program (16C84 programmmer) without it I wouldn't be able to
 * write this program (many thanks).
 *
 * The programming algorithms used are taken from the data sheets
 * DS30228D/E/F (C) 1995/1996 Microchip Technology Incorporated.
 * Thanks to Arizona Microchip Technology Ltd for making this information
 * available without restriction.
 *
 * The PC printer port FAQ by Zhahai Stewart for useful info on
 * PC parallel ports.
 *
 *
 * Hardware requirements:
 *
 * The software is tested using the following hardware:
 *
 *      80386 33 Mhz Portable by Texas Instruments.     (Not quiet Compliant)
 *      80486DX2 66 Mhz SOYO Motherboard 25E2.          (Fully Compliant)
 *      Pentium 133 Mhz ASUS Motherboard P55T2P4.       (Fully Compliant)
 *      Pentium Pro 200 Mhz Intel Motherboard Aurora.   (Fully Compliant)
 *
 * The software is written in such a way that it's CPU speed independent,
 * however I made the assumption that the used hardware is IBM compliant.
 * This means that the PIT timer 1 is set to 15.08 uS (count 18), and that
 * the timer 1 overflow bit can be found at address 0x61,4. If this isn't
 * so then a special timeout mechanism ensures that the program puls doesn't
 * exceed the length specified by MICROCHIP. However before programming a
 * 16C74 it's advised to test your computers hardware by issuing the -t
 * command. This command will test if your computers hardware is IBM
 * compliant, if it isn't then the program will try to adjust the hardware
 * in such a way that it meets the standard. We strongly advice not to use
 * the programmer if the hardware can't be made compliant, if you do use
 * the programmer it can/will permanently damage the device.
 *
 *
 * Legal stuff:
 *
 * This is copyright software, but I do not seek to profit from
 * it.  I wrote it in the hope that it will prove useful.  Feel
 * free to modify the program, but please DO NOT distribute
 * modified versions.  Instead, send me your modifications and
 * improvements so that they may, at my discretion, be included in a
 * future release.
 *
 * Although I cannot undertake to support this software, I would
 * welcome bug reports, suggestions or comments which should be
 * sent to:
 *
 *    Paul Vollebregt
 *    Po Box 65824
 *    The Hague 2506 CE.
 *    The Netherlands, NL.
 *    EUROPE.
 *
 *    e-mail: vollebre@worldonline.nl
 *
 *
 * Copyright (C) 1996 Paul Vollebregt
 * This program is free software.  Permission is granted to use,
 * copy, or redistribute this program so long as it is not sold
 * for profit.
 *
 * THIS PROGRAM IS PROVIDED AS IS AND WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <conio.h>
#include <string.h>
#include <time.h>

#define U7407    /* define U7407 if H/W uses non-inverting buffers */

#define IN      64
#define VPP     8
#define VDD     4
#define CLK     2
#define OUT     1

#define LDCONF  0
#define LDPROG  2
#define RDPROG  4
#define INCADD  6
#define BEGPRG  8
#define ENDPRG  14

#define VPPDLY  500    /* this is in milliseconds */
#define PRGDLY  3      /* this is in 15.08 microseconds */

#define PSIZE   4096
#define CSIZE   8

#define BODEN	64
#define CP      48
#define PWRTE   8
#define WDTE    4

#define RC      3
#define HS      2
#define XT      1
#define LP      0

#define INHX16  16
#define INHX8M  8
#define Unknowntype 0

#ifdef U7407
#define inbit           (inportb(s_reg)&IN)
#define vppon           (d_bits |= VPP)
#define vppoff          (d_bits &= ~VPP)
#define vddon           (d_bits |= VDD)
#define vddoff          (d_bits &= ~VDD)
#define clkhi           (d_bits |= CLK)
#define clklo           (d_bits &= ~CLK)
#define outhi           (d_bits |= OUT)
#define outlo           (d_bits &= ~OUT)
#else
#define inbit           (~inportb(s_reg)&IN)
#define vppon           (d_bits &= ~VPP)
#define vppoff          (d_bits |= VPP)
#define vddon           (d_bits &= ~VDD)
#define vddoff          (d_bits |= VDD)
#define clkhi           (d_bits &= ~CLK)
#define clklo           (d_bits |= CLK)
#define outhi           (d_bits &= ~OUT)
#define outlo           (d_bits |= OUT)
#endif /* U7407 */
#define assert          (outportb(d_reg,d_bits))
#define Sdelay          (outportb(d_reg,d_bits))        // CPU Speed independent
							//  small delay routine.
#define yes     (c=getche(), c == 'y' || c == 'Y')

int progbuf[PSIZE];
int confbuf[CSIZE];
int fuses = LP + CP + 0x3F00;

int d_bits = 0;
int d_reg;
int s_reg;
int check;
int iport = 0;
int hextype = INHX16;
int set_fuses = 0;

char *progname = "PIC16C74 Programmer";
char *version = "Version 1.0b";
char *copyright = "Copyright (C) 1996 Paul Vollebregt.";
char *email = "vollebre@worldonline.nl";

// 5sec
void microdelaytest(void)        // Go for Warp speed.
{
   asm     pushf
   asm     push ax
   asm     push cx
   asm     mov  cx,5              // 5 * 65535 * 15.08 us = 5 sec.

next3:
   asm	   push cx
   asm     mov cx,0xFFFF          // 65535 * 15.08 us = 1 sec.
   asm     in  al,0x61
   asm     and al,0x10
   asm     mov ah,al

next2:
   asm     push cx                // Timeout 100uS if no puls is detected.      
   asm     mov cx,80              // 80 * ca 1.25uS = 100 uS.
next1:                            // test 0x61 for 100 uS, if still
   asm     in  al,0x61		  // equal then error and exit.
   asm     and al,0x10
   asm     cmp ah,al
   asm     loope next1
   asm     jz error               //  Q, timeout ?.

   asm     pop cx
   asm     mov ah,al
   asm     loop next2

   asm     pop  cx
   asm     loop next3

   asm     jmp exit

error:
   asm     pop cx                 // Timeout, clear stack.
   asm     pop cx

exit:
   asm     pop cx
   asm 	   pop ax
   asm     popf
}

// True PC speed independent 100uS delay.
// Timeout is supported for noncompliant hardware.
void microdelay(void)		// Go for Warp speed.
{
   asm     pushf
   asm     push ax
   asm     push cx
   asm     cli
   asm     mov cx,PRGDLY     // 3 * 15 us = 45.0 us + command(ENDPRG) = 100us
   asm     in  al,0x61
   asm     and al,0x10
   asm     mov ah,al

next2:
   asm     push cx                // Timeout 100uS if no puls is detected.
   asm     mov cx,80              // 80 * ca 1.25uS = 100 uS.
next1:                            // test 0x61 for 100 uS, if still
   asm     in  al,0x61		  // equal then error and exit.
   asm     and al,0x10
   asm     cmp ah,al
   asm     loope next1
   asm     jz error               //  Q, timeout ?.

   asm     pop cx
   asm     mov ah,al
   asm     loop next2
   asm     jmp exit

error:
   asm     pop cx                 // Timeout, clear stack.

exit:
   asm     pop cx
   asm     pop ax
   asm     popf
}

void idle_mode(void)
{
   vppoff, clklo, outlo, assert;
   delay(VPPDLY);
   vddoff, assert;
   delay(VPPDLY);
   printf("\nVDD and VPP off\n");
}

void prog_mode(void)
{
   printf("\nVDD and VPP on\n");
   vppoff, vddon, clklo, outlo, assert;
   delay(VPPDLY);
   vppon, assert;
   delay(VPPDLY);
}

void quit(char *s, FILE *fp)
{
   if (fp != NULL) fclose(fp);
   fprintf(stderr,"\nERROR: %s\n",s);
   idle_mode();
   exit(1);
}

void usage(void)
{
   char c;
   printf("%s %s %s\n",progname,version,copyright);
   printf("\nUsage: pic74  [ -mode ] [ -options ]  objfile \n\n");
   printf("Example: pic74 -p -hw myprog.obj\n\n");
   printf("     mode:\n");
   printf("       dump:     d = read memory and dump to screen\n");
   printf("       read:     r = read memory and store data in objfile\n");
   printf("       program:  p = program memory and read data from objfile\n");
   printf("       verify:   v = verify program memory and data from objfile\n");
   printf("       blank:    b = blankcheck program and configuration memory\n");
   printf("       auto:     a = blankcheck, program and verify data from objfile\n\n");
   printf("     options:\n");
   printf("       Fuses:    l = LP,   x = XT,    h = HS,  r = RC\n");
   printf("                 w = WDTE, p = PWRTE, c = CP\n");
   printf("       Port:     2 = LPT2, 3 = LPT3\n");
   printf("       Hex:      8 = INHX8M\n");
   printf("       Defaults: LP, (no) WDTE, (no) PWRTE, (no) CP, LPT1, INHX16\n\n");
   printf("NOTES  Before you start test your computer and hardware by: pic74 -t\n");
   printf("       Don't program the security fuses of the PIC 16C74 EPROM version !.");
   printf("\n\nmore (y/n) : ");
   if (yes)
   {
    printf("\n\nBug reports to: %s\n",email);
    printf("	        Paul Vollebregt\n");
    printf("	        Po Box 65824\n");
    printf("	        The Hague 2506 CE.\n");
    printf("	        The Netherlands, NL.\n");
    printf("	        EUROPE.\n");
    exit(1);
   }
   printf("\n");
   exit(1);
}

void test_hw(void)
{
    int b;

    printf("Testing programmer hardware, please wait\n\n");
    printf("VDD and VPP on\n");
    vppoff, vddon, outlo, clklo, assert;   /* better have VDD on in case PIC is there */
    delay(VPPDLY);
    vppoff, vddon, outhi, clklo, assert;
    b = inbit;
    vppoff, vddon, outlo, clklo, assert;
    if ( b != IN || inbit != 0 )
    {
       fprintf(stderr,"NOTE: Using LPT at 0x%04X\n",d_reg);
       quit("Programmmer hardware fault.  Check power, connections and port used.",NULL);
    }
    else printf("\nProgrammmer hardware detected.\n");
    idle_mode();
}

void start(void)
{
   int c;

   printf("\nPlease insert PIC 16C74 and press (y/n) to start: ");
   if (!yes)
   {
    printf("\n");
    exit(1);
   }
   printf("\n");
}


void setup(void)
{
   int i, b;

   vppoff, vddoff, clklo, outlo, assert;
   d_reg = peek(0,0x408+iport);  /* find the base address of LPT port */
   s_reg = d_reg+1;

   switch ( d_reg ) {  /* I don't want to blow anything up, so check port */
       case 0x3BC:
       case 0x378:
       case 0x278: break;
       default:  fprintf(stderr,"NOTE: LPT at 0x%04X\n",d_reg);
		 quit("Unlikely LPT address",NULL);
   }

   test_hw();

   for ( i=0; i<PSIZE; ++i )
      progbuf[i] = 0x3FFF;
   for ( i=0; i<CSIZE; ++i )
      confbuf[i] = 0x3FFF;
}

void clock_out(int bit)
{
   bit? outhi: outlo, clkhi, assert;
   Sdelay;
   clklo, assert;
   Sdelay;
   outlo, assert;
   Sdelay;
}

int clock_in(void)
{
   int b;

   outhi, clkhi, assert;
   Sdelay;
   clklo, assert;
   Sdelay;
   b = inbit? 1: 0;
   return b;
}

void out_word(int w)
{
   int b;

   clock_out(0);
   for ( b=0; b<14; ++b ) clock_out(w&(1<<b));
   clock_out(0);
}

int in_word(void)
{
   int b, w;

   (void) clock_in();
   for ( w=0, b=0; b<14; ++b ) w += clock_in()<<b;
   (void) clock_in();
   return w;
}

void command(int cmd)
{
   int b;

   outlo, assert;
   Sdelay;
   for ( b=0; b<6; ++b ) clock_out(cmd&(1<<b));
   outhi, assert;
   Sdelay;
}

void get_option(char *s)
{
   char *sp;

   for ( sp=s; *sp; ++sp )
      switch ( *sp )
      {
	 case 'l':
	 case 'L': fuses = (fuses&0x3FFC) + LP; set_fuses = 1; break;
	 case 'x':
	 case 'X': fuses = (fuses&0x3FFC) + XT; set_fuses = 1; break;
	 case 'h':
	 case 'H': fuses = (fuses&0x3FFC) + HS; set_fuses = 1; break;
	 case 'r':
	 case 'R': fuses = (fuses&0x3FFC) + RC; set_fuses = 1; break;
	 case 'w':
	 case 'W': fuses |= WDTE; set_fuses = 1; break;
	 case 'p':
	 case 'P': fuses |= PWRTE; set_fuses = 1; break;
	 case 'c':
	 case 'C': fuses &= ~CP; set_fuses = 1; break;
	 case '2': iport = 2; break;
	 case '3': iport = 4; break;
	 case '8': hextype = INHX8M; break;
	 case '-':
	 case '/': break;
	 default: usage();
      }
   confbuf[7]=fuses;
}


void ask_fuses(void)
{
   int c, osc, wdte, pwrte, cp, boden;

   do {
      printf("\nOscillator type:\n 0)-LP  1)-XT  2)-HS  3)-RC ? ");
      osc = getche() - '0';
   } while ( osc < 0 || osc > 3 );

   printf("\nEnable watchdog timer (y/n) ? ");
   wdte = yes? WDTE: 0;
   printf("\nEnable power-up timer (y/n) ? ");
   pwrte = yes? PWRTE: 0;
   printf("\n         Protect code (y/n) ? ");
   cp = yes?  0: CP;
   printf("\n         Brown out enabled (y/n) ? ");
   boden = yes?  0: BODEN;
   printf("\n");
   fuses = osc + wdte + pwrte + cp + boden;
}


int hexdigit(FILE *fp)
{
   int c;

   if ( (c=getc(fp)) == EOF )
      quit("Unexpected EOF in object file.",fp);

   c -= c>'9'? 'A'-10: '0';
   if ( c<0 || c>0xF )
    quit("Hex digit expected, error in object file.",fp);
   return c;
}

int hexbyte(FILE *fp)
{
   int b;

   b = hexdigit(fp);
   b = (b<<4) + hexdigit(fp);
   check += b;               /* nasty side-effect, but useful trick */
   return b;
}

unsigned hexword(FILE *fp)
{
   unsigned w;

   w = hexbyte(fp);
   w = (w<<8) + hexbyte(fp);
   return w;
}

#define swab(w) (((w)>>8)+(((w)&0xFF)<<8))   /* swap bytes */

void loadhex(FILE *fp, int buf1[], int buf2[], int buf1size, int buf2size)
{
   int type, nwords, i;
   unsigned address, w;

   type = 0;
   rewind(fp);
   while ( type != 1 )
   {
      if ( getc(fp) != ':' )
       quit("Expected ':', in object file.",fp);
      check = 0;
      w = hexbyte(fp);
      nwords = (hextype == INHX8M)? w/2: w;
      w = hexword(fp);
      address = (hextype == INHX8M)? w/2: w;
      type = hexbyte(fp);
      for ( i=0; i<nwords; ++i, ++address )
      {
       if ( address < 0x2000 )
       {
	if ( address >= buf1size )
	 quit("Impossible address found in object file.",fp);
	w = hexword(fp);
	buf1[address] = (hextype == INHX8M)? swab(w): w;
       }
       else
       {
	if ( address >= buf2size+0x2000 )
	 quit("Impossible address found in object file.",fp);
	w = hexword(fp);
	buf2[address-0x2000] = (hextype == INHX8M)? swab(w): w;
       }
      }
      (void) hexbyte(fp);
      (void) getc(fp);
      if ( check&0xFF )
     quit("Checksum error in object file.",fp);
   }
}

// return 0 if buffer not dumped.
// return 1 if buffer dumped.
int dumphex(int buf1[], int buf2[], int buf1size, int buf2size)
{
   int i, j, data;
   unsigned address;

   char string[80]="";
   char str[10]="";
   char text[20]="";

// read program memory
   printf("program memory\n");
   for( address=0; address < buf1size; address)
   {
    text[0]='\0';
    sprintf(string,"  %04X :  ",address);
    for( i=0; i < 8; i++)
    {
     data=buf1[address];
     if (((data>>8) >= 48)&&((data>>8) <= 122)) sprintf(str,"%c",(data>>8));
     else sprintf(str,".");
     strcat(text,str);
     if (((data&0xFF) >= 48)&&((data&0xFF) <= 122)) sprintf(str,"%c",(data&0xFF));
     else sprintf(str,".");
     strcat(text,str);
     if (i==3) sprintf(str,"%04X ³ ",data);
     else sprintf(str,"%04X ",data);
     strcat(string,str);
     address++;
    }
    printf("%s ³  %s\n",string, text);
   }

// read configuration memory
   printf("\nconfiguration memory\n");
   for( address=0x2000, j=0; address < (0x2000+buf2size); address)
   {
    text[0]='\0';
    sprintf(string,"  %04X :  ",address);
    for( i=0; i < 8; i++, j++)
    {
     data=buf2[j];                      // get data word.
     if (((data>>8) >= 48)&&((data>>8) <= 122)) sprintf(str,"%c",(data>>8));
     else sprintf(str,".");
     strcat(text,str);
     if (((data&0xFF) >= 48)&&((data&0xFF) <= 122)) sprintf(str,"%c",(data&0xFF));
     else sprintf(str,".");
     strcat(text,str);
     if (i==3) sprintf(str,"%04X ³ ",data);
     else sprintf(str,"%04X ",data);
     strcat(string,str);
     address++;
    }
    printf("%s ³  %s\n",string, text);
   }
   return(1);
}


// return 0 if buffer not saved.
// return 1 if buffer saved.
int savehex(FILE *fp, int buf1[], int buf2[], int buf1size, int buf2size)
{
   int type, nwords, i, j, data;
   unsigned address, Xaddress;

   char string[50];
   char str[10];

// read program memory
   for( address=0; address < buf1size; address)
   {
    nwords= 0x08;
    nwords = (hextype == INHX8M)? nwords*2: nwords;
    Xaddress = (hextype == INHX8M)? address*2: address;
    check = nwords;
    check += Xaddress>>8;
    check += Xaddress;
    sprintf(string,":%02X%04X00",nwords,Xaddress);
    for( i=0; i < 8; i++)
    {
     data=buf1[address];
     check += data>>8;
     check += data;
     data = (hextype == INHX8M)? swab(data): data;
     sprintf(str,"%04X",data);
     strcat(string,str);
     address++;
    }
    check=~(check-1);
    sprintf(str,"%02X\n",(check&0xFF));
    strcat(string,str);
    if (fputs(string, fp)==EOF) return(0);
   }

// read configuration memory
   for( address=0x2000, j=0; address < (0x2000+buf2size); address)
   {
    nwords= 0x08;
    nwords = (hextype == INHX8M)? nwords*2: nwords;
    Xaddress = (hextype == INHX8M)? address*2: address;
    check = nwords;
    check += Xaddress>>8;
    check += Xaddress;
    sprintf(string,":%02X%04X00",nwords,Xaddress);
    for( i=0; i < 8; i++, j++)
    {
     data=buf2[j];                      // get data word.
     check += data>>8;                  // calculate checksum.
     check += data;
     data = (hextype == INHX8M)? swab(data): data;
     sprintf(str,"%04X",data);          // add to string.
     strcat(string,str);
     address++;
    }
    check=~(check-1);                   // two's complement.
    sprintf(str,"%02X\n",(check&0xFF));
    strcat(string,str);                 // add checksum.
    if (fputs(string, fp)==EOF) return(0);
   }
   sprintf(string,":00000001FF\n");     // last line.
   if (fputs(string, fp)==EOF) return(0);

   return(1);
}

// return hex file type.
short FindFileType(FILE *fp)
{
  unsigned short nwords, i;

  if ( getc(fp) == ':')                    // :
  {
   if ((nwords = hexbyte(fp)) <= 0x10)     // size
    if (hexword(fp) <= 0x4000)             // address
     if (hexbyte(fp) != 0x01)              // type (EOF)
      for(i=0;i<=(nwords*4)+2;i++)	  // assume INHX16 Type.
       if (getc(fp)==0x0A)
        if (i==(nwords*4)+2)               // Q, INHX16 ?.
	{
	  printf("\nINHX16 hex format detected.\n");
	  return INHX16;
	}
	else if (i==(nwords*2)+2)          // Q, INHX8M ?.
	     {
	       printf("\nINHX8M hex format detected.\n");
	       return INHX8M;
	     }
	     else
	     {
	       printf("\nERROR: Hex file corrupted.\n");
	       return Unknowntype;
	     }
  }
  printf("\nERROR: Unknown hex File Type.\n");
  return Unknowntype;
}

void programcycle(int data)
{
   command(LDPROG);         // Load data command.
   out_word(data);          // Send Data byte
   command(BEGPRG);         // Send Begin Programming command.
   microdelay();            // wait 100 us.
   command(ENDPRG);         // Send End Programming Command.
}

// return 0 if blank error.
// return 1 if blank.
int blankcheck(void)
{
   int i;

   prog_mode();             // switch VDD on and raise VPP after 100 ms.
   printf("\nBlankchecking\n");
   for ( i=0; i<PSIZE; i++, command(INCADD) )
   {
    command(RDPROG);        // Get Data word.
    if (0x3FFF != in_word() & 0x3FFF)
    {
     printf("\nBlankcheck failed at address 0x%04X \n",i);
     idle_mode();
     return(0);
    }
    if ((i & 0x00FF)==0) printf(".");
   }
   command(LDCONF);         // Load Configuration.
   out_word(0x3FFF);        // Set all-bits to "1"
   for ( i=0; i<CSIZE; i++, command(INCADD) )
   {
    command(RDPROG);        // Get Data word.
    if (0x3FFF != in_word() & 0x3FFF)
    {
     printf("\nBlankcheck failed at address 0x%04X \n",i+0x2000);
     idle_mode();
     return(0);
    }
   }
   printf("\nBlankcheck successful\n");
   idle_mode();
   return(1);
}

// return 0 if verify error.
// return 1 if verify ok.
int verifycheck(void)
{
   int i, *buf1, *buf2;

   buf1 = progbuf;
   buf2 = confbuf;

   prog_mode();             // switch VDD on and raise VPP after 100 ms.
   printf("\nVerifying\n");

// verify program memory.
   for ( i=0; i<PSIZE; i++, command(INCADD) )
   {
    command(RDPROG);        // Get Data word.
    if (buf1[i] != in_word() & 0x3FFF)
    {
     printf("\nverifycheck failed at address 0x%04X \n",i);
     idle_mode();
     return(0);
    }
    if ((i & 0x00FF)==0) printf(".");
   }

// verify configuration memory.
   command(LDCONF);         // Load Configuration.
   out_word(0x3FFF);        // Set all-bits to "1"
   for ( i=0; i<CSIZE; i++, command(INCADD) )
   {
    command(RDPROG);        // Get Data word.
    if (i < 4)
    {
     if (buf2[i] != in_word() & 0x3FFF)
     {
      printf("\nverifycheck failed at address 0x%04X \n",i+0x2000);
      idle_mode();
      return(0);
     }
    }
    else
    {
     if ((i==7)&&(buf2[i]&0x003F != (in_word() & 0x003F)))
     {
      printf("\nverifycheck failed at address 0x%04X \n",i+0x2000);
      idle_mode();
      return(0);
     }
    }
   }
   printf("\nverifycheck successful\n");
   idle_mode();
   return(1);
}

void readdata(void)
{
   int i, *buf1, *buf2;

   buf1 = progbuf;
   buf2 = confbuf;

   prog_mode();             // switch VDD on and raise VPP after 100 ms.
   printf("\nReading Program and Configuration memory\n");

// read program memory.
   for ( i=0; i<PSIZE; i++, command(INCADD) )
   {
    command(RDPROG);        // Get Data word.
    buf1[i] = in_word();
    if ((i & 0x00FF)==0) printf(".");
   }

// read configuration memory.
   command(LDCONF);
   out_word(0x3FFF);
   for ( i=0; i< CSIZE; i++, command(INCADD) )
   {
     command(RDPROG);           // Get configuration words.
     buf2[i] = in_word();       // Store.
   }
   idle_mode();
}

// return 0 if programming error.
// return 1 if programming ok.
int progdata(void)
{
   int i, j, n, b, *buf1, *buf2;

   buf1 = progbuf;
   buf2 = confbuf;

   prog_mode();             // switch VDD on and raise VPP after 100 ms.
   printf("\nProgramming program memory\n");

// program program memory.
   for ( i=0; i<PSIZE; i++, command(INCADD) )
   {
    n=0;
    if (buf1[i]!=0x3FFF)       // Skip 0x3FFF
    {
     do
     {
      n++;
      programcycle(buf1[i]);   	// program word.
      command(RDPROG);        	// Get Data word.
      if ( buf1[i] == in_word())
      {
       n=n*3;
       do
       {
	n--;
	programcycle(buf1[i]);   	// program word.
       }
       while( n != 0);
       break;
      }
     }
     while( n < 25);
    }
    if ( n == 0)
    {
     if ((i & 0x00FF)==0) printf(".");
    }
    else
    {
     printf("\nprogramming error at address 0x%04X \n",i);
     idle_mode();
     return(0);
    }
   }

// program configuration memory.
   printf("\nProgramming configuration memory\n");
   command(LDCONF);
   out_word(0x3FFF);
   for ( i=0x2000, j=0; j<CSIZE; i++, j++, command(INCADD) )
   {
    n=0;
    if ((buf2[j]!=0x3FFF)&&!((j >= 4)&&(j <= 6)))
    {
     do
     {
      n++;
      programcycle(buf2[j]);   	// program word.
      command(RDPROG);        	// Get Data word.
      if ((( b = in_word()) == buf2[j] )&&( i < 0x2004))
      {
       n=n*3;
       do
       {
	n--;
	programcycle(buf2[j]);   	// program word.
       }
       while( n != 0);
       break;
      }
      else
      {
       if (( i == 0x2007 )&&((b&0x003F) == (buf2[j]&0x003F)))
       {
	n=n*3;
	do
	{
	 n--;
	 programcycle(buf2[j]);   	// program word.
	}
	while( n != 0);
	break;
       }
      }
     }
     while( n < 25);
    }
    if ( n == 0)
    {
     printf(".");
    }
    else
    {
     printf("\nprogramming error at configuration address 0x%04X \n",i);
     printf("\nexpecting 0x%04X , reading 0x%04X from chip \n",buf2[j], b);
     idle_mode();
     return(0);
    }
   }
   idle_mode();

   return(1);
}

void dump(void)
{
   readdata();
   dumphex(progbuf, confbuf, PSIZE, CSIZE);
}

void verify(int argc, char * argv[])
{
   FILE *objfp;
   char c;

   if ((c = *argv[argc-1]) != '-' && c != '/')
   {
    if ( (objfp = fopen(argv[(argc-1)],"r")) == NULL )
     quit("Can't open objfile",NULL);
    if ( (argc == 4)&&((( c = *argv[2]) == '-' || c == '/' ))) get_option(++argv[2]);
    hextype=FindFileType(objfp);
    if (hextype) loadhex(objfp, progbuf, confbuf, PSIZE, CSIZE);
    fclose(objfp);
    if (hextype) verifycheck();
   }
   else  quit("Can't find objfile",NULL);
}

void autoprogram(int argc, char * argv[])
{
   FILE *objfp;
   char c;

   if ((c = *argv[argc-1]) != '-' && c != '/')
   {
    if ( (objfp = fopen(argv[(argc-1)],"r")) == NULL )
     quit("Can't open objfile",NULL);
    if ( (argc == 4)&&((( c = *argv[2]) == '-' || c == '/' ))) get_option(++argv[2]);
    hextype=FindFileType(objfp);
    if (hextype) loadhex(objfp, progbuf, confbuf, PSIZE, CSIZE);
    fclose(objfp);
    if (hextype)
     if (blankcheck())
      if (progdata())
       if (verifycheck())
        printf("programming successful.\n");
   }
   else  quit("Can't find objfile",NULL);
}


void read(int argc, char * argv[])
{
   FILE *objfp;
   char c;
   if (( c = *argv[argc-1]) != '-' && c != '/')
   {
    readdata();
    if ( (objfp = fopen(argv[(argc-1)],"w")) == NULL )
     quit("Can't open objfile",NULL);
    if ( (argc == 4)&&((( c = *argv[2]) == '-' || c == '/' ))) get_option(++argv[2]);
    savehex(objfp, progbuf, confbuf, PSIZE, CSIZE);
    fclose(objfp);
   }
   else  quit("Can't find objfile",NULL);
}

void program(int argc, char * argv[])
{
   FILE *objfp;
   char c;

   if ((c = *argv[argc-1]) != '-' && c != '/')
   {
    if ( (objfp = fopen(argv[(argc-1)],"r")) == NULL )
     quit("Can't open objfile",NULL);
    if ( (argc == 4)&&((( c = *argv[2]) == '-' || c == '/' ))) get_option(++argv[2]);
    hextype=FindFileType(objfp);
    if (hextype) loadhex(objfp, progbuf, confbuf, PSIZE, CSIZE);
    fclose(objfp);
    if (hextype)
     if (progdata())
      printf("programming successful.\n");
   }
   else  quit("Can't find objfile",NULL);
}

short fulltestmode(void)
{
   time_t t;

   printf("\nThis is a computer PIT hardware test, this test takes a few sec.\n");
   printf("\nPlease wait, testing the computer PIT hardware.\n");
   t=time(NULL);
   microdelaytest();        // test microdelay hardware using warp drive.
   t=time(NULL)-t;
   if ( t > 6 || t < 4)
   {
    switch(t)
    {
	case 0:
		printf("\nSorry your computers PIT hardware isn't compliant with the IBM standard.\n");
		printf("Using the programmer is for your own risk and could damage the PIC16C74 !.\n");
		break;
	default:
		if (t <= 50)
		{
		 printf("\nSorry your computers PIT hardware isn't quiet compliant with the IBM standard.\n");
		 printf("The PIC16C74 will be programmed with a puls of %duS instead of 100uS, this\n",(t*100)/5);
		 printf("puls is still within the limits specified by MICROCHIP.\n");
		}
		else
		{
		 printf("\nSorry your computers PIT hardware isn't compliant with the IBM standard. Don't\n");
		 printf("using this computer to programmer the PIC16C74, it will damage the PIC16C74 !.\n");
		}
		break;
     }
    return 0x01;
   }
   else
   {
    printf("\nYour computers PIT hardware is compliant with the IBM standard.\n");
    return 0x00;
   }
}

void MakeCompl(void)
{
   time_t t;
   int c;

   printf("\nThis function will try to make your hardware IBM compliant it's possible\n");
   printf("that your computer will stop responding. If this happens then it's not\n");
   printf("possible to make your hardware IBM compliant !.\n");
   printf("\nAre you sure you want to make your hardware IBM compliant ? (y/n) : ");
   if (yes)
   {
    printf("\nTrying to make the hardware IBM compliant, this takes a few sec.\n");
    disable();              // modify timer 1.
    outportb(0x43,0x76);    // set stuurwoord.
    outportb(0x41,18);      // set LSB tellerwaarde. (15us)
    outportb(0x41,0);       // set MSB tellerwaarde.
    enable();
    t=time(NULL);
    microdelaytest();        // test microdelay hardware using warp drive.
    t=time(NULL)-t;
    if ( t > 6 || t < 4)
     printf("\nSorry it's impossible to make your computers PIT hardware compliant\n");
    else
     printf("\nYour computers PIT hardware is now compliant with the IBM standard.\n");
   }
}

void main(int argc, char *argv[])
{
   int  mode, c;

   if (( argc <= 1)||( argc >= 5 )) usage();
   else
   {
    if ( (mode = *argv[1]) == '-' || mode == '/' )
    {
     setup();
     mode = *(++argv[1]);

nextpic74:
     switch(mode)
     {
      case 'd':
      case 'D':
		start();
		dump();
		printf("\nDo you want to dump another PIC 16C74 (y/n) ? ");
		if (yes) goto nextpic74;
		break;
      case 'b':
      case 'B':
		start();
		blankcheck();
		printf("\nDo you want to blankcheck another PIC 16C74 (y/n) ? ");
		if (yes) goto nextpic74;
		break;
      case 'v':
      case 'V':
		if (argc <= 2) usage();
		start();
		verify(argc, argv);
		printf("\nDo you want to verify another PIC 16C74 (y/n) ? ");
		if (yes) goto nextpic74;
		break;
      case 'a':
      case 'A':
		if (argc <= 2) usage();
		start();
		autoprogram(argc, argv);
		printf("\nDo you want to autoprogram another PIC 16C74 (y/n) ? ");
		if (yes) goto nextpic74;
		break;
      case 'r':
      case 'R':
		if (argc <= 2) usage();
		start();
		read(argc, argv);
		break;
      case 'p':
      case 'P':
		if (argc <= 2) usage();
		start();
		program(argc, argv);
		printf("\nDo you want to program another PIC 16C74 (y/n) ? ");
		if (yes) goto nextpic74;
		break;
      case 't':
      case 'T':
		if (fulltestmode())
		 MakeCompl();
		break;
      case 's':
      case 'S':
		break;
      default:
		usage();
		break;
     }
    }
    else	usage();
   }
}
