#include <klika_os.h>

char* name = "Hello world";

void puts(char* text) {
	int x = 5;
	while (*text) {
		syscall(0, x++, 5, *text++);
	}
}

int main() {
	puts(name);
	while(1) {}
	return 0;
}