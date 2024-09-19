/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/bootblock.h>
#include <arch/io.h>
#include <cpu/cpu.h>
#include <halt.h>
#include <stdint.h>
#include <bootblock_common.h>
#include <console/console.h>
#include <cpu/486/bist.h>

void __weak bootblock_early_northbridge_init(void) { }
void __weak bootblock_early_southbridge_init(void) { }


void bootblock_early_cpu_init(void)
{
	//detect cpu type
	//detect cpuid exists
}


void bootblock_soc_early_init(void)
{
	post_code('E');
	bootblock_early_northbridge_init();
	post_code('F');
	bootblock_early_southbridge_init();
	post_code('G');
	bootblock_early_cpu_init();
	post_code('H');
}


asmlinkage void bootblock_c_entry_bist(uint64_t base_timestamp, uint32_t bist)
{
#if 0
	asm volatile(
		"movb $0x39, %%al\n\t"     // Move the character to AL register
		"movw $0x3F8, %%dx\n\t" // Load 0x3F8 (COM1 port) into DX register
		"outb %%al, %%dx"       // Send the character to COM1 port
		:::
	);
#endif

	post_code('C');

	/* Halt if there was a built in self test failure */
	if (bist) {
		//NOTICE only error console init
		console_init();
		report_bist_failure(bist);
	}

	post_code('I');

	// asm volatile(
	// 	"mov %0, %%al\n\t"     // Move the character to AL register
	// 	"mov $0x3F8, %%dx\n\t" // Load 0x3F8 (COM1 port) into DX register
	// 	"out %%al, %%dx"       // Send the character to COM1 port
	// 	:
	// 	: "r" (xxx)	//output constraint r = readonly
	// 	: "%al", "%dx"	//clobbered registers
	// );


	/* Call lib/bootblock.c main() */
	bootblock_main_with_basetime(base_timestamp);
}
