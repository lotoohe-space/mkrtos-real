ENTRY(Reset_Handler)
MEMORY
{
 ROM (arx) : ORIGIN = 0x00000000, LENGTH = 0x3000
  RAM (arw) : ORIGIN = 0x20000000 + 0x10000 - 0x4000, LENGTH = 0x4000 - 0x300
}
SECTIONS
{
    . = ORIGIN(ROM);
    .text :
    {
        KEEP(*(.isr_vector))
        *(.text)
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
        cpio_start = .;
        *(.cpio*)
        . = ALIGN(4);
        cpio_end = .;
    } > ROM
    . = ALIGN(4);
    __data_load__ = LOADADDR(.data);
    . = ALIGN(4);
    .data :
    {
        __data_start__ = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        __data_end__ = .;
    } > RAM AT> ROM
    . = ALIGN(4);
    .bss :
    {
        __bss_start__ = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > RAM
    . = ALIGN(4);
    .heap :
    {
        end = .;
        __HeapBase = .;
        *(.heap)
    } > RAM
    .stack_dummy :
    {
        *(.stack)
    } > RAM
    __StackTop = ORIGIN(RAM) + LENGTH(RAM);
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);
}
