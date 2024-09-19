/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <cbmem.h>
#include <console/console.h>
#include <main_decl.h>
#include <program_loading.h>
#include <timestamp.h>

/*
 * Systems without a native coreboot cache-as-ram teardown may implement
 * this to use an alternate method.
 */
__weak void late_car_teardown(void) { /* do nothing */ }

//NOTICE called from exit_car.S, right after enabling L1 cache in CR0
void main(void)
{
	late_car_teardown();

	console_init();

printk(BIOS_DEBUG, "=+=+=+=+ postcar main %p\n", main);

	/*
	 * CBMEM needs to be recovered because timestamps rely on
	 * the cbmem infrastructure being around. Explicitly recover it.
	 *
	 * On some platforms CBMEM needs to be initialized earlier.
	 * Use cbmem_online() to avoid init CBMEM twice.
	 */
	if (!cbmem_online()) {
		// printk(BIOS_DEBUG, "TODO cbmem_initialize\n");
		cbmem_initialize();
	}

	// asm volatile("rdtsc" :::);
	// asm volatile("rdtsc" :::);
	// asm volatile("rdtsc" :::);
	// asm volatile("rdtsc" :::);

	timestamp_add_now(TS_POSTCAR_START);

	// asm volatile("rdtsc" :::);
	// asm volatile("rdtsc" :::);
	// asm volatile("rdtsc" :::);
	// asm volatile("rdtsc" :::);

printk(BIOS_DEBUG, "postcar before run ramstage\n");

	/* Load and run ramstage. */
	run_ramstage();
}
