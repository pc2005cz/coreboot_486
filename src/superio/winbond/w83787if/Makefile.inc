# SPDX-License-Identifier: GPL-2.0-or-later

bootblock-$(CONFIG_SUPERIO_WINBOND_W83787IF) += early_init.c
romstage-$(CONFIG_SUPERIO_WINBOND_W83787IF) += early_init.c
ramstage-$(CONFIG_SUPERIO_WINBOND_W83787IF) += superio.c
