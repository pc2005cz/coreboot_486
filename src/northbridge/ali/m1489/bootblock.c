/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/helpers.h>	//ARRAY_SIZE


#include <console/console.h>
#include <arch/bootblock.h>

#include "regs.h"


#include <console/streams.h>	//temp


//TODO
static const struct ali_cfg_entry bootblock_table[] = {
	{0x3, 0xc5},	//unlock

	{0x10, 0xff},	//disable SIMM
	{0x11, 0xff},	//disable SIMM

	{0x12, 0x81},	//0: Exxx ROM
	{0x13, 0xf0},	//C outside RAM shadow, D in RAM shadow

	// {0x14, 0x9f},
	{0x14, 0x80},	//no shadow (no RAM yet)
	{0x15, 0x50},	//TODO check point

	//0x16, 0x17 (cache) later

	{0x19, 0x44},
	{0x1a, 0x5f},
	{0x1b, 0x45},
	{0x1c, 0x00},

	// {0x1d, 0x00},	//not documented but 0?
	{0x1e, 0x08},	//8?? must be kept low???
	{0x20, 0x2c},
	{0x21, 0x36},	//5: "1" ROM access at 0xffffffff
	{0x22, 0x1d},	//bit 1 should be "1" ?

	{0x25, 0x80},	//gp mem
	{0x26, 0x00},
	{0x27, 0x00},
	{0x28, 0x04},

	{0x29, 0x32},
	{0x2a, 0x45},
	{0x2b, 0xd7},

	{0x30, 0x10},
	{0x31, 0x00},
	{0x32, 0x00},
	{0x33, 0x00},
	{0x34, 0x00},

	{0x35, 0x80},
	{0x36, 0x00},
	{0x37, 0x00},
	{0x38, 0x00},

	{0x39, 0x00},
	{0x3a, 0x00},
	{0x3b, 0x00},
	{0x3c, 0x00},

	{0x3d, 0x05},
	//3e: ??, 3f: 0x70 shadow
	{0x40, 0x2b},
	//0x41??

	{0x42, 0x00},
	{0x43, 0x00},

	{0x44, 0x1f},	//!!
	{0x45, 0x80},	//system reset asserted

};


static void ali_early_init(void)
{
	printk(BIOS_DEBUG, "init northbridge ======== \n");
/*
	uint32_t esp;

	asm volatile(
		"movl %%esp, %0\n"
		: "=r" (esp)
		:
		:
	);


	//0x00000f50

	console_tx_byte(esp & 0xff);
	console_tx_byte((esp>>8) & 0xff);
	console_tx_byte((esp>>16) & 0xff);
	console_tx_byte((esp>>24) & 0xff);

	console_tx_byte('$');
	*/

// uint8_t *mem;

// before is 0, after 0x81, CPU restart will leave regs enabled

// printk(BIOS_DEBUG, "BEF %x\n", cfg_read(0x12));
//
// // uint8_t *mem = (uint8_t *)0xfffe0000;
// uint8_t *mem = (uint8_t *)0x000e0000;
//
// for (unsigned idx=0;idx<0x100;idx++) {
// 	printk(BIOS_DEBUG, "%02x ", mem[idx]);
// }
//
// printk(BIOS_DEBUG, "\n");
//

	// cfg_write(0x10, 0xff);	//ram disabled
	// cfg_write(0x11, 0xff);	//ram disabled

#if 0
	console_tx_byte('>');
	console_tx_byte('>');
	console_tx_byte('>');
	console_tx_byte('>');

	uint8_t *val = 0;
	for (unsigned idx=0;idx<0x3000;idx++) {
		console_tx_byte(val[idx]);
	}

	console_tx_byte('<');
	console_tx_byte('<');
	console_tx_byte('<');
	console_tx_byte('<');
#endif

	// cfg_unlock();

	// printk(BIOS_DEBUG, "B1\n");

	for (unsigned idx=0;idx<ARRAY_SIZE(bootblock_table);idx++) {
		// volatile u8 xxx;
		// printk(BIOS_DEBUG, "C1\n");

		// xxx = bootblock_table[idx].val;
		// printk(BIOS_DEBUG, "C2%%\n");

		// asm volatile("rdtsc" :::);

#if 0
		printk(BIOS_DEBUG, " %02x <= %02x\n",
			bootblock_table[idx].idx, bootblock_table[idx].val
		);
#endif
		// asm volatile("rdtsc" :::);

		// printk(BIOS_DEBUG, "C3\n");

		cfg_write(bootblock_table[idx].idx, bootblock_table[idx].val);

		// printk(BIOS_DEBUG, "C4\n");
	}


	// printk(BIOS_DEBUG, "AFT %x\n", cfg_read(0x12));
 //
	// // mem = (uint8_t *)0xfffe0000;
	// mem = (uint8_t *)0x000e0000;
 //
	// for (unsigned idx=0;idx<0x100;idx++) {
	// 	printk(BIOS_DEBUG, "%02x ", mem[idx]);
	// }
 //
	// printk(BIOS_DEBUG, "\n");

#if 0
	printk(BIOS_DEBUG, "LOW\n");

	mem = (uint8_t *)0xf0000;
	// mem = (uint8_t *)0x000e0000;

	for (unsigned idx=0;idx<0x100;idx++) {
		printk(BIOS_DEBUG, "%02x ", mem[idx]);
	}

	printk(BIOS_DEBUG, "\n");


	printk(BIOS_DEBUG, "HIGH\n");

	mem = (uint8_t *)0xffff0000;

	for (unsigned idx=0;idx<0x100;idx++) {
		printk(BIOS_DEBUG, "%02x ", mem[idx]);
	}

	printk(BIOS_DEBUG, "\n");
#endif

}


void bootblock_early_northbridge_init(void)
{
	outb(1, 0xcfb);		//use PCI config mechanism 1

	//enable BIOS access
	ali_early_init();

	// printk(BIOS_DEBUG, "D1\n");

}
