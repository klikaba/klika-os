#include <klika_os.h>

int num = 0xdead;
int sum;
char *name = "zaharije pasalic";

int increment() {
	sum = num;
	return num++;
}

int main() {
	while(1) {
		increment();
		syscall(0, 5, 5, sum);
	}
	return 0;
}