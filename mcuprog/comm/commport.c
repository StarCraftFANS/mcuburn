#include <commport.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <heximage.h>
#include <sys/time.h>
#include <time.h>
struct termios options;

int
serial_open (const char *device)
{
  int fd;
  if (device == NULL)
    return -1;

  fd = open (device, O_RDWR |O_NOCTTY | O_NONBLOCK);
  if (fd < 0)
    return fd;
  tcflush(fd,TCOFLUSH);
  tcflush(fd,TCIFLUSH);
  fcntl (fd,F_SETFL,fcntl(fd,F_GETFL)& ~O_NONBLOCK);
  fcntl (fd, F_SETFL, O_NONBLOCK);
  return fd;
}

int
serial_setup (int fd)
{
        int mask=0;
if(tcgetattr(fd,&options)==-1)
return -1;
     bzero(&options,sizeof(options));
     cfmakeraw(&options);
     options.c_cflag|=CS8|CLOCAL|CREAD|B9600;
    options.c_iflag = IGNPAR | IGNBRK | IXON | IXOFF;
    options.c_cc[VTIME]    = 1;   /* inter-character timer used */
    options.c_cc[VMIN]     = 0;   /* blocking read until 0 chars received */
    cfsetispeed(&options,B9600);
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd, TCSANOW, &options);
     
     /*
     mask=TIOCM_RTS;
     mask|=TIOCM_DTR;
    if(ioctl(fd,TIOCMBIS,&mask)==-1)
    return -1;            
    */
     return 0;
}
int
serial_close (int fd)
{
  if (fd < 0)
    return fd;
  return close (fd);
}

int 
fd_wait(int fd,unsigned long wtusec,unsigned long wtsec)
{
	fd_set fds;
	int ret;
	struct timeval tv;

	FD_ZERO(&fds);
	FD_SET(fd,&fds);
	
	tv.tv_sec=wtsec;    /* timeout after 1 sec */
	tv.tv_usec=wtusec;
	ret=select(fd+1,&fds,NULL,NULL,&tv);
	if(ret==0)
	{
		return(-1); /* received nothing, bad */
	}
	else if(ret!=1)
	{
		return(-1);
	}
     return 1;
}


int readbyte(int fd,char *byte)
{
	if(read(fd,byte,1)!=1)
	{
		return(-1);		
	}
     else
	return(1);
}

int 
readbuffer(int fd,char *buf,int len)
{
        unsigned int counter=0;
        char byte;
        int slen=0;
        int ilen,jlen;
memset(buf,0,len);
while(slen < len)
{
     if(read(fd,&byte,1) == 1)
     {
        buf[slen]=byte;
        slen++;
     }
     if(slen >= 0)
        if((buf[slen-1] == 'o' && buf[slen] == 'k'))
          {
                buf[slen-1]=0;
                break;
          }
         else if( byte == '>')
          {
                 buf[slen]=0;
                 break;
          }
                 
     counter++;
     usleep(30);      
     if(counter == 0xfffffffe) 
             break;
}
}

int 
writebuffer(int fd,char *buf,int len)
{
          int exceeds=0;       
       unsigned char byte=0;
       int index=0;
       struct timeval freshtime,senttime;
       struct timezone tzval;
      gettimeofday(&freshtime,&tzval);
      gettimeofday(&senttime,&tzval);
     while(index < len)
     {
     byte=buf[index];      
     if(exceeds )
     {
         if(write(fd,&byte,1)==1)
          {
             exceeds=0;
        gettimeofday(&senttime,&tzval);
        fprintf(stderr,"%x senttime %d",byte,senttime.tv_sec);
          index++;
          usleep(30);
          }
     }
          gettimeofday(&freshtime,&tzval);
          if( (freshtime.tv_sec-senttime.tv_sec) > 400 ) 
                  break;
          if( !exceeds &&
               (freshtime.tv_sec-senttime.tv_sec) > 2 ) 
                  exceeds=1;
     }

}
int sendfile(int fd,char *argv)
{
sendihx(fd,argv);
}
