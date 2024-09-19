/* SPDX-License-Identifier: GPL-2.0-only */

/* CACHE_ROM_SIZE defined here. */
#include <memlayout.h>

/* This file is included inside a SECTIONS block */
. = CONFIG_DCACHE_RAM_BASE;
.car.data . (NOLOAD) : {
	_car_region_start = . ;
#if CONFIG(PAGING_IN_CACHE_AS_RAM)
	/* Page table pre-allocation. CONFIG_DCACHE_RAM_BASE should be 4KiB
	 * aligned when using this option. */
	REGION(pagetables, ., 4K * CONFIG_NUM_CAR_PAGE_TABLE_PAGES, 4K)
#endif
	/* Stack for CAR stages. Since it persists across all stages that
	 * use CAR it can be reused. The chipset/SoC is expected to provide
	 * the stack size. */
	REGION(car_stack, ., CONFIG_DCACHE_BSP_STACK_SIZE, 4)
	/* The pre-ram cbmem console as well as the timestamp region are fixed
	 * in size. Therefore place them above the car global section so that
	 * multiple stages (romstage and verstage) have a consistent
	 * link address of these shared objects. */
	PRERAM_CBMEM_CONSOLE(., CONFIG_PRERAM_CBMEM_CONSOLE_SIZE)
#if CONFIG(PAGING_IN_CACHE_AS_RAM)
	. = ALIGN(32);
	/* Page directory pointer table resides here. There are 4 8-byte entries
	 * totalling 32 bytes that need to be 32-byte aligned. The reason the
	 * pdpt are not colocated with the rest of the page tables is to reduce
	 * fragmentation of the CAR space that persists across stages. */
	REGION(pdpt, ., 32, 32)
#endif

	TIMESTAMP(., 0x200)

#if !CONFIG(NO_CBFS_MCACHE)
	CBFS_MCACHE(., CONFIG_CBFS_MCACHE_SIZE)
#endif
/*#if !CONFIG(NO_FMAP_CACHE)
	FMAP_CACHE(., FMAP_SIZE)
#endif
*/
	/* Reserve sizeof(struct ehci_dbg_info). */
	REGION(car_ehci_dbg_info, ., 80, 1)

	/* _bss and _ebss provide symbols to per-stage
	 * variables that are not shared like the timestamp and the pre-ram
	 * cbmem console. This is useful for clearing this area on a per-stage
	 * basis when more than one stage uses cache-as-ram. */

#if ENV_SEPARATE_BSS
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_bss = .;
	/* Allow global uninitialized variables for stages without CAR teardown. */
	*(.bss)
	*(.bss.*)
	*(.sbss)
	*(.sbss.*)
	/* '*_E' GNAT generated global variables actually are un-initialized
	 * (filled with zeros) variables which are initialized at
	 * runtime. Therefore, they can be placed in the _bss region. */
#if CONFIG(ROMSTAGE_LIBHWBASE)
	*(.data.hw__*_E)
#endif
#if CONFIG(EARLY_GFX_GMA)
	*(.data.gma*_E)
	/* libgfxinit uses a boolean variable to track its initialization
	 * state. Since the initial value is False it can safely be placed in
	 * the _bss region. */
	*(.data.hw__gfx__gma__initialized)
#endif
	. = ALIGN(ARCH_POINTER_ALIGN_SIZE);
	_ebss = .;
	RECORD_SIZE(bss)
#endif

#if ENV_ROMSTAGE && CONFIG(ASAN_IN_ROMSTAGE)
	_shadow_size = (_ebss - _car_region_start) >> 3;
	REGION(asan_shadow, ., _shadow_size, ARCH_POINTER_ALIGN_SIZE)
#endif
	_car_unallocated_start = .;
	_car_region_end = . + CONFIG_DCACHE_RAM_SIZE - (. - _car_region_start);
}



/* Global variables are not allowed in romstage
 * This section is checked during stage creation to ensure
 * that there are no global variables present
 */

#if CONFIG(NORTHBRIDGE_ALI_M1489)
. = 0x000fff00;
#elif CONFIG(NORTHBRIDGE_UMC_UM8881)
. = 0xffffff00;
#endif

.illegal_globals . : {
	*(.data)
	*(.data.*)
}

_bogus = ASSERT((CONFIG_DCACHE_RAM_SIZE == 0) || (SIZEOF(.car.data) <= CONFIG_DCACHE_RAM_SIZE), "Cache as RAM area is too full");
#if CONFIG(PAGING_IN_CACHE_AS_RAM)
_bogus2 = ASSERT(_pagetables == ALIGN(_pagetables, 4096), "_pagetables aren't 4KiB aligned");
#endif
_bogus3 = ASSERT(CONFIG_DCACHE_BSP_STACK_SIZE > 0x0, "BSP stack size not configured");
#if CONFIG(NO_XIP_EARLY_STAGES) && (ENV_ROMSTAGE || ENV_VERSTAGE)
_bogus4 = ASSERT(_eprogram <= _car_region_end, "Stage end too high !");
/* _bogus5 = ASSERT(_program >= _car_unallocated_start, "Stage start too low!"); */
#endif
