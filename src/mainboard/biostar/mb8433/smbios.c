/* SPDX-License-Identifier: GPL-2.0-only */

#include <smbios.h>

#define VOLTAGE_5	(1 << 0)
#define VOLTAGE_3_3	(1 << 1)

/* Returns the processor voltage in legacy bitfield */
unsigned int smbios_cpu_get_voltage(void)
{
	return VOLTAGE_5 | VOLTAGE_3_3; /* Unknown */
}
