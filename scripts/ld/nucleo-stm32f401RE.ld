MEMORY
{
    flash (rx)      : ORIGIN = 0x08000000, LENGTH = 512K
    ram   (xrw)     : ORIGIN = 0x20000000, LENGTH = 96K
}

STACK_END = 0x20017000;

SECTIONS
{
    . = ORIGIN(flash);
    .text : {
        *(.vectors);
        . = ALIGN(1024);
        *(.got)
        *(.got*)
        *(.text);
        *(.text*)
        . = ALIGN(4);
        *(.rodata);
        *(.rodata*)
        . = ALIGN(4);
        INCLUDE scripts/ld/section_common.ld
    } > flash

    INIT_DATA_VALUES = LOADADDR(.data);

    .data : {
        INIT_DATA_START = .;
        *(.data);
        *(.data*);
        INIT_DATA_END = .;
    } > ram AT > flash

    . = ALIGN(4);

    .bss : {
        BSS_START = .;
        *(.bss);
        *(.bss*);
        . = ALIGN(4);
        BSS_END = .;
    } > ram

    .heap_stack : {
    . = ALIGN(4);
    HEAP_START = .;
    . = ALIGN(4);
    } > ram
}
