# Klika-OS

Klika-OS is a hobbyist and educational operating system writen in C (and some small parts are written in assembly).

## Features

- 64bit OS (x86_64)
- Grub 2
- Pagging (2MB) pages
- Multiprocess / multi thread
- Drivers: serial, mouse, keyboard, vesa, ata
- Userspace (ring 3)
- System calls (int 0x80)
- Graphics (double buffered)
- ATA PIO 
- FAT 12/16/32 support (using dosfs) - read only & no long names
- Window manager (kernel space) 
- GUI library (user space) - based on messaging (similar to WinAPI)

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

## Apps
- exit()
- More compponents (button, text, radio, checkbox, ...)
- Calculator
- Sudoku


## Images

![Screenshot](https://github.com/klikaba/klika-os/blob/master/assets/screen_1.png?raw=true)

