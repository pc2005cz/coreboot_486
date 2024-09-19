/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>
#include <console/cbmem_console.h>
#include <console/console.h>
#include <console/uart.h>
#include <console/streams.h>
#include <device/pci.h>
#include <option.h>
#include <version.h>

#define FIRST_CONSOLE (ENV_BOOTBLOCK || (CONFIG(NO_BOOTBLOCK_CONSOLE) && ENV_ROMSTAGE))

//these requires CaR
static int console_inited;
static int console_loglevel;

int get_log_level(void)
{
	if (console_inited == 0)
		return -1;

	return console_loglevel;
}

static void init_log_level(void)
{
	console_loglevel = get_console_loglevel();

	if (!FIRST_CONSOLE)
		console_loglevel = get_uint_option("debug_level", console_loglevel);
}

int console_log_level(int msg_level)
{
	int log_level = get_log_level();

	if (log_level < 0)
		return CONSOLE_LOG_NONE;

	if (msg_level <= log_level)
		return CONSOLE_LOG_ALL;

	if (CONFIG(CONSOLE_CBMEM) && (msg_level <= BIOS_DEBUG))
		return CONSOLE_LOG_FAST;

	return 0;
}

void console_init(void)
{
	init_log_level();

	if (CONFIG(DEBUG_CONSOLE_INIT))
		console_inited = 1;

	if (CONFIG(EARLY_PCI_BRIDGE) && (ENV_BOOTBLOCK || ENV_ROMSTAGE))
		pci_early_bridge_init();


	asm volatile(
		"movb $'j', %%al\n\t"     // Move the character to AL register
		"movw $0x3F8, %%dx\n\t" // Load 0x3F8 (COM1 port) into DX register
		"outb %%al, %%dx"       // Send the character to COM1 port
		:::
	);

	console_hw_init();

	console_inited = 1;

	if (ENV_BOOTBLOCK && CONFIG(CONSOLE_CBMEM_PRINT_PRE_BOOTBLOCK_CONTENTS))
		cbmem_dump_console();

#if 0

	uint8_t * mem = (uint8_t *) CONFIG_DCACHE_RAM_BASE;

	// asm volatile("rdtsc" :::);

	printk(BIOS_NOTICE, "\n\nDUMP:\n");
	// asm volatile("rdtsc" :::);

	for (unsigned idx=0; idx < CONFIG_DCACHE_RAM_SIZE; idx++) {
		if ((idx % 16) == 0) {
			printk(BIOS_NOTICE, "%08x: ", CONFIG_DCACHE_RAM_BASE + idx);
		}

		printk(BIOS_NOTICE, "%02x ", mem[idx]);

		if ((idx % 16) == 15) {
			printk(BIOS_NOTICE, "\n");
		}
	}
#endif

// printk(BIOS_NOTICE, "\n\nconsole_inited: %p\n", &console_inited);

//NOTICE prvni vyskyt vypisu integeru, s modernim kompilatorem vygeneruje endbr32 v deleni radu -> reset

	printk(BIOS_NOTICE, "\n\ncoreboot-%s%s %s " ENV_STRING " starting (log level: %i)...\n",
	       coreboot_version, coreboot_extra_version, coreboot_build, get_log_level());
}
