/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ARCH_MEMLAYOUT_H
#define __ARCH_MEMLAYOUT_H

/* Intel386 psABI requires a 16 byte aligned stack. */
#define ARCH_STACK_ALIGN_SIZE 16

/* 32bit pointers on 486 */
#define ARCH_POINTER_ALIGN_SIZE 4

#endif /* __ARCH_MEMLAYOUT_H */
