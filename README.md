# KlikaOS 

![](https://github.com/klikaba/klika-os/workflows/C%20CI/badge.svg)

KlikaOS is a hobbyist and educational operating system written in C (and some small parts are written in assembly).

## Features

- x86_64 OS
- Grub2 as boot loader
- Kernel @ 0xFFFF800000000000
- Paging 
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

You can build KLIKAOS using Vagrant Dev Environment or on your local machine.

### Build using Vagrant

Vagrantfile already contains all dependencies needed to build KLIKAOS. To use Vagrant as development environment 
following dependencies must be installed on your host machine:

* VirtualBox
* Vagrant
* QEMU (for MacOS use brew: `brew install qemu`)

Run following:

```
vagrant up
vagrant ssh
cd /vagrant
make all run
```

This will create bootable ISO image and HD image in `images` folder. To run it, you will need qemu on your local machine. You can run it (FROM YOUR LOCAL MACHINE):

```
qemu-system-x86_64 -cdrom images/klika-os-x86_64.iso -m 128 -drive file=images/disk.img,format=raw,index=0,media=disk -boot order=d -serial stdio
```

### Build on your local machine

Most effective way to build and run KLIKAOS is to install GCC cross compiler for your platform. GCC tools must be named using `x86_64-elf` prefix like:

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

Easiest way is to look into Vagrantfile and see what needs to be installed.

To build and run with qemu:

```
make all run
```


## Run

Repostiory already contains pre-built ISO and HD images. Fastest way to run it will be using qemu:

### Using qemu

```
qemu-system-x86_64 -cdrom images/klikaos-x86_64.iso -m 128 -drive file=images/disk.img,format=raw,index=0,media=disk -boot order=d -serial stdio
```

### Using VBox

Convert raw qemu image to .vdi image:

```
qemu-img convert -O vdi images/disk.img images/disk.vdi
```

Mount disk.vdi (master) and klikaos-x86_64.iso (slave). Start machine.

## Creating KLIKAOS apps

Best way to create new app is just to copy `simple_win` from `apps` folder. Example for GUI app:


```c
// See ./apps/simple_win
#include <klikaos.h>
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
	window = window_create(100, 100, 300, 300, "Simple Window", MSG_USER_WIN, WINDOW_ATTR_NONE, WINDOW_FRAME_DEFAULT);
	button_create(window, 10, layout_y, 100, 30, "Click me +", MSG_USER_BTN1);
	button_create(window, 120, layout_y, 100, 30, "Click me -", MSG_USER_BTN2);
	label = label_create(window, 10, layout_y + 40, 200, 20, "Number of clicks", MSG_USER_LABEL);

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


## Contributors ✨

Thanks goes to these wonderful people ([emoji key](https://allcontributors.org/docs/en/emoji-key)):

<!-- ALL-CONTRIBUTORS-LIST:START - Do not remove or modify this section -->
<!-- prettier-ignore-start -->
<!-- markdownlint-disable -->
<table>
  <tr>
    <td align="center"><a href="https://github.com/SamirHodzic"><img src="https://avatars0.githubusercontent.com/u/10077663?v=4" width="100px;" alt=""/><br /><sub><b>SamirH</b></sub></a><br /><a href="https://github.com/klikaba/klika-os/commits?author=SamirHodzic" title="Code">💻</a></td>
    <td align="center"><a href="http://www.ensarsarajcic.com"><img src="https://avatars2.githubusercontent.com/u/2764831?v=4" width="100px;" alt=""/><br /><sub><b>Ensar Sarajčić</b></sub></a><br /><a href="https://github.com/klikaba/klika-os/commits?author=esensar" title="Code">💻</a></td>
    <td align="center"><a href="https://github.com/ahamza1"><img src="https://avatars0.githubusercontent.com/u/17404743?v=4" width="100px;" alt=""/><br /><sub><b>Almir Hamza</b></sub></a><br /><a href="https://github.com/klikaba/klika-os/commits?author=ahamza1" title="Code">💻</a></td>
  </tr>
</table>

<!-- markdownlint-enable -->
<!-- prettier-ignore-end -->
<!-- ALL-CONTRIBUTORS-LIST:END -->

This project follows the [all-contributors](https://github.com/all-contributors/all-contributors) specification. Contributions of any kind welcome!
