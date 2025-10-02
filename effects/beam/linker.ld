ENTRY(init);

MEMORY {
    RAM (rwx): ORIGIN = 0x20020000, LENGTH = 64K
}

SECTIONS {
    . = ORIGIN(RAM) + SIZEOF_HEADERS;

    .text ALIGN(4) :
    {
        *(.text)
    } > RAM

    .data ALIGN(4):
    {
        *(.data)
    } > RAM

    .bss ALIGN(4):
    {
        *(.bss)
    } > RAM
}
