#ifndef MCUTARGET_H
#define MCUTARGET_H
#include <comm.h>
#include <heximage.h>
typedef enum mcutarget
{
TARGET_ATMEL_89c5x,
TARGET_ATMEL_89c5xxx,
TARGET_ADI_ADUC
}mcutarget_t;

typedef struct mcudev
     {
        char *name;     
        unsigned long id;
        unsigned long product;
        mcutarget_t target;
        unsigned long ramstart;
        unsigned long rambase;
        unsigned long flashmask;
        unsigned long *inacess_mem[2];
        unsigned long maxflashsize;
        unsigned long maxramsize;
        unsigned long flashsectors;
        unsigned long percopysize;
        commtype_t *pcom;
        image_t    *image;        
}mcudev_t;
#endif /*MCUTARGET_H */

