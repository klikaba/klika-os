#include <klikaos.h>
#include <windows.h>
#include <gfx.h>
#include <bmp.h>

message_t msg;
window_t  *window;

#define MSG_USER_WIN (WINDOW_USER_MESSAGE + 1)
#define MSG_USER_BTN_SIMPLE_WIN (WINDOW_USER_MESSAGE + 2)
#define MSG_USER_BTN_DEMO1 (WINDOW_USER_MESSAGE + 3)
#define MSG_USER_BTN_DEMO2 (WINDOW_USER_MESSAGE + 4)
#define MSG_USER_BTN_FILETREE (WINDOW_USER_MESSAGE + 5)

bmp_image_t app_icon_bmp;
bmp_image_t wallpaper;

void start_app(char *filename) {
	syscall(SYSCall_process_from_file, filename);
}

int main() {
	bmp_from_file("/apps/desktop/appicon.bmp", &app_icon_bmp);
	bmp_from_file("/assets/wallp.bmp", &wallpaper);
	
	window = window_create(0, 0, 1024, 768, "Applications", MSG_USER_WIN, WINDOW_ATTR_BOTTOM | WINDOW_ATTR_NO_DRAG, WINDOW_FRAME_NONE);
	bmp_blit(WINDOW_EXT(window)->context, &wallpaper, 2, 0);
	window_present(window);

	button_t *btn = button_create(window, 10, 10, 64, 64 + 9, "Simple", MSG_USER_BTN_SIMPLE_WIN);
	button_set_image(btn, BUTTON_STATE_NORMAL, &app_icon_bmp);

	btn = button_create(window, 90, 10, 64, 64 + 9, "Demo1", MSG_USER_BTN_DEMO1);
	button_set_image(btn, BUTTON_STATE_NORMAL, &app_icon_bmp);

	btn = button_create(window, 170, 10, 64, 64 + 9, "Demo2", MSG_USER_BTN_DEMO2);
	button_set_image(btn, BUTTON_STATE_NORMAL, &app_icon_bmp);

	btn = button_create(window, 250, 10, 64, 64 + 9, "Explorer", MSG_USER_BTN_FILETREE);
	button_set_image(btn, BUTTON_STATE_NORMAL, &app_icon_bmp);

	while(window_get_message(window, &msg)) { 
		switch(msg.message) {
			case MSG_USER_BTN_SIMPLE_WIN:
				start_app("/apps/simplew/simplew");				
				break;
			case MSG_USER_BTN_DEMO1:
				start_app("/apps/demo1/demo1");				
				break;
			case MSG_USER_BTN_DEMO2:
				start_app("/apps/demo2/demo2");				
				break;
			case MSG_USER_BTN_FILETREE:
				start_app("/apps/fexpl/fexpl");
				break;
		}
		window_dispatch(window, &msg);
	}
	return 0;
}
