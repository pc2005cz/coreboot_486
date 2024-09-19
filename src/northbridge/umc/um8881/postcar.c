/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "reg_init.h"

void late_car_teardown(void);


void late_car_teardown(void)
{
	//L2 must be enabled/made coherent right after getting from 0xffff0000
	//it is too late here
}
