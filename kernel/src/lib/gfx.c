#include <gfx.h>
#include <vesa.h>
#include <string.h>
#include "./font_8x8.h"

#define FRAMEBUFFER_32 ((uint32_t*)video->addr)

#define FIRST_PIXEL(x, y) ((x) + ((y) * (video->width)))

// Be carefull with this macro :)
#define CLIP_X(x) x = x < 0 ? 0 : x; x = x < video->width ? x : video->width-1
#define CLIP_Y(y) y = y < 0 ? 0 : y; y = y < video->height ? y : video->height-1
#define CLIP_XY(x, y) CLIP_X(x); CLIP_Y(y)

void gfx_putpixel(video_info_t* video, int x, int y, uint32_t color) {
	CLIP_XY(x, y);
	FRAMEBUFFER_32[FIRST_PIXEL(x, y)] = color;
}

void gfx_hline(video_info_t* video, int x1, int x2, int y, uint32_t color) {
	CLIP_X(x1);
	CLIP_X(x2);
	CLIP_Y(y);
	for (int x=x1; x<=x2; x++) {
		FRAMEBUFFER_32[FIRST_PIXEL(x, y)] = color;
	}
}

void gfx_fillrect(video_info_t* video, int x1, int y1, int x2, int y2, uint32_t color) {
	for (int i=y1; i<=y2; i++) {
		gfx_hline(video, x1, x2, i, color);
	}
}

void gfx_putchar(video_info_t* video, int x, int y, uint32_t fgcolor, uint32_t bgcolor, const char c) {
  uint8_t i, j;
  for(i = 0; i < 8; i++) {
    for(j = 0; j < 8; j++) {
    	if (x+i >= 0 && x+1 < video->width &&
    		  y+j >= 0 && y+1 < video->height) {
  			FRAMEBUFFER_32[FIRST_PIXEL(x+i, y+j)] = ((font8x8_basic[c & 0x7F][j] >> i ) & 1) ? fgcolor : bgcolor;
  		}
    }
  }
}

// Optimize + CLIP
void gfx_blit(video_info_t* video, int x, int y, int width, int height, uint32_t* src) {
	for (int i=0; i<height; i++) {
		for (int j=0; j<width; j++) {
			FRAMEBUFFER_32[FIRST_PIXEL(x+j, y+i)] = *(src + (j + i * width));
		}
	}
}

// TODO: this is silly : use transparent map or alpha channel
void gfx_blit_transparent(video_info_t* video, int x, int y, int width, int height, uint32_t* src, uint32_t trans_color) {
	for (int i=0; i<height; i++) {
		for (int j=0; j<width; j++) {
			uint32_t c = *(src + (j + i * width));
			if (c != trans_color) {
				FRAMEBUFFER_32[FIRST_PIXEL(x+j, y+i)] = *(src + (j + i * width));
			}
		}
	}
}

void gfx_puts(video_info_t* video, int x, int y, uint32_t fgcolor, uint32_t bgcolor, const char *c) {
	while(*c) {
		gfx_putchar(video, x, y, fgcolor, bgcolor, *c++);
		x += 8;
	}
}
