#include <klika_os.h>
#include <windows.h>
#include <gfx.h>
#include <bmp.h>

message_t msg;
window_t  *window;

#define MSG_USER_WIN (WINDOW_USER_MESSAGE + 1)
#define MSG_USER_BTN_SIMPLE_WIN (WINDOW_USER_MESSAGE + 2)

bmp_image_t app_icon_bmp;

void start_app(char *filename) {
	syscall(SYSCall_process_from_file, filename);
}

int main() {
	bmp_from_file("/apps/desktop/appicon.bmp", &app_icon_bmp);
	
	DEBUG("Appicon : %X", app_icon_bmp);
	window = window_create(120, 120, 300, 200, "Applications", MSG_USER_WIN);
	button_t *btn = button_create(window, 10, WINDOW_BAR_HEIGHT + 10, 64, 64 + 9, "Simple", MSG_USER_BTN_SIMPLE_WIN);
	button_set_image(btn, BUTTON_STATE_NORMAL, &app_icon_bmp);

	while(window_get_message(window, &msg)) { 
		switch(msg.message) {
			case MSG_USER_BTN_SIMPLE_WIN:
				start_app("/apps/simplew/simplew");				
				break;
		}
		window_dispatch(window, &msg);
	}
	return 0;
}