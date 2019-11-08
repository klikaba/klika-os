cc = x86_64-elf-gcc
ld = x86_64-elf-ld
as = x86_64-elf-as
objdump = x86_64-elf-objdump
nasm = nasm
qemu = qemu-system-x86_64
bochs = bochs

arch ?= x86_64
kernel := build/kernel-$(arch).bin
iso := build/klika-os-$(arch).iso
linker_script := src/ldscripts/kernel.ld
grub_cfg := src/grub/grub.cfg
assembly_source_files := $(wildcard src/drivers/*.asm) $(wildcard src/kernel/*.asm) $(wildcard src/lib/*.asm) $(wildcard src/arch/x86_64/*.asm)
assembly_object_files := $(patsubst src/%.asm, build/%.o, $(assembly_source_files))
nasm_flags := -w-number-overflow -felf64
c_include := 'src/include'
cflags := -fno-pic  -m64 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
		  -ffreestanding -mno-red-zone -mno-mmx -mno-sse -mno-sse2 \
	      -I $(c_include) -nostartfiles -nodefaultlibs -fno-exceptions \
	      -Wall -Wextra -Werror -c -mcmodel=large
c_source_files := $(wildcard src/drivers/*.c) $(wildcard src/kernel/*.c) $(wildcard src/lib/*.c) $(wildcard src/arch/x86_64/*.c)
c_object_files := $(patsubst src/%.c, \
	build/%.o, $(c_source_files))
quemu_mem := 128
floppy_image = floppy.img

.PHONY: all clean run iso

all: $(kernel)

clean:
	@rm -r build

run: $(iso)
	$(qemu) -cdrom $(iso) -m $(quemu_mem) -drive file=$(floppy_image),if=floppy,format=raw -boot order=d  

run-bocsh: $(iso)
	$(bochs) -f bochsrc.txt -q

run-qemu: $(iso)
	$(qemu) -cdrom $(iso) -m $(quemu_mem) -drive file=$(floppy_image),if=floppy,format=raw -boot order=d -s

iso: $(iso)

debug: nasm_flags += -g -F dwarf
debug: cflags += -g
debug: all

$(iso): $(kernel) $(grub_cfg)
	@mkdir -p build/isofiles/boot/grub
	@cp $(kernel) build/isofiles/boot/kernel.bin
	@cp $(grub_cfg) build/isofiles/boot/grub
	@grub-mkrescue -o $(iso) build/isofiles 2> /dev/null
	@rm -r build/isofiles

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
