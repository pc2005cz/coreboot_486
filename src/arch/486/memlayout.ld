/* SPDX-License-Identifier: GPL-2.0-only */

#include <memlayout.h>
#include <arch/header.ld>

SECTIONS
{
	/*
	 * It would be good to lay down RAMSTAGE, ROMSTAGE, etc consecutively
	 * like other architectures/chipsets it's not possible because of
	 * the linking games played during romstage creation by trying
	 * to find the final landing place in CBFS for XIP. Therefore,
	 * conditionalize with macros.
	 */
#if ENV_RAMSTAGE
	/* Relocated at runtime in cbmem so the address does not matter. */
	RAMSTAGE(8M, 8M)

#elif ENV_ROMSTAGE
	/* The 1M size is not allocated. It's just for basic size checking.
	 * Link at 32MiB address and rely on cbfstool to relocate to XIP. */
	ROMSTAGE(CONFIG_ROMSTAGE_ADDR, 1M)

	INCLUDE "romstage/arch/486/car.ld"
#elif ENV_SEPARATE_VERSTAGE
	/* The 1M size is not allocated. It's just for basic size checking.
	 * Link at 32MiB address and rely on cbfstool to relocate to XIP. */
	VERSTAGE(CONFIG_VERSTAGE_ADDR, 1M)

	INCLUDE "verstage/arch/486/car.ld"
#elif ENV_BOOTBLOCK

	INCLUDE "bootblock/arch/486/car.ld"

#elif ENV_POSTCAR
	POSTCAR(8M, 1M)
#endif
}

#if ENV_BOOTBLOCK
	INCLUDE "bootblock/arch/486/bootblock.ld"
#endif  /* ENV_BOOTBLOCK */
