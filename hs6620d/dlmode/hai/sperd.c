#include <stdint.h>
#include <string.h>

#define REG32(a)	(*(volatile uint32_t *)(a))

void uputc(int c) {
	while ((REG32(0x40040014) & 0x60) != 0x60);
	REG32(0x40040000) = c;
}

int ugetc(void) {
	while ((REG32(0x40040014) & 0x01) != 0x01);
	return REG32(0x40040000);
}

/* Startup Function */
__attribute__((noreturn)) void _start(void) {
	extern char _sbss, _ebss;
	memset(&_sbss, 0, &_ebss - &_sbss);

	for (;;) {
		for (char *c = "barusa mikosu!\n"; *c; c++)
			uputc(*c);
	}
}
 
/* Vectors (ROM) */
extern char __system_sp;
extern void SystickHandler(void);
__attribute__((section(".vectors"))) const void *_vectors_rom[] = {
	&__system_sp,		/* master stack pointer */
	_start,			/* reset */
	NULL,			/* nmi */
	NULL,			/* hardfault */
	NULL,			/* memmanage */
	NULL,			/* busfault */
	NULL,			/* usagefault */
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,			/* svcall */
	NULL,			/* debugmon */
	(void*)0x66200200,			// for HS6620D +0x34
	NULL,			/* pendsv */
	NULL,			/* systick */
};
