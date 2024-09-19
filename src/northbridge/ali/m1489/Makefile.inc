## SPDX-License-Identifier: GPL-2.0-or-later

ifeq ($(CONFIG_NORTHBRIDGE_ALI_M1489),y)

bootblock-y += bootblock.c

ramstage-y += northbridge.c
ramstage-y += ide.c

romstage-y += raminit.c
romstage-y += romstage.c
# romstage-$(CONFIG_DEBUG_RAM_SETUP) += debug.c
romstage-y += memmap.c

#enable L1 in chipset
postcar-y += postcar.c

ramstage-y += ramstage_exit.c

endif
