#include <messaging.h>
#include <klika_os.h>
#include <syscalls.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdint.h>

uint32_t handle;
message_t msg;

int main() {
	handle = syscall(SYSCall_windows_create, 100, 100, 600, 400, "Hello app");

	while(syscall(SYSCall_windows_get_message, &msg, handle)) { 
		if (msg.message != 0) {
			switch(msg.message) {
				case MESSAGE_MOUSE_MOVE:
					DEBUG("Mouse move (%i, %i)", msg.mouse_x, msg.mouse_y);
					break;
				case MESSAGE_MOUSE_CLICK:
					DEBUG("Mouse click (%i, %i) : %i", msg.mouse_x, msg.mouse_y, msg.mouse_buttons);
					break;
				case MESSAGE_KEY:
					DEBUG("Keyboard (%i, %c)", msg.key << 8, msg.key & 0xFF);
					break;
			}
		}
	}
	return 0;
}