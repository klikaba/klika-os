platform_sufix ?= x86_64-elf-
cc = $(platform_sufix)gcc
ld = $(platform_sufix)ld
as = $(platform_sufix)as
nm = $(platform_sufix)nm
objdump = $(platform_sufix)objdump
objcopy = $(platform_sufix)objcopy
nasm = nasm
qemu = qemu-system-x86_64
bochs = bochs

arch ?= x86_64
kernel := build/kernel-$(arch).bin
isoname := klika-os-$(arch).iso
iso := build/$(isoname)
linker_script := src/ldscripts/kernel.ld
grub_cfg := src/grub/grub.cfg
assembly_source_files := $(wildcard src/drivers/*.asm) $(wildcard src/kernel/*.asm) $(wildcard src/lib/*.asm) $(wildcard src/arch/x86_64/*.asm)
assembly_object_files := $(patsubst src/%.asm, build/%.o, $(assembly_source_files))
nasm_flags := -w-number-overflow -felf64
c_include := 'src/include'
cflags := -fno-pic  -m64 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
		  -ffreestanding -mno-red-zone -mno-mmx -mno-sse -mno-sse2 \
	      -I $(c_include) -nostartfiles -nodefaultlibs -fno-exceptions \
	      -Wall -Wextra -Werror -c -mcmodel=large -Wno-implicit-fallthrough -O2 \
	      -Wno-parentheses
c_source_files := $(wildcard src/drivers/*.c) $(wildcard src/kernel/*.c) $(wildcard src/lib/*.c) $(wildcard src/arch/x86_64/*.c) $(wildcard src/syscalls/*.c)
c_object_files := $(patsubst src/%.c, \
	build/%.o, $(c_source_files))
quemu_mem := 128
hd_image = ../images/disk.img

.PHONY: all clean run iso install

all: $(kernel)

clean:
	@rm -rfv build

install:
	dd if=/dev/zero of=../images/disk.img count=81920 bs=512
	mformat -F -v KLIKA-OS -i ../images/disk.img
	mcopy -svn -i ../images/disk.img ../images/disk/* ::. 

run: $(iso)
	$(qemu) -cdrom $(iso) -m $(quemu_mem) -drive file=$(hd_image),format=raw,index=0,media=disk  -boot order=d -serial stdio

run-bocsh: $(iso)
	$(objcopy) --only-keep-debug $(kernel) $(kernel).sym
	$(nm) $(kernel).sym | grep " T " | awk '{ print $$1" "$$3 }' > $(kernel).bochs.sym
	$(bochs) -rc ../bochs.rc -f ../bochs.cfg -q

run-qemu: $(iso)
	$(qemu) -cdrom $(iso) -m $(quemu_mem) -drive file=$(hd_image),format=raw,index=0,media=disk  -boot order=d -s -S

iso: $(iso)

debug: nasm_flags += -g -F dwarf
debug: cflags += -g 
debug: all

$(iso): $(kernel) $(grub_cfg)
	@mkdir -p build/isofiles/boot/grub
	@cp $(kernel) build/isofiles/boot/kernel.bin
	@cp $(grub_cfg) build/isofiles/boot/grub
	@grub-mkrescue -o $(iso) build/isofiles 2> /dev/null
	@cp $(iso) ../images/$(isoname)

$(kernel): $(assembly_object_files) $(c_object_files) $(linker_script)
	$(ld) -nostdlib -n -T $(linker_script) -o $(kernel) $(assembly_object_files) $(c_object_files)
	$(objdump) -D $(kernel) > build/kernel.dump.asm
	$(objdump) -x $(kernel) >> build/kernel.headers.txt

# compile assembly files
build/%.o: src/%.asm
	@mkdir -p $(shell dirname $@)
	$(nasm) $(nasm_flags) $< -o $@

# compile c files
build/%.o: src/%.c
	@mkdir -p $(shell dirname $@)
	$(cc) $(cflags) $< -o $@
