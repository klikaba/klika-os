#ifndef __MOUSE_H
#define __MOUSE_H

#include <stdint.h>

#define MOUSE_LEFT    1
#define MOUSE_RIGHT   2
#define MOUSE_MIDLE   4

struct mouse_packet_struct {
  uint8_t flags;
  int32_t dx;
  int32_t dy;
} __attribute__ ((packed));

typedef struct mouse_packet_struct mouse_packet_t;

void init_kernel_mouse(void);

#endif