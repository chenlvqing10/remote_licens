#include <linux/slab.h>
#include <linux/io.h>
#include "vu_cfg.h"

static void Wr_Reg(unsigned int reg, unsigned int value)
{
	writel(value, (void *)reg);
}



cvu_clt_t				cvu_ctrl;
cvu_status_t			cvu_status;
cvu_irq_en_t			cvu_irq_en;
cvu_irq_pd_t			cvu_irq_pend;
cvu_irq_clr_t			cvu_clear;
rsz_datain_cfg0_t		input_fmt;
rsz_datain_cfg1_t		input_stride;
rsz_datain_cfg2_t		input_size;
rsz_datain_addr_t		input_y_ddr;
rsz_datain_uvaddr_t	input_uv_ddr;
rsz_dataout_cfg0_t		output_fmt;

rsz_dataout_cfg1_t		normal_output_size;

rsz_dataout_pym0_cfg_t	output_pym0_size;
rsz_dataout_pym1_cfg_t  output_pym1_size;
rsz_dataout_pym2_cfg_t  output_pym2_size;
rsz_dataout_pym3_cfg_t  output_pym3_size;
rsz_dataout_pym4_cfg_t  output_pym4_size;
rsz_dataout_pym5_cfg_t  output_pym5_size;
rsz_dataout_pym6_cfg_t  output_pym6_size;
rsz_dataout_pym7_cfg_t  output_pym7_size;

rsz_dataout_addr_t		normal_output_addr;

rsz_cfg0_t				offset_set;
rsz_cfg1_t				crop_size;
rsz_cfg2_t				normal_sclaex;
rsz_cfg3_t				normal_sclaey;

rsz_pym0_cfg0_t			pym0_scalex;
rsz_pym0_cfg1_t			pym0_scaley;
rsz_pym1_cfg0_t			pym1_scalex;
rsz_pym1_cfg1_t			pym1_scaley;
rsz_pym2_cfg0_t			pym2_scalex;
rsz_pym2_cfg1_t			pym2_scaley;
rsz_pym3_cfg0_t			pym3_scalex;
rsz_pym3_cfg1_t			pym3_scaley;
rsz_pym4_cfg0_t			pym4_scalex;
rsz_pym4_cfg1_t			pym4_scaley;
rsz_pym5_cfg0_t			pym5_scalex;
rsz_pym5_cfg1_t			pym5_scaley;
rsz_pym6_cfg0_t			pym6_scalex;
rsz_pym6_cfg1_t			pym6_scaley;
rsz_pym7_cfg0_t			pym7_scalex;
rsz_pym7_cfg1_t			pym7_scaley;

rsz_addr_reg_t			pym_addr;

void init_register(void)
{
	cvu_ctrl.dwval = 0;
	cvu_status.dwval = 0;
	cvu_irq_en.dwval = 0;
	cvu_irq_pend.dwval = 0;
	cvu_clear.dwval = 0;
	input_fmt.dwval = 0;
	input_stride.dwval = 0;
	input_size.dwval = 0;
	input_y_ddr.dwval = 0;
	input_uv_ddr.dwval = 0;
	output_fmt.dwval = 0;
	normal_output_size.dwval = 0;
	output_pym0_size.dwval = 0;
	output_pym1_size.dwval = 0;
	output_pym2_size.dwval = 0;
	output_pym3_size.dwval = 0;
	output_pym4_size.dwval = 0;
	output_pym5_size.dwval = 0;
	output_pym6_size.dwval = 0;
	output_pym7_size.dwval = 0;
	offset_set.dwval = 0;
	crop_size.dwval = 0;
	normal_sclaex.dwval = 0;
	normal_sclaey.dwval = 0;
	pym0_scalex.dwval = 0;
	pym0_scaley.dwval = 0;
	pym1_scalex.dwval = 0;
	pym1_scaley.dwval = 0;
	pym2_scalex.dwval = 0;
	pym2_scaley.dwval = 0;
	pym3_scalex.dwval = 0;
	pym3_scaley.dwval = 0;
	pym4_scalex.dwval = 0;
	pym4_scaley.dwval = 0;
	pym5_scalex.dwval = 0;
	pym5_scaley.dwval = 0;
	pym6_scalex.dwval = 0;
	pym6_scaley.dwval = 0;
	pym7_scalex.dwval = 0;
	pym7_scaley.dwval = 0;
	pym_addr.dwval = 0;
}

void set_rsz_register(unsigned int src_addr, unsigned int src_uv_addr,
	unsigned int rsz_addr, unsigned int src_width, unsigned int src_height,
	unsigned int root_src_width, unsigned int root_src_height,
	unsigned int real_src_stride,
	IMAGE_FORMAT src_fmt,
	unsigned int y_stride, unsigned int uv_stride,
	unsigned int dst_width, unsigned int dst_height,
	unsigned int real_dst_stride,
	IMAGE_FORMAT dst_fmt,
	double scalex, double scaley,
	unsigned int start_point_ddr,
	unsigned int uv_start_point_ddr,
	unsigned int offset_x, unsigned int offset_y,
	bool whc_flag)
{
	if (src_fmt == YUV420 || src_fmt == YVU420) {
		/* 0:YUV420/YVU420 1:YVU420 4:GRAY 6:WHC=RGB */
		input_fmt.bits.in_fmt = 0;
	} else if (src_fmt == GRAY_WHC)
		input_fmt.bits.in_fmt = 4;
	else if (src_fmt == RGB || (src_fmt == RGB_WHC && dst_fmt == RGB_WHC))
		input_fmt.bits.in_fmt = 6;
	else if (src_fmt == RGB_WHC && dst_fmt == GRAY_WHC)
		input_fmt.bits.in_fmt = 2;

	input_fmt.bits.in_chn = 0;
	input_fmt.bits.normal_flag = 0;

	input_stride.bits.y_stride = y_stride >> 5; /* y_stride/32 */
	input_stride.bits.uv_stride = uv_stride >> 5; /* uv_stride/32 */


	input_size.bits.in_width = src_width - 1;
	input_size.bits.in_height = src_height - 1;

	/* unsigned int src_addr = 0x60000000; */
	/* unsigned int src_uv_addr = 0x62000000; */
	input_y_ddr.bits.in_addr = src_addr + start_point_ddr;

	if (src_fmt == YUV420 || src_fmt == YVU420)
		input_uv_ddr.bits.in_uv_addr = src_uv_addr + uv_start_point_ddr;

	if (dst_fmt == GRAY || dst_fmt == GRAY_WHC)
		output_fmt.bits.out_fmt = 4; /* 2:RGB 4:GRAY  6:WHC */
	else
		output_fmt.bits.out_fmt = 2;

	output_fmt.bits.t_flag = whc_flag; /* 转置标志 */
	output_fmt.bits.yuv420_flag = 0;
	if (src_fmt == YVU420)
		output_fmt.bits.yuv420_flag = 1;

	output_pym0_size.bits.out_pym_width = dst_width - 1;
	output_pym0_size.bits.out_pym_height = dst_height - 1;

	offset_set.bits.offset_x = offset_x;
	if (src_fmt == RGB || src_fmt == RGB_WHC)
		offset_set.bits.offset_y_flag = 0;
	else {
		if (offset_y % 2)
			offset_set.bits.offset_y_flag = 1;
		else
			offset_set.bits.offset_y_flag = 0;
	}

	pym0_scalex.bits.scale_x = (unsigned int)(scalex * (1 << 24));
	pym0_scaley.bits.scale_y = (unsigned int)(scaley * (1 << 24));
	pym_addr.bits.rsz_addr = rsz_addr;

	cvu_clear.bits.cv_clr = 1;
	cvu_clear.bits.para_clr = 1;
	cvu_clear.bits.time_out_clr = 1;

	cvu_irq_en.bits.int_en = 1;
	cvu_irq_en.bits.para_err = 1;
	cvu_irq_en.bits.time_out_err = 1;
	cvu_irq_en.bits.time_out_cnt = 13000;

	cvu_ctrl.bits.start = 1;
	cvu_ctrl.bits.op_mode = 1; /* 1:normal 2:pym */
	cvu_ctrl.bits.pym_num = 0; /* pym number */
}

void clear_rsz_cfg(unsigned int base_reg)
{

	Wr_Reg(base_reg + CVU_DIN_CFG0, 0);
	ax_err(0, "CVU_DIN_CFG0 %08x,%08x\n", CVU_DIN_CFG0, 0);

	Wr_Reg(base_reg + CVU_DIN_CFG1, 0);
	ax_err(0, "CVU_DIN_CFG1 %08x,%08x\n", CVU_DIN_CFG1, 0);

	Wr_Reg(base_reg + CVU_DIN_CFG2, 0);
	ax_err(0, "CVU_DIN_CFG2 %08x,%08x\n", CVU_DIN_CFG2, 0);

	Wr_Reg(base_reg + CVU_DIN_YADDR, 0);
	ax_err(0, "CVU_DIN_YADDR %08x,%08x\n",
		CVU_DIN_YADDR, 0);

	Wr_Reg(base_reg + CVU_DIN_UVADDR, 0);
	ax_err(0, "CVU_DIN_UVADDR %08x,%08x\n",
		CVU_DIN_UVADDR, 0);

	Wr_Reg(base_reg + CVU_DOUT_CFG0, 0);
	ax_err(0, "CVU_DOUT_CFG0 %08x,%08x\n", CVU_DOUT_CFG0, 0);

	Wr_Reg(base_reg + CVU_DOUT_PYM0_CFG, 0);
	ax_err(0, "CVU_DOUT_PYM0_CFG %08x,%08x\n",
		CVU_DOUT_PYM0_CFG, 0);

	Wr_Reg(base_reg + CVU_RSZ_CFG0, 0);
	ax_err(0, "CVU_RSZ_CFG0 %08x,%08x\n", CVU_RSZ_CFG0, 0);

	Wr_Reg(base_reg + CVU_RSZ_PYM0_CFG0, 0);
	ax_err(0, "CVU_RSZ_PYM0_CFG0 %08x,%08x\n",
		CVU_RSZ_PYM0_CFG0, 0);

	Wr_Reg(base_reg + CVU_RSZ_PYM0_CFG1, 0);
	ax_err(0, "CVU_RSZ_PYM0_CFG1 %08x,%08x\n",
		CVU_RSZ_PYM0_CFG1, 0);

	Wr_Reg(base_reg + CVU_RSZ_ADDR, 0);
	ax_err(0, "CVU_RSZ_ADDR %08x,%08x\n", CVU_RSZ_ADDR, 0);

	/* Wr_Reg(CVU_DIN_CFG0,cvu_clear.dwval); */
	Wr_Reg(base_reg + CVU_INT_EN, 0);
	ax_err(0, "CVU_INT_EN %08x,%08x\n", CVU_INT_EN, 0);


	Wr_Reg(base_reg + CVU_CTL, 0);
	ax_err(0, "CVU_CTL %08x,%08x\n", CVU_CTL, 0);
}

void set_rsz_cfg(rsz_cfg_t rsz_cfg, unsigned int base_reg)
{
	unsigned int crop_width = rsz_cfg.rect.w;
	unsigned int crop_height = rsz_cfg.rect.h;
	unsigned int offset_x = rsz_cfg.rect.x; /* 从0开始计数 */
	unsigned int offset_y = rsz_cfg.rect.y;
	unsigned int dst_width = rsz_cfg.ow;
	unsigned int dst_height = rsz_cfg.oh;
	IMAGE_FORMAT img_format = rsz_cfg.fmt_in;
	IMAGE_FORMAT dst_format = rsz_cfg.fmt_out;
	bool whc_flag = false;
	unsigned int root_width = rsz_cfg.iw;
	unsigned int root_height = rsz_cfg.ih;

	/* unsigned int root_rgbstride = rsz_cfg.istride; */
	unsigned int root_stride = rsz_cfg.istride;
	unsigned int root_stride_whc = root_height * 3;
	unsigned int root_yuv_stride = root_width;
	/* unsigned int root_gray_stride = root_width; */

	unsigned int offset_x_whc = offset_y; /* 从0开始计数 */
	unsigned int offset_y_whc = offset_x;
	unsigned int crop_width_whc = crop_height;
	unsigned int crop_height_whc = crop_width;
	/* 从0开始计数 */
	unsigned int align_offset_x_whc = offset_x_whc / 32 * 32;
	unsigned int align_offset_y_whc = 0;
	 /* 对齐32后图像宽度 */
	unsigned int real_align_crop_width_whc = crop_width_whc
		+ offset_x_whc - align_offset_x_whc;
	unsigned int align_crop_width_whc = (crop_width_whc + offset_x_whc
		- align_offset_x_whc + 1) / 2 * 2; /* 对齐偶数 */
	unsigned int align_crop_height_whc = 0;
	unsigned int align_crop_stride_whc = 0;

	unsigned int align_offset_x = offset_x / 32 * 32; /* 从0开始计数 */
	unsigned int align_offset_y = 0;
	unsigned int real_align_crop_width = crop_width + offset_x
		- align_offset_x; /* 对齐32后图像宽度 */
	unsigned int align_crop_width = (crop_width + offset_x
		- align_offset_x + 1) / 2 * 2; /* 对齐偶数 */
	unsigned int align_crop_height = 0;
	unsigned int align_crop_stride = 0;

	unsigned int src_width = crop_width;
	unsigned int src_height = crop_height;

	unsigned int dst_stride = dst_width * 3;
	unsigned int dst_stride_hw = dst_height * 3;

	double scalex = (double)src_width / dst_width;
	double scaley = (double)src_height / dst_height;

	/* int yblock_cnt = 0, xblock_cnt = 0; */
	unsigned int offset_point = 0;
	unsigned int start_point_len = 0;
	unsigned int uv_start_point_len = 0;
	unsigned int real_src_stride = 0;
	unsigned int real_dst_stride = 0;
	unsigned int temp = 0;
	if (img_format == GRAY_WHC) {
		root_stride = root_width;
		root_stride_whc = root_height;
	}
	if (img_format == RGB_WHC || img_format == GRAY_WHC)
		whc_flag = false;
	else
		whc_flag = true;

	if ((offset_y_whc % 2)) { /* offset 奇数判断 */
		align_offset_y_whc = offset_y_whc - 1;
		align_crop_height_whc = crop_height_whc + 1;
	} else {
		align_offset_y_whc = offset_y_whc;
		align_crop_height_whc = crop_height_whc;
	}
	/* 对齐偶数 */
	align_crop_height_whc = (align_crop_height_whc + 1) / 2 * 2;
	align_crop_stride_whc = align_crop_width_whc * 3;

	if (img_format == GRAY_WHC && dst_format == GRAY_WHC) {
		align_crop_stride_whc = align_crop_width_whc;
	} else if (img_format == RGB_WHC  && dst_format == GRAY_WHC) {
		if ((offset_x_whc / 5) % 2)
			align_offset_x_whc = (offset_x_whc / 5 - 1) * 5;
		else
			align_offset_x_whc = offset_x_whc / 5 * 5;
		/* align_offset_y_whc = offset_y_whc; */
		real_align_crop_width_whc = crop_width_whc + offset_x_whc
			- align_offset_x_whc;
		/* align_crop_height_whc = crop_height_whc; */
		align_crop_stride_whc = real_align_crop_width_whc * 3;
	} else if (img_format == RGB_WHC && dst_format == RGB_WHC) {
		if ((offset_x_whc / 5) % 2)
			align_offset_x_whc = (offset_x_whc / 5 - 1) * 5;
		else
			align_offset_x_whc = offset_x_whc / 5 * 5;
		align_offset_y_whc = offset_y_whc;
		real_align_crop_width_whc = crop_width_whc + offset_x_whc
			- align_offset_x_whc;
		align_crop_height_whc = crop_height_whc;
		align_crop_stride_whc = real_align_crop_width_whc * 3;
	}


	if ((offset_y % 2)) { /* offset 奇数判断 */
		align_offset_y = offset_y - 1;
		align_crop_height = crop_height + 1;
	} else {
		align_offset_y = offset_y;
		align_crop_height = crop_height;
	}
	align_crop_height = (align_crop_height + 1) / 2 * 2; /* 对齐偶数 */
	align_crop_stride = align_crop_width * 3;

	if (img_format == RGB) {
		if ((offset_x / 5) % 2)
			align_offset_x = (offset_x / 5 - 1) * 5;
		else
			align_offset_x = offset_x / 5 * 5;
		align_offset_y = offset_y;
		real_align_crop_width = crop_width + offset_x - align_offset_x;
		align_crop_height = crop_height;
		align_crop_stride = real_align_crop_width * 3;
	}



	if (img_format == YUV420 || img_format == YVU420)
		real_src_stride = (unsigned int)((root_yuv_stride + 31)
			/ 32 * 32);  /* 只适用于YUV420或GRAY输入模式 */
	else if (img_format == GRAY_WHC)
		real_src_stride = (unsigned int)((root_stride_whc + 31)
			/ 32 * 32);
	else if (img_format == RGB) {
		temp = (int)((root_stride + 14) / 15);
		if ((temp % 2) == 0)
			real_src_stride = temp * 16;
		else
			real_src_stride = (temp + 1) * 16;
	} else if (img_format == RGB_WHC) {
		temp = (int)((root_stride_whc + 14) / 15);
		if ((temp % 2) == 0)
			real_src_stride = temp * 16;
		else
			real_src_stride = (temp + 1) * 16;
	}

	if (dst_format == GRAY_WHC)
		real_dst_stride = (unsigned int)((dst_height + 31)
			/ 32 * 32); /* 只适用于GRAY WHC输出模式 */
	else if (dst_format == RGB_WHC) {
		temp = (int)((dst_stride_hw + 14) / 15);
		if ((temp % 2) == 0)
			real_dst_stride = temp * 16;
		else
			real_dst_stride = (temp + 1) * 16;
	} else if (dst_format == RGB) {
		temp = (int)((dst_stride + 14) / 15);
		if ((temp % 2) == 0)
			real_dst_stride = temp * 16;
		else
			real_dst_stride = (temp + 1) * 16;
	}

	if (img_format == YUV420 || img_format == YVU420) {
		/* 此处offset+x是否+1 */
		start_point_len = align_offset_y * real_src_stride + offset_x
			/ 32 * 32 * 1;
		uv_start_point_len = (align_offset_y * real_src_stride) / 2
			+ offset_x / 32 * 32 * 1;
		offset_point = offset_x - offset_x / 32 * 32;
	} else if (img_format == GRAY_WHC) {
		/* 此处offset+x是否+1 */
		start_point_len = align_offset_y_whc * real_src_stride
			+ offset_x_whc / 32 * 32 * 1;
		uv_start_point_len = 0;
		offset_point = offset_x_whc - offset_x_whc / 32 * 32;
	} else if (img_format == RGB) {
		/* 此处计算有问题，5个像素点后，
		 * 中间补0,此处有问题?? */
		start_point_len = offset_y * real_src_stride
			+ align_offset_x / 10 * 32;
		uv_start_point_len = 0;
		offset_point = offset_x - align_offset_x;
	} else if (img_format == RGB_WHC) {
		/* 此处计算有问题，5个像素点后，
		 * 中间补0,此处有问题?? */
		start_point_len = offset_y_whc * real_src_stride
			+ align_offset_x_whc / 10 * 32;
		uv_start_point_len = 0;
		offset_point = offset_x_whc - align_offset_x_whc;
	}

	/* init file & register */
	init_register();
	if (img_format == YUV420 || img_format == RGB || img_format == YVU420)
		set_rsz_register(rsz_cfg.phyaddrin, rsz_cfg.phyaddrin_uv,
			rsz_cfg.phyaddrout, real_align_crop_width, crop_height,
			root_width, root_height, real_src_stride, img_format,
			real_src_stride, real_src_stride, dst_width, dst_height,
			real_dst_stride, dst_format, scalex, scaley,
			start_point_len, uv_start_point_len, offset_point,
			offset_y, whc_flag);
	else if (img_format == GRAY_WHC || img_format == RGB_WHC)
		set_rsz_register(rsz_cfg.phyaddrin, rsz_cfg.phyaddrin_uv,
			rsz_cfg.phyaddrout, crop_height_whc,
			real_align_crop_width_whc, root_width,
			root_height, real_src_stride, img_format,
			real_src_stride, real_src_stride,
			dst_width, dst_height, real_dst_stride, dst_format,
			scalex, scaley, start_point_len, uv_start_point_len,
			offset_point, offset_y_whc, whc_flag);

	Wr_Reg(base_reg + CVU_DIN_CFG0, input_fmt.dwval);
	ax_err(0, "CVU_DIN_CFG0 %08x,%08x\n", CVU_DIN_CFG0, input_fmt.dwval);

	Wr_Reg(base_reg + CVU_DIN_CFG1, input_stride.dwval);
	ax_err(0, "CVU_DIN_CFG1 %08x,%08x\n", CVU_DIN_CFG1, input_stride.dwval);

	Wr_Reg(base_reg + CVU_DIN_CFG2, input_size.dwval);
	ax_err(0, "CVU_DIN_CFG2 %08x,%08x\n", CVU_DIN_CFG2, input_size.dwval);

	Wr_Reg(base_reg + CVU_DIN_YADDR, input_y_ddr.dwval);
	ax_err(0, "CVU_DIN_YADDR %08x,%08x\n",
		CVU_DIN_YADDR, input_y_ddr.dwval);

	Wr_Reg(base_reg + CVU_DIN_UVADDR, input_uv_ddr.dwval);
	ax_err(0, "CVU_DIN_UVADDR %08x,%08x\n",
		CVU_DIN_UVADDR, input_uv_ddr.dwval);

	Wr_Reg(base_reg + CVU_DOUT_CFG0, output_fmt.dwval);
	ax_err(0, "CVU_DOUT_CFG0 %08x,%08x\n", CVU_DOUT_CFG0, output_fmt.dwval);

	Wr_Reg(base_reg + CVU_DOUT_PYM0_CFG, output_pym0_size.dwval);
	ax_err(0, "CVU_DOUT_PYM0_CFG %08x,%08x\n",
		CVU_DOUT_PYM0_CFG, output_pym0_size.dwval);

	Wr_Reg(base_reg + CVU_RSZ_CFG0, offset_set.dwval);
	ax_err(0, "CVU_RSZ_CFG0 %08x,%08x\n", CVU_RSZ_CFG0, offset_set.dwval);

	Wr_Reg(base_reg + CVU_RSZ_PYM0_CFG0, pym0_scalex.dwval);
	ax_err(0, "CVU_RSZ_PYM0_CFG0 %08x,%08x\n",
		CVU_RSZ_PYM0_CFG0, pym0_scalex.dwval);

	Wr_Reg(base_reg + CVU_RSZ_PYM0_CFG1, pym0_scaley.dwval);
	ax_err(0, "CVU_RSZ_PYM0_CFG1 %08x,%08x\n",
		CVU_RSZ_PYM0_CFG1, pym0_scaley.dwval);

	Wr_Reg(base_reg + CVU_RSZ_ADDR, pym_addr.dwval);
	ax_err(0, "CVU_RSZ_ADDR %08x,%08x\n", CVU_RSZ_ADDR, pym_addr.dwval);

	/* Wr_Reg(CVU_DIN_CFG0,cvu_clear.dwval); */
	Wr_Reg(base_reg + CVU_INT_EN, cvu_irq_en.dwval);
	ax_err(0, "CVU_INT_EN %08x,%08x\n", CVU_INT_EN, cvu_irq_en.dwval);

	Wr_Reg(base_reg + CVU_CTL, cvu_ctrl.dwval);
	ax_err(0, "CVU_CTL %08x,%08x\n", CVU_CTL, cvu_ctrl.dwval);
}



void set_pym_register(
	unsigned int pym_layer_num,
	unsigned int src_width,
	unsigned int src_height,
	IMAGE_FORMAT src_fmt,
	unsigned int y_stride,
	unsigned int uv_stride,
	unsigned int *dst_width,
	unsigned int *dst_height,
	unsigned int *dst_stride,
	IMAGE_FORMAT dst_fmt,
	unsigned int src_ddr,
	unsigned int src_uv_ddr,
	unsigned int dst_ddr)
{
	int ii = 0;
	double scalex[8];
	double scaley[8];
	if (src_fmt == YUV420) {
		/* 0:YUV420 1:YVU420 4:GRAY 6:WHC=RGB */
		input_fmt.bits.in_fmt = 0;
		output_fmt.bits.yuv420_flag = 0;
	} else if (src_fmt == YVU420) {
		input_fmt.bits.in_fmt = 1;
		output_fmt.bits.yuv420_flag = 1;
	} else if (src_fmt == RGB)
		input_fmt.bits.in_fmt = 6;

	input_fmt.bits.in_chn = 0;
	/* input_fmt.bits.normal_flag = 1; */

	input_stride.bits.y_stride = y_stride >> 5; /* y_stride/32 */
	input_stride.bits.uv_stride = uv_stride >> 5; /* uv_stride/32 */

	input_size.bits.in_width = src_width - 1;
	input_size.bits.in_height = src_height - 1;

	/* unsigned int src_addr = src_ddr; */
	/* unsigned int src_uv_addr = 0x62000000; */
	input_y_ddr.bits.in_addr = src_ddr;
	if (src_fmt == YUV420 || src_fmt == YVU420)
		input_uv_ddr.bits.in_uv_addr = src_uv_ddr;

	if (dst_fmt == GRAY)
		output_fmt.bits.out_fmt = 4; /* 2:RGB 4:GRAY */
	else
		output_fmt.bits.out_fmt = 2;

	output_fmt.bits.t_flag = 0; /* 转置标志 */

	output_pym0_size.bits.out_pym_width = dst_width[0] - 1;
	output_pym0_size.bits.out_pym_height = dst_height[0] - 1;

	output_pym1_size.bits.out_pym_width = dst_width[1] - 1;
	output_pym1_size.bits.out_pym_height = dst_height[1] - 1;

	output_pym2_size.bits.out_pym_width = dst_width[2] - 1;
	output_pym2_size.bits.out_pym_height = dst_height[2] - 1;

	output_pym3_size.bits.out_pym_width = dst_width[3] - 1;
	output_pym3_size.bits.out_pym_height = dst_height[3] - 1;

	output_pym4_size.bits.out_pym_width = dst_width[4] - 1;
	output_pym4_size.bits.out_pym_height = dst_height[4] - 1;

	output_pym5_size.bits.out_pym_width = dst_width[5] - 1;
	output_pym5_size.bits.out_pym_height = dst_height[5] - 1;

	output_pym6_size.bits.out_pym_width = dst_width[6] - 1;
	output_pym6_size.bits.out_pym_height = dst_height[6] - 1;

	output_pym7_size.bits.out_pym_width = dst_width[7] - 1;
	output_pym7_size.bits.out_pym_height = dst_height[7] - 1;

	for (ii = 0; ii < pym_layer_num; ii++) {
		scalex[ii] = (double) src_width / dst_width[ii];
		scaley[ii] = (double) src_height / dst_height[ii];
	}

	pym0_scalex.bits.scale_x = (unsigned int)(scalex[0] * (1 << 15)) << 9;
	pym0_scaley.bits.scale_y = (unsigned int)(scaley[0] * (1 << 15)) << 9;

	pym1_scalex.bits.scale_x = (unsigned int)(scalex[1] * (1 << 15));
	pym1_scaley.bits.scale_y = (unsigned int)(scaley[1] * (1 << 15));

	pym2_scalex.bits.scale_x = (unsigned int)(scalex[2] * (1 << 15));
	pym2_scaley.bits.scale_y = (unsigned int)(scaley[2] * (1 << 15));

	pym3_scalex.bits.scale_x = (unsigned int)(scalex[3] * (1 << 15));
	pym3_scaley.bits.scale_y = (unsigned int)(scaley[3] * (1 << 15));

	pym4_scalex.bits.scale_x = (unsigned int)(scalex[4] * (1 << 15));
	pym4_scaley.bits.scale_y = (unsigned int)(scaley[4] * (1 << 15));

	pym5_scalex.bits.scale_x = (unsigned int)(scalex[5] * (1 << 15));
	pym5_scaley.bits.scale_y = (unsigned int)(scaley[5] * (1 << 15));

	pym6_scalex.bits.scale_x = (unsigned int)(scalex[6] * (1 << 15));
	pym6_scaley.bits.scale_y = (unsigned int)(scaley[6] * (1 << 15));

	pym7_scalex.bits.scale_x = (unsigned int)(scalex[7] * (1 << 15));
	pym7_scaley.bits.scale_y = (unsigned int)(scaley[7] * (1 << 15));

	pym_addr.bits.rsz_addr = dst_ddr;

	cvu_clear.bits.cv_clr = 1;
	cvu_clear.bits.para_clr = 1;
	cvu_clear.bits.time_out_clr = 1;

	cvu_irq_en.bits.int_en = 1;
	cvu_irq_en.bits.para_err = 1;
	cvu_irq_en.bits.time_out_err = 1;
	cvu_irq_en.bits.time_out_cnt = 13000;

	cvu_ctrl.bits.start = 1;
	cvu_ctrl.bits.op_mode = 2; /* 1:normal 2:pym */
	cvu_ctrl.bits.pym_num = pym_layer_num-1; /* pym number */
}

void set_pym_cfg(pym_cfg_t pym_cfg, unsigned int base_reg)
{
	int src_width = pym_cfg.iw;
	int src_height = pym_cfg.ih;
	/* int rgbstride = pym_cfg.iw * 3; */
	/* int src_stride; */
	int yuv_stride = pym_cfg.istride;

	int dst_stride[8] = {0};
	int real_dst_stride[8] = {0};
	int inputfmt = pym_cfg.fmt_in;
	int outputfmt = pym_cfg.fmt_out;
	IMAGE_FORMAT in_format = 0; /* input_fmt */
	IMAGE_FORMAT img_format = 0; /* dst fmt */
	unsigned int temp;
	int i;

	if (inputfmt == 0)
		in_format = YUV420;
	else if (inputfmt == 1)
		in_format = YVU420;

	if (outputfmt == 0)
		img_format = RGB;
	else if (outputfmt == 1)
		img_format = GRAY;


	for (i = 0; i < pym_cfg.pym_layer; i++)	{
		if (img_format == RGB) {
			dst_stride[i] = pym_cfg.ow[i] * 3;
			temp = (unsigned int)((dst_stride[i] + 14) / 15);
			if ((temp % 2) == 0)
				real_dst_stride[i] = temp * 16;
			else
				real_dst_stride[i] = (temp + 1) * 16;
		} else  if (img_format == GRAY) {
			dst_stride[i] = pym_cfg.ow[i];
			real_dst_stride[i] = (dst_stride[i] + 31) / 32 * 32;
		}
	}

	init_register();
	set_pym_register(pym_cfg.pym_layer, src_width, src_height,
		in_format, yuv_stride,
		yuv_stride, pym_cfg.ow, pym_cfg.oh, dst_stride, img_format,
		pym_cfg.phyaddrin, pym_cfg.phyaddrin_uv, pym_cfg.phyaddrout);

	Wr_Reg(base_reg + CVU_DIN_CFG0, input_fmt.dwval);
	ax_err(0, "CVU_DIN_CFG0 %08x,%08x\n", CVU_DIN_CFG0, input_fmt.dwval);

	Wr_Reg(base_reg + CVU_DIN_CFG1, input_stride.dwval);
	ax_err(0, "CVU_DIN_CFG1 %08x,%08x\n", CVU_DIN_CFG1, input_stride.dwval);

	Wr_Reg(base_reg + CVU_DIN_CFG2, input_size.dwval);
	ax_err(0, "CVU_DIN_CFG2 %08x,%08x\n", CVU_DIN_CFG2, input_size.dwval);

	Wr_Reg(base_reg + CVU_DIN_YADDR, input_y_ddr.dwval);
	ax_err(0, "CVU_DIN_YADDR %08x,%08x\n",
		CVU_DIN_YADDR, input_y_ddr.dwval);

	Wr_Reg(base_reg + CVU_DIN_UVADDR, input_uv_ddr.dwval);
	ax_err(0, "CVU_DIN_UVADDR %08x,%08x\n",
		CVU_DIN_UVADDR, input_uv_ddr.dwval);

	Wr_Reg(base_reg + CVU_DOUT_CFG0, output_fmt.dwval);
	ax_err(0, "CVU_DOUT_CFG0 %08x,%08x\n", CVU_DOUT_CFG0, output_fmt.dwval);

	Wr_Reg(base_reg + CVU_DOUT_PYM0_CFG, output_pym0_size.dwval);
	ax_err(0, "CVU_DOUT_PYM0_CFG %08x,%08x\n",
		CVU_DOUT_PYM0_CFG, output_pym0_size.dwval);

	Wr_Reg(base_reg + CVU_DOUT_PYM1_CFG, output_pym1_size.dwval);
	ax_err(0, "CVU_DOUT_PYM1_CFG %08x,%08x\n",
		CVU_DOUT_PYM1_CFG, output_pym1_size.dwval);

	Wr_Reg(base_reg + CVU_DOUT_PYM2_CFG, output_pym2_size.dwval);
	ax_err(0, "CVU_DOUT_PYM2_CFG %08x,%08x\n",
		CVU_DOUT_PYM2_CFG, output_pym2_size.dwval);

	Wr_Reg(base_reg + CVU_DOUT_PYM3_CFG, output_pym3_size.dwval);
	ax_err(0, "CVU_DOUT_PYM3_CFG %08x,%08x\n",
		CVU_DOUT_PYM3_CFG, output_pym3_size.dwval);

	Wr_Reg(base_reg + CVU_DOUT_PYM4_CFG, output_pym4_size.dwval);
	ax_err(0, "CVU_DOUT_PYM4_CFG %08x,%08x\n",
		CVU_DOUT_PYM4_CFG, output_pym4_size.dwval);

	Wr_Reg(base_reg + CVU_DOUT_PYM5_CFG, output_pym5_size.dwval);
	ax_err(0, "CVU_DOUT_PYM5_CFG %08x,%08x\n",
		CVU_DOUT_PYM5_CFG, output_pym5_size.dwval);

	Wr_Reg(base_reg + CVU_DOUT_PYM6_CFG, output_pym6_size.dwval);
	ax_err(0, "CVU_DOUT_PYM6_CFG %08x,%08x\n",
		CVU_DOUT_PYM6_CFG, output_pym6_size.dwval);

	Wr_Reg(base_reg + CVU_DOUT_PYM7_CFG, output_pym7_size.dwval);
	ax_err(0, "CVU_DOUT_PYM7_CFG %08x,%08x\n",
		CVU_DOUT_PYM7_CFG, output_pym7_size.dwval);

	Wr_Reg(base_reg + CVU_RSZ_PYM0_CFG0, pym0_scalex.dwval);
	ax_err(0, "CVU_RSZ_PYM0_CFG0 %08x,%08x\n",
		CVU_RSZ_PYM0_CFG0, pym0_scalex.dwval);

	Wr_Reg(base_reg + CVU_RSZ_PYM1_CFG0, pym1_scalex.dwval);
	ax_err(0, "CVU_RSZ_PYM1_CFG0 %08x,%08x\n",
		CVU_RSZ_PYM1_CFG0, pym1_scalex.dwval);

	Wr_Reg(base_reg + CVU_RSZ_PYM2_CFG0, pym2_scalex.dwval);
	ax_err(0, "CVU_RSZ_PYM2_CFG0 %08x,%08x\n",
		CVU_RSZ_PYM2_CFG0, pym2_scalex.dwval);

	Wr_Reg(base_reg + CVU_RSZ_PYM3_CFG0, pym3_scalex.dwval);
	ax_err(0, "CVU_RSZ_PYM3_CFG0 %08x,%08x\n",
		CVU_RSZ_PYM3_CFG0, pym3_scalex.dwval);

	Wr_Reg(base_reg + CVU_RSZ_PYM4_CFG0, pym4_scalex.dwval);
	ax_err(0, "CVU_RSZ_PYM4_CFG0 %08x,%08x\n",
		CVU_RSZ_PYM4_CFG0, pym4_scalex.dwval);

	Wr_Reg(base_reg + CVU_RSZ_PYM5_CFG0, pym5_scalex.dwval);
	ax_err(0, "CVU_RSZ_PYM5_CFG0 %08x,%08x\n",
		CVU_RSZ_PYM5_CFG0, pym5_scalex.dwval);

	Wr_Reg(base_reg + CVU_RSZ_PYM6_CFG0, pym6_scalex.dwval);
	ax_err(0, "CVU_RSZ_PYM6_CFG0 %08x,%08x\n",
		CVU_RSZ_PYM6_CFG0, pym6_scalex.dwval);

	Wr_Reg(base_reg + CVU_RSZ_PYM7_CFG0, pym7_scalex.dwval);
	ax_err(0, "CVU_RSZ_PYM7_CFG0 %08x,%08x\n",
		CVU_RSZ_PYM7_CFG0, pym7_scalex.dwval);


	Wr_Reg(base_reg + CVU_RSZ_PYM0_CFG1, pym0_scaley.dwval);
	ax_err(0, "CVU_RSZ_PYM0_CFG1 %08x,%08x\n",
		CVU_RSZ_PYM0_CFG1, pym0_scaley.dwval);

	Wr_Reg(base_reg + CVU_RSZ_PYM1_CFG1, pym1_scaley.dwval);
	ax_err(0, "CVU_RSZ_PYM1_CFG1 %08x,%08x\n",
		CVU_RSZ_PYM1_CFG1, pym1_scaley.dwval);

	Wr_Reg(base_reg + CVU_RSZ_PYM2_CFG1, pym2_scaley.dwval);
	ax_err(0, "CVU_RSZ_PYM2_CFG1 %08x,%08x\n",
		CVU_RSZ_PYM2_CFG1, pym2_scaley.dwval);

	Wr_Reg(base_reg + CVU_RSZ_PYM3_CFG1, pym3_scaley.dwval);
	ax_err(0, "CVU_RSZ_PYM3_CFG1 %08x,%08x\n",
		CVU_RSZ_PYM3_CFG1, pym3_scaley.dwval);

	Wr_Reg(base_reg + CVU_RSZ_PYM4_CFG1, pym4_scaley.dwval);
	ax_err(0, "CVU_RSZ_PYM4_CFG1 %08x,%08x\n",
		CVU_RSZ_PYM4_CFG1, pym4_scaley.dwval);

	Wr_Reg(base_reg + CVU_RSZ_PYM5_CFG1, pym5_scaley.dwval);
	ax_err(0, "CVU_RSZ_PYM5_CFG1 %08x,%08x\n",
		CVU_RSZ_PYM5_CFG1, pym5_scaley.dwval);

	Wr_Reg(base_reg + CVU_RSZ_PYM6_CFG1, pym6_scaley.dwval);
	ax_err(0, "CVU_RSZ_PYM6_CFG1 %08x,%08x\n",
		CVU_RSZ_PYM6_CFG1, pym6_scaley.dwval);

	Wr_Reg(base_reg + CVU_RSZ_PYM7_CFG1, pym7_scaley.dwval);
	ax_err(0, "CVU_RSZ_PYM7_CFG1 %08x,%08x\n",
		CVU_RSZ_PYM7_CFG1, pym7_scaley.dwval);

	Wr_Reg(base_reg + CVU_RSZ_ADDR, pym_addr.dwval);
	ax_err(0, "CVU_RSZ_ADDR %08x,%08x\n", CVU_RSZ_ADDR, pym_addr.dwval);

	/* Wr_Reg(CVU_DIN_CFG0,cvu_clear.dwval); */
	Wr_Reg(base_reg + CVU_INT_EN, cvu_irq_en.dwval);
	ax_err(0, "CVU_INT_EN %08x,%08x\n", CVU_INT_EN, cvu_irq_en.dwval);

	Wr_Reg(base_reg + CVU_CTL, cvu_ctrl.dwval);
	ax_err(0, "CVU_CTL %08x,%08x\n", CVU_CTL, cvu_ctrl.dwval);
}

void clear_pym_cfg(unsigned int base_reg)
{

	Wr_Reg(base_reg + CVU_DIN_CFG0, 0);
	ax_err(0, "CVU_DIN_CFG0 %08x,%08x\n", CVU_DIN_CFG0, 0);

	Wr_Reg(base_reg + CVU_DIN_CFG1, 0);
	ax_err(0, "CVU_DIN_CFG1 %08x,%08x\n", CVU_DIN_CFG1, 0);

	Wr_Reg(base_reg + CVU_DIN_CFG2, 0);
	ax_err(0, "CVU_DIN_CFG2 %08x,%08x\n", CVU_DIN_CFG2, 0);

	Wr_Reg(base_reg + CVU_DIN_YADDR, 0);
	ax_err(0, "CVU_DIN_YADDR %08x,%08x\n",
		CVU_DIN_YADDR, 0);

	Wr_Reg(base_reg + CVU_DIN_UVADDR, 0);
	ax_err(0, "CVU_DIN_UVADDR %08x,%08x\n",
		CVU_DIN_UVADDR, 0);

	Wr_Reg(base_reg + CVU_DOUT_CFG0, 0);
	ax_err(0, "CVU_DOUT_CFG0 %08x,%08x\n", CVU_DOUT_CFG0, 0);

	Wr_Reg(base_reg + CVU_DOUT_PYM0_CFG, 0);
	ax_err(0, "CVU_DOUT_PYM0_CFG %08x,%08x\n",
		CVU_DOUT_PYM0_CFG, 0);

	Wr_Reg(base_reg + CVU_DOUT_PYM1_CFG, 0);
	ax_err(0, "CVU_DOUT_PYM1_CFG %08x,%08x\n",
		CVU_DOUT_PYM1_CFG, 0);

	Wr_Reg(base_reg + CVU_DOUT_PYM2_CFG, 0);
	ax_err(0, "CVU_DOUT_PYM2_CFG %08x,%08x\n",
		CVU_DOUT_PYM2_CFG, 0);

	Wr_Reg(base_reg + CVU_DOUT_PYM3_CFG, 0);
	ax_err(0, "CVU_DOUT_PYM3_CFG %08x,%08x\n",
		CVU_DOUT_PYM3_CFG, 0);

	Wr_Reg(base_reg + CVU_DOUT_PYM4_CFG, 0);
	ax_err(0, "CVU_DOUT_PYM4_CFG %08x,%08x\n",
		CVU_DOUT_PYM4_CFG, 0);

	Wr_Reg(base_reg + CVU_DOUT_PYM5_CFG, 0);
	ax_err(0, "CVU_DOUT_PYM5_CFG %08x,%08x\n",
		CVU_DOUT_PYM5_CFG, 0);

	Wr_Reg(base_reg + CVU_DOUT_PYM6_CFG, 0);
	ax_err(0, "CVU_DOUT_PYM6_CFG %08x,%08x\n",
		CVU_DOUT_PYM6_CFG, 0);

	Wr_Reg(base_reg + CVU_DOUT_PYM7_CFG, 0);
	ax_err(0, "CVU_DOUT_PYM7_CFG %08x,%08x\n",
		CVU_DOUT_PYM7_CFG, 0);

	Wr_Reg(base_reg + CVU_RSZ_PYM0_CFG0, 0);
	ax_err(0, "CVU_RSZ_PYM0_CFG0 %08x,%08x\n",
		CVU_RSZ_PYM0_CFG0, 0);

	Wr_Reg(base_reg + CVU_RSZ_PYM1_CFG0, 0);
	ax_err(0, "CVU_RSZ_PYM1_CFG0 %08x,%08x\n",
		CVU_RSZ_PYM1_CFG0, 0);

	Wr_Reg(base_reg + CVU_RSZ_PYM2_CFG0, 0);
	ax_err(0, "CVU_RSZ_PYM2_CFG0 %08x,%08x\n",
		CVU_RSZ_PYM2_CFG0, 0);

	Wr_Reg(base_reg + CVU_RSZ_PYM3_CFG0, 0);
	ax_err(0, "CVU_RSZ_PYM3_CFG0 %08x,%08x\n",
		CVU_RSZ_PYM3_CFG0, 0);

	Wr_Reg(base_reg + CVU_RSZ_PYM4_CFG0, 0);
	ax_err(0, "CVU_RSZ_PYM4_CFG0 %08x,%08x\n",
		CVU_RSZ_PYM4_CFG0, 0);

	Wr_Reg(base_reg + CVU_RSZ_PYM5_CFG0, 0);
	ax_err(0, "CVU_RSZ_PYM5_CFG0 %08x,%08x\n",
		CVU_RSZ_PYM5_CFG0, 0);

	Wr_Reg(base_reg + CVU_RSZ_PYM6_CFG0, 0);
	ax_err(0, "CVU_RSZ_PYM6_CFG0 %08x,%08x\n",
		CVU_RSZ_PYM6_CFG0, 0);

	Wr_Reg(base_reg + CVU_RSZ_PYM7_CFG0, 0);
	ax_err(0, "CVU_RSZ_PYM7_CFG0 %08x,%08x\n",
		CVU_RSZ_PYM7_CFG0, 0);


	Wr_Reg(base_reg + CVU_RSZ_PYM0_CFG1, 0);
	ax_err(0, "CVU_RSZ_PYM0_CFG1 %08x,%08x\n",
		CVU_RSZ_PYM0_CFG1, 0);

	Wr_Reg(base_reg + CVU_RSZ_PYM1_CFG1, 0);
	ax_err(0, "CVU_RSZ_PYM1_CFG1 %08x,%08x\n",
		CVU_RSZ_PYM1_CFG1, 0);

	Wr_Reg(base_reg + CVU_RSZ_PYM2_CFG1, 0);
	ax_err(0, "CVU_RSZ_PYM2_CFG1 %08x,%08x\n",
		CVU_RSZ_PYM2_CFG1, 0);

	Wr_Reg(base_reg + CVU_RSZ_PYM3_CFG1, 0);
	ax_err(0, "CVU_RSZ_PYM3_CFG1 %08x,%08x\n",
		CVU_RSZ_PYM3_CFG1, 0);

	Wr_Reg(base_reg + CVU_RSZ_PYM4_CFG1, 0);
	ax_err(0, "CVU_RSZ_PYM4_CFG1 %08x,%08x\n",
		CVU_RSZ_PYM4_CFG1, 0);

	Wr_Reg(base_reg + CVU_RSZ_PYM5_CFG1, 0);
	ax_err(0, "CVU_RSZ_PYM5_CFG1 %08x,%08x\n",
		CVU_RSZ_PYM5_CFG1, 0);

	Wr_Reg(base_reg + CVU_RSZ_PYM6_CFG1, 0);
	ax_err(0, "CVU_RSZ_PYM6_CFG1 %08x,%08x\n",
		CVU_RSZ_PYM6_CFG1, 0);

	Wr_Reg(base_reg + CVU_RSZ_PYM7_CFG1, 0);
	ax_err(0, "CVU_RSZ_PYM7_CFG1 %08x,%08x\n",
		CVU_RSZ_PYM7_CFG1, 0);

	Wr_Reg(base_reg + CVU_RSZ_ADDR, 0);
	ax_err(0, "CVU_RSZ_ADDR %08x,%08x\n", CVU_RSZ_ADDR, 0);

	/* Wr_Reg(CVU_DIN_CFG0,cvu_clear.dwval); */
	Wr_Reg(base_reg + CVU_INT_EN, 0);
	ax_err(0, "CVU_INT_EN %08x,%08x\n", CVU_INT_EN, 0);

	Wr_Reg(base_reg + CVU_CTL, 0);
	ax_err(0, "CVU_CTL %08x,%08x\n", CVU_CTL, 0);
}