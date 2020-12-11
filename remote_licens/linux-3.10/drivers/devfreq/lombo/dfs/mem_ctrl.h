/* mem_ctrl.h */

#ifndef ___MEM_CTRL___H___
#define ___MEM_CTRL___H___

#define BASE_MEM_CTRL                0x0121B000

#define VA_MEM_CTRL_VER                      (0x00000000 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_MON_MODE_CTRL            (0x00000004 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_BW_CTRL                  (0x00000008 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_INT_EN                   (0x0000000C + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_UNITS_CFG                (0x00000010 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_MON_STATUS               (0x00000014 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_BW_STM                   (0x00000018 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_RD_LAT_OF                (0x00000020 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_WR_LAT_OF                (0x00000024 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_TOTAL_DAT                (0x00000040 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_TOTAL_RDAT               (0x00000044 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_TOTAL_WDAT               (0x00000048 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_TOTAL_RPDAT              (0x00000050 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_TOTAL_WPDAT              (0x00000054 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M0_RADAT            (0x00000080 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M1_RADAT            (0x00000090 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M2_RADAT            (0x000000A0 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M3_RADAT            (0x000000B0 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M4_RADAT            (0x000000C0 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M5_RADAT            (0x000000D0 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M6_RADAT            (0x000000E0 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M0_WADAT            (0x00000084 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M1_WADAT            (0x00000094 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M2_WADAT            (0x000000A4 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M3_WADAT            (0x000000B4 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M4_WADAT            (0x000000C4 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M5_WADAT            (0x000000D4 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M6_WADAT            (0x000000E4 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M0_RPDAT            (0x00000088 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M1_RPDAT            (0x00000098 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M2_RPDAT            (0x000000A8 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M3_RPDAT            (0x000000B8 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M4_RPDAT            (0x000000C8 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M5_RPDAT            (0x000000D8 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M6_RPDAT            (0x000000E8 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M0_WPDAT            (0x0000008C + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M1_WPDAT            (0x0000009C + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M2_WPDAT            (0x000000AC + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M3_WPDAT            (0x000000BC + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M4_WPDAT            (0x000000CC + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M5_WPDAT            (0x000000DC + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M6_WPDAT            (0x000000EC + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M0_RD_CMD_NUM       (0x00000200 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M1_RD_CMD_NUM       (0x00000210 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M2_RD_CMD_NUM       (0x00000220 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M3_RD_CMD_NUM       (0x00000230 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M4_RD_CMD_NUM       (0x00000240 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M5_RD_CMD_NUM       (0x00000250 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M6_RD_CMD_NUM       (0x00000260 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M0_RD_LATENCY       (0x00000204 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M1_RD_LATENCY       (0x00000214 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M2_RD_LATENCY       (0x00000224 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M3_RD_LATENCY       (0x00000234 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M4_RD_LATENCY       (0x00000244 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M5_RD_LATENCY       (0x00000254 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M6_RD_LATENCY       (0x00000264 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M0_WR_CMD_NUM       (0x00000208 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M1_WR_CMD_NUM       (0x00000218 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M2_WR_CMD_NUM       (0x00000228 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M3_WR_CMD_NUM       (0x00000238 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M4_WR_CMD_NUM       (0x00000248 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M5_WR_CMD_NUM       (0x00000258 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M6_WR_CMD_NUM       (0x00000268 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M0_WR_LATENCY       (0x0000020C + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M1_WR_LATENCY       (0x0000021C + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M2_WR_LATENCY       (0x0000022C + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M3_WR_LATENCY       (0x0000023C + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M4_WR_LATENCY       (0x0000024C + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M5_WR_LATENCY       (0x0000025C + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_M6_WR_LATENCY       (0x0000026C + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_MEM_PI_GAT               (0x00001000 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_MEM_PI_RST               (0x00001004 + BASE_MEM_CTRL + VA_MEM_CTRL)
#define VA_MEM_CTRL_AXIM_DBG                 (0x00001040 + BASE_MEM_CTRL + VA_MEM_CTRL)

#define DATA_MEM_CTRL_VER                    0x00000100
#define DATA_MEM_CTRL_MON_MODE_CTRL          0x00000000
#define DATA_MEM_CTRL_BW_CTRL                0x00000000
#define DATA_MEM_CTRL_INT_EN                 0x00000000
#define DATA_MEM_CTRL_UNITS_CFG              0x00000000
#define DATA_MEM_CTRL_MON_STATUS             0x00000000
#define DATA_MEM_CTRL_BW_STM                 0x00000000
#define DATA_MEM_CTRL_RD_LAT_OF              0x00000000
#define DATA_MEM_CTRL_WR_LAT_OF              0x00000000
#define DATA_MEM_CTRL_TOTAL_DAT              0x00000000
#define DATA_MEM_CTRL_TOTAL_RDAT             0x00000000
#define DATA_MEM_CTRL_TOTAL_WDAT             0x00000000
#define DATA_MEM_CTRL_TOTAL_RPDAT            0x00000000
#define DATA_MEM_CTRL_TOTAL_WPDAT            0x00000000
#define DATA_MEM_CTRL_M0_RADAT          0x00000000
#define DATA_MEM_CTRL_M1_RADAT          0x00000000
#define DATA_MEM_CTRL_M2_RADAT          0x00000000
#define DATA_MEM_CTRL_M3_RADAT          0x00000000
#define DATA_MEM_CTRL_M4_RADAT          0x00000000
#define DATA_MEM_CTRL_M5_RADAT          0x00000000
#define DATA_MEM_CTRL_M6_RADAT          0x00000000
#define DATA_MEM_CTRL_M0_WADAT          0x00000000
#define DATA_MEM_CTRL_M1_WADAT          0x00000000
#define DATA_MEM_CTRL_M2_WADAT          0x00000000
#define DATA_MEM_CTRL_M3_WADAT          0x00000000
#define DATA_MEM_CTRL_M4_WADAT          0x00000000
#define DATA_MEM_CTRL_M5_WADAT          0x00000000
#define DATA_MEM_CTRL_M6_WADAT          0x00000000
#define DATA_MEM_CTRL_M0_RPDAT          0x00000000
#define DATA_MEM_CTRL_M1_RPDAT          0x00000000
#define DATA_MEM_CTRL_M2_RPDAT          0x00000000
#define DATA_MEM_CTRL_M3_RPDAT          0x00000000
#define DATA_MEM_CTRL_M4_RPDAT          0x00000000
#define DATA_MEM_CTRL_M5_RPDAT          0x00000000
#define DATA_MEM_CTRL_M6_RPDAT          0x00000000
#define DATA_MEM_CTRL_M0_WPDAT          0x00000000
#define DATA_MEM_CTRL_M1_WPDAT          0x00000000
#define DATA_MEM_CTRL_M2_WPDAT          0x00000000
#define DATA_MEM_CTRL_M3_WPDAT          0x00000000
#define DATA_MEM_CTRL_M4_WPDAT          0x00000000
#define DATA_MEM_CTRL_M5_WPDAT          0x00000000
#define DATA_MEM_CTRL_M6_WPDAT          0x00000000
#define DATA_MEM_CTRL_M0_RD_CMD_NUM     0x00000000
#define DATA_MEM_CTRL_M1_RD_CMD_NUM     0x00000000
#define DATA_MEM_CTRL_M2_RD_CMD_NUM     0x00000000
#define DATA_MEM_CTRL_M3_RD_CMD_NUM     0x00000000
#define DATA_MEM_CTRL_M4_RD_CMD_NUM     0x00000000
#define DATA_MEM_CTRL_M5_RD_CMD_NUM     0x00000000
#define DATA_MEM_CTRL_M6_RD_CMD_NUM     0x00000000
#define DATA_MEM_CTRL_M0_RD_LATENCY     0x00000000
#define DATA_MEM_CTRL_M1_RD_LATENCY     0x00000000
#define DATA_MEM_CTRL_M2_RD_LATENCY     0x00000000
#define DATA_MEM_CTRL_M3_RD_LATENCY     0x00000000
#define DATA_MEM_CTRL_M4_RD_LATENCY     0x00000000
#define DATA_MEM_CTRL_M5_RD_LATENCY     0x00000000
#define DATA_MEM_CTRL_M6_RD_LATENCY     0x00000000
#define DATA_MEM_CTRL_M0_WR_CMD_NUM     0x00000000
#define DATA_MEM_CTRL_M1_WR_CMD_NUM     0x00000000
#define DATA_MEM_CTRL_M2_WR_CMD_NUM     0x00000000
#define DATA_MEM_CTRL_M3_WR_CMD_NUM     0x00000000
#define DATA_MEM_CTRL_M4_WR_CMD_NUM     0x00000000
#define DATA_MEM_CTRL_M5_WR_CMD_NUM     0x00000000
#define DATA_MEM_CTRL_M6_WR_CMD_NUM     0x00000000
#define DATA_MEM_CTRL_M0_WR_LATENCY     0x00000000
#define DATA_MEM_CTRL_M1_WR_LATENCY     0x00000000
#define DATA_MEM_CTRL_M2_WR_LATENCY     0x00000000
#define DATA_MEM_CTRL_M3_WR_LATENCY     0x00000000
#define DATA_MEM_CTRL_M4_WR_LATENCY     0x00000000
#define DATA_MEM_CTRL_M5_WR_LATENCY     0x00000000
#define DATA_MEM_CTRL_M6_WR_LATENCY     0x00000000
#define DATA_MEM_CTRL_MEM_PI_GAT             0x00000000
#define DATA_MEM_CTRL_MEM_PI_RST             0x00000000
#define DATA_MEM_CTRL_AXIM_DBG               0x00000000

/* Memory Control Module Version Register */
typedef union {
	u32 val;
	struct {
	u32 ver_l:5;            /* The low 5bits of version register */
	u32 rsvd0:3;            /* - */
	u32 ver_h:3;            /* The high 3bits of version register */
	u32 rsvd1:1;            /* - */
	u32 comp:1;             /* Backward Compatibility
				 * [1¡¯b0]: Incompatible to last version
				 * of hardware
				 * [1¡¯b1]: Compatible to last version
				 * of hardware */
	u32 rsvd2:19;           /* - */
	} bits;
} reg_mem_ctrl_ver_t;

#define MEM_CTRL_VER_COMP_0     0x0
#define MEM_CTRL_VER_COMP_1     0x1

/* SDRAM Bandwidth Monitor Mode Control Register */
typedef union {
	u32 val;
	struct {
	u32 real:1;             /* REAL MODE ENABLE
				 * [1¡¯b0]: Manual Mode
				 * [1¡¯b1]: Real Mode
				 * Note: When in Real Mode, the corresponding
				 * monitor register indicates the recent
				 * sample-window value. */
	u32 rsvd0:31;           /* - */
	} bits;
} reg_mem_ctrl_mon_mode_ctrl_t;

#define MEM_CTRL_MON_MODE_CTRL_REAL_0   0x0
#define MEM_CTRL_MON_MODE_CTRL_REAL_1   0x1

/* SDRAM Bandwidth Monitor Control Register */
typedef union {
	u32 val;
	struct {
	u32 en:1;               /* Bandwidth monitor enable
				 * [1¡¯b0]: Stop
				 * [1¡¯b1]: Start
				 * When set to 1, the corresponding bandwidth
				 * monitor would be restarted. */
	u32 rsvd0:31;           /* - */
	} bits;
} reg_mem_ctrl_bw_ctrl_t;

#define MEM_CTRL_BW_CTRL_EN_0   0x0
#define MEM_CTRL_BW_CTRL_EN_1   0x1

/* Memory Control Interrupt Enable Control Register */
typedef union {
	u32 val;
	struct {
	u32 ov_en:1;            /* [1¡¯b0]: Disable
				 * [1¡¯b1]: Enable
				 * Enable the interrupt that any monitor
				 * overflow */
	u32 rsvd0:31;           /* - */
	} bits;
} reg_mem_ctrl_int_en_t;

#define MEM_CTRL_INT_EN_OV_EN_0 0x0
#define MEM_CTRL_INT_EN_OV_EN_1 0x1

/* Monitor Units Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 sample:3;           /* [3¡¯b000]: 1ms
				 * [3¡¯b001]: 10ms
				 * [3¡¯b010]: 100ms
				 * [3¡¯b011]: 1s
				 * [3¡¯b100]: 10s
				 * [3¡¯b101]-[3¡¯b111]: RSVD
				 * Note: Sample cycle time-widow unit */
	u32 rsvd0:5;            /* - */
	u32 data:3;             /* [3¡¯b000]: 1Byte
				 * [3¡¯b001]: 16Bytes
				 * [3¡¯b010]: 64Bytes
				 * [3¡¯b011]: 256Bytes
				 * [3¡¯b100]: 1KBytes
				 * [3¡¯b101]: 4KBytes
				 * [3¡¯b110]: 64KBytes
				 * [3¡¯b111]: 128KBytes
				 * Note: Data Size unit in data-flow case */
	u32 rsvd1:1;            /* - */
	u32 rsvd2:4;            /* - */
	u32 clk_sel:2;          /* [2¡¯b00]: DISABLE
				 * [2¡¯b01]: 24MHz
				 * [2¡¯b10]- [2¡¯b11]:RSVD
				 * Note: software must correctly set this
				 * bit field. The clock control register
				 * in PRCM. */
	u32 rsvd3:14;           /* - */
	} bits;
} reg_mem_ctrl_units_cfg_t;

#define MEM_CTRL_UNITS_CFG_SAMPLE_0     0x0
#define MEM_CTRL_UNITS_CFG_SAMPLE_1     0x1
#define MEM_CTRL_UNITS_CFG_SAMPLE_2     0x2
#define MEM_CTRL_UNITS_CFG_SAMPLE_3     0x3
#define MEM_CTRL_UNITS_CFG_SAMPLE_4     0x4
#define MEM_CTRL_UNITS_CFG_DATA_0       0x0
#define MEM_CTRL_UNITS_CFG_DATA_1       0x1
#define MEM_CTRL_UNITS_CFG_DATA_2       0x2
#define MEM_CTRL_UNITS_CFG_DATA_3       0x3
#define MEM_CTRL_UNITS_CFG_DATA_4       0x4
#define MEM_CTRL_UNITS_CFG_DATA_5       0x5
#define MEM_CTRL_UNITS_CFG_DATA_6       0x6
#define MEM_CTRL_UNITS_CFG_DATA_7       0x7
#define MEM_CTRL_UNITS_CFG_CLK_SEL_0    0x0
#define MEM_CTRL_UNITS_CFG_CLK_SEL_1    0x1

/* Monitor Status Register */
typedef union {
	u32 val;
	struct {
	u32 data_of:1;          /* [1¡¯b0]: Not-pending
				 * [1¡¯b1]: Pending
				 * Any data flow monitor overflow */
	u32 cycles_of:1;        /* [1¡¯b0]: Not-pending
				 * [1¡¯b1]: Pending
				 * The cycles in BW_STM register overflow */
	u32 rsvd0:30;           /* - */
	} bits;
} reg_mem_ctrl_mon_status_t;

#define MEM_CTRL_MON_STATUS_DATA_OF_0   0x0
#define MEM_CTRL_MON_STATUS_DATA_OF_1   0x1
#define MEM_CTRL_MON_STATUS_CYCLES_OF_0 0x0
#define MEM_CTRL_MON_STATUS_CYCLES_OF_1 0x1

/* Bandwidth Statistic Time Register */
typedef union {
	u32 val;
	struct {
	u32 cycles:32;          /* It indicates the number of sample cycles
				 * in a specified statistic window. The
				 * sample cycle unit refer to UNITS_CFG
				 * register. */
	} bits;
} reg_mem_ctrl_bw_stm_t;

/* Read Latency Counter Overflow Status Register */
typedef union {
	u32 val;
	struct {
	u32 cnt_of:7;           /* [1¡¯b0]: Not-pending
				 * [1¡¯b1]: Pending
				 * Master [m] read latency counter overflow.
				 * Each bit stands for a corresponding
				 * master counter */
	u32 rsvd0:25;           /* - */
	} bits;
} reg_mem_ctrl_rd_lat_of_t;

#define MEM_CTRL_RD_LAT_OF_CNT_OF_0     0x0
#define MEM_CTRL_RD_LAT_OF_CNT_OF_1     0x1

/* Write Latency Counter Overflow Status Register */
typedef union {
	u32 val;
	struct {
	u32 cnt_of:7;           /* [1¡¯b0]: Not-pending
				 * [1¡¯b1]: Pending
				 * Master [m] write latency counter overflow.
				 * Each bit stands for a corresponding
				 * master counter */
	u32 rsvd0:25;           /* - */
	} bits;
} reg_mem_ctrl_wr_lat_of_t;

#define MEM_CTRL_WR_LAT_OF_CNT_OF_0     0x0
#define MEM_CTRL_WR_LAT_OF_CNT_OF_1     0x1

/* Total SDRAM Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the total sdram data flow
				 * in a specified statistic window. The
				 * unit refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_total_dat_t;

/* Total SDRAM Read Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the total sdram read data
				 * flow in a specified statistic window.
				 * The unit refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_total_rdat_t;

/* Total SDRAM Write Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the total sdram write data
				 * flow in a specified statistic window.
				 * The unit refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_total_wdat_t;

/* Total SDRAM Read Peak Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the total read data flow
				 * in a specified sample window. The unit
				 * refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_total_rpdat_t;

/* Total SDRAM Write Peak Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the total write data flow
				 * in a specified sample window. The unit
				 * refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_total_wpdat_t;

/* Master[CH] Read Accumulation Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master read data flow
				 * in a specified statistic window. The
				 * unit refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m0_radat_t;

/* Master[CH] Read Accumulation Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master read data flow
				 * in a specified statistic window. The
				 * unit refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m1_radat_t;

/* Master[CH] Read Accumulation Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master read data flow
				 * in a specified statistic window. The
				 * unit refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m2_radat_t;

/* Master[CH] Read Accumulation Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master read data flow
				 * in a specified statistic window. The
				 * unit refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m3_radat_t;

/* Master[CH] Read Accumulation Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master read data flow
				 * in a specified statistic window. The
				 * unit refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m4_radat_t;

/* Master[CH] Read Accumulation Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master read data flow
				 * in a specified statistic window. The
				 * unit refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m5_radat_t;

/* Master[CH] Read Accumulation Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master read data flow
				 * in a specified statistic window. The
				 * unit refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m6_radat_t;

/* Master[CH] Write Accumulation Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master write data flow
				 * in a specified statistic window. The
				 * unit refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m0_wadat_t;

/* Master[CH] Write Accumulation Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master write data flow
				 * in a specified statistic window. The
				 * unit refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m1_wadat_t;

/* Master[CH] Write Accumulation Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master write data flow
				 * in a specified statistic window. The
				 * unit refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m2_wadat_t;

/* Master[CH] Write Accumulation Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master write data flow
				 * in a specified statistic window. The
				 * unit refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m3_wadat_t;

/* Master[CH] Write Accumulation Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master write data flow
				 * in a specified statistic window. The
				 * unit refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m4_wadat_t;

/* Master[CH] Write Accumulation Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master write data flow
				 * in a specified statistic window. The
				 * unit refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m5_wadat_t;

/* Master[CH] Write Accumulation Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master write data flow
				 * in a specified statistic window. The
				 * unit refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m6_wadat_t;

/* Master[CH] Read Peak Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master read data flow
				 * in a specified sample window. The unit
				 * refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m0_rpdat_t;

/* Master[CH] Read Peak Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master read data flow
				 * in a specified sample window. The unit
				 * refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m1_rpdat_t;

/* Master[CH] Read Peak Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master read data flow
				 * in a specified sample window. The unit
				 * refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m2_rpdat_t;

/* Master[CH] Read Peak Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master read data flow
				 * in a specified sample window. The unit
				 * refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m3_rpdat_t;

/* Master[CH] Read Peak Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master read data flow
				 * in a specified sample window. The unit
				 * refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m4_rpdat_t;

/* Master[CH] Read Peak Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master read data flow
				 * in a specified sample window. The unit
				 * refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m5_rpdat_t;

/* Master[CH] Read Peak Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master read data flow
				 * in a specified sample window. The unit
				 * refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m6_rpdat_t;

/* Master[CH] Write Peak Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master write data flow
				 * in a specified sample window. The unit
				 * refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m0_wpdat_t;

/* Master[CH] Write Peak Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master write data flow
				 * in a specified sample window. The unit
				 * refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m1_wpdat_t;

/* Master[CH] Write Peak Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master write data flow
				 * in a specified sample window. The unit
				 * refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m2_wpdat_t;

/* Master[CH] Write Peak Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master write data flow
				 * in a specified sample window. The unit
				 * refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m3_wpdat_t;

/* Master[CH] Write Peak Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master write data flow
				 * in a specified sample window. The unit
				 * refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m4_wpdat_t;

/* Master[CH] Write Peak Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master write data flow
				 * in a specified sample window. The unit
				 * refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m5_wpdat_t;

/* Master[CH] Write Peak Data Flow Register */
typedef union {
	u32 val;
	struct {
	u32 size:32;            /* It indicates the master write data flow
				 * in a specified sample window. The unit
				 * refer to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m6_wpdat_t;

/* Master[CH] Read-Command Numbers Register */
typedef union {
	u32 val;
	struct {
	u32 num:32;     /* It indicates the master READ command numbers
			 * in a specified sample window. The unit refer
			 * to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m0_rd_cmd_num_t;

/* Master[CH] Read-Command Numbers Register */
typedef union {
	u32 val;
	struct {
	u32 num:32;     /* It indicates the master READ command numbers
			 * in a specified sample window. The unit refer
			 * to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m1_rd_cmd_num_t;

/* Master[CH] Read-Command Numbers Register */
typedef union {
	u32 val;
	struct {
	u32 num:32;     /* It indicates the master READ command numbers
			 * in a specified sample window. The unit refer
			 * to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m2_rd_cmd_num_t;

/* Master[CH] Read-Command Numbers Register */
typedef union {
	u32 val;
	struct {
	u32 num:32;     /* It indicates the master READ command numbers
			 * in a specified sample window. The unit refer
			 * to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m3_rd_cmd_num_t;

/* Master[CH] Read-Command Numbers Register */
typedef union {
	u32 val;
	struct {
	u32 num:32;     /* It indicates the master READ command numbers
			 * in a specified sample window. The unit refer
			 * to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m4_rd_cmd_num_t;

/* Master[CH] Read-Command Numbers Register */
typedef union {
	u32 val;
	struct {
	u32 num:32;     /* It indicates the master READ command numbers
			 * in a specified sample window. The unit refer
			 * to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m5_rd_cmd_num_t;

/* Master[CH] Read-Command Numbers Register */
typedef union {
	u32 val;
	struct {
	u32 num:32;     /* It indicates the master READ command numbers
			 * in a specified sample window. The unit refer
			 * to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m6_rd_cmd_num_t;

/* Master[CH] Read Latency Register */
typedef union {
	u32 val;
	struct {
	u32 cycles:32;          /* It indicates the master READ latency
				 * cycles in a specified sample window.
				 * Unit: 256Cycles(The cycle clock source
				 * is from maxi clock) */
	} bits;
} reg_mem_ctrl_m0_rd_latency_t;

/* Master[CH] Read Latency Register */
typedef union {
	u32 val;
	struct {
	u32 cycles:32;          /* It indicates the master READ latency
				 * cycles in a specified sample window.
				 * Unit: 256Cycles(The cycle clock source
				 * is from maxi clock) */
	} bits;
} reg_mem_ctrl_m1_rd_latency_t;

/* Master[CH] Read Latency Register */
typedef union {
	u32 val;
	struct {
	u32 cycles:32;          /* It indicates the master READ latency
				 * cycles in a specified sample window.
				 * Unit: 256Cycles(The cycle clock source
				 * is from maxi clock) */
	} bits;
} reg_mem_ctrl_m2_rd_latency_t;

/* Master[CH] Read Latency Register */
typedef union {
	u32 val;
	struct {
	u32 cycles:32;          /* It indicates the master READ latency
				 * cycles in a specified sample window.
				 * Unit: 256Cycles(The cycle clock source
				 * is from maxi clock) */
	} bits;
} reg_mem_ctrl_m3_rd_latency_t;

/* Master[CH] Read Latency Register */
typedef union {
	u32 val;
	struct {
	u32 cycles:32;          /* It indicates the master READ latency
				 * cycles in a specified sample window.
				 * Unit: 256Cycles(The cycle clock source
				 * is from maxi clock) */
	} bits;
} reg_mem_ctrl_m4_rd_latency_t;

/* Master[CH] Read Latency Register */
typedef union {
	u32 val;
	struct {
	u32 cycles:32;          /* It indicates the master READ latency
				 * cycles in a specified sample window.
				 * Unit: 256Cycles(The cycle clock source
				 * is from maxi clock) */
	} bits;
} reg_mem_ctrl_m5_rd_latency_t;

/* Master[CH] Read Latency Register */
typedef union {
	u32 val;
	struct {
	u32 cycles:32;          /* It indicates the master READ latency
				 * cycles in a specified sample window.
				 * Unit: 256Cycles(The cycle clock source
				 * is from maxi clock) */
	} bits;
} reg_mem_ctrl_m6_rd_latency_t;

/* Master[CH] Write-Command Numbers Register */
typedef union {
	u32 val;
	struct {
	u32 num:32;     /* It indicates the master WRITE command numbers
			 * in a specified sample window. The unit refer
			 * to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m0_wr_cmd_num_t;

/* Master[CH] Write-Command Numbers Register */
typedef union {
	u32 val;
	struct {
	u32 num:32;     /* It indicates the master WRITE command numbers
			 * in a specified sample window. The unit refer
			 * to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m1_wr_cmd_num_t;

/* Master[CH] Write-Command Numbers Register */
typedef union {
	u32 val;
	struct {
	u32 num:32;     /* It indicates the master WRITE command numbers
			 * in a specified sample window. The unit refer
			 * to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m2_wr_cmd_num_t;

/* Master[CH] Write-Command Numbers Register */
typedef union {
	u32 val;
	struct {
	u32 num:32;     /* It indicates the master WRITE command numbers
			 * in a specified sample window. The unit refer
			 * to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m3_wr_cmd_num_t;

/* Master[CH] Write-Command Numbers Register */
typedef union {
	u32 val;
	struct {
	u32 num:32;     /* It indicates the master WRITE command numbers
			 * in a specified sample window. The unit refer
			 * to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m4_wr_cmd_num_t;

/* Master[CH] Write-Command Numbers Register */
typedef union {
	u32 val;
	struct {
	u32 num:32;     /* It indicates the master WRITE command numbers
			 * in a specified sample window. The unit refer
			 * to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m5_wr_cmd_num_t;

/* Master[CH] Write-Command Numbers Register */
typedef union {
	u32 val;
	struct {
	u32 num:32;     /* It indicates the master WRITE command numbers
			 * in a specified sample window. The unit refer
			 * to UNITS_CFG register. */
	} bits;
} reg_mem_ctrl_m6_wr_cmd_num_t;

/* Master[CH] Write Latency Register */
typedef union {
	u32 val;
	struct {
	u32 cycles:32;          /* It indicates the master WRITE latency
				 * cycles in a specified sample window.
				 * Unit: 256Cycles(The cycle clock source
				 * is from maxi clock) */
	} bits;
} reg_mem_ctrl_m0_wr_latency_t;

/* Master[CH] Write Latency Register */
typedef union {
	u32 val;
	struct {
	u32 cycles:32;          /* It indicates the master WRITE latency
				 * cycles in a specified sample window.
				 * Unit: 256Cycles(The cycle clock source
				 * is from maxi clock) */
	} bits;
} reg_mem_ctrl_m1_wr_latency_t;

/* Master[CH] Write Latency Register */
typedef union {
	u32 val;
	struct {
	u32 cycles:32;          /* It indicates the master WRITE latency
				 * cycles in a specified sample window.
				 * Unit: 256Cycles(The cycle clock source
				 * is from maxi clock) */
	} bits;
} reg_mem_ctrl_m2_wr_latency_t;

/* Master[CH] Write Latency Register */
typedef union {
	u32 val;
	struct {
	u32 cycles:32;          /* It indicates the master WRITE latency
				 * cycles in a specified sample window.
				 * Unit: 256Cycles(The cycle clock source
				 * is from maxi clock) */
	} bits;
} reg_mem_ctrl_m3_wr_latency_t;

/* Master[CH] Write Latency Register */
typedef union {
	u32 val;
	struct {
	u32 cycles:32;          /* It indicates the master WRITE latency
				 * cycles in a specified sample window.
				 * Unit: 256Cycles(The cycle clock source
				 * is from maxi clock) */
	} bits;
} reg_mem_ctrl_m4_wr_latency_t;

/* Master[CH] Write Latency Register */
typedef union {
	u32 val;
	struct {
	u32 cycles:32;          /* It indicates the master WRITE latency
				 * cycles in a specified sample window.
				 * Unit: 256Cycles(The cycle clock source
				 * is from maxi clock) */
	} bits;
} reg_mem_ctrl_m5_wr_latency_t;

/* Master[CH] Write Latency Register */
typedef union {
	u32 val;
	struct {
	u32 cycles:32;          /* It indicates the master WRITE latency
				 * cycles in a specified sample window.
				 * Unit: 256Cycles(The cycle clock source
				 * is from maxi clock) */
	} bits;
} reg_mem_ctrl_m6_wr_latency_t;

/* Memory Port Interface Clock Gating Register */
typedef union {
	u32 val;
	struct {
	u32 m0:1;               /* Master0 port clock gating
				 * [1¡¯b0]: asserted. Clock off.
				 * [1¡¯b1]: de-asserted */
	u32 m1:1;               /* Master1 port clock gating
				 * [1¡¯b0]: asserted. Clock off.
				 * [1¡¯b1]: de-asserted */
	u32 m2:1;               /* Master2 port clock gating
				 * [1¡¯b0]: asserted. Clock off.
				 * [1¡¯b1]: de-asserted */
	u32 m3:1;               /* Master3 port clock gating
				 * [1¡¯b0]: asserted. Clock off.
				 * [1¡¯b1]: de-asserted */
	u32 m4:1;               /* Master4 port clock gating
				 * [1¡¯b0]: asserted. Clock off.
				 * [1¡¯b1]: de-asserted */
	u32 m5:1;               /* Master5 port clock gating
				 * [1¡¯b0]: asserted. Clock off.
				 * [1¡¯b1]: de-asserted */
	u32 m6:1;               /* Master6 port clock gating
				 * [1¡¯b0]: asserted. Clock off.
				 * [1¡¯b1]: de-asserted */
	u32 m7:1;               /* Master7 port clock gating
				 * [1¡¯b0]: asserted. Clock off.
				 * [1¡¯b1]: de-asserted */
	u32 m8:1;               /* Master8 port clock gating
				 * [1¡¯b0]: asserted. Clock off.
				 * [1¡¯b1]: de-asserted */
	u32 m9:1;               /* Master9 port clock gating
				 * [1¡¯b0]: asserted. Clock off.
				 * [1¡¯b1]: de-asserted */
	u32 rsvd0:22;           /* - */
	} bits;
} reg_mem_ctrl_mem_pi_gat_t;

#define MEM_CTRL_MEM_PI_GAT_M0_0        0x0
#define MEM_CTRL_MEM_PI_GAT_M0_1        0x1
#define MEM_CTRL_MEM_PI_GAT_M1_0        0x0
#define MEM_CTRL_MEM_PI_GAT_M1_1        0x1
#define MEM_CTRL_MEM_PI_GAT_M2_0        0x0
#define MEM_CTRL_MEM_PI_GAT_M2_1        0x1
#define MEM_CTRL_MEM_PI_GAT_M3_0        0x0
#define MEM_CTRL_MEM_PI_GAT_M3_1        0x1
#define MEM_CTRL_MEM_PI_GAT_M4_0        0x0
#define MEM_CTRL_MEM_PI_GAT_M4_1        0x1
#define MEM_CTRL_MEM_PI_GAT_M5_0        0x0
#define MEM_CTRL_MEM_PI_GAT_M5_1        0x1
#define MEM_CTRL_MEM_PI_GAT_M6_0        0x0
#define MEM_CTRL_MEM_PI_GAT_M6_1        0x1
#define MEM_CTRL_MEM_PI_GAT_M7_0        0x0
#define MEM_CTRL_MEM_PI_GAT_M7_1        0x1
#define MEM_CTRL_MEM_PI_GAT_M8_0        0x0
#define MEM_CTRL_MEM_PI_GAT_M8_1        0x1
#define MEM_CTRL_MEM_PI_GAT_M9_0        0x0
#define MEM_CTRL_MEM_PI_GAT_M9_1        0x1

/* Memory Port Interface Reset Register */
typedef union {
	u32 val;
	struct {
	u32 m0:1;               /* Master0 port reset
				 * [1¡¯b0]: asserted. Reset.
				 * [1¡¯b1]: de-asserted. Not reset. */
	u32 m1:1;               /* Master1 port reset
				 * [1¡¯b0]: asserted. Reset.
				 * [1¡¯b1]: de-asserted. Not reset. */
	u32 m2:1;               /* Master2 port reset
				 * [1¡¯b0]: asserted. Reset.
				 * [1¡¯b1]: de-asserted. Not reset. */
	u32 m3:1;               /* Master3 port reset
				 * [1¡¯b0]: asserted. Reset.
				 * [1¡¯b1]: de-asserted. Not reset. */
	u32 m4:1;               /* Master4 port reset
				 * [1¡¯b0]: asserted. Reset.
				 * [1¡¯b1]: de-asserted. Not reset. */
	u32 m5:1;               /* Master5 port reset
				 * [1¡¯b0]: asserted. Reset.
				 * [1¡¯b1]: de-asserted. Not reset. */
	u32 m6:1;               /* Master6 port reset
				 * [1¡¯b0]: asserted. Reset.
				 * [1¡¯b1]: de-asserted. Not reset. */
	u32 m7:1;               /* Master7 port reset
				 * [1¡¯b0]: asserted. Reset.
				 * [1¡¯b1]: de-asserted. Not reset. */
	u32 m8:1;               /* Master8 port reset
				 * [1¡¯b0]: asserted. Reset.
				 * [1¡¯b1]: de-asserted. Not reset. */
	u32 m9:1;               /* Master9 port reset
				 * [1¡¯b0]: asserted. Reset.
				 * [1¡¯b1]: de-asserted. Not reset. */
	u32 rsvd0:22;           /* - */
	} bits;
} reg_mem_ctrl_mem_pi_rst_t;

#define MEM_CTRL_MEM_PI_RST_M0_0        0x0
#define MEM_CTRL_MEM_PI_RST_M0_1        0x1
#define MEM_CTRL_MEM_PI_RST_M1_0        0x0
#define MEM_CTRL_MEM_PI_RST_M1_1        0x1
#define MEM_CTRL_MEM_PI_RST_M2_0        0x0
#define MEM_CTRL_MEM_PI_RST_M2_1        0x1
#define MEM_CTRL_MEM_PI_RST_M3_0        0x0
#define MEM_CTRL_MEM_PI_RST_M3_1        0x1
#define MEM_CTRL_MEM_PI_RST_M4_0        0x0
#define MEM_CTRL_MEM_PI_RST_M4_1        0x1
#define MEM_CTRL_MEM_PI_RST_M5_0        0x0
#define MEM_CTRL_MEM_PI_RST_M5_1        0x1
#define MEM_CTRL_MEM_PI_RST_M6_0        0x0
#define MEM_CTRL_MEM_PI_RST_M6_1        0x1
#define MEM_CTRL_MEM_PI_RST_M7_0        0x0
#define MEM_CTRL_MEM_PI_RST_M7_1        0x1
#define MEM_CTRL_MEM_PI_RST_M8_0        0x0
#define MEM_CTRL_MEM_PI_RST_M8_1        0x1
#define MEM_CTRL_MEM_PI_RST_M9_0        0x0
#define MEM_CTRL_MEM_PI_RST_M9_1        0x1

/* AXI Master Interface Debug Register */
typedef union {
	u32 val;
	struct {
	u32 dbg_en:7;           /* Enable the debug function
				 * [1¡¯b0]: disable.
				 * [1¡¯b1]: enable the debug function.
				 * Note: each bit stands for a AXI master
				 * interface. */
	u32 rsvd0:1;            /* - */
	u32 clr:7;              /* Clear the XPI bus residual status.
				 * [1¡¯b0]: not clear.
				 * [1¡¯b1]: clear.
				 * Note: The software must clear the corresponding
				 * bit if finished. */
	u32 rsvd1:9;            /* - */
	u32 clr_stat:7;         /* If the XPI bus residual status has been
				 * cleared.
				 * [1¡¯b0]: UN-finished.
				 * [1¡¯b1]: Finished. */
	u32 rsvd2:1;            /* - */
	} bits;
} reg_mem_ctrl_axim_dbg_t;

#define MEM_CTRL_AXIM_DBG_DBG_EN_0      0x0
#define MEM_CTRL_AXIM_DBG_DBG_EN_1      0x1
#define MEM_CTRL_AXIM_DBG_CLR_0         0x0
#define MEM_CTRL_AXIM_DBG_CLR_1         0x1
#define MEM_CTRL_AXIM_DBG_CLR_STAT_0    0x0
#define MEM_CTRL_AXIM_DBG_CLR_STAT_1    0x1

/* reg_mem_ctrl_t bank */
typedef struct tag_mem_ctrl {
	reg_mem_ctrl_ver_t             ver;            /* 0000 */
	reg_mem_ctrl_mon_mode_ctrl_t   mon_mode_ctrl;  /* 0004 */
	reg_mem_ctrl_bw_ctrl_t         bw_ctrl;        /* 0008 */
	reg_mem_ctrl_int_en_t          int_en;         /* 000C */
	reg_mem_ctrl_units_cfg_t       units_cfg;      /* 0010 */
	reg_mem_ctrl_mon_status_t      mon_status;     /* 0014 */
	reg_mem_ctrl_bw_stm_t          bw_stm;         /* 0018 */
	u32                            res0[1];
	reg_mem_ctrl_rd_lat_of_t       rd_lat_of;      /* 0020 */
	reg_mem_ctrl_wr_lat_of_t       wr_lat_of;      /* 0024 */
	u32                            res1[6];
	reg_mem_ctrl_total_dat_t       total_dat;      /* 0040 */
	reg_mem_ctrl_total_rdat_t      total_rdat;     /* 0044 */
	reg_mem_ctrl_total_wdat_t      total_wdat;     /* 0048 */
	u32                            res2[1];
	reg_mem_ctrl_total_rpdat_t     total_rpdat;    /* 0050 */
	reg_mem_ctrl_total_wpdat_t     total_wpdat;    /* 0054 */
	u32                            res3[10];
	reg_mem_ctrl_m0_radat_t        m0_radat;       /* 0080 */
	reg_mem_ctrl_m0_wadat_t        m0_wadat;       /* 0084 */
	reg_mem_ctrl_m0_rpdat_t        m0_rpdat;       /* 0088 */
	reg_mem_ctrl_m0_wpdat_t        m0_wpdat;       /* 008C */
	reg_mem_ctrl_m1_radat_t        m1_radat;       /* 0090 */
	reg_mem_ctrl_m1_wadat_t        m1_wadat;       /* 0094 */
	reg_mem_ctrl_m1_rpdat_t        m1_rpdat;       /* 0098 */
	reg_mem_ctrl_m1_wpdat_t        m1_wpdat;       /* 009C */
	reg_mem_ctrl_m2_radat_t        m2_radat;       /* 00A0 */
	reg_mem_ctrl_m2_wadat_t        m2_wadat;       /* 00A4 */
	reg_mem_ctrl_m2_rpdat_t        m2_rpdat;       /* 00A8 */
	reg_mem_ctrl_m2_wpdat_t        m2_wpdat;       /* 00AC */
	reg_mem_ctrl_m3_radat_t        m3_radat;       /* 00B0 */
	reg_mem_ctrl_m3_wadat_t        m3_wadat;       /* 00B4 */
	reg_mem_ctrl_m3_rpdat_t        m3_rpdat;       /* 00B8 */
	reg_mem_ctrl_m3_wpdat_t        m3_wpdat;       /* 00BC */
	reg_mem_ctrl_m4_radat_t        m4_radat;       /* 00C0 */
	reg_mem_ctrl_m4_wadat_t        m4_wadat;       /* 00C4 */
	reg_mem_ctrl_m4_rpdat_t        m4_rpdat;       /* 00C8 */
	reg_mem_ctrl_m4_wpdat_t        m4_wpdat;       /* 00CC */
	reg_mem_ctrl_m5_radat_t        m5_radat;       /* 00D0 */
	reg_mem_ctrl_m5_wadat_t        m5_wadat;       /* 00D4 */
	reg_mem_ctrl_m5_rpdat_t        m5_rpdat;       /* 00D8 */
	reg_mem_ctrl_m5_wpdat_t        m5_wpdat;       /* 00DC */
	reg_mem_ctrl_m6_radat_t        m6_radat;       /* 00E0 */
	reg_mem_ctrl_m6_wadat_t        m6_wadat;       /* 00E4 */
	reg_mem_ctrl_m6_rpdat_t        m6_rpdat;       /* 00E8 */
	reg_mem_ctrl_m6_wpdat_t        m6_wpdat;       /* 00EC */
	u32                            res4[68];
	reg_mem_ctrl_m0_rd_cmd_num_t   m0_rd_cmd_num;  /* 0200 */
	reg_mem_ctrl_m0_rd_latency_t   m0_rd_latency;  /* 0204 */
	reg_mem_ctrl_m0_wr_cmd_num_t   m0_wr_cmd_num;  /* 0208 */
	reg_mem_ctrl_m0_wr_latency_t   m0_wr_latency;  /* 020C */
	reg_mem_ctrl_m1_rd_cmd_num_t   m1_rd_cmd_num;  /* 0210 */
	reg_mem_ctrl_m1_rd_latency_t   m1_rd_latency;  /* 0214 */
	reg_mem_ctrl_m1_wr_cmd_num_t   m1_wr_cmd_num;  /* 0218 */
	reg_mem_ctrl_m1_wr_latency_t   m1_wr_latency;  /* 021C */
	reg_mem_ctrl_m2_rd_cmd_num_t   m2_rd_cmd_num;  /* 0220 */
	reg_mem_ctrl_m2_rd_latency_t   m2_rd_latency;  /* 0224 */
	reg_mem_ctrl_m2_wr_cmd_num_t   m2_wr_cmd_num;  /* 0228 */
	reg_mem_ctrl_m2_wr_latency_t   m2_wr_latency;  /* 022C */
	reg_mem_ctrl_m3_rd_cmd_num_t   m3_rd_cmd_num;  /* 0230 */
	reg_mem_ctrl_m3_rd_latency_t   m3_rd_latency;  /* 0234 */
	reg_mem_ctrl_m3_wr_cmd_num_t   m3_wr_cmd_num;  /* 0238 */
	reg_mem_ctrl_m3_wr_latency_t   m3_wr_latency;  /* 023C */
	reg_mem_ctrl_m4_rd_cmd_num_t   m4_rd_cmd_num;  /* 0240 */
	reg_mem_ctrl_m4_rd_latency_t   m4_rd_latency;  /* 0244 */
	reg_mem_ctrl_m4_wr_cmd_num_t   m4_wr_cmd_num;  /* 0248 */
	reg_mem_ctrl_m4_wr_latency_t   m4_wr_latency;  /* 024C */
	reg_mem_ctrl_m5_rd_cmd_num_t   m5_rd_cmd_num;  /* 0250 */
	reg_mem_ctrl_m5_rd_latency_t   m5_rd_latency;  /* 0254 */
	reg_mem_ctrl_m5_wr_cmd_num_t   m5_wr_cmd_num;  /* 0258 */
	reg_mem_ctrl_m5_wr_latency_t   m5_wr_latency;  /* 025C */
	reg_mem_ctrl_m6_rd_cmd_num_t   m6_rd_cmd_num;  /* 0260 */
	reg_mem_ctrl_m6_rd_latency_t   m6_rd_latency;  /* 0264 */
	reg_mem_ctrl_m6_wr_cmd_num_t   m6_wr_cmd_num;  /* 0268 */
	reg_mem_ctrl_m6_wr_latency_t   m6_wr_latency;  /* 026C */
	u32                            res5[868];
	reg_mem_ctrl_mem_pi_gat_t      mem_pi_gat;     /* 1000 */
	reg_mem_ctrl_mem_pi_rst_t      mem_pi_rst;     /* 1004 */
	u32                            res6[14];
	reg_mem_ctrl_axim_dbg_t        axim_dbg;       /* 1040 */
} reg_mem_ctrl_t;

#endif /* ___MEM_CTRL___H___ */
