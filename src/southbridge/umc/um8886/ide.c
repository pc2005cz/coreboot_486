/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <console/console.h>
#include <arch/io.h>
#include <stdbool.h>
#include <device/pci_ops.h>
#include <device/pci.h>
#include <device/pci_ids.h>

#include <northbridge/umc/um8881/reg_init.h>


#define TA_PIO0		3
#define TA_PIO1		2
#define TA_PIO2		1
#define TA_PIO3		0
#define TA_PIO4		0

#define TB_PIO0		6
#define TB_PIO1		5
#define TB_PIO2		4
#define TB_PIO3		3
#define TB_PIO4		2

#define TC_PIO0		0xb
#define TC_PIO1		8
#define TC_PIO2		5
#define TC_PIO3		1
#define TC_PIO4		0


#define TIMING_A(pm, ps, sm, ss) (	\
	((pm & 0x3) << 6) |		\
	((ps & 0x3) << 4) |		\
	((sm & 0x3) << 2) |		\
	((ss & 0x3) << 0) )

#define TIMING_B(master, slave) (	\
	((master & 0xf) << 4) |		\
	((slave & 0xf) << 0) )

#define TIMING_C(master, slave) (	\
	((master & 0xf) << 4) |		\
	((slave & 0xf) << 0) )


static const struct umc_cfg_entry ide_table[] = {
	{0x04, 0xff, 0x01 | 4},	//enable PCI comm
	//funguje i pro =5
	// {0x04, 0xff, 0x0f},	//enable PCI comm

	{0x40, 0xff, 0x00},
	//primary and secondary TODO check if sec IO disappear in linux
	{0x41, 0xff, 0x80 /*| 0x40*/ | 0xd},
	{0x42, 0xff, 0x33},

	//NOTICE this won't boot .. check!!!!
	//dual drives?

	//timing A primary (master, slave) + secondary (master, slave)
	{0x43, 0xff, TIMING_A(TA_PIO4, TA_PIO4, TA_PIO4, TA_PIO4)},

	//timing B primary (master, slave)
	{0x44, 0xff, TIMING_B(TB_PIO4, TB_PIO4)},
	//timing B secondary
	{0x45, 0xff, TIMING_B(TB_PIO4, TB_PIO4)},

	//timing C primary
	{0x46, 0xff, TIMING_C(TC_PIO4, TC_PIO4)},
	//timing C secondary
	{0x47, 0xff, TIMING_C(TC_PIO4, TC_PIO4)},

	//unknown, but set
	{0x48, 0xff, 0x55},
	{0x49, 0xff, 0x55},
	{0x4a, 0xff, 0x55},
	{0x4b, 0xff, 0x55},
	{0x4c, 0xff, 0x88},
	{0x4d, 0xff, 0x88},
	{0x4e, 0xff, 0xaa},
	{0x4f, 0xff, 0xaa},

#if 0
	//disable PCI
	{0x04, 0x00, 0x00},
	//disable IDE
	{0x41, 0x00, 0x0d},
#endif
};


static void ide_init(struct device *dev	)
{
	//NOTICE after enumeration
	printk(BIOS_DEBUG, "**enable IDE\n");

	umc_registers_init(IDE_CONTROLLER, ide_table, ARRAY_SIZE(ide_table));
}


static struct device_operations ide_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= ide_init,

	//TODO if requires special enable sequence
	// .enable			= enable_dev,
	.ops_pci		= &pci_dev_ops_pci,
};


static const struct pci_driver um8886bf_ide __pci_driver = {
	.ops	= &ide_ops,
	.vendor	= PCI_VID_UMC,
	.device	= PCI_DID_UMC_UM8886BF,
};
