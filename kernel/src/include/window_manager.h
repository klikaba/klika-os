#ifndef __WINDOW_MANAGER_H
#define __WINDOW_MANAGER_H

#include <messaging.h>
#include <process.h>
#include <stdbool.h>
#include <stdint.h>
#include <mouse.h>

#define MAX_WINDOW_NAME_LENGTH 128
#define MAX_MESSAGE_QUEUE_LENGTH 128

#define WINDOW_ATTR_TRANSP 0x01
#define WINDOW_ATTR_BOTTOM 0x02
#define WINDOW_ATTR_NO_DRAG 0x04

typedef struct {
	uint32_t* buffer;
	int width;
	int height;
	int bpp;
} context_t;

typedef struct window_struct {
	uint32_t handle;
	int x;
	int y;
	int z;
	int width;
	int height;
	uint32_t attributes;
	int message_queue_index;
	task_t* parent_task;
	message_t message_queue[MAX_MESSAGE_QUEUE_LENGTH];
	context_t context;
} window_t;

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


#define MAX_WINDOW_COUNT 64

extern window_t* window_list[MAX_WINDOW_COUNT];

window_t* window_create(int x, int y, int width, int height, uint32_t attributes);
void window_close(window_t *window);
void window_present_context(window_t* win, context_t* context);

void window_handle_mouse();
void window_need_redraw();

void window_add_message(window_t *win, message_t *msg);
void window_add_message_to_focused(message_t *msg);
bool window_pop_message(window_t* win, message_t* msg_out);

window_t* window_find(uint32_t handle);
void init_kernel_window_manager();
void window_manager_redraw();

#endif
