OUTPUT_FORMAT(elf64-x86-64)

KERNEL_LMA = 0x100000; 
KERNEL_VMA = 0xFFFF800000000000;

ENTRY(start)
SECTIONS
{
    . = KERNEL_LMA;
    .boot : {
      *(.multiboot_header)
    }

    . += KERNEL_VMA;
  
    .text : AT(ADDR(.text) - KERNEL_VMA)
    {
      *(EXCLUDE_FILE(multiboot_header) .text)
      *(.text)
    }

    .rodata ALIGN (0x1000) : AT(ADDR(.rodata) - KERNEL_VMA)
    {
      *(.rodata)
    }

    .data ALIGN (0x1000) : AT(ADDR(.data) - KERNEL_VMA)
    {
      *(.data)
    }

    .bss ALIGN (0x1000) : AT(ADDR(.bss) - KERNEL_VMA)
    {
      *(.bss)
      /*
      * You usually need to include generated COMMON symbols
      * under kernel BSS section or use gcc's -fno-common
      */
      *(COMMON)
    }

    /DISCARD/ :
    {
      *(.comment)
      /* Exception handling section - we do not need it*/
      *(.eh_frame)
    }

    kernel_end = ALIGN(4K);

}
