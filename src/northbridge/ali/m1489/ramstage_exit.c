/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>

#include <program_loading.h>
#include <cbmem.h>

#include "regs.h"

//NOTICE only in ramstage, rest is weak
void platform_prog_run(struct prog *prog)
{
	//reset BIOS access to default

	printk(BIOS_DEBUG, "ROM access to default\n");

	//NOTICE cfg_modify(u8 idx, u8 clear, u8 set)

	//enable ROM in A31
	cfg_modify(0x21, 0, 0x20);

	//normal E/F access
	cfg_modify(0x2b, 0x20, 0);

	//disable flash write, segment E is ISA
	cfg_modify(0x12, 0, 0x00);

	//shadow region read enabled (for seabios)
	//disable write (seabios will enable it where wants)
	cfg_modify(0x14, 0x20, 0x10);

	//write fast back to back, CPU->PCI write burst
	//CPU to PCI write byte merge, CPU to PCI write buffer
	//dynamic PCI memory, dynamic ISA memory
	// cfg_modify(0x20, 0, 0x3f);	//TODO orig
	cfg_modify(0x20, 1, 0x3e);	//disable fast dynamic ISA

	//bit 1 should be "1"
	cfg_modify(0x22, 0, 0x2);


	//TODO init right before ramstage!! with CAR enabled
	//move it there eventually if stable enough

	//shadow region L1
	//uncached! (or uniflash wont work)
	//cacheable (+ L2: with write protection?)
	// cfg_modify(0x17, 0x80 | 0x40, 8 | 0x10);
	//NOTICE force L2 cache nondirty (bit can be wrong in chip after reset)
	//NOTICE force L2 cache miss (always load address)

	//L1, L2 cache enable
	// cfg_modify(0x16, 0, 0x07);

	// asm volatile("rdtsc" :::);
	// asm volatile("rdtsc" :::);
	// asm volatile("rdtsc" :::);
	// asm volatile("rdtsc" :::);


	//normal dirty and miss functionality
	// cfg_modify(0x17, 8 | 0x10, 0);


	///////////////

#if 0
	//NOTICE TEMP TODO
	cfg_modify(0x12, 0x80, 0);	//disable hidden refresh
	cfg_modify(0x20, 0xff, 0);	//disable all PCI buffers
	cfg_modify(0x21, 4, 0);		//disable improve reliability DX4 2-1-1-1

	cfg_modify(0x22, 2, 0);		//reserved bit 1 back to 0 ?

	cfg_modify(0x22, 8 | 0x10, 0);	//no PCI improvements
#endif

}
