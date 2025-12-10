#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H

#include "../drivers/types.h"

void outb(unsigned short port, unsigned char data);
unsigned char inb(unsigned short port);

#endif
