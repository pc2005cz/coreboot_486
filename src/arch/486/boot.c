/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/boot/boot.h>
#include <arch/cpu.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <program_loading.h>
#include <ip_checksum.h>
#include <symbols.h>
#include <assert.h>

int payload_arch_usable_ram_quirk(uint64_t start, uint64_t size)
{
	if (start < 1 * MiB && (start + size) <= 1 * MiB) {
		printk(BIOS_DEBUG,
			"Payload being loaded at 0x%08llx below 1MiB without region being marked as RAM usable.\n", start);
		return 1;
	}

	return 0;
}

void arch_prog_run(struct prog *prog)
{
#if ENV_RAMSTAGE && ENV_X86_64
	const uint32_t arg = pointer_to_uint32_safe(prog_entry_arg(prog));
	const uint32_t entry = pointer_to_uint32_safe(prog_entry(prog));

	/* On x86 coreboot payloads expect to be called in protected mode */
	protected_mode_jump(entry, arg);
#else
#if ENV_X86_64
	void (*doit)(void *arg);
#else
	/* Ensure the argument is pushed on the stack. */
	asmlinkage void (*doit)(void *arg);
#endif

	#if ENV_ROMSTAGE
	#if 0
	u8 * dptr = (u8 *)prog;
	printk(BIOS_DEBUG, "PROG dump:\n");
	for (unsigned idx=0;idx<28;idx++) {
		if ((idx % 16) == 0) {
			printk(BIOS_DEBUG, "%p: ", &dptr[idx]);
		}
		printk(BIOS_DEBUG, "%02hhx ", dptr[idx]);
		if ((idx % 16) == 15) {
			printk(BIOS_DEBUG, "\n");
		}
	}
	printk(BIOS_DEBUG, "\n");

	dptr = (u8 *)prog_entry(prog);
	printk(BIOS_DEBUG, "doit dump:\n");
	for (unsigned idx=0;idx<0x100;idx++) {
		if ((idx % 16) == 0) {
			printk(BIOS_DEBUG, "%p: ", &dptr[idx]);
		}
		printk(BIOS_DEBUG, "%02hhx ", dptr[idx]);
		if ((idx % 16) == 15) {
			printk(BIOS_DEBUG, "\n");
		}
	}
	printk(BIOS_DEBUG, "\n");

	#endif
	#endif

	doit = prog_entry(prog);

	printk(BIOS_DEBUG, "DOIT prog:%p doit:%p arg %p\n",
		prog, doit, prog_entry_arg(prog)
	);

	doit(prog_entry_arg(prog));
#endif
}
