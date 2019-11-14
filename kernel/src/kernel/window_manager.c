#include <window_manager.h>
#include <mmu_heap.h>
#include <process.h>
#include <stdint.h>
#include <string.h>
#include <kernel.h>
#include <assert.h>
#include <mouse.h>
#include <timer.h>
#include <vesa.h>
#include <gfx.h>
#include <isr.h>

#define WIN_BACKGROUND_COLOR 	0x7F7F7F
#define WIN_BORDER_COLOR      0xCCCCCC
#define WIN_INACTIVE_FRAME  	0x0000AA
#define WIN_ACTIVE_FRAME    	0x0000FF
#define WIN_FRAME_TEXT_COLOR  0xFFFFFF

#define WIN_SORT_VAL(win) (win == NULL ? 1000000000 : win->z) 

window_t* window_list[MAX_WINDOW_COUNT];
video_info_t buffer_video_info;
uint64_t __last_update_tick = 0;
uint32_t __window_handle = 1000; // 1000 just magic number to start from (0 means no window)
int __window_count = 0;

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
  for (__window_count=0; __window_count<MAX_WINDOW_COUNT; __window_count++) {
  	if (window_list[__window_count] == NULL) break;
  }
}


bool window_point_inside(window_t* win, int x, int y) {
	return x >= win->x && x <= win->x + win->width &&
				 y >= win->y && y <= win->y + win->height;
}

#define WINDOW_BAR_HEIGHT  (8 + 2*3)

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

window_t* window_create(int x, int y, int width, int height, char* title) {
	window_t* new_win = (window_t*)malloc(sizeof(window_t));
	new_win->x = x;
	new_win->y = y;
	new_win->z = find_max_z() + 1;
	new_win->width = width;
	new_win->height = height;
	new_win->handle = __window_handle++;
	new_win->message_queue_index = 0;
	memset(new_win->message_queue, 0, sizeof(message_t)*MAX_MESSAGE_QUEUE_LENGTH);
	strncpy(new_win->title, title, MAX_WINDOW_NAME_LENGTH-1);
	for(int i=0; i<MAX_WINDOW_COUNT; i++) {
		if (window_list[i] == NULL) {
			window_list[i] = new_win;
			task_list_current->win_handle = new_win->handle;
			break;
		}
	}
	return new_win;
}

void window_draw(window_t *win, bool is_top) {
	int x1 = win->x;
	int y1 = win->y;
	int x2 = win->x + win->width;
	int y2 = win->y + win->height;

	// Main background
	gfx_fillrect(&buffer_video_info, x1, y1, x2, y2, WIN_BORDER_COLOR);
	gfx_fillrect(&buffer_video_info, x1+1, y1+1, x2-1, y2-1, WIN_BACKGROUND_COLOR);

	// Frame top
	uint32_t top_frame_color = is_top ? WIN_ACTIVE_FRAME : WIN_INACTIVE_FRAME;
	gfx_fillrect(&buffer_video_info, x1 + 1, y1 + 1, x2 - 1, y1 + WINDOW_BAR_HEIGHT, top_frame_color);

	// Frame label
	gfx_puts(&buffer_video_info, x1 + 1 + 3, y1 + 1 + 3, WIN_FRAME_TEXT_COLOR, top_frame_color, win->title);
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
				if (mouse_buttons & LEFT_CLICK) {
					color = 0xFFFFFF;
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
}

// HOLYY SHAJT - simplify this poop
void window_handle_mouse() {
	if (mouse_buttons & LEFT_CLICK) {
		if (window_to_drag != NULL) {
			window_to_drag->x += mouse_x - __old_mouse_x;
			window_to_drag->y += mouse_y - __old_mouse_y;
			__old_mouse_x = mouse_x;
			__old_mouse_y = mouse_y;
		}	
		else {
			// Find win under the mouse and bring it to top
			int idx = window_find_xy(mouse_x, mouse_y);
			if (idx != -1) {
				window_bring_to_front(idx);
				window_t* win = window_list[idx];
				// Check for dragging
				if (window_point_inside_bar(win, mouse_x, mouse_y)) {
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
}

void window_manager_redraw() {
	window_sort_windows();
	memset((void*)buffer_video_info.addr, 0, VIDEO_INFO_MEM_SIZE(buffer_video_info));
	window_handle_mouse();
  window_draw_all();
  window_draw_mouse();
  present_video_buffer();
}

window_t* window_find_top() {
	if (__window_count == 0) {
		return NULL;
	}
	return window_list[__window_count-1];
}


void window_add_message(message_t msg) {
	window_t* win = window_find_top();
	if (win == NULL) {
		return;
	}
	win->message_queue[win->message_queue_index++] = msg;
	DEBUG("WIN: Add Message %i\n\r", msg.message);
	if (win->message_queue_index >= MAX_MESSAGE_QUEUE_LENGTH) {
		win->message_queue_index = 0;
	}
}

bool window_pop_message(message_t* msg_out, window_t* win) {
	int peek = win->message_queue_index - 1;
	if (peek < 0) {
		peek = MAX_MESSAGE_QUEUE_LENGTH-1;
	}
	if (win->message_queue[peek].message != 0) {
		DEBUG("WIN: peek %i (%i)\n\r", peek, win->message_queue[peek].message);
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
	DEBUG("SIZEOF LIST :%i\n\r", sizeof(window_list));
	memset(window_list, 0, MAX_WINDOW_COUNT * sizeof(window_t*));
	memcpy(&buffer_video_info, &vesa_video_info, sizeof(video_info_t));
	uint32_t screen_size = VIDEO_INFO_MEM_SIZE(vesa_video_info);
	DEBUG("WIN: Allocating double buffer size : %i (%i MB)\n\r", screen_size, screen_size / 1024 / 1024);
	buffer_video_info.addr = (uint64_t)malloc(screen_size);
  DEBUG("WIN: Double Frame buffer addr: 0x%X\n\r", buffer_video_info.addr);
  DEBUG("WIN: Double Frame buffer linear addr: 0x%X\n\r", buffer_video_info.linear_addr);
  DEBUG("WIN: Double Frame buffer info: %i x %i : %ibpp\n\r", buffer_video_info.width, buffer_video_info.height, buffer_video_info.bits);
  DEBUG("WIN: Double Frame buffer pitch: %i\n\r", buffer_video_info.pitch);
  DEBUG("WIN: Double Frame buffer type: %i\n\r", buffer_video_info.type);
}
