/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <device/pci_ops.h>
#include <cbmem.h>
#include <commonlib/helpers.h>
#include <program_loading.h>

#include "regs.h"

uintptr_t cbmem_top_chipset(void)
{
	// return 64*MiB;	//NOTICE debug max

	const u8 bank_size_mb[9] = {1, 4, 2, 16, 64, 8, 16, 8, 4};

	//NOTICE rest of combinations evaluates as 0 (TODO match with controller behavior)

	uintptr_t sum = 0;
	for (unsigned idx = 0x10; idx<0x12; idx++) {
		u8 reg = cfg_read(idx);

		if ((reg & 0xf) < 9) {
			sum += bank_size_mb[reg & 0xf];
		}

		if (((reg >> 4) & 0xf) < 9) {
			sum += bank_size_mb[(reg >> 4) & 0xf];
		}
	}

	return sum*MiB;
}
