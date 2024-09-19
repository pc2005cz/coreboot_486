/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <delay.h>
#include <stdint.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <console/console.h>
#include <timestamp.h>

#include "regs.h"

/* Debugging macros. */
#define pr_err(x...)		printk(BIOS_ERR, x)
#define pr_info(x...)		printk(BIOS_INFO, x)


static void write_phys(uintptr_t addr, u32 value)
{
	write32p(addr, value);
}

static void phys_memory_barrier(void)
{
	asm volatile ("" ::: "memory");
}

static u32 read_phys(uintptr_t addr)
{
	return read32p(addr);
}

static void dram_precfg(void)
{
	u8 val;

	val = pci_read_config8(HOST_BRIDGE, CPUCFG);
	val &= ~CPUCFG_DRAM_SPEED;
	val |= 0;	//set slowest timing for detection
	pci_write_config8(HOST_BRIDGE, CPUCFG, val);

	//no parity, biggest type, normal refresh, longest delays/waits
	val = (2U<<5);	//4/8/16/32M type (can be overridden if asymmetric setting)
	pci_write_config8(HOST_BRIDGE, DRAMCFG, val);

	//pins

	mdelay(1);	//before stabilize
}


//NOTICE quick_ram_check_or_die (TODO delete)

//check if value written at one address ends up in second
//destroys content
//return true if aliases
static bool addr_is_aliasing(
	u32 val,
	uintptr_t ptr_write,
	uintptr_t ptr_read
)
{
	//reset readout address word
	write_phys(ptr_read, 0);
	phys_memory_barrier();

	if (read_phys(ptr_read) != 0) {
		// memory error
		die("Can't read back its own reset value!");
	}

	write_phys(ptr_write, val);
	phys_memory_barrier();

	if (read_phys(ptr_write) != val) {
		// memory error
		die("Can't read back its own test value!");
	}

	if (read_phys(ptr_read) == val) {
		//aliases
		return true;
	} else {
		//not aliases
		return false;
	}
}


static void bank_precfg(u8 bank)
{
	if (bank > 7) bank = 0;

	//p131, set FPM for size test
	pci_write_config8(HOST_BRIDGE, RAM_EDO, 0);

	//TODO

	// pci_write_config8(HOST_BRIDGE, MISC, );	//MISC_EDO_DETECT ??


	//p133, no assymetry, use set dram to 16M from reg 0x41 DRAMCFG
	u8 reg8 = pci_read_config8(HOST_BRIDGE, DRAM_ASYMMETRIC);
	reg8 &= ~(3U << (bank*2));
	pci_write_config8(HOST_BRIDGE, DRAM_ASYMMETRIC, reg8);

	//set memory size for detected bank
	for (u8 idx=0;idx<8;idx++) {
		//p122
		if (idx < bank) {
			//no accumulation before
			pci_write_config8(HOST_BRIDGE, DRAMBANK_BOUNDARY_BASE+idx, 0);
		} else {
			//single accumulation after
			//max value: 16Mx32 addressing => 64MB per bank
			pci_write_config8(HOST_BRIDGE, DRAMBANK_BOUNDARY_BASE+idx, 0x40);
		}
	}

	//TODO CAR?
}


//set all words in bank to 0
//TODO maybe not necessary now
static void bank_erase(void)
{
	for (uintptr_t addr=0;addr<(1UL << 26);addr+=sizeof(u32)) {
		write_phys(addr, 0);
	}

	phys_memory_barrier();	//probably not necessary on 486
}


//decode table at p25, we need to detect which MAx lines are ignored by SIMM
//use 16M config (all MAx are used)
//iterate from MA11 to MA0 for both cas and ras sizes
//reset address 0
//write test word to MA11
//readback from MA11 = different -> mem failed
//readback address 0 =
//	MA11 value -> MA11 is NC (wraps)
//	reset value -> MA11 is connected
//	different value -> mem failed
//... repeat for other MA signal

struct ma_test {
	u32 test;
	uintptr_t pwrite;
	uintptr_t pread;
	u8 size;
};


//TODO probably compress eventually only to bitshift values
static const struct ma_test cas_test[] = {
	{0x12345678, 1UL << 25, 0, 12},
	{0x55555555, 1UL << 12, 0, 11},
	{0xaaaaaaaa, 1UL << 11, 0, 10},
	{0xcccccccc, 1UL << 10, 0, 9},
	{0x33333333, 1UL << 9, 0, 8},
	{0xf0f0f0f0, 1UL << 8, 0, 7},
	{0x0f0f0f0f, 1UL << 7, 0, 6},
	{0xff00ff00, 1UL << 6, 0, 5},
	{0x00ff00ff, 1UL << 5, 0, 4},
	{0xffff0000, 1UL << 4, 0, 3},
	{0x0000ffff, 1UL << 2, 0, 2},
	{0x87654321, 1UL << 3, 0, 1},
};


//returns number of valid MA bits for CAS
static u8 bank_cas_size(u8 bank)
{
	if (bank > 7) bank = 0;

	//loop until cas toggle stops aliasing to address 0
	for (u8 idx=0;idx<ARRAY_SIZE(cas_test);idx++) {
		if (! addr_is_aliasing(
			cas_test[idx].test,
			cas_test[idx].pwrite,
			cas_test[idx].pread)
		) {
			//not aliasing

			//TODO debug

			return cas_test[idx].size;
		}
	}

	die("memory error, cas size detected as 0");

	return 0;
}


//TODO probably compress eventually only to bitshift values
static const struct ma_test ras_test[] = {
	{0xcafebabe, 1UL << 24, 0, 12},
	{0xcafed00d, 1UL << 23, 0, 11},
	{0xbeefbeef, 1UL << 21, 0, 10},
	{0x0112358D, 1UL << 20, 0, 9},
	{0xabcdabcd, 1UL << 19, 0, 8},
	{0xdcbadcba, 1UL << 18, 0, 7},
	{0x01189998, 1UL << 17, 0, 6},
	{0x81999119, 1UL << 16, 0, 5},
	{0x7253,     1UL << 15, 0, 4},
	{0x77777777, 1UL << 14, 0, 3},
	{0x31415926, 1UL << 22, 0, 2},
	{0x27182818, 1UL << 13, 0, 1},
};


//returns number of valid MA bits for CAS
static u8 bank_ras_size(u8 bank)
{
	if (bank > 7) bank = 0;

	//ras test
	// *ras = 0;
	for (u8 idx=0;idx<ARRAY_SIZE(ras_test);idx++) {
		if (! addr_is_aliasing(
			ras_test[idx].test,
			ras_test[idx].pwrite,
			ras_test[idx].pread)
		) {
			//not aliasing

			//TODO debug

			return ras_test[idx].size;
		}
	}

	die("memory error, ras size detected as 0");

	return 0;
}


//EDO duck test: if it works like EDO -> it is EDO
static bool bank_is_edo(u8 bank)
{
	if (bank > 7) bank = 0;

	//TODO
	// pci_write_config8(HOST_BRIDGE, MISC, );	//MISC_EDO_DETECT ??

	uintptr_t ptr_write = 0;

	//p131, set EDO on selected bank
	pci_write_config8(HOST_BRIDGE, RAM_EDO, 1U<<bank);

	write_phys(0, 0x55555555);
	phys_memory_barrier();
	if (read_phys(ptr_write) != 0x55555555) {
		return false;
	}

	write_phys(0, 0xaaaaaaaa);
	phys_memory_barrier();
	if (read_phys(ptr_write) != 0xaaaaaaaa) {
		return false;
	}

	write_phys(0, 0x00000000);
	phys_memory_barrier();
	if (read_phys(ptr_write) != 0x00000000) {
		return false;
	}

	write_phys(0, 0xffffffff);
	phys_memory_barrier();
	if (read_phys(ptr_write) != 0xffffffff) {
		return false;
	}

	//let's say EDO works

	return true;
}

#if 0
static bool bank_has_parity(void)
{
/*
	u8 reg8 = pci_read_config16(HOST_BRIDGE, DRAMCFG);
	reg8 |= DRAMCFG_PARITY_EN;
	pci_write_config8(HOST_BRIDGE, DRAMCFG, reg8);


	//TODO
	// // pci_write_config8(HOST_BRIDGE, 5, 0);
		SERR 5-4 6-7 p115
		p24

*/
	return false;
}
#endif

void dram_initialize(void);


void dram_initialize(void)
{
	//TODO resume, cannot overwrite RAM

	timestamp_add_now(TS_INITRAM_START);

	dram_precfg();

	//TODO 4dps only 2 SIMMs (4 banks)

	u8 reg8_edo = 0;
	u8 min_type = 2;
	u8 bank_mb_size[8] = {0};

	for (u8 bank=0;bank<4;bank++) {
		bank_precfg(bank);

		bank_erase();

		u8 cas = bank_cas_size(bank);

		u8 ras = bank_ras_size(bank);

		//TODO debug!
		if (bank_is_edo(bank)) {
			//TODO debug!
			reg8_edo |= 1U << bank;
		}

		//TODO bank_has_parity (it is all or nothing anyway)

		if ((cas+ras-18) <= 6) {
			bank_mb_size[bank] = 1U << (cas+ras-18);	//u32 words in CASxRAS array
		} else {
			bank_mb_size[bank] = 1U << 6;
		}

		u8 reg8_assym = pci_read_config8(HOST_BRIDGE, DRAM_ASYMMETRIC);
		reg8_assym &= ~(3U << (bank*2));

		if (ras == 12) {
			if (cas == 10) {
				//assymetric 4M (12x10)
				reg8_assym |= 3U << (bank*2);
			} else if (cas == 9) {
				//assymetric 2M (12x9)
				reg8_assym |= 2U << (bank*2);
			} else if (cas == 8) {
				//assymetric 1M (12x8)
				reg8_assym |= 1U << (bank*2);
			} else {
				//symmetric 4/8/16M
				if (cas == 12) {
					//16M
					min_type = 2;
				} else if (cas == 11) {
					//8M
					min_type = 2;
				} else {
					bank_mb_size[bank] = 0;
					pr_err("unsupported CAS/RAS size, disabled\n");
				}
			}
		} else if (ras == 11) {
			if (cas == 11) {
				//symmetric 4M
				min_type = 2;
			} else if (cas == 10) {
				//symmetric 2M
				min_type = 1;
			} else {
				bank_mb_size[bank] = 0;
				pr_err("unsupported CAS/RAS size, disabled\n");
			}
		} else if (ras == 10) {
			if (cas == 10) {
				//symmetric 1M
				min_type = 1;
			} else if (cas == 9) {
				//symmetric 512k
				min_type = 0;
			} else {
				bank_mb_size[bank] = 0;
				pr_err("unsupported CAS/RAS size, disabled\n");
			}
		} else if (ras == 9) {
			if (cas == 9) {
				//symmetric 256k
				min_type = 0;
			} else {
				bank_mb_size[bank] = 0;
				pr_err("unsupported CAS/RAS size, disabled\n");
			}
		} else {
			bank_mb_size[bank] = 0;
			pr_err("unsupported CAS/RAS size, disabled\n");
		}

		pci_write_config8(HOST_BRIDGE, DRAM_ASYMMETRIC, reg8_assym);
	}

	//TODO if assymetric smaller than symmetric, which smallest type should be there

	u8 reg8_dramcfg = pci_read_config8(HOST_BRIDGE, DRAMCFG);
	reg8_dramcfg &= ~DRAMCFG_TYPE;
	reg8_dramcfg |= min_type << 5;
	pci_write_config8(HOST_BRIDGE, DRAMCFG, reg8_dramcfg);

	pci_write_config8(HOST_BRIDGE, RAM_EDO, reg8_edo);

	u16 sum_mb = 0;
	//finalize bank boundary -> enable dram?
	for (u8 bank=0;bank<8;bank++) {
		//p25 table
		sum_mb += bank_mb_size[bank];
		if (sum_mb > 255) {
			//TODO warning?
			sum_mb = 255;
		}

		//p122 boundary reg
		pci_write_config8(HOST_BRIDGE, DRAMBANK_BOUNDARY_BASE+bank, sum_mb);
	}

	timestamp_add_now(TS_INITRAM_END);
}
