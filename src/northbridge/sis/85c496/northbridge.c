/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/486/sp_init.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdint.h>

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
	.vendor = PCI_VID_SIS,
	.device = PCI_DID_SIS_SIS496,
};


#if 0
//TODO pci_domain_read_resources
static void i440bx_domain_read_resources(struct device *dev)
{
	struct device *mc_dev;
	uint32_t pci_tolm;

	pci_domain_read_resources(dev);

	pci_tolm = find_pci_tolm(dev->link_list);
	mc_dev = dev->link_list->children;
	if (mc_dev) {
		unsigned long tomk, tolmk;
		int idx;

		/* Figure out which areas are/should be occupied by RAM. The
		 * value of the highest DRB denotes the end of the physical
		 * memory (in units of 8MB).
		 */
		tomk = ((unsigned long)pci_read_config8(mc_dev, DRB7));

		/* Convert to KB. */
		tomk *= (8 * 1024);

		printk(BIOS_DEBUG, "Setting RAM size to %ld MB\n", tomk / 1024);

		/* Compute the top of low memory. */
		tolmk = pci_tolm / 1024;

		if (tolmk >= tomk) {
			/* The PCI hole does not overlap the memory. */
			tolmk = tomk;
		}

		/* Report the memory regions. */
		idx = 10;
		ram_resource_kb(dev, idx++, 0, 640);
		ram_resource_kb(dev, idx++, 768, tolmk - 768);
	}
}
#endif

static struct device_operations pci_domain_ops = {
	.read_resources		= pci_domain_read_resources,
	.set_resources		= pci_domain_set_resources,
	.scan_bus		= pci_domain_scan_bus,
};

// static int sis_max_cpus(void)
// {
// 	return 1;
// }

// static const struct mp_ops mp_ops_no_smm = {
// 	.get_cpu_count = sis_max_cpus,
// };

void mp_init_cpus(struct bus *cpu_bus)
{
	// const struct mp_ops *ops = &mp_ops_no_smm;

	// sp_init_with_smm(cpu_bus, ops);
}

static struct device_operations cpu_bus_ops = {
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

struct chip_operations northbridge_sis_85c496_ops = {
	CHIP_NAME("SiS 85C496 Northbridge")
	.enable_dev = enable_dev,
};
