## SPDX-License-Identifier: GPL-2.0-or-later

ifeq ($(CONFIG_NORTHBRIDGE_UMC_UM8881),y)

# https://doc.coreboot.org/_images/comparison_coreboot_uefi.svg

bootblock-y += bootblock.c

romstage-y += reg_init.c
romstage-y += raminit.c
romstage-y += romstage.c
romstage-y += memmap.c

#enable L1 in chipset
postcar-y += postcar.c

ramstage-y += reg_init.c
ramstage-y += northbridge.c
ramstage-y += ramstage_exit.c
ramstage-y += memmap.c

endif
