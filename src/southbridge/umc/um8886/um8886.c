/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <device/pci_ops.h>
#include <device/device.h>
#include <device/pci.h>
#include <console/console.h>
#include <pc80/isa-dma.h>
#include <pc80/i8259.h>

#include <northbridge/umc/um8881/reg_init.h>

static void um8886_init(void *chip_info)
{
	//NOTICE right after ramstage
	printk(BIOS_DEBUG, "UMC IBC init, TODO\n");
	//TODO which can be left to seabios?
	isa_dma_init();
	setup_i8259();
	i8259_configure_irq_trigger(9, 1);
}


struct chip_operations southbridge_umc_um8886_ops = {
	CHIP_NAME("UMC 8886BF IBC (southbridge)")

	//TODO if requires special enable sequence
	// .enable_dev	= enable_dev,
	.init		= um8886_init,
};
