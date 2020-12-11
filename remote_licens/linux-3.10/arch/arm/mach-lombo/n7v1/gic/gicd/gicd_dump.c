/* gicd_dump.c */

#ifndef ___GICD___DUMP__C___
#define ___GICD___DUMP__C___

reg_info_t gicd_info[] = {
	{VA_GICD_CTLR, DATA_GICD_CTLR, dump_gicd_ctlr, "GICD_CTLR", 0},
	{VA_GICD_TYPER, DATA_GICD_TYPER, dump_gicd_typer, "GICD_TYPER", 0},
	{VA_GICD_IIDR, DATA_GICD_IIDR, dump_gicd_iidr, "GICD_IIDR", 0},
	{VA_GICD_IGROUPR0, DATA_GICD_IGROUPR0, dump_gicd_igroupr0, "GICD_IGROUPR0", 0},
	{VA_GICD_IGROUPR1, DATA_GICD_IGROUPR1, dump_gicd_igroupr1, "GICD_IGROUPR1", 0},
	{VA_GICD_IGROUPR2, DATA_GICD_IGROUPR2, dump_gicd_igroupr2, "GICD_IGROUPR2", 0},
	{VA_GICD_IGROUPR3, DATA_GICD_IGROUPR3, dump_gicd_igroupr3, "GICD_IGROUPR3", 0},
	{VA_GICD_IGROUPR4, DATA_GICD_IGROUPR4, dump_gicd_igroupr4, "GICD_IGROUPR4", 0},
	{VA_GICD_IGROUPR5, DATA_GICD_IGROUPR5, dump_gicd_igroupr5, "GICD_IGROUPR5", 0},
	{VA_GICD_IGROUPR6, DATA_GICD_IGROUPR6, dump_gicd_igroupr6, "GICD_IGROUPR6", 0},
	{VA_GICD_ISENABLER0, DATA_GICD_ISENABLER0, dump_gicd_isenabler0, "GICD_ISENABLER0", 0},
	{VA_GICD_ISENABLER1, DATA_GICD_ISENABLER1, dump_gicd_isenabler1, "GICD_ISENABLER1", 0},
	{VA_GICD_ISENABLER2, DATA_GICD_ISENABLER2, dump_gicd_isenabler2, "GICD_ISENABLER2", 0},
	{VA_GICD_ISENABLER3, DATA_GICD_ISENABLER3, dump_gicd_isenabler3, "GICD_ISENABLER3", 0},
	{VA_GICD_ISENABLER4, DATA_GICD_ISENABLER4, dump_gicd_isenabler4, "GICD_ISENABLER4", 0},
	{VA_GICD_ISENABLER5, DATA_GICD_ISENABLER5, dump_gicd_isenabler5, "GICD_ISENABLER5", 0},
	{VA_GICD_ISENABLER6, DATA_GICD_ISENABLER6, dump_gicd_isenabler6, "GICD_ISENABLER6", 0},
	{VA_GICD_ICENABLER0, DATA_GICD_ICENABLER0, dump_gicd_icenabler0, "GICD_ICENABLER0", 0},
	{VA_GICD_ICENABLER1, DATA_GICD_ICENABLER1, dump_gicd_icenabler1, "GICD_ICENABLER1", 0},
	{VA_GICD_ICENABLER2, DATA_GICD_ICENABLER2, dump_gicd_icenabler2, "GICD_ICENABLER2", 0},
	{VA_GICD_ICENABLER3, DATA_GICD_ICENABLER3, dump_gicd_icenabler3, "GICD_ICENABLER3", 0},
	{VA_GICD_ICENABLER4, DATA_GICD_ICENABLER4, dump_gicd_icenabler4, "GICD_ICENABLER4", 0},
	{VA_GICD_ICENABLER5, DATA_GICD_ICENABLER5, dump_gicd_icenabler5, "GICD_ICENABLER5", 0},
	{VA_GICD_ICENABLER6, DATA_GICD_ICENABLER6, dump_gicd_icenabler6, "GICD_ICENABLER6", 0},
	{VA_GICD_ISPENDR0, DATA_GICD_ISPENDR0, dump_gicd_ispendr0, "GICD_ISPENDR0", 0},
	{VA_GICD_ISPENDR1, DATA_GICD_ISPENDR1, dump_gicd_ispendr1, "GICD_ISPENDR1", 0},
	{VA_GICD_ISPENDR2, DATA_GICD_ISPENDR2, dump_gicd_ispendr2, "GICD_ISPENDR2", 0},
	{VA_GICD_ISPENDR3, DATA_GICD_ISPENDR3, dump_gicd_ispendr3, "GICD_ISPENDR3", 0},
	{VA_GICD_ISPENDR4, DATA_GICD_ISPENDR4, dump_gicd_ispendr4, "GICD_ISPENDR4", 0},
	{VA_GICD_ISPENDR5, DATA_GICD_ISPENDR5, dump_gicd_ispendr5, "GICD_ISPENDR5", 0},
	{VA_GICD_ISPENDR6, DATA_GICD_ISPENDR6, dump_gicd_ispendr6, "GICD_ISPENDR6", 0},
	{VA_GICD_ICPENDR0, DATA_GICD_ICPENDR0, dump_gicd_icpendr0, "GICD_ICPENDR0", 0},
	{VA_GICD_ICPENDR1, DATA_GICD_ICPENDR1, dump_gicd_icpendr1, "GICD_ICPENDR1", 0},
	{VA_GICD_ICPENDR2, DATA_GICD_ICPENDR2, dump_gicd_icpendr2, "GICD_ICPENDR2", 0},
	{VA_GICD_ICPENDR3, DATA_GICD_ICPENDR3, dump_gicd_icpendr3, "GICD_ICPENDR3", 0},
	{VA_GICD_ICPENDR4, DATA_GICD_ICPENDR4, dump_gicd_icpendr4, "GICD_ICPENDR4", 0},
	{VA_GICD_ICPENDR5, DATA_GICD_ICPENDR5, dump_gicd_icpendr5, "GICD_ICPENDR5", 0},
	{VA_GICD_ICPENDR6, DATA_GICD_ICPENDR6, dump_gicd_icpendr6, "GICD_ICPENDR6", 0},
	{VA_GICD_ISACTIVER0, DATA_GICD_ISACTIVER0, dump_gicd_isactiver0, "GICD_ISACTIVER0", 0},
	{VA_GICD_ISACTIVER1, DATA_GICD_ISACTIVER1, dump_gicd_isactiver1, "GICD_ISACTIVER1", 0},
	{VA_GICD_ISACTIVER2, DATA_GICD_ISACTIVER2, dump_gicd_isactiver2, "GICD_ISACTIVER2", 0},
	{VA_GICD_ISACTIVER3, DATA_GICD_ISACTIVER3, dump_gicd_isactiver3, "GICD_ISACTIVER3", 0},
	{VA_GICD_ISACTIVER4, DATA_GICD_ISACTIVER4, dump_gicd_isactiver4, "GICD_ISACTIVER4", 0},
	{VA_GICD_ISACTIVER5, DATA_GICD_ISACTIVER5, dump_gicd_isactiver5, "GICD_ISACTIVER5", 0},
	{VA_GICD_ISACTIVER6, DATA_GICD_ISACTIVER6, dump_gicd_isactiver6, "GICD_ISACTIVER6", 0},
	{VA_GICD_ICACTIVER0, DATA_GICD_ICACTIVER0, dump_gicd_icactiver0, "GICD_ICACTIVER0", 0},
	{VA_GICD_ICACTIVER1, DATA_GICD_ICACTIVER1, dump_gicd_icactiver1, "GICD_ICACTIVER1", 0},
	{VA_GICD_ICACTIVER2, DATA_GICD_ICACTIVER2, dump_gicd_icactiver2, "GICD_ICACTIVER2", 0},
	{VA_GICD_ICACTIVER3, DATA_GICD_ICACTIVER3, dump_gicd_icactiver3, "GICD_ICACTIVER3", 0},
	{VA_GICD_ICACTIVER4, DATA_GICD_ICACTIVER4, dump_gicd_icactiver4, "GICD_ICACTIVER4", 0},
	{VA_GICD_ICACTIVER5, DATA_GICD_ICACTIVER5, dump_gicd_icactiver5, "GICD_ICACTIVER5", 0},
	{VA_GICD_ICACTIVER6, DATA_GICD_ICACTIVER6, dump_gicd_icactiver6, "GICD_ICACTIVER6", 0},
	{VA_GICD_IPRIORITYR0, DATA_GICD_IPRIORITYR0, dump_gicd_ipriorityr0, "GICD_IPRIORITYR0", 0},
	{VA_GICD_IPRIORITYR1, DATA_GICD_IPRIORITYR1, dump_gicd_ipriorityr1, "GICD_IPRIORITYR1", 0},
	{VA_GICD_IPRIORITYR2, DATA_GICD_IPRIORITYR2, dump_gicd_ipriorityr2, "GICD_IPRIORITYR2", 0},
	{VA_GICD_IPRIORITYR3, DATA_GICD_IPRIORITYR3, dump_gicd_ipriorityr3, "GICD_IPRIORITYR3", 0},
	{VA_GICD_IPRIORITYR4, DATA_GICD_IPRIORITYR4, dump_gicd_ipriorityr4, "GICD_IPRIORITYR4", 0},
	{VA_GICD_IPRIORITYR5, DATA_GICD_IPRIORITYR5, dump_gicd_ipriorityr5, "GICD_IPRIORITYR5", 0},
	{VA_GICD_IPRIORITYR6, DATA_GICD_IPRIORITYR6, dump_gicd_ipriorityr6, "GICD_IPRIORITYR6", 0},
	{VA_GICD_IPRIORITYR7, DATA_GICD_IPRIORITYR7, dump_gicd_ipriorityr7, "GICD_IPRIORITYR7", 0},
	{VA_GICD_IPRIORITYR8, DATA_GICD_IPRIORITYR8, dump_gicd_ipriorityr8, "GICD_IPRIORITYR8", 0},
	{VA_GICD_IPRIORITYR9, DATA_GICD_IPRIORITYR9, dump_gicd_ipriorityr9, "GICD_IPRIORITYR9", 0},
	{VA_GICD_IPRIORITYR10, DATA_GICD_IPRIORITYR10, dump_gicd_ipriorityr10, "GICD_IPRIORITYR10", 0},
	{VA_GICD_IPRIORITYR11, DATA_GICD_IPRIORITYR11, dump_gicd_ipriorityr11, "GICD_IPRIORITYR11", 0},
	{VA_GICD_IPRIORITYR12, DATA_GICD_IPRIORITYR12, dump_gicd_ipriorityr12, "GICD_IPRIORITYR12", 0},
	{VA_GICD_IPRIORITYR13, DATA_GICD_IPRIORITYR13, dump_gicd_ipriorityr13, "GICD_IPRIORITYR13", 0},
	{VA_GICD_IPRIORITYR14, DATA_GICD_IPRIORITYR14, dump_gicd_ipriorityr14, "GICD_IPRIORITYR14", 0},
	{VA_GICD_IPRIORITYR15, DATA_GICD_IPRIORITYR15, dump_gicd_ipriorityr15, "GICD_IPRIORITYR15", 0},
	{VA_GICD_IPRIORITYR16, DATA_GICD_IPRIORITYR16, dump_gicd_ipriorityr16, "GICD_IPRIORITYR16", 0},
	{VA_GICD_IPRIORITYR17, DATA_GICD_IPRIORITYR17, dump_gicd_ipriorityr17, "GICD_IPRIORITYR17", 0},
	{VA_GICD_IPRIORITYR18, DATA_GICD_IPRIORITYR18, dump_gicd_ipriorityr18, "GICD_IPRIORITYR18", 0},
	{VA_GICD_IPRIORITYR19, DATA_GICD_IPRIORITYR19, dump_gicd_ipriorityr19, "GICD_IPRIORITYR19", 0},
	{VA_GICD_IPRIORITYR20, DATA_GICD_IPRIORITYR20, dump_gicd_ipriorityr20, "GICD_IPRIORITYR20", 0},
	{VA_GICD_IPRIORITYR21, DATA_GICD_IPRIORITYR21, dump_gicd_ipriorityr21, "GICD_IPRIORITYR21", 0},
	{VA_GICD_IPRIORITYR22, DATA_GICD_IPRIORITYR22, dump_gicd_ipriorityr22, "GICD_IPRIORITYR22", 0},
	{VA_GICD_IPRIORITYR23, DATA_GICD_IPRIORITYR23, dump_gicd_ipriorityr23, "GICD_IPRIORITYR23", 0},
	{VA_GICD_IPRIORITYR24, DATA_GICD_IPRIORITYR24, dump_gicd_ipriorityr24, "GICD_IPRIORITYR24", 0},
	{VA_GICD_IPRIORITYR25, DATA_GICD_IPRIORITYR25, dump_gicd_ipriorityr25, "GICD_IPRIORITYR25", 0},
	{VA_GICD_IPRIORITYR26, DATA_GICD_IPRIORITYR26, dump_gicd_ipriorityr26, "GICD_IPRIORITYR26", 0},
	{VA_GICD_IPRIORITYR27, DATA_GICD_IPRIORITYR27, dump_gicd_ipriorityr27, "GICD_IPRIORITYR27", 0},
	{VA_GICD_IPRIORITYR28, DATA_GICD_IPRIORITYR28, dump_gicd_ipriorityr28, "GICD_IPRIORITYR28", 0},
	{VA_GICD_IPRIORITYR29, DATA_GICD_IPRIORITYR29, dump_gicd_ipriorityr29, "GICD_IPRIORITYR29", 0},
	{VA_GICD_IPRIORITYR30, DATA_GICD_IPRIORITYR30, dump_gicd_ipriorityr30, "GICD_IPRIORITYR30", 0},
	{VA_GICD_IPRIORITYR31, DATA_GICD_IPRIORITYR31, dump_gicd_ipriorityr31, "GICD_IPRIORITYR31", 0},
	{VA_GICD_IPRIORITYR32, DATA_GICD_IPRIORITYR32, dump_gicd_ipriorityr32, "GICD_IPRIORITYR32", 0},
	{VA_GICD_IPRIORITYR33, DATA_GICD_IPRIORITYR33, dump_gicd_ipriorityr33, "GICD_IPRIORITYR33", 0},
	{VA_GICD_IPRIORITYR34, DATA_GICD_IPRIORITYR34, dump_gicd_ipriorityr34, "GICD_IPRIORITYR34", 0},
	{VA_GICD_IPRIORITYR35, DATA_GICD_IPRIORITYR35, dump_gicd_ipriorityr35, "GICD_IPRIORITYR35", 0},
	{VA_GICD_IPRIORITYR36, DATA_GICD_IPRIORITYR36, dump_gicd_ipriorityr36, "GICD_IPRIORITYR36", 0},
	{VA_GICD_IPRIORITYR37, DATA_GICD_IPRIORITYR37, dump_gicd_ipriorityr37, "GICD_IPRIORITYR37", 0},
	{VA_GICD_IPRIORITYR38, DATA_GICD_IPRIORITYR38, dump_gicd_ipriorityr38, "GICD_IPRIORITYR38", 0},
	{VA_GICD_IPRIORITYR39, DATA_GICD_IPRIORITYR39, dump_gicd_ipriorityr39, "GICD_IPRIORITYR39", 0},
	{VA_GICD_IPRIORITYR40, DATA_GICD_IPRIORITYR40, dump_gicd_ipriorityr40, "GICD_IPRIORITYR40", 0},
	{VA_GICD_IPRIORITYR41, DATA_GICD_IPRIORITYR41, dump_gicd_ipriorityr41, "GICD_IPRIORITYR41", 0},
	{VA_GICD_IPRIORITYR42, DATA_GICD_IPRIORITYR42, dump_gicd_ipriorityr42, "GICD_IPRIORITYR42", 0},
	{VA_GICD_IPRIORITYR43, DATA_GICD_IPRIORITYR43, dump_gicd_ipriorityr43, "GICD_IPRIORITYR43", 0},
	{VA_GICD_IPRIORITYR44, DATA_GICD_IPRIORITYR44, dump_gicd_ipriorityr44, "GICD_IPRIORITYR44", 0},
	{VA_GICD_IPRIORITYR45, DATA_GICD_IPRIORITYR45, dump_gicd_ipriorityr45, "GICD_IPRIORITYR45", 0},
	{VA_GICD_IPRIORITYR46, DATA_GICD_IPRIORITYR46, dump_gicd_ipriorityr46, "GICD_IPRIORITYR46", 0},
	{VA_GICD_IPRIORITYR47, DATA_GICD_IPRIORITYR47, dump_gicd_ipriorityr47, "GICD_IPRIORITYR47", 0},
	{VA_GICD_IPRIORITYR48, DATA_GICD_IPRIORITYR48, dump_gicd_ipriorityr48, "GICD_IPRIORITYR48", 0},
	{VA_GICD_IPRIORITYR49, DATA_GICD_IPRIORITYR49, dump_gicd_ipriorityr49, "GICD_IPRIORITYR49", 0},
	{VA_GICD_IPRIORITYR50, DATA_GICD_IPRIORITYR50, dump_gicd_ipriorityr50, "GICD_IPRIORITYR50", 0},
	{VA_GICD_IPRIORITYR51, DATA_GICD_IPRIORITYR51, dump_gicd_ipriorityr51, "GICD_IPRIORITYR51", 0},
	{VA_GICD_IPRIORITYR52, DATA_GICD_IPRIORITYR52, dump_gicd_ipriorityr52, "GICD_IPRIORITYR52", 0},
	{VA_GICD_IPRIORITYR53, DATA_GICD_IPRIORITYR53, dump_gicd_ipriorityr53, "GICD_IPRIORITYR53", 0},
	{VA_GICD_IPRIORITYR54, DATA_GICD_IPRIORITYR54, dump_gicd_ipriorityr54, "GICD_IPRIORITYR54", 0},
	{VA_GICD_IPRIORITYR55, DATA_GICD_IPRIORITYR55, dump_gicd_ipriorityr55, "GICD_IPRIORITYR55", 0},
	{VA_GICD_ITARGETSR0, DATA_GICD_ITARGETSR0, dump_gicd_itargetsr0, "GICD_ITARGETSR0", 0},
	{VA_GICD_ITARGETSR1, DATA_GICD_ITARGETSR1, dump_gicd_itargetsr1, "GICD_ITARGETSR1", 0},
	{VA_GICD_ITARGETSR2, DATA_GICD_ITARGETSR2, dump_gicd_itargetsr2, "GICD_ITARGETSR2", 0},
	{VA_GICD_ITARGETSR3, DATA_GICD_ITARGETSR3, dump_gicd_itargetsr3, "GICD_ITARGETSR3", 0},
	{VA_GICD_ITARGETSR4, DATA_GICD_ITARGETSR4, dump_gicd_itargetsr4, "GICD_ITARGETSR4", 0},
	{VA_GICD_ITARGETSR5, DATA_GICD_ITARGETSR5, dump_gicd_itargetsr5, "GICD_ITARGETSR5", 0},
	{VA_GICD_ITARGETSR6, DATA_GICD_ITARGETSR6, dump_gicd_itargetsr6, "GICD_ITARGETSR6", 0},
	{VA_GICD_ITARGETSR7, DATA_GICD_ITARGETSR7, dump_gicd_itargetsr7, "GICD_ITARGETSR7", 0},
	{VA_GICD_ITARGETSR8, DATA_GICD_ITARGETSR8, dump_gicd_itargetsr8, "GICD_ITARGETSR8", 0},
	{VA_GICD_ITARGETSR9, DATA_GICD_ITARGETSR9, dump_gicd_itargetsr9, "GICD_ITARGETSR9", 0},
	{VA_GICD_ITARGETSR10, DATA_GICD_ITARGETSR10, dump_gicd_itargetsr10, "GICD_ITARGETSR10", 0},
	{VA_GICD_ITARGETSR11, DATA_GICD_ITARGETSR11, dump_gicd_itargetsr11, "GICD_ITARGETSR11", 0},
	{VA_GICD_ITARGETSR12, DATA_GICD_ITARGETSR12, dump_gicd_itargetsr12, "GICD_ITARGETSR12", 0},
	{VA_GICD_ITARGETSR13, DATA_GICD_ITARGETSR13, dump_gicd_itargetsr13, "GICD_ITARGETSR13", 0},
	{VA_GICD_ITARGETSR14, DATA_GICD_ITARGETSR14, dump_gicd_itargetsr14, "GICD_ITARGETSR14", 0},
	{VA_GICD_ITARGETSR15, DATA_GICD_ITARGETSR15, dump_gicd_itargetsr15, "GICD_ITARGETSR15", 0},
	{VA_GICD_ITARGETSR16, DATA_GICD_ITARGETSR16, dump_gicd_itargetsr16, "GICD_ITARGETSR16", 0},
	{VA_GICD_ITARGETSR17, DATA_GICD_ITARGETSR17, dump_gicd_itargetsr17, "GICD_ITARGETSR17", 0},
	{VA_GICD_ITARGETSR18, DATA_GICD_ITARGETSR18, dump_gicd_itargetsr18, "GICD_ITARGETSR18", 0},
	{VA_GICD_ITARGETSR19, DATA_GICD_ITARGETSR19, dump_gicd_itargetsr19, "GICD_ITARGETSR19", 0},
	{VA_GICD_ITARGETSR20, DATA_GICD_ITARGETSR20, dump_gicd_itargetsr20, "GICD_ITARGETSR20", 0},
	{VA_GICD_ITARGETSR21, DATA_GICD_ITARGETSR21, dump_gicd_itargetsr21, "GICD_ITARGETSR21", 0},
	{VA_GICD_ITARGETSR22, DATA_GICD_ITARGETSR22, dump_gicd_itargetsr22, "GICD_ITARGETSR22", 0},
	{VA_GICD_ITARGETSR23, DATA_GICD_ITARGETSR23, dump_gicd_itargetsr23, "GICD_ITARGETSR23", 0},
	{VA_GICD_ITARGETSR24, DATA_GICD_ITARGETSR24, dump_gicd_itargetsr24, "GICD_ITARGETSR24", 0},
	{VA_GICD_ITARGETSR25, DATA_GICD_ITARGETSR25, dump_gicd_itargetsr25, "GICD_ITARGETSR25", 0},
	{VA_GICD_ITARGETSR26, DATA_GICD_ITARGETSR26, dump_gicd_itargetsr26, "GICD_ITARGETSR26", 0},
	{VA_GICD_ITARGETSR27, DATA_GICD_ITARGETSR27, dump_gicd_itargetsr27, "GICD_ITARGETSR27", 0},
	{VA_GICD_ITARGETSR28, DATA_GICD_ITARGETSR28, dump_gicd_itargetsr28, "GICD_ITARGETSR28", 0},
	{VA_GICD_ITARGETSR29, DATA_GICD_ITARGETSR29, dump_gicd_itargetsr29, "GICD_ITARGETSR29", 0},
	{VA_GICD_ITARGETSR30, DATA_GICD_ITARGETSR30, dump_gicd_itargetsr30, "GICD_ITARGETSR30", 0},
	{VA_GICD_ITARGETSR31, DATA_GICD_ITARGETSR31, dump_gicd_itargetsr31, "GICD_ITARGETSR31", 0},
	{VA_GICD_ITARGETSR32, DATA_GICD_ITARGETSR32, dump_gicd_itargetsr32, "GICD_ITARGETSR32", 0},
	{VA_GICD_ITARGETSR33, DATA_GICD_ITARGETSR33, dump_gicd_itargetsr33, "GICD_ITARGETSR33", 0},
	{VA_GICD_ITARGETSR34, DATA_GICD_ITARGETSR34, dump_gicd_itargetsr34, "GICD_ITARGETSR34", 0},
	{VA_GICD_ITARGETSR35, DATA_GICD_ITARGETSR35, dump_gicd_itargetsr35, "GICD_ITARGETSR35", 0},
	{VA_GICD_ITARGETSR36, DATA_GICD_ITARGETSR36, dump_gicd_itargetsr36, "GICD_ITARGETSR36", 0},
	{VA_GICD_ITARGETSR37, DATA_GICD_ITARGETSR37, dump_gicd_itargetsr37, "GICD_ITARGETSR37", 0},
	{VA_GICD_ITARGETSR38, DATA_GICD_ITARGETSR38, dump_gicd_itargetsr38, "GICD_ITARGETSR38", 0},
	{VA_GICD_ITARGETSR39, DATA_GICD_ITARGETSR39, dump_gicd_itargetsr39, "GICD_ITARGETSR39", 0},
	{VA_GICD_ITARGETSR40, DATA_GICD_ITARGETSR40, dump_gicd_itargetsr40, "GICD_ITARGETSR40", 0},
	{VA_GICD_ITARGETSR41, DATA_GICD_ITARGETSR41, dump_gicd_itargetsr41, "GICD_ITARGETSR41", 0},
	{VA_GICD_ITARGETSR42, DATA_GICD_ITARGETSR42, dump_gicd_itargetsr42, "GICD_ITARGETSR42", 0},
	{VA_GICD_ITARGETSR43, DATA_GICD_ITARGETSR43, dump_gicd_itargetsr43, "GICD_ITARGETSR43", 0},
	{VA_GICD_ITARGETSR44, DATA_GICD_ITARGETSR44, dump_gicd_itargetsr44, "GICD_ITARGETSR44", 0},
	{VA_GICD_ITARGETSR45, DATA_GICD_ITARGETSR45, dump_gicd_itargetsr45, "GICD_ITARGETSR45", 0},
	{VA_GICD_ITARGETSR46, DATA_GICD_ITARGETSR46, dump_gicd_itargetsr46, "GICD_ITARGETSR46", 0},
	{VA_GICD_ITARGETSR47, DATA_GICD_ITARGETSR47, dump_gicd_itargetsr47, "GICD_ITARGETSR47", 0},
	{VA_GICD_ITARGETSR48, DATA_GICD_ITARGETSR48, dump_gicd_itargetsr48, "GICD_ITARGETSR48", 0},
	{VA_GICD_ITARGETSR49, DATA_GICD_ITARGETSR49, dump_gicd_itargetsr49, "GICD_ITARGETSR49", 0},
	{VA_GICD_ITARGETSR50, DATA_GICD_ITARGETSR50, dump_gicd_itargetsr50, "GICD_ITARGETSR50", 0},
	{VA_GICD_ITARGETSR51, DATA_GICD_ITARGETSR51, dump_gicd_itargetsr51, "GICD_ITARGETSR51", 0},
	{VA_GICD_ITARGETSR52, DATA_GICD_ITARGETSR52, dump_gicd_itargetsr52, "GICD_ITARGETSR52", 0},
	{VA_GICD_ITARGETSR53, DATA_GICD_ITARGETSR53, dump_gicd_itargetsr53, "GICD_ITARGETSR53", 0},
	{VA_GICD_ITARGETSR54, DATA_GICD_ITARGETSR54, dump_gicd_itargetsr54, "GICD_ITARGETSR54", 0},
	{VA_GICD_ITARGETSR55, DATA_GICD_ITARGETSR55, dump_gicd_itargetsr55, "GICD_ITARGETSR55", 0},
	{VA_GICD_ICFGR0, DATA_GICD_ICFGR0, dump_gicd_icfgr0, "GICD_ICFGR0", 0},
	{VA_GICD_ICFGR1, DATA_GICD_ICFGR1, dump_gicd_icfgr1, "GICD_ICFGR1", 0},
	{VA_GICD_ICFGR2, DATA_GICD_ICFGR2, dump_gicd_icfgr2, "GICD_ICFGR2", 0},
	{VA_GICD_ICFGR3, DATA_GICD_ICFGR3, dump_gicd_icfgr3, "GICD_ICFGR3", 0},
	{VA_GICD_ICFGR4, DATA_GICD_ICFGR4, dump_gicd_icfgr4, "GICD_ICFGR4", 0},
	{VA_GICD_ICFGR5, DATA_GICD_ICFGR5, dump_gicd_icfgr5, "GICD_ICFGR5", 0},
	{VA_GICD_ICFGR6, DATA_GICD_ICFGR6, dump_gicd_icfgr6, "GICD_ICFGR6", 0},
	{VA_GICD_ICFGR7, DATA_GICD_ICFGR7, dump_gicd_icfgr7, "GICD_ICFGR7", 0},
	{VA_GICD_ICFGR8, DATA_GICD_ICFGR8, dump_gicd_icfgr8, "GICD_ICFGR8", 0},
	{VA_GICD_ICFGR9, DATA_GICD_ICFGR9, dump_gicd_icfgr9, "GICD_ICFGR9", 0},
	{VA_GICD_ICFGR10, DATA_GICD_ICFGR10, dump_gicd_icfgr10, "GICD_ICFGR10", 0},
	{VA_GICD_ICFGR11, DATA_GICD_ICFGR11, dump_gicd_icfgr11, "GICD_ICFGR11", 0},
	{VA_GICD_ICFGR12, DATA_GICD_ICFGR12, dump_gicd_icfgr12, "GICD_ICFGR12", 0},
	{VA_GICD_ICFGR13, DATA_GICD_ICFGR13, dump_gicd_icfgr13, "GICD_ICFGR13", 0},
	{VA_GICD_SGIR, DATA_GICD_SGIR, dump_gicd_sgir, "GICD_SGIR", 0},
	{VA_GICD_CPENDSGIR0, DATA_GICD_CPENDSGIR0, dump_gicd_cpendsgir0, "GICD_CPENDSGIR0", 0},
	{VA_GICD_CPENDSGIR1, DATA_GICD_CPENDSGIR1, dump_gicd_cpendsgir1, "GICD_CPENDSGIR1", 0},
	{VA_GICD_CPENDSGIR2, DATA_GICD_CPENDSGIR2, dump_gicd_cpendsgir2, "GICD_CPENDSGIR2", 0},
	{VA_GICD_CPENDSGIR3, DATA_GICD_CPENDSGIR3, dump_gicd_cpendsgir3, "GICD_CPENDSGIR3", 0},
	{VA_GICD_SPENDSGIR0, DATA_GICD_SPENDSGIR0, dump_gicd_spendsgir0, "GICD_SPENDSGIR0", 0},
	{VA_GICD_SPENDSGIR1, DATA_GICD_SPENDSGIR1, dump_gicd_spendsgir1, "GICD_SPENDSGIR1", 0},
	{VA_GICD_SPENDSGIR2, DATA_GICD_SPENDSGIR2, dump_gicd_spendsgir2, "GICD_SPENDSGIR2", 0},
	{VA_GICD_SPENDSGIR3, DATA_GICD_SPENDSGIR3, dump_gicd_spendsgir3, "GICD_SPENDSGIR3", 0},
};

void dump_gicd(u32 mode)
{
	u32 cnt = sizeof(gicd_info) / sizeof(gicd_info[0]);
	u32 i = 0;
	char sz_temp[TMP_STR_LEN];
	char buffer[DUMP_BUF_LEN];

	def_memset(buffer, 0, DUMP_BUF_LEN);

	for (i = 0; i < cnt; i++) {
		def_memset(buffer, 0, DUMP_BUF_LEN);
		def_sprintf(sz_temp, "[%d]%s:[%08X]%08X\n",
					i,
					gicd_info[i].name,
					gicd_info[i].addr,
					gicd_info[i].reset);
		def_strcat(buffer, sz_temp);

		gicd_info[i].dump(gicd_info[i].addr, gicd_info[i].reset, mode, buffer);
		msg("%s", buffer);
	}
}

u32 dump_gicd_ctlr(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ctlr_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:enablegrp0\n", reg.bits.enablegrp0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:enablegrp1\n", reg.bits.enablegrp1);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_typer(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_typer_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:itlinesnumber\n", reg.bits.itlinesnumber);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cpunumber\n", reg.bits.cpunumber);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:securityextn\n", reg.bits.securityextn);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:lspi\n", reg.bits.lspi);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_iidr(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_iidr_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:implementor\n", reg.bits.implementor);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:revision\n", reg.bits.revision);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:variant\n", reg.bits.variant);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:productid\n", reg.bits.productid);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_igroupr0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_igroupr0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:grp_status_id0\n", reg.bits.grp_status_id0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id1\n", reg.bits.grp_status_id1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id2\n", reg.bits.grp_status_id2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id3\n", reg.bits.grp_status_id3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id4\n", reg.bits.grp_status_id4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id5\n", reg.bits.grp_status_id5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id6\n", reg.bits.grp_status_id6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id7\n", reg.bits.grp_status_id7);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id8\n", reg.bits.grp_status_id8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id9\n", reg.bits.grp_status_id9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id10\n", reg.bits.grp_status_id10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id11\n", reg.bits.grp_status_id11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id12\n", reg.bits.grp_status_id12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id13\n", reg.bits.grp_status_id13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id14\n", reg.bits.grp_status_id14);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id15\n", reg.bits.grp_status_id15);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id16\n", reg.bits.grp_status_id16);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id17\n", reg.bits.grp_status_id17);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id18\n", reg.bits.grp_status_id18);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id19\n", reg.bits.grp_status_id19);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id20\n", reg.bits.grp_status_id20);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id21\n", reg.bits.grp_status_id21);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id22\n", reg.bits.grp_status_id22);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id23\n", reg.bits.grp_status_id23);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id24\n", reg.bits.grp_status_id24);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id25\n", reg.bits.grp_status_id25);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id26\n", reg.bits.grp_status_id26);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id27\n", reg.bits.grp_status_id27);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id28\n", reg.bits.grp_status_id28);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id29\n", reg.bits.grp_status_id29);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id30\n", reg.bits.grp_status_id30);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id31\n", reg.bits.grp_status_id31);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_igroupr1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_igroupr1_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:grp_status_id32\n", reg.bits.grp_status_id32);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id33\n", reg.bits.grp_status_id33);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id34\n", reg.bits.grp_status_id34);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id35\n", reg.bits.grp_status_id35);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id36\n", reg.bits.grp_status_id36);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id37\n", reg.bits.grp_status_id37);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id38\n", reg.bits.grp_status_id38);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id39\n", reg.bits.grp_status_id39);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id40\n", reg.bits.grp_status_id40);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id41\n", reg.bits.grp_status_id41);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id42\n", reg.bits.grp_status_id42);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id43\n", reg.bits.grp_status_id43);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id44\n", reg.bits.grp_status_id44);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id45\n", reg.bits.grp_status_id45);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id46\n", reg.bits.grp_status_id46);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id47\n", reg.bits.grp_status_id47);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id48\n", reg.bits.grp_status_id48);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id49\n", reg.bits.grp_status_id49);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id50\n", reg.bits.grp_status_id50);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id51\n", reg.bits.grp_status_id51);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id52\n", reg.bits.grp_status_id52);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id53\n", reg.bits.grp_status_id53);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id54\n", reg.bits.grp_status_id54);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id55\n", reg.bits.grp_status_id55);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id56\n", reg.bits.grp_status_id56);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id57\n", reg.bits.grp_status_id57);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id58\n", reg.bits.grp_status_id58);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id59\n", reg.bits.grp_status_id59);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id60\n", reg.bits.grp_status_id60);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id61\n", reg.bits.grp_status_id61);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id62\n", reg.bits.grp_status_id62);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id63\n", reg.bits.grp_status_id63);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_igroupr2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_igroupr2_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:grp_status_id64\n", reg.bits.grp_status_id64);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id65\n", reg.bits.grp_status_id65);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id66\n", reg.bits.grp_status_id66);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id67\n", reg.bits.grp_status_id67);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id68\n", reg.bits.grp_status_id68);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id69\n", reg.bits.grp_status_id69);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id70\n", reg.bits.grp_status_id70);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id71\n", reg.bits.grp_status_id71);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id72\n", reg.bits.grp_status_id72);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id73\n", reg.bits.grp_status_id73);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id74\n", reg.bits.grp_status_id74);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id75\n", reg.bits.grp_status_id75);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id76\n", reg.bits.grp_status_id76);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id77\n", reg.bits.grp_status_id77);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id78\n", reg.bits.grp_status_id78);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id79\n", reg.bits.grp_status_id79);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id80\n", reg.bits.grp_status_id80);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id81\n", reg.bits.grp_status_id81);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id82\n", reg.bits.grp_status_id82);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id83\n", reg.bits.grp_status_id83);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id84\n", reg.bits.grp_status_id84);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id85\n", reg.bits.grp_status_id85);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id86\n", reg.bits.grp_status_id86);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id87\n", reg.bits.grp_status_id87);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id88\n", reg.bits.grp_status_id88);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id89\n", reg.bits.grp_status_id89);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id90\n", reg.bits.grp_status_id90);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id91\n", reg.bits.grp_status_id91);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id92\n", reg.bits.grp_status_id92);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id93\n", reg.bits.grp_status_id93);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id94\n", reg.bits.grp_status_id94);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id95\n", reg.bits.grp_status_id95);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_igroupr3(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_igroupr3_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:grp_status_id96\n", reg.bits.grp_status_id96);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id97\n", reg.bits.grp_status_id97);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id98\n", reg.bits.grp_status_id98);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id99\n", reg.bits.grp_status_id99);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id100\n", reg.bits.grp_status_id100);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id101\n", reg.bits.grp_status_id101);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id102\n", reg.bits.grp_status_id102);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id103\n", reg.bits.grp_status_id103);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id104\n", reg.bits.grp_status_id104);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id105\n", reg.bits.grp_status_id105);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id106\n", reg.bits.grp_status_id106);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id107\n", reg.bits.grp_status_id107);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id108\n", reg.bits.grp_status_id108);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id109\n", reg.bits.grp_status_id109);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id110\n", reg.bits.grp_status_id110);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id111\n", reg.bits.grp_status_id111);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id112\n", reg.bits.grp_status_id112);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id113\n", reg.bits.grp_status_id113);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id114\n", reg.bits.grp_status_id114);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id115\n", reg.bits.grp_status_id115);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id116\n", reg.bits.grp_status_id116);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id117\n", reg.bits.grp_status_id117);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id118\n", reg.bits.grp_status_id118);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id119\n", reg.bits.grp_status_id119);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id120\n", reg.bits.grp_status_id120);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id121\n", reg.bits.grp_status_id121);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id122\n", reg.bits.grp_status_id122);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id123\n", reg.bits.grp_status_id123);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id124\n", reg.bits.grp_status_id124);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id125\n", reg.bits.grp_status_id125);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id126\n", reg.bits.grp_status_id126);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id127\n", reg.bits.grp_status_id127);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_igroupr4(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_igroupr4_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:grp_status_id128\n", reg.bits.grp_status_id128);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id129\n", reg.bits.grp_status_id129);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id130\n", reg.bits.grp_status_id130);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id131\n", reg.bits.grp_status_id131);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id132\n", reg.bits.grp_status_id132);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id133\n", reg.bits.grp_status_id133);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id134\n", reg.bits.grp_status_id134);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id135\n", reg.bits.grp_status_id135);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id136\n", reg.bits.grp_status_id136);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id137\n", reg.bits.grp_status_id137);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id138\n", reg.bits.grp_status_id138);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id139\n", reg.bits.grp_status_id139);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id140\n", reg.bits.grp_status_id140);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id141\n", reg.bits.grp_status_id141);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id142\n", reg.bits.grp_status_id142);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id143\n", reg.bits.grp_status_id143);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id144\n", reg.bits.grp_status_id144);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id145\n", reg.bits.grp_status_id145);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id146\n", reg.bits.grp_status_id146);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id147\n", reg.bits.grp_status_id147);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id148\n", reg.bits.grp_status_id148);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id149\n", reg.bits.grp_status_id149);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id150\n", reg.bits.grp_status_id150);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id151\n", reg.bits.grp_status_id151);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id152\n", reg.bits.grp_status_id152);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id153\n", reg.bits.grp_status_id153);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id154\n", reg.bits.grp_status_id154);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id155\n", reg.bits.grp_status_id155);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id156\n", reg.bits.grp_status_id156);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id157\n", reg.bits.grp_status_id157);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id158\n", reg.bits.grp_status_id158);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id159\n", reg.bits.grp_status_id159);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_igroupr5(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_igroupr5_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:grp_status_id160\n", reg.bits.grp_status_id160);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id161\n", reg.bits.grp_status_id161);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id162\n", reg.bits.grp_status_id162);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id163\n", reg.bits.grp_status_id163);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id164\n", reg.bits.grp_status_id164);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id165\n", reg.bits.grp_status_id165);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id166\n", reg.bits.grp_status_id166);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id167\n", reg.bits.grp_status_id167);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id168\n", reg.bits.grp_status_id168);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id169\n", reg.bits.grp_status_id169);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id170\n", reg.bits.grp_status_id170);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id171\n", reg.bits.grp_status_id171);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id172\n", reg.bits.grp_status_id172);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id173\n", reg.bits.grp_status_id173);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id174\n", reg.bits.grp_status_id174);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id175\n", reg.bits.grp_status_id175);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id176\n", reg.bits.grp_status_id176);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id177\n", reg.bits.grp_status_id177);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id178\n", reg.bits.grp_status_id178);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id179\n", reg.bits.grp_status_id179);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id180\n", reg.bits.grp_status_id180);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id181\n", reg.bits.grp_status_id181);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id182\n", reg.bits.grp_status_id182);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id183\n", reg.bits.grp_status_id183);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id184\n", reg.bits.grp_status_id184);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id185\n", reg.bits.grp_status_id185);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id186\n", reg.bits.grp_status_id186);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id187\n", reg.bits.grp_status_id187);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id188\n", reg.bits.grp_status_id188);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id189\n", reg.bits.grp_status_id189);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id190\n", reg.bits.grp_status_id190);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id191\n", reg.bits.grp_status_id191);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_igroupr6(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_igroupr6_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:grp_status_id192\n", reg.bits.grp_status_id192);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id193\n", reg.bits.grp_status_id193);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id194\n", reg.bits.grp_status_id194);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id195\n", reg.bits.grp_status_id195);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id196\n", reg.bits.grp_status_id196);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id197\n", reg.bits.grp_status_id197);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id198\n", reg.bits.grp_status_id198);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id199\n", reg.bits.grp_status_id199);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id200\n", reg.bits.grp_status_id200);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id201\n", reg.bits.grp_status_id201);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id202\n", reg.bits.grp_status_id202);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id203\n", reg.bits.grp_status_id203);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id204\n", reg.bits.grp_status_id204);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id205\n", reg.bits.grp_status_id205);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id206\n", reg.bits.grp_status_id206);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id207\n", reg.bits.grp_status_id207);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id208\n", reg.bits.grp_status_id208);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id209\n", reg.bits.grp_status_id209);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id210\n", reg.bits.grp_status_id210);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id211\n", reg.bits.grp_status_id211);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id212\n", reg.bits.grp_status_id212);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id213\n", reg.bits.grp_status_id213);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id214\n", reg.bits.grp_status_id214);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id215\n", reg.bits.grp_status_id215);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id216\n", reg.bits.grp_status_id216);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id217\n", reg.bits.grp_status_id217);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id218\n", reg.bits.grp_status_id218);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id219\n", reg.bits.grp_status_id219);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id220\n", reg.bits.grp_status_id220);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id221\n", reg.bits.grp_status_id221);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id222\n", reg.bits.grp_status_id222);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:grp_status_id223\n", reg.bits.grp_status_id223);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_isenabler0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_isenabler0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_en_id0\n", reg.bits.set_en_id0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id1\n", reg.bits.set_en_id1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id2\n", reg.bits.set_en_id2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id3\n", reg.bits.set_en_id3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id4\n", reg.bits.set_en_id4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id5\n", reg.bits.set_en_id5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id6\n", reg.bits.set_en_id6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id7\n", reg.bits.set_en_id7);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id8\n", reg.bits.set_en_id8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id9\n", reg.bits.set_en_id9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id10\n", reg.bits.set_en_id10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id11\n", reg.bits.set_en_id11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id12\n", reg.bits.set_en_id12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id13\n", reg.bits.set_en_id13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id14\n", reg.bits.set_en_id14);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id15\n", reg.bits.set_en_id15);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id16\n", reg.bits.set_en_id16);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id17\n", reg.bits.set_en_id17);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id18\n", reg.bits.set_en_id18);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id19\n", reg.bits.set_en_id19);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id20\n", reg.bits.set_en_id20);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id21\n", reg.bits.set_en_id21);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id22\n", reg.bits.set_en_id22);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id23\n", reg.bits.set_en_id23);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id24\n", reg.bits.set_en_id24);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id25\n", reg.bits.set_en_id25);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id26\n", reg.bits.set_en_id26);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id27\n", reg.bits.set_en_id27);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id28\n", reg.bits.set_en_id28);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id29\n", reg.bits.set_en_id29);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id30\n", reg.bits.set_en_id30);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id31\n", reg.bits.set_en_id31);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_isenabler1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_isenabler1_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_en_id32\n", reg.bits.set_en_id32);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id33\n", reg.bits.set_en_id33);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id34\n", reg.bits.set_en_id34);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id35\n", reg.bits.set_en_id35);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id36\n", reg.bits.set_en_id36);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id37\n", reg.bits.set_en_id37);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id38\n", reg.bits.set_en_id38);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id39\n", reg.bits.set_en_id39);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id40\n", reg.bits.set_en_id40);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id41\n", reg.bits.set_en_id41);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id42\n", reg.bits.set_en_id42);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id43\n", reg.bits.set_en_id43);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id44\n", reg.bits.set_en_id44);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id45\n", reg.bits.set_en_id45);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id46\n", reg.bits.set_en_id46);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id47\n", reg.bits.set_en_id47);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id48\n", reg.bits.set_en_id48);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id49\n", reg.bits.set_en_id49);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id50\n", reg.bits.set_en_id50);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id51\n", reg.bits.set_en_id51);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id52\n", reg.bits.set_en_id52);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id53\n", reg.bits.set_en_id53);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id54\n", reg.bits.set_en_id54);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id55\n", reg.bits.set_en_id55);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id56\n", reg.bits.set_en_id56);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id57\n", reg.bits.set_en_id57);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id58\n", reg.bits.set_en_id58);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id59\n", reg.bits.set_en_id59);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id60\n", reg.bits.set_en_id60);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id61\n", reg.bits.set_en_id61);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id62\n", reg.bits.set_en_id62);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id63\n", reg.bits.set_en_id63);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_isenabler2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_isenabler2_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_en_id64\n", reg.bits.set_en_id64);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id65\n", reg.bits.set_en_id65);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id66\n", reg.bits.set_en_id66);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id67\n", reg.bits.set_en_id67);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id68\n", reg.bits.set_en_id68);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id69\n", reg.bits.set_en_id69);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id70\n", reg.bits.set_en_id70);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id71\n", reg.bits.set_en_id71);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id72\n", reg.bits.set_en_id72);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id73\n", reg.bits.set_en_id73);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id74\n", reg.bits.set_en_id74);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id75\n", reg.bits.set_en_id75);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id76\n", reg.bits.set_en_id76);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id77\n", reg.bits.set_en_id77);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id78\n", reg.bits.set_en_id78);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id79\n", reg.bits.set_en_id79);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id80\n", reg.bits.set_en_id80);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id81\n", reg.bits.set_en_id81);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id82\n", reg.bits.set_en_id82);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id83\n", reg.bits.set_en_id83);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id84\n", reg.bits.set_en_id84);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id85\n", reg.bits.set_en_id85);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id86\n", reg.bits.set_en_id86);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id87\n", reg.bits.set_en_id87);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id88\n", reg.bits.set_en_id88);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id89\n", reg.bits.set_en_id89);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id90\n", reg.bits.set_en_id90);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id91\n", reg.bits.set_en_id91);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id92\n", reg.bits.set_en_id92);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id93\n", reg.bits.set_en_id93);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id94\n", reg.bits.set_en_id94);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id95\n", reg.bits.set_en_id95);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_isenabler3(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_isenabler3_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_en_id96\n", reg.bits.set_en_id96);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id97\n", reg.bits.set_en_id97);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id98\n", reg.bits.set_en_id98);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id99\n", reg.bits.set_en_id99);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id100\n", reg.bits.set_en_id100);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id101\n", reg.bits.set_en_id101);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id102\n", reg.bits.set_en_id102);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id103\n", reg.bits.set_en_id103);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id104\n", reg.bits.set_en_id104);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id105\n", reg.bits.set_en_id105);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id106\n", reg.bits.set_en_id106);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id107\n", reg.bits.set_en_id107);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id108\n", reg.bits.set_en_id108);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id109\n", reg.bits.set_en_id109);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id110\n", reg.bits.set_en_id110);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id111\n", reg.bits.set_en_id111);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id112\n", reg.bits.set_en_id112);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id113\n", reg.bits.set_en_id113);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id114\n", reg.bits.set_en_id114);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id115\n", reg.bits.set_en_id115);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id116\n", reg.bits.set_en_id116);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id117\n", reg.bits.set_en_id117);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id118\n", reg.bits.set_en_id118);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id119\n", reg.bits.set_en_id119);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id120\n", reg.bits.set_en_id120);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id121\n", reg.bits.set_en_id121);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id122\n", reg.bits.set_en_id122);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id123\n", reg.bits.set_en_id123);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id124\n", reg.bits.set_en_id124);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id125\n", reg.bits.set_en_id125);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id126\n", reg.bits.set_en_id126);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id127\n", reg.bits.set_en_id127);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_isenabler4(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_isenabler4_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_en_id128\n", reg.bits.set_en_id128);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id129\n", reg.bits.set_en_id129);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id130\n", reg.bits.set_en_id130);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id131\n", reg.bits.set_en_id131);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id132\n", reg.bits.set_en_id132);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id133\n", reg.bits.set_en_id133);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id134\n", reg.bits.set_en_id134);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id135\n", reg.bits.set_en_id135);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id136\n", reg.bits.set_en_id136);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id137\n", reg.bits.set_en_id137);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id138\n", reg.bits.set_en_id138);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id139\n", reg.bits.set_en_id139);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id140\n", reg.bits.set_en_id140);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id141\n", reg.bits.set_en_id141);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id142\n", reg.bits.set_en_id142);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id143\n", reg.bits.set_en_id143);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id144\n", reg.bits.set_en_id144);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id145\n", reg.bits.set_en_id145);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id146\n", reg.bits.set_en_id146);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id147\n", reg.bits.set_en_id147);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id148\n", reg.bits.set_en_id148);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id149\n", reg.bits.set_en_id149);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id150\n", reg.bits.set_en_id150);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id151\n", reg.bits.set_en_id151);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id152\n", reg.bits.set_en_id152);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id153\n", reg.bits.set_en_id153);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id154\n", reg.bits.set_en_id154);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id155\n", reg.bits.set_en_id155);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id156\n", reg.bits.set_en_id156);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id157\n", reg.bits.set_en_id157);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id158\n", reg.bits.set_en_id158);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id159\n", reg.bits.set_en_id159);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_isenabler5(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_isenabler5_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_en_id160\n", reg.bits.set_en_id160);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id161\n", reg.bits.set_en_id161);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id162\n", reg.bits.set_en_id162);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id163\n", reg.bits.set_en_id163);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id164\n", reg.bits.set_en_id164);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id165\n", reg.bits.set_en_id165);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id166\n", reg.bits.set_en_id166);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id167\n", reg.bits.set_en_id167);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id168\n", reg.bits.set_en_id168);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id169\n", reg.bits.set_en_id169);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id170\n", reg.bits.set_en_id170);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id171\n", reg.bits.set_en_id171);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id172\n", reg.bits.set_en_id172);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id173\n", reg.bits.set_en_id173);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id174\n", reg.bits.set_en_id174);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id175\n", reg.bits.set_en_id175);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id176\n", reg.bits.set_en_id176);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id177\n", reg.bits.set_en_id177);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id178\n", reg.bits.set_en_id178);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id179\n", reg.bits.set_en_id179);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id180\n", reg.bits.set_en_id180);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id181\n", reg.bits.set_en_id181);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id182\n", reg.bits.set_en_id182);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id183\n", reg.bits.set_en_id183);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id184\n", reg.bits.set_en_id184);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id185\n", reg.bits.set_en_id185);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id186\n", reg.bits.set_en_id186);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id187\n", reg.bits.set_en_id187);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id188\n", reg.bits.set_en_id188);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id189\n", reg.bits.set_en_id189);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id190\n", reg.bits.set_en_id190);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id191\n", reg.bits.set_en_id191);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_isenabler6(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_isenabler6_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_en_id192\n", reg.bits.set_en_id192);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id193\n", reg.bits.set_en_id193);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id194\n", reg.bits.set_en_id194);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id195\n", reg.bits.set_en_id195);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id196\n", reg.bits.set_en_id196);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id197\n", reg.bits.set_en_id197);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id198\n", reg.bits.set_en_id198);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id199\n", reg.bits.set_en_id199);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id200\n", reg.bits.set_en_id200);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id201\n", reg.bits.set_en_id201);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id202\n", reg.bits.set_en_id202);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id203\n", reg.bits.set_en_id203);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id204\n", reg.bits.set_en_id204);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id205\n", reg.bits.set_en_id205);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id206\n", reg.bits.set_en_id206);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id207\n", reg.bits.set_en_id207);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id208\n", reg.bits.set_en_id208);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id209\n", reg.bits.set_en_id209);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id210\n", reg.bits.set_en_id210);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id211\n", reg.bits.set_en_id211);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id212\n", reg.bits.set_en_id212);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id213\n", reg.bits.set_en_id213);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id214\n", reg.bits.set_en_id214);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id215\n", reg.bits.set_en_id215);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id216\n", reg.bits.set_en_id216);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id217\n", reg.bits.set_en_id217);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id218\n", reg.bits.set_en_id218);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id219\n", reg.bits.set_en_id219);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id220\n", reg.bits.set_en_id220);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id221\n", reg.bits.set_en_id221);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id222\n", reg.bits.set_en_id222);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_en_id223\n", reg.bits.set_en_id223);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icenabler0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icenabler0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clr_en_id0\n", reg.bits.clr_en_id0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id1\n", reg.bits.clr_en_id1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id2\n", reg.bits.clr_en_id2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id3\n", reg.bits.clr_en_id3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id4\n", reg.bits.clr_en_id4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id5\n", reg.bits.clr_en_id5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id6\n", reg.bits.clr_en_id6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id7\n", reg.bits.clr_en_id7);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id8\n", reg.bits.clr_en_id8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id9\n", reg.bits.clr_en_id9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id10\n", reg.bits.clr_en_id10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id11\n", reg.bits.clr_en_id11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id12\n", reg.bits.clr_en_id12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id13\n", reg.bits.clr_en_id13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id14\n", reg.bits.clr_en_id14);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id15\n", reg.bits.clr_en_id15);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id16\n", reg.bits.clr_en_id16);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id17\n", reg.bits.clr_en_id17);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id18\n", reg.bits.clr_en_id18);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id19\n", reg.bits.clr_en_id19);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id20\n", reg.bits.clr_en_id20);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id21\n", reg.bits.clr_en_id21);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id22\n", reg.bits.clr_en_id22);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id23\n", reg.bits.clr_en_id23);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id24\n", reg.bits.clr_en_id24);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id25\n", reg.bits.clr_en_id25);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id26\n", reg.bits.clr_en_id26);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id27\n", reg.bits.clr_en_id27);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id28\n", reg.bits.clr_en_id28);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id29\n", reg.bits.clr_en_id29);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id30\n", reg.bits.clr_en_id30);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id31\n", reg.bits.clr_en_id31);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icenabler1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icenabler1_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clr_en_id32\n", reg.bits.clr_en_id32);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id33\n", reg.bits.clr_en_id33);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id34\n", reg.bits.clr_en_id34);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id35\n", reg.bits.clr_en_id35);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id36\n", reg.bits.clr_en_id36);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id37\n", reg.bits.clr_en_id37);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id38\n", reg.bits.clr_en_id38);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id39\n", reg.bits.clr_en_id39);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id40\n", reg.bits.clr_en_id40);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id41\n", reg.bits.clr_en_id41);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id42\n", reg.bits.clr_en_id42);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id43\n", reg.bits.clr_en_id43);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id44\n", reg.bits.clr_en_id44);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id45\n", reg.bits.clr_en_id45);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id46\n", reg.bits.clr_en_id46);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id47\n", reg.bits.clr_en_id47);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id48\n", reg.bits.clr_en_id48);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id49\n", reg.bits.clr_en_id49);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id50\n", reg.bits.clr_en_id50);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id51\n", reg.bits.clr_en_id51);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id52\n", reg.bits.clr_en_id52);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id53\n", reg.bits.clr_en_id53);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id54\n", reg.bits.clr_en_id54);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id55\n", reg.bits.clr_en_id55);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id56\n", reg.bits.clr_en_id56);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id57\n", reg.bits.clr_en_id57);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id58\n", reg.bits.clr_en_id58);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id59\n", reg.bits.clr_en_id59);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id60\n", reg.bits.clr_en_id60);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id61\n", reg.bits.clr_en_id61);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id62\n", reg.bits.clr_en_id62);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id63\n", reg.bits.clr_en_id63);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icenabler2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icenabler2_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clr_en_id64\n", reg.bits.clr_en_id64);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id65\n", reg.bits.clr_en_id65);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id66\n", reg.bits.clr_en_id66);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id67\n", reg.bits.clr_en_id67);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id68\n", reg.bits.clr_en_id68);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id69\n", reg.bits.clr_en_id69);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id70\n", reg.bits.clr_en_id70);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id71\n", reg.bits.clr_en_id71);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id72\n", reg.bits.clr_en_id72);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id73\n", reg.bits.clr_en_id73);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id74\n", reg.bits.clr_en_id74);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id75\n", reg.bits.clr_en_id75);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id76\n", reg.bits.clr_en_id76);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id77\n", reg.bits.clr_en_id77);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id78\n", reg.bits.clr_en_id78);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id79\n", reg.bits.clr_en_id79);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id80\n", reg.bits.clr_en_id80);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id81\n", reg.bits.clr_en_id81);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id82\n", reg.bits.clr_en_id82);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id83\n", reg.bits.clr_en_id83);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id84\n", reg.bits.clr_en_id84);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id85\n", reg.bits.clr_en_id85);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id86\n", reg.bits.clr_en_id86);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id87\n", reg.bits.clr_en_id87);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id88\n", reg.bits.clr_en_id88);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id89\n", reg.bits.clr_en_id89);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id90\n", reg.bits.clr_en_id90);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id91\n", reg.bits.clr_en_id91);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id92\n", reg.bits.clr_en_id92);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id93\n", reg.bits.clr_en_id93);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id94\n", reg.bits.clr_en_id94);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id95\n", reg.bits.clr_en_id95);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icenabler3(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icenabler3_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clr_en_id96\n", reg.bits.clr_en_id96);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id97\n", reg.bits.clr_en_id97);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id98\n", reg.bits.clr_en_id98);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id99\n", reg.bits.clr_en_id99);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id100\n", reg.bits.clr_en_id100);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id101\n", reg.bits.clr_en_id101);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id102\n", reg.bits.clr_en_id102);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id103\n", reg.bits.clr_en_id103);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id104\n", reg.bits.clr_en_id104);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id105\n", reg.bits.clr_en_id105);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id106\n", reg.bits.clr_en_id106);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id107\n", reg.bits.clr_en_id107);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id108\n", reg.bits.clr_en_id108);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id109\n", reg.bits.clr_en_id109);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id110\n", reg.bits.clr_en_id110);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id111\n", reg.bits.clr_en_id111);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id112\n", reg.bits.clr_en_id112);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id113\n", reg.bits.clr_en_id113);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id114\n", reg.bits.clr_en_id114);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id115\n", reg.bits.clr_en_id115);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id116\n", reg.bits.clr_en_id116);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id117\n", reg.bits.clr_en_id117);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id118\n", reg.bits.clr_en_id118);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id119\n", reg.bits.clr_en_id119);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id120\n", reg.bits.clr_en_id120);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id121\n", reg.bits.clr_en_id121);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id122\n", reg.bits.clr_en_id122);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id123\n", reg.bits.clr_en_id123);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id124\n", reg.bits.clr_en_id124);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id125\n", reg.bits.clr_en_id125);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id126\n", reg.bits.clr_en_id126);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id127\n", reg.bits.clr_en_id127);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icenabler4(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icenabler4_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clr_en_id128\n", reg.bits.clr_en_id128);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id129\n", reg.bits.clr_en_id129);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id130\n", reg.bits.clr_en_id130);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id131\n", reg.bits.clr_en_id131);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id132\n", reg.bits.clr_en_id132);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id133\n", reg.bits.clr_en_id133);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id134\n", reg.bits.clr_en_id134);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id135\n", reg.bits.clr_en_id135);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id136\n", reg.bits.clr_en_id136);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id137\n", reg.bits.clr_en_id137);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id138\n", reg.bits.clr_en_id138);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id139\n", reg.bits.clr_en_id139);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id140\n", reg.bits.clr_en_id140);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id141\n", reg.bits.clr_en_id141);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id142\n", reg.bits.clr_en_id142);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id143\n", reg.bits.clr_en_id143);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id144\n", reg.bits.clr_en_id144);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id145\n", reg.bits.clr_en_id145);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id146\n", reg.bits.clr_en_id146);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id147\n", reg.bits.clr_en_id147);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id148\n", reg.bits.clr_en_id148);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id149\n", reg.bits.clr_en_id149);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id150\n", reg.bits.clr_en_id150);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id151\n", reg.bits.clr_en_id151);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id152\n", reg.bits.clr_en_id152);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id153\n", reg.bits.clr_en_id153);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id154\n", reg.bits.clr_en_id154);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id155\n", reg.bits.clr_en_id155);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id156\n", reg.bits.clr_en_id156);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id157\n", reg.bits.clr_en_id157);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id158\n", reg.bits.clr_en_id158);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id159\n", reg.bits.clr_en_id159);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icenabler5(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icenabler5_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clr_en_id160\n", reg.bits.clr_en_id160);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id161\n", reg.bits.clr_en_id161);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id162\n", reg.bits.clr_en_id162);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id163\n", reg.bits.clr_en_id163);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id164\n", reg.bits.clr_en_id164);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id165\n", reg.bits.clr_en_id165);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id166\n", reg.bits.clr_en_id166);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id167\n", reg.bits.clr_en_id167);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id168\n", reg.bits.clr_en_id168);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id169\n", reg.bits.clr_en_id169);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id170\n", reg.bits.clr_en_id170);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id171\n", reg.bits.clr_en_id171);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id172\n", reg.bits.clr_en_id172);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id173\n", reg.bits.clr_en_id173);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id174\n", reg.bits.clr_en_id174);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id175\n", reg.bits.clr_en_id175);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id176\n", reg.bits.clr_en_id176);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id177\n", reg.bits.clr_en_id177);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id178\n", reg.bits.clr_en_id178);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id179\n", reg.bits.clr_en_id179);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id180\n", reg.bits.clr_en_id180);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id181\n", reg.bits.clr_en_id181);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id182\n", reg.bits.clr_en_id182);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id183\n", reg.bits.clr_en_id183);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id184\n", reg.bits.clr_en_id184);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id185\n", reg.bits.clr_en_id185);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id186\n", reg.bits.clr_en_id186);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id187\n", reg.bits.clr_en_id187);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id188\n", reg.bits.clr_en_id188);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id189\n", reg.bits.clr_en_id189);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id190\n", reg.bits.clr_en_id190);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id191\n", reg.bits.clr_en_id191);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icenabler6(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icenabler6_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clr_en_id192\n", reg.bits.clr_en_id192);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id193\n", reg.bits.clr_en_id193);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id194\n", reg.bits.clr_en_id194);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id195\n", reg.bits.clr_en_id195);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id196\n", reg.bits.clr_en_id196);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id197\n", reg.bits.clr_en_id197);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id198\n", reg.bits.clr_en_id198);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id199\n", reg.bits.clr_en_id199);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id200\n", reg.bits.clr_en_id200);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id201\n", reg.bits.clr_en_id201);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id202\n", reg.bits.clr_en_id202);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id203\n", reg.bits.clr_en_id203);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id204\n", reg.bits.clr_en_id204);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id205\n", reg.bits.clr_en_id205);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id206\n", reg.bits.clr_en_id206);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id207\n", reg.bits.clr_en_id207);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id208\n", reg.bits.clr_en_id208);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id209\n", reg.bits.clr_en_id209);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id210\n", reg.bits.clr_en_id210);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id211\n", reg.bits.clr_en_id211);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id212\n", reg.bits.clr_en_id212);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id213\n", reg.bits.clr_en_id213);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id214\n", reg.bits.clr_en_id214);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id215\n", reg.bits.clr_en_id215);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id216\n", reg.bits.clr_en_id216);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id217\n", reg.bits.clr_en_id217);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id218\n", reg.bits.clr_en_id218);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id219\n", reg.bits.clr_en_id219);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id220\n", reg.bits.clr_en_id220);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id221\n", reg.bits.clr_en_id221);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id222\n", reg.bits.clr_en_id222);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_en_id223\n", reg.bits.clr_en_id223);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ispendr0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ispendr0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_pend_id0\n", reg.bits.set_pend_id0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id1\n", reg.bits.set_pend_id1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id2\n", reg.bits.set_pend_id2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id3\n", reg.bits.set_pend_id3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id4\n", reg.bits.set_pend_id4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id5\n", reg.bits.set_pend_id5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id6\n", reg.bits.set_pend_id6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id7\n", reg.bits.set_pend_id7);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id8\n", reg.bits.set_pend_id8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id9\n", reg.bits.set_pend_id9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id10\n", reg.bits.set_pend_id10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id11\n", reg.bits.set_pend_id11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id12\n", reg.bits.set_pend_id12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id13\n", reg.bits.set_pend_id13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id14\n", reg.bits.set_pend_id14);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id15\n", reg.bits.set_pend_id15);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id16\n", reg.bits.set_pend_id16);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id17\n", reg.bits.set_pend_id17);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id18\n", reg.bits.set_pend_id18);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id19\n", reg.bits.set_pend_id19);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id20\n", reg.bits.set_pend_id20);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id21\n", reg.bits.set_pend_id21);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id22\n", reg.bits.set_pend_id22);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id23\n", reg.bits.set_pend_id23);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id24\n", reg.bits.set_pend_id24);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id25\n", reg.bits.set_pend_id25);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id26\n", reg.bits.set_pend_id26);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id27\n", reg.bits.set_pend_id27);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id28\n", reg.bits.set_pend_id28);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id29\n", reg.bits.set_pend_id29);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id30\n", reg.bits.set_pend_id30);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id31\n", reg.bits.set_pend_id31);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ispendr1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ispendr1_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_pend_id32\n", reg.bits.set_pend_id32);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id33\n", reg.bits.set_pend_id33);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id34\n", reg.bits.set_pend_id34);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id35\n", reg.bits.set_pend_id35);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id36\n", reg.bits.set_pend_id36);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id37\n", reg.bits.set_pend_id37);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id38\n", reg.bits.set_pend_id38);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id39\n", reg.bits.set_pend_id39);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id40\n", reg.bits.set_pend_id40);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id41\n", reg.bits.set_pend_id41);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id42\n", reg.bits.set_pend_id42);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id43\n", reg.bits.set_pend_id43);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id44\n", reg.bits.set_pend_id44);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id45\n", reg.bits.set_pend_id45);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id46\n", reg.bits.set_pend_id46);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id47\n", reg.bits.set_pend_id47);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id48\n", reg.bits.set_pend_id48);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id49\n", reg.bits.set_pend_id49);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id50\n", reg.bits.set_pend_id50);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id51\n", reg.bits.set_pend_id51);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id52\n", reg.bits.set_pend_id52);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id53\n", reg.bits.set_pend_id53);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id54\n", reg.bits.set_pend_id54);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id55\n", reg.bits.set_pend_id55);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id56\n", reg.bits.set_pend_id56);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id57\n", reg.bits.set_pend_id57);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id58\n", reg.bits.set_pend_id58);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id59\n", reg.bits.set_pend_id59);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id60\n", reg.bits.set_pend_id60);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id61\n", reg.bits.set_pend_id61);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id62\n", reg.bits.set_pend_id62);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id63\n", reg.bits.set_pend_id63);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ispendr2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ispendr2_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_pend_id64\n", reg.bits.set_pend_id64);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id65\n", reg.bits.set_pend_id65);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id66\n", reg.bits.set_pend_id66);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id67\n", reg.bits.set_pend_id67);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id68\n", reg.bits.set_pend_id68);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id69\n", reg.bits.set_pend_id69);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id70\n", reg.bits.set_pend_id70);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id71\n", reg.bits.set_pend_id71);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id72\n", reg.bits.set_pend_id72);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id73\n", reg.bits.set_pend_id73);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id74\n", reg.bits.set_pend_id74);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id75\n", reg.bits.set_pend_id75);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id76\n", reg.bits.set_pend_id76);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id77\n", reg.bits.set_pend_id77);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id78\n", reg.bits.set_pend_id78);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id79\n", reg.bits.set_pend_id79);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id80\n", reg.bits.set_pend_id80);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id81\n", reg.bits.set_pend_id81);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id82\n", reg.bits.set_pend_id82);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id83\n", reg.bits.set_pend_id83);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id84\n", reg.bits.set_pend_id84);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id85\n", reg.bits.set_pend_id85);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id86\n", reg.bits.set_pend_id86);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id87\n", reg.bits.set_pend_id87);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id88\n", reg.bits.set_pend_id88);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id89\n", reg.bits.set_pend_id89);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id90\n", reg.bits.set_pend_id90);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id91\n", reg.bits.set_pend_id91);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id92\n", reg.bits.set_pend_id92);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id93\n", reg.bits.set_pend_id93);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id94\n", reg.bits.set_pend_id94);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id95\n", reg.bits.set_pend_id95);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ispendr3(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ispendr3_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_pend_id96\n", reg.bits.set_pend_id96);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id97\n", reg.bits.set_pend_id97);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id98\n", reg.bits.set_pend_id98);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id99\n", reg.bits.set_pend_id99);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id100\n", reg.bits.set_pend_id100);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id101\n", reg.bits.set_pend_id101);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id102\n", reg.bits.set_pend_id102);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id103\n", reg.bits.set_pend_id103);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id104\n", reg.bits.set_pend_id104);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id105\n", reg.bits.set_pend_id105);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id106\n", reg.bits.set_pend_id106);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id107\n", reg.bits.set_pend_id107);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id108\n", reg.bits.set_pend_id108);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id109\n", reg.bits.set_pend_id109);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id110\n", reg.bits.set_pend_id110);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id111\n", reg.bits.set_pend_id111);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id112\n", reg.bits.set_pend_id112);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id113\n", reg.bits.set_pend_id113);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id114\n", reg.bits.set_pend_id114);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id115\n", reg.bits.set_pend_id115);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id116\n", reg.bits.set_pend_id116);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id117\n", reg.bits.set_pend_id117);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id118\n", reg.bits.set_pend_id118);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id119\n", reg.bits.set_pend_id119);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id120\n", reg.bits.set_pend_id120);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id121\n", reg.bits.set_pend_id121);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id122\n", reg.bits.set_pend_id122);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id123\n", reg.bits.set_pend_id123);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id124\n", reg.bits.set_pend_id124);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id125\n", reg.bits.set_pend_id125);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id126\n", reg.bits.set_pend_id126);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id127\n", reg.bits.set_pend_id127);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ispendr4(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ispendr4_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_pend_id128\n", reg.bits.set_pend_id128);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id129\n", reg.bits.set_pend_id129);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id130\n", reg.bits.set_pend_id130);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id131\n", reg.bits.set_pend_id131);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id132\n", reg.bits.set_pend_id132);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id133\n", reg.bits.set_pend_id133);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id134\n", reg.bits.set_pend_id134);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id135\n", reg.bits.set_pend_id135);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id136\n", reg.bits.set_pend_id136);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id137\n", reg.bits.set_pend_id137);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id138\n", reg.bits.set_pend_id138);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id139\n", reg.bits.set_pend_id139);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id140\n", reg.bits.set_pend_id140);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id141\n", reg.bits.set_pend_id141);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id142\n", reg.bits.set_pend_id142);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id143\n", reg.bits.set_pend_id143);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id144\n", reg.bits.set_pend_id144);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id145\n", reg.bits.set_pend_id145);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id146\n", reg.bits.set_pend_id146);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id147\n", reg.bits.set_pend_id147);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id148\n", reg.bits.set_pend_id148);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id149\n", reg.bits.set_pend_id149);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id150\n", reg.bits.set_pend_id150);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id151\n", reg.bits.set_pend_id151);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id152\n", reg.bits.set_pend_id152);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id153\n", reg.bits.set_pend_id153);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id154\n", reg.bits.set_pend_id154);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id155\n", reg.bits.set_pend_id155);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id156\n", reg.bits.set_pend_id156);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id157\n", reg.bits.set_pend_id157);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id158\n", reg.bits.set_pend_id158);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id159\n", reg.bits.set_pend_id159);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ispendr5(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ispendr5_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_pend_id160\n", reg.bits.set_pend_id160);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id161\n", reg.bits.set_pend_id161);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id162\n", reg.bits.set_pend_id162);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id163\n", reg.bits.set_pend_id163);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id164\n", reg.bits.set_pend_id164);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id165\n", reg.bits.set_pend_id165);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id166\n", reg.bits.set_pend_id166);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id167\n", reg.bits.set_pend_id167);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id168\n", reg.bits.set_pend_id168);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id169\n", reg.bits.set_pend_id169);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id170\n", reg.bits.set_pend_id170);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id171\n", reg.bits.set_pend_id171);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id172\n", reg.bits.set_pend_id172);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id173\n", reg.bits.set_pend_id173);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id174\n", reg.bits.set_pend_id174);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id175\n", reg.bits.set_pend_id175);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id176\n", reg.bits.set_pend_id176);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id177\n", reg.bits.set_pend_id177);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id178\n", reg.bits.set_pend_id178);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id179\n", reg.bits.set_pend_id179);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id180\n", reg.bits.set_pend_id180);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id181\n", reg.bits.set_pend_id181);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id182\n", reg.bits.set_pend_id182);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id183\n", reg.bits.set_pend_id183);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id184\n", reg.bits.set_pend_id184);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id185\n", reg.bits.set_pend_id185);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id186\n", reg.bits.set_pend_id186);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id187\n", reg.bits.set_pend_id187);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id188\n", reg.bits.set_pend_id188);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id189\n", reg.bits.set_pend_id189);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id190\n", reg.bits.set_pend_id190);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id191\n", reg.bits.set_pend_id191);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ispendr6(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ispendr6_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_pend_id192\n", reg.bits.set_pend_id192);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id193\n", reg.bits.set_pend_id193);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id194\n", reg.bits.set_pend_id194);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id195\n", reg.bits.set_pend_id195);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id196\n", reg.bits.set_pend_id196);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id197\n", reg.bits.set_pend_id197);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id198\n", reg.bits.set_pend_id198);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id199\n", reg.bits.set_pend_id199);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id200\n", reg.bits.set_pend_id200);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id201\n", reg.bits.set_pend_id201);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id202\n", reg.bits.set_pend_id202);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id203\n", reg.bits.set_pend_id203);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id204\n", reg.bits.set_pend_id204);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id205\n", reg.bits.set_pend_id205);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id206\n", reg.bits.set_pend_id206);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id207\n", reg.bits.set_pend_id207);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id208\n", reg.bits.set_pend_id208);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id209\n", reg.bits.set_pend_id209);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id210\n", reg.bits.set_pend_id210);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id211\n", reg.bits.set_pend_id211);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id212\n", reg.bits.set_pend_id212);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id213\n", reg.bits.set_pend_id213);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id214\n", reg.bits.set_pend_id214);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id215\n", reg.bits.set_pend_id215);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id216\n", reg.bits.set_pend_id216);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id217\n", reg.bits.set_pend_id217);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id218\n", reg.bits.set_pend_id218);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id219\n", reg.bits.set_pend_id219);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id220\n", reg.bits.set_pend_id220);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id221\n", reg.bits.set_pend_id221);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id222\n", reg.bits.set_pend_id222);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id223\n", reg.bits.set_pend_id223);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icpendr0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icpendr0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clr_pend_id0\n", reg.bits.clr_pend_id0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id1\n", reg.bits.clr_pend_id1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id2\n", reg.bits.clr_pend_id2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id3\n", reg.bits.clr_pend_id3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id4\n", reg.bits.clr_pend_id4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id5\n", reg.bits.clr_pend_id5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id6\n", reg.bits.clr_pend_id6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id7\n", reg.bits.clr_pend_id7);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id8\n", reg.bits.clr_pend_id8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id9\n", reg.bits.clr_pend_id9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id10\n", reg.bits.clr_pend_id10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id11\n", reg.bits.clr_pend_id11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id12\n", reg.bits.clr_pend_id12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id13\n", reg.bits.clr_pend_id13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id14\n", reg.bits.clr_pend_id14);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id15\n", reg.bits.clr_pend_id15);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id16\n", reg.bits.clr_pend_id16);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id17\n", reg.bits.clr_pend_id17);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id18\n", reg.bits.clr_pend_id18);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id19\n", reg.bits.clr_pend_id19);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id20\n", reg.bits.clr_pend_id20);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id21\n", reg.bits.clr_pend_id21);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id22\n", reg.bits.clr_pend_id22);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id23\n", reg.bits.clr_pend_id23);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id24\n", reg.bits.clr_pend_id24);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id25\n", reg.bits.clr_pend_id25);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id26\n", reg.bits.clr_pend_id26);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id27\n", reg.bits.clr_pend_id27);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id28\n", reg.bits.clr_pend_id28);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id29\n", reg.bits.clr_pend_id29);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id30\n", reg.bits.clr_pend_id30);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id31\n", reg.bits.clr_pend_id31);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icpendr1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icpendr1_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clr_pend_id32\n", reg.bits.clr_pend_id32);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id33\n", reg.bits.clr_pend_id33);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id34\n", reg.bits.clr_pend_id34);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id35\n", reg.bits.clr_pend_id35);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id36\n", reg.bits.clr_pend_id36);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id37\n", reg.bits.clr_pend_id37);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id38\n", reg.bits.clr_pend_id38);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id39\n", reg.bits.clr_pend_id39);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id40\n", reg.bits.clr_pend_id40);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id41\n", reg.bits.clr_pend_id41);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id42\n", reg.bits.clr_pend_id42);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id43\n", reg.bits.clr_pend_id43);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id44\n", reg.bits.clr_pend_id44);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id45\n", reg.bits.clr_pend_id45);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id46\n", reg.bits.clr_pend_id46);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id47\n", reg.bits.clr_pend_id47);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id48\n", reg.bits.clr_pend_id48);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id49\n", reg.bits.clr_pend_id49);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id50\n", reg.bits.clr_pend_id50);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id51\n", reg.bits.clr_pend_id51);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id52\n", reg.bits.clr_pend_id52);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id53\n", reg.bits.clr_pend_id53);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id54\n", reg.bits.clr_pend_id54);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id55\n", reg.bits.clr_pend_id55);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id56\n", reg.bits.clr_pend_id56);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id57\n", reg.bits.clr_pend_id57);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id58\n", reg.bits.clr_pend_id58);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id59\n", reg.bits.clr_pend_id59);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id60\n", reg.bits.clr_pend_id60);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id61\n", reg.bits.clr_pend_id61);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id62\n", reg.bits.clr_pend_id62);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id63\n", reg.bits.clr_pend_id63);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icpendr2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icpendr2_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clr_pend_id64\n", reg.bits.clr_pend_id64);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id65\n", reg.bits.clr_pend_id65);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id66\n", reg.bits.clr_pend_id66);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id67\n", reg.bits.clr_pend_id67);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id68\n", reg.bits.clr_pend_id68);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id69\n", reg.bits.clr_pend_id69);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id70\n", reg.bits.clr_pend_id70);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id71\n", reg.bits.clr_pend_id71);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id72\n", reg.bits.clr_pend_id72);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id73\n", reg.bits.clr_pend_id73);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id74\n", reg.bits.clr_pend_id74);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id75\n", reg.bits.clr_pend_id75);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id76\n", reg.bits.clr_pend_id76);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id77\n", reg.bits.clr_pend_id77);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id78\n", reg.bits.clr_pend_id78);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id79\n", reg.bits.clr_pend_id79);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id80\n", reg.bits.clr_pend_id80);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id81\n", reg.bits.clr_pend_id81);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id82\n", reg.bits.clr_pend_id82);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id83\n", reg.bits.clr_pend_id83);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id84\n", reg.bits.clr_pend_id84);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id85\n", reg.bits.clr_pend_id85);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id86\n", reg.bits.clr_pend_id86);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id87\n", reg.bits.clr_pend_id87);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id88\n", reg.bits.clr_pend_id88);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id89\n", reg.bits.clr_pend_id89);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id90\n", reg.bits.clr_pend_id90);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id91\n", reg.bits.clr_pend_id91);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id92\n", reg.bits.clr_pend_id92);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id93\n", reg.bits.clr_pend_id93);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id94\n", reg.bits.clr_pend_id94);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id95\n", reg.bits.clr_pend_id95);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icpendr3(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icpendr3_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clr_pend_id96\n", reg.bits.clr_pend_id96);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id97\n", reg.bits.clr_pend_id97);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id98\n", reg.bits.clr_pend_id98);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id99\n", reg.bits.clr_pend_id99);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id100\n", reg.bits.clr_pend_id100);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id101\n", reg.bits.clr_pend_id101);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id102\n", reg.bits.clr_pend_id102);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id103\n", reg.bits.clr_pend_id103);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id104\n", reg.bits.clr_pend_id104);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id105\n", reg.bits.clr_pend_id105);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id106\n", reg.bits.clr_pend_id106);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id107\n", reg.bits.clr_pend_id107);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id108\n", reg.bits.clr_pend_id108);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id109\n", reg.bits.clr_pend_id109);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id110\n", reg.bits.clr_pend_id110);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id111\n", reg.bits.clr_pend_id111);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id112\n", reg.bits.clr_pend_id112);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id113\n", reg.bits.clr_pend_id113);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id114\n", reg.bits.clr_pend_id114);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id115\n", reg.bits.clr_pend_id115);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id116\n", reg.bits.clr_pend_id116);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id117\n", reg.bits.clr_pend_id117);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id118\n", reg.bits.clr_pend_id118);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id119\n", reg.bits.clr_pend_id119);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id120\n", reg.bits.clr_pend_id120);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id121\n", reg.bits.clr_pend_id121);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id122\n", reg.bits.clr_pend_id122);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id123\n", reg.bits.clr_pend_id123);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id124\n", reg.bits.clr_pend_id124);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id125\n", reg.bits.clr_pend_id125);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id126\n", reg.bits.clr_pend_id126);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id127\n", reg.bits.clr_pend_id127);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icpendr4(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icpendr4_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clr_pend_id128\n", reg.bits.clr_pend_id128);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id129\n", reg.bits.clr_pend_id129);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id130\n", reg.bits.clr_pend_id130);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id131\n", reg.bits.clr_pend_id131);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id132\n", reg.bits.clr_pend_id132);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id133\n", reg.bits.clr_pend_id133);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id134\n", reg.bits.clr_pend_id134);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id135\n", reg.bits.clr_pend_id135);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id136\n", reg.bits.clr_pend_id136);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id137\n", reg.bits.clr_pend_id137);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id138\n", reg.bits.clr_pend_id138);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id139\n", reg.bits.clr_pend_id139);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id140\n", reg.bits.clr_pend_id140);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id141\n", reg.bits.clr_pend_id141);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id142\n", reg.bits.clr_pend_id142);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id143\n", reg.bits.clr_pend_id143);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id144\n", reg.bits.clr_pend_id144);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id145\n", reg.bits.clr_pend_id145);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id146\n", reg.bits.clr_pend_id146);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id147\n", reg.bits.clr_pend_id147);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id148\n", reg.bits.clr_pend_id148);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id149\n", reg.bits.clr_pend_id149);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id150\n", reg.bits.clr_pend_id150);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id151\n", reg.bits.clr_pend_id151);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id152\n", reg.bits.clr_pend_id152);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id153\n", reg.bits.clr_pend_id153);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id154\n", reg.bits.clr_pend_id154);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id155\n", reg.bits.clr_pend_id155);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id156\n", reg.bits.clr_pend_id156);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id157\n", reg.bits.clr_pend_id157);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id158\n", reg.bits.clr_pend_id158);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id159\n", reg.bits.clr_pend_id159);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icpendr5(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icpendr5_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clr_pend_id160\n", reg.bits.clr_pend_id160);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id161\n", reg.bits.clr_pend_id161);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id162\n", reg.bits.clr_pend_id162);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id163\n", reg.bits.clr_pend_id163);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id164\n", reg.bits.clr_pend_id164);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id165\n", reg.bits.clr_pend_id165);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id166\n", reg.bits.clr_pend_id166);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id167\n", reg.bits.clr_pend_id167);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id168\n", reg.bits.clr_pend_id168);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id169\n", reg.bits.clr_pend_id169);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id170\n", reg.bits.clr_pend_id170);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id171\n", reg.bits.clr_pend_id171);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id172\n", reg.bits.clr_pend_id172);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id173\n", reg.bits.clr_pend_id173);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id174\n", reg.bits.clr_pend_id174);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id175\n", reg.bits.clr_pend_id175);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id176\n", reg.bits.clr_pend_id176);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id177\n", reg.bits.clr_pend_id177);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id178\n", reg.bits.clr_pend_id178);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id179\n", reg.bits.clr_pend_id179);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id180\n", reg.bits.clr_pend_id180);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id181\n", reg.bits.clr_pend_id181);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id182\n", reg.bits.clr_pend_id182);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id183\n", reg.bits.clr_pend_id183);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id184\n", reg.bits.clr_pend_id184);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id185\n", reg.bits.clr_pend_id185);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id186\n", reg.bits.clr_pend_id186);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id187\n", reg.bits.clr_pend_id187);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id188\n", reg.bits.clr_pend_id188);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id189\n", reg.bits.clr_pend_id189);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id190\n", reg.bits.clr_pend_id190);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id191\n", reg.bits.clr_pend_id191);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icpendr6(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icpendr6_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clr_pend_id192\n", reg.bits.clr_pend_id192);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id193\n", reg.bits.clr_pend_id193);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id194\n", reg.bits.clr_pend_id194);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id195\n", reg.bits.clr_pend_id195);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id196\n", reg.bits.clr_pend_id196);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id197\n", reg.bits.clr_pend_id197);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id198\n", reg.bits.clr_pend_id198);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id199\n", reg.bits.clr_pend_id199);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id200\n", reg.bits.clr_pend_id200);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id201\n", reg.bits.clr_pend_id201);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id202\n", reg.bits.clr_pend_id202);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id203\n", reg.bits.clr_pend_id203);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id204\n", reg.bits.clr_pend_id204);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id205\n", reg.bits.clr_pend_id205);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id206\n", reg.bits.clr_pend_id206);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id207\n", reg.bits.clr_pend_id207);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id208\n", reg.bits.clr_pend_id208);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id209\n", reg.bits.clr_pend_id209);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id210\n", reg.bits.clr_pend_id210);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id211\n", reg.bits.clr_pend_id211);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id212\n", reg.bits.clr_pend_id212);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id213\n", reg.bits.clr_pend_id213);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id214\n", reg.bits.clr_pend_id214);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id215\n", reg.bits.clr_pend_id215);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id216\n", reg.bits.clr_pend_id216);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id217\n", reg.bits.clr_pend_id217);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id218\n", reg.bits.clr_pend_id218);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id219\n", reg.bits.clr_pend_id219);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id220\n", reg.bits.clr_pend_id220);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id221\n", reg.bits.clr_pend_id221);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id222\n", reg.bits.clr_pend_id222);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id223\n", reg.bits.clr_pend_id223);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_isactiver0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_isactiver0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_active_id0\n", reg.bits.set_active_id0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id1\n", reg.bits.set_active_id1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id2\n", reg.bits.set_active_id2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id3\n", reg.bits.set_active_id3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id4\n", reg.bits.set_active_id4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id5\n", reg.bits.set_active_id5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id6\n", reg.bits.set_active_id6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id7\n", reg.bits.set_active_id7);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id8\n", reg.bits.set_active_id8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id9\n", reg.bits.set_active_id9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id10\n", reg.bits.set_active_id10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id11\n", reg.bits.set_active_id11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id12\n", reg.bits.set_active_id12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id13\n", reg.bits.set_active_id13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id14\n", reg.bits.set_active_id14);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id15\n", reg.bits.set_active_id15);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id16\n", reg.bits.set_active_id16);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id17\n", reg.bits.set_active_id17);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id18\n", reg.bits.set_active_id18);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id19\n", reg.bits.set_active_id19);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id20\n", reg.bits.set_active_id20);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id21\n", reg.bits.set_active_id21);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id22\n", reg.bits.set_active_id22);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id23\n", reg.bits.set_active_id23);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id24\n", reg.bits.set_active_id24);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id25\n", reg.bits.set_active_id25);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id26\n", reg.bits.set_active_id26);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id27\n", reg.bits.set_active_id27);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id28\n", reg.bits.set_active_id28);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id29\n", reg.bits.set_active_id29);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id30\n", reg.bits.set_active_id30);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id31\n", reg.bits.set_active_id31);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_isactiver1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_isactiver1_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_active_id32\n", reg.bits.set_active_id32);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id33\n", reg.bits.set_active_id33);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id34\n", reg.bits.set_active_id34);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id35\n", reg.bits.set_active_id35);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id36\n", reg.bits.set_active_id36);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id37\n", reg.bits.set_active_id37);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id38\n", reg.bits.set_active_id38);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id39\n", reg.bits.set_active_id39);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id40\n", reg.bits.set_active_id40);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id41\n", reg.bits.set_active_id41);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id42\n", reg.bits.set_active_id42);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id43\n", reg.bits.set_active_id43);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id44\n", reg.bits.set_active_id44);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id45\n", reg.bits.set_active_id45);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id46\n", reg.bits.set_active_id46);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id47\n", reg.bits.set_active_id47);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id48\n", reg.bits.set_active_id48);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id49\n", reg.bits.set_active_id49);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id50\n", reg.bits.set_active_id50);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id51\n", reg.bits.set_active_id51);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id52\n", reg.bits.set_active_id52);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id53\n", reg.bits.set_active_id53);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id54\n", reg.bits.set_active_id54);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id55\n", reg.bits.set_active_id55);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id56\n", reg.bits.set_active_id56);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id57\n", reg.bits.set_active_id57);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id58\n", reg.bits.set_active_id58);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id59\n", reg.bits.set_active_id59);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id60\n", reg.bits.set_active_id60);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id61\n", reg.bits.set_active_id61);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id62\n", reg.bits.set_active_id62);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id63\n", reg.bits.set_active_id63);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_isactiver2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_isactiver2_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_active_id64\n", reg.bits.set_active_id64);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id65\n", reg.bits.set_active_id65);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id66\n", reg.bits.set_active_id66);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id67\n", reg.bits.set_active_id67);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id68\n", reg.bits.set_active_id68);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id69\n", reg.bits.set_active_id69);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id70\n", reg.bits.set_active_id70);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id71\n", reg.bits.set_active_id71);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id72\n", reg.bits.set_active_id72);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id73\n", reg.bits.set_active_id73);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id74\n", reg.bits.set_active_id74);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id75\n", reg.bits.set_active_id75);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id76\n", reg.bits.set_active_id76);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id77\n", reg.bits.set_active_id77);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id78\n", reg.bits.set_active_id78);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id79\n", reg.bits.set_active_id79);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id80\n", reg.bits.set_active_id80);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id81\n", reg.bits.set_active_id81);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id82\n", reg.bits.set_active_id82);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id83\n", reg.bits.set_active_id83);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id84\n", reg.bits.set_active_id84);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id85\n", reg.bits.set_active_id85);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id86\n", reg.bits.set_active_id86);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id87\n", reg.bits.set_active_id87);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id88\n", reg.bits.set_active_id88);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id89\n", reg.bits.set_active_id89);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id90\n", reg.bits.set_active_id90);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id91\n", reg.bits.set_active_id91);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id92\n", reg.bits.set_active_id92);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id93\n", reg.bits.set_active_id93);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id94\n", reg.bits.set_active_id94);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id95\n", reg.bits.set_active_id95);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_isactiver3(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_isactiver3_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_active_id96\n", reg.bits.set_active_id96);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id97\n", reg.bits.set_active_id97);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id98\n", reg.bits.set_active_id98);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id99\n", reg.bits.set_active_id99);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id100\n", reg.bits.set_active_id100);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id101\n", reg.bits.set_active_id101);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id102\n", reg.bits.set_active_id102);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id103\n", reg.bits.set_active_id103);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id104\n", reg.bits.set_active_id104);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id105\n", reg.bits.set_active_id105);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id106\n", reg.bits.set_active_id106);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id107\n", reg.bits.set_active_id107);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id108\n", reg.bits.set_active_id108);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id109\n", reg.bits.set_active_id109);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id110\n", reg.bits.set_active_id110);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id111\n", reg.bits.set_active_id111);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id112\n", reg.bits.set_active_id112);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id113\n", reg.bits.set_active_id113);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id114\n", reg.bits.set_active_id114);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id115\n", reg.bits.set_active_id115);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id116\n", reg.bits.set_active_id116);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id117\n", reg.bits.set_active_id117);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id118\n", reg.bits.set_active_id118);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id119\n", reg.bits.set_active_id119);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id120\n", reg.bits.set_active_id120);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id121\n", reg.bits.set_active_id121);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id122\n", reg.bits.set_active_id122);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id123\n", reg.bits.set_active_id123);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id124\n", reg.bits.set_active_id124);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id125\n", reg.bits.set_active_id125);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id126\n", reg.bits.set_active_id126);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id127\n", reg.bits.set_active_id127);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_isactiver4(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_isactiver4_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_active_id128\n", reg.bits.set_active_id128);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id129\n", reg.bits.set_active_id129);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id130\n", reg.bits.set_active_id130);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id131\n", reg.bits.set_active_id131);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id132\n", reg.bits.set_active_id132);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id133\n", reg.bits.set_active_id133);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id134\n", reg.bits.set_active_id134);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id135\n", reg.bits.set_active_id135);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id136\n", reg.bits.set_active_id136);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id137\n", reg.bits.set_active_id137);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id138\n", reg.bits.set_active_id138);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id139\n", reg.bits.set_active_id139);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id140\n", reg.bits.set_active_id140);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id141\n", reg.bits.set_active_id141);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id142\n", reg.bits.set_active_id142);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id143\n", reg.bits.set_active_id143);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id144\n", reg.bits.set_active_id144);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id145\n", reg.bits.set_active_id145);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id146\n", reg.bits.set_active_id146);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id147\n", reg.bits.set_active_id147);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id148\n", reg.bits.set_active_id148);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id149\n", reg.bits.set_active_id149);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id150\n", reg.bits.set_active_id150);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id151\n", reg.bits.set_active_id151);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id152\n", reg.bits.set_active_id152);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id153\n", reg.bits.set_active_id153);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id154\n", reg.bits.set_active_id154);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id155\n", reg.bits.set_active_id155);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id156\n", reg.bits.set_active_id156);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id157\n", reg.bits.set_active_id157);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id158\n", reg.bits.set_active_id158);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id159\n", reg.bits.set_active_id159);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_isactiver5(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_isactiver5_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_active_id160\n", reg.bits.set_active_id160);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id161\n", reg.bits.set_active_id161);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id162\n", reg.bits.set_active_id162);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id163\n", reg.bits.set_active_id163);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id164\n", reg.bits.set_active_id164);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id165\n", reg.bits.set_active_id165);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id166\n", reg.bits.set_active_id166);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id167\n", reg.bits.set_active_id167);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id168\n", reg.bits.set_active_id168);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id169\n", reg.bits.set_active_id169);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id170\n", reg.bits.set_active_id170);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id171\n", reg.bits.set_active_id171);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id172\n", reg.bits.set_active_id172);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id173\n", reg.bits.set_active_id173);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id174\n", reg.bits.set_active_id174);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id175\n", reg.bits.set_active_id175);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id176\n", reg.bits.set_active_id176);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id177\n", reg.bits.set_active_id177);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id178\n", reg.bits.set_active_id178);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id179\n", reg.bits.set_active_id179);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id180\n", reg.bits.set_active_id180);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id181\n", reg.bits.set_active_id181);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id182\n", reg.bits.set_active_id182);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id183\n", reg.bits.set_active_id183);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id184\n", reg.bits.set_active_id184);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id185\n", reg.bits.set_active_id185);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id186\n", reg.bits.set_active_id186);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id187\n", reg.bits.set_active_id187);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id188\n", reg.bits.set_active_id188);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id189\n", reg.bits.set_active_id189);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id190\n", reg.bits.set_active_id190);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id191\n", reg.bits.set_active_id191);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_isactiver6(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_isactiver6_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_active_id192\n", reg.bits.set_active_id192);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id193\n", reg.bits.set_active_id193);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id194\n", reg.bits.set_active_id194);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id195\n", reg.bits.set_active_id195);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id196\n", reg.bits.set_active_id196);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id197\n", reg.bits.set_active_id197);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id198\n", reg.bits.set_active_id198);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id199\n", reg.bits.set_active_id199);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id200\n", reg.bits.set_active_id200);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id201\n", reg.bits.set_active_id201);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id202\n", reg.bits.set_active_id202);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id203\n", reg.bits.set_active_id203);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id204\n", reg.bits.set_active_id204);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id205\n", reg.bits.set_active_id205);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id206\n", reg.bits.set_active_id206);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id207\n", reg.bits.set_active_id207);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id208\n", reg.bits.set_active_id208);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id209\n", reg.bits.set_active_id209);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id210\n", reg.bits.set_active_id210);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id211\n", reg.bits.set_active_id211);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id212\n", reg.bits.set_active_id212);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id213\n", reg.bits.set_active_id213);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id214\n", reg.bits.set_active_id214);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id215\n", reg.bits.set_active_id215);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id216\n", reg.bits.set_active_id216);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id217\n", reg.bits.set_active_id217);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id218\n", reg.bits.set_active_id218);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id219\n", reg.bits.set_active_id219);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id220\n", reg.bits.set_active_id220);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id221\n", reg.bits.set_active_id221);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id222\n", reg.bits.set_active_id222);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_active_id223\n", reg.bits.set_active_id223);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icactiver0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icactiver0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clear_active_id0\n", reg.bits.clear_active_id0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id1\n", reg.bits.clear_active_id1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id2\n", reg.bits.clear_active_id2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id3\n", reg.bits.clear_active_id3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id4\n", reg.bits.clear_active_id4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id5\n", reg.bits.clear_active_id5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id6\n", reg.bits.clear_active_id6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id7\n", reg.bits.clear_active_id7);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id8\n", reg.bits.clear_active_id8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id9\n", reg.bits.clear_active_id9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id10\n", reg.bits.clear_active_id10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id11\n", reg.bits.clear_active_id11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id12\n", reg.bits.clear_active_id12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id13\n", reg.bits.clear_active_id13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id14\n", reg.bits.clear_active_id14);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id15\n", reg.bits.clear_active_id15);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id16\n", reg.bits.clear_active_id16);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id17\n", reg.bits.clear_active_id17);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id18\n", reg.bits.clear_active_id18);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id19\n", reg.bits.clear_active_id19);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id20\n", reg.bits.clear_active_id20);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id21\n", reg.bits.clear_active_id21);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id22\n", reg.bits.clear_active_id22);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id23\n", reg.bits.clear_active_id23);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id24\n", reg.bits.clear_active_id24);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id25\n", reg.bits.clear_active_id25);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id26\n", reg.bits.clear_active_id26);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id27\n", reg.bits.clear_active_id27);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id28\n", reg.bits.clear_active_id28);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id29\n", reg.bits.clear_active_id29);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id30\n", reg.bits.clear_active_id30);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id31\n", reg.bits.clear_active_id31);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icactiver1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icactiver1_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clear_active_id32\n", reg.bits.clear_active_id32);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id33\n", reg.bits.clear_active_id33);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id34\n", reg.bits.clear_active_id34);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id35\n", reg.bits.clear_active_id35);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id36\n", reg.bits.clear_active_id36);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id37\n", reg.bits.clear_active_id37);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id38\n", reg.bits.clear_active_id38);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id39\n", reg.bits.clear_active_id39);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id40\n", reg.bits.clear_active_id40);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id41\n", reg.bits.clear_active_id41);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id42\n", reg.bits.clear_active_id42);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id43\n", reg.bits.clear_active_id43);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id44\n", reg.bits.clear_active_id44);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id45\n", reg.bits.clear_active_id45);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id46\n", reg.bits.clear_active_id46);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id47\n", reg.bits.clear_active_id47);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id48\n", reg.bits.clear_active_id48);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id49\n", reg.bits.clear_active_id49);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id50\n", reg.bits.clear_active_id50);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id51\n", reg.bits.clear_active_id51);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id52\n", reg.bits.clear_active_id52);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id53\n", reg.bits.clear_active_id53);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id54\n", reg.bits.clear_active_id54);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id55\n", reg.bits.clear_active_id55);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id56\n", reg.bits.clear_active_id56);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id57\n", reg.bits.clear_active_id57);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id58\n", reg.bits.clear_active_id58);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id59\n", reg.bits.clear_active_id59);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id60\n", reg.bits.clear_active_id60);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id61\n", reg.bits.clear_active_id61);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id62\n", reg.bits.clear_active_id62);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id63\n", reg.bits.clear_active_id63);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icactiver2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icactiver2_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clear_active_id64\n", reg.bits.clear_active_id64);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id65\n", reg.bits.clear_active_id65);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id66\n", reg.bits.clear_active_id66);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id67\n", reg.bits.clear_active_id67);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id68\n", reg.bits.clear_active_id68);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id69\n", reg.bits.clear_active_id69);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id70\n", reg.bits.clear_active_id70);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id71\n", reg.bits.clear_active_id71);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id72\n", reg.bits.clear_active_id72);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id73\n", reg.bits.clear_active_id73);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id74\n", reg.bits.clear_active_id74);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id75\n", reg.bits.clear_active_id75);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id76\n", reg.bits.clear_active_id76);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id77\n", reg.bits.clear_active_id77);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id78\n", reg.bits.clear_active_id78);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id79\n", reg.bits.clear_active_id79);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id80\n", reg.bits.clear_active_id80);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id81\n", reg.bits.clear_active_id81);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id82\n", reg.bits.clear_active_id82);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id83\n", reg.bits.clear_active_id83);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id84\n", reg.bits.clear_active_id84);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id85\n", reg.bits.clear_active_id85);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id86\n", reg.bits.clear_active_id86);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id87\n", reg.bits.clear_active_id87);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id88\n", reg.bits.clear_active_id88);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id89\n", reg.bits.clear_active_id89);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id90\n", reg.bits.clear_active_id90);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id91\n", reg.bits.clear_active_id91);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id92\n", reg.bits.clear_active_id92);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id93\n", reg.bits.clear_active_id93);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id94\n", reg.bits.clear_active_id94);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id95\n", reg.bits.clear_active_id95);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icactiver3(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icactiver3_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clear_active_id96\n", reg.bits.clear_active_id96);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id97\n", reg.bits.clear_active_id97);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id98\n", reg.bits.clear_active_id98);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id99\n", reg.bits.clear_active_id99);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id100\n", reg.bits.clear_active_id100);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id101\n", reg.bits.clear_active_id101);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id102\n", reg.bits.clear_active_id102);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id103\n", reg.bits.clear_active_id103);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id104\n", reg.bits.clear_active_id104);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id105\n", reg.bits.clear_active_id105);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id106\n", reg.bits.clear_active_id106);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id107\n", reg.bits.clear_active_id107);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id108\n", reg.bits.clear_active_id108);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id109\n", reg.bits.clear_active_id109);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id110\n", reg.bits.clear_active_id110);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id111\n", reg.bits.clear_active_id111);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id112\n", reg.bits.clear_active_id112);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id113\n", reg.bits.clear_active_id113);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id114\n", reg.bits.clear_active_id114);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id115\n", reg.bits.clear_active_id115);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id116\n", reg.bits.clear_active_id116);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id117\n", reg.bits.clear_active_id117);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id118\n", reg.bits.clear_active_id118);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id119\n", reg.bits.clear_active_id119);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id120\n", reg.bits.clear_active_id120);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id121\n", reg.bits.clear_active_id121);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id122\n", reg.bits.clear_active_id122);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id123\n", reg.bits.clear_active_id123);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id124\n", reg.bits.clear_active_id124);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id125\n", reg.bits.clear_active_id125);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id126\n", reg.bits.clear_active_id126);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id127\n", reg.bits.clear_active_id127);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icactiver4(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icactiver4_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clear_active_id128\n", reg.bits.clear_active_id128);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id129\n", reg.bits.clear_active_id129);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id130\n", reg.bits.clear_active_id130);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id131\n", reg.bits.clear_active_id131);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id132\n", reg.bits.clear_active_id132);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id133\n", reg.bits.clear_active_id133);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id134\n", reg.bits.clear_active_id134);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id135\n", reg.bits.clear_active_id135);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id136\n", reg.bits.clear_active_id136);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id137\n", reg.bits.clear_active_id137);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id138\n", reg.bits.clear_active_id138);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id139\n", reg.bits.clear_active_id139);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id140\n", reg.bits.clear_active_id140);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id141\n", reg.bits.clear_active_id141);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id142\n", reg.bits.clear_active_id142);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id143\n", reg.bits.clear_active_id143);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id144\n", reg.bits.clear_active_id144);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id145\n", reg.bits.clear_active_id145);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id146\n", reg.bits.clear_active_id146);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id147\n", reg.bits.clear_active_id147);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id148\n", reg.bits.clear_active_id148);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id149\n", reg.bits.clear_active_id149);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id150\n", reg.bits.clear_active_id150);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id151\n", reg.bits.clear_active_id151);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id152\n", reg.bits.clear_active_id152);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id153\n", reg.bits.clear_active_id153);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id154\n", reg.bits.clear_active_id154);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id155\n", reg.bits.clear_active_id155);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id156\n", reg.bits.clear_active_id156);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id157\n", reg.bits.clear_active_id157);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id158\n", reg.bits.clear_active_id158);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id159\n", reg.bits.clear_active_id159);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icactiver5(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icactiver5_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clear_active_id160\n", reg.bits.clear_active_id160);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id161\n", reg.bits.clear_active_id161);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id162\n", reg.bits.clear_active_id162);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id163\n", reg.bits.clear_active_id163);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id164\n", reg.bits.clear_active_id164);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id165\n", reg.bits.clear_active_id165);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id166\n", reg.bits.clear_active_id166);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id167\n", reg.bits.clear_active_id167);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id168\n", reg.bits.clear_active_id168);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id169\n", reg.bits.clear_active_id169);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id170\n", reg.bits.clear_active_id170);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id171\n", reg.bits.clear_active_id171);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id172\n", reg.bits.clear_active_id172);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id173\n", reg.bits.clear_active_id173);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id174\n", reg.bits.clear_active_id174);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id175\n", reg.bits.clear_active_id175);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id176\n", reg.bits.clear_active_id176);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id177\n", reg.bits.clear_active_id177);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id178\n", reg.bits.clear_active_id178);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id179\n", reg.bits.clear_active_id179);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id180\n", reg.bits.clear_active_id180);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id181\n", reg.bits.clear_active_id181);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id182\n", reg.bits.clear_active_id182);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id183\n", reg.bits.clear_active_id183);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id184\n", reg.bits.clear_active_id184);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id185\n", reg.bits.clear_active_id185);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id186\n", reg.bits.clear_active_id186);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id187\n", reg.bits.clear_active_id187);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id188\n", reg.bits.clear_active_id188);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id189\n", reg.bits.clear_active_id189);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id190\n", reg.bits.clear_active_id190);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id191\n", reg.bits.clear_active_id191);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icactiver6(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icactiver6_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clear_active_id192\n", reg.bits.clear_active_id192);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id193\n", reg.bits.clear_active_id193);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id194\n", reg.bits.clear_active_id194);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id195\n", reg.bits.clear_active_id195);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id196\n", reg.bits.clear_active_id196);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id197\n", reg.bits.clear_active_id197);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id198\n", reg.bits.clear_active_id198);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id199\n", reg.bits.clear_active_id199);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id200\n", reg.bits.clear_active_id200);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id201\n", reg.bits.clear_active_id201);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id202\n", reg.bits.clear_active_id202);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id203\n", reg.bits.clear_active_id203);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id204\n", reg.bits.clear_active_id204);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id205\n", reg.bits.clear_active_id205);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id206\n", reg.bits.clear_active_id206);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id207\n", reg.bits.clear_active_id207);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id208\n", reg.bits.clear_active_id208);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id209\n", reg.bits.clear_active_id209);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id210\n", reg.bits.clear_active_id210);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id211\n", reg.bits.clear_active_id211);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id212\n", reg.bits.clear_active_id212);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id213\n", reg.bits.clear_active_id213);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id214\n", reg.bits.clear_active_id214);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id215\n", reg.bits.clear_active_id215);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id216\n", reg.bits.clear_active_id216);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id217\n", reg.bits.clear_active_id217);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id218\n", reg.bits.clear_active_id218);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id219\n", reg.bits.clear_active_id219);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id220\n", reg.bits.clear_active_id220);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id221\n", reg.bits.clear_active_id221);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id222\n", reg.bits.clear_active_id222);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clear_active_id223\n", reg.bits.clear_active_id223);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id0\n", reg.bits.ipriority_id0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id1\n", reg.bits.ipriority_id1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id2\n", reg.bits.ipriority_id2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id3\n", reg.bits.ipriority_id3);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr1_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id4\n", reg.bits.ipriority_id4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id5\n", reg.bits.ipriority_id5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id6\n", reg.bits.ipriority_id6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id7\n", reg.bits.ipriority_id7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr2_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id8\n", reg.bits.ipriority_id8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id9\n", reg.bits.ipriority_id9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id10\n", reg.bits.ipriority_id10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id11\n", reg.bits.ipriority_id11);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr3(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr3_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id12\n", reg.bits.ipriority_id12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id13\n", reg.bits.ipriority_id13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id14\n", reg.bits.ipriority_id14);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id15\n", reg.bits.ipriority_id15);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr4(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr4_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id16\n", reg.bits.ipriority_id16);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id17\n", reg.bits.ipriority_id17);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id18\n", reg.bits.ipriority_id18);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id19\n", reg.bits.ipriority_id19);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr5(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr5_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id20\n", reg.bits.ipriority_id20);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id21\n", reg.bits.ipriority_id21);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id22\n", reg.bits.ipriority_id22);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id23\n", reg.bits.ipriority_id23);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr6(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr6_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id24\n", reg.bits.ipriority_id24);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id25\n", reg.bits.ipriority_id25);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id26\n", reg.bits.ipriority_id26);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id27\n", reg.bits.ipriority_id27);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr7(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr7_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id28\n", reg.bits.ipriority_id28);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id29\n", reg.bits.ipriority_id29);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id30\n", reg.bits.ipriority_id30);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id31\n", reg.bits.ipriority_id31);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr8(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr8_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id32\n", reg.bits.ipriority_id32);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id33\n", reg.bits.ipriority_id33);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id34\n", reg.bits.ipriority_id34);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id35\n", reg.bits.ipriority_id35);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr9(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr9_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id36\n", reg.bits.ipriority_id36);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id37\n", reg.bits.ipriority_id37);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id38\n", reg.bits.ipriority_id38);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id39\n", reg.bits.ipriority_id39);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr10(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr10_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id40\n", reg.bits.ipriority_id40);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id41\n", reg.bits.ipriority_id41);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id42\n", reg.bits.ipriority_id42);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id43\n", reg.bits.ipriority_id43);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr11(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr11_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id44\n", reg.bits.ipriority_id44);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id45\n", reg.bits.ipriority_id45);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id46\n", reg.bits.ipriority_id46);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id47\n", reg.bits.ipriority_id47);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr12(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr12_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id48\n", reg.bits.ipriority_id48);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id49\n", reg.bits.ipriority_id49);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id50\n", reg.bits.ipriority_id50);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id51\n", reg.bits.ipriority_id51);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr13(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr13_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id52\n", reg.bits.ipriority_id52);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id53\n", reg.bits.ipriority_id53);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id54\n", reg.bits.ipriority_id54);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id55\n", reg.bits.ipriority_id55);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr14(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr14_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id56\n", reg.bits.ipriority_id56);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id57\n", reg.bits.ipriority_id57);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id58\n", reg.bits.ipriority_id58);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id59\n", reg.bits.ipriority_id59);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr15(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr15_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id60\n", reg.bits.ipriority_id60);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id61\n", reg.bits.ipriority_id61);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id62\n", reg.bits.ipriority_id62);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id63\n", reg.bits.ipriority_id63);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr16(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr16_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id64\n", reg.bits.ipriority_id64);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id65\n", reg.bits.ipriority_id65);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id66\n", reg.bits.ipriority_id66);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id67\n", reg.bits.ipriority_id67);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr17(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr17_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id68\n", reg.bits.ipriority_id68);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id69\n", reg.bits.ipriority_id69);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id70\n", reg.bits.ipriority_id70);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id71\n", reg.bits.ipriority_id71);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr18(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr18_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id72\n", reg.bits.ipriority_id72);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id73\n", reg.bits.ipriority_id73);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id74\n", reg.bits.ipriority_id74);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id75\n", reg.bits.ipriority_id75);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr19(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr19_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id76\n", reg.bits.ipriority_id76);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id77\n", reg.bits.ipriority_id77);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id78\n", reg.bits.ipriority_id78);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id79\n", reg.bits.ipriority_id79);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr20(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr20_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id80\n", reg.bits.ipriority_id80);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id81\n", reg.bits.ipriority_id81);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id82\n", reg.bits.ipriority_id82);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id83\n", reg.bits.ipriority_id83);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr21(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr21_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id84\n", reg.bits.ipriority_id84);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id85\n", reg.bits.ipriority_id85);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id86\n", reg.bits.ipriority_id86);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id87\n", reg.bits.ipriority_id87);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr22(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr22_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id88\n", reg.bits.ipriority_id88);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id89\n", reg.bits.ipriority_id89);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id90\n", reg.bits.ipriority_id90);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id91\n", reg.bits.ipriority_id91);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr23(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr23_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id92\n", reg.bits.ipriority_id92);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id93\n", reg.bits.ipriority_id93);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id94\n", reg.bits.ipriority_id94);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id95\n", reg.bits.ipriority_id95);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr24(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr24_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id96\n", reg.bits.ipriority_id96);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id97\n", reg.bits.ipriority_id97);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id98\n", reg.bits.ipriority_id98);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id99\n", reg.bits.ipriority_id99);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr25(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr25_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id100\n", reg.bits.ipriority_id100);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id101\n", reg.bits.ipriority_id101);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id102\n", reg.bits.ipriority_id102);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id103\n", reg.bits.ipriority_id103);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr26(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr26_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id104\n", reg.bits.ipriority_id104);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id105\n", reg.bits.ipriority_id105);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id106\n", reg.bits.ipriority_id106);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id107\n", reg.bits.ipriority_id107);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr27(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr27_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id108\n", reg.bits.ipriority_id108);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id109\n", reg.bits.ipriority_id109);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id110\n", reg.bits.ipriority_id110);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id111\n", reg.bits.ipriority_id111);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr28(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr28_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id112\n", reg.bits.ipriority_id112);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id113\n", reg.bits.ipriority_id113);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id114\n", reg.bits.ipriority_id114);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id115\n", reg.bits.ipriority_id115);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr29(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr29_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id116\n", reg.bits.ipriority_id116);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id117\n", reg.bits.ipriority_id117);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id118\n", reg.bits.ipriority_id118);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id119\n", reg.bits.ipriority_id119);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr30(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr30_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id120\n", reg.bits.ipriority_id120);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id121\n", reg.bits.ipriority_id121);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id122\n", reg.bits.ipriority_id122);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id123\n", reg.bits.ipriority_id123);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr31(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr31_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id124\n", reg.bits.ipriority_id124);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id125\n", reg.bits.ipriority_id125);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id126\n", reg.bits.ipriority_id126);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id127\n", reg.bits.ipriority_id127);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr32(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr32_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id128\n", reg.bits.ipriority_id128);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id129\n", reg.bits.ipriority_id129);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id130\n", reg.bits.ipriority_id130);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id131\n", reg.bits.ipriority_id131);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr33(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr33_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id132\n", reg.bits.ipriority_id132);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id133\n", reg.bits.ipriority_id133);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id134\n", reg.bits.ipriority_id134);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id135\n", reg.bits.ipriority_id135);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr34(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr34_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id136\n", reg.bits.ipriority_id136);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id137\n", reg.bits.ipriority_id137);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id138\n", reg.bits.ipriority_id138);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id139\n", reg.bits.ipriority_id139);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr35(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr35_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id140\n", reg.bits.ipriority_id140);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id141\n", reg.bits.ipriority_id141);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id142\n", reg.bits.ipriority_id142);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id143\n", reg.bits.ipriority_id143);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr36(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr36_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id144\n", reg.bits.ipriority_id144);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id145\n", reg.bits.ipriority_id145);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id146\n", reg.bits.ipriority_id146);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id147\n", reg.bits.ipriority_id147);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr37(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr37_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id148\n", reg.bits.ipriority_id148);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id149\n", reg.bits.ipriority_id149);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id150\n", reg.bits.ipriority_id150);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id151\n", reg.bits.ipriority_id151);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr38(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr38_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id152\n", reg.bits.ipriority_id152);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id153\n", reg.bits.ipriority_id153);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id154\n", reg.bits.ipriority_id154);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id155\n", reg.bits.ipriority_id155);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr39(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr39_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id156\n", reg.bits.ipriority_id156);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id157\n", reg.bits.ipriority_id157);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id158\n", reg.bits.ipriority_id158);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id159\n", reg.bits.ipriority_id159);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr40(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr40_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id160\n", reg.bits.ipriority_id160);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id161\n", reg.bits.ipriority_id161);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id162\n", reg.bits.ipriority_id162);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id163\n", reg.bits.ipriority_id163);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr41(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr41_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id164\n", reg.bits.ipriority_id164);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id165\n", reg.bits.ipriority_id165);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id166\n", reg.bits.ipriority_id166);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id167\n", reg.bits.ipriority_id167);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr42(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr42_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id168\n", reg.bits.ipriority_id168);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id169\n", reg.bits.ipriority_id169);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id170\n", reg.bits.ipriority_id170);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id171\n", reg.bits.ipriority_id171);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr43(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr43_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id172\n", reg.bits.ipriority_id172);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id173\n", reg.bits.ipriority_id173);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id174\n", reg.bits.ipriority_id174);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id175\n", reg.bits.ipriority_id175);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr44(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr44_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id176\n", reg.bits.ipriority_id176);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id177\n", reg.bits.ipriority_id177);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id178\n", reg.bits.ipriority_id178);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id179\n", reg.bits.ipriority_id179);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr45(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr45_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id180\n", reg.bits.ipriority_id180);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id181\n", reg.bits.ipriority_id181);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id182\n", reg.bits.ipriority_id182);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id183\n", reg.bits.ipriority_id183);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr46(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr46_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id184\n", reg.bits.ipriority_id184);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id185\n", reg.bits.ipriority_id185);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id186\n", reg.bits.ipriority_id186);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id187\n", reg.bits.ipriority_id187);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr47(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr47_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id188\n", reg.bits.ipriority_id188);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id189\n", reg.bits.ipriority_id189);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id190\n", reg.bits.ipriority_id190);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id191\n", reg.bits.ipriority_id191);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr48(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr48_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id192\n", reg.bits.ipriority_id192);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id193\n", reg.bits.ipriority_id193);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id194\n", reg.bits.ipriority_id194);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id195\n", reg.bits.ipriority_id195);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr49(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr49_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id196\n", reg.bits.ipriority_id196);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id197\n", reg.bits.ipriority_id197);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id198\n", reg.bits.ipriority_id198);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id199\n", reg.bits.ipriority_id199);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr50(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr50_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id200\n", reg.bits.ipriority_id200);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id201\n", reg.bits.ipriority_id201);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id202\n", reg.bits.ipriority_id202);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id203\n", reg.bits.ipriority_id203);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr51(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr51_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id204\n", reg.bits.ipriority_id204);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id205\n", reg.bits.ipriority_id205);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id206\n", reg.bits.ipriority_id206);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id207\n", reg.bits.ipriority_id207);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr52(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr52_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id208\n", reg.bits.ipriority_id208);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id209\n", reg.bits.ipriority_id209);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id210\n", reg.bits.ipriority_id210);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id211\n", reg.bits.ipriority_id211);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr53(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr53_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id212\n", reg.bits.ipriority_id212);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id213\n", reg.bits.ipriority_id213);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id214\n", reg.bits.ipriority_id214);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id215\n", reg.bits.ipriority_id215);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr54(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr54_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id216\n", reg.bits.ipriority_id216);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id217\n", reg.bits.ipriority_id217);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id218\n", reg.bits.ipriority_id218);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id219\n", reg.bits.ipriority_id219);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_ipriorityr55(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_ipriorityr55_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id220\n", reg.bits.ipriority_id220);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id221\n", reg.bits.ipriority_id221);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id222\n", reg.bits.ipriority_id222);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id223\n", reg.bits.ipriority_id223);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id0\n", reg.bits.ipriority_id0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id1\n", reg.bits.ipriority_id1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id2\n", reg.bits.ipriority_id2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id3\n", reg.bits.ipriority_id3);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr1_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id4\n", reg.bits.ipriority_id4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id5\n", reg.bits.ipriority_id5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id6\n", reg.bits.ipriority_id6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id7\n", reg.bits.ipriority_id7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr2_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id8\n", reg.bits.ipriority_id8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id9\n", reg.bits.ipriority_id9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id10\n", reg.bits.ipriority_id10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id11\n", reg.bits.ipriority_id11);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr3(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr3_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id12\n", reg.bits.ipriority_id12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id13\n", reg.bits.ipriority_id13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id14\n", reg.bits.ipriority_id14);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id15\n", reg.bits.ipriority_id15);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr4(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr4_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id16\n", reg.bits.ipriority_id16);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id17\n", reg.bits.ipriority_id17);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id18\n", reg.bits.ipriority_id18);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id19\n", reg.bits.ipriority_id19);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr5(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr5_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id20\n", reg.bits.ipriority_id20);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id21\n", reg.bits.ipriority_id21);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id22\n", reg.bits.ipriority_id22);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id23\n", reg.bits.ipriority_id23);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr6(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr6_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id24\n", reg.bits.ipriority_id24);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id25\n", reg.bits.ipriority_id25);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id26\n", reg.bits.ipriority_id26);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id27\n", reg.bits.ipriority_id27);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr7(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr7_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id28\n", reg.bits.ipriority_id28);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id29\n", reg.bits.ipriority_id29);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id30\n", reg.bits.ipriority_id30);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id31\n", reg.bits.ipriority_id31);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr8(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr8_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id32\n", reg.bits.ipriority_id32);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id33\n", reg.bits.ipriority_id33);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id34\n", reg.bits.ipriority_id34);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id35\n", reg.bits.ipriority_id35);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr9(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr9_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id36\n", reg.bits.ipriority_id36);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id37\n", reg.bits.ipriority_id37);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id38\n", reg.bits.ipriority_id38);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id39\n", reg.bits.ipriority_id39);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr10(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr10_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id40\n", reg.bits.ipriority_id40);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id41\n", reg.bits.ipriority_id41);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id42\n", reg.bits.ipriority_id42);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id43\n", reg.bits.ipriority_id43);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr11(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr11_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id44\n", reg.bits.ipriority_id44);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id45\n", reg.bits.ipriority_id45);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id46\n", reg.bits.ipriority_id46);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id47\n", reg.bits.ipriority_id47);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr12(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr12_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id48\n", reg.bits.ipriority_id48);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id49\n", reg.bits.ipriority_id49);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id50\n", reg.bits.ipriority_id50);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id51\n", reg.bits.ipriority_id51);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr13(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr13_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id52\n", reg.bits.ipriority_id52);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id53\n", reg.bits.ipriority_id53);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id54\n", reg.bits.ipriority_id54);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id55\n", reg.bits.ipriority_id55);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr14(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr14_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id56\n", reg.bits.ipriority_id56);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id57\n", reg.bits.ipriority_id57);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id58\n", reg.bits.ipriority_id58);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id59\n", reg.bits.ipriority_id59);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr15(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr15_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id60\n", reg.bits.ipriority_id60);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id61\n", reg.bits.ipriority_id61);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id62\n", reg.bits.ipriority_id62);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id63\n", reg.bits.ipriority_id63);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr16(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr16_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id64\n", reg.bits.ipriority_id64);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id65\n", reg.bits.ipriority_id65);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id66\n", reg.bits.ipriority_id66);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id67\n", reg.bits.ipriority_id67);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr17(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr17_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id68\n", reg.bits.ipriority_id68);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id69\n", reg.bits.ipriority_id69);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id70\n", reg.bits.ipriority_id70);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id71\n", reg.bits.ipriority_id71);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr18(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr18_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id72\n", reg.bits.ipriority_id72);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id73\n", reg.bits.ipriority_id73);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id74\n", reg.bits.ipriority_id74);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id75\n", reg.bits.ipriority_id75);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr19(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr19_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id76\n", reg.bits.ipriority_id76);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id77\n", reg.bits.ipriority_id77);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id78\n", reg.bits.ipriority_id78);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id79\n", reg.bits.ipriority_id79);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr20(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr20_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id80\n", reg.bits.ipriority_id80);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id81\n", reg.bits.ipriority_id81);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id82\n", reg.bits.ipriority_id82);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id83\n", reg.bits.ipriority_id83);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr21(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr21_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id84\n", reg.bits.ipriority_id84);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id85\n", reg.bits.ipriority_id85);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id86\n", reg.bits.ipriority_id86);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id87\n", reg.bits.ipriority_id87);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr22(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr22_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id88\n", reg.bits.ipriority_id88);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id89\n", reg.bits.ipriority_id89);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id90\n", reg.bits.ipriority_id90);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id91\n", reg.bits.ipriority_id91);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr23(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr23_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id92\n", reg.bits.ipriority_id92);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id93\n", reg.bits.ipriority_id93);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id94\n", reg.bits.ipriority_id94);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id95\n", reg.bits.ipriority_id95);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr24(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr24_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id96\n", reg.bits.ipriority_id96);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id97\n", reg.bits.ipriority_id97);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id98\n", reg.bits.ipriority_id98);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id99\n", reg.bits.ipriority_id99);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr25(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr25_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id100\n", reg.bits.ipriority_id100);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id101\n", reg.bits.ipriority_id101);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id102\n", reg.bits.ipriority_id102);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id103\n", reg.bits.ipriority_id103);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr26(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr26_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id104\n", reg.bits.ipriority_id104);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id105\n", reg.bits.ipriority_id105);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id106\n", reg.bits.ipriority_id106);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id107\n", reg.bits.ipriority_id107);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr27(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr27_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id108\n", reg.bits.ipriority_id108);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id109\n", reg.bits.ipriority_id109);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id110\n", reg.bits.ipriority_id110);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id111\n", reg.bits.ipriority_id111);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr28(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr28_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id112\n", reg.bits.ipriority_id112);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id113\n", reg.bits.ipriority_id113);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id114\n", reg.bits.ipriority_id114);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id115\n", reg.bits.ipriority_id115);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr29(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr29_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id116\n", reg.bits.ipriority_id116);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id117\n", reg.bits.ipriority_id117);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id118\n", reg.bits.ipriority_id118);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id119\n", reg.bits.ipriority_id119);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr30(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr30_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id120\n", reg.bits.ipriority_id120);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id121\n", reg.bits.ipriority_id121);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id122\n", reg.bits.ipriority_id122);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id123\n", reg.bits.ipriority_id123);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr31(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr31_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id124\n", reg.bits.ipriority_id124);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id125\n", reg.bits.ipriority_id125);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id126\n", reg.bits.ipriority_id126);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id127\n", reg.bits.ipriority_id127);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr32(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr32_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id128\n", reg.bits.ipriority_id128);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id129\n", reg.bits.ipriority_id129);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id130\n", reg.bits.ipriority_id130);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id131\n", reg.bits.ipriority_id131);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr33(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr33_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id132\n", reg.bits.ipriority_id132);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id133\n", reg.bits.ipriority_id133);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id134\n", reg.bits.ipriority_id134);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id135\n", reg.bits.ipriority_id135);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr34(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr34_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id136\n", reg.bits.ipriority_id136);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id137\n", reg.bits.ipriority_id137);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id138\n", reg.bits.ipriority_id138);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id139\n", reg.bits.ipriority_id139);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr35(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr35_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id140\n", reg.bits.ipriority_id140);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id141\n", reg.bits.ipriority_id141);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id142\n", reg.bits.ipriority_id142);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id143\n", reg.bits.ipriority_id143);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr36(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr36_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id144\n", reg.bits.ipriority_id144);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id145\n", reg.bits.ipriority_id145);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id146\n", reg.bits.ipriority_id146);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id147\n", reg.bits.ipriority_id147);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr37(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr37_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id148\n", reg.bits.ipriority_id148);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id149\n", reg.bits.ipriority_id149);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id150\n", reg.bits.ipriority_id150);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id151\n", reg.bits.ipriority_id151);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr38(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr38_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id152\n", reg.bits.ipriority_id152);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id153\n", reg.bits.ipriority_id153);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id154\n", reg.bits.ipriority_id154);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id155\n", reg.bits.ipriority_id155);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr39(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr39_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id156\n", reg.bits.ipriority_id156);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id157\n", reg.bits.ipriority_id157);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id158\n", reg.bits.ipriority_id158);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id159\n", reg.bits.ipriority_id159);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr40(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr40_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id160\n", reg.bits.ipriority_id160);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id161\n", reg.bits.ipriority_id161);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id162\n", reg.bits.ipriority_id162);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id163\n", reg.bits.ipriority_id163);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr41(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr41_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id164\n", reg.bits.ipriority_id164);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id165\n", reg.bits.ipriority_id165);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id166\n", reg.bits.ipriority_id166);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id167\n", reg.bits.ipriority_id167);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr42(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr42_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id168\n", reg.bits.ipriority_id168);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id169\n", reg.bits.ipriority_id169);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id170\n", reg.bits.ipriority_id170);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id171\n", reg.bits.ipriority_id171);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr43(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr43_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id172\n", reg.bits.ipriority_id172);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id173\n", reg.bits.ipriority_id173);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id174\n", reg.bits.ipriority_id174);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id175\n", reg.bits.ipriority_id175);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr44(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr44_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id176\n", reg.bits.ipriority_id176);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id177\n", reg.bits.ipriority_id177);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id178\n", reg.bits.ipriority_id178);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id179\n", reg.bits.ipriority_id179);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr45(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr45_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id180\n", reg.bits.ipriority_id180);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id181\n", reg.bits.ipriority_id181);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id182\n", reg.bits.ipriority_id182);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id183\n", reg.bits.ipriority_id183);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr46(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr46_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id184\n", reg.bits.ipriority_id184);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id185\n", reg.bits.ipriority_id185);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id186\n", reg.bits.ipriority_id186);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id187\n", reg.bits.ipriority_id187);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr47(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr47_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id188\n", reg.bits.ipriority_id188);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id189\n", reg.bits.ipriority_id189);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id190\n", reg.bits.ipriority_id190);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id191\n", reg.bits.ipriority_id191);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr48(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr48_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id192\n", reg.bits.ipriority_id192);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id193\n", reg.bits.ipriority_id193);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id194\n", reg.bits.ipriority_id194);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id195\n", reg.bits.ipriority_id195);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr49(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr49_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id196\n", reg.bits.ipriority_id196);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id197\n", reg.bits.ipriority_id197);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id198\n", reg.bits.ipriority_id198);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id199\n", reg.bits.ipriority_id199);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr50(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr50_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id200\n", reg.bits.ipriority_id200);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id201\n", reg.bits.ipriority_id201);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id202\n", reg.bits.ipriority_id202);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id203\n", reg.bits.ipriority_id203);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr51(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr51_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id204\n", reg.bits.ipriority_id204);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id205\n", reg.bits.ipriority_id205);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id206\n", reg.bits.ipriority_id206);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id207\n", reg.bits.ipriority_id207);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr52(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr52_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id208\n", reg.bits.ipriority_id208);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id209\n", reg.bits.ipriority_id209);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id210\n", reg.bits.ipriority_id210);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id211\n", reg.bits.ipriority_id211);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr53(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr53_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id212\n", reg.bits.ipriority_id212);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id213\n", reg.bits.ipriority_id213);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id214\n", reg.bits.ipriority_id214);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id215\n", reg.bits.ipriority_id215);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr54(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr54_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id216\n", reg.bits.ipriority_id216);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id217\n", reg.bits.ipriority_id217);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id218\n", reg.bits.ipriority_id218);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id219\n", reg.bits.ipriority_id219);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_itargetsr55(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_itargetsr55_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:ipriority_id220\n", reg.bits.ipriority_id220);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id221\n", reg.bits.ipriority_id221);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id222\n", reg.bits.ipriority_id222);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ipriority_id223\n", reg.bits.ipriority_id223);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icfgr0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icfgr0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:cfg_id0\n", reg.bits.cfg_id0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id1\n", reg.bits.cfg_id1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id2\n", reg.bits.cfg_id2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id3\n", reg.bits.cfg_id3);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id4\n", reg.bits.cfg_id4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id5\n", reg.bits.cfg_id5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id6\n", reg.bits.cfg_id6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id7\n", reg.bits.cfg_id7);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id8\n", reg.bits.cfg_id8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id9\n", reg.bits.cfg_id9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id10\n", reg.bits.cfg_id10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id11\n", reg.bits.cfg_id11);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id12\n", reg.bits.cfg_id12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id13\n", reg.bits.cfg_id13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id14\n", reg.bits.cfg_id14);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id15\n", reg.bits.cfg_id15);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icfgr1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icfgr1_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:cfg_id16\n", reg.bits.cfg_id16);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id17\n", reg.bits.cfg_id17);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id18\n", reg.bits.cfg_id18);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id19\n", reg.bits.cfg_id19);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id20\n", reg.bits.cfg_id20);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id21\n", reg.bits.cfg_id21);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id22\n", reg.bits.cfg_id22);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id23\n", reg.bits.cfg_id23);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id24\n", reg.bits.cfg_id24);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id25\n", reg.bits.cfg_id25);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id26\n", reg.bits.cfg_id26);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id27\n", reg.bits.cfg_id27);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id28\n", reg.bits.cfg_id28);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id29\n", reg.bits.cfg_id29);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id30\n", reg.bits.cfg_id30);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id31\n", reg.bits.cfg_id31);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icfgr2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icfgr2_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:cfg_id32\n", reg.bits.cfg_id32);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id33\n", reg.bits.cfg_id33);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id34\n", reg.bits.cfg_id34);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id35\n", reg.bits.cfg_id35);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id36\n", reg.bits.cfg_id36);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id37\n", reg.bits.cfg_id37);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id38\n", reg.bits.cfg_id38);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id39\n", reg.bits.cfg_id39);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id40\n", reg.bits.cfg_id40);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id41\n", reg.bits.cfg_id41);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id42\n", reg.bits.cfg_id42);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id43\n", reg.bits.cfg_id43);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id44\n", reg.bits.cfg_id44);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id45\n", reg.bits.cfg_id45);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id46\n", reg.bits.cfg_id46);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id47\n", reg.bits.cfg_id47);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icfgr3(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icfgr3_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:cfg_id48\n", reg.bits.cfg_id48);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id49\n", reg.bits.cfg_id49);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id50\n", reg.bits.cfg_id50);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id51\n", reg.bits.cfg_id51);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id52\n", reg.bits.cfg_id52);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id53\n", reg.bits.cfg_id53);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id54\n", reg.bits.cfg_id54);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id55\n", reg.bits.cfg_id55);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id56\n", reg.bits.cfg_id56);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id57\n", reg.bits.cfg_id57);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id58\n", reg.bits.cfg_id58);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id59\n", reg.bits.cfg_id59);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id60\n", reg.bits.cfg_id60);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id61\n", reg.bits.cfg_id61);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id62\n", reg.bits.cfg_id62);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id63\n", reg.bits.cfg_id63);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icfgr4(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icfgr4_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:cfg_id64\n", reg.bits.cfg_id64);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id65\n", reg.bits.cfg_id65);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id66\n", reg.bits.cfg_id66);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id67\n", reg.bits.cfg_id67);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id68\n", reg.bits.cfg_id68);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id69\n", reg.bits.cfg_id69);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id70\n", reg.bits.cfg_id70);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id71\n", reg.bits.cfg_id71);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id72\n", reg.bits.cfg_id72);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id73\n", reg.bits.cfg_id73);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id74\n", reg.bits.cfg_id74);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id75\n", reg.bits.cfg_id75);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id76\n", reg.bits.cfg_id76);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id77\n", reg.bits.cfg_id77);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id78\n", reg.bits.cfg_id78);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id79\n", reg.bits.cfg_id79);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icfgr5(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icfgr5_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:cfg_id80\n", reg.bits.cfg_id80);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id81\n", reg.bits.cfg_id81);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id82\n", reg.bits.cfg_id82);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id83\n", reg.bits.cfg_id83);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id84\n", reg.bits.cfg_id84);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id85\n", reg.bits.cfg_id85);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id86\n", reg.bits.cfg_id86);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id87\n", reg.bits.cfg_id87);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id88\n", reg.bits.cfg_id88);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id89\n", reg.bits.cfg_id89);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id90\n", reg.bits.cfg_id90);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id91\n", reg.bits.cfg_id91);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id92\n", reg.bits.cfg_id92);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id93\n", reg.bits.cfg_id93);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id94\n", reg.bits.cfg_id94);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id95\n", reg.bits.cfg_id95);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icfgr6(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icfgr6_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:cfg_id96\n", reg.bits.cfg_id96);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id97\n", reg.bits.cfg_id97);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id98\n", reg.bits.cfg_id98);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id99\n", reg.bits.cfg_id99);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id100\n", reg.bits.cfg_id100);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id101\n", reg.bits.cfg_id101);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id102\n", reg.bits.cfg_id102);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id103\n", reg.bits.cfg_id103);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id104\n", reg.bits.cfg_id104);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id105\n", reg.bits.cfg_id105);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id106\n", reg.bits.cfg_id106);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id107\n", reg.bits.cfg_id107);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id108\n", reg.bits.cfg_id108);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id109\n", reg.bits.cfg_id109);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id110\n", reg.bits.cfg_id110);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id111\n", reg.bits.cfg_id111);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icfgr7(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icfgr7_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:cfg_id112\n", reg.bits.cfg_id112);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id113\n", reg.bits.cfg_id113);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id114\n", reg.bits.cfg_id114);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id115\n", reg.bits.cfg_id115);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id116\n", reg.bits.cfg_id116);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id117\n", reg.bits.cfg_id117);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id118\n", reg.bits.cfg_id118);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id119\n", reg.bits.cfg_id119);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id120\n", reg.bits.cfg_id120);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id121\n", reg.bits.cfg_id121);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id122\n", reg.bits.cfg_id122);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id123\n", reg.bits.cfg_id123);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id124\n", reg.bits.cfg_id124);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id125\n", reg.bits.cfg_id125);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id126\n", reg.bits.cfg_id126);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id127\n", reg.bits.cfg_id127);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icfgr8(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icfgr8_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:cfg_id128\n", reg.bits.cfg_id128);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id129\n", reg.bits.cfg_id129);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id130\n", reg.bits.cfg_id130);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id131\n", reg.bits.cfg_id131);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id132\n", reg.bits.cfg_id132);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id133\n", reg.bits.cfg_id133);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id134\n", reg.bits.cfg_id134);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id135\n", reg.bits.cfg_id135);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id136\n", reg.bits.cfg_id136);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id137\n", reg.bits.cfg_id137);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id138\n", reg.bits.cfg_id138);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id139\n", reg.bits.cfg_id139);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id140\n", reg.bits.cfg_id140);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id141\n", reg.bits.cfg_id141);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id142\n", reg.bits.cfg_id142);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id143\n", reg.bits.cfg_id143);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icfgr9(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icfgr9_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:cfg_id144\n", reg.bits.cfg_id144);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id145\n", reg.bits.cfg_id145);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id146\n", reg.bits.cfg_id146);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id147\n", reg.bits.cfg_id147);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id148\n", reg.bits.cfg_id148);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id149\n", reg.bits.cfg_id149);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id150\n", reg.bits.cfg_id150);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id151\n", reg.bits.cfg_id151);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id152\n", reg.bits.cfg_id152);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id153\n", reg.bits.cfg_id153);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id154\n", reg.bits.cfg_id154);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id155\n", reg.bits.cfg_id155);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id156\n", reg.bits.cfg_id156);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id157\n", reg.bits.cfg_id157);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id158\n", reg.bits.cfg_id158);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id159\n", reg.bits.cfg_id159);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icfgr10(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icfgr10_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:cfg_id160\n", reg.bits.cfg_id160);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id161\n", reg.bits.cfg_id161);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id162\n", reg.bits.cfg_id162);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id163\n", reg.bits.cfg_id163);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id164\n", reg.bits.cfg_id164);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id165\n", reg.bits.cfg_id165);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id166\n", reg.bits.cfg_id166);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id167\n", reg.bits.cfg_id167);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id168\n", reg.bits.cfg_id168);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id169\n", reg.bits.cfg_id169);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id170\n", reg.bits.cfg_id170);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id171\n", reg.bits.cfg_id171);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id172\n", reg.bits.cfg_id172);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id173\n", reg.bits.cfg_id173);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id174\n", reg.bits.cfg_id174);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id175\n", reg.bits.cfg_id175);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icfgr11(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icfgr11_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:cfg_id176\n", reg.bits.cfg_id176);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id177\n", reg.bits.cfg_id177);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id178\n", reg.bits.cfg_id178);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id179\n", reg.bits.cfg_id179);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id180\n", reg.bits.cfg_id180);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id181\n", reg.bits.cfg_id181);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id182\n", reg.bits.cfg_id182);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id183\n", reg.bits.cfg_id183);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id184\n", reg.bits.cfg_id184);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id185\n", reg.bits.cfg_id185);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id186\n", reg.bits.cfg_id186);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id187\n", reg.bits.cfg_id187);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id188\n", reg.bits.cfg_id188);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id189\n", reg.bits.cfg_id189);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id190\n", reg.bits.cfg_id190);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id191\n", reg.bits.cfg_id191);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icfgr12(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icfgr12_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:cfg_id192\n", reg.bits.cfg_id192);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id193\n", reg.bits.cfg_id193);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id194\n", reg.bits.cfg_id194);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id195\n", reg.bits.cfg_id195);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id196\n", reg.bits.cfg_id196);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id197\n", reg.bits.cfg_id197);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id198\n", reg.bits.cfg_id198);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id199\n", reg.bits.cfg_id199);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id200\n", reg.bits.cfg_id200);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id201\n", reg.bits.cfg_id201);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id202\n", reg.bits.cfg_id202);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id203\n", reg.bits.cfg_id203);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id204\n", reg.bits.cfg_id204);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id205\n", reg.bits.cfg_id205);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id206\n", reg.bits.cfg_id206);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id207\n", reg.bits.cfg_id207);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_icfgr13(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_icfgr13_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:cfg_id208\n", reg.bits.cfg_id208);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id209\n", reg.bits.cfg_id209);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id210\n", reg.bits.cfg_id210);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id211\n", reg.bits.cfg_id211);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id212\n", reg.bits.cfg_id212);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id213\n", reg.bits.cfg_id213);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id214\n", reg.bits.cfg_id214);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id215\n", reg.bits.cfg_id215);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id216\n", reg.bits.cfg_id216);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id217\n", reg.bits.cfg_id217);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id218\n", reg.bits.cfg_id218);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id219\n", reg.bits.cfg_id219);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id220\n", reg.bits.cfg_id220);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id221\n", reg.bits.cfg_id221);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id222\n", reg.bits.cfg_id222);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cfg_id223\n", reg.bits.cfg_id223);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_sgir(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_sgir_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:sgiintid\n", reg.bits.sgiintid);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:nsatt\n", reg.bits.nsatt);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cputargetlist\n", reg.bits.cputargetlist);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:targetlistfilter\n", reg.bits.targetlistfilter);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_cpendsgir0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_cpendsgir0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clr_pend_id0\n", reg.bits.clr_pend_id0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id1\n", reg.bits.clr_pend_id1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id2\n", reg.bits.clr_pend_id2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id3\n", reg.bits.clr_pend_id3);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_cpendsgir1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_cpendsgir1_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clr_pend_id4\n", reg.bits.clr_pend_id4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id5\n", reg.bits.clr_pend_id5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id6\n", reg.bits.clr_pend_id6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id7\n", reg.bits.clr_pend_id7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_cpendsgir2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_cpendsgir2_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clr_pend_id8\n", reg.bits.clr_pend_id8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id9\n", reg.bits.clr_pend_id9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id10\n", reg.bits.clr_pend_id10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id11\n", reg.bits.clr_pend_id11);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_cpendsgir3(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_cpendsgir3_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:clr_pend_id12\n", reg.bits.clr_pend_id12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id13\n", reg.bits.clr_pend_id13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id14\n", reg.bits.clr_pend_id14);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clr_pend_id15\n", reg.bits.clr_pend_id15);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_spendsgir0(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_spendsgir0_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_pend_id0\n", reg.bits.set_pend_id0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id1\n", reg.bits.set_pend_id1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id2\n", reg.bits.set_pend_id2);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id3\n", reg.bits.set_pend_id3);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_spendsgir1(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_spendsgir1_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_pend_id4\n", reg.bits.set_pend_id4);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id5\n", reg.bits.set_pend_id5);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id6\n", reg.bits.set_pend_id6);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id7\n", reg.bits.set_pend_id7);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_spendsgir2(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_spendsgir2_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_pend_id8\n", reg.bits.set_pend_id8);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id9\n", reg.bits.set_pend_id9);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id10\n", reg.bits.set_pend_id10);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id11\n", reg.bits.set_pend_id11);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicd_spendsgir3(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicd_spendsgir3_t reg;

	def_memset((void *)&reg, 0, sizeof(reg));
	def_assert(pout != NULL);

	switch (mode) {
	case DUMP_TYPE_DATA:
		reg.val = data;
		break;
	case DUMP_TYPE_READ:
		reg.val = READREG32(addr);
		break;
	case DUMP_TYPE_WRITE:
		WRITEREG32(addr, data);
		reg.val = READREG32(addr);
		break;
	}

	def_sprintf(sz_temp, "0x%X:set_pend_id12\n", reg.bits.set_pend_id12);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id13\n", reg.bits.set_pend_id13);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id14\n", reg.bits.set_pend_id14);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:set_pend_id15\n", reg.bits.set_pend_id15);
	def_strcat(pout, sz_temp);

	return 0;
}

#endif /* ___GICD___DUMP__C___ */
