#include <klikaos.h>
#include <windows.h>
#include <gfx.h>
#include <bmp.h>
#include <dirent.h>
#include <malloc.h>
#include <stdlib.h>

message_t msg;
window_t  *window;

#define MSG_USER_WIN (WINDOW_USER_MESSAGE + 1)

#define MSG_USER_BTN_APP (WINDOW_USER_MESSAGE + 10)

bmp_image_t app_icon_bmp;
bmp_image_t wallpaper;

char* apps[20];
int app_count = 0;

void start_app(char *filename) {
	syscall(SYSCall_process_from_file, filename);
}

void load_directory_entries(char* dirname) {
	DIR *dp;
	DIRENT *ep;     
	dp = opendir(dirname);

	if (dp != NULL)
	{
		while (ep = readdir (dp)) {
			if (ep->name[0] != '.') {
				char* app_name = file_name(ep);
				if (strcmp(app_name, "DESKTOP") != 0)  {
					apps[app_count++] = app_name;
				} else {
					free(app_name);
				}
			}
		}

		closedir (dp);
	}
	else
		DEBUG("Couldn't open the directory (%s)\n", dirname);
}

int main() {
	bmp_from_file("/apps/desktop/appicon.bmp", &app_icon_bmp);
	bmp_from_file("/assets/wallp.bmp", &wallpaper);
	
	window = window_create(0, 0, 1024, 768, "Applications", MSG_USER_WIN, WINDOW_ATTR_BOTTOM | WINDOW_ATTR_NO_DRAG, WINDOW_FRAME_NONE);
	bmp_blit(WINDOW_EXT(window)->context, &wallpaper, 2, 0);
	window_present(window);

	load_directory_entries("/apps");

	int i;
	for (i = 0; i < app_count; i++) {
		button_t *btn = button_create(window, 10 + i * 80, 10, 64, 64 + 9, apps[i], MSG_USER_BTN_APP + i);
		button_set_image(btn, BUTTON_STATE_NORMAL, &app_icon_bmp);
	}

	while(window_get_message(window, &msg)) { 
		if (msg.message >= MSG_USER_BTN_APP && msg.message < MSG_USER_BTN_APP + app_count) {
			char* app_name = apps[msg.message - MSG_USER_BTN_APP];
			char full_app_name[64];
			for(i = 0; i < 64; i++) full_app_name[i] = 0;
			sprintf(full_app_name, "/apps/%s/%s", app_name, app_name);
			start_app(full_app_name);
		}
		window_dispatch(window, &msg);
	}
	return 0;
}
