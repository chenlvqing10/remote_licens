/* viss.h */

#ifndef ___VISS___H___
#define ___VISS___H___

#define BASE_VISS                0x01400000

#define VA_VISS_VER                      (0x00000000 + BASE_VISS + VA_VISS)
#define VA_VISS_FTR                      (0x00000004 + BASE_VISS + VA_VISS)
#define VA_VISS_VIC_CFG                  (0x00000010 + BASE_VISS + VA_VISS)
#define VA_VISS_MCSI_CFG                 (0x00000014 + BASE_VISS + VA_VISS)
#define VA_VISS_TVD_CFG                  (0x00000020 + BASE_VISS + VA_VISS)
#define VA_VISS_ISP_CFG                  (0x00000030 + BASE_VISS + VA_VISS)
#define VA_VISS_ISP_LITE_CFG             (0x00000034 + BASE_VISS + VA_VISS)
#define VA_VISS_ISP_PIX_SEL              (0x00000040 + BASE_VISS + VA_VISS)
#define VA_VISS_PLL_MUX_CTRL             (0x000000A0 + BASE_VISS + VA_VISS)
#define VA_VISS_ADPLL_FAC                (0x000000B0 + BASE_VISS + VA_VISS)
#define VA_VISS_ADPLL_TUNE0              (0x000000B4 + BASE_VISS + VA_VISS)
#define VA_VISS_ADPLL_TUNE1              (0x000000B8 + BASE_VISS + VA_VISS)
#define VA_VISS_ADPLL_STAT               (0x000000BC + BASE_VISS + VA_VISS)
#define VA_VISS_MBIST_CFG                (0x00000100 + BASE_VISS + VA_VISS)
#define VA_VISS_AXI_RD_QOS_LVL           (0x00000200 + BASE_VISS + VA_VISS)
#define VA_VISS_AXI_WT_QOS_LVL           (0x00000204 + BASE_VISS + VA_VISS)

#define DATA_VISS_VER                    0x00001101
#define DATA_VISS_FTR                    0x00000313
#define DATA_VISS_VIC_CFG                0x00000000
#define DATA_VISS_MCSI_CFG               0x00000000
#define DATA_VISS_TVD_CFG                0x00000000
#define DATA_VISS_ISP_CFG                0x00000000
#define DATA_VISS_ISP_LITE_CFG           0x00000000
#define DATA_VISS_ISP_PIX_SEL            0x00000000
#define DATA_VISS_PLL_MUX_CTRL           0x00000000
#define DATA_VISS_ADPLL_FAC              0x00000000
#define DATA_VISS_ADPLL_TUNE0            0x13000000
#define DATA_VISS_ADPLL_TUNE1            0x37079111
#define DATA_VISS_ADPLL_STAT             0x00000000
#define DATA_VISS_MBIST_CFG              0x00000000
#define DATA_VISS_AXI_RD_QOS_LVL         0x00F00000
#define DATA_VISS_AXI_WT_QOS_LVL         0x00F00000

/* VISS Version Register */
typedef union {
	u32 val;
	struct {
	u32 ver_l:5;           /* */
	u32 rsvd0:3;           /* */
	u32 ver_h:3;           /* */
	u32 rsvd1:1;           /* */
	u32 comp:1;            /* */
	u32 rsvd2:19;          /* */
	} bits;
} reg_viss_ver_t;

/* Feature Register */
typedef union {
	u32 val;
	struct {
	u32 has_vic:1;                 /* */
	u32 has_mcsi:1;                /* */
	u32 rsvd0:2;                   /* */
	u32 has_tvd:1;                 /* */
	u32 rsvd1:3;                   /* */
	u32 has_isp:1;                 /* */
	u32 has_isp_lite:1;            /* */
	u32 rsvd2:22;                  /* */
	} bits;
} reg_viss_ftr_t;

#define VISS_FTR_HAS_VIC_0      0x0
#define VISS_FTR_HAS_VIC_1      0x1
#define VISS_FTR_HAS_MCSI_0     0x0
#define VISS_FTR_HAS_MCSI_1     0x1
#define VISS_FTR_HAS_TVD_0      0x0
#define VISS_FTR_HAS_TVD_1      0x1
#define VISS_FTR_HAS_ISP_0      0x0
#define VISS_FTR_HAS_ISP_1      0x1
#define VISS_FTR_HAS_ISP_LITE_0 0x0
#define VISS_FTR_HAS_ISP_LITE_1 0x1

/* VIC Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 rst:1;             /* */
	u32 hclk_gat:1;        /* */
	u32 clk_gat:1;         /* */
	u32 clk_src:1;         /* */
	u32 mclk_gat:1;        /* */
	u32 mclk_src:1;        /* */
	u32 rsvd0:2;           /* */
	u32 clk_dir:4;         /* */
	u32 rsvd1:4;           /* */
	u32 mclk_dir:5;        /* */
	u32 rsvd2:11;          /* */
	} bits;
} reg_viss_vic_cfg_t;

#define VISS_VIC_CFG_RST_0      0x0
#define VISS_VIC_CFG_RST_1      0x1
#define VISS_VIC_CFG_HCLK_GAT_0 0x0
#define VISS_VIC_CFG_HCLK_GAT_1 0x1
#define VISS_VIC_CFG_CLK_GAT_0  0x0
#define VISS_VIC_CFG_CLK_GAT_1  0x1
#define VISS_VIC_CFG_CLK_SRC_0  0x0
#define VISS_VIC_CFG_CLK_SRC_1  0x1
#define VISS_VIC_CFG_MCLK_GAT_0 0x0
#define VISS_VIC_CFG_MCLK_GAT_1 0x1
#define VISS_VIC_CFG_MCLK_SRC_0 0x0
#define VISS_VIC_CFG_MCLK_SRC_1 0x1

/* MIPI_CSI Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 rst:1;                     /* */
	u32 hclk_gat:1;                /* */
	u32 clk_gat:1;                 /* */
	u32 clk_src:1;                 /* */
	u32 mclk_gat:1;                /* */
	u32 mclk_src:1;                /* */
	u32 cfgclk_gat:1;              /* */
	u32 rsvd0:1;                   /* */
	u32 clk_dir:4;                 /* */
	u32 rsvd1:4;                   /* */
	u32 mclk_dir:5;                /* */
	u32 rsvd2:11;                  /* */
	} bits;
} reg_viss_mcsi_cfg_t;

#define VISS_MCSI_CFG_RST_0             0x0
#define VISS_MCSI_CFG_RST_1             0x1
#define VISS_MCSI_CFG_HCLK_GAT_0        0x0
#define VISS_MCSI_CFG_HCLK_GAT_1        0x1
#define VISS_MCSI_CFG_CLK_GAT_0         0x0
#define VISS_MCSI_CFG_CLK_GAT_1         0x1
#define VISS_MCSI_CFG_CLK_SRC_0         0x0
#define VISS_MCSI_CFG_CLK_SRC_1         0x1
#define VISS_MCSI_CFG_MCLK_GAT_0        0x0
#define VISS_MCSI_CFG_MCLK_GAT_1        0x1
#define VISS_MCSI_CFG_MCLK_SRC_0        0x0
#define VISS_MCSI_CFG_MCLK_SRC_1        0x1
#define VISS_MCSI_CFG_CFGCLK_GAT_0      0x0
#define VISS_MCSI_CFG_CFGCLK_GAT_1      0x1

/* TVD Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 rst:1;             /* */
	u32 hclk_gat:1;        /* */
	u32 clk_gat:1;         /* */
	u32 clk_src:1;         /* */
	u32 rsvd0:4;           /* */
	u32 clk_dir:5;         /* */
	u32 rsvd1:19;          /* */
	} bits;
} reg_viss_tvd_cfg_t;

#define VISS_TVD_CFG_RST_0      0x0
#define VISS_TVD_CFG_RST_1      0x1
#define VISS_TVD_CFG_HCLK_GAT_0 0x0
#define VISS_TVD_CFG_HCLK_GAT_1 0x1
#define VISS_TVD_CFG_CLK_GAT_0  0x0
#define VISS_TVD_CFG_CLK_GAT_1  0x1
#define VISS_TVD_CFG_CLK_SRC_0  0x0
#define VISS_TVD_CFG_CLK_SRC_1  0x1

/* ISP Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 rst:1;             /* */
	u32 hclk_gat:1;        /* */
	u32 clk_gat:1;         /* */
	u32 clk_src:1;         /* */
	u32 rsvd0:4;           /* */
	u32 clk_dir:4;         /* */
	u32 rsvd1:20;          /* */
	} bits;
} reg_viss_isp_cfg_t;

#define VISS_ISP_CFG_RST_0      0x0
#define VISS_ISP_CFG_RST_1      0x1
#define VISS_ISP_CFG_HCLK_GAT_0 0x0
#define VISS_ISP_CFG_HCLK_GAT_1 0x1
#define VISS_ISP_CFG_CLK_GAT_0  0x0
#define VISS_ISP_CFG_CLK_GAT_1  0x1
#define VISS_ISP_CFG_CLK_SRC_0  0x0
#define VISS_ISP_CFG_CLK_SRC_1  0x1

/* ISP Lite Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 rst:1;             /* */
	u32 hclk_gat:1;        /* */
	u32 clk_gat:1;         /* */
	u32 clk_src:1;         /* */
	u32 rsvd0:4;           /* */
	u32 clk_dir:4;         /* */
	u32 rsvd1:20;          /* */
	} bits;
} reg_viss_isp_lite_cfg_t;

#define VISS_ISP_LITE_CFG_RST_0         0x0
#define VISS_ISP_LITE_CFG_RST_1         0x1
#define VISS_ISP_LITE_CFG_HCLK_GAT_0    0x0
#define VISS_ISP_LITE_CFG_HCLK_GAT_1    0x1
#define VISS_ISP_LITE_CFG_CLK_GAT_0     0x0
#define VISS_ISP_LITE_CFG_CLK_GAT_1     0x1
#define VISS_ISP_LITE_CFG_CLK_SRC_0     0x0
#define VISS_ISP_LITE_CFG_CLK_SRC_1     0x1

/* ISP Pixel Channel Select Register */
typedef union {
	u32 val;
	struct {
	u32 isp_pix_sel:1;             /* */
	u32 ispl_pix_sel:1;            /* */
	u32 rsvd0:30;                  /* */
	} bits;
} reg_viss_isp_pix_sel_t;

#define VISS_ISP_PIX_SEL_ISP_PIX_SEL_0  0x0
#define VISS_ISP_PIX_SEL_ISP_PIX_SEL_1  0x1
#define VISS_ISP_PIX_SEL_ISPL_PIX_SEL_0 0x0
#define VISS_ISP_PIX_SEL_ISPL_PIX_SEL_1 0x1

/* PLL_MUX Control Register */
typedef union {
	u32 val;
	struct {
	u32 pllmux_clk_sel:1;          /* */
	u32 rsvd0:3;                   /* */
	u32 adpll_pre_div:1;           /* */
	u32 rsvd1:27;                  /* */
	} bits;
} reg_viss_pll_mux_ctrl_t;

#define VISS_PLL_MUX_CTRL_PLLMUX_CLK_SEL_0      0x0
#define VISS_PLL_MUX_CTRL_PLLMUX_CLK_SEL_1      0x1
#define VISS_PLL_MUX_CTRL_ADPLL_PRE_DIV_0       0x0
#define VISS_PLL_MUX_CTRL_ADPLL_PRE_DIV_1       0x1

/* ADPLL Factor Register */
typedef union {
	u32 val;
	struct {
	u32 n:8;               /* */
	u32 rsvd0:23;          /* */
	u32 en:1;              /* */
	} bits;
} reg_viss_adpll_fac_t;

#define VISS_ADPLL_FAC_EN_0     0x0
#define VISS_ADPLL_FAC_EN_1     0x1

/* ADPLL Tune0 Register */
typedef union {
	u32 val;
	struct {
	u32 tune0:15;                  /* */
	u32 analog_rsvd00:1;           /* */
	u32 tune1:7;                   /* */
	u32 auto_adrange:1;            /* */
	u32 tune2:1;                   /* */
	u32 tune3:1;                   /* */
	u32 analog_rsvd21:2;           /* */
	u32 tune4:1;                   /* */
	u32 analog_rsvd32:3;           /* */
	} bits;
} reg_viss_adpll_tune0_t;

#define VISS_ADPLL_TUNE0_AUTO_ADRANGE_0 0x0
#define VISS_ADPLL_TUNE0_AUTO_ADRANGE_1 0x1
#define VISS_ADPLL_TUNE0_TUNE4_0        0x0
#define VISS_ADPLL_TUNE0_TUNE4_1        0x1

/* ADPLL Tune1 Register */
typedef union {
	u32 val;
	struct {
	u32 tune0:2;                   /* */
	u32 analog_rsvd00:2;           /* */
	u32 tune1:2;                   /* */
	u32 analog_rsvd11:2;           /* */
	u32 tune2:1;                   /* */
	u32 analog_rsvd22:3;           /* */
	u32 tune3:1;                   /* */
	u32 slewrate:3;                /* */
	u32 tune4:3;                   /* */
	u32 analog_rsvd43:1;           /* */
	u32 tune5:3;                   /* */
	u32 analog_rsvd54:1;           /* */
	u32 tune6:3;                   /* */
	u32 analog_rsvd65:1;           /* */
	u32 tune7:3;                   /* */
	u32 analog_rsvd76:1;           /* */
	} bits;
} reg_viss_adpll_tune1_t;

#define VISS_ADPLL_TUNE1_TUNE2_0        0x0
#define VISS_ADPLL_TUNE1_TUNE2_1        0x1
#define VISS_ADPLL_TUNE1_TUNE3_0        0x0
#define VISS_ADPLL_TUNE1_TUNE3_1        0x1
#define VISS_ADPLL_TUNE1_SLEWRATE_0     0x0
#define VISS_ADPLL_TUNE1_SLEWRATE_1     0x1
#define VISS_ADPLL_TUNE1_SLEWRATE_2     0x2
#define VISS_ADPLL_TUNE1_SLEWRATE_3     0x3
#define VISS_ADPLL_TUNE1_SLEWRATE_4     0x4
#define VISS_ADPLL_TUNE1_SLEWRATE_5     0x5
#define VISS_ADPLL_TUNE1_SLEWRATE_6     0x6
#define VISS_ADPLL_TUNE1_SLEWRATE_7     0x7

/* ADPLL Status Register */
typedef union {
	u32 val;
	struct {
	u32 ferr:9;                    /* */
	u32 analog_rsvd00:6;           /* */
	u32 flock:1;                   /* */
	u32 perr:7;                    /* */
	u32 analog_rsvd11:8;           /* */
	u32 plock:1;                   /* */
	} bits;
} reg_viss_adpll_stat_t;

#define VISS_ADPLL_STAT_FLOCK_0         0x0
#define VISS_ADPLL_STAT_FLOCK_1         0x1
#define VISS_ADPLL_STAT_PLOCK_0         0x0
#define VISS_ADPLL_STAT_PLOCK_1         0x1

/* MBIST Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 vic_cfg:1;                 /* */
	u32 mcsi_cfg:1;                /* */
	u32 rsvd0:2;                   /* */
	u32 tvd_cfg:1;                 /* */
	u32 rsvd1:3;                   /* */
	u32 isp_cfg:1;                 /* */
	u32 isp_lite_cfg:1;            /* */
	u32 rsvd2:6;                   /* */
	u32 vic_sta:1;                 /* */
	u32 mcsi_sta:1;                /* */
	u32 rsvd3:2;                   /* */
	u32 tvd_sta:1;                 /* */
	u32 rsvd4:3;                   /* */
	u32 isp_sta:1;                 /* */
	u32 isp_lite_sta:1;            /* */
	u32 rsvd5:6;                   /* */
	} bits;
} reg_viss_mbist_cfg_t;

#define VISS_MBIST_CFG_VIC_CFG_0        0x0
#define VISS_MBIST_CFG_VIC_CFG_1        0x1
#define VISS_MBIST_CFG_MCSI_CFG_0       0x0
#define VISS_MBIST_CFG_MCSI_CFG_1       0x1
#define VISS_MBIST_CFG_TVD_CFG_0        0x0
#define VISS_MBIST_CFG_TVD_CFG_1        0x1
#define VISS_MBIST_CFG_ISP_CFG_0        0x0
#define VISS_MBIST_CFG_ISP_CFG_1        0x1
#define VISS_MBIST_CFG_ISP_LITE_CFG_0   0x0
#define VISS_MBIST_CFG_ISP_LITE_CFG_1   0x1
#define VISS_MBIST_CFG_VIC_STA_0        0x0
#define VISS_MBIST_CFG_VIC_STA_1        0x1
#define VISS_MBIST_CFG_MCSI_STA_0       0x0
#define VISS_MBIST_CFG_MCSI_STA_1       0x1
#define VISS_MBIST_CFG_TVD_STA_0        0x0
#define VISS_MBIST_CFG_TVD_STA_1        0x1
#define VISS_MBIST_CFG_ISP_STA_0        0x0
#define VISS_MBIST_CFG_ISP_STA_1        0x1
#define VISS_MBIST_CFG_ISP_LITE_STA_0   0x0
#define VISS_MBIST_CFG_ISP_LITE_STA_1   0x1

/* AXI Read QoS Level Register */
typedef union {
	u32 val;
	struct {
	u32 vic:4;             /* */
	u32 mcsi:4;            /* */
	u32 isp:4;             /* */
	u32 isp_lite:4;        /* */
	u32 tvd_wb:4;          /* */
	u32 tvd_3d:4;          /* */
	u32 rsvd0:8;           /* */
	} bits;
} reg_viss_axi_rd_qos_lvl_t;

/* AXI Write QoS Level Register */
typedef union {
	u32 val;
	struct {
	u32 vic:4;             /* */
	u32 mcsi:4;            /* */
	u32 isp:4;             /* */
	u32 isp_lite:4;        /* */
	u32 tvd_wb:4;          /* */
	u32 tvd_3d:4;          /* */
	u32 rsvd0:8;           /* */
	} bits;
} reg_viss_axi_wt_qos_lvl_t;

/* reg_viss_t bank */
typedef struct tag_viss {
	reg_viss_ver_t             ver;            /* 0000 */
	reg_viss_ftr_t             ftr;            /* 0004 */
	u32                        res0[2];
	reg_viss_vic_cfg_t         vic_cfg;        /* 0010 */
	reg_viss_mcsi_cfg_t        mcsi_cfg;       /* 0014 */
	u32                        res1[2];
	reg_viss_tvd_cfg_t         tvd_cfg;        /* 0020 */
	u32                        res2[3];
	reg_viss_isp_cfg_t         isp_cfg;        /* 0030 */
	reg_viss_isp_lite_cfg_t    isp_lite_cfg;   /* 0034 */
	u32                        res3[2];
	reg_viss_isp_pix_sel_t     isp_pix_sel;    /* 0040 */
	u32                        res4[23];
	reg_viss_pll_mux_ctrl_t    pll_mux_ctrl;   /* 00A0 */
	u32                        res5[3];
	reg_viss_adpll_fac_t       adpll_fac;      /* 00B0 */
	reg_viss_adpll_tune0_t     adpll_tune0;    /* 00B4 */
	reg_viss_adpll_tune1_t     adpll_tune1;    /* 00B8 */
	reg_viss_adpll_stat_t      adpll_stat;     /* 00BC */
	u32                        res6[16];
	reg_viss_mbist_cfg_t       mbist_cfg;      /* 0100 */
	u32                        res7[63];
	reg_viss_axi_rd_qos_lvl_t  axi_rd_qos_lvl; /* 0200 */
	reg_viss_axi_wt_qos_lvl_t  axi_wt_qos_lvl; /* 0204 */
} reg_viss_t;

#endif /* ___VISS___H___ */
