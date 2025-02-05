ENTRY(_dlstart)
PHDRS
{
  headers      PT_PHDR PHDRS;
  header_load  PT_LOAD PHDRS;
  code         PT_LOAD;
  data         PT_LOAD;
  dynamic      PT_DYNAMIC;
  interp       PT_INTERP;
}
SECTIONS
{
    .text : {
        . = ALIGN(4);
        __text_start__ = .;
        KEEP(*(.first))
        *(.text)
        *(.text.*)

        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        *(SORT(.rodata.*))
        *(.rodata)

        KEEP(*(.eh_frame*))

        . = ALIGN(4);
        __rel_start__ = .;
        *(.rel.data .rel.data.* .rel.gnu.linkonce.d.*)
        __rel_end__ = .;
    }:code
    .ARM.exidx : {
        . = ALIGN(4);
        __exdix_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        /* This is used by the startup in order to initialize the .data secion */
        __exdix_end = .;
    }:code
    PROVIDE(__ram_size__ = __bss_end__ - __data_start__);

    .got : {
        . = ALIGN(4);
        __data_start__ = .;
        __got_start__ = .;
        *(.got)
        __got_end__ = .;
    } :data
  
    .data : {
        . = ALIGN(4);
        *(.data)
        *(.data.*)
    
        *(vtable)
        *(.data*)
    
        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
    
        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);
    
        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        . = ALIGN(4);
        _shell_command_start = .;
        KEEP(*(shellCommand))
        _shell_command_end = .;
    
        . = ALIGN(4);
        /* All data end */
        __data_end__ = .;
    }:data
    /* .debug : { *(.debug) }:text */
    /* .line : { *(.line) }:text */

    PROVIDE(__heap_size__ = __heap_end__ - __heap_start__);
    PROVIDE(__stack_size__ = __stack_end__ - __stack_start__);
    .bss : {
        . = ALIGN(4);
        /* This is used by the startup in order to initialize the .bss secion */
        __bss_start__ = .;
        *(.bss)
        *(COMMON)
    
        . = ALIGN(4);
        __heap_start__ = .;
        KEEP(*(.bss.heap))
        __heap_end__ = .;
    
        . = ALIGN(4);
        __stack_start__ = .;
        KEEP(*(.bss.stack))
        __stack_end__ = .;
    
        *(.bss.*)
        /* This is used by the startup in order to initialize the .bss secion */
        . = ALIGN(4);
        __bss_end__ = .;
    }:data
    .dynamic : { *(.dynamic) }:dynamic
    .permissions_table : {
        . = ALIGN(4);
        __permissions_table_start__ = .;
        KEEP(*(.permissions_table))
        __permissions_table_end__ = .;
    }:data
    _end = .;
}
