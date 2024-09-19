/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/486/sp_init.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdint.h>
#include <string.h>

#include <cbmem.h>

#include "regs.h"
#include "ide.h"

static void northbridge_init(struct device *dev)
{
	printk(BIOS_SPEW, "Northbridge Init\n");
}

static struct device_operations northbridge_operations = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init	     = northbridge_init,
};

static const struct pci_driver northbridge_driver __pci_driver = {
	.ops = &northbridge_operations,
	.vendor = PCI_VID_AL,
	.device = PCI_DID_AL_M1489,
};


/* Report the regions. */

static void ali_domain_read_resources(struct device *dev)
{
	pci_domain_read_resources(dev);
	int idx = 2;

	//ISA IO disable from PCI? (VGA IO should be special)
	fixed_io_from_to_flags(
		dev,
		idx++,
		0,
		0x1000,
		IORESOURCE_RESERVE
	);


	//low mem
	ram_from_to(dev, idx++, 0, 0xa0000ULL);	//0xa0000

#if 1
	//VGA
	fixed_mem_from_to_flags(
		dev,
		idx++,
		0xa0000ULL,
		0xc0000ULL,
		IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED
	);

	//BIOSes
	fixed_mem_from_to_flags(
		dev,
		idx++,
		0xc0000ULL,
		0x100000ULL,
		IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED
	);
#endif

	//after bios to end of the RAM
	ram_from_to(dev, idx++, 0x100000ULL, (unsigned long)cbmem_top());


	printk(BIOS_INFO, "CBMEM TOP !!! 0x%p\n", cbmem_top());

#if 1
	//possible PCI area, end of the RAM to end of the wire
	fixed_mem_from_to_flags(
		dev,
		idx++,
		(unsigned long)cbmem_top(),
		0x08000000ULL,
		IORESOURCE_RESERVE
	);
#endif

#if 1
	//not wired area in ALI, DO NOT USE!
	fixed_mem_from_to_flags(
		dev,
		idx++,
		0x08000000ULL,
		0xf8000000ULL,
		IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED
	);
#endif
	//TODO test if from 0xff000000 or 0xffff0000

#if 0
	//PCI area to BOOT segment
	fixed_mem_from_to_flags(
		dev,
		idx++,
		0xf8000000ULL,
		0xffff0000ULL,
		IORESOURCE_RESERVE | IORESOURCE_PREFETCH
	);

#endif

#if 1
	//BOOT segment
	fixed_mem_from_to_flags(
		dev,
		idx++,
		0xffff0000ULL,
		0x100000000ULL,
		IORESOURCE_FIXED | IORESOURCE_STORED | IORESOURCE_ASSIGNED
	);
#endif
}


static void ide_init(struct device *dev)
{
	ali_ide_init();
}



struct device_operations pci_domain_ops = {
	// .read_resources		= pci_domain_read_resources,
	.read_resources		= ali_domain_read_resources,
	.set_resources		= pci_domain_set_resources,
	.scan_bus		= pci_domain_scan_bus,

	.init		   = ide_init,
};


#if 0

static void memdump(void)
{

	uint8_t *mem;

	mem = (uint8_t *)0x000e0000;
	printk(BIOS_DEBUG, "%p: %02x %02x %02x +++++ \n",
		mem,
		cfg_read(0x12),
		cfg_read(0x14),
		cfg_read(0x21)
	);
	for (unsigned idx=0;idx<128;idx++) {
		printk(BIOS_DEBUG, "%02x ", mem[idx]);
	}
	printk(BIOS_DEBUG, "\n");

	mem = (uint8_t *)0x000f0000;
	printk(BIOS_DEBUG, "%p:\n", mem);
	for (unsigned idx=0;idx<128;idx++) {
		printk(BIOS_DEBUG, "%02x ", mem[idx]);
	}
	printk(BIOS_DEBUG, "\n");

	mem = (uint8_t *)0xffff0000;
	printk(BIOS_DEBUG, "%p:\n", mem);
	for (unsigned idx=0;idx<128;idx++) {
		printk(BIOS_DEBUG, "%02x ", mem[idx]);
	}
	printk(BIOS_DEBUG, "\n");
}

#endif

void mp_init_cpus(struct bus *cpu_bus)
{
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

	//ramstage disable original ROM, do shadow
	cfg_unlock();

	//TODO TODO TODO TODO TODO TODO test RAM/ROM/ISA on E/F segments



	DIS_SEGMENT_E;	//disable ROME, so we can have shadowE
	EN_SHADOW_EF;	//enable shadow E/F
	EN_ROM_A31;	//enable BOOT ... now we can switch between E and F there


	printk(BIOS_INFO, "set shadow E to copy into\n");

	EN_SWAP_EF;	//map ROME to FFFF/BOOT

	//copy ROME to shadowE
	memcpy((void*)0xe0000, (void*)0xffff0000, 0x10000);


	printk(BIOS_INFO, "shadow F\n");

	DIS_SWAP_EF;	//map ROMF to FFFF/BOOT

	//copy ROMF to shadowF
	memcpy((void*)0xf0000, (void*)0xffff0000, 0x10000);


	#if 0

	cfg_write(0x2b, cfg_read(0x2b) | 0x20);	//swap access

	EN_SEGMENT_E;
	EN_SHADOW_EF;
	EN_ROM_A31;
	memdump();
	//E: empty, F: ff ff 68 81 16 (RAM?), BOOT: 5f 5f 46 4d 41 romE

	EN_SEGMENT_E;
	EN_SHADOW_EF;
	DIS_ROM_A31;
	memdump();
	//E: empty, F: ff ff 68 81 16 (RAM?), BOOT: empty

	EN_SEGMENT_E;
	DIS_SHADOW_EF;
	EN_ROM_A31;
	memdump();
	//E: ROMF  0c 0f 84 cd f2, F: ROME 5f 5f 46 4d 41, BOOT: ROME 5f 5f 46 4d 41

	EN_SEGMENT_E;
	DIS_SHADOW_EF;
	DIS_ROM_A31;
	memdump();
	//E: ROMF 0c 0f 84 cd f2 ff, F: ROME 5f 5f 46 4d 41 50, BOOT: empty

	//////////

	DIS_SEGMENT_E;
	EN_SHADOW_EF;
	EN_ROM_A31;
	memdump();
	//E: empty, F: ff ff 68 81 16 (RAM?), BOOT: ROME 5f 5f 46 4d 41 50 5f

	DIS_SEGMENT_E;
	EN_SHADOW_EF;
	DIS_ROM_A31;
	memdump();
	//E: empty, F: ff ff 68 81 16 (RAMF?), BOOT: empty

	DIS_SEGMENT_E;
	DIS_SHADOW_EF;
	EN_ROM_A31;
	memdump();
	//E: empty, F: ROME, BOOT: ROME

	DIS_SEGMENT_E;
	DIS_SHADOW_EF;
	DIS_ROM_A31;
	memdump();
	//E: empty, F: ROME, BOOT: empty


	//disable EF, make shadow

	//void *write_tables(void)

	// cfg_write(0x12, cfg_read(0x12) & ~7);		//disable E

	// cfg_write(0x14, 0xbf);	//enable shadow
#endif


}

struct device_operations cpu_bus_ops = {
	.read_resources   = noop_read_resources,
	.set_resources    = noop_set_resources,
	.init	     = mp_cpu_bus_init,
};

static void enable_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		dev->ops = &pci_domain_ops;
	}
	else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &cpu_bus_ops;
	}
}

struct chip_operations northbridge_ali_m1489_ops = {
	CHIP_NAME("ALI M1489 Northbridge (FINALI)")
	.enable_dev = enable_dev,
};
