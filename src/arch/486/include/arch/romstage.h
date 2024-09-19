/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ARCH_ROMSTAGE_H__
#define __ARCH_ROMSTAGE_H__

#include <stddef.h>
#include <stdint.h>

void mainboard_romstage_entry(void);

/*
 * prepare_and_run_postcar() determines the stack to use after
 * cache-as-ram is torn down as well as the MTRR settings to use.
 */
void __noreturn prepare_and_run_postcar(void);

/*
 * Systems without a native coreboot cache-as-ram teardown may implement
 * this to use an alternate method.
 */
void late_car_teardown(void);

/*
 * Cache the TSEG region at the top of ram. This region is
 * not restricted to SMM mode until SMM has been relocated.
 * By setting the region to cacheable it provides faster access
 * when relocating the SMM handler as well as using the TSEG
 * region for other purposes.
 */
void postcar_enable_tseg_cache(void);

#endif /* __ARCH_ROMSTAGE_H__ */
