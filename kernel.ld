ENTRY(boot)
SECTIONS {
. = 0x80200000;
.text :{
KEEP(*(.text.boot));
*(.text .text.*);
}
.rodata : ALIGN(4) {
*(.rodata .rodata.*);
}
.data : ALIGN(4) {
*(.data .data.*);
}
.bss : ALIGN(4) {
__bss = .;
*(.bss .bss.* .sbss .sbss.*);
__bss_end = .;
}
. = ALIGN(4);
. += 128 * 1024; /* 128KB stack size */
__stack_top = .;
. = ALIGN(4096); /* Align to 4 KiB page boundary */
__free_ram_start = .;
. += 32 * 1024 * 1024; /* 32 MiB free RAM for page frames */
__free_ram_end = .;
}