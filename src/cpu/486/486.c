/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
https://elixir.bootlin.com/coreboot/4.19/source/src/arch/x86/assembly_entry.S#L66
https://elixir.bootlin.com/coreboot/4.19/source/src/arch/x86/romstage.c#L8
	car_stage_entry
https://elixir.bootlin.com/coreboot/4.19/source/src/cpu/intel/car/romstage.c#L18
	romstage_main
	mainboard_romstage_entry
*/

#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/486/cache.h>
#include <cpu/486/name.h>
#include <device/device.h>
#include <string.h>

unsigned long tsc_freq_mhz(void);

unsigned long tsc_freq_mhz(void) {
	return 133333333;
}

static void cpu_486_init(struct device *dev)
{
	char processor_name[49];

	/* Turn on caching if we haven't already */
	enable_cache();

	/* Print processor name */
	if (cpu_have_cpuid()) {
		fill_processor_name(processor_name);
	} else {
		//TODO pre cpuid 486 from DX? 0x400+revision ID
		strncpy(processor_name, "pre-CPUID", 48);
	}

	printk(BIOS_INFO, "CPU: %s.\n", processor_name);
}

static struct device_operations cpu_dev_ops = {
	.init = cpu_486_init,
};

static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_ANY, 0, 0 },
	CPU_TABLE_END
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};

struct chip_operations cpu_486_ops = {
	CHIP_NAME("486 CPU")
};
