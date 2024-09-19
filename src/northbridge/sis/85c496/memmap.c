/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <device/pci_ops.h>
#include <cbmem.h>
#include <commonlib/helpers.h>
#include <program_loading.h>

uintptr_t cbmem_top_chipset(void)
{
	//TODO
	return 0x4000000;
}

void fill_postcar_frame(struct postcar_frame *pcf)
{
	uintptr_t top_of_ram;

	/* Cache CBMEM region as WB. */
	top_of_ram = (uintptr_t)cbmem_top();

}
