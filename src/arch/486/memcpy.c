/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
#include <stdbool.h>
#include <asan.h>

#include <console/console.h>

#if CONFIG(NORTHBRIDGE_ALI_M1489)	//pc2005 enable for ALI
#	include "../../northbridge/ali/m1489/regs.h"
#elif CONFIG(NORTHBRIDGE_UMC_UM8881)
// #	include "../../northbridge/umc/um8881/regs.h"
#endif


#if CONFIG(NORTHBRIDGE_ALI_M1489)	//pc2005 enable for ALI

//original
static void __memcpy_raw(void *dest, const void *src, size_t n)
{
	unsigned long d0, d1, d2;

	asm volatile(
#if ENV_X86_64
		"rep ; movsd\n\t"
		"mov %4,%%rcx\n\t"
#else
		"rep ; movsl\n\t"
		"movl %4,%%ecx\n\t"
#endif
		"rep ; movsb\n\t"
		: "=&c" (d0), "=&D" (d1), "=&S" (d2)
		: "0" (n >> 2), "g" (n & 3), "1" (dest), "2" (src)
		: "memory"
	);

	// return dest;
}


void *memcpy(void *dest, const void *src, size_t n)
{

#if (ENV_ROMSTAGE && CONFIG(ASAN_IN_ROMSTAGE)) || \
	(ENV_RAMSTAGE && CONFIG(ASAN_IN_RAMSTAGE))
	check_memory_region((unsigned long)src, n, false, _RET_IP_);
	check_memory_region((unsigned long)dest, n, true, _RET_IP_);
#endif

	// printk(BIOS_DEBUG, "!!MCPY %p<-%p n:%u\n", dest, src, n);

	uintptr_t dstaddr = (uintptr_t) dest;
	uintptr_t srcaddr = (uintptr_t) src;

	if ((ENV_RAMSTAGE) && (srcaddr >= 0xe0000) && (srcaddr < 0x100000)) {
		size_t frag_size = n;

		//pc2005
		DIS_SEGMENT_E;	//so we can have RAM
		EN_SHADOW_EF;	//enable RAM	TODO check if only shadow is enough (higher priority)
		EN_ROM_A31;	//enable FFFF

		//now we can switch between E and F in FFFF

		if (srcaddr < 0xf0000) {
			//map E to FFFF
			EN_SWAP_EF;

			frag_size = n < (0xf0000-srcaddr) ? n: 0xf0000-srcaddr;

			// printk(BIOS_DEBUG, " ROME a:0x%lx s:0x%x\n", srcaddr, frag_size);

			__memcpy_raw((void*)dstaddr, (void*)(srcaddr | 0xffff0000), frag_size);

			dstaddr += frag_size;	//next start
			srcaddr += frag_size;	//should be either 0xf0000 or less (and frag == 0)
			frag_size = n - frag_size;

			// printk(BIOS_DEBUG, " ROMEnew a:0x%lx s:0x%x\n", srcaddr, frag_size);
		}

		if (frag_size) {
			//rest of buffer should be always in 0xf0000

			if (frag_size > 0x10000) {
				printk(BIOS_INFO, "overflowed 0xfffff!!!\n");
			}

			//map F to FFFF
			DIS_SWAP_EF;

			// printk(BIOS_DEBUG, " ROMF a:0x%lx s:0x%x\n", srcaddr, frag_size);

			__memcpy_raw((void*)dstaddr, (void*)(srcaddr | 0xffff0000), frag_size);
		}
	} else {
		__memcpy_raw(dest, src, n);
	}

	return dest;
}

#elif CONFIG(NORTHBRIDGE_UMC_UM8881)

void *memcpy(void *vdest, const void *vsrc, size_t bytes)
{
	const char *src = vsrc;
	char *dest = vdest;
	int i;

	for (i = 0; i < (int)bytes; i++)
		dest[i] = src[i];

	return vdest;
}

#endif
