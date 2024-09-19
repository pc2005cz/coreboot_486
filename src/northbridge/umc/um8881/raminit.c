/* SPDX-License-Identifier: GPL-2.0-or-later */


/////// TODO 32M 10x12 double + 16M 11x11 single se spatn edetekuje

#define __SIMPLE_DEVICE__

#include <stdint.h>
#include <device/mmio.h>
#include <console/console.h>
#include <arch/io.h>
#include <device/pci_ops.h>
#include <cbmem.h>

#include "reg_init.h"

void dram_init(void);
void l2_cache_init(void);

//////////////////

//NOTICE could be chance it's stuck at this value, try more than one
static const u32 test_patterns[] = {
	0x12345678,
	0x3355aacc,
	0xf0f0f0f0,
	0x0f0f0f0f,
	0x87654321,
};

enum address_detect {
	ADDR_UNIMPLEMENTED,
	ADDR_ALIASED,
	ADDR_ACCESSIBLE,
};

//use patterns to check if two addresses leads to the same memory or are volatile
//NOTICE flush and disable any cache between CPU and tested location
//destroys content
//if both addresses matches, check only volatility
//don't check address witch 0x10 distance
static
enum address_detect check_address(
	uintptr_t valid,
	uintptr_t tested
)
{
	// printk(BIOS_INFO, "  Aliasing test: @0x%lx ? @0x%lx\n", valid, tested);

	for (unsigned idx = 0; idx < ARRAY_SIZE(test_patterns); idx++) {
		//init to known value
		write32p(valid, 0);
		write32p(tested, 0);
		asm volatile ("" ::: "memory");	//memory barrier

		//if we are testing alias, write inverted pattern
		if (valid != tested) {
			write32p(tested, ~ (test_patterns[idx]));
			asm volatile ("" ::: "memory");	//memory barrier
		}

		//set known working address to the test pattern
		write32p(valid, test_patterns[idx]);
		asm volatile ("" ::: "memory");	//memory barrier

		//now tested address will have either
		//	inverted pattern -> accessible
		//	direct pattern -> alias
		//	different value -> unimplemented

		//NOTICE NOTICE NOTICE
		//not connected chip will leave pins floating, limit this by writing a valid value
		//+0x10 = same octet of wires but different address
		//0x10 also a different cacheline
		//don't use 4, cache detection uses that
		write32p(tested + 0x10, 0);
		write32p(valid + 0x10, 0);
		asm volatile ("" ::: "memory");	//memory barrier

#if 0
		printk(BIOS_DEBUG, "RB1 %08x @0x%08lx\n", read32p(tested), tested);
		// printk(BIOS_DEBUG, "RB2 %08x\n", read32p(tested));
		// printk(BIOS_DEBUG, "RB3 %08x\n", read32p(tested));
		// printk(BIOS_DEBUG, "RB4 %08x\n", read32p(tested));

		read32p(tested);
		read32p(tested);
		read32p(tested);
		read32p(tested);
#endif

		//compare valid with pattern

		if (read32p(tested) == (~ test_patterns[idx])) {
			//not aliasing this pattern
			//check next pattern

// printk(BIOS_INFO, "  not aliasing %08x\n", read32p(tested));

		} else if (read32p(tested) == test_patterns[idx]) {
			//aliasing

// printk(BIOS_INFO, "  aliasing %08x\n", read32p(tested));
			if (valid != tested) {
				//if different addresses, return aliased immediatelly
				return ADDR_ALIASED;
			}

			//first check all patterns to declare accessible
		} else {
			//does not retain the value

// printk(BIOS_INFO, "  unimplemented %08x\n", read32p(tested));

			return ADDR_UNIMPLEMENTED;
		}
	}

	return ADDR_ACCESSIBLE;
}

///////////////////////////////////

struct bank_descriptor {
	u8 cols;	//could fit 4 bits
	u8 rows;	//could fit 4 bits
	bool double_sided;
};


static
unsigned get_bank_desc_to_size(
	struct bank_descriptor *bank
)
{
	if (bank == NULL) {
		return 0;
	}

	if ((bank->cols == 0) || (bank->rows == 0)) {
		return 0;
	}

	return 1UL << (bank->cols + bank->rows + 2 + (bank->double_sided?1:0));
}

//TODO ali has address 2, how to erase neighbor

static const u8 columns_to_address[12] = {
	//don't test 2 3 4 as it collides with check address algo
	5, 5, 5, 5, 6, 7, 8, 9, 10, 23, 24, 25
};

static const u8 rows_to_address[12] = {
	11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22
};


//TODO better function argument?
static
u8 detect_ma_bits(
	const u8 * map
)
{
	for (unsigned idx = 0; idx < 12; idx++) {
		const unsigned addr = 1UL << map[idx];

// printk(BIOS_INFO, "MA %u %hhu %x\n",idx,map[idx], addr);

		if (check_address(0, addr) != ADDR_ACCESSIBLE) {
			//MA bit (=idx) is unused, marks number of useable bits
//
// printk(BIOS_INFO, "  exit %u\n", idx);

			return idx;
		}
	}

	return 12;
}


//returns bank size and is it is double sided, NULL pointers are ignored
static
void detect_bank(
	struct bank_descriptor *banks,
	unsigned index
)
{
	if (banks == NULL) {
		return;
	}

	if (index >= 4) {
		return;
	}

	printk(BIOS_DEBUG, "Bank %u: ", index);

	banks[index].cols = 0;
	banks[index].rows = 0;
	banks[index].double_sided = false;

	if (check_address(0, 0) != ADDR_ACCESSIBLE) {
		//not implemented
		printk(BIOS_DEBUG, "N/A\n");
		return;
	}

	banks[index].cols = detect_ma_bits(columns_to_address);
// printk(BIOS_DEBUG, "===== CCC %u\n", banks[index].cols);

	banks[index].rows = detect_ma_bits(rows_to_address);
// printk(BIOS_DEBUG, "===== RRR %u\n", banks[index].rows);

	//64*MiB is implemented -> double sided
	//64 cannot be r/w -> single sided

	//NOTICE we are lazy, we are expecting other side has the same size

	if (check_address(64*MiB, 64*MiB) == ADDR_ACCESSIBLE) {
		banks[index].double_sided = true;
	}

	printk(BIOS_DEBUG, "columns %u, rows %u, %s sided, total size %u kiB\n",
		banks[index].cols,
		banks[index].rows,
		banks[index].double_sided ? "double" : "single",
		get_bank_desc_to_size(&banks[index]) / KiB
	);

	return;
}


//TODO change this if more configuration options is discovered
static
void sanitize_bank(
	struct bank_descriptor *bank
)
{
	if ((bank->cols == 10) && (bank->rows == 10)) {
	} else if ((bank->cols == 10) && (bank->rows == 11)) {
	} else if ((bank->cols == 10) && (bank->rows == 12)) {

	} else if ((bank->cols == 11) && (bank->rows == 11)) {
	} else if ((bank->cols == 11) && (bank->rows == 12)) {

	} else if ((bank->cols == 12) && (bank->rows == 12)) {


	} else if ((bank->cols == 0) && (bank->rows == 0)) {
		//disabled
	} else {
		printk(BIOS_ERR, "%u col(s), %u row(s) unsupported\n",
			bank->cols,
			bank->rows
		);

		//disable
		bank->cols = 0;
		bank->rows = 0;
		bank->double_sided = false;
	}
}


struct region_descriptor {
	unsigned totalsize_reg_val;
	bool two_banks;
	bool double_sided;
	bool use_upper;
	bool require_ma11;
};


static
u8 bank_size_to_region_size(
	unsigned size
)
{

// printk(BIOS_INFO, "bank_size_to_region_size %u\n", size);

	switch (size) {
		case 2*MiB:
			return 1;
		case 4*MiB:
			return 2;
		case 8*MiB:
			return 3;
		case 16*MiB:
			return 4;
		case 32*MiB:
			return 5;
		case 64*MiB:
			return 6;
		case 128*MiB:
			return 7;
		default:
			return 0;
	}

	return 0;
}


//unify two banks into a single region
//also if unified region is too small, check if a single bank is better
static
void unify_banks_to_region(
	struct bank_descriptor *lower,
	struct bank_descriptor *upper,
	struct region_descriptor *result
)
{
	if (lower == NULL) {
		return;
	}
	if (upper == NULL) {
		return;
	}
	if (result == NULL) {
		return;
	}

	unsigned lower_size_original = get_bank_desc_to_size(lower);
	unsigned upper_size_original = get_bank_desc_to_size(upper);

// printk(BIOS_DEBUG, "Region lbs:%u ubs:%u\n", lower_size_original, upper_size_original);


	if ((lower_size_original + upper_size_original) == 0) {
		// both banks are disabled
// printk(BIOS_DEBUG, "both banks are empty\n");
		result->totalsize_reg_val = 0;
		result->double_sided = false;
		result->two_banks = false;
		result->use_upper = false;
		result->require_ma11 = false;
		return;
	}

	// one bank is disabled
	if (lower_size_original == 0) {
// printk(BIOS_DEBUG, "lower bank is empty\n");
		result->totalsize_reg_val = bank_size_to_region_size(get_bank_desc_to_size(upper));
		result->double_sided = upper->double_sided;
		result->two_banks = false;
		result->use_upper = true;
		result->require_ma11 = (upper->rows == 12)?true:false;
		return;
	} else if (upper_size_original == 0) {
// printk(BIOS_DEBUG, "upper bank is empty\n");
		result->totalsize_reg_val = bank_size_to_region_size(get_bank_desc_to_size(lower));
		result->double_sided = lower->double_sided;
		result->two_banks = false;
		result->use_upper = false;
		result->require_ma11 = (lower->rows == 12)?true:false;
		return;
	}

	//both are filled
	//gets common denominator

	struct bank_descriptor tmp;

	tmp.cols = MIN(lower->cols, upper->cols);
	tmp.rows = MIN(lower->rows, upper->rows);
	tmp.double_sided = lower->double_sided && upper->double_sided;

	unsigned result_size = get_bank_desc_to_size(&tmp) * 2;

	if (lower_size_original > result_size) {
// printk(BIOS_DEBUG, "lower bank is bigger than combined\n");

		// LOWER is higher than the common denominator, disable UPPER
		result->totalsize_reg_val = bank_size_to_region_size(lower_size_original);
		result->double_sided = lower->double_sided;
		result->two_banks = false;
		result->use_upper = false;
		result->require_ma11 = (lower->rows == 12)?true:false;
	} else if (upper_size_original > result_size) {
// printk(BIOS_DEBUG, "upper bank is bigger than combined\n");

		// UPPER is higher than the common denominator, disable LOWER
		result->totalsize_reg_val = bank_size_to_region_size(upper_size_original);
		result->double_sided = upper->double_sided;
		result->two_banks = false;
		result->use_upper = true;
		result->require_ma11 = (upper->rows == 12)?true:false;
	} else {
// printk(BIOS_DEBUG, "use combined banks\n");

		//use common denominator
		result->totalsize_reg_val = bank_size_to_region_size(result_size);
		result->double_sided = tmp.double_sided;
		result->two_banks = true;
		result->use_upper = false;
		result->require_ma11 = (tmp.rows == 12)?true:false;
	}
}


static
void show_dram_regions(
	struct region_descriptor *region_base,
	unsigned char region_idx,
	unsigned char bank_lower,
	unsigned char bank_upper
)
{
	if (region_base == NULL)
		return;
	if (region_idx >= 2)
		return;
	if (bank_lower >= 4)
		return;
	if (bank_upper >= 4)
		return;

	printk(BIOS_INFO, "Region %u (banks %u+%u): ",
		region_idx,
		bank_lower,
		bank_upper
	);

	struct region_descriptor * const region = &region_base[region_idx];

	if (region->totalsize_reg_val == 0) {
		printk(BIOS_INFO, "Empty\n");
	} else {
		printk(BIOS_INFO, "%ux %u MiB, %s sided%s%s\n",
			region->two_banks?2:1,
			1 << region->totalsize_reg_val,
			region->double_sided?"double":"single",
			region->require_ma11?", MA11":"",
			region->use_upper?", swapped":""
		);
	}
}


///////////////////////////////////////
///////////////// API /////////////////
///////////////////////////////////////


enum l2_chip_size {
	L2_SIZE_NONE = 0,	//also failed
	L2_SIZE_64K = 1,
	L2_SIZE_128K = 2,
	L2_SIZE_256K = 3,
	L2_SIZE_512K = 4,
	L2_SIZE_1024K = 5,
	L2_SIZE_2048K = 6,	//or reserved TODO test it (poke L2 works with this)
	L2_SIZE_4096K = 7,	//or reserved TODO test it (poke L2 works with this)
};


//NOTICE all cache configuration must be set by now (timing, WT/WB, dirty/tag)
//NOTICE DRAM must be configured and running
void l2_cache_init(void)
{
	unsigned cache_size = 0;

	printk(BIOS_INFO, "UMC L2 cache init\n");

	//set L2 cache to max, single bank, Wt, timing 3222 (slowest)
	pci_update_config8(HOST_BRIDGE, HB50_L2_CFG_0,
		~(7),
		0x80 | L2_SIZE_1024K
	);

	//force L2 cache hit
	pci_update_config8(HOST_BRIDGE, HB5A_MISC_0, ~0, 0x40);

	if (check_address(0, 0) != ADDR_ACCESSIBLE) {
		//L2 not installed, disable it
		printk(BIOS_INFO, "L2 cache not installed, disabling\n");

		pci_update_config8(HOST_BRIDGE, HB50_L2_CFG_0, 0, 0);

		cache_size = 0;
	} else {
		//enable two banks to test if filled
		pci_update_config8(HOST_BRIDGE, HB50_L2_CFG_0, ~0, 8);

		if (check_address(0, 4) == ADDR_ACCESSIBLE) {
			printk(BIOS_INFO, "Dual bank L2 cache\n");
		} else {
			//revert to a single bank L2
			pci_update_config8(HOST_BRIDGE, HB50_L2_CFG_0, ~8, 0);

			printk(BIOS_INFO, "Single bank L2 cache\n");
		}

		const unsigned l2_size[8] = {
			[L2_SIZE_NONE] = 0,
			[L2_SIZE_64K] = 64*KiB,
			[L2_SIZE_128K] = 128*KiB,
			[L2_SIZE_256K] = 256*KiB,
			[L2_SIZE_512K] = 512*KiB,
			[L2_SIZE_1024K] = 1024*KiB,
			[L2_SIZE_2048K] = 2048*KiB,
			[L2_SIZE_4096K] = 4096*KiB,
		};

		for (unsigned idx = L2_SIZE_64K; idx < L2_SIZE_2048K; idx++) {
			if (check_address(0, l2_size[idx]) != ADDR_ACCESSIBLE) {
				//if the value at the l2_size address matches address 0
				//cache size is l2_size, value in register is index

				cache_size = l2_size[idx];
				printk(BIOS_INFO, "L2 cache size %u kiB\n", cache_size/KiB);

				pci_update_config8(HOST_BRIDGE, HB50_L2_CFG_0, ~7, idx);
				break;
			}
		}
	}

	//disable forced hit
	pci_update_config8(HOST_BRIDGE, HB5A_MISC_0, ~0x40, 0);

	if (cache_size > 0) {
		//if L2 cache enabled, initialize (random poweron data)

// //experimental
// pci_update_config8(HOST_BRIDGE, HB5A_MISC_0, ~0, 0x40);

		printk(BIOS_DEBUG, "Init L2 to be coherent with RAM: ");

#if 0
		//read twice L2 size
		for (uintptr_t addr = 0x200000; addr < 0x200000 + cache_size*2; addr+=4) {
			if ((addr % 0x10000) == 0) {
				printk(BIOS_DEBUG, "%6lx ", addr);
			}

			read32p(addr);
			// write32p(addr, addr);
		}
		printk(BIOS_DEBUG, "\n");
#endif

	}

	//NOTICE INVD is run in exit_car.S
	//NOTICE if there are stale data/code in L2 cache, after enabling there will be random crashes
	//it seems invd will cause stale data in L2 cache to write to RAM
}


///////////////////////////////////////////

void dram_init(void)
{
	printk(BIOS_INFO, "UMC DRAM init\n");

	//banks need to be sorted by being occupied of SIMM module

	/*
	=======================
	NOTICE not sure if single sided SIMMs are always using same RAS
	-> located on the same "side"
	*/

	//disable L2
	pci_update_config8(HOST_BRIDGE, HB50_L2_CFG_0, 0x7f, 0);

	//disable L2 force hit
	pci_update_config8(HOST_BRIDGE, HB5A_MISC_0, ~0x40, 0);

	/*
	============================
	Detection algorithm:
	write magic at address 0
	write magic inversion to expected size
	check if inversion was written to address 0
	============================
	Pair capacity computation algorithm:
	NOTICE according to existing documentation it looks weirdly designed :-P
	Both SIMMs zero -> disable
	Lower SIMM filled -> set its capacity
	Upper SIMM filled -> set its capacity and swap slot bit
	Both SIMMs filled -> fun begins
		single + single || double+double
			Same capacity -> sum capacity
			Different capacity -> crop the larger one
				cropped + other > uncropped -> set summed capacity
				cropped + other < uncropped -> disable other, redo
		single + double || double+single -> a pair can have only one type of sides
			halved double > single -> needs to have same sizes
	*/

	//all banks will be tested as singular module at maximum columns and rows
	//all tests inside lower region
	//TODO revision 4 of chipset required

	//set lower region to 12x12 (2x 64 MiB - double sided)
	pci_update_config8(HOST_BRIDGE, HB52_DRAM_CFG_0, ~0x7f, 0x07);

	//set lower region to be double sided, 1 bank
	pci_update_config8(HOST_BRIDGE, HB53_DRAM_CFG_1, ~0x3f, 0x02);

	//enable assymetrical/MA11 support for detection of valid columns/rows
	pci_update_config8(HOST_BRIDGE, HB61_EDO_CFG, ~(0), 0x10);

	/*
	===================================
	MAxx should be mapped now this way:
	===================================
	pin		col	row
	--------------------
	MA00	2	11
	MA01	3	12
	MA02	4	13
	MA03	5	14
	MA04	6	15
	MA05	7	16
	MA06	8	17
	MA07	9	18
	MA08	10	19
	MA09	23	20
	MA10	24	21
	MA11	25	22
	*/

	struct bank_descriptor banks[4];

	for (unsigned idx = 0; idx < 4; idx++) {
		const u8 hb52_or_mask[4] = {
			0, 0, 8, 8	//lower region, upper region
		};

		const u8 hb53_or_mask[4] = {
			0, 0x10, 0, 0x10	//always swap in lower region
		};

		pci_update_config8(HOST_BRIDGE, HB52_DRAM_CFG_0, ~8, hb52_or_mask[idx]);
		pci_update_config8(HOST_BRIDGE, HB53_DRAM_CFG_1, ~0x30, hb53_or_mask[idx]);

		//TODO maybe return just a single structure?
		detect_bank(banks, idx);

		//make sure if only sane ranges were detected (disable if not)
		sanitize_bank(&banks[idx]);
	}

	//now banks should be detected and sanitized

	//combine bank 0/1 and 2/3 to two regions

	struct region_descriptor regions[2];

	unify_banks_to_region(&banks[0], &banks[1], &regions[0]);
	unify_banks_to_region(&banks[2], &banks[3], &regions[1]);

	//TODO incorporate bank swap and region swap
	show_dram_regions(regions, 0, 0, 1);
	show_dram_regions(regions, 1, 2, 3);

	//now compare which region is larger, and set registers

	u8 hb52_or_mask_final = 0;
	u8 hb53_or_mask_final = 0;
	u8 hb61_or_mask_final = 0;

	unsigned upper_idx = 1;
	unsigned lower_idx = 0;

	if (regions[1].totalsize_reg_val > regions[0].totalsize_reg_val) {
		upper_idx = 0;
		lower_idx = 1;

		//swap regions
		hb52_or_mask_final |= 8;

		printk(BIOS_INFO, "Swap regions\n");
	}

	//U region size
	hb52_or_mask_final |= (regions[upper_idx].totalsize_reg_val & 7) << 4;

	//L region size
	hb52_or_mask_final |= regions[lower_idx].totalsize_reg_val & 7;

	pci_update_config8(HOST_BRIDGE, HB52_DRAM_CFG_0, ~0x7f, hb52_or_mask_final);

	//swap banks in U region
	hb53_or_mask_final |= regions[upper_idx].use_upper ? (1 << 5) : 0;

	//swap banks in L region
	hb53_or_mask_final |= regions[lower_idx].use_upper ? (1 << 4) : 0;

	//U region single/double sided
	hb53_or_mask_final |= regions[upper_idx].double_sided ? (1 << 3) : 0;

	//U region 1/2 banks
	hb53_or_mask_final |= regions[upper_idx].two_banks ? (1 << 2) : 0;

	//L region single/double sided
	hb53_or_mask_final |= regions[lower_idx].double_sided ? (1 << 1) : 0;

	//L region 1/2 banks
	hb53_or_mask_final |= regions[lower_idx].two_banks ? (1 << 0) : 0;

	pci_update_config8(HOST_BRIDGE, HB53_DRAM_CFG_1, ~0x3f, hb53_or_mask_final);

	//U region requires MA11
	//NOTICE absolute identification
	hb61_or_mask_final |= regions[upper_idx].require_ma11 ? (1 << 5) : 0;

	//L region requires MA11
	//NOTICE absolute identification
	hb61_or_mask_final |= regions[lower_idx].require_ma11 ? (1 << 4) : 0;

	pci_update_config8(HOST_BRIDGE, HB61_EDO_CFG, ~0x30, hb61_or_mask_final);

	//TODO EDO detect?

#if 0
	//TODO set computed combination
	//TODO temp force 4x 32MB
	// pci_update_config8(HOST_BRIDGE, HB52_DRAM_CFG_0, ~0x7f, 0x66);
	// pci_update_config8(HOST_BRIDGE, HB53_DRAM_CFG_1, ~0x3f, 0x0f);
#elif 0
	//TEMP 32MB single module
	// pci_update_config8(HOST_BRIDGE, HB52_DRAM_CFG_0, ~0x7f, 0x05);
	// pci_update_config8(HOST_BRIDGE, HB53_DRAM_CFG_1, ~0x3f, 0x02);
#endif

	unsigned ram_size_4m = cbmem_top_chipset()/(4*MiB);
	if (ram_size_4m == 0) {
		ram_size_4m = 1;
	}

	//size of RAM ?? TODO rounded!
	pci_update_config8(ISA_BRIDGE, IBC51_MEM_SIZE, 1, (ram_size_4m & 0x7f) << 1);

	printk(BIOS_DEBUG, "DRAM configured\n");
}
