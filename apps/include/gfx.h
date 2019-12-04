#ifndef __GFX_H
#define __GFX_H

#define CONTEXT_32 ((uint32_t*)context->buffer)
#define FIRST_PIXEL(x, y) ((x) + ((y) * (context->width)))

// Be carefull with this macro :)
#define CLIP_X(x) x = x < 0 ? 0 : x; x = x < context->width ? x : context->width-1
#define CLIP_Y(y) y = y < 0 ? 0 : y; y = y < context->height ? y : context->height-1
#define CLIP_XY(x, y) CLIP_X(x); CLIP_Y(y)

void gfx_putpixel(context_t* context, int x, int y, uint32_t color);

void gfx_hline(context_t* context, int x1, int x2, int y, uint32_t color);
void gfx_vline(context_t* context, int x, int y1, int y2, uint32_t color);
void gfx_line(context_t *context, int x1, int y1, int x2, int y2, int color);

void gfx_rect(context_t* context, int x1, int y1, int x2, int y2, uint32_t color);
void gfx_fillrect(context_t* context, int x1, int y1, int x2, int y2, uint32_t color);
void gfx_fillrect_dot(context_t* context, int x1, int y1, int x2, int y2, uint32_t color);
void gfx_rect_width(context_t *context, int x1, int y1, int x2, int y2, uint32_t color, int width);
void gfx_draw_shadowed_box(context_t *context, int x1, int y1, int x2, int y2, uint32_t color, uint32_t bg_color);

void gfx_putchar(context_t* context, int x, int y, uint32_t fgcolor, uint32_t bgcolor, const char c);
void gfx_putchar_trans(context_t* context, int x, int y, uint32_t color, const char c);
void gfx_puts(context_t* context, int x, int y, uint32_t fgcolor, uint32_t bgcolor, const char *c);
void gfx_puts_trans(context_t* context, int x, int y, uint32_t color, const char *c);

void gfx_blit(context_t* context, int x, int y, int width, int height, uint32_t* src);
void gfx_blit_trans(context_t* context, int x, int y, int width, int height, uint32_t* src, uint32_t color);


#endif

