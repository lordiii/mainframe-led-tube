SECTIONS
{
    . = 0x20010000;

    .bss.fx_binary ALIGN(4) (NOLOAD) :
    {
        *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.fx_binary)))
        . = ALIGN(32);
    } > DTCM

    . = ADDR(.bss.fx_binary) + (64 * 1024);

    .bss.fx_jmp_table ALIGN(4) (NOLOAD) :
    {
        *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.fx_jmp_table)))
        . = ALIGN(32);
    } > DTCM
}
