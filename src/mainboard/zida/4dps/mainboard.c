/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
// #include <southbridge/sis/85c497/sis85c497.h>

/**
 * Mainboard specific enables.
 *
 * @param chip_info Ignored
 */
static void mainboard_init(void *chip_info)
{
	//write 0x89/0x88 to IO 0x250 (EFER) to enable superio
	//	outb(0x6f, PM_IO_BASE + 0x37);

}

struct chip_operations mainboard_ops = {
	.init = mainboard_init
};
