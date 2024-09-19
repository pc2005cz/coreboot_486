/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <device/pci_ops.h>
#include <device/device.h>
#include <device/pci.h>
#include <console/console.h>

#include <northbridge/umc/um8881/reg_init.h>

void um8886_early_init(void);


static const struct umc_cfg_entry ibc_init_table[] = {
	{0x04,			0xff,	0x0f},	//enable IO MEM BM SPEC

	{IBC40_CFG_0,		0xff,	0x04},
	{IBC41_CFG_1,		0xff,	0x42},
	{0x42,			0xff,	0x08},
	{IBC43_INTAB_IRQ,	0xff,	0xff},
	{IBC44_INTCD_IRQ,	0xff,	0xff},
	{IBC45_CFG_2,		0xff,	0x04},	//TODO not set in vanilla with PS/2


	//ps/2 mouse enabled
	{IBC45_CFG_2,		0x01,	0x01},
	{IBC40_CFG_0,		0x10,	0x10},


	{IBC46_INT_EN,		0xff,	0x10 | 0xf},
	// {IBC46_INT_EN,		0xff,	0x10},	//INTx disable TODO TODO TODO TODO


	{IBC47_INT_LEVEL,	0xff,	0x7f},	//0xf pci is level triggered

	//IBC50_PCI_VIDEO
	// {IBC51_MEM_SIZE,	0xff,	0x40},	//set bit 1 after filling segment E/F
	{IBC56_CLOCK,		0xff,	0x01},
	{IBC57_ROM_DECODE,	0xff,	0x30},

	{IBC70_MONITOR_0,	0xff,	0x6f},
	{IBC71_MONITOR_1,	0xff,	0xff},
	{IBC72_MONITOR_2,	0xff,	0x01},

	{IBC90_WAKEUP_0,	0xff,	0x02},
	{IBC91_WAKEUP_1,	0xff,	0x00},
	{0x92,			0xff,	0x1f},

	{IBCA0_CFG_4,		0xff,	0x00},
	{IBCA1_CFG_5,		0xff,	0x80},
	{IBCA2_SMM_EVENT,	0xff,	0x00},
	{0xa3,			0xff,	0x00},
	{IBCA4_CLOCK_RATIO,	0xff,	0x81},
	{0xa5,			0xff,	0x00},
	{0xa6,			0xff,	0x00},
	{IBCA7_UNKNOWN_0,	0xff,	0xef},
	{IBCA8_CFG_6,		0xff,	0x28},
};


void um8886_early_init(void)
{
	//NOTICE before switching to ramstage
	printk(BIOS_DEBUG, "UMC IBC early init\n");

	//init IBC registers
	umc_registers_init(
		ISA_BRIDGE,
		ibc_init_table,
		ARRAY_SIZE(ibc_init_table)
	);
	//TODO make some configurable from CMOS
}
