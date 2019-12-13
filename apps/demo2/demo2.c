#include <klikaos.h>
#include <windows.h>
#include <gfx.h>
#include <bmp.h>

#define MSG_USER_WIN (WINDOW_USER_MESSAGE + 1)

message_t msg;
window_t  *window;

bmp_image_t bmps[9];
int bmp_index = 0;
int count = 0;

void draw_frame() {
	bmp_image_t *bmp = &bmps[bmp_index];

	bmp_blit(WINDOW_EXT(window)->context, bmp, 2, WINDOW_BAR_HEIGHT);
	window_present(window);

	// TODO - replace this with timer once implemented
	if (count++ % 8 == 0) {
		bmp_index = (bmp_index + 1) % 9;
	}
}

int main() {
	bmp_from_file("/apps/demo2/0000.bmp", &bmps[0]);
	bmp_from_file("/apps/demo2/0001.bmp", &bmps[1]);
	bmp_from_file("/apps/demo2/0002.bmp", &bmps[2]);
	bmp_from_file("/apps/demo2/0003.bmp", &bmps[3]);
	bmp_from_file("/apps/demo2/0004.bmp", &bmps[4]);
	bmp_from_file("/apps/demo2/0005.bmp", &bmps[5]);
	bmp_from_file("/apps/demo2/0006.bmp", &bmps[6]);
	bmp_from_file("/apps/demo2/0007.bmp", &bmps[7]);
	bmp_from_file("/apps/demo2/0008.bmp", &bmps[8]);
	
	window = window_create(120, 120, 498+5, 372 + WINDOW_BAR_HEIGHT + 4, "Applications", MSG_USER_WIN, WINDOW_ATTR_NONE, WINDOW_FRAME_DEFAULT);

	while(1) {
		window_peek_message(window, &msg);
		if (msg.message != 0) {
			window_dispatch(window, &msg);
		}
		draw_frame();
	}
	return 0;
}
