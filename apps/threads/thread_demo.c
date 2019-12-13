#include <klikaos.h>
#include <windows.h>
#include <malloc.h>
#include <stdlib.h>
#include <thread.h>

#define MSG_USER_WIN (WINDOW_USER_MESSAGE + 1)
#define MSG_USER_LABEL1 (WINDOW_USER_MESSAGE + 2)
#define MSG_USER_LABEL2 (WINDOW_USER_MESSAGE + 3)
#define MSG_USER_LABEL3 (WINDOW_USER_MESSAGE + 4)

message_t msg;
window_t *window;
label_t  *label1, *label2, *label3;

long counter1 = 0;
long counter2 = 0;
long counter3 = 0;

void* thread1() {
	while(1) {
		counter1++;
	}
	return NULL;
}

void* thread2() {
	while(1) {
		counter2++;
	}
	return NULL;
}

void* thread3() {
	while(1) {
		counter3++;
	}
	return NULL;
}

void set_label_counter(label_t *label, int counter) {
	char buff[123];

	sprintf(buff, "Count: %i", counter);
	label_set_text(label, buff);	
}

int main() {
	window = window_create(100, 100, 300, 300, "Simple Window", MSG_USER_WIN, WINDOW_ATTR_NONE, WINDOW_FRAME_DEFAULT);

	label1 = label_create(window, 10, 50, 100, 20, "", MSG_USER_LABEL1);
	label2 = label_create(window, 10, 70, 100, 20, "", MSG_USER_LABEL2);
	label3 = label_create(window, 10, 90, 100, 20, "", MSG_USER_LABEL3);

	thread_create(thread1, 1024);
	thread_create(thread2, 1024);
	thread_create(thread3, 1024);

	while(window_get_message(window, &msg)) { 
		switch(msg.message) {
			case WINDOW_LIB_MESSAGE_DRAW:
				set_label_counter(label1, counter1);
				set_label_counter(label2, counter2);
				set_label_counter(label3, counter3);
				window_invalidate(window);
				break;
		}		
		window_dispatch(window, &msg);
	}
	return 0;
}
