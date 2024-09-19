/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <stdint.h>
#include <device/mmio.h>
#include <console/console.h>
#include <arch/io.h>

#include "regs.h"

void l2_cache_init(void);
void dram_init(void);


//////////////////

//NOTICE could be chance it's stuck at this value, try more than one
static const u32 test_patterns[] = {
	0x12345678,
	0x3355aacc,
	0xf0f0f0f0,
	0x0f0f0f0f,
	0x87654321,
};


static
bool is_addr_volatile(uintptr_t addr)
{
	for (unsigned idx=0;idx<ARRAY_SIZE(test_patterns);idx++) {
		//write test pattern to requested address
		write32p(addr, test_patterns[idx]);
		asm volatile ("" ::: "memory");	//memory barrier

		// printk(BIOS_INFO, "Va %08x\n", read32p(addr));
		// printk(BIOS_INFO, "Vb %08x\n", read32p(addr));
		// printk(BIOS_INFO, "Vc %08x\n", read32p(addr));
		// printk(BIOS_INFO, "Vd %08x\n", read32p(addr));
		// printk(BIOS_INFO, "Ve %08x\n", read32p(addr));
		// printk(BIOS_INFO, "Vf %08x\n", read32p(addr));

#if 1
		read32p(addr);
		read32p(addr);
		read32p(addr);
		read32p(addr);
		read32p(addr);
		read32p(addr);
#endif

		// printk(BIOS_INFO, "VOL %08x\n", read32p(addr));

		/*
		asm volatile("rdtsc" :::);
		asm volatile("rdtsc" :::);
		asm volatile("rdtsc" :::);
		asm volatile("rdtsc" :::);
		asm volatile("rdtsc" :::);
		asm volatile("rdtsc" :::);
		asm volatile("rdtsc" :::);*/



		if (read32p(addr) != test_patterns[idx]) {
			return true;
		}
	}

	//all test patterns were stored and read back OK
	return false;
}


//toggle MSb of address (row or column) and check if aliasing to addres 0
//destroys content
static
bool is_aliasing(
	uintptr_t addr,
	u32 test
)
{
	//reset address 0 to ~test
	write32p(0, ~test);
	asm volatile ("" ::: "memory");	//memory barrier
	write32p(addr, test);
	asm volatile ("" ::: "memory");	//memory barrier

#if 1
	read32p(0);
	read32p(0);
	read32p(0);
	read32p(0);
	read32p(addr);
	read32p(addr);
	read32p(addr);
	read32p(addr);
	#endif

	// printk(BIOS_INFO, "ALIAS1 %08x %08x\n", read32p(0), read32p(addr));
	// printk(BIOS_INFO, "ALIAS2 %08x %08x\n", read32p(0), read32p(addr));
	// printk(BIOS_INFO, "ALIAS3 %08x %08x\n", read32p(0), read32p(addr));
	// printk(BIOS_INFO, "ALIAS4 %08x %08x\n", read32p(0), read32p(addr));
	// printk(BIOS_INFO, "ALIAS5 %08x %08x\n", read32p(0), read32p(addr));
	// printk(BIOS_INFO, "ALIAS6 %08x %08x\n", read32p(0), read32p(addr));

	//read from address 0 if aliasing
	if (read32p(0) == test) {
		//aliasing
		// printk(BIOS_INFO, "aliasing %08x\n", read32p(0));
		return true;
	} else {
		//not aliasing
		// printk(BIOS_INFO, "not aliasing %08x\n", read32p(0));
		return false;
	}
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



struct ma_size {
	u8 cpu_addr_bit;
	u8 mem_addr_size;
};


/*
 * 16M address
 * columns	rows
 * 3		14
 * 4		15
 * 5		16
 * 6		17
 * 7		18
 * 8		19
 * 9		20
 * 10		21
 * 2		22
 * 11		23
 * 12		24
 * 13		25
 */

static const struct ma_size row_table[] = {
	{25,	12},
	{24,	11},
	{23,	10},
	{22,	9},	//if A22=1 aliasing -> error
};

static const struct ma_size column_table[] = {
	{13,	12},
	{12,	11},
	{11,	10},
	{2,	9},
	{10,	8},	//if A10=1 aliasing -> error
};


static
u8 detect_address_size(const struct ma_size * table, unsigned count)
{
	//test if address 0 is non volatile
	if (is_addr_volatile(0)) {
		printk(BIOS_NOTICE, "Address 0 is volatile\n");
		return 0;
	}

	//detect CAS vector size
	for (unsigned idx=0;idx<count;idx++) {
		uintptr_t cpu_addr = 1UL << table[idx].cpu_addr_bit;
		u8 address_size = table[idx].mem_addr_size;

		// printk(BIOS_INFO, "test %08lx %u\n", cpu_addr, address_size);

		//test if tested address is non volatile
		if (is_addr_volatile(cpu_addr)) {
			/*
			 * address is volatile
			 * should either alias to nonvolatile address 0 or
			 * be nonvolatile itself
			 *
			 * access outside RAM?
			 */
			printk(BIOS_INFO, "Address %08lx is volatile, wrong region?\n",
			       cpu_addr
			);
			return 0;
		}

		/* can store arbitrary pattern without alias */
		if (! is_aliasing(cpu_addr, cpu_addr)) {
			printk(BIOS_INFO, "Address size %u\n", address_size);

			return address_size;
		}
	}

	printk(BIOS_WARNING, "Address size cannot be detected\n");

	return 0;
}


//ras_cas
enum simm_type {
	NONE	=	0xfU,
	R09_C09 =	0,
	R10_C10 =	1,
	R09_C10 =	2,
	R11_C11 =	3,
	R12_C12 =	4,
	R11_C10 =	5,
	R12_C10 =	6,
	R12_C09 =	7,
	R12_C08 =	8,
};


struct simm_map_type {
	u8 rows:4;
	u8 columns:4;
	enum simm_type type;
};


static const struct simm_map_type map_table[] = {
	{9, 9,		R09_C09},
	{10, 10,	R10_C10},
	{9, 10,		R09_C10},
	{11, 11,	R11_C11},
	{12, 12,	R12_C12},
	{11, 10,	R11_C10},
	{12, 10,	R12_C10},
	{12, 9,		R12_C09},
	{12, 8,		R12_C08},
};


static
enum simm_type detect_size(void)
{
	// printk(BIOS_INFO, "==%02x %02x %02x %02x\n",
	//	cfg_read(0x10),
	//	cfg_read(0x11),
	//	cfg_read(0x16),
	//	cfg_read(0x17)
	// );

	//detect CAS vector size
	u8 columns = detect_address_size(column_table, ARRAY_SIZE(column_table));
	if (columns == 0) {
		printk(BIOS_INFO, "Disable bank, no columns\n");

		return NONE;
	}

	//detect RAS vector size
	u8 rows = detect_address_size(row_table, ARRAY_SIZE(row_table));
	if (rows == 0) {
		printk(BIOS_WARNING, "Disable bank, row size error\n");

		return NONE;
	}

	for (unsigned idx = 0; idx < ARRAY_SIZE(map_table); idx++) {
		if (
			(map_table[idx].rows == rows) &&
			(map_table[idx].columns == columns)
		) {
			printk(BIOS_INFO, "Found %lukiB columns:%u rows:%u\n",
				4UL << (rows + columns - 10),
				columns,
				rows
			);

			return map_table[idx].type;
		}
	}

	//TODO can use nearest smaller capacity
	printk(BIOS_WARNING, "Disable unsupported: %lukiB columns:%u rows:%u\n",
		4UL << (rows + columns - 10),
		columns,
		rows
	);

	return NONE;
}


//EDO duck test: if it works like EDO -> it is EDO
static
bool detect_edo(void)
{
	bool is_edo = false;

	//edo test enable
	cfg_modify(0x1a, 0, 0x20);

	//TODO other patterns?
	// write32p(0, 0x55aa33ccUL);
	// asm volatile ("" ::: "memory");	//memory barrier

#if 0
	read32p(0);
	read32p(0);
	read32p(0);
	read32p(0);
	read32p(0);
	read32p(0);
	read32p(0);
#endif

	if (! is_addr_volatile(0)) {

	// if (read32p(0) == 0x55aa33ccUL) {
		is_edo = true;
		printk(BIOS_INFO, "EDO\n");
	} else {
		printk(BIOS_INFO, "FPM\n");
	}

	//edo test disable (other banks)
	cfg_modify(0x1a, 0x20, 0);

	return is_edo;
}



///////////////////////
///////////////////////
///////////////////////


enum l2_chip_size {
	L2_CHIP_NONE = 0,	//also reserved
	L2_CHIP_32K = 1,
	L2_CHIP_64K = 2,
	L2_CHIP_128K = 3,
};


void l2_cache_init(void)
{
	printk(BIOS_INFO, "Detect L2 cache\n");

	printk(BIOS_INFO, "[16]=0x%02x [17]=0x%02x\n",
	       cfg_read(0x16),
	       cfg_read(0x17)
	);

	//L2 force HIT, force NONDIRTY, don't force MISS
	//disable L1/L2 cacheable shadow
	//1 bank of cache
	//max cache size TODO ??
	cfg_modify(0x17, 4 | 8 | 0xc0 | 3, 0x20 | 0x10 | 3);
	// cfg_modify(0x17, 4 | 8 | 0xc0 | 3, 0x20 | 0x10 );

	//disable L1 cache, enable L2 cache
	cfg_modify(0x16, 1, 2);

	//NOTICE one would think, force HIT is even without defined any DRAM :-P
	//also NOTICE L2 as RAM would need the DRAM faked
	//also nobody noticed because power-on is with 1MB DRAM enabled :-P

	//set DRAM banks to max (so cache can be filled?)
	// cfg_write(0x10, (R12_C12 << 4) | R12_C12);
	// cfg_write(0x11, (R12_C12 << 4) | R12_C12);
	cfg_write(0x10, (NONE << 4) | R12_C12);

	if (is_addr_volatile(0)) {
		//no L2 cache
		printk(BIOS_INFO, "L2 cache not found\n");

		//deconfigure L2 cache
		cfg_modify(0x17, 4 | 8 | 0x20 | 0x10 | 3, L2_CHIP_NONE);

		//disable L2 cache
		cfg_modify(0x16, 2, 0);

		return;
	}

	//set 2 banks
	cfg_modify(0x17, 0, 4);
/*
	if (is_aliasing(4, 0xee7733cc) ) {
		printk(BIOS_NOTICE, "?? 1 bank\n");
	} else {
		printk(BIOS_NOTICE, "?? 2 banks\n");
	}*/

	// //write pattern to address 0x4
	// write32p(4, 0xcc3377ee);
 //
	// asm volatile ("" ::: "memory");	//barrier??

	printk(BIOS_NOTICE, "444444\n");

	bool has_2_banks = false;

	if (is_addr_volatile(4)) {
		//1 bank
		printk(BIOS_NOTICE, "1 bank\n");
		// cfg_modify(0x17, 4, 0);	//reset to 1 bank
	} else {
		//2 banks
		printk(BIOS_NOTICE, "2 banks\n");
		//leave registers as it is
		has_2_banks = true;
	}

	//NOTICE now we know bank configuration of L2 cache

#if 0
	//re-test if address 0 is non volatile ()
	if (is_addr_volatile(0)) {
		printk(BIOS_NOTICE, "Address 0 is volatile\n");
		cfg_modify(0x17, 3 | 4 | 8 | 0x20 | 0x10 | 3, L2_CHIP_NONE);
		return;
	}

	//TODO test addresses?
#endif

	//detect size

	enum l2_chip_size chip_size = L2_CHIP_NONE;

	//TODO error states

	//address 19
	if (! is_aliasing(1UL << 19, 0xcafebabe)) {
		//found 1MiB L2 cache (8x 128kiB)
		chip_size = L2_CHIP_128K;
		// has_2_banks = true;
	} else if (! is_aliasing(1UL << 18, 0xdeadbeef)) {
		//NOTICE bug in flowchart?
		chip_size = L2_CHIP_128K;
		// has_2_banks = !has_2_banks;
	} else if (! is_aliasing(1UL << 17, 0xcccccccc)) {
		//NOTICE bug in flowchart?
		chip_size = L2_CHIP_64K;
		// has_2_banks = !has_2_banks;
	} else if (! is_aliasing(1UL << 16, 0x33333333)) {
		chip_size = L2_CHIP_32K;
		// has_2_banks = false;
	}

	const u8 chip_table[4] = {0, 32, 64, 128};

	if (chip_size != L2_CHIP_NONE) {
		printk(BIOS_INFO, "%ukiB L2 cache, %u bank(s), %ukiB chips\n",
			chip_table[chip_size & 3]*4*(has_2_banks?2:1),
			has_2_banks?2:1,
			chip_table[chip_size & 3]
		);
	} else {
		printk(BIOS_INFO, "No L2 cache\n");
	}

	//disable L2 cache
	cfg_modify(0x16, 2, 0);

	//disable forced modes
	cfg_modify(0x17, 3 | 4 | 0x8 | 0x10 | 0x20, chip_size | (has_2_banks ? 4:0));

	return;
}


void dram_init(void)
{
	//TODO CDEF : ROM vs RAM vs ISA

	printk(BIOS_INFO, "DRAM initialization\n");

	printk(BIOS_INFO, "[16]=0x%02x [17]=0x%02x\n",
	       cfg_read(0x16),
	       cfg_read(0x17)
	);


	//reset DRAM banks to "none"
	cfg_write(0x10, (NONE << 4) | NONE);
	cfg_write(0x11, (NONE << 4) | NONE);

	//no hidden refresh, no ras timeout, C not in ROM, 15M not ISA
	cfg_modify(0x12, 0xc0 | 2 | 4 | 8, 0);

	//C, D in RAM
	cfg_write(0x13, 0xff);

	//E F NOT in RAM, shadow R/W, 2-1-1-1 reliability ???
	cfg_write(0x14, 0xf0);

	//disable L1 /KEN, disable L2 cache
	cfg_modify(0x16, 1 | 2, 0);

	//L2 always MISS, disable shadow cacheable L1/L2
	// cfg_modify(0x17, 0x10 | 0x20 | 0x40 | 0x80, 8);
	cfg_modify(0x17, 0x10 | 0x20 | 0x40 | 0x80, 0);

	//disable EDO, hidden 15us timing (but is disabled)
	// cfg_write(0x1a, 0x0);
	cfg_write(0x1a, 0x40);

	//slow timing, CAS-before-RAS, no parity, biggest CPU time slot (why not)
	// cfg_write(0x1b, 0xc0);
	cfg_write(0x1b, 0x00);

	//disable PCI memory hole
	cfg_modify(0x20, 0xc0, 0);

	//TODO FPU irqs?, system refresh (normal) to 15us? (safest?)
	cfg_modify(0x2a, 0x60, 0);

	//D region not in ROM
	cfg_modify(0x44, 0xc0, 0);

	//NOTICE set max addressing size and use address mapping to find unimplemented address wires in DRAM chip (will alias to (u32*)0)



#if 0
	printk(BIOS_INFO, "CFG ");

	for (unsigned idx=0;idx<16;idx++) {
		printk(BIOS_INFO, "_%01x ", idx);
	}

	printk(BIOS_INFO, "\n");

	for (unsigned idx=0;idx<0x50;idx++) {
		if ((idx % 16) == 0) {
			printk(BIOS_INFO, "%02x: ", idx);
		}

		printk(BIOS_INFO, "%02x ", cfg_read(idx));

		if ((idx % 16) == 15) {
			printk(BIOS_INFO, "\n");
		}
	}
#endif

	//todo
	cfg_write(0x1b, 0x45);
	cfg_write(0x20, 0x8);
	cfg_write(0x21, 0x32);
	cfg_write(0x2a, 0x45);
	cfg_modify(0x12, 0, 0x80);	//hidden

#if 0
	write32p(0, 0x11111111UL);
	read32p(0);
	write32p(0, 0x22222222UL);
	read32p(0);
	write32p(0, 0x33333333UL);
	read32p(0);
	write32p(0, 0x44444444UL);
	read32p(0);

	read32p(0);
	read32p(0);
#endif

	enum simm_type bank_size[4] = {NONE,NONE,NONE,NONE};


	//bank 0
	printk(BIOS_INFO, "Bank 0:\n");
	// asm volatile("rdtsc" :::);
	// asm volatile("rdtsc" :::);
	// asm volatile("rdtsc" :::);
	// asm volatile("rdtsc" :::);

	cfg_write(0x10, (NONE << 4) | R12_C12);	//64M addr (R/C = 12/12)
	// asm volatile("rdtsc" :::);

// cfg_modify(0x1a, 0xf, 0);


	bank_size[0] = detect_size();
	if (bank_size[0] != NONE) {
		//set EDO for bank
		cfg_modify(0x1a, 0, 1 << 0);
		detect_edo();	//NOTICE EDO bit will stay on
	}
	// cfg_write(0x10, (NONE << 4) | NONE);

	// cfg_modify(0x1a, 0xf, 0);

	//bank 1
	printk(BIOS_INFO, "Bank 1:\n");
	cfg_write(0x10, (R12_C12 << 4) | NONE);	//64M addr (R/C = 12/12)
	bank_size[1] = detect_size();
	if (bank_size[1] != NONE) {
		//set EDO for bank
		cfg_modify(0x1a, 0, 1 << 1);
		detect_edo();	//NOTICE EDO bit will stay on
	}
	// cfg_write(0x10, (NONE << 4) | NONE);

	// cfg_modify(0x1a, 0xf, 0);

	//other register
	cfg_write(0x10, (NONE << 4) | NONE);


	//bank 2
	printk(BIOS_INFO, "Bank 2:\n");
	cfg_write(0x11, (NONE << 4) | R12_C12);	//64M addr (R/C = 12/12)
	bank_size[2] = detect_size();
	if (bank_size[2] != NONE) {
		//set EDO for bank
		cfg_modify(0x1a, 0, 1 << 2);
		detect_edo();	//NOTICE EDO bit will stay on
	}
	// cfg_write(0x11, (NONE << 4) | NONE);

	// cfg_modify(0x1a, 0xf, 0);

	//bank 3
	printk(BIOS_INFO, "Bank 3:\n");
	cfg_write(0x11, (R12_C12 << 4) | NONE);	//64M addr (R/C = 12/12)
	bank_size[3] = detect_size();
	if (bank_size[3] != NONE) {
		//set EDO for bank
		cfg_modify(0x1a, 0, 1 << 3);
		detect_edo();	//NOTICE EDO bit will stay on
	}
	// cfg_write(0x11, (NONE << 4) | NONE);

	//enable DRAM

	cfg_write(0x10,
		((bank_size[1] << 4) & 0xf0) |
		(bank_size[0] & 0xf)
	);

	cfg_write(0x11,
		((bank_size[3] << 4) & 0xf0) |
		(bank_size[2] & 0xf)
	);

	//reenable l2 cache, if was enabled before
	//TODO detect it from reserved config in 0x17
	// if (is_l2_enabled) {
	// 	cfg_modify(0x17, 0, 2);
	// }

	//reenable l1 cache, if was enabled before
	// if (is_l1_enabled) {
	// 	cfg_modify(0x17, 0, 1);
	// }

	//TODO enable refresh from cmos


	printk(BIOS_INFO, "DRAM enabled\n");
}
