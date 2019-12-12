#include <klikaos.h>
#include <windows.h>
#include <stdlib.h>
#include <gfx.h>
#include <dirent.h>

#define MSG_USER_WIN (WINDOW_USER_MESSAGE + 1)
#define MSG_USER_LABEL (WINDOW_USER_MESSAGE + 2)

int label_counter = 0;
message_t msg;
window_t  *window;
window_t  *label;

void load_directory_entries(char* dirname, uint8_t depth) {
	int layout_y = WINDOW_BAR_HEIGHT + 10;
	DEBUG("LOOKING FOR FILES IN %s\n", dirname);
	DIR *dp;
	DIRENT *ep;     
	dp = opendir(dirname);

	if (dp != NULL)
	{
		DEBUG("OPENED THE DIR");
		while (ep = readdir (dp)) {
			DEBUG("FOUND FILE %s\n", ep->name);
			if (ep->name[0] != '.') {
				char display_name[64];
				sprintf(display_name, "%s/%s(%s)", dirname, ep->name, ep->attr & ATTR_DIRECTORY ? "DIR" : "FILE");
				label = label_create(window, depth * 10, layout_y + 30 * label_counter++, 100, 20, display_name, MSG_USER_LABEL);
			}
			/* if (ep->name[0] != '.' && ep->attr & ATTR_DIRECTORY) { */
			/* 	char full_name[64]; */
			/* 	sprintf(full_name, "%s/%s", dirname, ep->name); */
			/* 	load_directory_entries(full_name, depth + 1); */
			/* } */
		}

		(void) closedir (dp);
	}
	else
		DEBUG("Couldn't open the directory (%s)\n", dirname);
}

int main() {
	window = window_create(100, 100, 300, 300, "File Explorer", MSG_USER_WIN, WINDOW_ATTR_NONE, WINDOW_FRAME_DEFAULT);

	load_directory_entries("/apps", 1);

	while(window_get_message(window, &msg)) { 
		window_dispatch(window, &msg);
	}
	return 0;
}
