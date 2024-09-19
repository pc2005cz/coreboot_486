//TODO clean


/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <cpu/486/sp_init.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdint.h>
#include <string.h>
#include <device/mmio.h>	//for read32p

#include <cbmem.h>

#include "reg_init.h"


///////////// host bridge
///////////// host bridge
///////////// host bridge


/*
static void northbridge_init(struct device *dev)
{
	//NOTICE after UMC hb_init (after PCI enum))
	printk(BIOS_INFO, "*** UMC northbridge_init\n");
}*/

static struct device_operations hb_operations = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	// .acpi_fill_ssdt		= generate_cpu_entries,
	.ops_pci		= &pci_dev_ops_pci,
};

static const struct pci_driver hb_driver __pci_driver = {
	.ops	= &hb_operations,
	.vendor	= PCI_VID_UMC,
	.device	= PCI_DID_UMC_UM8881F,
};


///////////// domain
///////////// domain
///////////// domain


/* Report the regions. */

static void umc_domain_read_resources(struct device *dev)
{
	printk(BIOS_INFO, "***** UMC domain read resources\n");

	/////TODO TODO TODO TODO cbmem_top generate from registers?


	pci_domain_read_resources(dev);

	int idx = 4;

	//ISA IO disable from PCI? (VGA IO should be special)
	//io, fixed, assigned
	fixed_io_from_to_flags(
		dev,
		idx++,
		0,
		0x1000,
		IORESOURCE_RESERVE
	);


	//low mem
	//cacheable, stored, mem, fixed, assigned
	ram_from_to(dev, idx++, 0, 0xa0000ULL);	//0xa0000

#if 1
	//VGA
	//fixed, assigned, mem
	fixed_mem_from_to_flags(
		dev,
		idx++,
		0xa0000ULL,
		0xc0000ULL,
		IORESOURCE_STORED
	);

	//BIOSes
	//fixed, assigned, mem
	fixed_mem_from_to_flags(
		dev,
		idx++,
		0xc0000ULL,
		0x100000ULL,
		IORESOURCE_RESERVE /*| IORESOURCE_READONLY*/
		// 0 /* | IORESOURCE_RESERVE*/  /*| IORESOURCE_READONLY*/

#if 0
		IORESOURCE_CACHEABLE | IORESOURCE_STORED
		| IORESOURCE_READONLY /*test*/
#endif
	);
#endif

	// printk(BIOS_INFO, "CBMEM TOP !!! 0x%p\n", cbmem_top());

	//after bios to end of the RAM
	//cacheable, stored, mem, fixed, assigned
	ram_from_to(dev, idx++, 0x100000ULL, (unsigned long)cbmem_top());

#if 0	//TODO must be NOT used
	//possible PCI area, end of the RAM to end of the wire
	//fixed, assigned, mem
	fixed_mem_from_to_flags(
		dev,
		idx++,
		(unsigned long)cbmem_top(),
		0xfffe0000ULL,
		IORESOURCE_RESERVE
	);
#endif

#if 0
	//TODO does UMC have entire 4GB space? finali has unwired address wires
	//fixed, assigned, mem
	fixed_mem_from_to_flags(
		dev,
		idx++,
		0x08000000ULL,
		0xf8000000ULL,
		IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED
	);
#endif

#if 0
	//PCI area to BOOT segment
	//fixed, assigned, mem
	fixed_mem_from_to_flags(
		dev,
		idx++,
		0x08000000ULL,
		0xfffe0000ULL,
		IORESOURCE_RESERVE | IORESOURCE_PREFETCH
	);

#endif

#if 1
	//C/D/E/F may be accessible at the end of space
	//don't put PCI regions there

	//BOOT segment
	//fixed, assigned, mem
	fixed_mem_from_to_flags(
		dev,
		idx++,
		0xfffc0000ULL,
		0x100000000ULL,
		IORESOURCE_RESERVE | IORESOURCE_STORED
	);
#endif
}

/*
static void hb_init(struct device *dev)
{
	//NOTICE after UMC mp_init_cpus (after PCI enumeration)
	printk(BIOS_INFO, "*** UMC hb_init\n");
}*/


struct device_operations um8881_pci_domain_ops = {
	.read_resources		= umc_domain_read_resources,
	.set_resources		= pci_domain_set_resources,
	.scan_bus		= pci_domain_scan_bus,

	// .init			= hb_init,
// #if CONFIG(HAVE_ACPI_TABLES)
	// .write_acpi_tables	= northbridge_write_acpi_tables,
	// .acpi_name			= northbridge_acpi_name,
	// .acpi_fill_ssdt   = ssdt_set_above_4g_pci,
};

///////////////////////////


#if 0
static void _memdump(uintptr_t ptr)
{
	uint8_t *mem = (uint8_t *)ptr;
	printk(BIOS_DEBUG, "%p:\n", mem);
	for (unsigned idx=0; idx<128; idx++) {
		if ((idx % 16) == 0) {
			printk(BIOS_DEBUG, "%p: ", &mem[idx]);
		}
		printk(BIOS_DEBUG, "%02hhx ", mem[idx]);
		if (((idx % 16) == 15) || (idx == 127)) {
			printk(BIOS_DEBUG, "\n");
		}
	}
}
#endif


void mp_init_cpus(struct bus *cpu_bus)
{
	//NOTICE after PCI enumeration
	printk(BIOS_INFO, "*** UMC mp_init_cpus\n");

	// const struct mp_ops *ops = &mp_ops_no_smm;

	//NO SMM pc2005
	// sp_init_with_smm(cpu_bus, ops);

#if 0
	void* current_address;

	// asm volatile("rdtsc" :::);
	asm volatile(
		"call get_eip\n\t"
		"get_eip:\n\t"
		"pop %%eax\n\t"
		"movl %%eax, %0\n\t"
		: "=r" (current_address)::"%eax"
	);
	printk(BIOS_INFO, "***INIT CPU EIP:%p\n", current_address);
#endif

//TODO EXPERIMENTAL
#if 0
	asm volatile("nop" :::);
	asm volatile("nop" :::);
	asm volatile("nop" :::);
	asm volatile("nop" :::);
	asm volatile("nop" :::);
	asm volatile("nop" :::);
	asm volatile("nop" :::);
	asm volatile("nop" :::);
	asm volatile("nop" :::);
	asm volatile("rdtsc" :::);

		//read twice L2 size
		for (uintptr_t addr = 0x200000; addr < 0x200000 + 256*KiB*2; addr+=4) {
			// if ((addr % 0x10000) == 0) {
			// 	printk(BIOS_DEBUG, "%6lx ", addr);
			// }

			read32p(addr);
			// write32p(addr, addr);
		}
		// printk(BIOS_DEBUG, "\n");
#endif

//disable forced L2 hit
// pci_update_config8(HOST_BRIDGE, HB5A_MISC_0, ~0x40, 0);




	//ramstage disable original ROM, do shadow

	//TODO TODO TODO TODO TODO TODO test RAM/ROM/ISA on E/F segments

	//TODO make seg E/F shadow+writeable ?

	//no cacheable bios
	pci_update_config8(HOST_BRIDGE, HB55_SHADOW_CFG, ~0x3f, 0);

	//CSEG shadow
	// printk(BIOS_INFO, "++++RAM++++ segment Call use RAM shadow\n");
	//enable will also disable VGA (unless it's bios is copied over)
	// pci_update_config8(HOST_BRIDGE, HB54_SHADOW_EN, ~0, 0xe);

	//ESEG shadow, enable Eseg read
	// printk(BIOS_INFO, "++++RAM++++ segment E use RAM shadow\n");
	// pci_update_config8(HOST_BRIDGE, HB54_SHADOW_EN, ~0, 1);

	//FSEG shadow + global shadow enable
	// printk(BIOS_INFO, "++++RAM++++ segment F use RAM shadow\n");
	// pci_update_config8(HOST_BRIDGE, HB55_SHADOW_CFG, ~0x40, 0x80);

	//enable write protect
	// pci_update_config8(HOST_BRIDGE, HB55_SHADOW_CFG, ~0, 0x40);

	//disable write protect
	pci_update_config8(HOST_BRIDGE, HB55_SHADOW_CFG, ~0x40, 0);

#if 0
	//TODO
	//make bios cacheable
	pci_update_config8(HOST_BRIDGE, HB55_SHADOW_CFG, ~0, 0x20);

	printk(BIOS_INFO, "TODO TEMP force enable L2\n");
	pci_update_config8(HOST_BRIDGE, HB50_L2_CFG_0, (u8)~0x87, 0x03);
	// pci_update_config8(HOST_BRIDGE, HB5A_MISC_0, ~0, 0x4f);

	for (uintptr_t addr = 0; addr < 256*KiB*2; addr+=4) {
		if ((addr % 0x10000) == 0) {
			printk(BIOS_DEBUG, "%6lx ", addr);
		}

		read32p(addr);
		// write32p(addr, addr);
	}
	printk(BIOS_DEBUG, "\n");

	pci_update_config8(HOST_BRIDGE, HB50_L2_CFG_0, ~0, 0x80);

	// pci_update_config8(HOST_BRIDGE, HB5A_MISC_0, ~0x4f, 0);
#endif


	//void *write_tables(void)

#if 0
	printk(BIOS_DEBUG, "dump HB:\n");
	for (unsigned idx=0x50;idx<0x63;idx++) {
		if ((idx % 16) == 0) {
			printk(BIOS_DEBUG, "%02x: ", idx);
		}
		printk(BIOS_DEBUG, "%02hhx ",
		       pci_read_config8(HOST_BRIDGE, idx)
		);
		if (((idx % 16) == 15) || (idx == (0x63-1))) {
			printk(BIOS_DEBUG, "\n");
		}
	}

	printk(BIOS_DEBUG, "dump IBC:\n");
	for (unsigned idx=0x40;idx<0xb0;idx++) {
		if ((idx % 16) == 0) {
			printk(BIOS_DEBUG, "%02x: ", idx);
		}
		printk(BIOS_DEBUG, "%02hhx ",
		       pci_read_config8(ISA_BRIDGE, idx)
		);
		if (((idx % 16) == 15) || (idx == (0xb0-1))) {
			printk(BIOS_DEBUG, "\n");
		}
	}
#endif



	//TODO make seg E F read only? (probably not yet)
}

struct device_operations um8881_cpu_bus_ops = {
	.read_resources	= noop_read_resources,
	.set_resources	= noop_set_resources,
	.init		= mp_cpu_bus_init,
};

// static void enable_dev(struct device *dev)
// {
// 	/* Set the operations if it is a special bus type */
// 	if (dev->path.type == DEVICE_PATH_DOMAIN) {
// 		dev->ops = &pci_domain_ops;
// 	}
// 	else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
// 		dev->ops = &cpu_bus_ops;
// 	}
// }

struct chip_operations northbridge_umc_um8881_ops = {
	CHIP_NAME("UMC UM8881 Northbridge (HB4 Super Energy Star Green)")
	// .enable_dev = enable_dev,
};
