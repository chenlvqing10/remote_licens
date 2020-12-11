/* gtimer_dump.c */

#ifndef ___GTIMER___DUMP__C___
#define ___GTIMER___DUMP__C___

reg_info_t gtimer_info[] = {
	{VA_GTIMER_GTMR_VER, DATA_GTIMER_GTMR_VER, dump_gtimer_gtmr_ver, "GTIMER_GTMR_VER", 0},
	{VA_GTIMER_GTMR0_CLK_CTRL, DATA_GTIMER_GTMR0_CLK_CTRL, dump_gtimer_gtmr0_clk_ctrl, "GTIMER_GTMR0_CLK_CTRL", 0},
	{VA_GTIMER_GTMR0_STORE_VAL, DATA_GTIMER_GTMR0_STORE_VAL, dump_gtimer_gtmr0_store_val, "GTIMER_GTMR0_STORE_VAL", 0},
	{VA_GTIMER_GTMR0_CUR_VAL, DATA_GTIMER_GTMR0_CUR_VAL, dump_gtimer_gtmr0_cur_val, "GTIMER_GTMR0_CUR_VAL", 0},
	{VA_GTIMER_GTMR0_CTRL, DATA_GTIMER_GTMR0_CTRL, dump_gtimer_gtmr0_ctrl, "GTIMER_GTMR0_CTRL", 0},
	{VA_GTIMER_GTMR0_INT_EN, DATA_GTIMER_GTMR0_INT_EN, dump_gtimer_gtmr0_int_en, "GTIMER_GTMR0_INT_EN", 0},
	{VA_GTIMER_GTMR0_CLR_INT_PENDING, DATA_GTIMER_GTMR0_CLR_INT_PENDING, dump_gtimer_gtmr0_clr_int_pending, "GTIMER_GTMR0_CLR_INT_PENDING", 0},
	{VA_GTIMER_GTMR0_INT_PENDING, DATA_GTIMER_GTMR0_INT_PENDING, dump_gtimer_gtmr0_int_pending, "GTIMER_GTMR0_INT_PENDING", 0},
	{VA_GTIMER_GTMR1_CLK_CTRL, DATA_GTIMER_GTMR1_CLK_CTRL, dump_gtimer_gtmr1_clk_ctrl, "GTIMER_GTMR1_CLK_CTRL", 0},
	{VA_GTIMER_GTMR1_STORE_VAL, DATA_GTIMER_GTMR1_STORE_VAL, dump_gtimer_gtmr1_store_val, "GTIMER_GTMR1_STORE_VAL", 0},
	{VA_GTIMER_GTMR1_CUR_VAL, DATA_GTIMER_GTMR1_CUR_VAL, dump_gtimer_gtmr1_cur_val, "GTIMER_GTMR1_CUR_VAL", 0},
	{VA_GTIMER_GTMR1_CTRL, DATA_GTIMER_GTMR1_CTRL, dump_gtimer_gtmr1_ctrl, "GTIMER_GTMR1_CTRL", 0},
	{VA_GTIMER_GTMR1_INT_EN, DATA_GTIMER_GTMR1_INT_EN, dump_gtimer_gtmr1_int_en, "GTIMER_GTMR1_INT_EN", 0},
	{VA_GTIMER_GTMR1_CLR_INT_PENDING, DATA_GTIMER_GTMR1_CLR_INT_PENDING, dump_gtimer_gtmr1_clr_int_pending, "GTIMER_GTMR1_CLR_INT_PENDING", 0},
	{VA_GTIMER_GTMR1_INT_PENDING, DATA_GTIMER_GTMR1_INT_PENDING, dump_gtimer_gtmr1_int_pending, "GTIMER_GTMR1_INT_PENDING", 0},
	{VA_GTIMER_GTMR2_CLK_CTRL, DATA_GTIMER_GTMR2_CLK_CTRL, dump_gtimer_gtmr2_clk_ctrl, "GTIMER_GTMR2_CLK_CTRL", 0},
	{VA_GTIMER_GTMR2_STORE_VAL, DATA_GTIMER_GTMR2_STORE_VAL, dump_gtimer_gtmr2_store_val, "GTIMER_GTMR2_STORE_VAL", 0},
	{VA_GTIMER_GTMR2_CUR_VAL, DATA_GTIMER_GTMR2_CUR_VAL, dump_gtimer_gtmr2_cur_val, "GTIMER_GTMR2_CUR_VAL", 0},
	{VA_GTIMER_GTMR2_CTRL, DATA_GTIMER_GTMR2_CTRL, dump_gtimer_gtmr2_ctrl, "GTIMER_GTMR2_CTRL", 0},
	{VA_GTIMER_GTMR2_INT_EN, DATA_GTIMER_GTMR2_INT_EN, dump_gtimer_gtmr2_int_en, "GTIMER_GTMR2_INT_EN", 0},
	{VA_GTIMER_GTMR2_CLR_INT_PENDING, DATA_GTIMER_GTMR2_CLR_INT_PENDING, dump_gtimer_gtmr2_clr_int_pending, "GTIMER_GTMR2_CLR_INT_PENDING", 0},
	{VA_GTIMER_GTMR2_INT_PENDING, DATA_GTIMER_GTMR2_INT_PENDING, dump_gtimer_gtmr2_int_pending, "GTIMER_GTMR2_INT_PENDING", 0},
	{VA_GTIMER_GTMR3_CLK_CTRL, DATA_GTIMER_GTMR3_CLK_CTRL, dump_gtimer_gtmr3_clk_ctrl, "GTIMER_GTMR3_CLK_CTRL", 0},
	{VA_GTIMER_GTMR3_STORE_VAL, DATA_GTIMER_GTMR3_STORE_VAL, dump_gtimer_gtmr3_store_val, "GTIMER_GTMR3_STORE_VAL", 0},
	{VA_GTIMER_GTMR3_CUR_VAL, DATA_GTIMER_GTMR3_CUR_VAL, dump_gtimer_gtmr3_cur_val, "GTIMER_GTMR3_CUR_VAL", 0},
	{VA_GTIMER_GTMR3_CTRL, DATA_GTIMER_GTMR3_CTRL, dump_gtimer_gtmr3_ctrl, "GTIMER_GTMR3_CTRL", 0},
	{VA_GTIMER_GTMR3_INT_EN, DATA_GTIMER_GTMR3_INT_EN, dump_gtimer_gtmr3_int_en, "GTIMER_GTMR3_INT_EN", 0},
	{VA_GTIMER_GTMR3_CLR_INT_PENDING, DATA_GTIMER_GTMR3_CLR_INT_PENDING, dump_gtimer_gtmr3_clr_int_pending, "GTIMER_GTMR3_CLR_INT_PENDING", 0},
	{VA_GTIMER_GTMR3_INT_PENDING, DATA_GTIMER_GTMR3_INT_PENDING, dump_gtimer_gtmr3_int_pending, "GTIMER_GTMR3_INT_PENDING", 0},
};

void dump_gtimer(u32 mode)
{
	u32 cnt = sizeof(gtimer_info) / sizeof(gtimer_info[0]);
	u32 i = 0;
	char sz_temp[TMP_STR_LEN];
	char buffer[DUMP_BUF_LEN];

	def_memset(buffer, 0, DUMP_BUF_LEN);

	for (i = 0; i < cnt; i++) {
		def_memset(buffer, 0, DUMP_BUF_LEN);
		def_sprintf(sz_temp, "[%u]%s:[%08X]%08X\n",
					i,
					gtimer_info[i].name,
					gtimer_info[i].addr,
					gtimer_info[i].reset);
		def_strcat(buffer, sz_temp);

		gtimer_info[i].dump(gtimer_info[i].addr, gtimer_info[i].reset, mode, buffer);
		msg("%s", buffer);
	}
}

u32 dump_gtimer_gtmr_ver(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr_ver_t reg;

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

u32 dump_gtimer_gtmr0_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr0_clk_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:clk_div\n", reg.bits.clk_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clk_sel\n", reg.bits.clk_sel);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gtimer_gtmr0_store_val(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr0_store_val_t reg;

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

	def_sprintf(sz_temp, "0x%X:store_val\n", reg.bits.store_val);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gtimer_gtmr0_cur_val(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr0_cur_val_t reg;

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

	def_sprintf(sz_temp, "0x%X:cur_val\n", reg.bits.cur_val);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gtimer_gtmr0_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr0_ctrl_t reg;

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
	def_sprintf(sz_temp, "0x%X:nrdy\n", reg.bits.nrdy);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gtimer_gtmr0_int_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr0_int_en_t reg;

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

u32 dump_gtimer_gtmr0_clr_int_pending(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr0_clr_int_pending_t reg;

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

u32 dump_gtimer_gtmr0_int_pending(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr0_int_pending_t reg;

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

u32 dump_gtimer_gtmr1_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr1_clk_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:clk_div\n", reg.bits.clk_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clk_sel\n", reg.bits.clk_sel);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gtimer_gtmr1_store_val(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr1_store_val_t reg;

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

	def_sprintf(sz_temp, "0x%X:store_val\n", reg.bits.store_val);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gtimer_gtmr1_cur_val(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr1_cur_val_t reg;

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

	def_sprintf(sz_temp, "0x%X:cur_val\n", reg.bits.cur_val);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gtimer_gtmr1_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr1_ctrl_t reg;

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
	def_sprintf(sz_temp, "0x%X:nrdy\n", reg.bits.nrdy);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gtimer_gtmr1_int_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr1_int_en_t reg;

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

u32 dump_gtimer_gtmr1_clr_int_pending(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr1_clr_int_pending_t reg;

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

u32 dump_gtimer_gtmr1_int_pending(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr1_int_pending_t reg;

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

u32 dump_gtimer_gtmr2_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr2_clk_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:clk_div\n", reg.bits.clk_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clk_sel\n", reg.bits.clk_sel);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gtimer_gtmr2_store_val(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr2_store_val_t reg;

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

	def_sprintf(sz_temp, "0x%X:store_val\n", reg.bits.store_val);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gtimer_gtmr2_cur_val(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr2_cur_val_t reg;

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

	def_sprintf(sz_temp, "0x%X:cur_val\n", reg.bits.cur_val);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gtimer_gtmr2_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr2_ctrl_t reg;

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
	def_sprintf(sz_temp, "0x%X:nrdy\n", reg.bits.nrdy);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gtimer_gtmr2_int_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr2_int_en_t reg;

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

u32 dump_gtimer_gtmr2_clr_int_pending(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr2_clr_int_pending_t reg;

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

u32 dump_gtimer_gtmr2_int_pending(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr2_int_pending_t reg;

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

u32 dump_gtimer_gtmr3_clk_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr3_clk_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:clk_div\n", reg.bits.clk_div);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:clk_sel\n", reg.bits.clk_sel);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gtimer_gtmr3_store_val(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr3_store_val_t reg;

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

	def_sprintf(sz_temp, "0x%X:store_val\n", reg.bits.store_val);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gtimer_gtmr3_cur_val(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr3_cur_val_t reg;

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

	def_sprintf(sz_temp, "0x%X:cur_val\n", reg.bits.cur_val);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gtimer_gtmr3_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr3_ctrl_t reg;

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
	def_sprintf(sz_temp, "0x%X:nrdy\n", reg.bits.nrdy);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_gtimer_gtmr3_int_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr3_int_en_t reg;

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

u32 dump_gtimer_gtmr3_clr_int_pending(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr3_clr_int_pending_t reg;

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

u32 dump_gtimer_gtmr3_int_pending(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_gtimer_gtmr3_int_pending_t reg;

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

#endif /* ___GTIMER___DUMP__C___ */
