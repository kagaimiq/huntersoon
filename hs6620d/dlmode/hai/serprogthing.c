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
	/* clear BSS & copy DATA */ {
	extern char _sbss, _ebss;
	memset(&_sbss, 0, &_ebss - &_sbss);
	}

	for (;;) {
		uint8_t cmd = ugetc();

		switch (cmd) {
		/* NOP */
		case 0x00:
			uputc(0x06);
			break;

		/* Query programmer iface version */
		case 0x01:
			uputc(0x06);
			uputc(0x01);
			uputc(0x00);
			break;

		/* Query supported commands bitmap */
		case 0x02:
			uputc(0x06);

			for (int i = 0; i < 0x100; i += 8) {
				uint8_t b = 0;
				for (int j = 0; j < 8; j++) {
					switch (i+j) {
					case 0x00:	/* NOP */
					case 0x01:	/* Query programmer iface version */
					case 0x02:	/* Query supported commands bitmap */
					case 0x03:	/* Query programmer name */
					case 0x04:	/* Query serial buffer size */
					case 0x05:	/* Query supported bustypes */
					case 0x10:	/* Sync NOP */
					case 0x12:	/* Set used bustype */
					case 0x13:	/* Perform SPI operation */
						b |= 1<<j;
					}
				}
				uputc(b);
			}
			break;

		/* Query programmer name */
		case 0x03:
			uputc(0x06);

			static const char name[16] = "HS6620 serprog!";
			for (int i = 0; i < 16; i++)
				uputc(name[i]);
			break;

		/* Query serial buffer size */
		case 0x04:
			uputc(0x06);
			uputc(0>>0);
			uputc(1>>8);
			break;

		/* Query supported bustypes */
		case 0x05:
			uputc(0x06);
			uputc(0x08);
			break;

		/* Sync NOP */
		case 0x10:
			uputc(0x15);
			uputc(0x06);
			break;

		/* Set used bustype */
		case 0x12:
			{
				uint8_t bt = ugetc();
				uputc((bt == 0x08) ? 0x06 : 0x15);
			}
			break;

		/* Perform SPI operation */
		case 0x13:
			{
				int slen = ugetc()|(ugetc()<<8)|(ugetc()<<16);
				int rlen = ugetc()|(ugetc()<<8)|(ugetc()<<16);

				/*while (slen--) ugetc();
				uputc(0x06);
				while (rlen--) uputc(0x00);*/

				static volatile uint8_t data[0x10000]; // super unoptimized

				#if 0
				if ((slen + rlen) <= 8) {
					/* send&receive fully fits in the command regs */

					for (int i = 0; i < slen; i++) data[i] = ugetc();

					REG32(0x50800010) = (data[0]<<24)|(data[1]<<16)|(data[2]<<8)|data[3];
					REG32(0x50800014) = (data[4]<<24)|(data[5]<<16)|(data[6]<<8)|data[7];

					REG32(0x5080000C) = (((slen+rlen)*8)<<8)|(0<<4)|1;
					while (!(REG32(0x50800004) & 1)); REG32(0x50800004) = 1;

					for (int i = 0; i < 8; i++) // voodoo magic !!!
						data[i] = REG32(0x50800018 + ((i >> 2) * 4)) >> (8 * (3 - (i & 3)));

					uputc(0x06);
					for (int i = 0; i < rlen; i++) uputc(data[slen+i]);
				} else
				#endif
				if ((slen <= 8) && (rlen <= 0xffff)) {
					/* Sends a command, receives a data block */

					for (int i = 0; i < slen; i++) data[i] = ugetc();

					REG32(0x50800010) = (data[0]<<24)|(data[1]<<16)|(data[2]<<8)|data[3];
					REG32(0x50800014) = (data[4]<<24)|(data[5]<<16)|(data[6]<<8)|data[7];
					REG32(0x50800020) = (uint32_t)data;
					REG32(0x5080000C) = (((rlen+0xf)&~0xf)<<16)|((slen*8)<<8)|(0<<4)|1;
					while (!(REG32(0x50800004) & 1)); REG32(0x50800004) = 1;

					uputc(0x06);
					for (int i = 0; i < rlen; i++) uputc(data[i]);

				} else if ((slen <= (0xffff+8)) && (rlen == 0)) {
					/* Sends a command and a data block */

					while (slen--) ugetc();
					uputc(0x15);
				} else {
					/* Nah, that's not possible then */
					while (slen--) ugetc();
					uputc(0x15);
				}
			}
			break;
		}
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
