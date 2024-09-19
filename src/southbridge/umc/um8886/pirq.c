/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/pirq_routing.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>

#include <northbridge/umc/um8881/reg_init.h>

//route[] index is INTA/B/C/D, route[] value is IRQ
#if CONFIG(PIRQ_ROUTE) && CONFIG(GENERATE_PIRQ_TABLE)
void pirq_assign_irqs(const unsigned char route[CONFIG_MAX_PIRQ_LINKS])
{
	//TODO CONFIG_MAX_PIRQ_LINKS != 4 ?

	// printk(BIOS_DEBUG, "PIRQ assign R[0]=%x\n", route[0]);
	// printk(BIOS_DEBUG, "PIRQ assign R[1]=%x\n", route[1]);
	// printk(BIOS_DEBUG, "PIRQ assign R[2]=%x\n", route[2]);
	// printk(BIOS_DEBUG, "PIRQ assign R[3]=%x\n", route[3]);

	struct device * const  pdev = dev_find_device(
		PCI_VID_UMC, PCI_DID_UMC_UM8886A, 0
	);
	if (!pdev) {
		printk(BIOS_ERR, "UMC IBC not found, cannot assign PIRQ!\n");
		return;
	}

	pci_write_config8(
		pdev,
		IBC43_INTAB_IRQ,
		((route[0] & 0xf) << 4) | (route[1] & 0xf)
	);

	pci_write_config8(
		pdev,
		IBC44_INTCD_IRQ,
		((route[2] & 0xf) << 4) | (route[3] & 0xf)
	);
}
#endif
