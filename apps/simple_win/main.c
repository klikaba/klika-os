#include <klika_os.h>
#include <stdlib.h>

int main() {
	syscall(SYSCall_windows_create, 100, 100, 700, 400);
	while(1) { }
	return 0;
}