#include <klikaos.h>
#include <windows.h>
#include <stdlib.h>
#include <gfx.h>

message_t msg;
window_t  *window;
window_t  *label;
window_t  *label_mem;
mmu_frame_stats_t memory;

long counter = 0;

#define MSG_USER_WIN (WINDOW_USER_MESSAGE + 1)
#define MSG_USER_LABEL_1 (WINDOW_USER_MESSAGE + 2)
#define MSG_USER_LABEL_2 (WINDOW_USER_MESSAGE + 3)

void get_free_memory() {
	char buff[123];
	mmu_frame_stats_t stats;

	memory_stats(&stats);
	sprintf(buff, "u:%iMB f:%iMB", stats.used_frames * 2, stats.free_frames * 2);
	label_set_text(label_mem, buff);
}

int main() {
	int layout_y = WINDOW_BAR_HEIGHT + 10;
	window = window_create(100, 100, 300, 300, "System Info", MSG_USER_WIN, WINDOW_ATTR_NONE, WINDOW_FRAME_DEFAULT);
	label = label_create(window, 10, layout_y + 20, 100, 20, "Klika OS Version: 1.0.0", MSG_USER_LABEL_1);

	label_mem = label_create(window, 10, layout_y + 60, 100, 20, "", MSG_USER_LABEL_2);

	while(window_get_message(window, &msg)) {
		get_free_memory();
		window_dispatch(window, &msg);
	}
	return 0;
}