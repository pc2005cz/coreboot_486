/* SPDX-License-Identifier: GPL-2.0-or-later */

#define __SIMPLE_DEVICE__

#include <device/pci_ops.h>
#include <console/console.h>	//printk

#include "reg_init.h"

///////////

void umc_registers_init(
	pci_devfn_t dev,
	const struct umc_cfg_entry * reg_table,
	unsigned count
)
{
	for (unsigned idx = 0; idx < count; idx++) {

// printk(BIOS_DEBUG, "reg 0x%02hhx\n", reg_table[idx].reg);

		pci_update_config8(
			dev,
			reg_table[idx].reg,
			~ reg_table[idx].clear,
			reg_table[idx].set
		);
	}
}
