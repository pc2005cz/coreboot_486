/*
#################### HB HB HB HB HB HB HB HB HB HB ####################
00:10.0 Host bridge [0600]: United Microelectronics [UMC] UM8881F [1060:8881] (rev 04)

00: 60 10 81 88 06 00 00 22 04 00 00 06 00 00 00 00
... (all zeroes)
50: fb a4 e6 8f 03 c1 ff 0f ff 0f 0d 2c e0 30 00 ff
60: 00 f4 01

alt
50: db a5 e6 cf 03 c1 ff 0f ff 0f 0d 2c e0 30 00 ff
60: 02 f4 01

---- after reset
00: 60 10 81 88 06 00 00 02 04 00 00 06 00 00 00 00
... (all zeroes)
50: 00 00 01 00 00 40 ff 0f ff 0f 00 00 00 0f 00 ff
60: 00 00 00


#################### ISA ISA ISA ISA ISA ISA ISA ISA ####################
00:12.0 ISA bridge [0601]: United Microelectronics [UMC] UM8886A [1060:886a] (rev 0d)

00: 60 10 6a 88 0f 00 00 02 0d 00 01 06 00 00 80 00
... (all zeroes)
40: 04 42 08 9a cc 00 14 74 00 00 00 00 00 00 00 00
50: 81 41 00 00 00 00 01 30 00 00 00 00 00 00 00 00
60: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
70: 6f ff 01 00 00 00 00 00 00 00 00 00 00 00 00 00
80: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
90: 02 00 1f 00 00 00 00 00 00 00 00 00 00 00 00 00
a0: 00 80 00 00 81 00 00 ef 28 00 00 00 00 00 00 00

------after power on
00: 60 10 6a 88 0f 00 00 02 0d 00 01 06 00 00 80 00
... (all zeroes)
40: 01 04 08 9a bc 00 10 30 00 00 00 00 00 00 00 00
50: 00 02
... (rest is zeroes)


#################### IDE IDE IDE IDE IDE IDE IDE IDE ####################
00:12.1 IDE interface [0101]: United Microelectronics [UMC] UM8886BF [1060:673a] (rev 0d) (prog-if 00 [ISA Compatibility mode-only controller])

00: 60 10 3a 67 05 00 00 00 0d 00 01 01 00 00 80 00
... (all zeroes)
30: 00 00 00 00 00 00 00 00 00 00 00 00 0e 00 00 00

40: 00 8d 33 3f 26 66 0b bb 55 55 55 55 88 88 aa aa
... (all zeroes)

==============
init values
00: 60 10 3a 67 04 00 00 00 0d 00 01 01 00 00 80 00
... (all zeroes)
40: 00 c0 30 aa 88 88 aa aa 88 88 aa aa 88 88 aa aa
...rest is zeroes

==================


+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
10c12r32m2e slot differences 1x simm
HB50=fb
HB51=a4
HB52=85 simm1, 85 simm2, 8d simm3, 8d simm4
HB53=82 simm1, 92 simm2, 82 simm3, 92 simm4
HB61=f4
IBC51=11 ( (VAL>>1) * 4M)
IBCA7=0f (simm1, 8f (simm4) (false hit probably)

11c11r16m1f 1x simm
HB52=84 simm1, 84 simm2, 8c simm3, 8c simm4
HB53=80 simm1, 90 simm2, 80 simm3, 90 simm4
HB61=c4

*/

#ifndef NORTHBRIDGE_UMC_UM8881_REG_INIT_H
#define NORTHBRIDGE_UMC_UM8881_REG_INIT_H

#include <types.h>
#include <device/pci_type.h>

#define HOST_BRIDGE		PCI_DEV(0, 0x10, 0)
#define ISA_BRIDGE		PCI_DEV(0, 0x12, 0)
#define IDE_CONTROLLER		PCI_DEV(0, 0x12, 1)

////// Host Bridge

#define HB50_L2_CFG_0		0x50
#define HB51_L2_CFG_1		0x51
#define HB52_DRAM_CFG_0		0x52
#define HB53_DRAM_CFG_1		0x53
#define HB54_SHADOW_EN		0x54
#define HB55_SHADOW_CFG		0x55
#define HB56_MEMHOLE_BASE	0x56
#define HB57_MEMHOLE_SIZE	0x57
#define HB58_UNKNOWN_0		0x58
#define HB59_UNKNOWN_1		0x59
#define HB5A_MISC_0		0x5a
//5B unknown
#define HB5C_SMRAM_0		0x5c
#define HB5D_SMRAM_1_DRAM_CFG_2	0x5d
//5E unknown
//5F unknown
#define HB60_SMRAM_2		0x60
#define HB61_EDO_CFG		0x61
#define HB62_BURST		0x62

///////// IBC

#define IBC40_CFG_0		0x40
#define IBC41_CFG_1		0x41
//IBC42 unknown
#define IBC43_INTAB_IRQ		0x43
#define IBC44_INTCD_IRQ		0x44
#define IBC45_CFG_2		0x45
#define IBC46_INT_EN		0x46
#define IBC47_INT_LEVEL		0x47
//48-4f
#define IBC50_PCI_VIDEO		0x50
#define IBC51_MEM_SIZE		0x51
//52-55
#define IBC56_CLOCK		0x56
#define IBC57_ROM_DECODE	0x57
//58-6f
#define IBC70_MONITOR_0		0x70
#define IBC71_MONITOR_1		0x71
#define IBC72_MONITOR_2		0x72
#define IBC73_MONITOR_3		0x73
#define IBC74_MONITOR_4		0x74
//75
#define IBC76_SMI_CFG		0x76
//77-81
#define IBC82_CFG_3		0x82
//83-8f
#define IBC90_WAKEUP_0		0x90
#define IBC91_WAKEUP_1		0x91
//92-9f
#define IBCA0_CFG_4		0xa0
#define IBCA1_CFG_5		0xa1
#define IBCA2_SMM_EVENT		0xa2
//a3
#define IBCA4_CLOCK_RATIO	0xa4
//a5-a6
#define IBCA7_UNKNOWN_0		0xa7
#define IBCA8_CFG_6		0xa8

///////// IDE

// #define IDE41_ENABLE		0x41


///////////////////////

struct umc_cfg_entry {
	u8 reg;		//register index
	u8 clear;	//which bits to clear (00010010 -> xxx0xx0x)
	u8 set;		//which bits to set (00000010 -> xxxxxx1x)
	//NOTICE first clear, then set
};

void umc_registers_init(
	pci_devfn_t dev,
	const struct umc_cfg_entry * reg_table,
	unsigned count
);

#endif /* NORTHBRIDGE_UMC_UM8881_REG_INIT_H */
