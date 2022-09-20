#include <stdint.h>
#include <string.h>
#include "xprintf.h"



static inline uint32_t reg32_read(uint32_t addr) {
	return *(volatile uint32_t*)addr;
}
static inline void reg32_write(uint32_t addr, uint32_t val) {
	*(volatile uint32_t*)addr = val;
}
static inline void reg32_wsmask(uint32_t addr, int shift, uint32_t mask, uint32_t val) {
	*(volatile uint32_t*)addr =
		(*(volatile uint32_t*)addr & ~(mask << shift)) | ((val & mask) << shift);
}
static inline uint32_t reg32_rsmask(uint32_t addr, int shift, uint32_t mask) {
	return (*(volatile uint32_t*)addr >> shift) & mask;
}



void uputc(int c) {
	while ((reg32_read(0x40040014) & 0x60) != 0x60);
	reg32_write(0x40040000, c);
}

int ugetc(void) {
	while ((reg32_read(0x40040014) & 0x01) != 0x01);
	return reg32_read(0x40040000);
	return 0;
}


// uart
#define PIN_UART_TX			5
#define PIN_UART_RX			6
// lcd
#define PIN_LCD_RES			12	// reset
#define PIN_LCD_CS			13	// chip select
#define PIN_LCD_RS			11	// register select
#define PIN_LCD_SDA			18	// data
#define PIN_LCD_SCL			22	// clock
#define PIN_LCD_BLEN			19	// backlight enable
// accelerometer
#define PIN_ACCEL_SCL			16
#define PIN_ACCEL_SDA			29
#define PIN_ACCEL_IRQ			28
// heart rate
#define PIN_HR_p2			7	// unknown yet
#define PIN_HR_p3			23	// unknown yet
#define PIN_HR_LED			24
// spi flash (external)
#define PIN_SFLASH_CS			26
#define PIN_SFLASH_MISO			8
#define PIN_SFLASH_WP			9
#define PIN_SFLASH_MOSI			10
#define PIN_SFLASH_SCK			14
#define PIN_SFLASH_VDD			15	// yes, the power is sourced from a pin!!
// etc
#define PIN_SWCLK			0	// SWD clock
#define PIN_SWDIO			1	// SWD data
#define PIN_VIBR			3	// vibration motor
#define PIN_DLMODE			4	// download mode
#define PIN_KEY				21	// touch key

//#include "wa.h"



void gpio_set_iomux(int pin, uint8_t mux) {
	if ((pin == 9) || (pin >= 15))
		mux ^= 3;

	reg32_wsmask(0x40000080 + (pin & ~3), (pin & 3) * 8, 0xff, mux);
}

void gpio_set_direction(int pin, int output) {
	uint32_t addr = (pin >= 16) ? 0x41240000 : 0x41200000;
	if (pin >= 16) pin -= 16;

	reg32_write(addr + (output?0x10:0x14), 1 << pin);
}

void gpio_set_level(int pin, int value) {
	uint32_t addr = (pin >= 16) ? 0x41240000 : 0x41200000;
	if (pin >= 16) pin -= 16;

	reg32_wsmask(addr+0x04, pin, 1, !!value);
}

int gpio_get_level(int pin) {
	uint32_t addr = (pin >= 16) ? 0x41240000 : 0x41200000;
	if (pin >= 16) pin -= 16;

	return reg32_rsmask(addr+0x00, pin, 1);
}




void mpif_spi3w_send(const uint8_t *ptr, int len) {
	gpio_set_direction(PIN_LCD_SDA, 1);

	xprintf(".... %x %d\n", ptr, len);

	while (len-- > 0) {
		uint8_t b = *ptr++;

		for (uint8_t m = 0x80; m; m >>= 1) {
			gpio_set_level(PIN_LCD_SDA, b & m);
			gpio_set_level(PIN_LCD_SCL, 1);
			gpio_set_level(PIN_LCD_SCL, 0);
		}
	}
}


void DBI_SendCMD(uint8_t val) {
	gpio_set_level(PIN_LCD_RS, 0); // cmd
	mpif_spi3w_send(&val, 1);
}

void DBI_SendDAT(uint8_t val) {
	gpio_set_level(PIN_LCD_RS, 1); // dat
	mpif_spi3w_send(&val, 1);
}

void DBI_WriteDAT(const uint8_t *ptr, int len) {
	gpio_set_level(PIN_LCD_RS, 1); // dat
	mpif_spi3w_send(ptr, len);
}

void DBI_SetWindow(int x, int y, int w, int h) {
	w = x + w - 1;
	h = y + h - 1;

	DBI_SendCMD(0x2A);
		DBI_SendDAT(x>>8);
		DBI_SendDAT(x>>0);
		DBI_SendDAT(w>>8);
		DBI_SendDAT(w>>0);

	DBI_SendCMD(0x2B);
		DBI_SendDAT(y>>8);
		DBI_SendDAT(y>>0);
		DBI_SendDAT(h>>8);
		DBI_SendDAT(h>>0);
}



/* Startup Function */
__attribute__((noreturn)) void _start(void) {
	extern char _sbss, _ebss;
	memset(&_sbss, 0, &_ebss - &_sbss);

	//------------------

	/*((void (*)())0x8001c30)();

	reg32_wsmask(0x40001004, 0, 0xff06, 0x0206);
	reg32_write(0x4000100e, 1);

	((void (*)())0x8035318)();

	reg32_wsmask(0x400e0000, 13, 1, 1);
	reg32_wsmask(0x400e0000, 24, 1, 1);
	reg32_write(0x400e0020, 0);
	reg32_write(0x400e0058, ~0);

	reg32_wsmask(0x400e0018, 16, 0x3, 0x1);
	reg32_wsmask(0x400e002c, 31, 1, 0);
	reg32_wsmask(0x400e0048, 16, 0x7, 0x4);
	reg32_wsmask(0x400e0050, 12, 0x7, 0x5);
	reg32_wsmask(0x400e0018, 20, 1, 1);

	reg32_wsmask(0x400e0000, 4, 1, 0);

	reg32_wsmask(0x400e0024, 26, 1, 1);
	reg32_wsmask(0x400e0028, 0, 0x30f, 0x100);
	reg32_wsmask(0x400e0000, 0, 0xf, 0x3);
	reg32_wsmask(0x400e0050, 15, 0x3, 0x2);

	reg32_wsmask(0x400e00ec, 2, 1, 1);
	reg32_wsmask(0x400e00ec, 1, 1, 0);*/

	//------------------

	reg32_write(0x40001000+0x64, 0); // GPIO clock

	/*
	reg32_write(0x40001000+0x24, 0x0a001a06); // 115200

	reg32_write(0x40030000+0x0c, 0x00); // lcr
	reg32_write(0x40030000+0x04, 0x00); // icr
	reg32_write(0x40030000+0x08, 0x00); // fsr ----?
	reg32_write(0x40030000+0x0c, 0x80); // lcr: en dll/dlh access
	reg32_write(0x40030000+0x00, 0x01); // dll
	reg32_write(0x40030000+0x04, 0x00); // dlh
	reg32_write(0x40030000+0x0c, 0x03); // lcr: 8n1

	gpio_set_iomux(PIN_UART_RX, 0x0f);
	gpio_set_iomux(PIN_UART_TX, 0x10);*/

	// an empty response packet
	uputc(0x10);
	uputc(0x02);
	uputc(0x10);
	uputc(0x03);
	uputc(0x00);

	for (volatile int i = 1000000; i; i--);

	xdev_out(uputc); //xdev_in(ugetc);
	xputs("\e[1;37;41m---- Konnichiwa, HunterSun! ----\e[0m\n");

	//------------------------------

	gpio_set_level(PIN_LCD_RES,  1);
	gpio_set_level(PIN_LCD_CS,   1);
	gpio_set_level(PIN_LCD_RS,   0);
	gpio_set_level(PIN_LCD_SDA,  0);
	gpio_set_level(PIN_LCD_SCL,  0);
	gpio_set_level(PIN_LCD_BLEN, 0);

	gpio_set_direction(PIN_LCD_RES,  1);
	gpio_set_direction(PIN_LCD_CS,   1);
	gpio_set_direction(PIN_LCD_RS,   1);
	gpio_set_direction(PIN_LCD_SDA,  1);
	gpio_set_direction(PIN_LCD_SCL,  1);
	gpio_set_direction(PIN_LCD_BLEN, 1);

	gpio_set_iomux(PIN_LCD_RES,  0x1c);
	gpio_set_iomux(PIN_LCD_CS,   0x1c);
	gpio_set_iomux(PIN_LCD_RS,   0x1c);
	gpio_set_iomux(PIN_LCD_SDA,  0x1c);
	gpio_set_iomux(PIN_LCD_SCL,  0x1c);
	gpio_set_iomux(PIN_LCD_BLEN, 0x1c);

	//------------------------------

	/* Send init commands */
	const uint8_t initcmds[] = {
		1, 0x01, // soft reset
		1, 0x11, // get out of sleep mode
		1, 0x28, // turn off display

		1, 0xfe,
		1, 0xef,
		2, 0xb3, 0x2b,
		2, 0xb5, 0x01,
		2, 0xb6, 0x11,
		2, 0xac, 0x0b,
		2, 0xb4, 0x21,
		2, 0xb1, 0xc8,
		2, 0xc0, 0xc3,
		3, 0xc6, 0x1f, 0x00,
		3, 0xf8, 0x80, 0x06,
		3, 0xf3, 0x01, 0x03,
		3, 0xf5, 0x48, 0x90,
		2, 0xb2, 0x0d,
		2, 0xea, 0x65,
		3, 0xeb, 0x75, 0x66,
		2, 0xb0, 0x3d,
		2, 0xc2, 0x0d,
		2, 0xc3, 0x4c,
		2, 0xc4, 0x10,
		2, 0xc5, 0x10,
		3, 0xe6, 0x40, 0x27,
		3, 0xe7, 0x60, 0x07,
		2, 0xa3, 0x12,
		15, 0xf0, 0x17, 0x39, 0x1b, 0x4e, 0x95, 0x32, 0x33, 0x00, 0x12, 0x0a, 0x09, 0x12, 0x12, 0x0f,
		15, 0xf1, 0x17, 0x39, 0x1b, 0x4e, 0x65, 0x32, 0x33, 0x00, 0x12, 0x0a, 0x09, 0x0e, 0x0c, 0x0f,
		1, 0xfe,
		1, 0xef,

		1, 0x13, // set normal display
		1, 0x29, // turn on display
		2, 0x36, 0x68, // memory data acc ctrl
		2, 0x3a, 0x05, // pixel format: DBI=RGB565
		//2, 0x3a, 0x06, // pixel format: DBI=RGB666

		0,
	};

	gpio_set_level(PIN_LCD_CS, 0);

	for (const uint8_t *cmd = initcmds; *cmd; ) {
		uint8_t clen = *cmd++;

		DBI_SendCMD(cmd[0]);
		if (cmd[0] == 0x01) {
			for (volatile int i = 500000; i; i--);
		}

		if (clen > 1)
			DBI_WriteDAT((void *)&cmd[1], clen - 1);

		cmd += clen;
	}

	gpio_set_level(PIN_LCD_CS, 1);

	gpio_set_level(PIN_LCD_BLEN, 1);

	//------------------------------

	gpio_set_level(PIN_LCD_CS, 0);

	DBI_SendCMD(0x3A); // pixel fmt
	DBI_SendDAT(0x05); // dbi=>rgb565

	DBI_SetWindow(0,24, 160,80); // window

	DBI_SendCMD(0x2C); // data
	DBI_WriteDAT(dst_bin + 0x400, 160 * 80 * 2);

	gpio_set_level(PIN_LCD_CS, 1);

	for (;;) ;
}




void AssertDieHandler(char *msg, char *file, char *func, int line) {
	xprintf("\e[1;33;41;5m[[ROM ASSERT]] <%s:%d> (%s) %s\e[0m", file,line,func,msg);
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
	AssertDieHandler,			// HS6620D ROM's 0x08000be4 calls it
	NULL,
	NULL,
	NULL,
	NULL,			/* svcall */
	NULL,			/* debugmon */
	(void*)0x66200200,			// required by the HS6620D ROM
	NULL,			/* pendsv */
	NULL,			/* systick */

	NULL,			/*  0: */
	NULL,			/*  1: */
	NULL,			/*  2: */
	NULL,			/*  3: */
	NULL,			/*  4: */
	NULL,			/*  5: */
	NULL,			/*  6: */
	NULL,			/*  7: */
	NULL,			/*  8: UART0 */
	NULL,			/*  9: UART1 */
	NULL,			/* 10: */
	NULL,			/* 11: GPIO0/GPIO1 */
	NULL,			/* 12: */
	NULL,			/* 13: */
	NULL,			/* 14: */
	NULL,			/* 15: */
	NULL,			/* 16: */
	NULL,			/* 17: */
	NULL,			/* 18: */
	NULL,			/* 19: */
	NULL,			/* 20: RTC */
	NULL,			/* 21: */
	NULL,			/* 22: */
	NULL,			/* 23: */
	NULL,			/* 24: */
	NULL,			/* 25: */
	NULL,			/* 26: */
	NULL,			/* 27: */
	NULL,			/* 28: TIMER0 */
	NULL,			/* 29: TIMER1 */
	NULL,			/* 30: TIMER2 */
	NULL,			/* 31: */
	NULL,			/* 32: */
	NULL,			/* 33: */
	NULL,			/* 34: */
	NULL,			/* 35: */
	NULL,			/* 36: */
	NULL,			/* 37: */
	NULL,			/* 38: */
	NULL,			/* 39: */
	NULL,			/* 40: */
	NULL,			/* 41: */
	NULL,			/* 42: */
	NULL,			/* 43: */
	NULL,			/* 44: */
	NULL,			/* 45: */
	NULL,			/* 46: */
	NULL,			/* 47: */
};
