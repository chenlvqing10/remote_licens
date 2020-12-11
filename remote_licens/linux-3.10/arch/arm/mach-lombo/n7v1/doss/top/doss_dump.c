/* doss_dump.c */

#ifndef ___DOSS___DUMP__C___
#define ___DOSS___DUMP__C___

reg_info_t doss_info[] = {
	{VA_DOSS_VER, DATA_DOSS_VER, dump_doss_ver, "DOSS_VER", 0},
	{VA_DOSS_DO0_FTR, DATA_DOSS_DO0_FTR, dump_doss_do0_ftr, "DOSS_DO0_FTR", 0},
	{VA_DOSS_DO1_FTR, DATA_DOSS_DO1_FTR, dump_doss_do1_ftr, "DOSS_DO1_FTR", 0},
	{VA_DOSS_RESET, DATA_DOSS_RESET, dump_doss_reset, "DOSS_RESET", 0},
	{VA_DOSS_ROUTE, DATA_DOSS_ROUTE, dump_doss_route, "DOSS_ROUTE", 0},
	{VA_DOSS_TCON0_CTRL, DATA_DOSS_TCON0_CTRL, dump_doss_tcon0_ctrl, "DOSS_TCON0_CTRL", 0},
	{VA_DOSS_TCON1_CTRL, DATA_DOSS_TCON1_CTRL, dump_doss_tcon1_ctrl, "DOSS_TCON1_CTRL", 0},
	{VA_DOSS_TCON0_CLK, DATA_DOSS_TCON0_CLK, dump_doss_tcon0_clk, "DOSS_TCON0_CLK", 0},
	{VA_DOSS_TCON1_CLK, DATA_DOSS_TCON1_CLK, dump_doss_tcon1_clk, "DOSS_TCON1_CLK", 0},
	{VA_DOSS_LVDS0_CTRL, DATA_DOSS_LVDS0_CTRL, dump_doss_lvds0_ctrl, "DOSS_LVDS0_CTRL", 0},
	{VA_DOSS_LVDS1_CTRL, DATA_DOSS_LVDS1_CTRL, dump_doss_lvds1_ctrl, "DOSS_LVDS1_CTRL", 0},
	{VA_DOSS_LVDS0_CLK, DATA_DOSS_LVDS0_CLK, dump_doss_lvds0_clk, "DOSS_LVDS0_CLK", 0},
	{VA_DOSS_LVDS1_CLK, DATA_DOSS_LVDS1_CLK, dump_doss_lvds1_clk, "DOSS_LVDS1_CLK", 0},
	{VA_DOSS_MIPI_DSI0_CTRL, DATA_DOSS_MIPI_DSI0_CTRL, dump_doss_mipi_dsi0_ctrl, "DOSS_MIPI_DSI0_CTRL", 0},
	{VA_DOSS_MIPI_DSI1_CTRL, DATA_DOSS_MIPI_DSI1_CTRL, dump_doss_mipi_dsi1_ctrl, "DOSS_MIPI_DSI1_CTRL", 0},
	{VA_DOSS_MIPI_DSI0_REF_CLK, DATA_DOSS_MIPI_DSI0_REF_CLK, dump_doss_mipi_dsi0_ref_clk, "DOSS_MIPI_DSI0_REF_CLK", 0},
	{VA_DOSS_MIPI_DSI1_REF_CLK, DATA_DOSS_MIPI_DSI1_REF_CLK, dump_doss_mipi_dsi1_ref_clk, "DOSS_MIPI_DSI1_REF_CLK", 0},
	{VA_DOSS_MIPI_DSI0_CFG_CLK, DATA_DOSS_MIPI_DSI0_CFG_CLK, dump_doss_mipi_dsi0_cfg_clk, "DOSS_MIPI_DSI0_CFG_CLK", 0},
	{VA_DOSS_MIPI_DSI1_CFG_CLK, DATA_DOSS_MIPI_DSI1_CFG_CLK, dump_doss_mipi_dsi1_cfg_clk, "DOSS_MIPI_DSI1_CFG_CLK", 0},
	{VA_DOSS_TVE0_CTRL, DATA_DOSS_TVE0_CTRL, dump_doss_tve0_ctrl, "DOSS_TVE0_CTRL", 0},
	{VA_DOSS_TVE1_CTRL, DATA_DOSS_TVE1_CTRL, dump_doss_tve1_ctrl, "DOSS_TVE1_CTRL", 0},
	{VA_DOSS_TVE0_CLK, DATA_DOSS_TVE0_CLK, dump_doss_tve0_clk, "DOSS_TVE0_CLK", 0},
	{VA_DOSS_TVE1_CLK, DATA_DOSS_TVE1_CLK, dump_doss_tve1_clk, "DOSS_TVE1_CLK", 0},
};

void dump_doss(u32 mode)
{
	u32 cnt = sizeof(doss_info) / sizeof(doss_info[0]);
	u32 i = 0;
	char sz_temp[TMP_STR_LEN];
	char buffer[DUMP_BUF_LEN];

	def_memset(buffer, 0, DUMP_BUF_LEN);

	for (i = 0; i < cnt; i++) {
		def_memset(buffer, 0, DUMP_BUF_LEN);
		def_sprintf(sz_temp, "[%u]%s:[%08X]%08X\n",
					i,
					doss_info[i].name,
					doss_info[i].addr,
					doss_info[i].reset);
		def_strcat(buffer, sz_temp);

		doss_info[i].dump(doss_info[i].addr, doss_info[i].reset, mode, buffer);
		msg("%s", buffer);
	}
}

u32 dump_doss_ver(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_doss_ver_t reg;

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

u32 dump_doss_do0_ftr(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_doss_do0_ftr_t reg;

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

	def_sprintf(sz_temp, "0x%X:rgb\n", reg.bits.rgb);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:lvds\n", reg.bits.lvds);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dsi\n", reg.bits.dsi);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:edp\n", reg.bits.edp);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tve\n", reg.bits.tve);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:hdmi\n", reg.bits.hdmi);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tcon\n", reg.bits.tcon);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_doss_do1_ftr(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_doss_do1_ftr_t reg;

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

	def_sprintf(sz_temp, "0x%X:rgb\n", reg.bits.rgb);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:lvds\n", reg.bits.lvds);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:dsi\n", reg.bits.dsi);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:edp\n", reg.bits.edp);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tve\n", reg.bits.tve);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:hdmi\n", reg.bits.hdmi);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:tcon\n", reg.bits.tcon);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_doss_reset(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_doss_reset_t reg;

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

	def_sprintf(sz_temp, "0x%X:rst\n", reg.bits.rst);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_doss_route(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_doss_route_t reg;

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

	def_sprintf(sz_temp, "0x%X:src_do0\n", reg.bits.src_do0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:src_do1\n", reg.bits.src_do1);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_doss_tcon0_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_doss_tcon0_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:slave\n", reg.bits.slave);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gate\n", reg.bits.gate);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:rst\n", reg.bits.rst);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_doss_tcon1_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_doss_tcon1_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:slave\n", reg.bits.slave);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gate\n", reg.bits.gate);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:rst\n", reg.bits.rst);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_doss_tcon0_clk(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_doss_tcon0_clk_t reg;

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

	def_sprintf(sz_temp, "0x%X:div0\n", reg.bits.div0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div1\n", reg.bits.div1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gate\n", reg.bits.gate);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_doss_tcon1_clk(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_doss_tcon1_clk_t reg;

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

	def_sprintf(sz_temp, "0x%X:div0\n", reg.bits.div0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:div1\n", reg.bits.div1);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gate\n", reg.bits.gate);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_doss_lvds0_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_doss_lvds0_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:slave\n", reg.bits.slave);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gate\n", reg.bits.gate);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:rst\n", reg.bits.rst);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_doss_lvds1_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_doss_lvds1_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:slave\n", reg.bits.slave);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gate\n", reg.bits.gate);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:rst\n", reg.bits.rst);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_doss_lvds0_clk(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_doss_lvds0_clk_t reg;

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

	def_sprintf(sz_temp, "0x%X:gate\n", reg.bits.gate);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_doss_lvds1_clk(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_doss_lvds1_clk_t reg;

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

	def_sprintf(sz_temp, "0x%X:gate\n", reg.bits.gate);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_doss_mipi_dsi0_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_doss_mipi_dsi0_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:slave\n", reg.bits.slave);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gate\n", reg.bits.gate);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:rst\n", reg.bits.rst);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_doss_mipi_dsi1_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_doss_mipi_dsi1_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:slave\n", reg.bits.slave);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gate\n", reg.bits.gate);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:rst\n", reg.bits.rst);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_doss_mipi_dsi0_ref_clk(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_doss_mipi_dsi0_ref_clk_t reg;

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

	def_sprintf(sz_temp, "0x%X:div0\n", reg.bits.div0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gate\n", reg.bits.gate);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_doss_mipi_dsi1_ref_clk(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_doss_mipi_dsi1_ref_clk_t reg;

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

	def_sprintf(sz_temp, "0x%X:div0\n", reg.bits.div0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gate\n", reg.bits.gate);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_doss_mipi_dsi0_cfg_clk(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_doss_mipi_dsi0_cfg_clk_t reg;

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

	def_sprintf(sz_temp, "0x%X:gate\n", reg.bits.gate);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_doss_mipi_dsi1_cfg_clk(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_doss_mipi_dsi1_cfg_clk_t reg;

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

	def_sprintf(sz_temp, "0x%X:gate\n", reg.bits.gate);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_doss_tve0_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_doss_tve0_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:slave\n", reg.bits.slave);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gate\n", reg.bits.gate);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:rst\n", reg.bits.rst);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_doss_tve1_ctrl(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_doss_tve1_ctrl_t reg;

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

	def_sprintf(sz_temp, "0x%X:slave\n", reg.bits.slave);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gate\n", reg.bits.gate);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:rst\n", reg.bits.rst);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_doss_tve0_clk(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_doss_tve0_clk_t reg;

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

	def_sprintf(sz_temp, "0x%X:div0\n", reg.bits.div0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gate\n", reg.bits.gate);
	def_strcat(pout, sz_temp);

	return 0;
}

u32 dump_doss_tve1_clk(u32 addr, u32 data, u32 mode, char *outbuf)
{
	char *pout = outbuf;
	char sz_temp[TMP_STR_LEN];
	reg_doss_tve1_clk_t reg;

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

	def_sprintf(sz_temp, "0x%X:div0\n", reg.bits.div0);
	def_strcat(pout, sz_temp);
	def_sprintf(sz_temp, "0x%X:gate\n", reg.bits.gate);
	def_strcat(pout, sz_temp);

	return 0;
}

#endif /* ___DOSS___DUMP__C___ */
