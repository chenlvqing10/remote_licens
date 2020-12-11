/*
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Authors:
 *	lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include"cv_reg.h"

cvu_ctl_t			cvu_ctrl;
cvu_status_t			cvu_status;
cvu_irq_en_t			cvu_irq_en;
cvu_irq_pend_t			cvu_irq_pend;
cvu_irq_clear_t			cvu_claer;
rsz_datain_cfg0_t		input_fmt;
rsz_datain_cfg1_t		input_stride;
rsz_datain_cfg2_t		input_size;
rsz_datain_addr_t		input_y_ddr;
rsz_datain_uv_addr_t		input_uv_ddr;
rsz_dataout_cfg0_t		output_fmt;
rsz_dataout_pym0_cfg_t		output_pym0_size;
rsz_cfg0_t			offset_set;
rsz_pym0_cfg0_t			pym0_scalex;
rsz_pym0_cfg1_t			pym0_scaley;
rsz_addr_reg_t			pym_addr;

void init_register_value(void)
{
	cvu_ctrl.dwval = 0;
	cvu_status.dwval = 0;
	cvu_irq_en.dwval = 0;
	cvu_irq_pend.dwval = 0;
	cvu_claer.dwval = 0;
	input_fmt.dwval = 0;
	input_stride.dwval = 0;
	input_size.dwval = 0;
	input_y_ddr.dwval = 0;
	input_uv_ddr.dwval = 0;
	output_fmt.dwval = 0;
	offset_set.dwval = 0;
	pym0_scalex.dwval = 0;
	pym0_scaley.dwval = 0;
	pym_addr.dwval = 0;
}

void register_value(int src_width, int src_height, IMAGE_FORMAT src_fmt,
		     int y_stride, int uv_stride, int src_data_addr,
		     int src_uv_data_addr, int dst_width, int dst_height,
		     int dst_data_addr, int scalex, int scaley,
		     int start_point_ddr, int uv_start_point_ddr, int offset_x,
		     int offset_y, bool whc_flag)
{
	int src_addr;
	int src_uv_addr;

	if (src_fmt == YUV420)  /* 0:YUV420 1:YVU420 4:GRAY 6:WHC=RGB */
		input_fmt.bits.in_fmt = 0;
	else if (src_fmt == GRAY || src_fmt == GRAY_WHC)
		input_fmt.bits.in_fmt = 4;
	else if (src_fmt == RGB_WHC)
		input_fmt.bits.in_fmt = 6;

	input_fmt.bits.in_chn = 0;

	input_stride.bits.y_stride = y_stride >> 5;   /* y_stride/32 */
	input_stride.bits.uv_stride = uv_stride >> 5; /* uv_stride/32 */

	input_size.bits.in_width = src_width - 1;
	input_size.bits.in_height = src_height - 1;

	src_addr = src_data_addr;
	src_uv_addr = src_uv_data_addr;
	input_y_ddr.bits.in_addr = src_addr + start_point_ddr;
	if (src_fmt == YUV420)
		input_uv_ddr.bits.in_uv_addr = src_uv_addr + uv_start_point_ddr;

	if (src_fmt == GRAY || src_fmt == GRAY_WHC)
		output_fmt.bits.out_fmt = 4; /* 2:RGB 4:GRAY  6:WHC */
	else
		output_fmt.bits.out_fmt = 2;

	output_fmt.bits.t_flag = whc_flag; /* transposition mark */

	output_pym0_size.bits.out_pym_width = dst_width - 1;
	output_pym0_size.bits.out_pym_height = dst_height - 1;

	offset_set.bits.offset_x = offset_x;
	if (src_fmt == RGB_WHC)
		offset_set.bits.offset_y_flag = 0;
	else {
		if (offset_y % 2)
			offset_set.bits.offset_y_flag = 1;
		else
			offset_set.bits.offset_y_flag = 0;
	}

	pym0_scalex.bits.scale_x = scalex;
	pym0_scaley.bits.scale_y = scaley;

	pym_addr.bits.rsz_addr = dst_data_addr;

	cvu_claer.bits.cv_clr = 1;
	cvu_claer.bits.para_clr = 1;
	cvu_claer.bits.time_out_clr = 1;

	cvu_irq_en.bits.int_en = 1;
	cvu_irq_en.bits.para_err = 1;
	cvu_irq_en.bits.time_out_err = 1;
	cvu_irq_en.bits.time_out_cnt = 13000;
}

void cv_register_set(void *regs_base, rsz_cfg_t reg_param)
{
	/* int src_width        = reg_param.iw; */
	int src_height       = reg_param.ih;
	int src_stride       = reg_param.istride;
	int src_fmt          = reg_param.fmt_in;
	int src_data_addr    = reg_param.phyaddrin;
	int src_uv_data_addr = reg_param.phyaddrin + src_stride * src_height;
	int crop_width       = reg_param.rect.w;
	int crop_height      = reg_param.rect.h;
	int offset_x         = reg_param.rect.x;
	int offset_y         = reg_param.rect.y;
	int dst_width        = reg_param.ow;
	int dst_height       = reg_param.oh;
	int scalex           = reg_param.scale_x;
	int scaley           = reg_param.scale_y;
	int dst_data_addr    = reg_param.phyaddrout;

	IMAGE_FORMAT img_format = RGB_WHC;
	bool whc_flag = false;
	int offset_x_distance = 0;
	int offset_point_addr = 0;
	int offset_uv_point_addr = 0;
	int real_src_stride = 0;
	int temp = 0;
	int real_dst_stride = 0;

	/* int root_width = src_width; */
	int root_height = src_height;         /* gray_whc stride */
	int root_stride = src_stride;         /* yuv420 or gray stride */
	int root_stride_whc = src_height * 3; /* rgb_whc stride */
	int dst_stride_whc = dst_height * 3;

	int align_offset_x;
	int align_offset_y;
	int align_crop_width;

	int offset_x_whc;
	int offset_y_whc;
	int align_offset_y_whc;
	int crop_width_whc;
	int crop_height_whc;
	int align_offset_x_whc;
	int align_crop_width_whc;

#if 0
	pr_info("[ax debug] src_height = %d\n", src_height);
	pr_info("[ax debug] src_stride = %d\n", src_stride);
	pr_info("[ax debug] src_fmt = %d\n", src_fmt);
	pr_info("[ax debug] src_data_addr = 0x%08x\n", src_data_addr);
	pr_info("[ax debug] src_uv_data_addr = 0x%08x\n", src_uv_data_addr);
	pr_info("[ax debug] crop_width = %d\n", crop_width);
	pr_info("[ax debug] crop_height = %d\n", crop_height);
	pr_info("[ax debug] offset_x = %d\n", offset_x);
	pr_info("[ax debug] offset_y = %d\n", offset_y);
	pr_info("[ax debug] dst_width = %d\n", dst_width);
	pr_info("[ax debug] dst_height = %d\n", dst_height);
	pr_info("[ax debug] scalex = %d\n", scalex);
	pr_info("[ax debug] scaley = %d\n", scaley);
	pr_info("[ax debug] dst_data_addr = 0x%08x\n", dst_data_addr);
#endif

	if (src_fmt == 0)
		img_format = GRAY;
	else if (src_fmt == 1)
		img_format = GRAY_WHC;
	else if (src_fmt == 2)
		img_format = YUV420;
	else if (src_fmt == 3)
		img_format = RGB_WHC;

	if (img_format == RGB_WHC || img_format == GRAY_WHC)
		whc_flag = false;
	else
		whc_flag = true;

	if (img_format == YUV420 || img_format == GRAY)
		real_src_stride = (int)((root_stride + 31) / 32 * 32);
	else if (img_format == GRAY_WHC)
		real_src_stride = (int)((root_height + 31) / 32 * 32);
	else if (img_format == RGB_WHC) {
		temp = (int)((root_stride_whc + 14) / 15);
		if ((temp % 2) == 0)
			real_src_stride = temp * 16;
		else
			real_src_stride = (temp + 1) * 16;
	}

	if (img_format == GRAY || img_format == GRAY_WHC)
		real_dst_stride = (int)((dst_height + 31) / 32 * 32);
	else if (img_format == RGB_WHC || img_format == YUV420) {
		temp = (int)((dst_stride_whc + 14) / 15);
		if ((temp % 2) == 0)
			real_dst_stride = temp * 16;
		else
			real_dst_stride = (temp + 1) * 16;
	}

	align_offset_x = offset_x / 32 * 32;
	align_offset_y = 0;
	align_crop_width = crop_width + offset_x - align_offset_x;

	if (offset_y % 2)
		align_offset_y = offset_y - 1;
	else
		align_offset_y = offset_y;

	offset_x_whc = offset_y;
	offset_y_whc = offset_x;
	align_offset_y_whc = 0;
	crop_width_whc = crop_height;
	crop_height_whc = crop_width;
	align_offset_x_whc = offset_x_whc / 32 * 32;
	align_crop_width_whc = crop_width_whc +
			       offset_x_whc -
			       align_offset_x_whc;
	if ((offset_y_whc % 2))
		align_offset_y_whc = offset_y_whc - 1;
	else
		align_offset_y_whc = offset_y_whc;

	if (img_format == YUV420 || img_format == GRAY) {
		offset_point_addr = align_offset_y * real_src_stride +
				    align_offset_x;
		offset_uv_point_addr = (align_offset_y * real_src_stride) / 2 +
					align_offset_x;
		offset_x_distance = offset_x - align_offset_x;
	} else if (img_format == GRAY_WHC) {
		offset_point_addr = align_offset_y_whc * real_src_stride +
				    offset_x_whc / 32 * 32 * 1;
		offset_uv_point_addr = 0;
		offset_x_distance = offset_x_whc - offset_x_whc / 32 * 32;
	} else if (img_format == RGB_WHC) {
		offset_point_addr = offset_y_whc * real_src_stride +
				    align_offset_x_whc / 10 * 32;
		offset_uv_point_addr = 0;
		offset_x_distance = offset_x_whc - align_offset_x_whc;
	}

	init_register_value();

	if (img_format == YUV420 || img_format == GRAY)
		register_value(align_crop_width,
			       crop_height,
			       img_format,
			       real_src_stride,
			       real_src_stride,
			       src_data_addr,
			       src_uv_data_addr,
			       dst_width,
			       dst_height,
			       dst_data_addr,
			       scalex,
			       scaley,
			       offset_point_addr,
			       offset_uv_point_addr,
			       offset_x_distance,
			       offset_y,
			       whc_flag);
	else if (img_format == GRAY_WHC || img_format == RGB_WHC)
		register_value(crop_height_whc,
			       align_crop_width_whc,
			       img_format,
			       real_src_stride,
			       real_src_stride,
			       src_data_addr,
			       src_uv_data_addr,
			       dst_width,
			       dst_height,
			       dst_data_addr,
			       scalex, scaley,
			       offset_point_addr,
			       offset_uv_point_addr,
			       offset_x_distance,
			       offset_y_whc,
			       whc_flag);

#if 0
	pr_info("[ax debug] 0x%08x = 0x%08x\n", input_fmt.dwval,
						regs_base + RSZ_DATAIN_CFG0);
	pr_info("[ax debug] 0x%08x = 0x%08x\n", input_stride.dwval,
						regs_base + RSZ_DATAIN_CFG1);
	pr_info("[ax debug] 0x%08x = 0x%08x\n", input_size.dwval,
						regs_base + RSZ_DATAIN_CFG2);
	pr_info("[ax debug] 0x%08x = 0x%08x\n", input_y_ddr.dwval,
						regs_base + RSZ_DATAIN_ADDR);
	pr_info("[ax debug] 0x%08x = 0x%08x\n", input_uv_ddr.dwval,
						regs_base + RSZ_DATAIN_UV_ADDR);
	pr_info("[ax debug] 0x%08x = 0x%08x\n", output_fmt.dwval,
						regs_base + RSZ_DATAOUT_CFG0);
	pr_info("[ax debug] 0x%08x = 0x%08x\n", output_pym0_size.dwval,
					regs_base + RSZ_DATAOUT_PYM0_CFG);
	pr_info("[ax debug] 0x%08x = 0x%08x\n", offset_set.dwval,
						regs_base + RSZ_CFG0);
	pr_info("[ax debug] 0x%08x = 0x%08x\n", pym0_scalex.dwval,
						regs_base + RSZ_PYM0_CFG0);
	pr_info("[ax debug] 0x%08x = 0x%08x\n", pym0_scaley.dwval,
						regs_base + RSZ_PYM0_CFG1);
	pr_info("[ax debug] 0x%08x = 0x%08x\n", pym_addr.dwval,
						regs_base + RSZ_ADDR_REG);
	pr_info("[ax debug] 0x%08x = 0x%08x\n", cvu_claer.dwval,
						regs_base + CV_IRQ_CLEAR);
	pr_info("[ax debug] 0x%08x = 0x%08x\n", cvu_irq_en.dwval,
						regs_base + CV_IRQ_EN);
	pr_info("[ax debug] 0x%08x = 0x%08x\n", 0x00000001,
						regs_base + CV_CTL);
#endif
	/* writel(0x80000000,regs_base + CVU_CTL); */
	/* writel(0x00000000,regs_base + CVU_CTL); */
	writel(input_fmt.dwval, regs_base + RSZ_DATAIN_CFG0);
	writel(input_stride.dwval, regs_base + RSZ_DATAIN_CFG1);
	writel(input_size.dwval, regs_base + RSZ_DATAIN_CFG2);
	writel(input_y_ddr.dwval, regs_base + RSZ_DATAIN_ADDR);
	writel(input_uv_ddr.dwval, regs_base + RSZ_DATAIN_UV_ADDR);
	writel(output_fmt.dwval, regs_base + RSZ_DATAOUT_CFG0);
	writel(output_pym0_size.dwval, regs_base + RSZ_DATAOUT_PYM0_CFG);
	writel(offset_set.dwval, regs_base + RSZ_CFG0);
	writel(pym0_scalex.dwval, regs_base + RSZ_PYM0_CFG0);
	writel(pym0_scaley.dwval, regs_base + RSZ_PYM0_CFG1);
	writel(pym_addr.dwval, regs_base + RSZ_ADDR_REG);

	writel(cvu_claer.dwval, regs_base + CV_IRQ_CLEAR);
	writel(cvu_irq_en.dwval, regs_base + CV_IRQ_EN);

	writel(0x00001001, regs_base + CV_CTL);
}

