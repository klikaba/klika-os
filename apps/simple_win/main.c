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
					DEBUG("Message: %i - Mouse move (%i, %i)", msg.message, msg.mouse_x, msg.mouse_y);
					break;
				case MESSAGE_MOUSE_CLICK:
					DEBUG("Message: %i - Mouse click (%i, %i) : %i", msg.message, msg.mouse_x, msg.mouse_y, msg.mouse_buttons);
					break;
				case MESSAGE_KEY_PRESS:
					DEBUG("Message: %i - Key press (%c)", msg.message, msg.key & 0xFF);
					break;
				case MESSAGE_KEY_RELEASE:
					DEBUG("Message: %i - Key release (%c)", msg.message, msg.key & 0xFF);
					break;
				default:
					DEBUG("Got message : %i", msg.message);
					break;
			}
		}
	}
	return 0;
}