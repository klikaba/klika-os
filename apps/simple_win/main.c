#include <klika_os.h>
#include <windows.h>
#include <stdlib.h>
#include <gfx.h>


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
    bmp_header_t header;
    uint32_t*  data;
} __attribute__((packed)) bmp_image_t;


message_t msg;
window_t window;

long counter = 0;
char buff[123];

void do_app_draw() {
	sprintf(buff, "Count: %i", counter++);

	gfx_puts(&window.context, 10, 20, 0, WIN_BACKGROUND_COLOR, buff);

	window_send_message_simple(WINDOW_LIB_MESSAGE_PREDRAW, &window);
}


int main() {
	window = window_create(100, 100, 600, 400, "Hello World!");

	DEBUG("Win context: 0x%X", window.context.buffer);
	while(window_get_message(&msg, &window)) { 
		if (msg.message != 0) {
			switch(msg.message) {
				case MESSAGE_MOUSE_MOVE:
					// DEBUG("Message: %i - Mouse move (%i, %i)", msg.message, msg.mouse_x, msg.mouse_y);
					break;
				case MESSAGE_MOUSE_CLICK:
					// DEBUG("Message: %i - Mouse click (%i, %i) : %i", msg.message, msg.mouse_x, msg.mouse_y, msg.mouse_buttons);
					break;
				case MESSAGE_KEY_PRESS:
					// DEBUG("Message: %i - Key press (%c)", msg.message, msg.key & 0xFF);
					break;
				case MESSAGE_KEY_RELEASE:
					// DEBUG("Message: %i - Key release (%c)", msg.message, msg.key & 0xFF);
					break;
				case WINDOW_LIB_MESSAGE_DRAW:
					do_app_draw();
					break;
				default:
					// DEBUG("Got message : %i", msg.message);
					break;
			}
			window_dispatch_message(&msg, &window);
		}
	}
	return 0;
}