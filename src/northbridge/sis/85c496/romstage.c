/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/romstage.h>
#include <cbmem.h>
// #include <southbridge/intel/i82371eb/i82371eb.h>
// #include <northbridge/intel/i440bx/raminit.h>


void dram_initialize(void);

void mainboard_romstage_entry(void)
{
	// sis85c497_early_init();	//TODO

	// if (cold_reset) {
	if (1) {	//TODO
		dram_initialize();
	}

	cbmem_initialize_empty();
}
