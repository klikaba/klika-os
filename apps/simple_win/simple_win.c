#include <klikaos.h>
#include <windows.h>
#include <malloc.h>
#include <stdlib.h>
#include <gfx.h>

typedef struct {
	uint32_t id;
	uint32_t stack_size;
	uint8_t *stack;
} thread_t;

typedef void* (*thread_function_t)();

thread_t* thread_create(thread_function_t thread_function, uint32_t stack_size) {
	thread_t *thread = malloc(sizeof(thread_t));

	thread->stack_size = stack_size;
	thread->stack = malloc(stack_size);

	thread->id = syscall(SYSCall_process_clone, thread_function, thread->stack - stack_size);

	if (thread->id == 0) {
		free(thread);
		return NULL;
	}
	return thread;
}


message_t msg;
window_t  *window;
window_t  *label;

long counter = 0;

#define MSG_USER_WIN (WINDOW_USER_MESSAGE + 1)
#define MSG_USER_BTN1 (WINDOW_USER_MESSAGE + 2)
#define MSG_USER_BTN2 (WINDOW_USER_MESSAGE + 3)
#define MSG_USER_LABEL (WINDOW_USER_MESSAGE + 4)

void* increment_counter() {
	char buff[123];

	while(1) {
		sprintf(buff, "Count: %i .", counter);
		counter++;
		label_set_text(label, buff);
		window_invalidate(window);
	}
	return NULL;
}

int main() {
	int layout_y = WINDOW_BAR_HEIGHT + 10;
	window = window_create(100, 100, 300, 300, "Simple Window", MSG_USER_WIN, WINDOW_ATTR_NONE, WINDOW_FRAME_DEFAULT);
	button_create(window, 10, layout_y, 100, 30, "Click me +", MSG_USER_BTN1);
	button_create(window, 120, layout_y, 100, 30, "Click me -", MSG_USER_BTN2);
	label = label_create(window, 10, layout_y + 40, 100, 20, "", MSG_USER_LABEL);

	thread_create(increment_counter, 1024);

	while(window_get_message(window, &msg)) { 
		window_dispatch(window, &msg);
	}
	return 0;
}
