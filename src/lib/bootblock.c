/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <arch/exception.h>
#include <bootblock_common.h>
#include <console/console.h>
#include <delay.h>
#include <metadata_hash.h>
#include <option.h>
#include <post.h>
#include <program_loading.h>
#include <security/tpm/tspi.h>
#include <symbols.h>
#include <timestamp.h>


__weak void bootblock_mainboard_early_init(void) { /* no-op */ }
__weak void bootblock_soc_early_init(void) { /* do nothing */ }
__weak void bootblock_soc_init(void) { /* do nothing */ }
__weak void bootblock_mainboard_init(void) { /* do nothing */ }

/*
 * This is a the same as the bootblock main(), with the difference that it does
 * not collect a timestamp. Instead it accepts the initial timestamp and
 * possibly additional timestamp entries as arguments. This can be used in cases
 * where earlier stamps are available. Note that this function is designed to be
 * entered from C code. This function assumes that the timer has already been
 * initialized, so it does not call init_timer().
 */
void bootblock_main_with_timestamp(uint64_t base_timestamp,
	struct timestamp_entry *timestamps, size_t num_timestamps)
{
	/* Initialize timestamps if we have TIMESTAMP region in memlayout.ld. */
	if (CONFIG(COLLECT_TIMESTAMPS) &&
	    REGION_SIZE(timestamp) > 0) {
		int i;
		timestamp_init(base_timestamp);
		for (i = 0; i < num_timestamps; i++)
			timestamp_add(timestamps[i].entry_id,
				      timestamps[i].entry_stamp);
	}

	timestamp_add_now(TS_BOOTBLOCK_START);

	// post_code('b');

	// asm volatile("rdtsc" :::);
	bootblock_soc_early_init();

	// printk(BIOS_DEBUG, "E1\n");

// post_code('m');

	// asm volatile("rdtsc" :::);
	bootblock_mainboard_early_init();
	// asm volatile("rdtsc" :::);

	// printk(BIOS_DEBUG, "E2\n");

	if (CONFIG(USE_OPTION_TABLE))
		sanitize_cmos();

	if (CONFIG(CMOS_POST))
		cmos_post_init();

// post_code('c');


	if (CONFIG(BOOTBLOCK_CONSOLE)) {
		console_init();

		// printk(BIOS_NOTICE, "A1\n");

		exception_init();
	}

	// printk(BIOS_NOTICE, "A2\n");

	bootblock_soc_init();
	// printk(BIOS_NOTICE, "A3\n");
	bootblock_mainboard_init();
	// printk(BIOS_NOTICE, "A4\n");

	if (CONFIG(TPM_MEASURED_BOOT_INIT_BOOTBLOCK)) {
		int s3resume = acpi_is_wakeup_s3();
		tpm_setup(s3resume);
	}

	// printk(BIOS_NOTICE, "A5\n");

	timestamp_add_now(TS_BOOTBLOCK_END);

	// printk(BIOS_NOTICE, "A6\n");

/*
	printk(BIOS_NOTICE, "%p\n", &deleteme);
	deleteme = 0x42236933;
	printk(BIOS_NOTICE, "%p\n", &deleteme);

uint8_t *const mem1 = (uint8_t *)0x01001f00;
printk(BIOS_DEBUG, "\n%p:\n", mem1);
for (unsigned idx=0;idx<0x100;idx++) {
	if ((idx % 16) == 0) {
		printk(BIOS_DEBUG, "%p: ", &mem1[idx]);
	}
	printk(BIOS_DEBUG, "%02hhx ", mem1[idx]);
	if ((idx % 16) == 15) {
		printk(BIOS_DEBUG, "\n");
	}
}*/


	// post_code('x');

#if 0
	asm volatile(
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"rdtsc\n\t"
		:::
	);
#endif

	// volatile int xxx = deleteme;

#if 0
	asm volatile(
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"rdtsc\n\t"
		:::
	);
#endif



#if 0
uint8_t *const mem1 = (uint8_t *)0xfffe0000;
printk(BIOS_DEBUG, "\n%p:\n", mem1);
for (unsigned idx=0;idx<0x100;idx++) {
	printk(BIOS_DEBUG, "%02x ", mem1[idx]);
}

uint8_t *const mem2 = (uint8_t *)0xffff0000;
printk(BIOS_DEBUG, "\n%p:\n", mem2);
for (unsigned idx=0;idx<0x100;idx++) {
	printk(BIOS_DEBUG, "%02x ", mem2[idx]);
}

uint8_t *const mem3 = (uint8_t *)0xe0000;
printk(BIOS_DEBUG, "\n%p:\n", mem3);
for (unsigned idx=0;idx<0x100;idx++) {
	printk(BIOS_DEBUG, "%02x ", mem3[idx]);
}

uint8_t *const mem4 = (uint8_t *)0xf0000;
printk(BIOS_DEBUG, "\n%p:\n", mem4);
for (unsigned idx=0;idx<0x100;idx++) {
	printk(BIOS_DEBUG, "%02x ", mem4[idx]);
}
#endif


	printk(BIOS_NOTICE, "before ROM STAGE\n");

	run_romstage();
}

void bootblock_main_with_basetime(uint64_t base_timestamp)
{
	bootblock_main_with_timestamp(base_timestamp, NULL, 0);
}

void main(void)
{
	uint64_t base_timestamp = 0;

	post_code('D');

	init_timer();

	// post_code('E');

	if (CONFIG(COLLECT_TIMESTAMPS))
		base_timestamp = timestamp_get();

	// post_code('F');

	bootblock_main_with_timestamp(base_timestamp, NULL, 0);
}

#if CONFIG(COMPRESS_BOOTBLOCK)
/*
 * This is the bootblock entry point when it is run after a decompressor stage.
 * For non-decompressor builds, _start is generally defined in architecture-
 * specific assembly code. In decompressor builds that architecture
 * initialization code already ran in the decompressor, so the bootblock can
 * start straight into common code with a C environment.
 */
void _start(struct bootblock_arg *arg);
void _start(struct bootblock_arg *arg)
{
	if (CONFIG(CBFS_VERIFICATION))
		metadata_hash_import_anchor(arg->metadata_hash_anchor);
	bootblock_main_with_timestamp(arg->base_timestamp, arg->timestamps,
				      arg->num_timestamps);
}

#endif
