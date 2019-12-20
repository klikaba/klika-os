#include <window_manager.h>
#include <mmu_heap.h>
#include <process.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <kernel.h>
#include <assert.h>
#include <mouse.h>
#include <timer.h>
#include <vesa.h>
#include <gfx.h>
#include <isr.h>

#define WINDOW_BAR_HEIGHT  44

#define WIN_SORT_VAL(win) (win == NULL ? 1000000000 : win->z) 

window_t* window_list[MAX_WINDOW_COUNT];
window_t* focused_window;
video_info_t buffer_video_info;
uint64_t __last_update_tick = 0;
uint16_t __window_handle = 1000; // 1000 just magic number to start from (0 means no window)
int __window_count = 0;

bool __window_need_redraw = true;

window_t* window_to_drag = NULL;
int __old_mouse_x, __old_mouse_y;

short mouse_icon[] =  {
	1,0,0,0,0,0,0,0,0,0,0,
	1,1,0,0,0,0,0,0,0,0,0,
	1,2,1,0,0,0,0,0,0,0,0,
	1,2,2,1,0,0,0,0,0,0,0,
	1,2,2,2,1,0,0,0,0,0,0,
	1,2,2,2,2,1,0,0,0,0,0,
	1,2,2,2,2,2,1,0,0,0,0,
	1,2,2,2,2,2,2,1,0,0,0,
	1,2,2,2,2,2,2,2,1,0,0,
	1,2,2,2,2,2,2,2,2,1,0,
	1,2,2,2,2,1,1,1,1,1,1,
	1,2,2,2,1,0,0,0,0,0,0,
	1,2,2,1,0,0,0,0,0,0,0,
	1,2,1,0,0,0,0,0,0,0,0,
	1,1,0,0,0,0,0,0,0,0,0,
	1,0,0,0,0,0,0,0,0,0,0,
};

uint32_t mouse_color_mapping[] = {0, 0xFFFFFFFF, 0};


void bmp_from_file(char *filename, bmp_image_t *bmp_out) {
	DEBUG("BMP FILE OPENING ...");
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		return;
	}
	DEBUG("BMP FILE OPENED");
	uint32_t size = fsize(file);


	void *buffer = malloc(size + 512);
	DEBUG("BMP ALLOCATED ...");

	fread(buffer, size, 1, file);
	DEBUG("BMP READ ...");

	bmp_out->buffer = buffer;
	bmp_out->header = (bmp_header_t*)bmp_out->buffer;
	bmp_out->data = (uint32_t*)(((uint8_t*)buffer) + bmp_out->header->offset);
	// Usually header is negative (means that image is stored : top - to -bottom)
	bmp_out->header->height_px = abs(bmp_out->header->height_px);
}

void bmp_close(bmp_image_t *bmp_image) {
	free(bmp_image->buffer);
}

static int find_window_index(window_t *window) {
	for (int i=0; i<MAX_WINDOW_COUNT; i++) {
		if (window_list[i] == window) {
			return i;
		}
	}
	return -1;
}

static bool add_window(window_t *window) {
	for(int i=0; i<MAX_WINDOW_COUNT; i++) {
		if (window_list[i] == NULL) {
			window_list[i] = window;
			return true;
		}
	}
	return false;
}

static int find_max_z() {
	int last_z = 0;
	for (int i=0; i<MAX_WINDOW_COUNT; i++) {
		if (window_list[i] != NULL && last_z < window_list[i]->z) {
			last_z = window_list[i]->z;
		}
	}
	return last_z;
}

void window_sort_windows() {
	uint32_t max_idx;
	int n = MAX_WINDOW_COUNT;
  for (int i = 0; i < n-1; i++) { 
    max_idx = i; 
    for (int j = i+1; j < n; j++) {
      if (WIN_SORT_VAL(window_list[j]) < WIN_SORT_VAL(window_list[max_idx])) {
        max_idx = j; 
      }
    }

    window_t* tmp = window_list[max_idx];
    window_list[max_idx] = window_list[i];
    window_list[i] = tmp;
  } 

  // find count
  int count = 0;
  for (; count < MAX_WINDOW_COUNT; count++) {
  	if (window_list[count] == NULL) break;
  }
  __window_count = count;
}


bool window_point_inside(window_t* win, int x, int y) {
	return x >= win->x && x <= win->x + win->width &&
				 y >= win->y && y <= win->y + win->height;
}


bool window_point_inside_bar(window_t* win, int x, int y) {
	return x >= win->x && x <= win->x + win->width &&
				 y >= win->y && y <= win->y + WINDOW_BAR_HEIGHT;
}

int window_find_xy(int x, int y) {
	if (__window_count == 0) {
		return -1;
	}
	for (int i=__window_count-1; i>=0; i--) {
		if (window_list[i] == NULL) { // is this IF needed? 
			break;
		}
		if (window_point_inside(window_list[i], x, y)) {
			return i;
		}
	}
	return -1;
}

window_t* window_find(uint32_t handle) {
	if (__window_count == 0) {
		return NULL;
	}
	// Biggest chance that searched handle belongs to topmost window
	for (int i=__window_count-1; i>=0; i--) {
		if (window_list[i]->handle == handle) {
			return window_list[i];
		}
	}
	return NULL;
}


void window_present_context(window_t* win  __UNUSED__, context_t* context) {
	assert(context->bpp == 32);
	fast_memcpy(win->context.buffer, context->buffer, context->width * context->height * 4);
  window_need_redraw();
}

window_t* window_create(int x, int y, int width, int height, uint32_t attributes) {
	window_t* new_win = (window_t*)malloc(sizeof(window_t));
	new_win->x = x;
	new_win->y = y;
	new_win->z = attributes & WINDOW_ATTR_BOTTOM ? 0 : find_max_z() + 1;
	new_win->width = width;
	new_win->height = height;
	new_win->attributes = attributes;
	new_win->handle = __window_handle++;

	// Message queue
	new_win->message_queue_index = 0;
	new_win->parent_task = task_list_current;
	memset(new_win->message_queue, 0, sizeof(message_t)*MAX_MESSAGE_QUEUE_LENGTH);

	// Context setup
	new_win->context.width = width;
	new_win->context.height = height;
	new_win->context.bpp = 32;
	new_win->context.buffer = (uint32_t*)malloc(width * height * (32 / 8));

	// Add to list of windows : for now array of pointer to win
	assert(add_window(new_win));
	task_list_current->window = new_win;
	focused_window = new_win;

	window_sort_windows();
	window_need_redraw();
	return new_win;
}

void window_close(window_t *window) {
	int win_idx = find_window_index(window);
	free(window->context.buffer);
	window_list[win_idx] = NULL;
	free(window);
	window_sort_windows();
	if (window == focused_window) {
		focused_window = __window_count == 0 ? NULL : window_list[__window_count - 1];
	}
	window_need_redraw();
}

void window_draw(window_t *win, bool is_top __UNUSED__) {
	// Prsent window buffer 
	gfx_blit(&buffer_video_info, win->x, win->y, win->width, win->height, win->context.buffer);
}

void window_draw_all() {
	for (int i=0; i<MAX_WINDOW_COUNT; i++) {
		// List is sorted : first NULL means rest of them are NULL
		if (window_list[i] == NULL) {
			break;
		}
		bool is_top = i == __window_count - 1;
		window_draw(window_list[i], is_top);
	}
}

// TODO: Optimise - move to gfx_blit
void window_draw_mouse() {
	short* buf = mouse_icon;
	for (int i=0; i<16; i++) {
		for (int j=0; j<11; j++) {
			if (*buf) {
				uint32_t color = mouse_color_mapping[*buf];
				if (mouse_buttons & MOUSE_LEFT_CLICK) {
					color ^= 0xFFFFFF;
				}
	    	if (mouse_x + j >= 0 && mouse_x + j < buffer_video_info.width &&
    		  mouse_y + i >= 0 && mouse_y + i < buffer_video_info.height) {
					((uint32_t*)buffer_video_info.addr)[(mouse_y + i) * buffer_video_info.width + mouse_x + j] = color;
				}
			}
			buf++;
		}
	}
}

void present_video_buffer() {
	#if WAIT_FOR_VERTICAL_RETRACE
  	while ((inpb(0x3DA) & 0x08));
  	while (!(inpb(0x3DA) & 0x08));
 	#endif	
  fast_memcpy((unsigned char*)vesa_video_info.addr, (unsigned char*)buffer_video_info.addr, VIDEO_INFO_MEM_SIZE(buffer_video_info));
}

void window_bring_to_front(int win_idx) {
	if (__window_count <= 1) {
		return;
	}
	assert(win_idx < __window_count);
	if (win_idx == __window_count-1) {
		return;
	}

	window_t* old_top_win = window_list[__window_count - 1];
	window_t* new_top_win = window_list[win_idx];

	// Exchange Z
	int tmpz = new_top_win->z;
	new_top_win->z = old_top_win->z;
	old_top_win->z = tmpz;

	// Sort
	window_t* tmp = window_list[__window_count - 1];
	window_list[__window_count - 1] = window_list[win_idx];
	window_list[win_idx] = tmp;
	window_need_redraw();
}

// HOOLI SHAJT - simplify this poop
void window_handle_mouse() {
	if (mouse_buttons & MOUSE_LEFT_CLICK) {
		if (window_to_drag != NULL) {
			window_to_drag->x += mouse_x - __old_mouse_x;
			window_to_drag->y += mouse_y - __old_mouse_y;
			if (__old_mouse_x != mouse_x || __old_mouse_y != mouse_y) {
				message_t msg;
				msg.message = MESSAGE_WINDOW_DRAG;
				msg.x = window_to_drag->x;
				msg.y = window_to_drag->y;
				window_add_message_to_focused(&msg);
			}
			__old_mouse_x = mouse_x;
			__old_mouse_y = mouse_y;
		}	
		else {
			// Find win under the mouse and bring it to top
			int idx = window_find_xy(mouse_x, mouse_y);
			if (idx != -1) {
				window_t* win = window_list[idx];
				focused_window = win;
				if (!(focused_window->attributes & WINDOW_ATTR_BOTTOM)) {
				        window_bring_to_front(idx);
				}
				// Check for dragging
				if (window_point_inside_bar(win, mouse_x, mouse_y) && !(win->attributes & WINDOW_ATTR_NO_DRAG)) {
					window_to_drag = win;
					__old_mouse_x = mouse_x;
					__old_mouse_y = mouse_y;
				} else {
					window_to_drag = NULL; // DIRTY
				}
			} else {
				window_to_drag = NULL; // DIRTY
			}
		}
	} else {
		window_to_drag = NULL; // DIRTY
	}
	window_need_redraw();
}

// bmp_image_t* bmp_read_from_memory(void* bmp_file) {
// 	bmp_image_t* bmp = (bmp_image_t*)bmp_file;
// 	DEBUG("BMP[wallpaper]: type  = %i\n", bmp->header.type);
// 	DEBUG("BMP[wallpaper]: size  = %i\n", bmp->header.size);
// 	DEBUG("BMP[wallpaper]: reserved1 = %i\n", bmp->header.reserved1);
// 	DEBUG("BMP[wallpaper]: reserved2 = %i\n", bmp->header.reserved2);
// 	DEBUG("BMP[wallpaper]: offset = %i\n", bmp->header.offset);
// 	DEBUG("BMP[wallpaper]: dib_header_size = %i\n", bmp->header.dib_header_size);
// 	DEBUG("BMP[wallpaper]: width_px = %i\n", bmp->header.width_px);
// 	DEBUG("BMP[wallpaper]: height_px = %i\n", bmp->header.height_px);
// 	DEBUG("BMP[wallpaper]: num_planes = %i\n", bmp->header.num_planes);
// 	DEBUG("BMP[wallpaper]: bits_per_pixel = %i\n", bmp->header.bits_per_pixel);
// 	DEBUG("BMP[wallpaper]: compression = %i\n", bmp->header.compression);
// 	DEBUG("BMP[wallpaper]: image_size_bytes = %i\n", bmp->header.image_size_bytes);
// 	DEBUG("BMP[wallpaper]: x_resolution_ppm = %i\n", bmp->header.x_resolution_ppm);
// 	DEBUG("BMP[wallpaper]: y_resolution_ppm = %i\n", bmp->header.y_resolution_ppm);
// 	DEBUG("BMP[wallpaper]: num_colors = %i\n", bmp->header.num_colors);
// 	DEBUG("BMP[wallpaper]: important_colors = %i\n", bmp->header.important_colors);
// 	assert(bmp->header.type == 0x4D42); // 'B' 'M'
// 	assert(bmp->header.bits_per_pixel == 32);
// 	assert(bmp->header.compression == 0);
// 	bmp->data = (uint32_t*)(((uint8_t*)bmp_file) + bmp->header.offset);
// 	return bmp;
// }

bmp_image_t wallpaper_bmp;

void window_need_redraw() {
	__window_need_redraw = true;
}

void window_manager_redraw() {
	if (!__window_need_redraw) {
		return;
	}
	window_sort_windows();
	// if (wallpaper_bmp.data != NULL) {
	// 	fast_memcpy((void*)buffer_video_info.addr, (unsigned char*)wallpaper_bmp.data, VIDEO_INFO_MEM_SIZE(buffer_video_info));
	// }
	// else {
	// 	memset((void*)buffer_video_info.addr, 0xC1, VIDEO_INFO_MEM_SIZE(buffer_video_info));
	// }
	window_handle_mouse();
  window_draw_all();
  window_draw_mouse();
  present_video_buffer();
	__window_need_redraw = false;
}

window_t* window_find_top() {
	if (__window_count == 0) {
		return NULL;
	}
	return window_list[__window_count-1];
}

void window_add_message(window_t *win, message_t *msg) {
	win->message_queue[win->message_queue_index++] = *msg;
	// if in WAIT state, awake 
	win->parent_task->state = PROCESS_STATE_READY;
	if (win->message_queue_index >= MAX_MESSAGE_QUEUE_LENGTH) {
		win->message_queue_index = 0;
	}
}

void window_add_message_to_focused(message_t *msg) {
	if (focused_window == NULL) {
		return;
	}
	window_add_message(focused_window, msg);
}

bool window_pop_message(window_t* win, message_t* msg_out) {
	int peek = win->message_queue_index - 1;
	if (peek < 0) {
		peek = MAX_MESSAGE_QUEUE_LENGTH-1;
	}
	if (win->message_queue[peek].message != 0) {
		*msg_out = win->message_queue[peek];

		win->message_queue[peek].message = 0;
		win->message_queue_index = peek;
		return true;
	}
	else {
		msg_out->message = 0;
	}
	return false;
}


void init_kernel_window_manager() {
	DEBUG("SIZEOF LIST :%i\n", sizeof(window_list));
	memset(window_list, 0, MAX_WINDOW_COUNT * sizeof(window_t*));
	memcpy(&buffer_video_info, &vesa_video_info, sizeof(video_info_t));
	uint32_t screen_size = VIDEO_INFO_MEM_SIZE(vesa_video_info);
	DEBUG("WIN: Allocating double buffer size : %i (%i MB)\n", screen_size, screen_size / 1024 / 1024);
	buffer_video_info.addr = (uint64_t)malloc(screen_size);
  DEBUG("WIN: Double Frame buffer addr: 0x%X\n", buffer_video_info.addr);
  DEBUG("WIN: Double Frame buffer linear addr: 0x%X\n", buffer_video_info.linear_addr);
  DEBUG("WIN: Double Frame buffer info: %i x %i : %ibpp\n", buffer_video_info.width, buffer_video_info.height, buffer_video_info.bits);
  DEBUG("WIN: Double Frame buffer pitch: %i\n", buffer_video_info.pitch);
  DEBUG("WIN: Double Frame buffer type: %i\n", buffer_video_info.type);
}
