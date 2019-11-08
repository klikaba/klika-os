#ifndef __SERIAL_H
#define __SERIAL_H

void serial_init(void);
void serial_write_com(int, unsigned char);
void init_kernel_serial();

#endif