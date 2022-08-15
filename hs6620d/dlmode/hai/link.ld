MEMORY {
	RAM (rwx): ORIGIN = 0x00000000, LENGTH = 128k
}

ENTRY(_start);

SECTIONS {
	.vectors : {
		KEEP(*(.vectors))
		. = ALIGN(64);
	} >RAM

	.text : {
		*(.text.startup)
		*(.text)
		*(.text.*)
	} >RAM

	.rodata : { 
		*(.rodata)
		*(.rodata.*)
	} >RAM

	.data : {
		*(.data)
		*(.data.*)
	} >RAM

	.bss : {
		_sbss = .;
		*(.bss)
		*(.bss.*)
		*(COMMON)
		_ebss = .;
	} >RAM

	.stack : {
		. = . + 2048;
		. = ALIGN(8);
		__system_sp = .;
	} >RAM

	end = .;
}
