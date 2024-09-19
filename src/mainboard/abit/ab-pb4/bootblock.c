/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootblock_common.h>
#include <superio/winbond/w83787if/winbond.h>
// #include <superio/winbond/w83977tf/w83977tf.h>

void bootblock_mainboard_early_init(void)
{
	//enable superio serial

	winbond_enable_serial(0, CONFIG_TTYS0_BASE);
}
