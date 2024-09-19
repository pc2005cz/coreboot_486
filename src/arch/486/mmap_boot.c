/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot_device.h>
// #include <spi_flash.h>
#include <stdint.h>


#if CONFIG(NORTHBRIDGE_ALI_M1489)	//pc2005 enable for ALI

// NOTICE 0xfffe0000 not existent on finali
#	define rom_base ((void *)(uintptr_t)(0x100000ULL-CONFIG_ROM_SIZE))

#elif CONFIG(NORTHBRIDGE_UMC_UM8881)

/* The ROM is memory mapped just below 4GiB. Form a pointer for the base. */
#	define rom_base ((void *)(uintptr_t)(0x100000000ULL-CONFIG_ROM_SIZE))

#endif


static const struct mem_region_device boot_dev =
	MEM_REGION_DEV_RO_INIT(rom_base, CONFIG_ROM_SIZE);

const struct region_device *boot_device_ro(void)
{
	return &boot_dev.rdev;
}

#if 0
uint32_t spi_flash_get_mmap_windows(struct flash_mmap_window *table)
{
	return 0;
}
#endif
