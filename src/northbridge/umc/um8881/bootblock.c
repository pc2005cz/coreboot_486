//TODO clean, unused here

/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/helpers.h>	//ARRAY_SIZE
#include <console/console.h>
#include <arch/bootblock.h>
#include <device/pci_ops.h>

#include "reg_init.h"
// #include <console/streams.h>	//temp

void bootblock_early_northbridge_init(void)
{
	post_code('U');
	post_code('M');
	post_code('C');

	//enable segment E
	pci_update_config8(ISA_BRIDGE, IBC57_ROM_DECODE, ~0, 0x20);

	//disable shadow CDE
	pci_write_config8(HOST_BRIDGE, HB54_SHADOW_EN, 0x00);

	//segment F, central off, disable caches everywhere
	pci_update_config8(HOST_BRIDGE, HB55_SHADOW_CFG, (u8) ~(0x80 | 0x40 | 0x20 | 1), 0x00);

	//bug s ISA targetem

#if 0
	//hack sata/raid
	pci_update_config8(PCI_DEV(0, 5, 0), 0x45, (u8)~0x80, 0);
	pci_write_config8(PCI_DEV(0, 5, 0), 0x0a, 1);
	pci_update_config8(PCI_DEV(0, 5, 0), 0x45, ~0, 0x80);
#endif


#if 0
	printk(BIOS_DEBUG, "memdump\n");
	mem = (uint8_t *)0xffff0000;
	for (unsigned idx=0;idx<0x100;idx++) {
		printk(BIOS_DEBUG, "%02x ", mem[idx]);
	}
	printk(BIOS_DEBUG, "\n");
#endif

}
