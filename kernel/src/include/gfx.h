#ifndef __GFX_H
#define __GFX_H

#include <stdint.h>
#include <vesa.h>

// void gfx_putpixel(video_info_t* video, int x, int y, uint32_t color);
// void gfx_hline(video_info_t* video, int x1, int x2, int y, uint32_t color);
// void gfx_fillrect(video_info_t* video, int x1, int y1, int x2, int y2, uint32_t color);
// void gfx_putchar(video_info_t* video, int x, int y, uint32_t fgcolor, uint32_t bgcolor, const char c);
// void gfx_puts(video_info_t* video, int x, int y, uint32_t fgcolor, uint32_t bgcolor, const char *c);
void gfx_blit(video_info_t* video, int x, int y, int width, int height, uint32_t* src);

#endif 