/*
   Easy-Downloader V2.1 for ATMEL 89C51/52/55WD
   Copyright(c) 2002 By Wichit Sirichote kswichit@kmitl.ac.th

   The source code was modified from ez52.c for programming the 89C55WD.

   89C55WD uses A14 tied to P3.4 not P3.0 as 89C55.

   The source code was compiled with Dunfiled Micro-C for 8051.

*/

#include <8051io.h>
#include <8051reg.h>
#include <8051bit.h>

#define xon 0x11
#define xoff 0x13

#define Vpp P3.3   // set Vpp = +5V clear Vpp = +12.5V
#define prog P2.7

// map materchip i/o port to the mode selection on ZIF

#define p26 P3.4  
#define p27 P3.5
#define p33 P3.2
#define p36 P3.6
#define p37 P3.7

register int i;
unsigned register char ACCU,temp,blank,chip,VPP;
register char command;
register unsigned int address,chksum;

char title[] = "\n Easy-Downloader V2.1 for ATMEL 89C51/52/55WD"
char prompt[] = "\n >"
char ok[] = "\n ok"

unsigned register int count,bytes,nonblank;

main()
{
  i = 0;
  count = 0;
  serinit(9600);
  getch();
  putstr(title);
  sendprompt();

  for(;;)
    {
    getcommand();
    prompting();
    setcounter();
    erase();
    write();
    read();
    lock();
    printchksum();
    getinfo();
    PGMparameters();
    }
}


getnum()
{
    char s[6];     /*  five characters plus terminator */
    char c;
    int i;
    c = 0;
    for (i = 0; c != 10; i++)
    {
       putch(xon);
       c = putchr(getch());
       s[i] = c;
    }
    s[i] = '\0';    /* put end-of-string to the last byte */
    if (i==0)
    return (-1);
    else
    return (_atoi(s));

}

getcommand()
{

    if ((SCON & 0x01) != 0)
        command = putchr(getch());
    else command = -1;             /* no cammand has entered */
}

getconsole()
{

    if ((SCON & 0x01) != 0)
  command = getchr();
    else command = -1;
}

putok()
{
    putstr(ok);
   /* putch(' ');  */
    sendprompt();
}

sendprompt()
{
    putstr(prompt);
}

prompting()

{
    if (command == '\n')
       {
        putstr(title);
 /* putok(); */
  sendprompt();
       }
}

erase()
{
    if (command == 'e')
    {

    setbit(Vpp)
    setbit(p26)
    clrbit(p27)
    setbit(p33)
    clrbit(p36)
    clrbit(p37)

    delay10us();

    clrbit(Vpp)

    delay(100);       // raise Vpp from +5V to +12.5V

    if(chip == 0x55)
    {
    clrbit(prog)
    asm" NOP";         // actual erase pulse was 200ns-500ns
    setbit(prog)
    }
    else
    {
    clrbit(prog)
    delay(10);           /* 10 ms prog pulse */
    setbit(prog)
    }

    delay(10);
    setbit(Vpp)

    delay(100);
    putok();
    }
}

write()  /* call sequence before write: 's', 't','e','w' */
{
    if (command == 'w')
    {
    setbit(Vpp)
    clrbit(p26)
    setbit(p27)
    setbit(p33)
    setbit(p36)
    setbit(p37)
    setbit(prog)

    delay10us();
    if (VPP == 0xff)  /* skip if VPP != 0xff,i.e., need 5Vpp for programming  */

    clrbit(Vpp)

    delay(100); /* raise programming voltage to 12V */
    chksum = 0;
    for (i = 0; i < count; i++)
    {                 /* use XON & XOFF flow control */
    putch(xon);       /* send XON */
    address = i;
    asm{
       mov P1,address  /* put address to P1 and P2 */
       mov A,address+1
       orl A,#$80
       mov P2,A
       }
    delay10us();
    P0 = getchr();  /* read raw bytes from console */
    putch(xoff);    /* send XOFF */
    chksum += P0;   /* summing chksum */
    pulseProg();

    delay100us();   // use delay instead of checking RDY/BSY
  /*
    asm " nop";
    asm " jnb rdy,*";  /* until ready */
    asm " nop";
    */

    }

    setbit(Vpp)    /* bring Vpp back to 5V */
    delay(100);
    putok();
    }
}

read()    /* read according to number of bytes by 'c' command  */
{  
    unsigned int i;
    if (command=='r')
    {
 /*  temp = getch(); */
  chksum = 0;               /* reset check sum word */

  setbit(Vpp)
  setbit(prog)
  clrbit(p26)
  clrbit(p27)
  clrbit(p33)
  setbit(p36)
  setbit(p37)

     blank = '1';
  for(i = 0; i < count; i++)
	{
    address = i;
    asm{
       mov P1,address
       mov A,address+1
       orl A,#$80
       mov P2,A
       }

   // delay10us();
       asm" mov P0,#$FF";   /* put FF before read back */
       chksum += P0;
       printA(P0);     /*    read in HEX */

   // setbit(p27)

    chkXOFF();
    }
    putok();
    }
}

printA(n)
char n;
{
      ACCU = n;
      ACCU >>= 4;  /* shift right 4 bits */
      putHEX();
      ACCU = n&15;
      putHEX();
   /*   delay(2);     */

}

chkXOFF()     /* use XON and XOFF for controlling flow with host */

{
      if(getconsole() == xoff)
         {
            do
            ;
            while(getconsole() != xon);
         }
}

putHEX()
{
     if (ACCU > 9)
        putch(ACCU+55);
      else putch(ACCU+48);
}

blanktest() /* if all bytes are 0FFH then blank =FF, else blank = 0 */
{
     if (P0 != 0xff)
       { blank = '0';     /* full */
        nonblank++;
       }
}
  
lock()  /* only protection mode 4 can be programmed,i.e., disabled further
program, verify, and external execution */

{
    if (command == 'l')
    {
     clrbit(Vpp)
     setbit(prog)
     setbit(p26)
     setbit(p27)
     setbit(p33)
     setbit(p36)
     setbit(p37)

	delay(100);
	pulseProg();
  delay(5);

     clrbit(p36)
     clrbit(p37)

  delay10us();
  pulseProg();
  delay(5);

     clrbit(p27)
     setbit(p36)
     clrbit(p37)

    delay10us();
    pulseProg();
    delay(5);
	putok();
    }

}

printchksum()
{
    if (command == 'c')
    {
    printf("\n CHKSUM = %04x",~chksum+1);
    putok();
    }
}

signature()  /* read signature ATMEL chip 51/52/55 12V or 5V Vpp */

{

     setbit(Vpp)
     setbit(prog)
     clrbit(p26)
     clrbit(p27)
     clrbit(p33)
     clrbit(p36)
     clrbit(p37)

    address = 0x31; // location stores ATMEL ID for 89C51 and 89C52
    asm{
       mov P1,address
       mov A,address+1
       orl A,#$80
       mov P2,A
       }
    delay10us();
       P0 = 0xFF;  /* put FF before read back */
       chip = P0;

// if chip = 0xFF, try with address 0x100 for 89C55WD also

    if(chip ==0xFF)
    {
    address = 0x100;     // stores ATMEL device ID 0x55 for 89C55WD
    asm{
       mov P1,address
       mov A,address+1
       orl A,#$80
       mov P2,A
       }
    delay10us();
       P0 = 0xFF;  /* put FF before read back */
       chip = P0;
       setbit(p27)
//       printf("\n %04x = %02x ",address,chip);
     }

     address++;
    asm{
       mov P1,address
       mov A,address+1
       orl A,#$80
       mov P2,A
       }
    delay10us();
       P0 = 0xff;   /* put FF before read back */
       VPP = P0; /* save Vpp 12V (FF) or 5V (5) into VPP */
}


testblank()  /* need to call signature function pior calling testblank */
{
      signature();

      switch(chip){
        case (0x51): /* 89C51 */
        bytes = 4096;   /* if chip == 0x51 or 0x61 then bytes = 4kB */
        break;
        case (0x61): /* 89LV51 */
        bytes = 4096;
        break;
        case (0x52): /* 89C52 */
        bytes = 8192;   /* if chip == 0x52 or 0x62 then bytes = 8kB */
        break;
        case (0x62): /* 89LV52 */
        bytes = 8192;
        break;
        case (0x55): /* 89C55 */
        bytes = 20480;  /* if chip == 0x55 or 0x65 then bytes = 20kB */
        break;
        case (0x65): /* 89LV55 */
        bytes = 20480;
        break;
        case 0xff:
        bytes = 0;
        }
  chksum = 0;               /* reset check sum word */
  nonblank = 0;             /* reset nonblank bytes counter */

  setbit(Vpp)
  setbit(prog)
  clrbit(p26)
  clrbit(p27)
  clrbit(p33)
  setbit(p36)
  setbit(p37)

   blank = '1';
  for(i = 0; i < bytes; i++)
	{
    address = i;
    asm{
       mov P1,address
       mov A,address+1
       orl A,#$80
       mov P2,A
       clr p27
       }

  //  delay10us();
       P0 = 0xff;   /* put FF before read back */
       chksum += P0;
       blanktest();
  //     setbit(p27)  /* next address */
	}
}

setcounter()
{
    if (command == 's')
    {
        count = getnum();
        putok();
    }
}

getinfo()
{
    if (command =='g')
    {
        testblank();
        if(chip!=0xff)
        {
        printf("\n found 89C%2x",chip);
          if (VPP==0xff)
        putstr("-12V");
          if (VPP==5)
        putstr("-5V");
        printf("\n nonblank %u bytes",nonblank);
        printf("\n bytes counter %u",count);
        }
        else(putstr("\n Not found 89Cxx"));
        putok();
    }
}

PGMparameters() /* for simple host interfacing */
{
    if (command=='p')
    {
        testblank();
        printf("%x,%u,%u",chip,nonblank,count);
        putok();
    }

}



/*
printhelp()
{
    if (command == '?')
    {
	putstr("\n e  erase");
	putstr("\n rb read BIN");
	putstr("\n rh read HEX");
	putstr("\n w  write");
	putstr("\n l  lock");
     }
	putok();

}
*/

pulseProg()

{
        if(chip ==0x55)
        {
        clrbit(prog)
        asm" nop";
        setbit(prog)
        }
        else
        {
        clrbit(prog)
	asm " nop";
	asm " nop";
	asm " nop";
        setbit(prog)
        }
}

delay10us()
{
    int i;
    for (i=0;i<1;i++);

}

delay100us()
{
   int i;
   for (i=0; i<20;i++);
}
