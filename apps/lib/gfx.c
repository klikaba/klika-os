#include <klikaos.h>
#include <windows.h>
#include <gfx.h>
#include <string.h>
#include <font_8x8.h>
#include <stdlib.h>

void gfx_putpixel(context_t* context, int x, int y, uint32_t color) {
	CLIP_XY(x, y);
	CONTEXT_32[FIRST_PIXEL(x, y)] = color;
}

void gfx_hline(context_t* context, int x1, int x2, int y, uint32_t color) {
	CLIP_X(x1);
	CLIP_X(x2);
	CLIP_Y(y);
	for (int x=x1; x<=x2; x++) {
		CONTEXT_32[FIRST_PIXEL(x, y)] = color;
	}
}

void gfx_vline(context_t* context, int x, int y1, int y2, uint32_t color) {
	CLIP_X(x);
	CLIP_Y(y1);
	CLIP_Y(y2);
	for (int y=y1; y<=y2; y++) {
		CONTEXT_32[FIRST_PIXEL(x, y)] = color;
	}
}

void gfx_rect(context_t* context, int x1, int y1, int x2, int y2, uint32_t color) {
	gfx_hline(context, x1, x2, y1, color);
	gfx_hline(context, x1, x2, y2, color);
	gfx_vline(context, x1, y1, y2, color);
	gfx_vline(context, x2, y1, y2, color);
}

void gfx_fillrect(context_t* context, int x1, int y1, int x2, int y2, uint32_t color) {
	for (int i=y1; i<=y2; i++) {
		gfx_hline(context, x1, x2, i, color);
	}
}

void gfx_fillrect_dot(context_t* context, int x1, int y1, int x2, int y2, uint32_t color) {
	for (; x1 <= x2; x1++) {
		for (; y1 <= y2; y1++) {
			if ((x1 % 2 == 0) || (y1 % 2 == 0)) {
				CONTEXT_32[FIRST_PIXEL(x1, y1)] = color;
			}
		}
	}
}

void gfx_putchar(context_t* context, int x, int y, uint32_t fgcolor, uint32_t bgcolor, const char c) {
  uint8_t i, j;
  for(i = 0; i < 8; i++) {
    for(j = 0; j < 8; j++) {
    	if (x+i >= 0 && x+1 < context->width &&
    		  y+j >= 0 && y+1 < context->height) {
  			CONTEXT_32[FIRST_PIXEL(x+i, y+j)] = ((font8x8_basic[c & 0x7F][j] >> i ) & 1) ? fgcolor : bgcolor;
  		}
    }
  }
}

void gfx_putchar_trans(context_t* context, int x, int y, uint32_t color, const char c) {
  uint8_t i, j;
  for(i = 0; i < 8; i++) {
    for(j = 0; j < 8; j++) {
    	if (x+i >= 0 && x+1 < context->width &&
    		  y+j >= 0 && y+1 < context->height) {
    		bool draw = ((font8x8_basic[c & 0x7F][j] >> i ) & 1);
    		if (draw) {
  				CONTEXT_32[FIRST_PIXEL(x+i, y+j)] = color;
  			}
  		}
    }
  }
}

void gfx_puts(context_t* context, int x, int y, uint32_t fgcolor, uint32_t bgcolor, const char *c) {
	while(*c) {
		gfx_putchar(context, x, y, fgcolor, bgcolor, *c++);
		x += 8;
	}
}

void gfx_puts_trans(context_t* context, int x, int y, uint32_t color, const char *c) {
	while(*c) {
		gfx_putchar_trans(context, x, y, color, *c++);
		x += 8;
	}
}

// Optimize + CLIP
void gfx_blit(context_t* context, int x, int y, int width, int height, uint32_t* src) {
	for (int i=0; i<height; i++) {
		for (int j=0; j<width; j++) {
			CONTEXT_32[FIRST_PIXEL(x+j, y+i)] = *(src + (j + i * width));
		}
	}
}

void gfx_blit_trans(context_t* context, int x, int y, int width, int height, uint32_t* src, uint32_t color) {
	for (int i=0; i<height; i++) {
		for (int j=0; j<width; j++) {
			uint32_t col = *(src + (j + i * width));
			if (col != color) {
				CONTEXT_32[FIRST_PIXEL(x+j, y+i)] = col;
			}
		}
	}
}

void gfx_rect_width(context_t *context, int x1, int y1, int x2, int y2, uint32_t color, int width) {
	for (int i=0; i<width; i++) {
		gfx_hline(context, x1, x2, y1+i, color);
		gfx_hline(context, x1, x2, y2-i, color);
		gfx_vline(context, x1+i, y1, y2, color);
		gfx_vline(context, x2-i, y1, y2, color);
	}
}

void gfx_draw_shadowed_box(context_t *context, int x1, int y1, int x2, int y2, uint32_t color, uint32_t bg_color) {
	gfx_rect_width(context, x1, y1, x2-2, y2-2, color, WINDOW_FRAME_WIDTH);
	gfx_rect_width(context, x1+2, y1+2, x2, y2, color, WINDOW_FRAME_WIDTH);
	gfx_fillrect(context, x1+2, y1+2, x2-4, y2-4, bg_color);
}

void gfx_line(context_t *context, int x1, int y1, int x2, int y2, int c) {
 int x,y,dx,dy,dx1,dy1,px,py,xe,ye,i;
 dx=x2-x1;
 dy=y2-y1;
 dx1=abs(dx);
 dy1=abs(dy);
 px=2*dy1-dx1;
 py=2*dx1-dy1;
 if(dy1<=dx1)
 {
  if(dx>=0)
  {
   x=x1;
   y=y1;
   xe=x2;
  }
  else
  {
   x=x2;
   y=y2;
   xe=x1;
  }
  gfx_putpixel(context,x,y,c);
  for(i=0;x<xe;i++)
  {
   x=x+1;
   if(px<0)
   {
    px=px+2*dy1;
   }
   else
   {
    if((dx<0 && dy<0) || (dx>0 && dy>0))
    {
     y=y+1;
    }
    else
    {
     y=y-1;
    }
    px=px+2*(dy1-dx1);
   }
   gfx_putpixel(context,x,y,c);
  }
 }
 else
 {
  if(dy>=0)
  {
   x=x1;
   y=y1;
   ye=y2;
  }
  else
  {
   x=x2;
   y=y2;
   ye=y1;
  }
  gfx_putpixel(context,x,y,c);
  for(i=0;y<ye;i++)
  {
   y=y+1;
   if(py<=0)
   {
    py=py+2*dx1;
   }
   else
   {
    if((dx<0 && dy<0) || (dx>0 && dy>0))
    {
     x=x+1;
    }
    else
    {
     x=x-1;
    }
    py=py+2*(dx1-dy1);
   }
   gfx_putpixel(context,x,y,c);
  }
 }
}
