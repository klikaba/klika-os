name = hello_world

cc = x86_64-elf-gcc
ld = x86_64-elf-ld
objdump = x86_64-elf-objdump
nasm = nasm
xxd = xxd
arch ?= x86_64
linker_script := ../lib/ldscript.ld
c_include := '../include'

cflags := -fno-pic  -m64 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
		  -ffreestanding -mno-red-zone -mno-mmx -mno-sse -mno-sse2 \
	      -I $(c_include) -nostartfiles -nodefaultlibs -fno-exceptions \
	      -Wall -Wextra -Werror -c -mcmodel=large

c_source_files := $(wildcard *.c)
c_object_files := $(patsubst %.c, build/%.o, $(c_source_files))

asm_startup_file := ../lib/startup.asm
nasm_flags :=  -felf64 -w-number-overflow

.PHONY: all clean run

all: $(name)

clean:
	@rm -r build

debug: nasm_flags += -g -F dwarf
debug: cflags += -g
debug: all

$(name): $(c_object_files) $(linker_script)
	$(ld) -nostdlib -n -T $(linker_script) -o $(name) $(c_object_files) # build/startup.o
	$(objdump) -D $(name) > build/$(name).dump.asm
	$(objdump) -x $(name) >> build/$(name).headers.txt
	xxd -i $(name) > build/$(name)_byte.c


# compile c files
build/%.o: %.c
	@mkdir -p $(shell dirname $@)
	$(cc) $(cflags) $< -o $@
