#pragma once

#include <stdint.h>
#include <windows/window.h>

typedef struct {
    uint16_t type;              // Magic identifier: 0x4d42
    uint32_t size;              // File size in bytes
    uint16_t reserved1;         // Not used
    uint16_t reserved2;         // Not used
    uint32_t offset;            // Offset to image data in bytes from beginning of file
    uint32_t dib_header_size;   // DIB Header size in bytes
    int32_t  width_px;          // Width of the image
    int32_t  height_px;         // Height of image
    uint16_t num_planes;        // Number of color planes
    uint16_t bits_per_pixel;    // Bits per pixel
    uint32_t compression;       // Compression type
    uint32_t image_size_bytes;  // Image size in bytes
    int32_t  x_resolution_ppm;  // Pixels per meter
    int32_t  y_resolution_ppm;  // Pixels per meter
    uint32_t num_colors;        // Number of colors
    uint32_t important_colors;  // Important colors
} __attribute__((packed)) bmp_header_t;

typedef struct {
    uint8_t *buffer;
    bmp_header_t *header;
    uint32_t *data;
} bmp_image_t;

void bmp_from_file(char *filename, bmp_image_t *bmp_out);
void bmp_close(bmp_image_t *bmp_image);
void bmp_blit(context_t* context, bmp_image_t *bmp, int x, int y);
void bmp_blit_clipped(context_t *context, bmp_image_t *bmp, int x, int y, int clip_x, int clip_y, int clip_w, int clip_h);