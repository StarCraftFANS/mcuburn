#ifndef PROTO_H
#define PROTO_H
#define MAXBUFLEN 512
#define TOTALRTIME    1000000L
#define RWTIME        30000L 
#define PROMPTBYTE    '>'
int sendcommand(int fd,char cmd,void *data);
void processdata(char *buf,int len);
#endif
