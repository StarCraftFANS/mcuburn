#include <proto.h>
#include <stdio.h>
#include <fcntl.h>
#include <mcutarget.h>
#include <heximage.h>
char sbuf[MAXBUFLEN];
int sendcommand(int fd,char  cmd,void *databuf)
{
        int fd1=-1;
        int i=0;
        char lenb[7];
          mcudev_t *mcuinfo=(mcudev_t *)databuf;
        char byte=0;
        memset(lenb,0,6);
        strncpy(lenb,"732\r\n",6);
        sbuf[0]=0;
     switch(cmd)
     {
        case 'g':
        case 'i':
        case 'G':
        case 'I':        
                byte='g';
                writebuffer(fd,&byte,1);
                usleep(300);
                readbuffer(fd,sbuf,MAXBUFLEN);
                break;
        case 'r':
        case 'R':
                byte='r';
                writebuffer(fd,&byte,1);
                byte='h';
                writebuffer(fd,&byte,1);
                usleep(30);
                readbuffer(fd,sbuf,MAXBUFLEN);
                break;
        case 's':
        case 'S':
                byte='s';
                writebuffer(fd,&byte,1);
                usleep(20);
                for(i=0;i<5;i++)
                {
                writebuffer(fd,&lenb[i],1);
                usleep(20);
                }
                usleep(300);
                    fprintf(stderr,"The code has been released\n");
                break;
        case 't':
        case 'T':
                byte='t';
                writebuffer(fd,&byte,1);
                usleep(100);
                break;
        case 'e':      
        case 'E':      
                byte='e';
                writebuffer(fd,&byte,1);
                usleep(260000L);
                break;
        case 'w':
        case 'W':
                if(mcuinfo == NULL) break;
                fd1=fd;
                byte='s';
                writebuffer(fd1,&byte,1);
                usleep(20);
                for(i=0;i<5;i++)
                {
                writebuffer(fd1,&lenb[i],1);
                usleep(30);
                }
		printf("writing file\n\r");
                fprintf(stderr,"The code has been released\n");
                byte='t';
                writebuffer(fd1,&byte,1);
                usleep(10);
                fprintf(stderr,"Writting counters\n");    
                byte='e';
                writebuffer(fd1,&byte,1);
                usleep(250000L);
                fprintf(stderr,"Erased\n");
                byte='w';
                writebuffer(fd1,&byte,1);
                usleep(40);
                usleep(100000L);
                byte=0;
                sendihx(fd1,&mcuinfo->image->Filename[0]);
                usleep(100000L);
                break;
        default:
                break;
     }
     
}

void 
processdata(char *disbuf,int len)
{
   
         char *foundstr=NULL; 
        char byte=0;
        int foundprompt=0,foundok=0;
int count=0;
int lencalc=0;
char *serbuf=&sbuf[0];
if(serbuf == NULL || disbuf == NULL)
        return;
lencalc=strlen(serbuf);
byte=serbuf[0];
serbuf[0]=' ';
count=lencalc-1;
while(count >= 0)
{
if(serbuf[count] == '>' && !foundprompt ) 
{
        char *foundstr=NULL;
        serbuf[count]=0;
        foundprompt=1;
}
if(!foundok && count > 0  && serbuf[count-1] == 'o' && serbuf[count] == 'k' )
{
serbuf[count-1]=0;
serbuf[count]=0;
foundok=1;
}
if(serbuf[count] == '\n' || serbuf[count] == '\r')
serbuf[count]=' ';
count--;
}

switch(byte)
{
        case 'g':
                 //if( ( foundstr=strstr(serbuf,"found") )!= NULL)
                   //     foundstr[16]=0;
                 break;
        default:
                    break;
}
memset(disbuf,0,len);
memcpy(disbuf,serbuf,strlen(serbuf)< len?strlen(serbuf):len);
}

