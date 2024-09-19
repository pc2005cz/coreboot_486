/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/pirq_routing.h>
#include <device/pci_ids.h>

// #define IRQ_MASK 0xdef8
//e.g. uart init dislike being on the same IRQ as PCI (uart IRQ will be disabled in probe)
#define IRQ_MASK 0x8e00

static const struct irq_routing_table mb8433_irq_routing_table = {
	PIRQ_SIGNATURE,
	PIRQ_VERSION,
	32 + 16 * CONFIG_IRQ_SLOT_COUNT,/* Max. number of devices on the bus */
	0x00,			/* Interrupt router bus */
	(0x12 << 3) | 0x0,	/* Interrupt router device */
	0,			/* IRQs devoted exclusively to PCI usage */
	PCI_VID_UMC,		/* Vendor */
	PCI_DID_UMC_UM8886A,	/* Device */	//isa bridge (IMO should be ISA bridge)
	0,			/* Miniport data */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */

	//TODO enable CONFIG_DEBUG_PIRQ to compute checksum
	0x20,			/* Checksum */
	/* clang-format off */
	{
		/* bus,       dev|fn, {link, bitmap}, {link, bitmap}, {link, bitmap}, {link, bitmap},  slot, rfu */
		{0x00,(0x3 << 3)|0x0, {{1, IRQ_MASK}, {2, IRQ_MASK}, {3, IRQ_MASK}, {4, IRQ_MASK}}, 0x1, 0x0}, /* near ISA */
		{0x00,(0x4 << 3)|0x0, {{2, IRQ_MASK}, {3, IRQ_MASK}, {4, IRQ_MASK}, {1, IRQ_MASK}}, 0x2, 0x0}, /* middle */
		{0x00,(0x5 << 3)|0x0, {{3, IRQ_MASK}, {4, IRQ_MASK}, {1, IRQ_MASK}, {2, IRQ_MASK}}, 0x3, 0x0}, /* near RAM */
	}
	/* clang-format on */
};

//TODO disable bits used for incompatible ISA

unsigned long write_pirq_routing_table(unsigned long addr)
{
	return copy_pirq_routing_table(addr, &mb8433_irq_routing_table);
}
