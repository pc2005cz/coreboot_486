#ifndef SOUTHBRIDGE_ALI_M1487_REGS_H
#define SOUTHBRIDGE_ALI_M1487_REGS_H

#include <arch/io.h>
#include <types.h>

/*

static __always_inline
u8 sb_read_cfg8(u16 idx)
{
	outw(idx, 0x22);
	return inb(0x23);
}


static __always_inline
u16 sb_read_cfg16(u16 idx)
{
	outw(idx, 0x22);
	return inw(0x23);
}


static __always_inline
void sb_write_cfg8(u16 idx, u8 val)
{
	outw(idx, 0x22);
	outb(val, 0x23);
}


static __always_inline
void sb_write_cfg16(u16 idx, u16 val)
{
	outw(idx, 0x22);
	outw(val, 0x23);
}
*/

#endif /* NORTHBRIDGE_ALI_M1487_REGS_H */
