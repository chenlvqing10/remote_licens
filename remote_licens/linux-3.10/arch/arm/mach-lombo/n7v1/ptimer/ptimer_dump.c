/* ptimer_dump.c */

#ifndef ___PTIMER___DUMP__C___
#define ___PTIMER___DUMP__C___

reg_info_t ptimer_info[] = {
	{VA_PTIMER_PTMR_VER, DATA_PTIMER_PTMR_VER, dump_ptimer_ptmr_ver, "PTIMER_PTMR_VER", 0},
	{VA_PTIMER_PTMR0_STORE_LOW_VAL, DATA_PTIMER_PTMR0_STORE_LOW_VAL, dump_ptimer_ptmr0_store_low_val, "PTIMER_PTMR0_STORE_LOW_VAL", 0},
	{VA_PTIMER_PTMR0_STORE_HIGH_VAL, DATA_PTIMER_PTMR0_STORE_HIGH_VAL, dump_ptimer_ptmr0_store_high_val, "PTIMER_PTMR0_STORE_HIGH_VAL", 0},
	{VA_PTIMER_PTMR0_CUR_LOW_VAL, DATA_PTIMER_PTMR0_CUR_LOW_VAL, dump_ptimer_ptmr0_cur_low_val, "PTIMER_PTMR0_CUR_LOW_VAL", 0},
	{VA_PTIMER_PTMR0_CUR_HIGH_VAL, DATA_PTIMER_PTMR0_CUR_HIGH_VAL, dump_ptimer_ptmr0_cur_high_val, "PTIMER_PTMR0_CUR_HIGH_VAL", 0},
	{VA_PTIMER_PTMR0_CTRL, DATA_PTIMER_PTMR0_CTRL, dump_ptimer_ptmr0_ctrl, "PTIMER_PTMR0_CTRL", 0},
	{VA_PTIMER_PTMR0_INT_EN, DATA_PTIMER_PTMR0_INT_EN, dump_ptimer_ptmr0_int_en, "PTIMER_PTMR0_INT_EN", 0},
	{VA_PTIMER_PTMR0_CLR_INT_PENDING, DATA_PTIMER_PTMR0_CLR_INT_PENDING, dump_ptimer_ptmr0_clr_int_pending, "PTIMER_PTMR0_CLR_INT_PENDING", 0},
	{VA_PTIMER_PTMR0_INT_PENDING, DATA_PTIMER_PTMR0_INT_PENDING, dump_ptimer_ptmr0_int_pending, "PTIMER_PTMR0_INT_PENDING", 0},
	{VA_PTIMER_PTMR1_STORE_LOW_VAL, DATA_PTIMER_PTMR1_STORE_LOW_VAL, dump_ptimer_ptmr1_store_low_val, "PTIMER_PTMR1_STORE_LOW_VAL", 0},
	{VA_PTIMER_PTMR1_STORE_HIGH_VAL, DATA_PTIMER_PTMR1_STORE_HIGH_VAL, dump_ptimer_ptmr1_store_high_val, "PTIMER_PTMR1_STORE_HIGH_VAL", 0},
	{VA_PTIMER_PTMR1_CUR_LOW_VAL, DATA_PTIMER_PTMR1_CUR_LOW_VAL, dump_ptimer_ptmr1_cur_low_val, "PTIMER_PTMR1_CUR_LOW_VAL", 0},
	{VA_PTIMER_PTMR1_CUR_HIGH_VAL, DATA_PTIMER_PTMR1_CUR_HIGH_VAL, dump_ptimer_ptmr1_cur_high_val, "PTIMER_PTMR1_CUR_HIGH_VAL", 0},
	{VA_PTIMER_PTMR1_CTRL, DATA_PTIMER_PTMR1_CTRL, dump_ptimer_ptmr1_ctrl, "PTIMER_PTMR1_CTRL", 0},
	{VA_PTIMER_PTMR1_INT_EN, DATA_PTIMER_PTMR1_INT_EN, dump_ptimer_ptmr1_int_en, "PTIMER_PTMR1_INT_EN", 0},
	{VA_PTIMER_PTMR1_CLR_INT_PENDING, DATA_PTIMER_PTMR1_CLR_INT_PENDING, dump_ptimer_ptmr1_clr_int_pending, "PTIMER_PTMR1_CLR_INT_PENDING", 0},
	{VA_PTIMER_PTMR1_INT_PENDING, DATA_PTIMER_PTMR1_INT_PENDING, dump_ptimer_ptmr1_int_pending, "PTIMER_PTMR1_INT_PENDING", 0},
};

void dump_ptimer(u32 mode)
{
	u32 cnt = sizeof(ptimer_info) / sizeof(ptimer_info[0]);
	u32 i = 0;
	char sz_temp[TMP_STR_LEN];
	char buffer[DUMP_BUF_LEN];

	def_memset(buffer, 0, DUMP_BUF_LEN);

	for (i = 0; i < cnt; i++) {
		def_memset(buffer, 0, DUMP_BUF_LEN);
		def_sprintf(sz_temp, "[%u]%s:[%08X]%08X\n",
					i,
					ptimer_info[i].name,
					ptimer_info[i].addr,
					ptimer_info[i].reset);
		def_strcat(buffer, sz_temp);

		ptimer_info[i].dump(ptimer_info[i].addr, ptimer_info[i].reset, mode, buffer);
		msg("%s", buffer);
	}
}

u32 dump_ptimer_ptmr_ver(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ptimer_ptmr_ver_t reg;

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

	def_sprintf(sz_temp, "0x%X:ver_l\n", reg.bits.ver_l);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ver_h\n", reg.bits.ver_h);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:comp\n", reg.bits.comp);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_ptimer_ptmr0_store_low_val(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ptimer_ptmr0_store_low_val_t reg;

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

	def_sprintf(sz_temp, "0x%X:low\n", reg.bits.low);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_ptimer_ptmr0_store_high_val(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ptimer_ptmr0_store_high_val_t reg;

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

	def_sprintf(sz_temp, "0x%X:high\n", reg.bits.high);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_ptimer_ptmr0_cur_low_val(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ptimer_ptmr0_cur_low_val_t reg;

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

	def_sprintf(sz_temp, "0x%X:low\n", reg.bits.low);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_ptimer_ptmr0_cur_high_val(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ptimer_ptmr0_cur_high_val_t reg;

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

	def_sprintf(sz_temp, "0x%X:high\n", reg.bits.high);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_ptimer_ptmr0_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ptimer_ptmr0_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:enable\n", reg.bits.enable);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:mode\n", reg.bits.mode);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:direction\n", reg.bits.direction);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_ptimer_ptmr0_int_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ptimer_ptmr0_int_en_t reg;

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

	def_sprintf(sz_temp, "0x%X:enable\n", reg.bits.enable);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_ptimer_ptmr0_clr_int_pending(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ptimer_ptmr0_clr_int_pending_t reg;

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

	def_sprintf(sz_temp, "0x%X:clr\n", reg.bits.clr);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_ptimer_ptmr0_int_pending(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ptimer_ptmr0_int_pending_t reg;

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

	def_sprintf(sz_temp, "0x%X:pending\n", reg.bits.pending);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_ptimer_ptmr1_store_low_val(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ptimer_ptmr1_store_low_val_t reg;

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

	def_sprintf(sz_temp, "0x%X:low\n", reg.bits.low);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_ptimer_ptmr1_store_high_val(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ptimer_ptmr1_store_high_val_t reg;

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

	def_sprintf(sz_temp, "0x%X:high\n", reg.bits.high);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_ptimer_ptmr1_cur_low_val(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ptimer_ptmr1_cur_low_val_t reg;

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

	def_sprintf(sz_temp, "0x%X:low\n", reg.bits.low);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_ptimer_ptmr1_cur_high_val(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ptimer_ptmr1_cur_high_val_t reg;

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

	def_sprintf(sz_temp, "0x%X:high\n", reg.bits.high);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_ptimer_ptmr1_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ptimer_ptmr1_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:enable\n", reg.bits.enable);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:mode\n", reg.bits.mode);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:direction\n", reg.bits.direction);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div\n", reg.bits.div);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_ptimer_ptmr1_int_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ptimer_ptmr1_int_en_t reg;

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

	def_sprintf(sz_temp, "0x%X:enable\n", reg.bits.enable);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_ptimer_ptmr1_clr_int_pending(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ptimer_ptmr1_clr_int_pending_t reg;

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

	def_sprintf(sz_temp, "0x%X:clr\n", reg.bits.clr);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_ptimer_ptmr1_int_pending(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ptimer_ptmr1_int_pending_t reg;

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

	def_sprintf(sz_temp, "0x%X:pending\n", reg.bits.pending);
	def_strcat(pout, sz_temp);

	return 0;
}

#endif /* ___PTIMER___DUMP__C___ */
