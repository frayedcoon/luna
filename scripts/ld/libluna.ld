MEMORY
{
    code (rx) : ORIGIN = 0x00000000               , LENGTH = 64K
    data (rw) : ORIGIN = 0x00000000 + LENGTH(code), LENGTH = 64K
}

SECTIONS
{
    .text : ALIGN(4)
    {
        *(.text);
        *(.text*)
        . = ALIGN(4);
        *(.rodata);
        *(.rodata*)
        . = ALIGN(4);
    } > code

    .data : ALIGN(4)
    {
        *(.got)
        *(.got*)
        . = ALIGN(4);
        *(.data);
        *(.data*)
        . = ALIGN(4);
    } > data
}