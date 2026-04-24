#ifndef __UART5_H
#define __UART5_H

#include "sys.h"


#define UART5_print 1
void uart5_init(u32 bound);
void uart5_WriteBuf(uint8_t *buf, uint8_t len);

#endif


