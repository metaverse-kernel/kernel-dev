OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64", "elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(init32)

SECTIONS {
    . = 1M;
    .entry :
    {
        init32 = .;
        *(.entry)
    }
    .multiboot2 :
    {
        *(.multiboot2)
    }
    .cpumeta :
    {
        *(.cpumeta)
    }
    . = 16M;
    .text :
    {
        *(.text)
    }
    .data :
    {
        *(.data)
    }
    .rodata :
    {
        *(.rodata)
    }
    .bss :
    {
        *(.bss)
    }
    .comment :
    {
        *(.comment)
    }
    .eh_frame :
    {
        *(.eh_frame)
    }
    .got :
    {
        *(.got)
    }
    .got.plt :
    {
        *(.got.plt)
    }
    .iplt :
    {}
    .rela.dyn :
    {}
    .igot.plt :
    {}
    .tbss :
    {
        *(.tbss)
    }
    .tdata :
    {
        *(.tdata)
    }
    .debug_line :
    {
        *(.debug_line)
    }
    .debug_abbrev :
    {
        *(.debug_abbrev)
    }
    .debug_info :
    {
        *(.debug_info)
    }
    .debug_str :
    {
        *(.debug_str)
    }
    .debug_aranges :
    {
        *(.debug_aranges)
    }
    .debug_ranges :
    {
        *(.debug_ranges)
    }
    .debug_pubnames :
    {
        *(.debug_pubnames)
    }
    .debug_pubtypes :
    {
        *(.debug_pubtypes)
    }
    .debug_frame :
    {
        *(.debug_frame)
    }
    .kend :
    {
        *(.kend)
    }
}
