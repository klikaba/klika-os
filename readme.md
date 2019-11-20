# Klika-OS

Klika-OS is a hobbyist and educational operating system writen in C (and some small parts are written in assembly).

## Features

- 64bit OS (x86_64)
- Grub 2
- Pagging (2MB) pages
- Multiprocess / multi thread
- Drivers: serial, mouse, keyboard, vesa
- Userspace (ring 3)
- System calls (int 0x80)
- Graphics (double buffered)
- Window manager (in kernel space) 
- GUI library (in user space) - based on messaging (similar to WinAPI)

## Kernel TODO
- Ram disk (based on James M tutorial http://jamesmolloy.co.uk/tutorial_html/8.-The%20VFS%20and%20the%20initrd.html)
- Serial driver to use vfs
- Time : kernel and syscall
- Kill process (partially done : extend to remove win)
- Load ELF larger than 2MB
- Optimise graphics
- Child threds in userspace
- Scalable fonts (https://gitlab.com/bztsrc/scalable-font)
- Full screen or no Exclusive mode (for game loops)

## Apps
- exit()
- More compponents (button, text, radio, checkbox, ...)
- Calculator
- Sudoku


## Images

![Screenshot](https://github.com/klikaba/klika-os/blob/master/assets/screen_1.png?raw=true)

