//TODO clean

/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <console/console.h>
#include <device/mmio.h>
#include <program_loading.h>
#include <cbmem.h>
#include <device/pci_ops.h>

#include "reg_init.h"

//NOTICE only in ramstage, rest is weak
void platform_prog_run(struct prog *prog)
{
	//reset BIOS access to default

	printk(BIOS_DEBUG, "ROM access to default\n");

	//enable write protect
	pci_update_config8(HOST_BRIDGE, HB55_SHADOW_CFG, ~0, 0x40);

	///////////////////////

	printk(BIOS_DEBUG, "dump HB:\n");
	for (unsigned idx=0x50;idx<0x63;idx++) {
		if ((idx % 16) == 0) {
			printk(BIOS_DEBUG, "%02x: ", idx);
		}
		printk(BIOS_DEBUG, "%02hhx ",
		       pci_read_config8(HOST_BRIDGE, idx)
		);
		if (((idx % 16) == 15) || (idx == (0x63-1))) {
			printk(BIOS_DEBUG, "\n");
		}
	}

	printk(BIOS_DEBUG, "dump IBC:\n");
	for (unsigned idx=0x40;idx<0xb0;idx++) {
		if ((idx % 16) == 0) {
			printk(BIOS_DEBUG, "%02x: ", idx);
		}
		printk(BIOS_DEBUG, "%02hhx ",
		       pci_read_config8(ISA_BRIDGE, idx)
		);
		if (((idx % 16) == 15) || (idx == (0xb0-1))) {
			printk(BIOS_DEBUG, "\n");
		}
	}

	printk(BIOS_DEBUG, "dump IDE:\n");
	for (unsigned idx=0x40;idx<0x60;idx++) {
		if ((idx % 16) == 0) {
			printk(BIOS_DEBUG, "%02x: ", idx);
		}
		printk(BIOS_DEBUG, "%02hhx ",
		       pci_read_config8(IDE_CONTROLLER, idx)
		);
		if (((idx % 16) == 15) || (idx == (0x60-1))) {
			printk(BIOS_DEBUG, "\n");
		}
	}

#if 0
#define PORT_INDEX	0x108
#define PORT_DATA	0x109

	printk(BIOS_DEBUG, "dump SIO:\n");
	for (unsigned idx=0xc0; idx<0xc3;idx++) {
		u8 tmp;

		//unlock
		outb(0xaa, PORT_INDEX);

		outb(idx, PORT_INDEX);
		tmp = inb(PORT_DATA);

		//lock
		outb(0x55, PORT_INDEX);

		printk(BIOS_DEBUG, "0x%02x: 0x%02hhx\n", idx, tmp);
	}
#endif

	printk(BIOS_DEBUG, "E/L 0x4d0:%02hhx 0x4d1:%02hhx\n", inb(0x4d0), inb(0x4d1));

	printk(BIOS_DEBUG, "PIT 0x40:%02hhx 0x41:%02hhx 0x42:%02hhx 0x43:%02hhx\n",
	       inb(0x40), inb(0x41), inb(0x42), inb(0x43));

	printk(BIOS_DEBUG, "PIC 0x20:%02hhx 0x21:%02hhx 0xa0:%02hhx 0xa1:%02hhx\n",
	       inb(0x20), inb(0x21), inb(0xa0), inb(0xa1));

	// printk(BIOS_DEBUG, "PPI 0x60:%02hhx 0x61:%02hhx 0x62:%02hhx 0x63:%02hhx\n",
	//        inb(0x60), inb(0x61), inb(0x62), inb(0x63));

	printk(BIOS_DEBUG, "0x92:%02hhx\n", inb(0x92));

	// printk(BIOS_DEBUG, "0x70:%02hhx\n", inb(0x70));
	// outb(inb(0x70) | 0x80, 0x70);
	// printk(BIOS_DEBUG, "0x70:%02hhx\n", inb(0x70));

	u32 cr0_val = 0;
	asm volatile(
		"movl %%cr0, %%eax\n\t"
		"movl %%eax, %0\n\t"
		: "=r" (cr0_val)::"%eax"
	);
	printk(BIOS_DEBUG, "CR0:%08x\n", cr0_val);

	// printk(BIOS_DEBUG, "50:%02hhx 5a:%02hhx\n",
	//        pci_read_config8(HOST_BRIDGE, HB50_L2_CFG_0),
	//        pci_read_config8(HOST_BRIDGE, HB5A_MISC_0)
	// );
}
