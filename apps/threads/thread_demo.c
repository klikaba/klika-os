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
label_t  *label1, *label2;

uint64_t counter1 = 0;
uint64_t counter2 = 0;

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


void set_label_counter(label_t *label, char *format, int counter) {
	char buff[123];

	sprintf(buff, format, counter);
	label_set_text(label, buff);	
}

int main() {
	window = window_create(100, 100, 300, 300, "Threads", MSG_USER_WIN, WINDOW_ATTR_NONE, WINDOW_FRAME_DEFAULT);

	label1 = label_create(window, 10, 50, 100, 20, "", MSG_USER_LABEL1);
	label2 = label_create(window, 10, 70, 100, 20, "", MSG_USER_LABEL2);

	thread_create(thread1, 1024);
	thread_create(thread2, 1024);

	while(window_get_message(window, &msg)) { 
		switch(msg.message) {
			case WINDOW_LIB_MESSAGE_DRAW:
				set_label_counter(label1, "Thread 1 : %i", counter1);
				set_label_counter(label2, "Thread 2 : %i", counter2);
				window_invalidate(window);
				break;
		}		
		window_dispatch(window, &msg);
	}
	return 0;
}
