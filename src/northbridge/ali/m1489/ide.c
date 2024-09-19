//page 81
/*
 * IDE _0 _1 _2 _3 _4 _5 _6 _7 _8 _9 _a _b _c _d _e _f
 * 00: 57 03 00 03 03 06 0e 1f ff 41 01 00 02 00 02 00
 * 10: 02 00 02 ff ff ff ff ff ff ff ff ff ff ff ff ff
 * 20: ff ff ff ff ff 00 01 01 0e 16 00 06 0e 06 0e 00
 * 30: 0e 16 0e 16 ff 01 ff ff ff ff ff ff ff ff ff ff
 */

#include <console/console.h>
#include <arch/io.h>
#include <stdbool.h>

#include "ide.h"

#define PORT_ID		0xf4
#define PORT_INDEX	0xf8
#define PORT_DATA	0xfc

struct ali_ide_entry {
	u8 idx;
	u8 val;
};


static __always_inline
u8 ide_read(u8 idx)
{
	outb(idx, PORT_INDEX);
	return inb(PORT_DATA);
}

static __always_inline
void ide_write(u8 idx, u8 val)
{
	outb(idx, PORT_INDEX);
	outb(val, PORT_DATA);
}

static __always_inline
void ide_unlock(void)
{
	outb(0x30, PORT_ID);
}

static __always_inline
void ide_lock(void)
{
	outb(0x00, PORT_ID);
}


#define PIO0_ACTIVE_TIME_16	165
#define PIO0_ACTIVE_TIME_8	290
#define PIO0_CYCLE_TIME		600

#define PIO1_ACTIVE_TIME_16	125
#define PIO1_ACTIVE_TIME_8	290
#define PIO1_CYCLE_TIME		383

#define PIO2_ACTIVE_TIME_16	100
#define PIO2_ACTIVE_TIME_8	290
#define PIO2_CYCLE_TIME		240

#define PIO3_ACTIVE_TIME_16	80
#define PIO3_ACTIVE_TIME_8	80
#define PIO3_CYCLE_TIME		180

#define PIO4_ACTIVE_TIME_16	70
#define PIO4_ACTIVE_TIME_8	70
#define PIO4_CYCLE_TIME		120


#define TIME_RC(bus_mhz, cyc_t)		(((cyc_t)*(bus_mhz)+999)/1000)

#define TIME_RA(bus_mhz, act_t)		(((act_t)*(bus_mhz)+999)/1000)
#define TIME_WA(bus_mhz, act_t)		(TIME_RA(bus_mhz, act_t) + 8)

#define TIME_RR(bus_mhz, cyc_t, act_t)	(TIME_RC(bus_mhz, cyc_t) - TIME_RA(bus_mhz, act_t))
#define TIME_WR(bus_mhz, cyc_t, act_t)	(TIME_RR(bus_mhz, cyc_t, act_t) + 8)



struct ide_timing {
	u8 byte_active;		//8bit
	u8 byte_recovery;	//8bit
	u8 read_active;		//16bit
	u8 write_active;	//16bit
	u8 read_recovery;	//16bit
	u8 write_recovery;	//16bit
};


#define IDE_TIMING(_name_, _bus_mhz_, _act16_, _act8_, _cyc_)	\
struct ide_timing _name_ = {					\
	.byte_active = TIME_WA(_bus_mhz_, _act8_),		\
	.byte_recovery = TIME_WR(_bus_mhz_, _cyc_, _act8_),	\
	.read_active = TIME_RA(_bus_mhz_, _act16_),		\
	.write_active = TIME_WA(_bus_mhz_, _act16_),		\
	.read_recovery = TIME_RR(_bus_mhz_, _cyc_, _act16_),	\
	.write_recovery = TIME_WR(_bus_mhz_, _cyc_, _act16_),	\
}

#define IDE_SLOWEST(_name_)	\
struct ide_timing _name_ = {					\
	.byte_active = 32,		\
	.byte_recovery = 32,	\
	.read_active = 32,		\
	.write_active = 32,		\
	.read_recovery = 32,	\
	.write_recovery = 32,	\
}



// static IDE_TIMING(pio0_33mhz, 33, PIO0_ACTIVE_TIME_16, PIO0_ACTIVE_TIME_8, PIO0_CYCLE_TIME);
// static IDE_TIMING(pio0_33mhz, 33, PIO4_ACTIVE_TIME_16, PIO4_ACTIVE_TIME_8, PIO4_CYCLE_TIME);
// static IDE_TIMING(pio0_33mhz, 33, PIO3_ACTIVE_TIME_16, PIO3_ACTIVE_TIME_8, PIO3_CYCLE_TIME);
static IDE_SLOWEST(pio0_33mhz);

//page 81
static const struct ali_ide_entry ide_table[] = {
	// {0x01, 0x03},	//enable IDE
	{0x01, 0x0f},	//enable IDE, data after T2

	// {0x02, 0},
	// {0x03, 3},
	// {0x04, 3},
	// {0x05, 6},
	// {0x06, 14},


	// {0x07, 0x7f},	//from original 0x1f
	{0x07, 0x80},		//buffer disabled

	//8 readonly

	// {0x09, 0x41},
	{0x09, 0x43},		//enabled 3F7 and 3F6

	// {0x0a, 0x4f},	//original 0x01, enable cdrom support, all IDE devices
	// {0x0a, 0x01},	//original 0x01, enable cdrom support, all IDE devices
	// {0x0a, 0x01},	//original = 0x01
	// {0x0a, 0x0f},	//original = 0x01
	{0x0a, 0x00},		//no buffer

	//b/c cha1.0 sector byte count
	{0x0b, 0},
	// {0x0c, 2},
	{0x0c, 0},

	//d/e cha1.1 sector byte count
	{0x0d, 0},
	//{0x0e, 2},
	{0x0e, 0},

	//f/10 cha2.0 sector byte count
	{0x0f, 0},
	// {0x10, 2},
	{0x10, 0},

	//11/12 cha2.1 sector byte count
	{0x11, 0},
	// {0x12, 2},
	{0x12, 0},

	// {0x25, 0},
	// {0x26, 1},
	// {0x27, 1},
	// {0x28, 14},
	// {0x29, 22},
	// {0x2a, 0},
	// {0x2b, 6},
	// {0x2c, 14},
	// {0x2d, 6},
	// {0x2e, 14},
	// {0x2f, 0},
	// {0x30, 14},
	// {0x31, 22},
	// {0x32, 14},
	// {0x33, 22},

	//0x34 readonly


	{0x34, 0x00},	//decoded

	// {0x35, 0x01},	//also RAID
	{0x35, 0x41},	//also RAID

	//NOTICE NOTICE NOTICE
	//0x41 -> reg 0x35 disables secondary IDE
	//0x01 -> reg 0x35 enables secondary IDE

};

#define IDE_PRIMARY	(true)
#define IDE_SECONDARY	(false)

#define IDE_MASTER	(true)
#define IDE_SLAVE	(false)

static __always_inline
void ide_drive_timing(
	bool primary_channel,
	bool master,
	struct ide_timing * t
)
{
	u8 idx_ra;
	u8 idx_wa;
	u8 idx_rr;
	u8 idx_wr;

	if (primary_channel) {	//channel 1
		if (master) {
			idx_ra = 0x03;
			idx_wa = 0x04;
			idx_rr = 0x26;
			idx_wr = 0x27;
		} else {
			idx_ra = 0x05;
			idx_wa = 0x06;
			idx_rr = 0x28;
			idx_wr = 0x29;
		}
	} else {	//channel 2
		if (master) {
			idx_ra = 0x2b;
			idx_wa = 0x2c;
			idx_rr = 0x30;
			idx_wr = 0x31;
		} else {
			idx_ra = 0x2d;
			idx_wa = 0x2e;
			idx_rr = 0x32;
			idx_wr = 0x33;
		}
	}

	ide_write(idx_ra, t->read_active<32 ? t->read_active:0);
	ide_write(idx_wa, t->write_active<32 ? t->write_active:0);
	ide_write(idx_rr, t->read_recovery<32 ? t->read_recovery:0);
	ide_write(idx_wr, t->write_recovery<32 ? t->write_recovery:0);
}


static __always_inline
void ide_channel_timing(
	bool primary_channel,
	struct ide_timing * t
)
{
	u8 idx_ba;
	u8 idx_br;

	if (primary_channel) {
		//channel 1
		idx_ba = 0x02;
		idx_br = 0x25;
	} else {
		//channel 2
		idx_ba = 0x2a;
		idx_br = 0x2f;
	}

	ide_write(idx_ba, t->byte_active<32 ? t->byte_active:0);
	ide_write(idx_br, t->byte_recovery<32 ? t->byte_recovery:0);
}


void ali_ide_init(void)
{
	printk(BIOS_DEBUG, "enable IDE\n");


	ide_unlock();

/*
	ide_write(0x1, 0x3);	//enable IDE

	ide_write(0x7, 0x7f);

	ide_write(0x9, 0x41);*/

	for (unsigned idx=0;idx<ARRAY_SIZE(ide_table);idx++) {
		ide_write(ide_table[idx].idx, ide_table[idx].val);
	}

	ide_drive_timing(IDE_PRIMARY, IDE_MASTER, &pio0_33mhz);
	ide_drive_timing(IDE_PRIMARY, IDE_SLAVE, &pio0_33mhz);
	ide_channel_timing(IDE_PRIMARY, &pio0_33mhz);

	ide_drive_timing(IDE_SECONDARY, IDE_MASTER, &pio0_33mhz);
	ide_drive_timing(IDE_SECONDARY, IDE_SLAVE, &pio0_33mhz);
	ide_channel_timing(IDE_SECONDARY, &pio0_33mhz);


	printk(BIOS_DEBUG, "CFG ");
	for (unsigned idx=0;idx<16;idx++) {
		printk(BIOS_DEBUG, "_%01x ", idx);
	}
	printk(BIOS_DEBUG, "\n");
	for (unsigned idx=0;idx<0x100;idx++) {
		if ((idx % 16) == 0) {
			printk(BIOS_DEBUG, "%02x: ", idx);
		}
		printk(BIOS_DEBUG, "%02x ", ide_read(idx));
		if ((idx % 16) == 15) {
			printk(BIOS_DEBUG, "\n");
		}
	}



	//must be locked or wont work
	ide_lock();
}
