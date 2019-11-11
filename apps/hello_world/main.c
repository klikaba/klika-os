#include <klika_os.h>

int main() {
	int i=0;
	while(1) {
		syscall(0, 0, 10, i++);
	}
	return 0;
}