/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * A generic romstage (pre-ram) driver for various Winbond Super I/O chips.
 *
 *
 *
 * # portyr 0x250 8 0x89
 * # portyr 0x251 8 0x0
 * # portyr 0x252
 * 0xd0
 *
 *# portyr 0x251 8 0x1
 * # portyr 0x252
 * 0x2c
 *
 * # portyr 0x251 8 0x2
 * # portyr 0x252
 * 0xff
 * # portyr 0x251 8 0x3
 * # portyr 0x252
 * 0x90
 * # portyr 0x251 8 0x4
 * # portyr 0x252
 * 0x0
 * # portyr 0x251 8 0x5
 * # portyr 0x252
 * 0x0
 * # portyr 0x251 8 0x6
 * # portyr 0x252
 * 0x0
 * # portyr 0x251 8 0x7
 * # portyr 0x252
 * 0x0
 * # portyr 0x251 8 0x8
 * # portyr 0x252
 * 0x0
 * # portyr 0x251 8 0x9
 * # portyr 0x252
 * 0x8
 *# portyr 0x251 8 0xa
 * # portyr 0x252
 * 0x1f
 * # portyr 0x251 8 0xc
 * # portyr 0x252
 * 0xff
 * # portyr 0x251 8 0xd
 * # portyr 0x252
 * 0xff
 * # portyr 0x251 8 0xe
 * # portyr 0x252
 * 0x0
 * # portyr 0x251 8 0xf
 * # portyr 0x252
 * 0xff
 *
 * wrap around?
 * # portyr 0x251 8 0x10
 * # portyr 0x252
 * 0xd0
 * # portyr 0x251 8 0x11 ; portyr 0x252
 * 0x2c
 * # portyr 0x251 8 0x12 ; portyr 0x252
 * 0xff
 * # portyr 0x251 8 0x13 ; portyr 0x252
 * 0x90
 * # portyr 0x251 8 0x14 ; portyr 0x252
 * 0x0
 * # portyr 0x251 8 0x15 ; portyr 0x252
 * 0x0
 *
 * will hang uart
 *portyr 0x251 8 0x16 ; portyr 0x252 8 0xff
 *
 */

#include <arch/io.h>
#include <stdint.h>
#include <commonlib/bsd/helpers.h>


#include "winbond.h"

#define WINBOND_ENTRY_KEY	0x89	//TODO other
#define WINBOND_EXIT_KEY	0x00

#define WINBOND_EFER	0x250	//enable port
#define WINBOND_EFIR	0x251	//index port
#define WINBOND_EFDR	0x252	//data port

static const u8 cfg[] = {
	0x10,
	0x2c,
	0xff,
	0x70,
	0x40,
	0x00,
	0x05,
	0x00,
	0x00,
	0x08,
	0x1f,
};


/* Read from configuration register */
static __always_inline
u8 read_cr(uint8_t cr)
{
	//older revisions ends at 0xa
	if (cr >= 0x16)
		return 0;
	outb(cr, WINBOND_EFIR);
	return inb(WINBOND_EFDR);
}

/* Write to configuration register */
static __always_inline
void write_cr(uint8_t val, uint8_t cr)
{
	if (cr >= 0x16)
		return;
	outb(cr, WINBOND_EFIR);
	outb(val, WINBOND_EFDR);
}

/* Enable configuration: pass entry key '0x87' into index port dev. */
void winbond_enter_conf_state(void)
{
	outb(WINBOND_ENTRY_KEY, WINBOND_EFER);
}

/* Disable configuration: pass exit key '0xAA' into index port dev. */
void winbond_exit_conf_state(void)
{
	outb(WINBOND_EXIT_KEY, WINBOND_EFER);
}

/* Bring up early serial debugging output before the RAM is initialized. */
void winbond_enable_serial(u8 which, u16 iobase)
{
	winbond_enter_conf_state();

	for (unsigned idx=0;idx<ARRAY_SIZE(cfg);idx++) {
		write_cr(cfg[idx], idx);
	}

	winbond_exit_conf_state();
}
