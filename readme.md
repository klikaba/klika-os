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


## Build


### Platform's Cross Compiler

Most effective way to build and run KLIKA-OS is to install GCC cross compiler for your platform. GCC tools must be named using `x86_64-elf` prefix like:

```
x86_64-elf-gcc
x86_64-elf-ld
x86_64-elf-as
x86_64-elf-nm
x86_64-elf-objdump
x86_64-elf-objcopy
...
```

Dependencies:

- qemu (4.0.0)
- mtools (MUST BE 4.0.23)
- gcc crosscompile
	- x86_64-elf-gcc (8.3.0)
	- x86_64-elf-ld (binutils 2.32)
	- x86_64-elf-as (binutils 2.32)
	- x86_64-elf-nm (binutils 2.32)
	- x86_64-elf-objdump (binutils 2.32)
	- x86_64-elf-objcopy (binutils 2.32)
	- nasm (2.14.02)
	- grub-mkrescue (2.05)

To build and run with qemu:

```
cd apps
make all
```

```
cd kernel
make all iso install run
```


### Using Vagrant

SOON.

## Run

Repostiory already contains pre-built ISO and HD images. Fastest way to run it will be using qemu:

### Using qemu

```
qemu -cdrom images/klika-os-x86_64.iso -m 128 -drive file=images/disk.img,format=raw,index=0,media=disk -boot order=d -serial stdio
```

### Using VBox

Convert raw qemu image to .vdi image:

```
qemu-img convert -O vdi images/disk.img images/disk.vdi
```

Mount disk.vdi (master) and klika-os-x86_64.iso (slave). Start machine.

## Building apps

Best way to create new app is just to copy `simple_win` from `apps` folder. 


```c
// See ./apps/simple_win
#include <klika_os.h>
#include <windows.h>
#include <stdlib.h>

#define MSG_USER_WIN		(WINDOW_USER_MESSAGE + 1)
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
- Desktop - discover apps from `/apps` folder
- exit()
- More components (button, text, radio, checkbox, ...)
- Calculator
- Sudoku

## Images

![Screenshot](https://github.com/klikaba/klika-os/blob/master/assets/screen_1.png?raw=true)

