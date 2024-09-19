/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci_ops.h>
#include <arch/io.h>


#define PORT_INDEX	0x108
#define PORT_DATA	0x109

#define UM8663_CR0	0xc0
#define UM8663_CR1	0xc1
#define UM8663_CR2	0xc2
#define UM8663_CR3	0xc3
#define UM8663_CR4	0xc4


struct umc_sio_cfg_entry {
	u8 idx;
	u8 val;
};


/*
poweron values
C0: 2f
C1: 9f
C2: 81
C3: ff
C4: ff
*/

//taken from linux
static const struct umc_sio_cfg_entry sio_table[] = {
	{UM8663_CR0, 0x0f},
	{UM8663_CR1, 0x8f},
	// {UM8663_CR1, 0x8f | 0x10},	//0x10 if floppy?
	// {UM8663_CR2, 0x80},	//81
};


/**
 * Mainboard specific enables.
 *
 * @param chip_info Ignored
 */
static void mainboard_init(void *chip_info)
{
	printk(BIOS_DEBUG, "*** mainboard_init\n");


	//TODO into southbridge

#if 0
	for (unsigned idx=0;idx<ARRAY_SIZE(sio_table);idx++) {
		//unlock
		outb(0xaa, PORT_INDEX);

		outb(sio_table[idx].idx, PORT_INDEX);
		printk(BIOS_DEBUG, "%2hhx: %02hhx\n", sio_table[idx].idx, inb(PORT_DATA));

		//lock
		outb(0x55, PORT_INDEX);
	}
#endif

/*
 * c0: 0f
c1: 8f
c2: 81

 * */
	for (unsigned idx=0;idx<ARRAY_SIZE(sio_table);idx++) {
		//unlock
		outb(0xaa, PORT_INDEX);

		// for (volatile unsigned delay=0;delay<1000;delay++) {}

		outb(sio_table[idx].idx, PORT_INDEX);

		// for (volatile unsigned delay=0;delay<1000;delay++) {}

		outb(sio_table[idx].val, PORT_DATA);

		// for (volatile unsigned delay=0;delay<1000;delay++) {}

		//lock
		outb(0x55, PORT_INDEX);
	}

#if 1
	printk(BIOS_DEBUG, "SIO dump:\n");

	for (unsigned idx=0;idx<ARRAY_SIZE(sio_table);idx++) {
		//unlock
		outb(0xaa, PORT_INDEX);

		// must be written immediatelly

		outb(sio_table[idx].idx, PORT_INDEX);
		printk(BIOS_DEBUG, "%2hhx: %02hhx\n", sio_table[idx].idx, inb(PORT_DATA));

		//lock
		outb(0x55, PORT_INDEX);
	}
#endif

}

struct chip_operations mainboard_ops = {
	.init = mainboard_init
};
