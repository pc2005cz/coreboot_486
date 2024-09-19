/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "regs.h"

void late_car_teardown(void);


void late_car_teardown(void)
{
	//enable L1 generation, + writeback (TODO CPU detection)
	cfg_modify(0x16, 0, 0x1 | 0x4);
}
