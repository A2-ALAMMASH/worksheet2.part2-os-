ENTRY(loader)                 /* the name of the entry label */

SECTIONS {
    . = 0x00100000;           /* the code should be loaded at 1 MB */

    .text ALIGN (0x1000) :
    {
        *(.text)              /* all text sections from all files */
    }

    .rodata ALIGN (0x1000) :
    {
        *(.rodata*)           /* all read-only data sections */
    }

    .data ALIGN (0x1000) :
    {
        *(.data)              /* all data sections */
    }

    .bss ALIGN (0x1000) :
    {
        *(COMMON)             /* all COMMON sections */
        *(.bss)               /* all bss sections */
    }
}