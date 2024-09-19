/* SPDX-License-Identifier: GPL-2.0-only */

//TODO remove 686 and x86-64 code


#include <cpu/486/cr.h>
#include <cpu/486/cache.h>


#define UMC_PCI_BDFR(BUS, DEV, FN, REG) ( \
	(1 << 31) | \
	(((BUS) & 0xff) << 16) | \
	(((DEV) & 0x1f) << 11) | \
	(((FN)  & 0x07) << 8) | \
	(((REG)  & 0xff) << 0) \
)


#define send_uart		\
	movw	$0x3f8, %dx;	\
	outb	%al, %dx;	\
1:	movw	$0x3fd, %dx;	\
	inb	%dx, %al;	\
	testb	$0x20, %al;	\
	jz	1b


#define pci_write(BDFR, REG_IMM)	\
	movw	$0xcf8, %dx;	\
	movl	$ BDFR, %eax;	\
	outl	%eax, %dx;	\
	movw	$0xcfc, %dx;	\
	and	$0x3, %al;	\
	add	%al, %dl;	\
	movb	REG_IMM, %al;	\
	outb	%al, %dx

//value returned in AL
#define pci_read(BDFR)		\
	movw	$0xcf8, %dx;	\
	movl	$ BDFR, %eax;	\
	outl	%eax, %dx;	\
	movw	$0xcfc, %dx;	\
	and	$0x3, %al;	\
	add	%al, %dl;	\
	inb	%dx, %al




/* Place the stack in the bss section. It's not necessary to define it in
 * the linker script. */
	.section .bss, "aw", @nobits
.global _stack
.global _estack
.global _stack_size

_stack:
.space CONFIG_STACK_SIZE
_estack:
.set _stack_size, _estack - _stack

.text
.global _start
_start:
	/* Assume stack alignment doesn't matter here as chipset_teardown_car
	   is expected to be implemented in assembly. */

	/* Migrate GDT to this text segment */
#if ENV_X86_64
	call	gdt_init64
#else
	call	gdt_init
#endif

	mov	$'{', %al
	send_uart
	mov	$0xd, %al
	send_uart
	mov	$'{', %al
	send_uart
	mov	$0xd, %al
	send_uart
	mov	$'{', %al
	send_uart
	mov	$0xd, %al
	send_uart

#if ENV_X86_64
	mov	%rdi, %rax
	movabs %rax, _cbmem_top_ptr
#else
	/* The return argument is at 0(%esp), the calling argument at 4(%esp) */
	movl	4(%esp), %eax
	movl	%eax, _cbmem_top_ptr
#endif


#if 1
	call	1f
1:
	pop	%eax
	send_uart
	shr $0x8, %eax
	send_uart
	shr $0x8, %eax
	send_uart
	shr $0x8, %eax
	send_uart


	# # 01000000 = _cbmem_top_ptr
	# send_uart
	# shr	$0x8, %eax
	# send_uart
	# shr	$0x8, %eax
	# send_uart
	# shr	$0x8, %eax
	# send_uart
 #
 #
	# mov	$0xd, %al	//pc2005
	# send_uart

	# 04560f7c
	mov	%esp, %eax
	send_uart
	shr	$0x8, %eax
	send_uart
	shr	$0x8, %eax
	send_uart
	shr	$0x8, %eax
	send_uart

	mov	$0xd, %al	//pc2005
	send_uart

	# [esp] = 000e05dc
	mov	(%esp), %eax
	send_uart
	shr	$0x8, %eax
	send_uart
	shr	$0x8, %eax
	send_uart
	shr	$0x8, %eax
	send_uart

	mov	$0xd, %al	//pc2005
	send_uart


	# 01000000
	mov	_cbmem_top_ptr, %eax
	send_uart
	shr	$0x8, %eax
	send_uart
	shr	$0x8, %eax
	send_uart
	shr	$0x8, %eax
	send_uart

	mov	$0xd, %al	//pc2005
	send_uart

#endif

#if 0	//no cflush on 486 // TODO macro?
	/* Make sure _cbmem_top_ptr hits dram before invd */
	movl	$1, %eax
	cpuid
	btl	$CPUID_FEATURE_CLFLUSH_BIT, %edx
	jnc	skip_clflush
#if ENV_X86_64
	movabs	$_cbmem_top_ptr, %rax
	clflush	(%rax)
#else
	clflush	_cbmem_top_ptr
#endif
#endif

	# //page 135 137 83


	# movl	$0x3, %eax
	# movl	%eax, %tr5


#if 1
	# CR0 val = 60000011 cd+nw
	movl	%cr0, %eax

	send_uart
	shr	$0x8, %eax
	send_uart
	shr	$0x8, %eax
	send_uart
	shr	$0x8, %eax
	send_uart

	mov	$0xd, %al	//pc2005
	send_uart

	mov	%cr0, %eax
	andl	$(~(CR0_CD | CR0_NW)), %eax


	send_uart
	shr	$0x8, %eax
	send_uart
	shr	$0x8, %eax
	send_uart
	shr	$0x8, %eax
	send_uart

	mov	$0xd, %al	//pc2005
	send_uart

#endif


skip_clflush:
	/* chipset_teardown_car() is expected to disable cache-as-ram. */
	# call	chipset_teardown_car

	# wbinvd	//pc2005

	# invd

	mov	$0x33, %al	//pc2005
	send_uart
	mov	$0x34, %al	//pc2005
	send_uart
	mov	$0x35, %al	//pc2005
	send_uart
	mov	$0x36, %al	//pc2005
	send_uart
	mov	$0x37, %al	//pc2005
	send_uart


	//fill L2 cache, twice as size?
	movl	$0x200000, %edi
	movl	$0x300000, %ecx
	sub	%edi, %ecx
	shr	$2, %ecx
	cld

	# rep	lodsl	/* DF=0, up */

	xor	%eax, %eax
	rep	stosl	/* DF=0, up */

	invd

	/* Enable caching if not already enabled. */
#if ENV_X86_64
	mov	%cr0, %rax
	and     $(~(CR0_CD | CR0_NW)), %eax
	mov	%rax, %cr0
#else
	mov	%cr0, %eax
	and	$(~(CR0_CD | CR0_NW)), %eax
	mov	%eax, %cr0
#endif
	/* Ensure cache is clean. */
	# invd

	# //hack puvodne invd pc2005
	# wbinvd
	invd
	# invd

	movl	$0x200000, %edi
	movl	$0x400000, %ecx
	sub	%edi, %ecx
	shr	$2, %ecx
	cld

	rep	lodsl	/* DF=0, up */

	# xor	%eax, %eax
	# rep	stosl	/* DF=0, up */


	# //dies here, WHY???

#if 0
	pci_read(UMC_PCI_BDFR(0, 0x10, 0, 0x5a))
	movb	%al, %cl
	andb	$(~0x40), %cl
	pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x5a), %cl)
#endif

	//TODO switch to writeback? (any time probably)
#if 1
	pci_read(UMC_PCI_BDFR(0, 0x10, 0, 0x50))
	movb	%al, %cl
	orb	$(0x40), %cl
	pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x50), %cl)
#endif

	mov	$'X', %al	//pc2005
	send_uart
	mov	$0xc, %al	//pc2005
	send_uart
	mov	$0xb, %al	//pc2005
	send_uart
	mov	$0xa, %al	//pc2005
	send_uart

#if 0
	# //01ff21a5
	mov	%esp, %eax
	send_uart
	shr	$0x8, %eax
	send_uart
	shr	$0x8, %eax
	send_uart
	shr	$0x8, %eax
	send_uart

	mov	$0xd, %al	//pc2005
	send_uart

	call	test
test:
	pop	%eax
	send_uart
	shr $0x8, %eax
	send_uart
	shr $0x8, %eax
	send_uart
	shr $0x8, %eax
	send_uart


	mov	$0xa, %al	//pc2005
	send_uart


	# 02000000
	mov	_cbmem_top_ptr, %eax
	send_uart
	shr	$0x8, %eax
	send_uart
	shr	$0x8, %eax
	send_uart
	shr	$0x8, %eax
	send_uart

	mov	$0xd, %al	//pc2005
	send_uart

	# 01ff21a5
	mov	%esp, %eax
	send_uart
	shr	$0x8, %eax
	send_uart
	shr	$0x8, %eax
	send_uart
	shr	$0x8, %eax
	send_uart

	mov	$0xd, %al	//pc2005
	send_uart
#endif

	//NOTICE all levels of memory should be coherent by now

	movl	$_estack, %esp
	/* Align stack to 16 bytes at call instruction. */
	andl	$0xfffffff0, %esp

	/* Call this in assembly as some platforms like to mess with the bootflow and
	   call into main directly from chipset_teardown_car. */
	/* call	postcar_mtrr_setup */

#if 0
	# 01ff9410
	mov	%esp, %eax
	send_uart
	shr	$0x8, %eax
	send_uart
	shr	$0x8, %eax
	send_uart
	shr	$0x8, %eax
	send_uart

	mov	$0xd, %al	//pc2005
	send_uart
#endif

#if 0
	//quick search debug block
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	rdtsc
#endif

#if 0

	# 01ff93d0
	mov	%esp, %eax
	send_uart
	shr	$0x8, %eax
	send_uart
	shr	$0x8, %eax
	send_uart
	shr	$0x8, %eax
	send_uart

	mov	$0xd, %al	//pc2005
	send_uart
	mov	$0xa, %al	//pc2005
	send_uart

	# 01ff93d0
	mov	%esp, %eax
	send_uart
	shr	$0x8, %eax
	send_uart
	shr	$0x8, %eax
	send_uart
	shr	$0x8, %eax
	send_uart
#endif

	/* Call into main for postcar. */
	call	main
	/* Should never return. */
1:
	jmp	1b
