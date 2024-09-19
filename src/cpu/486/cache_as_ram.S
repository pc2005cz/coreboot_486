/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/486/post_code.h>
#include <cpu/486/cache.h>

#define UMC_PCI_BDFR(BUS, DEV, FN, REG) ( \
	(1 << 31) | \
	(((BUS) & 0xff) << 16) | \
	(((DEV) & 0x1f) << 11) | \
	(((FN)  & 0x07) << 8) | \
	(((REG)  & 0xff) << 0) \
)

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

#define UART_BASE	0x3f8

#define send_uart(base)		\
	movw	$(base+0), %dx;	\
	outb	%al, %dx;	\
1:	movw	$(base+5), %dx;	\
	inb	%dx, %al;	\
	testb	$0x20, %al;	\
	jz	1b



/* TODO use only vanilla

https://elixir.bootlin.com/coreboot/4.20.1/source/src/commonlib/include/commonlib/console/post_codes.h
*/
#include <cpu/intel/post_codes.h>

#define CBFS_FILE_MAGIC 0
#define CBFS_FILE_LEN (CBFS_FILE_MAGIC + 8)
#define CBFS_FILE_TYPE (CBFS_FILE_LEN + 4)
#define CBFS_FILE_CHECKSUM (CBFS_FILE_TYPE + 4)
#define CBFS_FILE_OFFSET (CBFS_FILE_CHECKSUM + 4)

.section .init, "ax", @progbits
.code32

.global bootblock_pre_c_entry
bootblock_pre_c_entry:

cache_as_ram:
	post_code(/*POST_BOOTBLOCK_CAR*/ '4')


#if 0
# POKER POKER POKER POKER POKER POKER POKER POKER POKER
# POKER POKER POKER POKER POKER POKER POKER POKER POKER
# POKER POKER POKER POKER POKER POKER POKER POKER POKER
# POKER POKER POKER POKER POKER POKER POKER POKER POKER
# POKER POKER POKER POKER POKER POKER POKER POKER POKER

/*
NOTICE
command sequence (bytes)
	OP VALUE MSB3 mSB2 lSB1 LSB0
return
	1 byte, either read value, or dummy/ack 0x42
OP:
	q	quit
	r/w	read/write memory
	i/o	read/write port
	C/c	enable/disable L1 cache
	!	invalidate L1
	?	writeback invalidate L1
	p/P	read/write PCI
	l/L	loop (N*65536) write 0/0xffffffff to specified memory
*/


#if 1
	/* init uart */
	movw	$(UART_BASE+1), %dx
	movb	$0x0, %al
	outb	%al, %dx
	movw	$(UART_BASE+3), %dx
	movb	$0x80, %al
	outb	%al, %dx
	movw	$(UART_BASE+0), %dx
	movb	$1, %al
	outb	%al, %dx
	movw	$(UART_BASE+1), %dx
	movb	$0x0, %al
	outb	%al, %dx
	movw	$(UART_BASE+3), %dx
	inb	%dx, %al
	andb	$0x7f, %al
	outb	%al, %dx
	movw	$(UART_BASE+1), %dx
	movb	$0x00, %al
	outb	%al, %dx
	movw	$(UART_BASE+2), %dx
	movb	$0xc7, %al
	outb	%al, %dx
	movw	$(UART_BASE+3), %dx
	movb	$0x03, %al
	outb	%al, %dx
	movw	$(UART_BASE+4), %dx
	movb	$0x01, %al
	outb	%al, %dx
#endif

#if 0
	movw	$(UART_BASE+5), %dx
	inb	%dx, %al
	test	$0x10, %al	# break indicated
	jz	exit_poker
#elif 0
	movw	$(UART_BASE+6), %dx
	inb	%dx, %al
	test	$0x80, %al	# activated carrier detect
	# test	$0x20, %al	# activated DSR
	# test	$0x10, %al	# activated CTS
	jz	exit_poker
#endif

	mov	$'P', %al
	send_uart(UART_BASE)
	mov	$'O', %al
	send_uart(UART_BASE)
	mov	$'K', %al
	send_uart(UART_BASE)
	mov	$'E', %al
	send_uart(UART_BASE)
	mov	$'R', %al
	send_uart(UART_BASE)
	mov	$'>', %al
	send_uart(UART_BASE)

poker:
	xor	%edi, %edi	#command
	xor	%esi, %esi	#parameter
	#edi .. esi
	# op value .. a3 a2 a1 a0

	mov	$6, %ecx

	# wait until rx
wait_rx:
	mov	$(UART_BASE+5), %dx
	in	%dx, %al
	test	$1, %al
	jz	wait_rx

	# read byte
	mov	$(UART_BASE+0), %dx
	in	%dx, %al

	mov	%esi, %ebx
	mov 	%al, %bl
	mov	%ebx, %esi

	dec	%ecx
	jcxz	process

	#count src dst
	# src >> dst

	shld	$8, %esi, %edi
	shl	$8, %esi

	jmp	wait_rx

process:

	#BX command (BH op, BL value)
	#EBP parameter

	mov	%edi, %ebx

	# quit
	cmp	$'q', %bh
	je	exit_poker

	# read/write memory
	cmp	$'r', %bh
	je	cmd_read_addr
	cmp	$'w', %bh
	je	cmd_write_addr

	# read/write port
	cmp	$'i', %bh
	je	cmd_read_port
	cmp	$'o', %bh
	je	cmd_write_port

	# cache
	cmp	$'C', %bh
	je	cmd_l1_enable
	cmp	$'c', %bh
	je	cmd_l1_disable
	cmp	$'!', %bh
	je	cmd_l1_invalidate
	cmp	$'?', %bh
	je	cmd_l1_writebackinvalidate

	# pci
	cmp	$'p', %bh
	je	cmd_read_pci
	cmp	$'P', %bh
	je	cmd_write_pci

	# loop write memory
	cmp	$'l', %bh
	je	cmd_loop_write_0
	cmp	$'L', %bh
	je	cmd_loop_write_ffffffff

	jmp	poker

send_reply:
	# data in al, waits automatically
	send_uart(UART_BASE)
	jmp	poker

cmd_read_addr:
	mov	%ds:(%esi), %al
	jmp	send_reply

cmd_write_addr:
	mov	%bl, %ds:(%esi)
	mov	$0x42, %al	#dummy/ack reply
	jmp	send_reply

cmd_read_port:
	mov	%esi, %edx
	in	%dx, %al
	jmp	send_reply

cmd_write_port:
	mov	%bl, %al
	mov	%esi, %edx
	out	%al, %dx
	mov	$0x42, %al	#dummy/ack reply
	jmp	send_reply

cmd_l1_enable:
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	movl	%eax, %cr0
	mov	$0x42, %al	#dummy/ack reply
	jmp	send_reply

cmd_l1_disable:
	movl	%cr0, %eax
	orl	$(CR0_CacheDisable | CR0_NoWriteThrough), %eax
	movl	%eax, %cr0
	mov	$0x42, %al	#dummy/ack reply
	jmp	send_reply

cmd_l1_invalidate:
	invd
	mov	$0x42, %al	#dummy/ack reply
	jmp	send_reply

cmd_l1_writebackinvalidate:
	wbinvd			#NOTICE will crash on WT-only 486
	mov	$0x42, %al	#dummy/ack reply
	jmp	send_reply

cmd_read_pci:
	movw	$0xcf8, %dx
	movl	%esi, %eax
	outl	%eax, %dx
	movw	$0xcfc, %dx
	and	$0x3, %al
	add	%al, %dl
	inb	%dx, %al

	jmp	send_reply

cmd_write_pci:
	movw	$0xcf8, %dx
	movl	%esi, %eax
	outl	%eax, %dx
	movw	$0xcfc, %dx
	and	$0x3, %al
	add	%al, %dl
	mov	%bl, %al	#write the value
	outb	%al, %dx

	mov	$0x42, %al	#dummy/ack reply
	jmp	send_reply

cmd_loop_write_0:
	xor	%ecx, %ecx
	mov	%bl, %cl
	shl	$16, %ecx
1:
	movl	$0, %ds:(%esi)
	loop	1b

	mov	$0x42, %al	#dummy/ack reply
	jmp	send_reply

cmd_loop_write_ffffffff:
	xor	%ecx, %ecx
	mov	%bl, %cl
	shl	$16, %ecx
1:
	movl	$0xffffffff, %ds:(%esi)
	loop	1b

	mov	$0x42, %al	#dummy/ack reply
	jmp	send_reply

# # # # # # # # # # # # # # #

exit_poker:

#endif
# POKER POKER POKER POKER POKER POKER POKER POKER POKER
# POKER POKER POKER POKER POKER POKER POKER POKER POKER
# POKER POKER POKER POKER POKER POKER POKER POKER POKER
# POKER POKER POKER POKER POKER POKER POKER POKER POKER
# POKER POKER POKER POKER POKER POKER POKER POKER POKER


# set up L1 cacheable range (usually it means to define fake RAM area)

#if CONFIG(NORTHBRIDGE_ALI_M1489)
	/* ALI specific, NB will not generate /KEN in noncacheable regions, probably everything outside RAM */

	/*unlock chipset*/
	movb	$3, %al
	outb	%al, $0x22
	movb	$0xc5, %al
	outb	%al, $0x23

	/*enable /KEN generation for L1*/
	movb	$0x16, %al
	outb	%al, $0x22
	movb	$(0x1 | 0x30), %al
	outb	%al, $0x23

	/* FAKE max RAM, so KEN gets generated ... TODO check */
	/* when 486 chipset resets the RAM is lost anyway? */
	movb	$0x10, %al
	outb	%al, $0x22
	movb	$0x44, %al
	outb	%al, $0x23

	movb	$0x11, %al
	outb	%al, $0x22
	movb	$0x44, %al
	outb	%al, $0x23

	/* TODO ?? enable 0xe0000 seems to be used before northbridge init */
	/* bootblock_soc_early_init which sets northbridge is called from e0000 region*/
	/* nevermind fffe0000 is not decoded in ALI and e0000 is not used in CB*/
	movb	$0x12, %al
	outb	%al, $0x22
	movb	$0x81, %al
	outb	%al, $0x23
#elif CONFIG(NORTHBRIDGE_UMC_UM8881)


#if 0	/* ========================= PCI DUMP ================ */
	movb	$0x0, %cl
pci_dump_loop:
	movw	$0xcf8, %dx
	movl	$ UMC_PCI_BDFR(0, 0x12, 1, 0), %eax
	movb	%cl, %al
	outl	%eax, %dx

	movw	$0xcfc, %dx
	and	$0x3, %al
	add	%al, %dl
	inb	%dx, %al

	send_uart(UART_BASE)

	inc	%cl

	//overflow
	cmp	$0x00, %cl
//	cmp	$0xb0, %cl
	jne	pci_dump_loop
#endif	/* ========================= PCI DUMP ================ */

	/*
	expects from poweron/reset:
		L2 disabled
		no forced L2 hit
		disabled ROM shadow, ROM cacheable
	*/

	/* define fake RAM 4x 32MB */
	/* CONFIG_DCACHE_RAM_BASE must be in range to L1 able to work as CAR */
	pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x52), $0x77)

	/* 2 double sided banks in both */
	pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x53), $0x0f)

	/* TODO EXPERIMENTAL set L1 bit ??? */
	# # # pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x51), $0x02)

#endif

#if 0
	post_code(/*POST_SOC_ENABLE_CACHE*/ '5')
#endif

	/*
	https://en.wikipedia.org/wiki/Control_register#CR0
	intel 486 pdf page 53
	*/

	/* Enable cache (CR0.CD = 0, CR0.NW = 0) */
	movl	%cr0, %eax
	andl	$(~(CR0_CacheDisable | CR0_NoWriteThrough)), %eax
	invd
	movl	%eax, %cr0

	/*
		p19 i486
		every mem write now goes outside CPU
		read will fill L1
	*/

	post_code(/*POST_SOC_FILL_CACHE*/ '6')

	/*
	address is defined in CONFIG_DCACHE_RAM_BASE
	must be in a defined RAM range to be captured in L1
	shouldn't be in first 1MB so it won't interfere with ROMs
	*/

	/* Fill cache lines */
	movl	$_car_region_start, %esi
	movl	$_car_region_end, %ecx
	sub	%esi, %ecx
	shr	$2, %ecx
	cld
	rep	lodsl	/* cld? DF=0 */

	post_code(POST_SOC_DISABLE_CACHE)	/* '-' */

	/* Enable Cache-as-RAM mode by disabling cache. */
	movl	%cr0, %eax
	orl	$(CR0_CacheDisable | CR0_NoWriteThrough), %eax
	movl	%eax, %cr0

	/* NOW in cache as RAM */

	post_code('A')

#if CONFIG(NORTHBRIDGE_ALI_M1489)
	/* disable L1 /ken generation in chipset */
	movb	$0x16, %al
	outb	%al, $0x22
	movb	$0x30, %al
	outb	%al, $0x23

#elif CONFIG(NORTHBRIDGE_UMC_UM8881)
	/* disable RAM (L1 CAR is now filled) */
	pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x52), $0x00)
	pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x53), $0x00)
#endif



#if 0
/*
FORCE table

linux
50: fb a4 e6 8f 03 c1 ff 0f ff 0f 0d 2c e0 30 00 ff
after this
50: fb a4 e6 8f 01 00 ff 0f ff 0f 0d 00 e0 30 00 ff
*/
pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x04), $0x0f)
# pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x50), $0xfb)
pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x51), $0xa4)
pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x52), $0xe6)
pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x53), $0x8f)
//pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x54), $0x03)
//pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x55), $0xc1)
pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x56), $0xff)
pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x57), $0x0f)
//pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x58), $0xff)
//pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x59), $0x0f)
pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x5a), $(0x0d /*| 0x40*/))	//?force L2 hit
pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x5b), $0x2c)
pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x5c), $0xe0)
pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x5d), $0x30)
pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x60), $0x00)
pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x61), $0xf4)
pci_write(UMC_PCI_BDFR(0, 0x10, 0, 0x62), $0x01)
////////////////////////////
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0x04), $0x07)	//0xf with special cycle
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0x40), $0x04)
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0x41), $0x42)
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0x42), $0x08)
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0x43), $0x9a)
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0x44), $0xbc)
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0x45), $0x00)
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0x46), $0x1f)	//disabled INTC
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0x47), $0x7f)
# pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0x50), $0x80 /*| 1*/)
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0x51), $0x40 /*| 1*/)
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0x56), $0x01)
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0x57), $0x30)
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0x70), $0x6f)
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0x71), $0xff)
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0x72), $0x01)
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0x90), $0x02)
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0x91), $0x00)
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0x92), $0x1f)
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0xa0), $0x00)
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0xa1), $0x80)
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0xa2), $0x00)
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0xa3), $0x00)
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0xa4), $0x81)
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0xa5), $0x00)
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0xa6), $0x00)
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0xa7), $0xef)
pci_write(UMC_PCI_BDFR(0, 0x12, 0, 0xa8), $0x28)
#endif


post_code('D')


#if 0	/* ========================= PCI DUMP ================ */
	movb	$0x0, %cl
pci_dump_loop2:
	movw	$0xcf8, %dx
	movl	$ UMC_PCI_BDFR(0, 0x10, 0, 0), %eax
	movb	%cl, %al
	outl	%eax, %dx

	movw	$0xcfc, %dx
	and	$0x3, %al
	add	%al, %dl
	inb	%dx, %al

	send_uart(UART_BASE)

	inc	%cl

	cmp	$0x80, %cl
	jne	pci_dump_loop2
#endif /* PCI DUMP */

	/*
		now no write goes outside (to RAM) if in hit region
		cache lines will not fill
		NW must be 1, or else CPU will keeps writing to RAM even on hits
	*/

	/* Clear the cache memory region. There should not be any traffic to RAM */
	movl	$_car_region_start, %edi
	movl	$_car_region_end, %ecx
	sub	%edi, %ecx
	shr	$2, %ecx

	xorl	%eax, %eax
	/* movl	$0x12345678, %eax */

	cld
	rep	stosl	/* cld? DF=0 */

	post_code(/*POST_BOOTBLOCK_BEFORE_C_ENTRY*/ 'X')

	//NOTICE _ecar_stack moved lower in linker script
	movl	$_ecar_stack, %esp

	/* Align the stack and keep aligned for call to bootblock_c_entry() */
	and	$0xfffffff0, %esp	/*lower 4 bits always 0*/
	sub	$4, %esp		/*== push 4 bytes*/

	/*
		Restore the BIST result, should be still in EBP
		set in entry32 bootblock_protected_mode_entry

		no DX stored, could be still intact? unless outb/inb with dx
		can be compressed to eax (BIST != 0 failed -> halt)
	*/

	pushl	%ebp

	/* Dummy timestamp - not implemented on 486 */
	pushl	$0
	pushl	$0


	//10:08000ff0
	post_code('{')

#if 0
	mov	%ss,%ax
	send_uart(UART_BASE)
	mov	%ah,%al
	send_uart(UART_BASE)

	mov	%esp,%eax
	send_uart(UART_BASE)
	shr $0x8, %eax
	send_uart(UART_BASE)
	shr $0x8, %eax
	send_uart(UART_BASE)
	shr $0x8, %eax
	send_uart(UART_BASE)


	pushl	$0x42434445
	pop	%eax
	send_uart(UART_BASE)
	shr $0x8, %eax
	send_uart(UART_BASE)
	shr $0x8, %eax
	send_uart(UART_BASE)
	shr $0x8, %eax
	send_uart(UART_BASE)
#endif

//////

#if 0
	call	test
test:
	pop	%eax
	send_uart(UART_BASE)
	shr $0x8, %eax
	send_uart(UART_BASE)
	shr $0x8, %eax
	send_uart(UART_BASE)
	shr $0x8, %eax
	send_uart(UART_BASE)
#endif


	post_code('}')

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
	post_code(/*POST_BOOTBLOCK_BEFORE_C_ENTRY*/ '8')
	#endif

/* before_c_entry: */
	call	bootblock_c_entry_bist
	/* Never returns */
.Lhlt:
	post_code(/*POST_DEAD_CODE*/ 'H')
	hlt
	jmp	.Lhlt

pagetables_name:
	.string "pagetables"
