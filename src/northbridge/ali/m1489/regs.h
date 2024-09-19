#ifndef NORTHBRIDGE_ALI_M1489_REGS_H
#define NORTHBRIDGE_ALI_M1489_REGS_H

/*
 * CFG _0 _1 _2 _3 _4 _5 _6 _7 _8 _9 _a _b _c _d _e _f
 * 00: ff 0d 0d c5 ff ff ff ff ff ff ff ff ff ff ff ff
 * 10: 66 66 01 03 9f 50 07 02 ff 54 5f 45 00 00 08 ff
 * 20: 08 32 1d ff ff 80 00 00 04 32 45 d7 ff ff ff ff
 * 30: 10 fd e0 00 00 00 e0 ee 00 00 00 00 03 01 00 00
 * 40: 2b ff 00 00 1f 80 ff ff ff ff ff ff ff ff ff ff
 *
 * 0x20 was 0x08 (or 0x2c?)
 *
 *
 * original
 *	0x12 nema zaplej hidden refresh?
 *	0x12 E segment v ROM
 *	0x13 no shadow segC ...
 *	0x20 pouze CPU to PCI write byte merge
 *	0x21 no improve reliability?
 *	0x22 no reserved bit to 1
 *
 *
 * single SIMM, front
 * CFG _0 _1 _2 _3 _4 _5 _6 _7 _8 _9 _a _b _c _d _e _f
 * 00: ff 0d 0d c5 ff ff ff ff ff ff ff ff ff ff ff ff
 * 10: 6f 6f 01 03 9f 50 07 02 ff 54 5a 45 00 00 08 ff
 * 20: 08 32 1d ff ff 80 00 00 04 32 45 d7 ff ff ff ff
 * 30: 10 fd e0 00 00 00 e0 ee 00 00 00 00 03 05 00 00
 * 40: 2b ff 00 00 1f 80 ff ff ff ff ff ff ff ff ff ff
 *
 *
 * early TAG rising 0x16.7 works only in back slot?? :-D
 *
 */

#include <arch/io.h>



//rom CS at 0xe0000
#define EN_SEGMENT_E	cfg_write(0x12, cfg_read(0x12) | 1)
#define DIS_SEGMENT_E	cfg_write(0x12, cfg_read(0x12) & ~1)

//shadow 0xe0000-0xf0000, read/write control
#define EN_SHADOW_EF	cfg_write(0x14, cfg_read(0x14) | 0x3f)
#define DIS_SHADOW_EF	cfg_write(0x14, cfg_read(0x14) & ~0x3f)

//enabled A31 ROM recognition 0xffff0000
#define EN_ROM_A31	cfg_write(0x21, cfg_read(0x21) | 0x20)
#define DIS_ROM_A31	cfg_write(0x21, cfg_read(0x21) & ~0x20)


//swap E F
#define EN_SWAP_EF	cfg_write(0x2b, cfg_read(0x2b) | 0x20)
#define DIS_SWAP_EF	cfg_write(0x2b, cfg_read(0x2b) & ~0x20)




#define PORT_INDEX	0x22
#define PORT_DATA	0x23

// #define R01_HW	0x1


struct ali_cfg_entry {
	u8 idx;
	u8 val;
};


//TODO spinlock?

static __always_inline
u8 cfg_read(u8 idx)
{
	outb(idx, PORT_INDEX);
	return inb(PORT_DATA);
}

static __always_inline
void cfg_write(u8 idx, u8 val)
{
	outb(idx, PORT_INDEX);
	outb(val, PORT_DATA);
}

static __always_inline
void cfg_modify(u8 idx, u8 clear, u8 set)
{
	u8 val;

	outb(idx, PORT_INDEX);

	val = inb(PORT_DATA);
	val &= ~clear;
	val |= set;
	outb(val, PORT_DATA);
}

static __always_inline
void cfg_unlock(void)
{
	cfg_write(0x03, 0xc5);
}





#endif /* NORTHBRIDGE_ALI_M1489_REGS_H */
