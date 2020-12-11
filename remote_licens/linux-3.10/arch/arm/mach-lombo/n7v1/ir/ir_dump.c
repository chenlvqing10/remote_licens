/* ir_dump.c */

#ifndef ___IR___DUMP__C___
#define ___IR___DUMP__C___

reg_info_t ir_info[] = {
	{VA_IR_VER, DATA_IR_VER, dump_ir_ver, "IR_VER", 0},
	{VA_IR_FTR, DATA_IR_FTR, dump_ir_ftr, "IR_FTR", 0},
	{VA_IR_CTL, DATA_IR_CTL, dump_ir_ctl, "IR_CTL", 0},
	{VA_IR_TIMEOUT_TH, DATA_IR_TIMEOUT_TH, dump_ir_timeout_th, "IR_TIMEOUT_TH", 0},
	{VA_IR_NOISE_TH, DATA_IR_NOISE_TH, dump_ir_noise_th, "IR_NOISE_TH", 0},
	{VA_IR_INT_EN, DATA_IR_INT_EN, dump_ir_int_en, "IR_INT_EN", 0},
	{VA_IR_INT_PD, DATA_IR_INT_PD, dump_ir_int_pd, "IR_INT_PD", 0},
	{VA_IR_INT_CLR, DATA_IR_INT_CLR, dump_ir_int_clr, "IR_INT_CLR", 0},
	{VA_IR_DATA, DATA_IR_DATA, dump_ir_data, "IR_DATA", 0},
};

void dump_ir(u32 mode)
{
	u32 cnt = sizeof(ir_info) / sizeof(ir_info[0]);
	u32 i = 0;
	char sz_temp[TMP_STR_LEN];
	char buffer[DUMP_BUF_LEN];

	def_memset(buffer, 0, DUMP_BUF_LEN);

	for (i = 0; i < cnt; i++) {
		def_memset(buffer, 0, DUMP_BUF_LEN);
		def_sprintf(sz_temp, "[%u]%s:[%08X]%08X\n",
					i,
					ir_info[i].name,
					ir_info[i].addr,
					ir_info[i].reset);
		def_strcat(buffer, sz_temp);

		ir_info[i].dump(ir_info[i].addr, ir_info[i].reset, mode, buffer);
		msg("%s", buffer);
	}
}

u32 dump_ir_ver(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ir_ver_t reg;

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

u32 dump_ir_ftr(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ir_ftr_t reg;

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

	def_sprintf(sz_temp, "0x%X:nec_pro\n", reg.bits.nec_pro);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sirc_pro\n", reg.bits.sirc_pro);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sirc_ext_15\n", reg.bits.sirc_ext_15);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:sirc_ext_20\n", reg.bits.sirc_ext_20);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:rc5_pro\n", reg.bits.rc5_pro);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:rc6_pro\n", reg.bits.rc6_pro);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ip_ver_l\n", reg.bits.ip_ver_l);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ip_ver_h\n", reg.bits.ip_ver_h);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_ir_ctl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ir_ctl_t reg;

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
	def_sprintf(sz_temp, "0x%X:sirc_ext\n", reg.bits.sirc_ext);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:protocol_sel\n", reg.bits.protocol_sel);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ir_invert\n", reg.bits.ir_invert);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ir_en\n", reg.bits.ir_en);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_ir_timeout_th(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ir_timeout_th_t reg;

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

	def_sprintf(sz_temp, "0x%X:thd\n", reg.bits.thd);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_ir_noise_th(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ir_noise_th_t reg;

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

	def_sprintf(sz_temp, "0x%X:thd\n", reg.bits.thd);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_ir_int_en(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ir_int_en_t reg;

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

	def_sprintf(sz_temp, "0x%X:ir_start\n", reg.bits.ir_start);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ir_frame\n", reg.bits.ir_frame);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ir_repeat\n", reg.bits.ir_repeat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ir_f_err\n", reg.bits.ir_f_err);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ir_to\n", reg.bits.ir_to);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_ir_int_pd(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ir_int_pd_t reg;

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

	def_sprintf(sz_temp, "0x%X:ir_start\n", reg.bits.ir_start);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ir_frame\n", reg.bits.ir_frame);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ir_repeat\n", reg.bits.ir_repeat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ir_f_err\n", reg.bits.ir_f_err);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ir_to\n", reg.bits.ir_to);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_ir_int_clr(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ir_int_clr_t reg;

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

	def_sprintf(sz_temp, "0x%X:ir_start\n", reg.bits.ir_start);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ir_frame\n", reg.bits.ir_frame);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ir_repeat\n", reg.bits.ir_repeat);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ir_f_err\n", reg.bits.ir_f_err);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:ir_to\n", reg.bits.ir_to);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_ir_data(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_ir_data_t reg;

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

	def_sprintf(sz_temp, "0x%X:data\n", reg.bits.data);
	def_strcat(pout, sz_temp);

	return 0;
}

#endif /* ___IR___DUMP__C___ */
