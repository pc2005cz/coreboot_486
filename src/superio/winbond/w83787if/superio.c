/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>

static struct device_operations w83787if_ops = {
	.read_resources		= noop_read_resources,
	.set_resources		= noop_set_resources,
	// .acpi_name		= cs35l53_acpi_name,
	// .acpi_fill_ssdt		= cs35l53_fill_ssdt,
};


static void enable_dev(struct device *dev)
{
	// pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);

	dev->ops = &w83787if_ops;
}


struct chip_operations superio_winbond_w83787if_ops = {
	CHIP_NAME("WINBOND W83787IF Super I/O")
	.enable_dev = enable_dev,
};
