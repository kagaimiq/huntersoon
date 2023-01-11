#include <stdint.h>
#include <string.h>

#define REG32(a)	(*(volatile uint32_t *)(a))

#define SFLASH_base		0x50800000

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

				while (slen > 0) {
					int n = slen; if (n > 8) n = 8;
					slen -= n;

					uint8_t data[8];
					for (int i = 0; i < n; i++) data[i] = ugetc();

					REG32(SFLASH_base + 0x10) = (data[0]<<24)|(data[1]<<16)|(data[2]<<8)|data[3];
					REG32(SFLASH_base + 0x14) = (data[4]<<24)|(data[5]<<16)|(data[6]<<8)|data[7];

					REG32(SFLASH_base + 0x0C) =
						((n * 8) << 8) |		// bit count
						(((slen || rlen) ? 1:0)<<6) |	// keep the CS low if we still have something to send or receive
						(0<<4) |			// CS = 0
						(1<<0);				// 1 = write (but really doesn't matter, as we aren't using DMA)

					while (!(REG32(SFLASH_base + 0x04) & (1<<0)));
					REG32(SFLASH_base + 0x04) = (1<<0); // clear int
				}

				uputc(0x06);

				REG32(SFLASH_base + 0x10) = 0xffffffff;
				REG32(SFLASH_base + 0x14) = 0xffffffff;

				while (rlen > 0) {
					int n = rlen; if (n > 1) n = 1;
					rlen -= n;

					REG32(SFLASH_base + 0x0C) =
						((n * 8) << 8) |	// bit count
						((rlen ? 1:0)<<6) |	// keep the CS low if we still have something to receive
						(0<<4) |		// CS = 0
						(1<<0);			// 1 = write (but really doesn't matter, as we aren't using DMA)

					while (!(REG32(SFLASH_base + 0x04) & (1<<0)));
					REG32(SFLASH_base + 0x04) = (1<<0); // clear int

					uputc(REG32(SFLASH_base + 0x18));
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
