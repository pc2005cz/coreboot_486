bootblock-y += bootblock.c
romstage-y += romstage.c

ramstage-$(CONFIG_GENERATE_PIRQ_TABLE) += irq_tables.c
