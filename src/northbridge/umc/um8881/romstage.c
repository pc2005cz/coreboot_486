/* SPDX-License-Identifier: GPL-2.0-or-later */

#define __SIMPLE_DEVICE__

#include <console/console.h>
#include <arch/romstage.h>
#include <cbmem.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <string.h>	//for memcpy

#include "reg_init.h"


static const struct umc_cfg_entry hb_init_table[] = {
	//2111
	//NOTICE wriback must be enabled at exit_car after filling
	{HB50_L2_CFG_0,			0x40 | 0x30,	0x30},

	//TODO TEST one freezes benches
	{HB51_L2_CFG_1,			0xff,	0xa4 | 0xc0 /*| 0x30*/},

	// {HB52_DRAM_CFG_0,		0,	0xe6},
	// {HB53_DRAM_CFG_1,		0,	0x8f},
	{HB52_DRAM_CFG_0,		0x00,	0x80},
	{HB53_DRAM_CFG_1,		0x00,	0x80 | 0x40},

	{HB54_SHADOW_EN,		0xff,	0x00},
	{HB55_SHADOW_CFG,		0xff,	0x40},	//readonly
	{HB56_MEMHOLE_BASE,		0xff,	0xff},
	{HB57_MEMHOLE_SIZE,		0xff,	0x0f},
	{HB58_UNKNOWN_0,		0xff,	0xff},
	{HB59_UNKNOWN_1,		0xff,	0x0f},
	{HB5A_MISC_0,			0xff,	0x0d},
	//5b unknown
	{HB5C_SMRAM_0,			0xff,	0xe0},
	{HB5D_SMRAM_1_DRAM_CFG_2,	0xff,	0x30},
	//5e-5f unknown, implemented
	{HB60_SMRAM_2,			0xff,	0x00},

	//TODO TEST one freezes benches
	{HB61_EDO_CFG,			0xff,	0xc4 /*| 0x8*/},	//EDO 3-1-1-1
	{HB62_BURST,			0xff,	0x01},
};


void dram_init(void);
void l2_cache_init(void);
void um8886_early_init(void);


void mainboard_romstage_entry(void)
{
	printk(BIOS_DEBUG, "mainboard romstage\n");

	//init HB registers
	umc_registers_init(
		HOST_BRIDGE,
		hb_init_table,
		ARRAY_SIZE(hb_init_table)
	);
	//TODO make some configurable from CMOS

	//detect dram, L2 disabled (no interference)
	dram_init();

	//L2 cache initialization
	l2_cache_init();

	//NOTICE L2 is configured and enabled now

	// asm volatile("rdtsc" :::);

	printk(BIOS_INFO, "RAM size %lu kiB\n", cbmem_top_chipset()/KiB);

	um8886_early_init();

/*
	printk(BIOS_DEBUG, "Erase high RAM\n");
	for (uintptr_t addr = 128*MiB - 1*MiB; addr < 128*MiB; addr+=4) {
		if ((addr % 0x10000) == 0)
			printk(BIOS_DEBUG, "[%08lx]\n",addr);
		write32p(addr, 0);
	}
*/

	//move raminit to bootblock?

	//disable write protect
	pci_update_config8(HOST_BRIDGE, HB55_SHADOW_CFG, ~0x40, 0);

	//copy ROME to shadowE
	printk(BIOS_INFO, "TODO? fill shadow E\n");
	//NOTICE when disabled, L2 will cause crashes ... may interference with L2 fill in a good way
	memcpy((void*)0xe0000, (void*)0xfffe0000, 0x10000);

	//copy ROMF to shadowF
	printk(BIOS_INFO, "TODO? fill shadow F\n");
	// memcpy((void*)0xf0000, (void*)0xffff0000, 0x10000);

	//CSEG shadow
	printk(BIOS_INFO, "++++ROM++++ segment C use RAM shadow\n");
	pci_update_config8(HOST_BRIDGE, HB54_SHADOW_EN, ~0, 0xe);

	//DSEG shadow
	printk(BIOS_INFO, "++++ROM++++ segment D use RAM shadow\n");
	pci_update_config8(HOST_BRIDGE, HB54_SHADOW_EN, ~0, 0xf0);

	//segE shadow read
	printk(BIOS_INFO, "++++ROM++++ segment E use RAM shadow\n");
	pci_update_config8(HOST_BRIDGE, HB54_SHADOW_EN, ~0, 1);

	//global + segF shadow read
	printk(BIOS_INFO, "++++ROM++++ segment F use RAM shadow\n");
	pci_update_config8(HOST_BRIDGE, HB55_SHADOW_CFG, ~0, 0x80);


	//cmos_get_uint_option

	//NOTICE NOTICE NOTICE NOTICE experiments
	//set if pci video bios installed?
	printk(BIOS_INFO, "Set if PCI VGA is used\n");
	pci_update_config8(ISA_BRIDGE, IBC50_PCI_VIDEO, ~0, 0x80);

	pci_update_config8(ISA_BRIDGE, IBC50_PCI_VIDEO, ~0, 1);

	pci_update_config8(ISA_BRIDGE, IBC51_MEM_SIZE, ~0, 1);
	//NOTICE NOTICE NOTICE NOTICE experiments


#if 0
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
#endif


// printk(BIOS_DEBUG, "cbmem_initialize {\n");

	cbmem_initialize_empty();

	printk(BIOS_DEBUG, "UMC HB romstage exit\n");
}
