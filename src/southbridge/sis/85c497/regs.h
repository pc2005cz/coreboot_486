#ifndef SOUTHBRIDGE_SIS_85C497_REGS_H
#define SOUTHBRIDGE_SIS_85C497_REGS_H

#include <arch/io.h>
#include <types.h>


//builtin 206 timing control
#define DMA_CFG		0x1
//CPU to 206 RW cycle wait state: 0:1ws,1:2ws,2:3ws,3:4ws
#define DMA_CFG_CPU206_WS	(3U<<6)
//16bit DMA wait state: 0:1ws,1:2ws,2:3ws,3:4ws
#define DMA_CFG_DMA16		(3U<<4)
//8bit DMA wait state: 0:1ws,1:2ws,2:3ws,3:4ws
#define DMA_CFG_DMA8		(3U<<2)
//DMAMEMR assertion timing: 0:delayed by 1clk after XIOR,1:same clk as XIOR
#define DMA_CFG_DMAMEMR_DELAY	(1U<<1)
//DMA clock: 0:DMA clock = ISA clock/2, 1:DMA clock = ISA clock
#define DMA_CFG_CLK		(1U<<0)


//ISA clock
#define ISA_CFG0	0x70
//ISA clock freq: 0:7.159MHz,1:1/4 PCI,2:1/3 PCI,3:reserved
#define ISA_CFG0_CLK		(3U<<6)


//ISA bus timing control
#define ISA_CFG1	0x71
//ISA 16bit IO recovery time (in busclock): 0:5clk,1:4clk,2:3clk,3:2clk
#define ISA_CFG1_RECOVERY16	(3U<<6)
//ISA 8bit IO recovery time (in busclock): 0:8clk,1:5clk,2:4clk,3:3clk
#define ISA_CFG1_RECOVERY8	(3U<<4)
//ISA 16bit IO wait states: 0:2ws,1:1ws
#define ISA_CFG1_WAIT16		(1U<<2)
//ISA 8bit IO wait states: 0:5ws,1:4ws
#define ISA_CFG1_WAIT8		(1U<<1)


//SMOUT [7:0] pins
#define SMOUT_L		0x72


//BIOS timer, 16bit
//counts after predefined IO port
#define BIOS_TMR	0x73
//BIOS timer base A[15:2]
//points to IO space, 32bit register (only lowest 8bit valid), 8/16/32 access
#define BIOS_TMR_BASE		(0x3fffU<<2)
//BIOS timer access enable: 1:enable
#define BIOS_TMR_EN		(1U<<0)


//DMA, deturbo,pin76,pin77 mux
#define MISC_CFG	0x75
//DMA 32bit address mode: 1:enable
#define MISC_CFG_DMA32		(1U<<7)
//deturbo: 1:enable
#define MISC_CFG_DETURBO_EN	(1U<<6)
//software deturbo: 0:off,1:on
#define MISC_CFG_DETURBO_ON	(1U<<5)
//deturbo hold time: 0:hold 4us every 12us,1:hold 8us every 12us
#define MISC_CFG_DETURBO_TIME	(1U<<4)
//pin76 mux: 0:SMOUT0,1:SMOUTW0
#define MISC_CFG_PIN76		(1U<<3)
//pin77 mux: 0:SMOUT1,1:SMOUTW1
#define MISC_CFG_PIN77		(1U<<2)


//SMOUT [15:8] pins
#define SMOUT_H		0x76


//interrupt edge/level, 16bit
//0:edge,1:level
#define EDGE_LEVEL_IRQ		0x4d0
#define EDGE_LEVEL_IRQ15	(1U<<15)
#define EDGE_LEVEL_IRQ14	(1U<<14)
#define EDGE_LEVEL_IRQ12	(1U<<12)
#define EDGE_LEVEL_IRQ11	(1U<<11)
#define EDGE_LEVEL_IRQ10	(1U<<10)
#define EDGE_LEVEL_IRQ9		(1U<<9)
#define EDGE_LEVEL_IRQ7		(1U<<7)
#define EDGE_LEVEL_IRQ6		(1U<<6)
#define EDGE_LEVEL_IRQ5		(1U<<5)
#define EDGE_LEVEL_IRQ4		(1U<<4)
#define EDGE_LEVEL_IRQ3		(1U<<3)


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


#endif /* SOUTHBRIDGE_SIS_85C497_REGS_H */
