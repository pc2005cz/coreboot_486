/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/pirq_routing.h>
#include <device/pci_ids.h>


//TODO from working system?

#define INTJ0   (0x10U | 0 | 0) //INT A/1?
#define INTJ1   (0x20U | 8 | 0) //INT B/2?
#define INTJ2   (0x40U | 0 | 1) //INT C/3?
#define INTJ3   (0x80U | 8 | 1) //INT D/4?

static const struct irq_routing_table pb4_irq_routing_table = {
	PIRQ_SIGNATURE,
	PIRQ_VERSION,
	32 + 16 * CONFIG_IRQ_SLOT_COUNT,/* Max. number of devices on the bus */
	0x00,			/* Interrupt router bus */
	(0x00 << 3) | 0x0,	/* Interrupt router device */
	0,			/* IRQs devoted exclusively to PCI usage */
	PCI_VID_AL,		/* Vendor */
	PCI_DID_AL_M1489,	/* Device */
	0,			/* Miniport data */
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, /* u8 rfu[11] */
	0x54,			/* Checksum */
	/* clang-format off */
	{
		/* bus,       dev|fn,   {link, bitmap}, {link, bitmap}, {link, bitmap}, {link, bitmap},  slot, rfu */
		{0x00,(0x03 << 3)|0x0, {{INTJ0, 0xdef8}, {INTJ1, 0xdef8}, {INTJ2, 0xdef8}, {INTJ3, 0xdef8}}, 0x1, 0x0}, /* near cpu */
		{0x00,(0x04 << 3)|0x0, {{INTJ1, 0xdef8}, {INTJ2, 0xdef8}, {INTJ3, 0xdef8}, {INTJ0, 0xdef8}}, 0x2, 0x0}, /* middle */
		{0x00,(0x05 << 3)|0x0, {{INTJ2, 0xdef8}, {INTJ3, 0xdef8}, {INTJ0, 0xdef8}, {INTJ1, 0xdef8}}, 0x3, 0x0}, /* near PISA */
		//TODO add PISA tree
	}
	/* clang-format on */
};

/*
slot 1 nejbliz cpu      00:0?
INTA    p57 - intj0
INTB    p59 - intj1
INTC    p56 - intj2
INTD    p58 - intj3
slot 2 prostredni       00:0?
INTA    p59 - intj1
INTB    p56 - intj2
INTC    p58 - intj3
INTD    p57 - intj0
slot 3 nejbliz PISA     00:0?
INTA    p56 - intj2
INTB    p58 - intj3
INTC    p57 - intj0
INTD    p59 - intj1

PISA
intj0 je zapojenej na side A 14	 p57
intj1 je zapojenej na side B 14	 p59
intj2 je zapojenej na side A 8	  p56
intj3 je zapojenej na side B 10	 p58
*/


unsigned long write_pirq_routing_table(unsigned long addr)
{
	return copy_pirq_routing_table(addr, &pb4_irq_routing_table);
}
