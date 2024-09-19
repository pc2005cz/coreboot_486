/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <cbmem.h>
#include <console/console.h>
#include <program_loading.h>
#include <reset.h>
#include <rmodule.h>
#include <stage_cache.h>
#include <timestamp.h>
#include <types.h>


//temp, for dump
#include <device/pci_ops.h>
#include <northbridge/umc/um8881/reg_init.h>


static void run_postcar_phase(void);

/* prepare_and_run_postcar() determines the stack to use after
 * cache-as-ram is torn down as well as the MTRR settings to use. */
void __noreturn prepare_and_run_postcar(void)
{
	printk(BIOS_DEBUG, "prepare_and_run_postcar\n");

	run_postcar_phase();
	/* We do not return here. */
	die("Failed to load postcar\n!");
}

static void load_postcar_cbfs(struct prog *prog)
{
	struct rmod_stage_load rsl = {
		.cbmem_id = CBMEM_ID_AFTER_CAR,
		.prog = prog,
	};

	printk(BIOS_NOTICE, "load_postcar_cbfs {\n");

	if (rmodule_stage_load(&rsl))
		die_with_post_code(POST_INVALID_ROM,
				   "Failed to load after CAR program.\n");

	printk(BIOS_NOTICE, "S2\n");

	stage_cache_add(STAGE_POSTCAR, prog);

	printk(BIOS_NOTICE, "load_postcar_cbfs }\n");


#if 0

	//pc2005 !!!!! TODO

	printk(BIOS_DEBUG, "dump HB:\n");
	for (unsigned idx=0;idx<0x63;idx++) {
		if ((idx % 16) == 0) {
			printk(BIOS_DEBUG, "%02x: ", idx);
		}
		printk(BIOS_DEBUG, "%02hhx ",
		       pci_read_config8(HOST_BRIDGE, idx)
		);
		if (((idx % 16) == 15) || (idx == (0x63-1))) {
			printk(BIOS_DEBUG, "\n");
		}
	}

	printk(BIOS_DEBUG, "dump IBC:\n");
	for (unsigned idx=0;idx<0xb0;idx++) {
		if ((idx % 16) == 0) {
			printk(BIOS_DEBUG, "%02x: ", idx);
		}
		printk(BIOS_DEBUG, "%02hhx ",
		       pci_read_config8(ISA_BRIDGE, idx)
		);
		if (((idx % 16) == 15) || (idx == (0xb0-1))) {
			printk(BIOS_DEBUG, "\n");
		}
	}
#endif

}

/*
 * Cache the TSEG region at the top of ram. This region is
 * not restricted to SMM mode until SMM has been relocated.
 * By setting the region to cacheable it provides faster access
 * when relocating the SMM handler as well as using the TSEG
 * region for other purposes.
 */
void postcar_enable_tseg_cache(void)
{
	// uintptr_t smm_base;
	// size_t smm_size;
//
	// smm_region(&smm_base, &smm_size);
}

static void postcar_cache_invalid(void)
{
	printk(BIOS_ERR, "postcar cache invalid.\n");
	board_reset();
}

static void run_postcar_phase(void)
{
	struct prog prog =
		PROG_INIT(PROG_POSTCAR, CONFIG_CBFS_PREFIX "/postcar");

// printk(BIOS_DEBUG, "M1\n");
#if 0
printk(BIOS_DEBUG, "0x500 table: ");
for (unsigned idx=0;idx<40;idx++) {
	u8 * dptr = (u8 *)0x500;
	printk(BIOS_DEBUG, "%02x ", dptr[idx]);
}
printk(BIOS_DEBUG, "\n");
#endif
	if (resume_from_stage_cache()) {

// printk(BIOS_DEBUG, "M2\n");

		stage_cache_load_stage(STAGE_POSTCAR, &prog);
		/* This is here to allow platforms to pass different stack
		   parameters between S3 resume and normal boot. On the
		   platforms where the values are the same it's a nop. */

		// printk(BIOS_DEBUG, "M3\n");

		if (prog_entry(&prog) == NULL)
			postcar_cache_invalid();

		// printk(BIOS_DEBUG, "M5\n");

	} else
		load_postcar_cbfs(&prog);

	// printk(BIOS_DEBUG, "M6\n");

	/* As postcar exist, it's end of romstage here */
	// timestamp_add_now(TS_ROMSTAGE_END);

	console_time_report();

	// printk(BIOS_DEBUG, "M7 cbmem_top() %p\n", cbmem_top());

	prog_set_arg(&prog, cbmem_top());

	// printk(BIOS_DEBUG, "M8\n");

	prog_run(&prog);
}
