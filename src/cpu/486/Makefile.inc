## SPDX-License-Identifier: GPL-2.0-or-later

bootblock-y += cache_as_ram.S

#TODO maybe add https://elixir.bootlin.com/coreboot/4.19/source/src/cpu/intel/model_206ax/bootblock.c#L70
#void bootblock_early_cpu_init(void) ?

#done https://elixir.bootlin.com/coreboot/4.19/source/src/cpu/intel/car/bootblock.c#L10

bootblock-y += bootblock.c

romstage-y += romstage.c

ramstage-y += 486.c

#TODO are unused functions automatically removed?
bootblock-y += delay_pit.c
ramstage-y += delay_pit.c
romstage-y += delay_pit.c
verstage_x86-y += delay_pit.c
postcar-y += delay_pit.c
smm-y += delay_pit.c

postcar-y += exit_car.S

ramstage-y += name.c

bootblock-y += entry32.S
bootblock-y += entry16.S
bootblock-y += reset16.S

ramstage-y += sp_init.c
