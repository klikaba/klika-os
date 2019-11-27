# Klika-OS

Klika-OS is a hobbyist and educational operating system writen in C (and some small parts are written in assembly).

## Features

- x86_64 OS
- Grub2 as boot loader
- Kernel @ 0xFFFF800000000000
- Pagging 
	- 2MB pages
	- Userspace mapped at 0x0000000000000000
	- Kernel max 512MB
	- Userspace max 512MB
	- sbrk to extend userspace memory
- Kernel heap (malloc/calloc/free)
- Multi thread
	- Kernelspace 
	- Userspace (ring3)
- Drivers 
	- Serial
	- Mouse
	- Keyboard
	- Vesa
	- Ata PIO
- System calls (int 0x80)
	- File, memory, messaging, windows, processes
- Graphics
	- Window manager in Kernel space
	- Double buffered
- FAT 12/16/32 support
	- Using DOSFS 1.02
	- Read only 
	- HD must be on Master, ISO CD on Slave
	- Mounted Disk.img to qemu
- GUI library
	- In userspace
	- Based on kernel messaging: get_message/dispatch_message
	- get_message blocks process (WAIT state)
	- Window look'n'feel logic in userspace
	- Fixed font 8x8
- BMP file support :(


## Simple app

```c
#include <klika_os.h>
#include <windows.h>
#include <stdlib.h>

#define MSG_USER_WIN 		(WINDOW_USER_MESSAGE + 1)
#define MSG_USER_BTN1 	(WINDOW_USER_MESSAGE + 2)
#define MSG_USER_BTN2 	(WINDOW_USER_MESSAGE + 3)
#define MSG_USER_LABEL 	(WINDOW_USER_MESSAGE + 4)

message_t msg;
window_t  *window;
window_t  *label;
long counter = 0;

void increment_counter(int add) {
	char buff[123];

	counter += add;
	sprintf(buff, "Count: %i", counter);
	label_set_text(label, buff);
}

int main() {
	int layout_y = WINDOW_BAR_HEIGHT + 10;
	window = window_create(100, 100, 300, 300, "Simple Window", MSG_USER_WIN);
	button_create(window, 10, layout_y, 100, 30, "Click me +", MSG_USER_BTN1);
	button_create(window, 120, layout_y, 100, 30, "Click me -", MSG_USER_BTN2);
	label = label_create(window, 10, layout_y + 40, 100, 20, "", MSG_USER_LABEL);

	while(window_get_message(window, &msg)) { 
		switch(msg.message) {
			case MSG_USER_BTN1:
				increment_counter(1);
				break;
			case MSG_USER_BTN2:
				increment_counter(-1);
				break;
		}
		window_dispatch(window, &msg);
	}
	return 0;
}
```

## Kernel TODO
- ATA Write Sector
- FAT 12/16/32 Write
- Time : kernel and syscall
- Kill process (partially done : extend to remove win)
- Load ELF larger than 2MB
- Optimise graphics functions (asm or 64bit)
- Child threads in user space
- Scalable fonts (https://gitlab.com/bztsrc/scalable-font)
- Full screen Exclusive mode (for game loops)
- Transaprent pixels

## Apps TODO
- exit()
- More compponents (button, text, radio, checkbox, ...)
- Calculator
- Sudoku


## Images

![Screenshot](https://github.com/klikaba/klika-os/blob/master/assets/screen_1.png?raw=true)

