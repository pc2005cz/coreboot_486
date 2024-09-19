#ifndef NORTHBRIDGE_SIS_85C496_REGS_H
#define NORTHBRIDGE_SIS_85C496_REGS_H

#define HOST_BRIDGE	PCI_DEV(0, 0, 0)

//set after CPU type determined
#define CPUCFG		0x40
//0:disable, 1:enable cpu burst write
#define CPUCFG_BURST_W_EN	(1U<<6)
//0:disable, 1:enable internal cache write back
#define CPUCFG_L1_WB_EN		(1U<<5)
//0:i486dx2, am486dx4, 1:i486sl/dx4, 2:p24/am486 enh/cx 5x86, 3:am486dxl, 5: cx486dx/dx2
#define CPUCFG_CPU_TYPE		(7U<<2)
//0: slowest 50MHz, 1:slower 40MHz, 2:faster 33MHz, 3:fastest 25MHz
#define CPUCFG_DRAM_SPEED	(3U<<0)


//DRAM configuration: refresh, timing, type, ...
#define DRAMCFG		0x41
//1: parity check enable
#define DRAMCFG_PARITY_EN	(1U<<7)
//0:256k/512k, 1: 1M/2M, 2:4M/8M/16M/32M, 3:reserved
#define DRAMCFG_TYPE		(3U<<5)
//0:normal, 1:1/4 refresh
#define DRAMCFG_SLOW_REFRESH	(1U<<4)
//ras to cas delay: 0:3T, 1:2T
#define DRAMCFG_RAS2CAS		(1U<<3)
//write cycle post: 0: 1 wait state, 1:0 wait state
#define DRAMCFG_WR_POST_EN	(1U<<2)
//write cycle CAS width in cpu T: 0:2T, 1:1T
#define DRAMCFG_WR_CAS_WIDTH	(1U<<1)
//CAS precharge in cpu T: 0:2T, 1:1T
#define DRAMCFG_CAS_PRECHARGE	(1U<<0)


//cache configuration, 16bit
#define CACHECFG	0x42
//burst order mode: 0:toggle (intel), 1:linear burst
#define CACHECFG_BURST_MODE	(1U<<15)
//L2 cache single/burst write cycle (cpu T): 0:3T/2T, 1:2T/1T
#define CACHECFG_WRITE_CYCLE	(1U<<11)
//L2 cache burst read next wait state (cpu T): 0:1T, 1:2T
#define CACHECFG_READ_NEXT	(1U<<10)
//L2 cache read lead off wait state (cpu T): 0:3T, 1:2T
#define CACHECFG_CYCLE_WS	(1U<<9)
//L2 cache write back/throught: 0:WT, 1:WB
#define CACHECFG_WRITE_MODE 	(1U<<8)
//L2 cache size: 1:64kB, 2:128kB, 3:256kB, 4:512kB, 5:1MB
#define CACHECFG_SIZE		(7U<<5)
//L2 cache interleave mode: 0:single bank,non-interleaved, 1:double bank, interleaved
#define CACHECFG_INTERLEAVED	(1U<<4)
//L2 cache dirty bit: 0:enabled dirty pin, 1:force dirty bit
#define CACHECFG_DIRTY_EN	(1U<<3)
//L2 cache tag mode: 0:8bit tag (pin 128 = TA7), 1:7bit tag (pin 128 = dirty)
#define CACHECFG_TAG_WIDTH	(1U<<2)
//L2 cache test: 0:normal, 1:test = always hit
#define CACHECFG_TEST_EN	(1U<<1)
//L2 cache enable: 1:enabled
#define CACHECFG_EN		(1U<<0)


//shadow config, 16bit
//shadow area 0xc0000-0xfffff
#define SHADOWCFG	0x44
//L1 cacheable: 0:L2 area is cacheable by L1, 1:non-cacheable by L1
//for P24 during WB
#define SHADOWCFG_L1_CACHEABLE	(1U<<11)
//PCI/ISA master access shadow RAM area enable: 1:enable
#define SHADOWCFG_AREA_EN	(1U<<10)
//area shadow read enable: 0:disable, 1:enable
#define SHADOWCFG_RD_EN		(1U<<9)
//area shadow write enable: 0:enable, 1:disable
#define SHADOWCFG_WR_EN		(1U<<8)
//subarea 0xf8000-0xfffff: 1:enable
#define SHADOWCFG_F8000_EN	(1U<<7)
//subarea 0xf0000-0xf7fff: 1:enable
#define SHADOWCFG_F0000_EN	(1U<<6)
//subarea 0xe8000-0xeffff: 1:enable
#define SHADOWCFG_E8000_EN	(1U<<5)
//subarea 0xe0000-0xe7fff: 1:enable
#define SHADOWCFG_E0000_EN	(1U<<4)
//subarea 0xd8000-0xdffff: 1:enable
#define SHADOWCFG_D8000_EN	(1U<<3)
//subarea 0xd0000-0xd7fff: 1:enable
#define SHADOWCFG_D0000_EN	(1U<<2)
//subarea 0xc8000-0xcffff: 1:enable
#define SHADOWCFG_C8000_EN	(1U<<1)
//subarea 0xc0000-0xc7fff: 1:enable
#define SHADOWCFG_C0000_EN	(1U<<0)


//set area as cacheable in or L1,L2
#define AREACTRL	0x46
//0xf8000-0xfffff RAM cacheable: 0:noncacheable, 1:cacheable
#define AREACTRL_F8000_EN	(1U<<7)
//0xf0000-0xf7fff RAM cacheable: 0:noncacheable, 1:cacheable
#define AREACTRL_F0000_EN	(1U<<6)
//0xe8000-0xeffff RAM cacheable: 0:noncacheable, 1:cacheable
#define AREACTRL_E8000_EN	(1U<<5)
//0xe0000-0xe7fff RAM cacheable: 0:noncacheable, 1:cacheable
#define AREACTRL_E0000_EN	(1U<<4)
//0xd8000-0xdffff RAM cacheable: 0:noncacheable, 1:cacheable
#define AREACTRL_D8000_EN	(1U<<3)
//0xd0000-0xd7fff RAM cacheable: 0:noncacheable, 1:cacheable
#define AREACTRL_D0000_EN	(1U<<2)
//0xc8000-0xcffff RAM cacheable: 0:noncacheable, 1:cacheable
#define AREACTRL_C8000_EN	(1U<<1)
//0xc0000-0xc7fff RAM cacheable: 0:noncacheable, 1:cacheable
#define AREACTRL_C0000_EN	(1U<<0)


//address decoder ISA vs PCI
#define ADDRDEC		0x47
//concurrent access CPU-RAM and PCI-PCI: 1:enable
#define ADDRDEC_CONCURRENT_EN	(1U<<4)
//0xfff80000 - 0xfffdffff forward to: 0: PCI then ISA, 1:PCI only (allows posted write)
#define ADDRDEC_FFF80000_FWD	(1U<<3)
//0xb0000 - 0xbffff forward to: 0: PCI then ISA, 1:PCI only (allows posted write)
#define ADDRDEC_B0000_FWD	(1U<<2)
//0xa0000 - 0xaffff forward to: 0: PCI then ISA, 1:PCI only (allows posted write)
#define ADDRDEC_A0000_FWD	(1U<<1)
//DRAM A,B,D,E segment relocate enable: 1: if DRAM <=8M append A,B,D,E segments on top
#define ADDRDEC_SEGMENT_RELOCATE_EN	(1U<<0)


//8 registers for 8 DRAM banks, boundaries for A[27:20]
//0:0M, 1:1M, 0x80:128M
//ex.: single side 1MB, double size 8MB, double side 2MB, single side 16MB
//(1, 1), (5, 9), (0xa,0xb), (0x1b, 0x1b)
//increases with the bank size
#define DRAMBANK_BOUNDARY_BASE	0x48


//exclusive area 0, 16bit
#define EXCL_AREA_0	0x50
//select area 0: 0: non-cacheable area, 1: PCI memory hole (area sent to PCI)
#define EXCL_AREA_0_SEL		(1U<<15)
//0: 0kB (disabled), 1:64kB,2:128kB,3:256kB,4:512kB,5:1MB,6:2MB,7:4MB
#define EXCL_AREA_0_SIZE	(7U<<12)
//base address A[27:16]
#define EXCL_AREA_0_BASE	(0xfffU<<0)


//exclusive area 1, 16bit
#define EXCL_AREA_1	0x52
//select area 1: 0: non-cacheable area, 1: PCI memory hole (area sent to PCI)
#define EXCL_AREA_1_SEL		(1U<<15)
//0: 0kB (disabled), 1:64kB,2:128kB,3:256kB,4:512kB,5:1MB,6:2MB,7:4MB
#define EXCL_AREA_1_SIZE	(7U<<12)
//base address A[27:16]
#define EXCL_AREA_1_BASE	(0xfffU<<0)


//exclusive area 2, 16bit
#define EXCL_AREA_2	0x54
//select area 2: 0: non-cacheable area, 1: PCI memory hole (area sent to PCI)
#define EXCL_AREA_2_SEL		(1U<<15)
//0: 0kB (disabled), 1:64kB,2:128kB,3:256kB,4:512kB,5:1MB,6:2MB,7:4MB
#define EXCL_AREA_2_SIZE	(7U<<12)
//base address A[27:16]
#define EXCL_AREA_2_BASE	(0xfffU<<0)


//PCI, keyboard cfg
#define ARB_CFG		0x56
//PCI arbitration, CPU request is: 0: weakest, 1: weaker, 2:stronger, 3:strongest
#define ARB_CFG_CPU_REQ		(3U<<6)
//CPU to PCI memory post write buffer enable: 1: enable
#define ARB_CFG_PCI_POST_WR_EN	(1U<<5)
//CPU to PCI memory burst write enable: 1: enable
#define ARB_CFG_PCI_BURST_WR_EN	(1U<<4)
//KB reset, init cpu command pending during smi control, 1:enable
#define ARB_CFG_KB_RST		(1U<<2)
//emulated PS/2 KB 92h port control: 1:enable
#define ARB_CFG_KB_P92_EMU_EN	(1U<<1)
//emulated keyboard fast reset latency: 0:2us, 1: 6us
#define ARB_CFG_KB_RST_LATENCY	(1U<<0)


//output pin config
#define PIN_CFG		0x57
//pci master burst rw main memory control: 1:enable
#define PIN_CFG_PCI_MASTER_BURST_EN	(1U<<7)
//CAS[3:0] output buffer driving strength: 0:12mA, 1:24mA
#define PIN_CFG_CAS_STRENGTH	(1U<<6)
//MA[10:0] output buffer driving strength: 0:12mA, 1:24mA
#define PIN_CFG_MA_STRENGTH	(1U<<5)
//MWE output buffer driving strength: 0:12mA, 1:24mA
#define PIN_CFG_MWE_STRENGTH	(1U<<4)
//mux, pin 207, pin 204: 0:pin207=LBD,pin204=lrdy, 1:pin207=preq2,pin204=pgnt2
#define PIN_CFG_204_207		(1U<<3)
//mux pins: 157/126/127
/*
pin	157	126	127
000	ras7	dirty	dirtywe
100	ma11	dirty	dirtywe
010	ras7	ma11	dirtywe
110	ma11	ma11	dirtywe
0x1	ras7	preq3	pgnt3
1x1	ma11	preq3	pgnt3

	largest dram	max banks	8bit tag+dirty	pci master
000	4Mx36		8		y		3
100	16Mx36		7		y		3
010	16Mx36		8		n		3
110	16Mx36		7		n		3
0x1	4Mx36		8		n		4
1x1	16Mx36		7		n		4
*/
#define PIN_CFG_157_126_127	(7U<<0)


//IDE, VESA config, 16bit
#define IDE_CFG	0x58
//IDE post write buffer enable: 1:enable
#define IDE_POST_WBUF_EN	(1U<<15)
//IDE dual prefetch buffers: 0x: disable ch0+ch1, 10:enable ch0+ch1, 11:enable only ch0
#define IDE_PREFETCH		(3U<<13)
//IDE addr setup time (cpu clks): 0:8clk, 1:1clk, 2:2, 3:3 ... 7:7clk
#define IDE_SETUP_TIME		(7U<<10)
//IDE channels IO: 0:ch0=1fxh,ch1=17xh, 1:ch0=17xh,ch1=1fxh
#define IDE_CH_PORTS		(1U<<9)
//IDE enable: 1:enable
#define IDE_EN			(1U<<8)
//IDE 1fxh enable: 1:enable
#define IDE_1FX_EN		(1U<<7)
//IDE 17xh enable: 1:enable
#define IDE_17X_EN		(1U<<6)
//IDE PMU DEVDET gen: 0:synchronous, 1:asynchronous
//to southbridge power management
#define IDE_PMU_DEVDET_MODE	(1U<<4)
//CPU init queued by stop grant mode control: 1:enable
#define IDE_CPU_INIT_BY_STOP_GRANT	(1U<<2)
//VESA ramdac IO write cycle shadow: 0:normal, 1:3c6-3c9h duplicated to PCI
#define IDE_RAMDAC_SHADOW	(1U<<1)
//VESA LBD sampling point: 0:T2, 1:T3
#define IDE_VESA_LBD_SAMPLE	(1U<<0)


//smram remapping configuration
#define SMRAM_CFG	0x5a
//ISA bus master request control "this bit must be written with 1": 1:enable
#define SMRAM_CFG_ISA_BM	(1U<<7)
//SMM A20M assetion control: 0:disabled (A20M not altered), 1:enable (A20M deasserted in SMM)
#define SMRAM_CFG_A20M		(1U<<5)
//SMRAM remapping mode:
//0:logical 0x60000-0x6ffff -> physical 0xa0000-0xaffff
//1:logical 0x60000-0x6ffff -> physical 0xb0000-0xbffff
//2:logical 0xe0000-0xeffff -> physical 0xa0000-0xaffff
//3:logical 0xe0000-0xeffff -> physical 0xb0000-0xbffff
#define SMRAM_CFG_REMAP		(3U<<3)
//SMRAM initialization remapping: 1:enable (event when not in SMM, can copy, handler)
#define SMRAM_CFG_REMAP_INIT	(1U<<2)
//SMRAM remapping: 1:enable
#define SMRAM_CFG_REMAP_EN	(1U<<1)


//programmable IO trap
#define IO_TRAP		0x5b
//IO trap 1: 0:1B, 1:2B, 4B 8B 16B 32B 64B 128B
#define IO_TRAP_AREA1		(7U<<5)
//IO trap 1: 1:enable
#define IO_TRAP_AREA1_EN	(1U<<4)
//IO trap 0: 0:1B, 1:2B, 4B 8B 16B 32B 64B 128B
#define IO_TRAP_AREA0		(7U<<1)
//IO trap 0: 1:enable
#define IO_TRAP_AREA0_EN	(1U<<0)


//programmable IO trap0, 16bit
#define IO_TRAP0_BASE	0x5c


//programmable IO trap1, 16bit
#define IO_TRAP1_BASE	0x5e


//ide controller channel 0 config, 16bit
//0: 16clk, 1:1, 2:2, ... 15:15
#define IDE_CH0		0x60
//ch0 slave drive recovery time
#define IDE_CH0_S_RECOVERY	(0xfU<<12)
//ch0 master drive recovery time
#define IDE_CH0_M_RECOVERY	(0xfU<<8)
//ch0 slave drive command active time
#define IDE_CH0_S_ACTIVE	(0xfU<<4)
//ch0 master drive command active time
#define IDE_CH0_M_ACTIVE	(0xfU<<0)


//ide controller channel 1 config, 16bit
//0: 16clk, 1:1, 2:2, ... 15:15
#define IDE_CH1		0x62
//ch1 slave drive recovery time
#define IDE_CH1_S_RECOVERY	(0xfU<<12)
//ch1 master drive recovery time
#define IDE_CH1_M_RECOVERY	(0xfU<<8)
//ch1 slave drive command active time
#define IDE_CH1_S_ACTIVE	(0xfU<<4)
//ch1 master drive command active time
#define IDE_CH1_M_ACTIVE	(0xfU<<0)


//exclusive area 3, 16bit
#define EXCL_AREA_3	0x64
//select area 3: 0: non-cacheable area, 1: PCI memory hole (area sent to PCI)
#define EXCL_AREA_3_SEL		(1U<<15)
//0: 0kB (disabled), 1:64kB,2:128kB,3:256kB,4:512kB,5:1MB,6:2MB,7:4MB
#define EXCL_AREA_3_SIZE	(7U<<12)
//base address A[27:16]
#define EXCL_AREA_3_BASE	(0xfffU<<0)


//DRAM type EDO of FPM
#define RAM_EDO		0x66
//bank 7 EDO: 0:fpm, 1:edo
#define RAM_EDO_BANK7	(0x1U<<7)
//bank 6 EDO: 0:fpm, 1:edo
#define RAM_EDO_BANK6	(0x1U<<6)
//bank 5 EDO: 0:fpm, 1:edo
#define RAM_EDO_BANK5	(0x1U<<5)
//bank 4 EDO: 0:fpm, 1:edo
#define RAM_EDO_BANK4	(0x1U<<4)
//bank 3 EDO: 0:fpm, 1:edo
#define RAM_EDO_BANK3	(0x1U<<3)
//bank 2 EDO: 0:fpm, 1:edo
#define RAM_EDO_BANK2	(0x1U<<2)
//bank 1 EDO: 0:fpm, 1:edo
#define RAM_EDO_BANK1	(0x1U<<1)
//bank 0 EDO: 0:fpm, 1:edo
#define RAM_EDO_BANK0	(0x1U<<0)


//miscelaneous
#define MISC	0x67
//emulated keyboard rese and A20M through ISA: 1:enable
#define MISC_EMULATED_RST	(0x1U<<7)
//emulated CPU A20M: 0:enable
#define MISC_EMULATED_A20M	(0x1U<<6)
//emulated CPU fast reset: 0:enable
#define MISC_EMULATED_RESET	(0x1U<<5)
//emulated kb 60/64h: 0:enable
#define MISC_EMULATED_KB	(0x1U<<4)
//LBIDE + this bit, mux: 2 banks of dram, pci-pci bridge, ext kbd reset
//lbide bit3	pin158	pin159
//1	x	ras6	ras5
//0	0	boff	hlock
//0	1	ras6	rc
#define MISC_158_159	(0x1U<<3)
//FPM/EDO DRAM detection: 1:enable
#define MISC_EDO_DETECT	(0x1U<<0)


//dram asymmetry, 16bit
#define DRAM_ASYMMETRIC	0x68
//bank 7: 0:dram type in reg41, 1: 1Mx36/12row/8col, 2: 2Mx36/12row/9col, 3: 4Mx36/12row/10col
#define DRAM_ASYMMETRIC_BANK7	(0x3U<<14)
//bank 6: 0:dram type in reg41, 1: 1Mx36/12row/8col, 2: 2Mx36/12row/9col, 3: 4Mx36/12row/10col
#define DRAM_ASYMMETRIC_BANK6	(0x3U<<12)
//bank 5: 0:dram type in reg41, 1: 1Mx36/12row/8col, 2: 2Mx36/12row/9col, 3: 4Mx36/12row/10col
#define DRAM_ASYMMETRIC_BANK5	(0x3U<<10)
//bank 4: 0:dram type in reg41, 1: 1Mx36/12row/8col, 2: 2Mx36/12row/9col, 3: 4Mx36/12row/10col
#define DRAM_ASYMMETRIC_BANK4	(0x3U<<8)
//bank 3: 0:dram type in reg41, 1: 1Mx36/12row/8col, 2: 2Mx36/12row/9col, 3: 4Mx36/12row/10col
#define DRAM_ASYMMETRIC_BANK3	(0x3U<<6)
//bank 2: 0:dram type in reg41, 1: 1Mx36/12row/8col, 2: 2Mx36/12row/9col, 3: 4Mx36/12row/10col
#define DRAM_ASYMMETRIC_BANK2	(0x3U<<4)
//bank 1: 0:dram type in reg41, 1: 1Mx36/12row/8col, 2: 2Mx36/12row/9col, 3: 4Mx36/12row/10col
#define DRAM_ASYMMETRIC_BANK1	(0x3U<<2)
//bank 0: 0:dram type in reg41, 1: 1Mx36/12row/8col, 2: 2Mx36/12row/9col, 3: 4Mx36/12row/10col
#define DRAM_ASYMMETRIC_BANK0	(0x3U<<0)


/* ********* southbridge 497 *********** */


//PMU config
#define PMU_CFG		0x80
//SMM enable: 1:enable
#define PMU_CFG_SMM		(0x1U<<7)
//stop clock enable: 1:enable
#define PMU_CFG_STOPCLK		(0x1U<<6)
//sw stop clock enable: 1:enable
#define PMU_CFG_SW_STOPCLK	(0x1U<<5)
//SMM request by: 0:SMI, 1:IRQ
#define PMU_CFG_SMM_REQ		(0x1U<<4)
//reserved, write 1
#define PMU_CFG_ALWAYS_1	(0x1U<<3)
//sw SMI generation: 1:enable
#define PMU_CFG_SW_GEN		(0x1U<<2)
//break switch: 1:enable
#define PMU_CFG_BREAK		(0x1U<<1)
//clock throttle: 1:enable
#define PMU_CFG_CLK_THROTTLE	(0x1U<<0)


//cpu config, deturbo, smi irq
#define PMU_CPU_TYPE	0x81
//deturbo trigger select: 0:hold cpu, 1:smi
#define PMU_DETURBO		(0x1U<<7)
//set right after CPU detection:
//0:i486dx2/am486dx4
//1:i486sl/i486dx4
//2:p24/am486dx2enh/cx5x86
//5:cx486dx/cx486dx2
#define PMU_PROC_TYPE		(0x7U<<2)
//smi irq for nonSMI CPU: 0:irq10, 1:irq11, 2:irq12, 3:irq15
#define PMU_SEL_SMI_IRQ		(0x3U<<0)


//port 22h mirror, read only
#define PMU_PORT22	0x82


//port 70h mirror, read only
#define PMU_PORT70	0x83


//misc
#define PMU_MISC2	0x84
//deturbo switch blocking clear: 1:clear blocking
#define PMU_MISC2_DETURBO_UNMASK	(0x1U<<2)
//break switch blocking clear: 1:clear blocking
#define PMU_MISC2_SWITCH_UNMASK		(0x1U<<1)
//stpclk assertion: 1:assert stpclk
#define PMU_MISC2_STPCLK_ASSERT		(0x1U<<0)


//stpclk event control
#define STPCLK_EVENT	0x85
//asserted if fast timer: 1:enable
#define STPCLK_EVENT_FTIMER	(0x1U<<7)
//asserted if generic timer: 1:enable
#define STPCLK_EVENT_GTIMER	(0x1U<<6)
//asserted if slow timer: 1:enable
#define STPCLK_EVENT_STIMER	(0x1U<<5)
//de-asserted if break switch: 1:enable
#define STPCLK_EVENT_BREAK	(0x1U<<4)
//de-asserted if irq: 1:enable
#define STPCLK_EVENT_IRQ	(0x1U<<3)
//de-asserted if NMI: 1:enable
#define STPCLK_EVENT_NMI	(0x1U<<2)
//de-asserted if DMA: 1:enable
#define STPCLK_EVENT_DMA	(0x1U<<1)
//de-asserted if PCI master: 1:enable
#define STPCLK_EVENT_PCIM	(0x1U<<0)


//stpclk irq mask, 16bit
#define STPCLK_IRQ	0x86
//use irq15: 1:select
#define STPCLK_IRQ15		(0x1U<<15)
//use irq14: 1:select
#define STPCLK_IRQ14		(0x1U<<14)
//use irq13: 1:select
#define STPCLK_IRQ13		(0x1U<<13)
//use irq12: 1:select
#define STPCLK_IRQ12		(0x1U<<12)
//use irq11: 1:select
#define STPCLK_IRQ11		(0x1U<<11)
//use irq10: 1:select
#define STPCLK_IRQ10		(0x1U<<10)
//use irq9: 1:select
#define STPCLK_IRQ9		(0x1U<<9)
//use irq8: 1:select
#define STPCLK_IRQ8		(0x1U<<8)
//use irq7: 1:select
#define STPCLK_IRQ7		(0x1U<<7)
//use irq6: 1:select
#define STPCLK_IRQ6		(0x1U<<6)
//use irq5: 1:select
#define STPCLK_IRQ5		(0x1U<<5)
//use irq4: 1:select
#define STPCLK_IRQ4		(0x1U<<4)
//use irq3: 1:select
#define STPCLK_IRQ3		(0x1U<<3)
//use irq2: 1:select
#define STPCLK_IRQ2		(0x1U<<2)
//use irq1: 1:select
#define STPCLK_IRQ1		(0x1U<<1)
//use irq0: 1:select
#define STPCLK_IRQ0		(0x1U<<0)


//timer control
#define TMR_CTRL	0x88
//slow timer: 0:keeps running when STPCLK, 1:disabled during STPCLK, reenabled after
#define TMR_CTRL_SLOW_BLOCK	(0x1U<<5)
//generic timer: 0:keeps running when STPCLK, 1:disabled during STPCLK, reenabled after
#define TMR_CTRL_GENERIC_BLOCK	(0x1U<<4)
//fast timer: 0:keeps running when STPCLK, 1:disabled during STPCLK, reenabled after
#define TMR_CTRL_FAST_BLOCK	(0x1U<<3)
//slow timer: 1:enable
#define TMR_CTRL_SLOW_EN	(0x1U<<2)
//generic timer: 1:enable
#define TMR_CTRL_GENERIC_EN	(0x1U<<1)
//fast timer: 1:enable
#define TMR_CTRL_FAST_EN	(0x1U<<0)


//fast timer count
//minimum value 2, interval=(counter-1)*0.6 sec
#define TMR_COUNT_FAST		0x89


//generic timer count
//minimum value 2, interval=(counter-1)*0.3 sec
#define TMR_COUNT_GENERIC	0x8a


//slow timer count
//minimum value 2, interval=(counter-1)*time base (96h-97h)
#define TMR_COUNT_SLOW		0x8b


//timers reset
#define TMR_RESET	0x8c
//fast timer: 1:reload
#define TMR_RESET_FAST		(0x1U<<2)
//generic timer: 1:reload
#define TMR_RESET_GENERIC	(0x1U<<1)
//slow timer: 1:reload
#define TMR_RESET_SLOW		(0x1U<<0)


//RMSMIBLK count
//minimum value 2, interval=(counter-1)*35us
#define RMSMIBLK_COUNT	0x8d


//clock throttling ON timer count
//counts when stpclk deasserted, at timeout stpclk is asserted
//minimum value 2, interval=(counter-1)*35us
#define THROTTLE_ON	0x8e


//clock throttling OFF timer count
//counts when stpclk asserted, at timeout stpclk is deasserted
//minimum value 2, interval=(counter-1)*35us
#define THROTTLE_OFF	0x8f


//clock throttling reload mask, 16bit
#define THROTTLE_MASK	0x90
//video, 0xa0000-0xbffff,0x3b0,0x3c0,0x3d0: 1:enable
#define THROTTLE_MASK_VIDEO	(0x1U<<9)
//irq request: 1:enable
#define THROTTLE_MASK_IRQ	(0x1U<<8)
//dma request: 1:enable
#define THROTTLE_MASK_DMA	(0x1U<<7)
//ms/kb (irq 1 3 4 12) request: 1:enable
#define THROTTLE_MASK_KBMS	(0x1U<<6)
//hdd/floppy request: 1:enable
#define THROTTLE_MASK_HDFD	(0x1U<<5)
//parports (0x278,0x378,0x3bc) request: 1:enable
#define THROTTLE_MASK_LPT	(0x1U<<4)
//serials (0x2f8,0x3f8,0x2e8,0x3e8) request: 1:enable
#define THROTTLE_MASK_COM	(0x1U<<3)
//IO trap 1 request: 1:enable
#define THROTTLE_MASK_IOTRAP1	(0x1U<<2)
//IO trap 0 request: 1:enable
#define THROTTLE_MASK_IOTRAP0	(0x1U<<1)
//PCI master request: 1:enable
#define THROTTLE_MASK_PCIM	(0x1U<<0)


//fast timer reload mask, 16bit
#define FTMR_MASK	0x92
//video, 0xa0000-0xbffff,0x3b0,0x3c0,0x3d0: 1:enable
#define FTMR_MASK_VIDEO		(0x1U<<9)
//irq request: 1:enable
#define FTMR_MASK_IRQ		(0x1U<<8)
//dma request: 1:enable
#define FTMR_MASK_DMA		(0x1U<<7)
//ms/kb (irq 1 3 4 12) request: 1:enable
#define FTMR_MASK_KBMS		(0x1U<<6)
//hdd/floppy request: 1:enable
#define FTMR_MASK_HDFD		(0x1U<<5)
//parports (0x278,0x378,0x3bc) request: 1:enable
#define FTMR_MASK_LPT		(0x1U<<4)
//serials (0x2f8,0x3f8,0x2e8,0x3e8) request: 1:enable
#define FTMR_MASK_COM		(0x1U<<3)
//IO trap 1 request: 1:enable
#define FTMR_MASK_IOTRAP1	(0x1U<<2)
//IO trap 0 request: 1:enable
#define FTMR_MASK_IOTRAP0	(0x1U<<1)
//PCI master request: 1:enable
#define FTMR_MASK_PCIM		(0x1U<<0)


//generic timer reload mask, 16bit
#define GTMR_MASK	0x94
//video, 0xa0000-0xbffff,0x3b0,0x3c0,0x3d0: 1:enable
#define GTMR_MASK_VIDEO		(0x1U<<9)
//irq request: 1:enable
#define GTMR_MASK_IRQ		(0x1U<<8)
//dma request: 1:enable
#define GTMR_MASK_DMA		(0x1U<<7)
//ms/kb (irq 1 3 4 12) request: 1:enable
#define GTMR_MASK_KBMS		(0x1U<<6)
//hdd/floppy request: 1:enable
#define GTMR_MASK_HDFD		(0x1U<<5)
//parports (0x278,0x378,0x3bc) request: 1:enable
#define GTMR_MASK_LPT		(0x1U<<4)
//serials (0x2f8,0x3f8,0x2e8,0x3e8) request: 1:enable
#define GTMR_MASK_COM		(0x1U<<3)
//IO trap 1 request: 1:enable
#define GTMR_MASK_IOTRAP1	(0x1U<<2)
//IO trap 0 request: 1:enable
#define GTMR_MASK_IOTRAP0	(0x1U<<1)
//PCI master request: 1:enable
#define GTMR_MASK_PCIM		(0x1U<<0)


//slow timer reload mask, 16bit
#define STMR_MASK	0x96
//slow timer time base: 0:70us,1:1.2s,2:4.8s,3:9.7s
#define STMR_TIME_BASE		(0x3U<<14)
//video, 0xa0000-0xbffff,0x3b0,0x3c0,0x3d0: 1:enable
#define STMR_MASK_VIDEO		(0x1U<<9)
//irq request: 1:enable
#define STMR_MASK_IRQ		(0x1U<<8)
//dma request: 1:enable
#define STMR_MASK_DMA		(0x1U<<7)
//ms/kb (irq 1 3 4 12) request: 1:enable
#define STMR_MASK_KBMS		(0x1U<<6)
//hdd/floppy request: 1:enable
#define STMR_MASK_HDFD		(0x1U<<5)
//parports (0x278,0x378,0x3bc) request: 1:enable
#define STMR_MASK_LPT		(0x1U<<4)
//serials (0x2f8,0x3f8,0x2e8,0x3e8) request: 1:enable
#define STMR_MASK_COM		(0x1U<<3)
//IO trap 1 request: 1:enable
#define STMR_MASK_IOTRAP1	(0x1U<<2)
//IO trap 0 request: 1:enable
#define STMR_MASK_IOTRAP0	(0x1U<<1)
//PCI master request: 1:enable
#define STMR_MASK_PCIM		(0x1U<<0)


//fast timer reload irq mask, 16bit
#define FTMR_IRQ	0x98
//use irq15: 1:select
#define FTMR_IRQ15		(0x1U<<15)
//use irq14: 1:select
#define FTMR_IRQ14		(0x1U<<14)
//use irq13: 1:select
#define FTMR_IRQ13		(0x1U<<13)
//use irq12: 1:select
#define FTMR_IRQ12		(0x1U<<12)
//use irq11: 1:select
#define FTMR_IRQ11		(0x1U<<11)
//use irq10: 1:select
#define FTMR_IRQ10		(0x1U<<10)
//use irq9: 1:select
#define FTMR_IRQ9		(0x1U<<9)
//use irq8: 1:select
#define FTMR_IRQ8		(0x1U<<8)
//use irq7: 1:select
#define FTMR_IRQ7		(0x1U<<7)
//use irq6: 1:select
#define FTMR_IRQ6		(0x1U<<6)
//use irq5: 1:select
#define FTMR_IRQ5		(0x1U<<5)
//use irq4: 1:select
#define FTMR_IRQ4		(0x1U<<4)
//use irq3: 1:select
#define FTMR_IRQ3		(0x1U<<3)
//use irq2: 1:select
#define FTMR_IRQ2		(0x1U<<2)
//use irq1: 1:select
#define FTMR_IRQ1		(0x1U<<1)
//use irq0: 1:select
#define FTMR_IRQ0		(0x1U<<0)


//generic timer reload irq mask, 16bit
#define GTMR_IRQ	0x9a
//use irq15: 1:select
#define GTMR_IRQ15		(0x1U<<15)
//use irq14: 1:select
#define GTMR_IRQ14		(0x1U<<14)
//use irq13: 1:select
#define GTMR_IRQ13		(0x1U<<13)
//use irq12: 1:select
#define GTMR_IRQ12		(0x1U<<12)
//use irq11: 1:select
#define GTMR_IRQ11		(0x1U<<11)
//use irq10: 1:select
#define GTMR_IRQ10		(0x1U<<10)
//use irq9: 1:select
#define GTMR_IRQ9		(0x1U<<9)
//use irq8: 1:select
#define GTMR_IRQ8		(0x1U<<8)
//use irq7: 1:select
#define GTMR_IRQ7		(0x1U<<7)
//use irq6: 1:select
#define GTMR_IRQ6		(0x1U<<6)
//use irq5: 1:select
#define GTMR_IRQ5		(0x1U<<5)
//use irq4: 1:select
#define GTMR_IRQ4		(0x1U<<4)
//use irq3: 1:select
#define GTMR_IRQ3		(0x1U<<3)
//use irq2: 1:select
#define GTMR_IRQ2		(0x1U<<2)
//use irq1: 1:select
#define GTMR_IRQ1		(0x1U<<1)
//use irq0: 1:select
#define GTMR_IRQ0		(0x1U<<0)


//slow timer reload irq mask, 16bit
#define STMR_IRQ	0x9c
//use irq15: 1:select
#define STMR_IRQ15		(0x1U<<15)
//use irq14: 1:select
#define STMR_IRQ14		(0x1U<<14)
//use irq13: 1:select
#define STMR_IRQ13		(0x1U<<13)
//use irq12: 1:select
#define STMR_IRQ12		(0x1U<<12)
//use irq11: 1:select
#define STMR_IRQ11		(0x1U<<11)
//use irq10: 1:select
#define STMR_IRQ10		(0x1U<<10)
//use irq9: 1:select
#define STMR_IRQ9		(0x1U<<9)
//use irq8: 1:select
#define STMR_IRQ8		(0x1U<<8)
//use irq7: 1:select
#define STMR_IRQ7		(0x1U<<7)
//use irq6: 1:select
#define STMR_IRQ6		(0x1U<<6)
//use irq5: 1:select
#define STMR_IRQ5		(0x1U<<5)
//use irq4: 1:select
#define STMR_IRQ4		(0x1U<<4)
//use irq3: 1:select
#define STMR_IRQ3		(0x1U<<3)
//use irq2: 1:select
#define STMR_IRQ2		(0x1U<<2)
//use irq1: 1:select
#define STMR_IRQ1		(0x1U<<1)
//use irq0: 1:select
#define STMR_IRQ0		(0x1U<<0)


//soft SMI generation, RMSMIBLK trigger
#define SMI_RESUME	0x9e
//RMSMIBLK timer start: 1:start RMSMIBLK counter
#define SMI_RESUME_RMSMIBLK_START	(0x1U<<1)
//SMI assert: 1:assert SMI
#define SMI_RESUME_SMI_ASSERT		(0x1U<<0)


//SMI status, 16bit
//read: 0:no req,1:req, write: 0:no effect,1:clear
#define SMI_STAT	0xa0
//deturbo switch request
#define SMI_STAT_DETURBO	(0x1U<<15)
//video,0xa0000-0xbffff,0x3b0,0x3c0,0x3d0
#define SMI_STAT_VIDEO		(0x1U<<14)
//irq
#define SMI_STAT_IRQ		(0x1U<<13)
//fast timer
#define SMI_STAT_FTMR_EXPIRED	(0x1U<<12)
//generic timer
#define SMI_STAT_GTMR_EXPIRED	(0x1U<<11)
//slow timer
#define SMI_STAT_STMR_EXPIRED	(0x1U<<10)
//com
#define SMI_STAT_COM		(0x1U<<9)
//lpt
#define SMI_STAT_LPT		(0x1U<<8)
//hdd/floppy
#define SMI_STAT_HDFD		(0x1U<<7)
//kb/ms
#define SMI_STAT_KBMS		(0x1U<<6)
//break
#define SMI_STAT_BREAK		(0x1U<<5)
//sw SMI
#define SMI_STAT_SWSMI		(0x1U<<4)
//IO trap 1
#define SMI_STAT_IOTRAP1	(0x1U<<3)
//IO trap 0
#define SMI_STAT_IOTRAP0	(0x1U<<2)
//PCIM
#define SMI_STAT_PCIM		(0x1U<<1)
//DMA
#define SMI_STAT_DMA		(0x1U<<0)


//SMI mask, 16bit
//0:ignored, 1:selected
#define SMI_MASK	0xa2
//video,0xa0000-0xbffff,0x3b0,0x3c0,0x3d0
#define SMI_MASK_VIDEO		(0x1U<<14)
//irq
#define SMI_MASK_IRQ		(0x1U<<13)
//fast timer
#define SMI_MASK_FTMR_EXPIRED	(0x1U<<12)
//generic timer
#define SMI_MASK_GTMR_EXPIRED	(0x1U<<11)
//slow timer
#define SMI_MASK_STMR_EXPIRED	(0x1U<<10)
//com
#define SMI_MASK_COM		(0x1U<<9)
//lpt
#define SMI_MASK_LPT		(0x1U<<8)
//hdd/floppy
#define SMI_MASK_HDFD		(0x1U<<7)
//kb/ms
#define SMI_MASK_KBMS		(0x1U<<6)
//break
#define SMI_MASK_BREAK		(0x1U<<5)
//sw SMI
#define SMI_MASK_SWSMI		(0x1U<<4)
//IO trap 1
#define SMI_MASK_IOTRAP1	(0x1U<<3)
//IO trap 0
#define SMI_MASK_IOTRAP0	(0x1U<<2)
//PCIM
#define SMI_MASK_PCIM		(0x1U<<1)
//DMA
#define SMI_MASK_DMA		(0x1U<<0)


//SMI IRQ mask, 16bit
#define SMI_IRQ		0xa4
//use irq15: 1:select
#define SMI_IRQ15		(0x1U<<15)
//use irq14: 1:select
#define SMI_IRQ14		(0x1U<<14)
//use irq13: 1:select
#define SMI_IRQ13		(0x1U<<13)
//use irq12: 1:select
#define SMI_IRQ12		(0x1U<<12)
//use irq11: 1:select
#define SMI_IRQ11		(0x1U<<11)
//use irq10: 1:select
#define SMI_IRQ10		(0x1U<<10)
//use irq9: 1:select
#define SMI_IRQ9		(0x1U<<9)
//use irq8: 1:select
#define SMI_IRQ8		(0x1U<<8)
//use irq7: 1:select
#define SMI_IRQ7		(0x1U<<7)
//use irq6: 1:select
#define SMI_IRQ6		(0x1U<<6)
//use irq5: 1:select
#define SMI_IRQ5		(0x1U<<5)
//use irq4: 1:select
#define SMI_IRQ4		(0x1U<<4)
//use irq3: 1:select
#define SMI_IRQ3		(0x1U<<3)
//use irq2: 1:select
#define SMI_IRQ2		(0x1U<<2)
//use irq1: 1:select
#define SMI_IRQ1		(0x1U<<1)
//use irq0: 1:select
#define SMI_IRQ0		(0x1U<<0)


//clock throttle on timer reload IRQ mask, 16bit
#define THROTTLE_IRQ	0xa6
//use irq15: 1:select
#define THROTTLE_IRQ15		(0x1U<<15)
//use irq14: 1:select
#define THROTTLE_IRQ14		(0x1U<<14)
//use irq13: 1:select
#define THROTTLE_IRQ13		(0x1U<<13)
//use irq12: 1:select
#define THROTTLE_IRQ12		(0x1U<<12)
//use irq11: 1:select
#define THROTTLE_IRQ11		(0x1U<<11)
//use irq10: 1:select
#define THROTTLE_IRQ10		(0x1U<<10)
//use irq9: 1:select
#define THROTTLE_IRQ9		(0x1U<<9)
//use irq8: 1:select
#define THROTTLE_IRQ8		(0x1U<<8)
//use irq7: 1:select
#define THROTTLE_IRQ7		(0x1U<<7)
//use irq6: 1:select
#define THROTTLE_IRQ6		(0x1U<<6)
//use irq5: 1:select
#define THROTTLE_IRQ5		(0x1U<<5)
//use irq4: 1:select
#define THROTTLE_IRQ4		(0x1U<<4)
//use irq3: 1:select
#define THROTTLE_IRQ3		(0x1U<<3)
//use irq2: 1:select
#define THROTTLE_IRQ2		(0x1U<<2)
//use irq1: 1:select
#define THROTTLE_IRQ1		(0x1U<<1)
//use irq0: 1:select
#define THROTTLE_IRQ0		(0x1U<<0)


//GPIO control
#define GPIO_CTRL	0xa8
//enable GPIO0: 1:enable
#define GPIO_CTRL_0_EN		(1U<<7)
//enable GPIO1: 1:enable
#define GPIO_CTRL_1_EN		(1U<<6)
//active level GPIO0: 0:low, 1:high
#define GPIO_CTRL_0_LVL		(1U<<5)
//active level GPIO1: 0:low, 1:high
#define GPIO_CTRL_1_LVL		(1U<<4)
//debounce GPIO0: 1:enable
#define GPIO_CTRL_0_DEBOUNCE	(1U<<3)
//debounce GPIO1: 1:enable
#define GPIO_CTRL_1_DEBOUNCE	(1U<<2)
//function GPIO0: 0:deassert STPCLK,1:trigger SMI
#define GPIO_CTRL_0_FCN		(1U<<1)
//function GPIO1: 0:deassert STPCLK,1:trigger SMI
#define GPIO_CTRL_1_FCN		(1U<<0)


//GPIO SMI status
//read: 0:no req,1:req, write: 0:no effect,1:clear
#define GPIO_STATUS	0xa9
//enable GPIO0
#define GPIO_STATUS_0		(1U<<1)
//enable GPIO1
#define GPIO_STATUS_1		(1U<<0)


//GPIO debounce count
//minimum value is 2, interval=(counter-1)*0.15s
#define GPIO_DEBOUNCE	0xaa
//debounce count GPIO0
#define GPIO_DEBOUNCE_0		(0xfU<<4)
//debounce count GPIO1
#define GPIO_DEBOUNCE_1		(0xfU<<0)


//PCI INTA routing
//offset: 0-3:INTA-INTD
#define INT_ROUTING	0xc0
//link enable: 1:enable
#define INT_ROUTING_EN		(1U<<7)
//link select:
//0-2:reserved
//3:irq3
//7:irq7
//8:reserved
//9:irq9
//12:irq12
//13:reserved
//14:irq14
//15:irq15
#define INT_ROUTING_SEL	(0xfU<<0)


//ISA irq active level, 16bit
#define ISA_LEVEL_IRQ	0xc4
#define ISA_LEVEL_IRQ15		(1U<<15)
#define ISA_LEVEL_IRQ14		(1U<<14)
#define ISA_LEVEL_IRQ12		(1U<<12)
#define ISA_LEVEL_IRQ11		(1U<<11)
#define ISA_LEVEL_IRQ10		(1U<<10)
#define ISA_LEVEL_IRQ9		(1U<<9)
#define ISA_LEVEL_IRQ7		(1U<<7)
#define ISA_LEVEL_IRQ6		(1U<<6)
#define ISA_LEVEL_IRQ5		(1U<<5)
#define ISA_LEVEL_IRQ4		(1U<<4)
#define ISA_LEVEL_IRQ3		(1U<<3)


//post init config
#define INIT_POST	0xc6
//init to CPU reset enable: 0:enable, init to CPU reset, 1:disable
#define INIT_POST_CPU_RST	(1U<<3)
//init to PCI reset enable: 0:enable, init to PCI reset, 1:disable
#define INIT_POST_PCI_RST	(1U<<2)
//INTC compatibility: 0:ISA (same attributes),1:PCI (different attributes)
#define INIT_POST_INTC		(1U<<1)
//PCI to ISA posting: 1:enable
#define INIT_POST_PCI2ISA	(1U<<0)


//deturbo switch, read only
#define DETURBO_STATUS	0xc7
//deturbo status: 0:deasserted,1:asserted
#define DETURBO_STATUS_VAL	(1U<<0)


//mailbox, 32bit (TODO probably 8/16/32 access)
#define MAILBOX		0xc8


//bios config
#define ROM_CFG		0xd0
//area 0xfffa0000-0xfffdffff: 1:enable
#define ROM_CFG_SEG_ABCD	(1U<<7)
//area 0xfffe0000-0xfffeffff,0xe0000-0xeffff: 1:enable
#define ROM_CFG_SEG_E		(1U<<6)
//area 0xffff0000-0xffffffff,0xf0000-0xfffff: 1:enable
#define ROM_CFG_SEG_F		(1U<<5)
//oneshot write enable: 1:enabled, 0:disable write once
#define ROM_CFG_WE		(1U<<4)
//write enable: 0:write protect,1:write enable
#define ROM_CFG_WP		(1U<<3)
//reg 0xd1,autodetect blocking control: 0:enable,1:disable
#define ROM_CFG_D1_AUTODETECT	(1U<<1)
//reg 0xd1,write enable: 0:disable,1:enable
#define ROM_CFG_D1_WE		(1U<<0)


//ISA address decoder
#define ISA_DECODE	0xd1
//area 0xf0000-0xfffff: 1:enable
#define ISA_DECODE_SEG_F0	(1U<<7)
//area 0xe0000-0xeffff: 1:enable
#define ISA_DECODE_SEG_E0	(1U<<6)
//area 0xd8000-0xdffff: 1:enable
#define ISA_DECODE_SEG_D8	(1U<<5)
//area 0xd0000-0xd7fff: 1:enable
#define ISA_DECODE_SEG_D0	(1U<<4)
//area 0xc8000-0xcffff: 1:enable
#define ISA_DECODE_SEG_C8	(1U<<3)
//area 0xc0000-0xc7fff: 1:enable
#define ISA_DECODE_SEG_C0	(1U<<2)
//area 0xa0000-0xbffff: 1:enable
#define ISA_DECODE_SEG_A0	(1U<<1)
//area 0x00000-0x9ffff: 1:enable
#define ISA_DECODE_SEG_00	(1U<<0)


//exclusive area 2, 16bit
//must be identical to 0x54 EXCL_AREA_2
#define ISA_EXCL_AREA_2		0xd2
//select area 2: 0: non-cacheable area, 1: ISA memory hole (area sent to PCI)
#define ISA_EXCL_AREA_2_SEL	(1U<<15)
//0: 0kB (disabled), 1:64kB,2:128kB,3:256kB,4:512kB,5:1MB,6:2MB,7:4MB
#define ISA_EXCL_AREA_2_SIZE	(7U<<12)
//base address A[23:16]
#define ISA_EXCL_AREA_2_BASE	(0xffU<<0)


//address decoder ISA vs PCI
#define MISC7		0xd4
//extended cmos ram page enable: 0:114B, 1:+128B
#define MISC7_EXT_CMOS		(1U<<6)
/*
	pin41		pin42
x0	gpio0		gpio1
01	smout2		smout3
11	cmemle 0xc00	cmemcs 0x800
*/
#define MISC7_MUX1		(1U<<5)
#define MISC7_MUX0		(1U<<3)
//SA,SBHE output buffer drive strenth: 0:24mA,1:12mA
#define MISC7_SASBHE_STRENGTH	(1U<<2)
//IOR,IOW output buffer drive strenth: 0:24mA,1:12mA
#define MISC7_IORW_STRENGTH	(1U<<1)


#endif /* NORTHBRIDGE_SIS_85C496_REGS_H */
