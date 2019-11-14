OUTPUT_FORMAT(elf64-x86-64)

USER_VMA = 0x0000000000000000;

ENTRY(start)
SECTIONS
{
    . = USER_VMA;

    .text : AT(ADDR(.text) - USER_VMA)
    {
      *(.text)
    }

    .rodata ALIGN (8) : AT(ADDR(.rodata) - USER_VMA)
    {
      *(.rodata)
    }

    .data ALIGN (8) : AT(ADDR(.data) - USER_VMA)
    {
      *(.data)
    }

    .bss ALIGN (8) : AT(ADDR(.bss) - USER_VMA)
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
    __user_app_end = ALIGN(8);
}
