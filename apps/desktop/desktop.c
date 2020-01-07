#include <klikaos.h>
#include <windows.h>
#include <gfx.h>
#include <bmp.h>
#include <dirent.h>
#include <malloc.h>
#include <stdlib.h>

#include "apps.h"

message_t msg;
window_t  *window;

#define MSG_USER_WIN (WINDOW_USER_MESSAGE + 1)

#define MSG_USER_BTN_APP (WINDOW_USER_MESSAGE + 10)

#define COLUMN_COUNT 12

bmp_image_t default_app_icon;
bmp_image_t wallpaper;

void start_app(char *filename) {
	syscall(SYSCall_process_from_file, filename);
}

int main() {
	bmp_from_file("/apps/desktop/appicon.bmp", &default_app_icon);
	bmp_from_file("/assets/wallp.bmp", &wallpaper);
	
	window = window_create(0, 0, 1024, 768, "Applications", MSG_USER_WIN, WINDOW_ATTR_BOTTOM | WINDOW_ATTR_NO_DRAG, WINDOW_FRAME_NONE);
	bmp_blit(WINDOW_EXT(window)->context, &wallpaper, 0, 0);
	window_present(window);

	populate_app_info();

	int app_index, i;
	for (i = 0; i < app_count; i++) {
		if (apps[i]->visible) {
			button_t *btn = button_create(window, 20 + (app_index % COLUMN_COUNT) * 84, 10 + (app_index / COLUMN_COUNT) * 89, 64, 64 + 9, apps[i]->display_name, MSG_USER_BTN_APP + i);
			if (!strcmp("/apps/desktop/appicon.bmp", apps[i]->icon_path)) {
				button_set_image(btn, BUTTON_STATE_NORMAL, &default_app_icon);
			} else {
				bmp_image_t* app_icon = malloc(sizeof(bmp_image_t));
				bmp_from_file(apps[i]->icon_path, app_icon);
				button_set_image(btn, BUTTON_STATE_NORMAL, app_icon);
			}
			app_index++;
		}
	}

	while(window_get_message(window, &msg)) {
		int app_index = msg.message - MSG_USER_BTN_APP;
		if (app_index >= 0 && app_index < app_count) {
			start_app(apps[app_index]->executable_path);
		}
		window_dispatch(window, &msg);
	}
	return 0;
}
