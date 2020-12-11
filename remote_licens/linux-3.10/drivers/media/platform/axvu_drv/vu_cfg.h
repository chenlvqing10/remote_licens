#ifndef __VU_CFG_H__
#define __VU_CFG_H__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include "vu_drv.h"

typedef enum _IMAGE_FORMAT {
	RGB_WHC = 0x01,
	YUV420 = 0x02,
	GRAY = 0x03,
	GRAY_WHC = 0x04,
	YVU420 = 0x05,
	RGB = 0x6,
} IMAGE_FORMAT;

typedef union {
	unsigned int dwval;
	struct {
		unsigned int start:1;
		unsigned int rvd0:11;
		unsigned int op_mode:2;
		unsigned int rvd1:2;
		unsigned int pym_num:4;
		unsigned int rvd2:9;
		unsigned int ot_detect_en:1;
		unsigned int dbg_ctl:1;
		unsigned int rst:1;
	} bits;
} cvu_clt_t;

/* 0x18 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int rvd0:4;
		unsigned int tmp_status:2;
		unsigned int rvd1:26;
	} bits;
} cvu_status_t;

/* 0x20 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int int_en:1;
		unsigned int rvd0:3;
		unsigned int para_err:1;
		unsigned int time_out_err:1;
		unsigned int rvd1:10;
		unsigned int time_out_cnt:16;
	} bits;
} cvu_irq_en_t;

/* 0x24 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int int_pd:1;
		unsigned int rvd0:3;
		unsigned int para_pd:1;
		unsigned int time_out_pd:1;
		unsigned int rvd1:26;
	} bits;
} cvu_irq_pd_t;

/* 0x28 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int cv_clr:1;
		unsigned int rvd0:3;
		unsigned int para_clr:1;
		unsigned int time_out_clr:1;
		unsigned int rvd1:26;
	} bits;
} cvu_irq_clr_t;

/* 0x30 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int in_fmt:3;
		unsigned int in_chn:11;
		unsigned int normal_flag:1;
		unsigned int rvd0:9;
		unsigned int stride_mode:3;
		unsigned int stride_h:5;
	} bits;
} rsz_datain_cfg0_t;

/* 0x34 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int y_stride:12; /* *32 */
		unsigned int rvd0:4;
		unsigned int uv_stride:12; /* *32 */
		unsigned int rvd1:4;
	} bits;
} rsz_datain_cfg1_t;

/* 0x38 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int in_width:12;
		unsigned int in_height:12; /* normal : offset y odd, +1 */
		unsigned int rvd0:8;
	} bits;
} rsz_datain_cfg2_t;

/* 0x3c */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int in_addr:32; /* normal : offset y odd,addr - line */
	} bits;
} rsz_datain_addr_t;

/* 0x40 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int in_uv_addr:32; /* normal: y_ddr/2 */
	} bits;
} rsz_datain_uvaddr_t;

/* 0x50 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int out_fmt:3;
		unsigned int out_chn:11;
		unsigned int t_flag:1;
		unsigned int rvd0:1;
		unsigned int yuv420_flag:1;
		unsigned int rvd1:15;
	} bits;
} rsz_dataout_cfg0_t;

/* 0x54  only normal */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int out_width:12; /* -1 */
		unsigned int out_height:12; /* -1 */
		unsigned int out_stride_mode:3;
		unsigned int out_stride_h:5;
	} bits;
} rsz_dataout_cfg1_t;

/* 0x60 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int out_pym_width:12; /* -1 */
		unsigned int out_pym_height:12; /* -1 */
		unsigned int out_pym_stride_mode:3;
		unsigned int out_pym_stride_h:5;
	} bits;
} rsz_dataout_pym0_cfg_t;

/* 0x64 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int out_pym_width:12; /* -1 */
		unsigned int out_pym_height:12; /* -1 */
		unsigned int out_pym_stride_mode:3;
		unsigned int out_pym_stride_h:5;
	} bits;
} rsz_dataout_pym1_cfg_t;

/* 0x68 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int out_pym_width:12; /* -1 */
		unsigned int out_pym_height:12; /* -1 */
		unsigned int out_pym_stride_mode:3;
		unsigned int out_pym_stride_h:5;
	} bits;
} rsz_dataout_pym2_cfg_t;

/* 0x6c */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int out_pym_width:12; /* -1 */
		unsigned int out_pym_height:12; /* -1 */
		unsigned int out_pym_stride_mode:3;
		unsigned int out_pym_stride_h:5;
	} bits;
} rsz_dataout_pym3_cfg_t;

/* 0x70 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int out_pym_width:12; /* -1 */
		unsigned int out_pym_height:12; /* -1 */
		unsigned int out_pym_stride_mode:3;
		unsigned int out_pym_stride_h:5;
	} bits;
} rsz_dataout_pym4_cfg_t;

/* 0x74 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int out_pym_width:12; /* -1 */
		unsigned int out_pym_height:12; /* -1 */
		unsigned int out_pym_stride_mode:3;
		unsigned int out_pym_stride_h:5;
	} bits;
} rsz_dataout_pym5_cfg_t;

/* 0x78 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int out_pym_width:12; /* -1 */
		unsigned int out_pym_height:12; /* -1 */
		unsigned int out_pym_stride_mode:3;
		unsigned int out_pym_stride_h:5;
	} bits;
} rsz_dataout_pym6_cfg_t;

/* 0x7c */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int out_pym_width:12; /* -1 */
		unsigned int out_pym_height:12; /* -1 */
		unsigned int out_pym_stride_mode:3;
		unsigned int out_pym_stride_h:5;
	} bits;
} rsz_dataout_pym7_cfg_t;

/* 0x80 normal */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int out_addr:32;
	} bits;
} rsz_dataout_addr_t;

/* 0x90 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int offset_x:12;
		unsigned int offset_y_flag:1;
		unsigned int rvd0:11;
		unsigned int flip_en:1;
		unsigned int pathway_en:1;
		unsigned int rvd1:6;
	} bits;
} rsz_cfg0_t;

/* 0x94 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int w_crop:12;
		unsigned int h_crop:12;
		unsigned int rvd0:8;
	} bits;
} rsz_cfg1_t;

/* 0x98  normal */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int scale_x:29;
		unsigned int rvd0:3;
	} bits;
} rsz_cfg2_t;

/* 0x9c */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int scale_y:29;
		unsigned int rvd0:3;
	} bits;
} rsz_cfg3_t;

/* 0xa0  pym0 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int scale_x:29;
		unsigned int rvd0:3;
	} bits;
} rsz_pym0_cfg0_t;

/* 0xa4 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int scale_y:29;
		unsigned int rvd0:3;
	} bits;
} rsz_pym0_cfg1_t;

/* 0xa8  pym1 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int scale_x:29;
		unsigned int rvd0:3;
	} bits;
} rsz_pym1_cfg0_t;

/* 0xac */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int scale_y:29;
		unsigned int rvd0:3;
	} bits;
} rsz_pym1_cfg1_t;

/* 0xb0  pym2 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int scale_x:29;
		unsigned int rvd0:3;
	} bits;
} rsz_pym2_cfg0_t;

/* 0xb4 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int scale_y:29;
		unsigned int rvd0:3;
	} bits;
} rsz_pym2_cfg1_t;

/* 0xb8  pym3 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int scale_x:29;
		unsigned int rvd0:3;
	} bits;
} rsz_pym3_cfg0_t;

/* 0xbc */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int scale_y:29;
		unsigned int rvd0:3;
	} bits;
} rsz_pym3_cfg1_t;

/* 0xc0  pym4 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int scale_x:29;
		unsigned int rvd0:3;
	} bits;
} rsz_pym4_cfg0_t;

/* 0xc4 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int scale_y:29;
		unsigned int rvd0:3;
	} bits;
} rsz_pym4_cfg1_t;

/* 0xc8  pym5 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int scale_x:29;
		unsigned int rvd0:3;
	} bits;
} rsz_pym5_cfg0_t;

/* 0xcc */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int scale_y:29;
		unsigned int rvd0:3;
	} bits;
} rsz_pym5_cfg1_t;

/* 0xd0  pym6 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int scale_x:29;
		unsigned int rvd0:3;
	} bits;
} rsz_pym6_cfg0_t;

/* 0xd4 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int scale_y:29;
		unsigned int rvd0:3;
	} bits;
} rsz_pym6_cfg1_t;

/* 0xd8  pym7 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int scale_x:29;
		unsigned int rvd0:3;
	} bits;
} rsz_pym7_cfg0_t;

/* 0xdc */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int scale_y:29;
		unsigned int rvd0:3;
	} bits;
} rsz_pym7_cfg1_t;

/* 0xe0 */
typedef union {
	unsigned int dwval;
	struct {
		unsigned int rsz_addr:32;
	} bits;
} rsz_addr_reg_t;

void set_rsz_cfg(rsz_cfg_t rsz_cfg, unsigned int base_reg);
void clear_rsz_cfg(unsigned int base_reg);
void set_pym_cfg(pym_cfg_t pym_cfg, unsigned int base_reg);
void clear_pym_cfg(unsigned int base_reg);

#endif
