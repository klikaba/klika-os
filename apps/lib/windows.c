#include <windows/window.h>

void window_dispatch_message(window_t *win, struct message_struct *msg) {
	bool consumed = false;
	if (win->window_procedure != NULL) {
		consumed = win->window_procedure(win, msg);
	}
	if (!consumed) {
		win->window_default_procedure(win, msg);
	}
}
