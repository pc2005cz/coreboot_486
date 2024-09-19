/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/pirq_routing.h>
#include <device/pci_ids.h>


//TODO from working system?

static const struct irq_routing_table sis_irq_routing_table = {
	PIRQ_SIGNATURE,
	PIRQ_VERSION,
	32 + 16 * CONFIG_IRQ_SLOT_COUNT,/* Max. number of devices on the bus */
	0x00,			/* Interrupt router bus */
	(0x00 << 3) | 0x0,	/* Interrupt router device */
	0,			/* IRQs devoted exclusively to PCI usage */
	PCI_VID_SIS,		/* Vendor */
	PCI_DID_SIS_SIS496,	/* Device */
	0,			/* Miniport data */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
	0x54,			/* Checksum */
	/* clang-format off */
	{
		/* bus,       dev|fn,   {link, bitmap}, {link, bitmap}, {link, bitmap}, {link, bitmap},  slot, rfu */
		{0x00,(0x0b << 3)|0x0, {{0xc0, 0xdef8}, {0xc1, 0xdef8}, {0xc2, 0xdef8}, {0xc3, 0xdef8}}, 0x1, 0x0},
		{0x00,(0x0d << 3)|0x0, {{0xc1, 0xdef8}, {0xc2, 0xdef8}, {0xc3, 0xdef8}, {0xc0, 0xdef8}}, 0x2, 0x0},
		{0x00,(0x0f << 3)|0x0, {{0xc2, 0xdef8}, {0xc3, 0xdef8}, {0xc0, 0xdef8}, {0xc1, 0xdef8}}, 0x3, 0x0},
	}
	/* clang-format on */
};

/*

NOTICE not sure if INTA is pirq0 or pirq3
assume pirq0 = inta (slot 1 directly to regs but most likely it will be viceversa)

INTA = reg 0xc0
INTB = reg 0xc1
INTC = reg 0xc2
INTD = reg 0xc3

slot1.intd	pin86 = pirq3
slot1.intc	pin87 = pirq2
slot1.intb	pin88 = pirq1
slot1.inta	pin89 = pirq0

slot2.intc	pin86
slot2.intb	pin87
slot2.inta	pin88
slot2.intd	pin89

slot3.intb	pin86
slot3.inta	pin87
slot3.intd	pin88
slot3.intc	pin89


*/


unsigned long write_pirq_routing_table(unsigned long addr)
{
	return copy_pirq_routing_table(addr, &sis_irq_routing_table);
}
