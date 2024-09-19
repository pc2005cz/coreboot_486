## SPDX-License-Identifier: GPL-2.0-or-later

ifeq ($(CONFIG_NORTHBRIDGE_SIS_85C496),y)

ramstage-y += northbridge.c

romstage-y += raminit.c
romstage-y += romstage.c
# romstage-$(CONFIG_DEBUG_RAM_SETUP) += debug.c
romstage-y += memmap.c


endif
