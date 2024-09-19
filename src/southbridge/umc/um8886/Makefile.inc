## SPDX-License-Identifier: GPL-2.0-or-later

ifeq ($(CONFIG_SOUTHBRIDGE_UMC_UM8886),y)

romstage-y += early_init.c

ramstage-y += ide.c
ramstage-y += um8886.c
ramstage-$(CONFIG_GENERATE_PIRQ_TABLE) += pirq.c

endif
