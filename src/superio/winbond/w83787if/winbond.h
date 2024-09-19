/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_WINBOND_W83787IF_WINBOND_H
#define SUPERIO_WINBOND_W83787IF_WINBOND_H

// #include <device/pnp_type.h>
#include <stdint.h>

void winbond_enable_serial(u8 which, u16 iobase);
void winbond_enter_conf_state(void);
void winbond_exit_conf_state(void);

#endif /* SUPERIO_WINBOND_W83787IF_WINBOND_H */
