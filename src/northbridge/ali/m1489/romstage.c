/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <arch/romstage.h>
#include <cbmem.h>
#include <device/mmio.h>

#include "regs.h"


static const struct ali_cfg_entry romstage_table[] = {
	{0x3, 0xc5},	//unlock

	//not ram detecct, just force used modules TODO TODO TODO TODO
	// {0x10, 0x66},	//32M double sided SIMM
	// {0x11, 0x66},	//32M double sided SIMM
	// {0x13, 0xf0},	//C outside RAM shadow, D in RAM shadow	TODO???

	// {0x16, 0x07},	//L1 L2 enable
	// {0x17, 0x02},	//L2 cache configuration (how to autodetect?)
	// {0x1d, 0x00},	//not documented but 0?
};


void dram_init(void);
void l2_cache_init(void);


void mainboard_romstage_entry(void)
{
	printk(BIOS_DEBUG, "mb dram_initialize\n");


#if 1

	// if (cold_reset) {
	if (1) {	//TODO
		for (unsigned idx=0;idx<ARRAY_SIZE(romstage_table);idx++) {
			cfg_write(romstage_table[idx].idx, romstage_table[idx].val);
		}

		l2_cache_init();

		//NOTICE L2 configured but not enabled

		dram_init();

		//NOTICE DRAM enabled

		// asm volatile("rdtsc" :::);
		// asm volatile("rdtsc" :::);
		// asm volatile("rdtsc" :::);
		// asm volatile("rdtsc" :::);

		printk(BIOS_DEBUG, "RAM size %lu kiB\n", cbmem_top_chipset() >> 10);

		//TODO originally fill was wrapped with force miss/nondirty
		//enable L2
		cfg_modify(0x16, 0, 0x02);

		//force miss
		cfg_modify(0x17, 0, 8);

		//TODO global RAM INIT is after this, maybe disable cache?
		//cache maybe poisoned now

		//L2 cache should be invalidated
		//scan max possible size of L2 cache, should fill itself completely
		//cache line is 16 bytes, so 16B steps?
		for (uintptr_t addr = 0; addr < 1*MiB; addr+=16) {
			if ((addr % 0x10000) == 0)
				printk(BIOS_DEBUG, "[%08lx]\n",addr);

			read32p(addr);
		}

		//disable force miss
		cfg_modify(0x17, 8, 0);
	}

#endif
	printk(BIOS_DEBUG, "TODO cbmem_initialize\n");

	cbmem_initialize_empty();

	printk(BIOS_DEBUG, "mb romstage exit\n");
}
