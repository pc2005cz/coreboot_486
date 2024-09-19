/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * This software and ancillary information (herein called SOFTWARE)
 * called LinuxBIOS is made available under the terms described here.
 *
 * The SOFTWARE has been approved for release with associated
 * LA-CC Number 00-34. Unless otherwise indicated, this SOFTWARE has
 * been authored by an employee or employees of the University of
 * California, operator of the Los Alamos National Laboratory under
 * Contract No. W-7405-ENG-36 with the U.S. Department of Energy.
 *
 * The U.S. Government has rights to use, reproduce, and distribute this
 * SOFTWARE. The public may copy, distribute, prepare derivative works
 * and publicly display this SOFTWARE without charge, provided that this
 * Notice and any statement of authorship are reproduced on all copies.
 *
 * Neither the Government nor the University makes any warranty, express
 * or implied, or assumes any liability or responsibility for the use of
 * this SOFTWARE.  If SOFTWARE is modified to produce derivative works,
 * such modified SOFTWARE should be clearly marked, so as not to confuse
 * it with the version available from LANL.
 *
 */


/* Start code to put an i386 or later processor into 32-bit protected mode.
 */

#include <arch/rom_segs.h>
#include <cpu/486/post_code.h>


#define send_uart		\
	movw	$0x3f8, %dx;	\
	outb	%al, %dx;	\
1:	movw	$0x3fd, %dx;	\
	inb	%dx, %al;	\
	testb	$0x20, %al;	\
	jz	1b



.section .init._start, "ax", @progbits

/* Symbol _start16bit must reachable from the reset vector, and be aligned to
 * 4kB to start AP CPUs with Startup IPI message without RAM.
 */
.code16
.globl _start16bit
.type _start16bit, @function

_start16bit:
	cli
	/* Save the BIST result */
	movl	%eax, %ebp

	post_init
	post_code(/*POST_RESET_VECTOR_CORRECT*/ 's')

	/* IMMEDIATELY invalidate the translation lookaside buffer (TLB) before
	 * executing any further code. Even though paging is disabled we
	 * could still get false address translations due to the TLB if we
	 * didn't invalidate it. Thanks to kmliu@sis.com.tw for this TLB fix.
	 */

	xorl	%eax, %eax
	movl	%eax, %cr3    /* Invalidate TLB*/

	/* Invalidating the cache here seems to be a bad idea on
	 * modern processors.  Don't.
	 * If we are hyperthreaded or we have multiple cores it is bad,
	 * for SMP startup.  On Opterons it causes a 5 second delay.
	 * Invalidating the cache was pure paranoia in any event.
	 * If your CPU needs it you can write a CPU dependent version of
	 * entry16.inc.
	 */

	 //486 is not a modern processor :-D
	 invd

	/* check if chipset is configured
	 * = segment E/F is shadowed in RAM -> contains seabios -> jump there
	 */
#if CONFIG(NORTHBRIDGE_ALI_M1489)
	//TODO
#elif CONFIG(NORTHBRIDGE_UMC_UM8881)
	//if 0x54 and 0x55 has shadow set -> guess the chipset is configured
	//NOTICE cannot check ram size register, minimum RAM is defined at power on

	movw	$0xcf8, %dx
	movl	$0x80008054, %eax	//BDFR: 0:0x10:0
	outl	%eax, %dx
	movw	$0xcfc, %dx
	and	$0x3, %al
	add	%al, %dl
	inb	%dx, %al

	test	$0x01, %al
	jz	no_soft_reset	/* segment E not shadowed */

	movw	$0xcf8, %dx
	movl	$0x80008055, %eax	//BDFR: 0:0x10:0
	outl	%eax, %dx
	movw	$0xcfc, %dx
	and	$0x3, %al
	add	%al, %dl
	inb	%dx, %al

	test	$0x80, %al
	jz	no_soft_reset	/* segment F not shadowed, global shadow disabled */


#if 1
	//read register 0xf of RTC
	inb	$0x70, %al
	and	$0x80, %al
	or	$0xf, %al
	outb	%al, $0x70
	inb	$0x71, %al
	cmp	$0, %al
	jz	no_soft_reset
#endif



	//also if no cmos/resume != 0 -> hard reset

#if 0
	jmp	no_soft_reset	//TEMP
#endif

	//chipset is configured -> soft reset (CPU only reset)
	//jump to shadow (seabios or other payload??)

	post_code(/*POST_RESET_VECTOR_CORRECT*/ 'W')

#if 0
	mov	%cr2, %eax
	send_uart
	shr	$0x8, %eax
	send_uart
	shr	$0x8, %eax
	send_uart
	shr	$0x8, %eax
	send_uart
#endif


	/* Restore BIST to %eax */
	movl	%ebp, %eax

	//TODO restore EDX?

	ljmpl	$0xf000, $0xfff0

no_soft_reset:
#endif

	/* Note: gas handles memory addresses in 16 bit code very poorly.
	 * In particular it doesn't appear to have a directive allowing you
	 * associate a section or even an absolute offset with a segment register.
	 *
	 * This means that anything except cs:ip relative offsets are
	 * a real pain in 16 bit mode.  And explains why it is almost
	 * impossible to get gas to do lgdt correctly.
	 *
	 * One way to work around this is to have the linker do the
	 * math instead of the assembler.  This solves the very
	 * practical problem of being able to write code that can
	 * be relocated.
	 *
	 * An lgdt call before we have memory enabled cannot be
	 * position independent, as we cannot execute a call
	 * instruction to get our current instruction pointer.
	 * So while this code is relocatable it isn't arbitrarily
	 * relocatable.
	 *
	 * The criteria for relocation have been relaxed to their
	 * utmost, so that we can use the same code for both
	 * our initial entry point and startup of the second CPU.
	 * The code assumes when executing at _start16bit that:
	 * (((cs & 0xfff) == 0) and (ip == _start16bit & 0xffff))
	 * or
	 * ((cs == anything) and (ip == 0)).
	 *
	 * The restrictions in reset16.inc mean that _start16bit initially
	 * must be loaded at or above 0xffff0000 or below 0x100000.
	 *
	 * The linker scripts computes gdtptr16_offset by simply returning
	 * the low 16 bits.  This means that the initial segment used
	 * when start is called must be 64K aligned.  This should not
	 * restrict the address as the ip address can be anything.
	 *
	 * Also load an IDT with NULL limit to prevent the 16bit IDT being used
	 * in protected mode before c_start.S sets up a 32bit IDT when entering
	 * RAM stage. In practise: CPU will shutdown on any exception.
	 * See IA32 manual Vol 3A 19.26 Interrupts.
	 */

	movw	%cs, %ax
	shlw	$4, %ax
	movw	$nullidt_offset, %bx
	subw	%ax, %bx
	lidt	%cs:(%bx)
	movw	$gdtptr_offset, %bx
	subw	%ax, %bx
	lgdtl	%cs:(%bx)

	post_code('T')

	movl	%cr0, %eax
	andl	$0x7FFAFFD1, %eax /* PG,AM,WP,NE,TS,EM,MP = 0 */
	orl	$0x60000001, %eax /* CD, NW, PE = 1 */
	movl	%eax, %cr0

	//pc2005 test CS
	post_code('A')
	post_code('{')

	mov	%cs, %bx

	movb	%bl, %al
	movw	$0x3f8, %dx
	outb	%al, %dx
1:	movw	$0x3fd, %dx
	inb	%dx, %al
	testb	$0x20, %al
	jz	1b

	movb	%bh, %al
	movw	$0x3f8, %dx
	outb	%al, %dx
1:	movw	$0x3fd, %dx
	inb	%dx, %al
	testb	$0x20, %al
	jz	1b

	post_code('}')

	/* Restore BIST to %eax */
	movl	%ebp, %eax

/*
	nop
	nop
	nop
	nop
	rdtsc*/


	/* Now that we are in protected mode jump to a 32 bit code segment. */
	ljmpl	$ROM_CODE_SEG, $bootblock_protected_mode_entry

	/**
	 * The gdt is defined in gdt_init.S, it has a 4 Gb code segment
	 * at 0x08, and a 4 GB data segment at 0x10;
	 */
__gdtptr:
	.long	gdtptr

.align	4
.globl nullidt
nullidt:
	.word	0	/* limit */
	.long	0
	.word	0
