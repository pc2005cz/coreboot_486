/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __X86_POST_CODE_H__
#define __X86_POST_CODE_H__

#include <commonlib/console/post_codes.h>

#if CONFIG(POST_IO) && !(ENV_BOOTBLOCK && CONFIG(NO_EARLY_BOOTBLOCK_POSTCODES))

#define post_init		\
	movw	$0x3f9, %dx;	\
	movb	$0x0, %al;	\
	outb	%al, %dx;	\
	movw	$0x3fb, %dx;	\
	movb	$0x80, %al;	\
	outb	%al, %dx;	\
	movw	$0x3f8, %dx;	\
	movb	$1, %al;	\
	outb	%al, %dx;	\
	movw	$0x3f9, %dx;	\
	movb	$0x0, %al;	\
	outb	%al, %dx;	\
	movw	$0x3fb, %dx;	\
	inb	%dx, %al;	\
	andb	$0x7f, %al;	\
	outb	%al, %dx;	\
	movw	$0x3f9, %dx;	\
	movb	$0x00, %al;	\
	outb	%al, %dx;	\
	movw	$0x3fa, %dx;	\
	movb	$0xc7, %al;	\
	outb	%al, %dx;	\
	movw	$0x3fb, %dx;	\
	movb	$0x03, %al;	\
	outb	%al, %dx;	\
	movw	$0x3fc, %dx;	\
	movb	$0x01, %al;	\
	outb	%al, %dx

/*
#define post_code(value)	\
	movb    $value, %al;    \
	outb    %al, $CONFIG_POST_IO_PORT
*/

#define post_code(value)	\
	movb	$value, %al;    \
	movw	$0x3f8, %dx;	\
	outb	%al, %dx;	\
1:	movw	$0x3fd, %dx;	\
	inb	%dx, %al;	\
	testb	$0x20, %al;	\
	jz	1b

#else
#define post_init
#define post_code(value)
#endif

#endif /* __X86_POST_CODE_H__ */
