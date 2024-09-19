/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <arch/romstage.h>
#include <device/pci_ops.h>
#include <cbmem.h>
#include <commonlib/helpers.h>
#include <program_loading.h>

#include "reg_init.h"

unsigned get_bank_size_mb(unsigned char slot);
bool get_bank_double_sided(unsigned char slot);


//NOTICE RAM _MUST_ be configured by now
uintptr_t cbmem_top_chipset(void)
{
	//TODO debug, forced max value
	// ram_size_mb = 128*MiB;

	//load size from already configured chipset
	u8 val;

	//get RAM size register
	val = pci_read_config8(HOST_BRIDGE, HB52_DRAM_CFG_0);

	unsigned ram_size_mb = 0;

	//lower region
	if ((val & 0x07) != 0) {
		ram_size_mb += (1 << (val & 0x07));
	}

	//upper region
	val = val >> 4;

	if ((val & 0x07) != 0) {
		ram_size_mb += (1 << (val & 0x07));
	}

	return ram_size_mb*MiB;
}


//NOTICE gets physical location of slot, find bank mapping
bool get_bank_double_sided(unsigned char slot)
{
	if (slot < 4) {
		return false;
	}

	const u8 r52 = pci_read_config8(HOST_BRIDGE, HB52_DRAM_CFG_0);
	const u8 r53 = pci_read_config8(HOST_BRIDGE, HB53_DRAM_CFG_1);

	const bool swap_regions = !!(r52 & 8);
	bool swap_banks = false;
	bool two_banks = false;
	bool double_sided = false;

	switch (slot) {
	case 0:
	case 1:
		//use upper region : lower region
		two_banks = !!(swap_regions ? (r53 & 0x4) : (r53 & 0x1));
		double_sided = !!(swap_regions ? (r53 & 0x8) : (r53 & 0x2));

		swap_banks = !!(r53 & 0x10);	//0<->1
		break;
	case 2:
	case 3:
		two_banks = !!(swap_regions ? (r53 & 0x1) : (r53 & 0x4));
		double_sided = !!(swap_regions ? (r53 & 0x2) : (r53 & 0x8));

		swap_banks = !!(r53 & 0x20);	//2<->3
		break;
	default:
		return false;
	}

	if (two_banks) {
		return double_sided;
	}

	//single bank

	switch (slot) {
	case 0:
	case 2:
		if (swap_banks) {
			//0,2 is upper
			return false;
		} else {
			//0,2 is lower
			return double_sided;
		}
		break;
	case 1:
	case 3:
		if (swap_banks) {
			//1,3 is lower
			return double_sided;
		} else {
			//1,3 is upper
			return false;
		}
		break;
	default:
		return false;
	}

	return false;
}


//NOTICE gets physical location of slot, find bank mapping
unsigned get_bank_size_mb(unsigned char slot)
{
	const u8 r52 = pci_read_config8(HOST_BRIDGE, HB52_DRAM_CFG_0);
	const u8 r53 = pci_read_config8(HOST_BRIDGE, HB53_DRAM_CFG_1);

	const bool swap_regions = !!(r52 & 8);
	bool swap_banks = false;
	bool two_banks = false;

	u8 region_size = 0;

	switch (slot) {
	case 0:
	case 1:
		//use upper region : lower region
		region_size = (swap_regions ? (r52>>4) : r52) & 7;
		two_banks = !!(swap_regions ? (r53 & 0x4) : (r53 & 0x1));
		swap_banks = !!(r53 & 0x10);	//0<->1
		break;
	case 2:
	case 3:
		region_size = (swap_regions ? r52 : (r52>>4)) & 7;
		two_banks = !!(swap_regions ? (r53 & 0x1) : (r53 & 0x4));
		swap_banks = !!(r53 & 0x20);	//2<->3
		break;
	default:
		return 0;
	}

	if (region_size == 0) {
		return 0;
	}

	if (two_banks) {
		return (1 << region_size)/2;
	}

	//single bank

	switch (slot) {
	case 0:
	case 2:
		if (swap_banks) {
			//0,2 is upper
			return 0;
		} else {
			//0,2 is lower
			return 1 << region_size;
		}
		break;
	case 1:
	case 3:
		if (swap_banks) {
			//1,3 is lower
			return 1 << region_size;
		} else {
			//1,3 is upper
			return 0;
		}
		break;
	default:
		return 0;
	}

	return 0;
}
