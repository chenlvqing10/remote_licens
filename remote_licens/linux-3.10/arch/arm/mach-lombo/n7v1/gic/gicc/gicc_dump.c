/* gicc_dump.c */

#ifndef ___GICC___DUMP__C___
#define ___GICC___DUMP__C___

reg_info_t gicc_info[] = {
	{VA_GICC_CTLR, DATA_GICC_CTLR, dump_gicc_ctlr, "GICC_CTLR", 0},
	{VA_GICC_PMR, DATA_GICC_PMR, dump_gicc_pmr, "GICC_PMR", 0},
	{VA_GICC_BPR, DATA_GICC_BPR, dump_gicc_bpr, "GICC_BPR", 0},
	{VA_GICC_IAR, DATA_GICC_IAR, dump_gicc_iar, "GICC_IAR", 0},
	{VA_GICC_EOIR, DATA_GICC_EOIR, dump_gicc_eoir, "GICC_EOIR", 0},
	{VA_GICC_RPR, DATA_GICC_RPR, dump_gicc_rpr, "GICC_RPR", 0},
	{VA_GICC_HPPIR, DATA_GICC_HPPIR, dump_gicc_hppir, "GICC_HPPIR", 0},
	{VA_GICC_ABPR, DATA_GICC_ABPR, dump_gicc_abpr, "GICC_ABPR", 0},
	{VA_GICC_AIAR, DATA_GICC_AIAR, dump_gicc_aiar, "GICC_AIAR", 0},
	{VA_GICC_AEOIR, DATA_GICC_AEOIR, dump_gicc_aeoir, "GICC_AEOIR", 0},
	{VA_GICC_AHPPIR, DATA_GICC_AHPPIR, dump_gicc_ahppir, "GICC_AHPPIR", 0},
	{VA_GICC_DIR, DATA_GICC_DIR, dump_gicc_dir, "GICC_DIR", 0},
};

void dump_gicc(u32 mode)
{
	u32 cnt = sizeof(gicc_info) / sizeof(gicc_info[0]);
	u32 i = 0;
	char sz_temp[TMP_STR_LEN];
	char buffer[DUMP_BUF_LEN];

	def_memset(buffer, 0, DUMP_BUF_LEN);

	for (i = 0; i < cnt; i++) {
		def_memset(buffer, 0, DUMP_BUF_LEN);
		def_sprintf(sz_temp, "[%d]%s:[%08X]%08X\n",
					i,
					gicc_info[i].name,
					gicc_info[i].addr,
					gicc_info[i].reset);
		def_strcat(buffer, sz_temp);

		gicc_info[i].dump(gicc_info[i].addr, gicc_info[i].reset, mode, buffer);
		msg("%s", buffer);
	}
}

u32 dump_gicc_ctlr(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicc_ctlr_t reg;

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
	def_sprintf(sz_temp, "0x%X:ackctl\n", reg.bits.ackctl);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fiqen\n", reg.bits.fiqen);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cbpr\n", reg.bits.cbpr);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fiqbypdisgrp0\n", reg.bits.fiqbypdisgrp0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:irqbypdisgrp0\n", reg.bits.irqbypdisgrp0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:fiqbypdisgrp1\n", reg.bits.fiqbypdisgrp1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:irqbypdisgrp1\n", reg.bits.irqbypdisgrp1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:eoimodes\n", reg.bits.eoimodes);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:eoimodens\n", reg.bits.eoimodens);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicc_pmr(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicc_pmr_t reg;

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

	def_sprintf(sz_temp, "0x%X:priority\n", reg.bits.priority);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicc_bpr(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicc_bpr_t reg;

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

	def_sprintf(sz_temp, "0x%X:binarypoint\n", reg.bits.binarypoint);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicc_iar(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicc_iar_t reg;

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

	def_sprintf(sz_temp, "0x%X:interruptid\n", reg.bits.interruptid);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cpuid\n", reg.bits.cpuid);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicc_eoir(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicc_eoir_t reg;

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

	def_sprintf(sz_temp, "0x%X:eoiintid\n", reg.bits.eoiintid);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cpuid\n", reg.bits.cpuid);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicc_rpr(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicc_rpr_t reg;

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

	def_sprintf(sz_temp, "0x%X:priority\n", reg.bits.priority);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicc_hppir(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicc_hppir_t reg;

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

	def_sprintf(sz_temp, "0x%X:pendintid\n", reg.bits.pendintid);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cpuid\n", reg.bits.cpuid);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicc_abpr(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicc_abpr_t reg;

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

	def_sprintf(sz_temp, "0x%X:binarypoint\n", reg.bits.binarypoint);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicc_aiar(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicc_aiar_t reg;

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

	def_sprintf(sz_temp, "0x%X:interruptid\n", reg.bits.interruptid);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cpuid\n", reg.bits.cpuid);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicc_aeoir(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicc_aeoir_t reg;

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

	def_sprintf(sz_temp, "0x%X:eoiintid\n", reg.bits.eoiintid);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cpuid\n", reg.bits.cpuid);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicc_ahppir(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicc_ahppir_t reg;

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

	def_sprintf(sz_temp, "0x%X:pendintid\n", reg.bits.pendintid);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cpuid\n", reg.bits.cpuid);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gicc_dir(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gicc_dir_t reg;

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

	def_sprintf(sz_temp, "0x%X:pendintid\n", reg.bits.pendintid);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:cpuid\n", reg.bits.cpuid);
	def_strcat(pout, sz_temp);

	return 0;
}

#endif /* ___GICC___DUMP__C___ */
