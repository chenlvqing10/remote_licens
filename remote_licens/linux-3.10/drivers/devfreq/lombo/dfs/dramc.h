/* dramc.h */

#ifndef ___DRAMC___H___
#define ___DRAMC___H___

#define BASE_DRAMC                0x01210000

#define VA_DRAMC_MSTR                (0x00000000 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_STAT                (0x00000004 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_MSTR1               (0x00000008 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_MRCTRL0             (0x00000010 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_MRCTRL1             (0x00000014 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_MRSTAT              (0x00000018 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_MSTR2               (0x00000028 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_PWRCTL              (0x00000030 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_PWRTMG              (0x00000034 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_HWLPCTL             (0x00000038 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_RFSHCTL0            (0x00000050 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_RFSHCTL1            (0x00000054 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_RFSHCTL3            (0x00000060 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_RFSHTMG             (0x00000064 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_INIT0               (0x000000D0 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_INIT1               (0x000000D4 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_INIT3               (0x000000DC + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_INIT4               (0x000000E0 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_INIT5               (0x000000E4 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_DIMMCTL             (0x000000F0 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_RANKCTL             (0x000000F4 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_CHCTL               (0x000000FC + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_DRAMTMG0            (0x00000100 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_DRAMTMG1            (0x00000104 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_DRAMTMG2            (0x00000108 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_DRAMTMG3            (0x0000010C + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_DRAMTMG4            (0x00000110 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_DRAMTMG5            (0x00000114 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_DRAMTMG8            (0x00000120 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_DRAMTMG15           (0x0000013C + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_ZQCTL0              (0x00000180 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_ZQCTL1              (0x00000184 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_DFITMG0             (0x00000190 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_DFITMG1             (0x00000194 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_DFILPCFG0           (0x00000198 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_DFIUPD0             (0x000001A0 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_DFIUPD1             (0x000001A4 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_DFIUPD2             (0x000001A8 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_DFIMISC             (0x000001B0 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_DFITMG2             (0x000001B4 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_DFISTAT             (0x000001BC + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_DFIPHYMSTR          (0x000001C4 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_ADDRMAP0            (0x00000200 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_ADDRMAP1            (0x00000204 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_ADDRMAP2            (0x00000208 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_ADDRMAP3            (0x0000020C + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_ADDRMAP4            (0x00000210 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_ADDRMAP5            (0x00000214 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_ADDRMAP6            (0x00000218 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_ADDRMAP9            (0x00000224 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_ADDRMAP10           (0x00000228 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_ADDRMAP11           (0x0000022C + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_ODTCFG              (0x00000240 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_ODTMAP              (0x00000244 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_SCHED               (0x00000250 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_SCHED1              (0x00000254 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_PERFHPR1            (0x0000025C + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_PERFLPR1            (0x00000264 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_PERFWR1             (0x0000026C + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_DBG0                (0x00000300 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_DBG1                (0x00000304 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_DBGCAM              (0x00000308 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_DBGCMD              (0x0000030C + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_DBGSTAT             (0x00000310 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_SWCTL               (0x00000320 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_SWSTAT              (0x00000324 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_POISONCFG           (0x0000036C + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_POISONSTAT          (0x00000370 + BASE_DRAMC + VA_DRAMC)
#define VA_DRAMC_PSTAT               (0x000003FC + BASE_DRAMC + VA_DRAMC)



#define DATA_DRAMC_MSTR                  0x01040001
#define DATA_DRAMC_STAT                  0x00000000
#define DATA_DRAMC_MSTR1                 0x00000000
#define DATA_DRAMC_MRCTRL0               0x00000010
#define DATA_DRAMC_MRCTRL1               0x00000000
#define DATA_DRAMC_MRSTAT                0x00000000
#define DATA_DRAMC_MSTR2                 0x00000001
#define DATA_DRAMC_PWRCTL                0x00000000
#define DATA_DRAMC_PWRTMG                0x00400010
#define DATA_DRAMC_HWLPCTL               0x00000003
#define DATA_DRAMC_RFSHCTL0              0x00210000
#define DATA_DRAMC_RFSHCTL1              0x00000000
#define DATA_DRAMC_RFSHCTL3              0x00000000
#define DATA_DRAMC_RFSHTMG               0x0062008C
#define DATA_DRAMC_INIT0                 0x0002004E
#define DATA_DRAMC_INIT1                 0x00000000
#define DATA_DRAMC_INIT3                 0x00000510
#define DATA_DRAMC_INIT4                 0x00000000
#define DATA_DRAMC_INIT5                 0x00100000
#define DATA_DRAMC_DIMMCTL               0x00000000
#define DATA_DRAMC_RANKCTL               0x0000066F
#define DATA_DRAMC_CHCTL                 0x00000001
#define DATA_DRAMC_DRAMTMG0              0x0F101B0F
#define DATA_DRAMC_DRAMTMG1              0x00080414
#define DATA_DRAMC_DRAMTMG2              0x0305060D
#define DATA_DRAMC_DRAMTMG3              0x0000400C
#define DATA_DRAMC_DRAMTMG4              0x05040405
#define DATA_DRAMC_DRAMTMG5              0x05050403
#define DATA_DRAMC_DRAMTMG8              0x00004405
#define DATA_DRAMC_DRAMTMG15             0x00000000
#define DATA_DRAMC_ZQCTL0                0x02000040
#define DATA_DRAMC_ZQCTL1                0x00000100
#define DATA_DRAMC_DFITMG0               0x07020002
#define DATA_DRAMC_DFITMG1               0x00000404
#define DATA_DRAMC_DFILPCFG0             0x07000000
#define DATA_DRAMC_DFIUPD0               0x00400003
#define DATA_DRAMC_DFIUPD1               0x00010001
#define DATA_DRAMC_DFIUPD2               0x80000000
#define DATA_DRAMC_DFIMISC               0x00000001
#define DATA_DRAMC_DFITMG2               0x00000202
#define DATA_DRAMC_DFISTAT               0x00000000
#define DATA_DRAMC_DFIPHYMSTR            0x00000001
#define DATA_DRAMC_ADDRMAP0              0x00000000
#define DATA_DRAMC_ADDRMAP1              0x00000000
#define DATA_DRAMC_ADDRMAP2              0x00000000
#define DATA_DRAMC_ADDRMAP3              0x00000000
#define DATA_DRAMC_ADDRMAP4              0x00000000
#define DATA_DRAMC_ADDRMAP5              0x00000000
#define DATA_DRAMC_ADDRMAP6              0x00000000
#define DATA_DRAMC_ADDRMAP9              0x00000000
#define DATA_DRAMC_ADDRMAP10             0x00000000
#define DATA_DRAMC_ADDRMAP11             0x00000000
#define DATA_DRAMC_ODTCFG                0x04000400
#define DATA_DRAMC_ODTMAP                0x00000011
#define DATA_DRAMC_SCHED                 0x00000005
#define DATA_DRAMC_SCHED1                0x00000000
#define DATA_DRAMC_PERFHPR1              0x0F000001
#define DATA_DRAMC_PERFLPR1              0x0F00007F
#define DATA_DRAMC_PERFWR1               0x0F00007F
#define DATA_DRAMC_DBG0                  0x00000000
#define DATA_DRAMC_DBG1                  0x00000000
#define DATA_DRAMC_DBGCAM                0x00000000
#define DATA_DRAMC_DBGCMD                0x00000000
#define DATA_DRAMC_DBGSTAT               0x00000000
#define DATA_DRAMC_SWCTL                 0x00000001
#define DATA_DRAMC_SWSTAT                0x00000001
#define DATA_DRAMC_POISONCFG             0x00110011
#define DATA_DRAMC_POISONSTAT            0x00000000
#define DATA_DRAMC_PSTAT                 0x00000000

/* Master Register0 */
typedef union {
	u32 val;
	struct {
	u32 ddr3:1;                     /* Select DDR3 SDRAM
					 * [0x0]: non-DDR3 SDRAM device in use
					 * [0x1]:DDR3 SDRAM device in use
					 * Value After Reset: "(MEMC_DDR3_EN==1)
					 * ? 0x1 : 0x0"
					 * Exists: MEMC_DDR3==1
					 * Programming Mode: Static */
	u32 rsvd0:7;                    /* - */
	u32 burst_mode:1;               /* Indicates burst mode.
					 * [0x0]:Sequential burst mode
					 * [0x1]:Interleaved burst mode
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_INCL_ARB==0
					 * Programming Mode: Static */
	u32 burstchop:1;                /* When set, enable burst-chop
					 * (BC4 or 8 on-the-fly) in DDR3.
					 * Burst Chop for Reads is exercised
					 * only in HIF configurations (UMCTL2_INCL_ARB
					 * not set) and if in full bus
					 * width mode (MSTR.data_bus_width
					 * = 00) and if MEMC_BURST_LENGTH=8
					 * or 16. Burst Chop for Writes
					 * is exercised only if Partial
					 * Writes enabled (UMCTL2_PARTIAL_WR=1)
					 * and if CRC is disabled (CRCPARCTL1.crc_enable
					 * = 0).
					 * BC4 (fixed) mode is not supported.
					 * [0x0]: Disabled[0x1]: Enabled
					 * Value After Reset: 0x0
					 * Exists: MEMC_DDR3_OR_4==1
					 * Programming Mode: Static */
	u32 en_2t_timing_mode:1;        /* If 1, then uMCTL2 uses 2T timing.
					 * Otherwise, uses 1T timing. In
					 * 2T timing, all command signals
					 * (except chip select) are held
					 * for 2 clocks on the SDRAM bus.
					 * Chip select is asserted on the
					 * second cycle of the command
					 * [0x0]: Disabled[0x1]: Enabled
					 * Value After Reset: 0x0
					 * Exists: MEMC_CMD_RTN2IDLE==0
					 * Programming Mode: Static */
	u32 rsvd1:1;                    /* - */
	u32 data_bus_width:2;           /* Selects proportion of DQ bus
					 * width that is used by theSDRAM[0x0]:
					 *  Full DQ bus width to SDRAM[0x1]:
					 *  Half DQ bus width to SDRAM[0x2]:
					 *  Quarter DQ bus width to SDRAM[0x3]:
					 *  ReservedNote that half bus
					 * width mode is only supported
					 * when the SDRAM bus width is
					 * a multiple of 16, and quarter
					 * bus width mode is only supported
					 * when the SDRAM bus width is
					 * a multiple of 32 and the configuration
					 * parameter MEMC_QBUS_SUPPORT
					 * is set. Bus width refers to
					 * DQ bus width (excluding any
					 * ECC width).Value After Reset:
					 * 0x0Exists: AlwaysProgramming
					 * Mode: Static */
	u32 rsvd2:1;                    /* - */
	u32 dll_off_mode:1;             /* Set to 1 when the uMCTL2 and
					 * DRAM has to be put in DLLoff
					 * mode for low frequency operation.Set
					 * to 0 to put uMCTL2 and DRAM
					 * in DLL-on mode for normal frequency
					 * operation.[0x0]: Disabled[0x1]:
					 * Enabled
					 * Value After Reset: 0x0Exists:
					 * MEMC_DDR3_OR_4==1Programming
					 * Mode: Quasi-dynamic Group 2 */
	u32 burst_rdwr:4;               /* SDRAM burst length used:
					 * [0x0]: Reserved[0x1]: Reserved[0x2]:
					 * Burst length of 4
					 * [0x3]: Reserved[0x4]: Burst
					 * length of 8[0x5]: Reserved
					 * [0x6]: Reserved
					 * [0x7]: Reserved
					 * [0x8]: Reserved
					 * [0x9]: Reserved
					 * [0xA]: Reserved
					 * [0xB]: Reserved
					 * [0xC]: Reserved
					 * [0xD]: Reserved
					 * [0xE]: Reserved
					 * [0xF]: ReservedThis controls
					 * the burst size used to access
					 * the SDRAM. This must match the
					 * burst length mode register setting
					 * in the SDRAM. For DDR3, this
					 * must be set to (BL8).Value
					 * After Reset: 0x4Exists: AlwaysProgramming
					 * Mode: Static */
	u32 rsvd3:2;                    /* - */
	u32 freq_ratio:1;               /* Selects the Frequency Ratio[0x0]:1:2
					 * Mode[0x1]:1:1 Mode
					 * Value After Reset: 0x0Exists:
					 * MEMC_PROG_FREQ_RATIO_EN==1Programming
					 * Mode: Static */
	u32 rsvd4:1;                    /* - */
	u32 active_ranks:2;             /* Only present for multi-rank
					 * configurations.Each bit represents
					 * one rank. For two-rank configurations,LSB
					 * is the lowest rank number.For
					 * 2 ranks following combinations
					 * are legal:
					 * [0x0]:Reserved[0x1]:One rank
					 * [0x2]:Reserved[0x3]:Two ranksValue
					 * After Reset:
					 * ((MEMC_NUM_RANKS==2) ? 0x3 :
					 * 0x1)Exists: MEMC_NUM_RANKS>1Programming
					 * Mode: Static */
	u32 rsvd5:6;                    /* - */
	} bits;
} reg_dramc_mstr_t;

#define DRAMC_MSTR_DDR3_0               0x0
#define DRAMC_MSTR_DDR3_1               0x1
#define DRAMC_MSTR_BURST_MODE_0         0x0
#define DRAMC_MSTR_BURST_MODE_1         0x1
#define DRAMC_MSTR_BURSTCHOP_0          0x0
#define DRAMC_MSTR_EN_2T_TIMING_MODE_0  0x0
#define DRAMC_MSTR_BURST_RDWR_0         0x0
#define DRAMC_MSTR_BURST_RDWR_3         0x3
#define DRAMC_MSTR_BURST_RDWR_6         0x6
#define DRAMC_MSTR_BURST_RDWR_7         0x7
#define DRAMC_MSTR_BURST_RDWR_8         0x8
#define DRAMC_MSTR_BURST_RDWR_9         0x9
#define DRAMC_MSTR_BURST_RDWR_A         0xA
#define DRAMC_MSTR_BURST_RDWR_B         0xB
#define DRAMC_MSTR_BURST_RDWR_C         0xC
#define DRAMC_MSTR_BURST_RDWR_D         0xD
#define DRAMC_MSTR_BURST_RDWR_E         0xE
#define DRAMC_MSTR_BURST_RDWR_F         0xF
#define DRAMC_MSTR_ACTIVE_RANKS_0       0x0
#define DRAMC_MSTR_ACTIVE_RANKS_2       0x2

/* Operating Mode Status Register */
typedef union {
	u32 val;
	struct {
	u32 operating_mode:2;                   /* Operating mode.
						 * [0x0]: Init
						 * [0x1]: Normal
						 * [0x2]: Power-down
						 * [0x3]: Self refresh
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Dynamic
						 * Range */
	u32 rsvd0:2;                            /* - */
	u32 selfref_type:2;                     /* Flags if Self Refresh
						 * is entered and if it
						 * was under Automatic
						 * Self Refresh control
						 * only or not.
						 * [0x0]:SDRAM is not in
						 * Self Refresh
						 * [0x1]: SDRAM is in Self
						 * Refresh, which was caused
						 * by PHY Master Request.
						 * [0x2]:SDRAM is in Self
						 * Refresh, which was not
						 * caused solely under
						 * Automatic Self Refresh
						 * control. It could have
						 * been caused by Hardware
						 * Low Power Interface
						 * and/or Software (PWRCTL.selfref_sw).
						 * If retry is enabled,
						 * this guarantees SRE
						 * command is executed
						 * correctly without parity
						 * error.
						 * [0x3]:SDRAM is in Self
						 * Refresh, which was caused
						 * by Automatic Self Refresh
						 * only. If retry is enabled,
						 * this guarantees SRE
						 * command is executed
						 * correctly without parity
						 * error.
						 * ¡ö
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Dynamic */
	u32 rsvd1:2;                            /* - */
	u32 selfref_state:2;                    /* Indicates that the module
						 * has feature1 function
						 * [0x0]: Active
						 * [0x1]: Inactive */
	u32 rsvd2:2;                            /* - */
	u32 selfref_cam_not_empty:1;            /* Self refresh with CAMs
						 * not empty. Set to 1
						 * when Self Refresh is
						 * entered but CAMs are
						 * not drained. Cleared
						 * after exiting Self Refresh.
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Dynamic */
	u32 rsvd3:19;                           /* - */
	} bits;
} reg_dramc_stat_t;

#define DRAMC_STAT_OPERATING_MODE_0             0x0
#define DRAMC_STAT_OPERATING_MODE_1             0x1
#define DRAMC_STAT_OPERATING_MODE_2             0x2
#define DRAMC_STAT_OPERATING_MODE_3             0x3
#define DRAMC_STAT_SELFREF_TYPE_0               0x0
#define DRAMC_STAT_SELFREF_TYPE_1               0x1
#define DRAMC_STAT_SELFREF_TYPE_2               0x2
#define DRAMC_STAT_SELFREF_TYPE_3               0x3
#define DRAMC_STAT_SELFREF_STATE_0              0x0
#define DRAMC_STAT_SELFREF_STATE_1              0x1

/* Master Register1 */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:16;                   /* - */
	u32 alt_addrmap_en:1;           /* Enable Alternative Address Map.
					 * [0x0]:Disable Alternative Address Map
					 * [0x1]: Enable Alternative Address Map
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_HET_RANK==1
					 * Programming Mode: Static */
	u32 rsvd1:15;                   /* - */
	} bits;
} reg_dramc_mstr1_t;

#define DRAMC_MSTR1_ALT_ADDRMAP_EN_0    0x0
#define DRAMC_MSTR1_ALT_ADDRMAP_EN_1    0x1

/* Mode Register Read/Write Control Register 0 */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:4;            /* - */
	u32 mr_rank:2;          /* Controls which rank is accessed by MRCTRL0.mr_wr.
				 * Normally, it is desired to access all
				 * ranks, so all bits should be set to
				 * 1. However, for multi-rank UDIMMs/RDIMMs/LRDIMMs
				 * which implement address mirroring, it
				 * may be necessary to access ranks indivi
				 * dually.
				 * Examples:
				 * [0x0]: select none
				 * [0x1]: select rank 0 only
				 * [0x2]: select rank 1 only
				 * [0x3]: select ranks 0 and 1
				 * Value After Reset: "((MEMC_NUM_RANKS==2)
				 * ? 0x3 : 0x1)"
				 * Exists: MEMC_NUM_RANKS_1_OR_2_OR_4==1
				 * Programming Mode: Dynamic */
	u32 rsvd1:6;            /* - */
	u32 mr_addr:4;          /* Address of the mode register that is
				 * to be written to.
				 * [0x0]: MR0
				 * [0x1]: MR1
				 * [0x2]: MR2
				 * [0x3]: MR3
				 * [0x4]: MR4
				 * [0x5]: MR5
				 * [0x6]: MR6
				 * [0x7]: MR7
				 * This signal is also used for writing
				 * to control words of the register chip
				 * on RDIMMs/LRDIMMs. In that case, it
				 * corresponds to the bank address bits
				 * sent to the RDIMM/LRDIMM
				 * Value After Reset: 0x0
				 * Exists: Always
				 * Programming Mode: Dynamic */
	u32 rsvd2:15;           /* - */
	u32 mr_wr:1;            /* Setting this register bit to 1 triggers
				 * a mode register read or write operation.
				 * When the MR operation is complete, the
				 * uMCTL2 automatically clears this bit.
				 * The other register fields of this register
				 * must be written in a separate APB transaction,
				 * before setting this mr_wr bit. It is
				 * recommended NOT to set this signal if
				 * in Init, Deep power-down or MPSM operating
				 * modes.
				 * [0x0]:no-effect
				 * [0x1]:trigger
				 * Value After Reset: 0x0
				 * Exists: Always Testable: readOnly Programming
				 * Mode: Dynamic */
	} bits;
} reg_dramc_mrctrl0_t;

#define DRAMC_MRCTRL0_MR_RANK_0         0x0
#define DRAMC_MRCTRL0_MR_RANK_1         0x1
#define DRAMC_MRCTRL0_MR_RANK_2         0x2
#define DRAMC_MRCTRL0_MR_RANK_3         0x3
#define DRAMC_MRCTRL0_MR_ADDR_0         0x0
#define DRAMC_MRCTRL0_MR_ADDR_1         0x1
#define DRAMC_MRCTRL0_MR_ADDR_2         0x2
#define DRAMC_MRCTRL0_MR_ADDR_3         0x3
#define DRAMC_MRCTRL0_MR_ADDR_4         0x4
#define DRAMC_MRCTRL0_MR_ADDR_5         0x5
#define DRAMC_MRCTRL0_MR_ADDR_6         0x6
#define DRAMC_MRCTRL0_MR_ADDR_7         0x7
#define DRAMC_MRCTRL0_MR_WR_0   0x0
#define DRAMC_MRCTRL0_MR_WR_1   0x1

/* Mode Register Read/Write Control Register 1 */
typedef union {
	u32 val;
	struct {
	u32 mr_data:16;         /* Mode register write data
				 * After Reset: 0x0
				 * Exists: Always
				 * Programming Mode: Dynamic Range */
	u32 rsvd0:16;           /* - */
	} bits;
} reg_dramc_mrctrl1_t;

/* Mode Register Read/Write Status Register */
typedef union {
	u32 val;
	struct {
	u32 mr_wr_busy:1;       /* The SoC core may initiate a MR write
				 * operation only if this signal is low.
				 * This signal goes high in the clock after
				 * the uMCTL2 accepts the MRW/MRR request.
				 * It goes low when the MRW/MRR command
				 * is issued to the SDRAM. It is recommended
				 * not to perform MRW/MRR commands when
				 * 'MRSTAT.mr_wr_busy' is high.
				 * [0x0]:  Indicates that the SoC core
				 * can initiate a mode register write operation
				 * [0x1]:  Indicates that mode register
				 * write operation is in progress
				 * Value After Reset: 0x0
				 * Exists: Always
				 * Programming Mode: Dynamic */
	u32 rsvd0:31;           /* - */
	} bits;
} reg_dramc_mrstat_t;

#define DRAMC_MRSTAT_MR_WR_BUSY_0       0x0
#define DRAMC_MRSTAT_MR_WR_BUSY_1       0x1

/* Master Register2 */
typedef union {
	u32 val;
	struct {
	u32 target_frequency:2;         /* If MSTR.frequency_mode=1, this
					 * field specifies the target frequency.
					 * [0x0]:  Frequency 0/Normal
					 * [0x1]:  Frequency 1/FREQ1
					 * [0x2]:  Frequency 2/FREQ2
					 * [0x3]:  Frequency 3/FREQ3
					 * If MSTR.frequency_mode=0, this
					 * field is ignored.
					 * Note: If the target frequency
					 * can be changed through Hardware
					 * Low Power Interface only, this
					 * field is not needed.
					 * Value After Reset: 0x1
					 * Exists: UMCTL2_FREQUENCY_NUM>2
					 * Programming Mode: Quasi-dynamic
					 * Group 2 */
	u32 rsvd0:30;                   /* - */
	} bits;
} reg_dramc_mstr2_t;

#define DRAMC_MSTR2_TARGET_FREQUENCY_0  0x0
#define DRAMC_MSTR2_TARGET_FREQUENCY_1  0x1
#define DRAMC_MSTR2_TARGET_FREQUENCY_2  0x2
#define DRAMC_MSTR2_TARGET_FREQUENCY_3  0x3

/* Low Power Control Register */
typedef union {
	u32 val;
	struct {
	u32 selfref_en:1;                       /* If true then the uMCTL2
						 * puts the SDRAM into
						 * Self Refresh after a
						 * programmable number
						 * of cycles "maximum idle
						 * clocks before Self Refresh
						 * (PWRTMG.selfref_to_x32)".
						 * This register bit may
						 * be re-programmed during
						 * the course of normal
						 * operation.
						 * [0x0]: Disabled[0x1]:
						 * Enabled
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Dynamic */
	u32 powerdown_en:1;                     /* If true then the uMCTL2
						 * goes into power-down
						 * after a programmable
						 * number of cycles "maximum
						 * idle clocks before power
						 * down" (PWRTMG.powerdown
						 * _to_x32).
						 * This register bit may
						 * be re-programmed during
						 * the course of normal
						 * operation.
						 * [0x0]: Disabled[0x1]:
						 * Enabled
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Dynamic */
	u32 rsvd0:1;                            /* - */
	u32 en_dfi_dram_clk_disable:1;          /* Enable the assertion
						 * of dfi_dram_clk_disable
						 * whenever a clock is
						 * not required by the SDRAM.
						 * If set to 0, dfi_dram_clk_disable
						 * is never asserted. Assertion
						 * of dfi_dram_clk_disable
						 * is as follows:
						 * In DDR2/DDR3, can only
						 * be asserted in Self Refresh.
						 * [0x0]: Disabled[0x1]:
						 * Enabled
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Dynamic */
	u32 rsvd1:1;                            /* - */
	u32 selfref_sw:1;                       /* A value of 1 to this
						 * register causes system
						 * to move to Self Refresh
						 * state immediately, as
						 * long as it is not in
						 * INIT or DPD/MPSM operating_mode.
						 * This is referred to
						 * as Software Entry/Exit
						 * to Self Refresh.
						 * [0x0]:  Software Exit
						 * from Self Refresh
						 * [0x1]:  Software Entry
						 * to Self Refresh
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Dynamic */
	u32 rsvd2:1;                            /* - */
	u32 dis_cam_drain_selfref:1;            /* Indicates whether skipping
						 * CAM draining is allowed
						 * when entering Self-Refresh.
						 * This register field
						 * cannot be modified while
						 * PWRCTL.selfref_sw==1.
						 * [0x0]:  CAMs must be
						 * empty before entering SR
						 * [0x1]:  CAMs are not
						 * emptied before entering
						 * SR (unsupported)
						 * Note, PWRCTL.dis_cam_drain_selfref=1
						 * is unsupported in this
						 * release. PWRCTL.dis_cam_drain_selfref=0
						 * is required.
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Dynamic */
	u32 rsvd3:24;                           /* - */
	} bits;
} reg_dramc_pwrctl_t;

#define DRAMC_PWRCTL_SELFREF_EN_0               0x0
#define DRAMC_PWRCTL_POWERDOWN_EN_0             0x0
#define DRAMC_PWRCTL_EN_DFI_DRAM_CLK_DISABLE_0  0x0
#define DRAMC_PWRCTL_SELFREF_SW_0               0x0
#define DRAMC_PWRCTL_SELFREF_SW_1               0x1
#define DRAMC_PWRCTL_DIS_CAM_DRAIN_SELFREF_0    0x0
#define DRAMC_PWRCTL_DIS_CAM_DRAIN_SELFREF_1    0x1

/* Low Power Timing Register */
typedef union {
	u32 val;
	struct {
	u32 powerdown_to_x32:5;         /* After this many clocks of the
					 * DDRC command channel being idle
					 * the uMCTL2 automatically puts
					 * the SDRAM into power- down.
					 * The DDRC command channel is
					 * considered idle when there are
					 * no HIF commands outstanding.
					 * This must be enabled in the
					 * PWRCTL.powerdown_en.
					 * Unit: Multiples of 32 DFI clocks
					 * FOR PERFORMANCE ONLY.
					 * Value After Reset: 0x10
					 * Exists: Always
					 * Programming Mode: Quasi-dynamic
					 * Group 4 */
	u32 rsvd0:11;                   /* - */
	u32 selfref_to_x32:8;           /* After this many clocks of the
					 * DDRC command channel being idle
					 * the uMCTL2 automatically puts
					 * the SDRAM into Self Refresh.
					 * The DDRC command channel is
					 * considered idle when there are
					 * no HIF commands outstanding.
					 * This must be enabled in the
					 * PWRCTL.selfref_en.
					 * Unit: Multiples of 32 DFI clocks.
					 * FOR PERFORMANCE ONLY.
					 * Value After Reset: 0x40
					 * Exists: Always
					 * Programming Mode: Quasi-dynamic
					 * Group 4 */
	u32 rsvd1:8;                    /* - */
	} bits;
} reg_dramc_pwrtmg_t;

/* Hardware Low Power Control Register */
typedef union {
	u32 val;
	struct {
	u32 hw_lp_en:1;                 /* Enable for Hardware Low Power
					 * Interface.
					 * [0x0]: Disabled[0x1]: Enabled
					 * Value After Reset: 0x1
					 * Exists: Always
					 * Programming Mode: Quasi-dynamic
					 * Group 3 */
	u32 hw_lp_exit_idle_en:1;       /* When this bit is programmed
					 * to 1 the cactive_in_ddrc pin
					 * of the DDRC can be used to exit
					 * from the automatic clock stop,
					 * automatic power down or automatic
					 * self-refresh modes. Note, it
					 * will not cause exit of Self-Refresh
					 * that was caused by Hardware
					 * Low Power Interface and/or Software
					 * (PWRCTL.selfref_sw).
					 * [0x0]: Disabled[0x1]: Enabled
					 * Value After Reset: 0x1
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd0:14;                   /* - */
	u32 hw_lp_idle_x32:12;          /* Hardware idle period. The cactive_ddrc
					 * output is driven low if the
					 * DDRC command channel is idle
					 * for hw_lp_idle * 32 cycles if
					 * not in INIT or DPD/MPSM operating_mode.
					 * The DDRC command channel is
					 * considered idle when there are
					 * no HIF commands outstanding.
					 * The hardware idle function is
					 * disabled when hw_lp_idle_x32=0.
					 * Unit: Multiples of 32 DFI clocks.
					 * FOR PERFORMANCE ONLY.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd1:4;                    /* - */
	} bits;
} reg_dramc_hwlpctl_t;

#define DRAMC_HWLPCTL_HW_LP_EN_0                0x0
#define DRAMC_HWLPCTL_HW_LP_EXIT_IDLE_EN_0      0x0

/* Refresh Control Register 0 */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:4;                    /* - */
	u32 refresh_burst:3;            /* The programmed value + 1 is
					 * the number of refresh timeouts
					 * that is allowed to accumulate
					 * before traffic is blocked and
					 * the refreshes are forced to
					 * execute. Closing pages to perform
					 * a refresh is a one-time penalty
					 * that must be paid for each group
					 * of refreshes. Therefore, performing
					 * refreshes in a burst reduces
					 * the per-refresh penalty of these
					 * page closings. Higher numbers
					 * for RFSHCTL.refresh_burst slightly
					 * increases utilization; lower
					 * numbers decreases the worst-
					 * case latency associated with
					 * refreshes.
					 * [0x0]:  single refresh
					 * [0x1]: burst-of-2 refresh
					 * [0x2]:Reserved
					 * [0x3]:Reserved
					 * [0x4]:Reserved
					 * [0x5]:Reserved
					 * [0x6]:Reserved
					 * [0x7]: burst-of-8 refresh
					 * For information on burst refresh
					 * feature refer to section 3.9 of
					 * DDR2 JEDEC specification - JESD
					 * 79-2F.pdf.
					 * For DDR2/3, the refresh is always
					 * per-rank and not per- bank.
					 * The rank refresh can be accumulated
					 * over 8*tREFI cycles using the
					 * burst refresh feature. In DDR4
					 * mode, according to Fine Granularity
					 * feature, 8 refreshes can be
					 * postponed in 1X mode, 16 refreshes
					 * in 2X mode and 32 refreshes
					 * in 4X mode. If using PHY-initiated
					 * updates, care must be taken
					 * in the setting of RFSHCTL0.refresh_burst,
					 * to ensure that tRFCmax is not
					 * violated due to a PHY-initiated
					 * update occurring shortly before
					 * a refresh burst was due. In
					 * this situation, the refresh
					 * burst will be delayed until
					 * the PHY- initiated update is
					 * complete.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Dynamic -
					 * Refresh Related */
	u32 rsvd1:5;                    /* - */
	u32 refresh_to_x32:5;           /* If the refresh timer (tRFCnom,
					 * also known as tREFI) has expired
					 * at least once, but it has not
					 * expired (RFSHCTL0.refresh_burst+1)
					 * times yet, then a speculative
					 * refresh may be performed. A
					 * speculative refresh is a refresh
					 * performed at a time when refresh
					 * would be useful, but before
					 * it is absolutely required. When
					 * the SDRAM bus is idle for a
					 * period of time determined by
					 * this RFSHCTL0.refresh_to_x32
					 * and the refresh timer has expired
					 * at least once since the last
					 * refresh, then a speculative
					 * refresh is performed. Speculative
					 * refreshes continues successively
					 * until there are no refreshes
					 * pending or until new reads or
					 * writes are issued to the uMCTL2.
					 * FOR PERFORMANCE ONLY.
					 * Unit: Multiples of 32 DFI clocks.
					 * Value After Reset: 0x10
					 * Exists: Always
					 * Programming Mode: Dynamic -
					 * Refresh Related */
	u32 rsvd2:3;                    /* - */
	u32 refresh_margin:4;           /* Threshold value in number of
					 * DFI clock cycles before the
					 * critical refresh or page timer
					 * expires. A critical refresh
					 * is to be issued before this
					 * threshold is reached. It is
					 * recommended that this not be
					 * changed from the default value,
					 * currently shown as 0x2. It must
					 * always be less than internally
					 * used t_rfc_nom/32. Note that
					 * internally used t_rfc_nom is
					 * equal to RFSHTMG.t_rfc_nom_x1_x32
					 * * 32 if RFSHTMG.t_rfc_nom_x1_sel=0.
					 * Unit: Multiples of 32 DFI clocks.
					 * Value After Reset: 0x2
					 * Exists: Always
					 * Programming Mode: Dynamic -
					 * Refresh Related */
	u32 rsvd3:8;                    /* - */
	} bits;
} reg_dramc_rfshctl0_t;

#define DRAMC_RFSHCTL0_REFRESH_BURST_0          0x0
#define DRAMC_RFSHCTL0_REFRESH_BURST_1          0x1
#define DRAMC_RFSHCTL0_REFRESH_BURST_2          0x2
#define DRAMC_RFSHCTL0_REFRESH_BURST_3          0x3
#define DRAMC_RFSHCTL0_REFRESH_BURST_4          0x4
#define DRAMC_RFSHCTL0_REFRESH_BURST_5          0x5
#define DRAMC_RFSHCTL0_REFRESH_BURST_6          0x6
#define DRAMC_RFSHCTL0_REFRESH_BURST_7          0x7

/* Refresh Control Register 1 */
typedef union {
	u32 val;
	struct {
	u32 rf_t0_start_value_x32:12;           /* Refresh timer start
						 * for rank 0 (only present
						 * in multi-rank configurations).
						 * This is useful in staggering
						 * the refreshes to multiple
						 * ranks to help traffic
						 * to proceed. This is
						 * explained in Refresh
						 * Controls section of
						 * architecture chapter.
						 * Unit: Multiples of 32
						 * DFI clock cycles.
						 * FOR PERFORMANCE ONLY.
						 * Value After Reset: 0x0
						 * Exists: MEMC_NUM_RANKS>1
						 * Programming Mode: Dynamic
						 * - Refresh Related */
	u32 rsvd0:4;                            /* - */
	u32 rf_t1_start_value_x32:12;           /* Refresh timer start
						 * for rank 1 (only present
						 * in multi-rank configurations).
						 * This is useful in staggering
						 * the refreshes to multiple
						 * ranks to help traffic
						 * to proceed. This is
						 * explained in Refresh
						 * Controls section of
						 * architecture chapter.
						 * Unit: Multiples of 32
						 * DFI clock cycles.
						 * FOR PERFORMANCE ONLY.
						 * Value After Reset: 0x0
						 * Exists: MEMC_NUM_RANKS>1
						 * Programming Mode: Dynamic
						 * - Refresh Related */
	u32 rsvd1:4;                            /* - */
	} bits;
} reg_dramc_rfshctl1_t;

/* Refresh Control Register 3 */
typedef union {
	u32 val;
	struct {
	u32 dis_auto_refresh:1;                 /* When '1', disable auto-refresh
						 * generated by the uMCTL2.
						 * When auto-refresh is
						 * disabled, the SoC core
						 * must generate refreshes
						 * using the registers
						 * DBGCMD.rankn_refresh.
						 * When dis_auto_refresh
						 * transitions from 0 to
						 * 1, any pending refreshes
						 * are immediately scheduled
						 * by the uMCTL2.
						 * This register field
						 * is changeable on the fly.
						 * [0x0]: Disabled[0x1]:
						 * Enabled
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Dynamic
						 * - Refresh Related */
	u32 refresh_update_level:1;             /* Toggle this signal (either
						 * from 0 to 1 or from
						 * 1 to 0) to indicate
						 * that the refresh register(s)
						 * have been updated. refresh_update_level
						 * must not be toggled
						 * when the DDRC is
						 * in reset (core_ddrc_rstn
						 * = 0).
						 * The refresh register(s)
						 * are automatically updated
						 * when exiting reset.
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Dynamic */
	u32 rsvd0:30;                           /* - */
	} bits;
} reg_dramc_rfshctl3_t;

#define DRAMC_RFSHCTL3_DIS_AUTO_REFRESH_0       0x0

/* Refresh Timing Register */
typedef union {
	u32 val;
	struct {
	u32 t_rfc_min:10;               /* tRFC (min): Minimum time from
					 * refresh to refresh or activate.
					 * When the controller is operating
					 * in 1:1 mode, t_rfc_min should
					 * be set to RoundUp(tRFCmin/tCK).
					 * When the controller is operating
					 * in 1:2 mode, t_rfc_min should
					 * be set to RoundUp(RoundUp(tRFCmin/tCK)/2).
					 * Unit: Clocks.
					 * Value After Reset: 0x8c
					 * Exists: Always
					 * Programming Mode: Dynamic -
					 * Refresh Related */
	u32 rsvd0:6;                    /* - */
	u32 t_rfc_nom_x1_x32:12;        /* tREFI: Average time interval
					 * between refreshes per rank (Specification:
					 * 7.8us for DDR2, DDR3).
					 * When the controller is operating
					 * in 1:2 frequency ratio mode,
					 * program this to (tREFI/2), no
					 * rounding up.
					 * Note that if RFSHTMG.t_rfc_nom_x1_sel
					 * == 1, RFSHTMG.t_rfc_nom_x1_x32
					 * must be greater than RFSHTMG.t_rfc_min;
					 * if RFSHTMG.t_rfc_nom_x1_sel
					 * == 0, RFSHTMG.t_rfc_nom_x1_x32
					 * * 32 must be greater than RFSHTMG.t_rfc_min;
					 * RFSHTMG.t_rfc_nom_x1_x32 must
					 * be greater than 0x1.
					 * Fixed 1x mode: RFSHTMG.t_rfc_nom_x1_x32
					 * must be less than or equal to 0xFFE.
					 * Unit: Clocks or multiples of
					 * 32 clocks, depending on
					 * RFSHTMG.t_rfc_nom_x1_sel.
					 * Value After Reset: 0x62
					 * Exists: Always
					 * Programming Mode: Dynamic -
					 * Refresh Related */
	u32 rsvd1:4;                    /* - */
	} bits;
} reg_dramc_rfshtmg_t;

/* SDRAM Initialization Register 0 */
typedef union {
	u32 val;
	struct {
	u32 pre_cke_x1024:12;             /* Cycles to wait after reset before
					 * driving CKE high to start the
					 * SDRAM initialization sequence.
					 * Unit: 1024 DFI clock cycles.
					 * DDR2 specifications typically
					 * require this to be programmed
					 * for a delay of >= 200 us.
					 * When the controller is operating
					 * in 1:2 frequency ratio mode,
					 * program this to JEDEC spec value
					 * divided by 2, and round it up
					 * to the next integer value.
					 * For DDR3 RDIMMs, this should
					 * include the time needed to satisfy
					 * tSTAB
					 * Value After Reset: 0x4e
					 * Exists: Always Programming Mode:
					 * Static */
	u32 rsvd0:4;                    /* - */
	u32 post_cke_x1024:10;          /* Cycles to wait after driving
					 * CKE high to start the SDRAM
					 * initialization sequence.
					 * Unit: 1024 DFI clock cycles.
					 * DDR2 typically requires a 400
					 * ns delay, requiring this value
					 * to be programmed to 2 at all
					 * clock speeds.
					 * When the controller is operating
					 * in 1:2 frequency ratio mode,
					 * program this to JEDEC spec value
					 * divided by 2, and round it up
					 * to the next integer value.
					 * Value After Reset: 0x2
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd1:4;                    /* - */
	u32 skip_dram_init:2;           /* If lower bit is enabled the
					 * SDRAM initialization routine
					 * is skipped. The upper bit decides
					 * what state the controller starts
					 * up in when reset is removed
					 * [0x0]: SDRAM Initialization
					 * routine is run after power-up
					 * [0x1]: SDRAM Initialization
					 * routine is skipped after power-
					 * up. Controller starts up in
					 * Normal Mode
					 * [0x2]: Reserved.
					 * [0x3]: SDRAM Initialization
					 * routine is skipped after power-
					 * up. Controller starts up in
					 * Self-refresh Mode
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Quasi-dynamic
					 * Group 2 */
	} bits;
} reg_dramc_init0_t;

#define DRAMC_INIT0_SKIP_DRAM_INIT_0    0x0
#define DRAMC_INIT0_SKIP_DRAM_INIT_1    0x1
#define DRAMC_INIT0_SKIP_DRAM_INIT_2    0x2
#define DRAMC_INIT0_SKIP_DRAM_INIT_3    0x3

/* SDRAM Initialization Register 1 */
typedef union {
	u32 val;
	struct {
	u32 pre_ocd_x32:4;              /* Wait period before driving the
					 * OCD complete command to
					 * SDRAM.
					 * Unit: Counts of a global timer
					 * that pulses every 32 DFI clock
					 * cycles.
					 * There is no known specific requirement
					 * for this; it may be set to zero.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd0:12;                   /* - */
	u32 dram_rstn_x1024:9;          /* Number of cycles to assert SDRAM
					 * reset signal during init sequence.
					 * This is only present for designs
					 * supporting DDR3 devices. For
					 * use with a Synopsys DDR PHY,
					 * this should be set to a minimum of 1.
					 * When the controller is operating
					 * in 1:2 frequency ratio mode,
					 * program this to JEDEC spec value
					 * divided by 2, and round it up
					 * to the next integer value.
					 * Unit: 1024 DFI clock cycles.
					 * Value After Reset: 0x0
					 * Exists: MEMC_DDR3_OR_4_OR_LPDDR4==1
					 * Programming Mode: Static */
	u32 rsvd1:7;                    /* - */
	} bits;
} reg_dramc_init1_t;

/* SDRAM Initialization Register 3 */
typedef union {
	u32 val;
	struct {
	u32 emr:16;     /* DDR2: Value to write to EMR register. Bits 9:7
			 * are for OCD and the setting in this register
			 * is ignored. The uMCTL2 sets those bits appropriately.
			 * DDR3/: Value to write to MR1 register Set bit
			 * 7 to 0. If PHY-evaluation mode training is enabled,
			 * this bit is set appropriately by the uMCTL2
			 * during write leveling.
			 * Value After Reset: 0x510
			 * Exists: Always
			 * Programming Mode: Quasi-dynamic Group 4 */
	u32 mr:16;      /* DDR2: Value to write to MR register. Bit 8 is
			 * for DLL and the setting here is ignored. The
			 * uMCTL2 sets this bit appropriately.
			 * DDR3: Value loaded into MR0 register.
			 * Value After Reset: 0x0
			 * Exists: Always
			 * Programming Mode: Quasi-dynamic Group 1 and Group 4 */
	} bits;
} reg_dramc_init3_t;

/* SDRAM Initialization Register 4 */
typedef union {
	u32 val;
	struct {
	u32 emr3:16;            /* DDR2: Value to write to EMR3 register.
				 * DDR3: Value to write to MR3 register
				 * After Reset: 0x0
				 * Exists: Always
				 * Programming Mode: Quasi-dynamic Group
				 * 2 and Group 4 */
	u32 emr2:16;            /* DDR2: Value to write to EMR2 register.
				 * DDR3: Value to write to MR2 register
				 * Value After Reset: 0x0
				 * Exists: Always
				 * Programming Mode: Quasi-dynamic Group 4 */
	} bits;
} reg_dramc_init4_t;

/* SDRAM Initialization Register 5 */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:16;                   /* - */
	u32 dev_zqinit_x32:8;           /* ZQ initial calibration, tZQINIT.
					 * Present only in designs configured
					 * to support DDR3. DDR3 typically
					 * requires 512 SDRAM clock cycles.
					 * When the controller is operating
					 * in 1:2 frequency ratio mode,
					 * program this to JEDEC spec value
					 * divided by 2, and round it up
					 * to the next integer value.
					 * Unit: 32 DFI clock cycles.
					 * Value After Reset: 0x10
					 * Exists: MEMC_DDR3_OR_4_OR_LPDDR2==1
					 * Programming Mode: Static */
	u32 rsvd1:8;                    /* - */
	} bits;
} reg_dramc_init5_t;

/* DIMM Control Register */
typedef union {
	u32 val;
	struct {
	u32 dimm_stagger_cs_en:1;       /* Staggering enable for multi-rank
					 * accesses (for multi-rank UDIMM,
					 * RDIMM and LRDIMM implementations
					 * only). This is not supported
					 * for mDDR, LPDDR2, LPDDR3 or LPDDR4
					 * SDRAMs.
					 * Note: Even if this bit is set
					 * it does not take care of software
					 * driven MR commands (via MRCTRL0/MRCTRL1),
					 * where software is responsible
					 * to send them to seperate ranks
					 * as appropriate.
					 * [0x0]: Do not stagger accesses
					 * [0x1]: Send all commands to
					 * even and odd ranks seperately
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 dimm_addr_mirr_en:1;        /* Address Mirroring Enable (for
					 * multi-rank UDIMM implementations
					 * and multi-rank DDR4 RDIMM/LRDIMM
					 * implementations).
					 * Some UDIMMs and DDR4 RDIMMs/LRDIMMs
					 * implement address mirroring
					 * for odd ranks, which means that
					 * the following address, bank
					 * address and bank group bits
					 * are swapped: (A3, A4), (A5,
					 * A6), (A7, A8), (BA0, BA1) and
					 * also (A11, A13), (BG0, BG1)
					 * for the DDR4. Setting this bit
					 * ensures that, for mode register
					 * accesses during the automatic
					 * initialization routine, these
					 * bits are swapped within the
					 * uMCTL2 to compensate for this
					 * UDIMM/RDIMM/LRDIMM swapping.
					 * In addition to the automatic
					 * initialization routine, in case
					 * of DDR4
					 * UDIMM/RDIMM/LRDIMM, they are
					 * swapped during the automatic
					 * MRS access to enable/disable
					 * of a particular DDR4 feature.
					 * Note: This has no effect on
					 * the address of any other memory
					 * accesses, or of software-driven
					 * mode register accesses. This
					 * is not supported for mDDR, LPDDR2,
					 * LPDDR3 or
					 * LPDDR4 SDRAMs.
					 * Note: In case of x16 DDR4 DIMMs,
					 * BG1 output of MRS for the odd
					 * ranks is same as BG0 because
					 * BG1 is invalid, hence dimm_dis_bg_mirroring
					 * register must be set to 1.
					 * [0x0]: Do not implement address
					 * mirroring[0x1]: For odd ranks,
					 * implement address mirroring
					 * for MRS commands to during initialization
					 * and for any automatic DDR4 MRS
					 * commands (to be used if UDIMM/RDIMM/LRDIMM
					 * implements address mirroring)
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd0:30;                   /* - */
	} bits;
} reg_dramc_dimmctl_t;

#define DRAMC_DIMMCTL_DIMM_STAGGER_CS_EN_0      0x0
#define DRAMC_DIMMCTL_DIMM_STAGGER_CS_EN_1      0x1
#define DRAMC_DIMMCTL_DIMM_ADDR_MIRR_EN_0       0x0

/* Rank Control Register */
typedef union {
	u32 val;
	struct {
	u32 max_rank_rd:4;              /* Only present for multi-rank
					 * configurations.
					 * Background: Reads to the same
					 * rank can be performed
					 * back-to-back. Reads to different
					 * ranks require additional gap
					 * dictated by the register RANKCTL.diff_rank_rd_gap.
					 * This is to avoid possible data
					 * bus contention as well as to
					 * give PHY enough time to switch
					 * the delay when changing ranks.
					 * The uMCTL2 arbitrates for bus
					 * access on a cycle-by-cycle basis;
					 * therefore after a read is scheduled,
					 * there are few clock cycles (determined
					 * by the value on RANKCTL.diff_rank_rd_gap
					 * register) in which only reads
					 * from the same rank are eligible
					 * to be scheduled. This prevents
					 * reads from other ranks from
					 * having fair access to the data bus.
					 * This parameter represents the
					 * maximum number of reads that
					 * can be scheduled consecutively
					 * to the same rank. After this
					 * number is reached, a delay equal
					 * to RANKCTL.diff_rank_rd_gap
					 * is inserted by the scheduler
					 * to allow all ranks a fair opportunity
					 * to be scheduled. Higher numbers
					 * increase bandwidth utilization,
					 * lower numbers increase fairness.
					 * This feature can be DISABLED
					 * by setting this register to
					 * 0. When set to 0, the Controller
					 * will stay on the same rank as
					 * long as commands are available
					 * for it.
					 * Minimum programmable value is
					 * 0 (feature disabled) and maximum
					 * programmable value is 0xF.
					 * FOR PERFORMANCE ONLY.
					 * Value After Reset: 0xf
					 * Exists: MEMC_NUM_RANKS>1
					 * Programming Mode: Static */
	u32 diff_rank_rd_gap:4;         /* Only present for multi-rank
					 * configurations.
					 * Indicates the number of clocks
					 * of gap in data responses when
					 * performing consecutive reads
					 * to different ranks. This is
					 * used to switch the delays in
					 * the PHY to match the
					 * rank requirements.
					 * This value should consider both
					 * PHY requirement and ODT
					 * requirement.
					 * ¡ö       PHY requirement:
					 * tphy_rdcsgap (see PHY databook
					 * for value of tphy_rdcsgap)
					 * ¡ö       ODT requirement:
					 * The value programmed in this
					 * register takes care of the ODT
					 * switch off timing requirement
					 * when switching ranks during reads.
					 * When the controller is operating
					 * in 1:1 mode, program this to
					 * the larger of PHY requirement
					 * or ODT requirement.
					 * When the controller is operating
					 * in 1:2 mode, program this to
					 * the larger value divided by
					 * two and round it up to the next
					 * integer.
					 * Value After Reset: 0x6
					 * Exists: MEMC_NUM_RANKS>1
					 * Programming Mode: Static */
	u32 diff_rank_wr_gap:4;         /* Only present for multi-rank
					 * configurations.
					 * Indicates the number of clocks
					 * of gap in data responses when
					 * performing consecutive writes
					 * to different ranks. This is
					 * used to switch the delays in
					 * the PHY to match the
					 * rank requirements.    This value
					 * should consider both PHY
					 * requirement and ODT requirement.
					 * ¡ö       PHY requirement:
					 * tphy_wrcsgap (see PHY databook
					 * for value of tphy_wrcsgap)
					 * If CRC feature is enabled, should
					 * be increased by 1.
					 * ¡ö       ODT requirement:
					 * The value programmed in this
					 * register takes care of the ODT
					 * switch off timing requirement
					 * when switching ranks during writes.
					 * When the controller is operating
					 * in 1:1 mode, program this to
					 * the larger of PHY requirement
					 * or ODT requirement.
					 * When the controller is operating
					 * in 1:2 mode, program this to
					 * the larger value divided by
					 * two and round it up to the next
					 * integer.
					 * Value After Reset: 0x6
					 * Exists: MEMC_NUM_RANKS>1
					 * Programming Mode: Quasi-dynamic
					 * Group 2 */
	u32 rsvd0:20;                   /* - */
	} bits;
} reg_dramc_rankctl_t;

/* Channel Control Register */
typedef union {
	u32 val;
	struct {
	u32 dual_channel_en:1;          /* Indicates whether shared-AC
					 * dual channel mode is enabled
					 * [0x0]: Send all commands to
					 * even and odd ranks separately
					 * Single channel mode; Channel
					 * 0 is active, Channel 1 is disabled
					 * [0x1]: Send all commands to
					 * even and odd ranks separately
					 * Shared-AC dual channel mode enabled
					 * Value After Reset: 0x1
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd0:31;                   /* - */
	} bits;
} reg_dramc_chctl_t;

#define DRAMC_CHCTL_DUAL_CHANNEL_EN_0   0x0
#define DRAMC_CHCTL_DUAL_CHANNEL_EN_1   0x1

/* SDRAM Timing Register 0 */
typedef union {
	u32 val;
	struct {
	u32 t_ras_min:6;        /* tRAS(min): Minimum time between activate
				 * and precharge to the same bank.
				 * When the controller is operating in
				 * 1:2 frequency mode, 1T
				 * mode, program this to tRAS(min)/2. No
				 * rounding up.
				 * When the controller is operating in
				 * 1:2 frequency ratio mode,
				 * 2T mode or LPDDR4 mode, program this
				 * to (tRAS(min)/2)
				 * and round it up to the next integer value.
				 * Unit: Clocks
				 * Value After Reset: 0xf
				 * Exists: Always
				 * Programming Mode: Quasi-dynamic Group
				 * 2 and Group 4 */
	u32 rsvd0:2;            /* - */
	u32 t_ras_max:7;        /* tRAS(max): Maximum time between activate
				 * and precharge to same bank. This is
				 * the maximum time that a page can be kept open
				 * Minimum value of this register is 1.
				 * Zero is invalid.
				 * When the controller is operating in
				 * 1:2 frequency ratio mode, program this
				 * to (tRAS(max)-1)/2. No rounding up.
				 * Unit: Multiples of 1024 clocks.
				 * Value After Reset: 0x1b
				 * Exists: Always
				 * Programming Mode: Quasi-dynamic Group
				 * 2 and Group 4 */
	u32 rsvd1:1;            /* - */
	u32 t_faw:6;            /* tFAW Valid only when 8 or more banks(or
				 * banks x bank groups) are present.
				 * In 8-bank design, at most 4 banks must
				 * be activated in a rolling window of
				 * tFAW cycles.
				 * When the controller is operating in
				 * 1:2 frequency ratio mode, program this
				 * to (tFAW/2) and round up to next integer
				 * value. In a 4-bank design, set this
				 * register to 0x1 independent of the 1:1/1:2
				 * frequency mode.
				 * Unit: Clocks
				 * Value After Reset: 0x10
				 * Exists: Always
				 * Programming Mode: Quasi-dynamic Group
				 * 2 and Group 4 */
	u32 rsvd2:2;            /* - */
	u32 wr2pre:7;           /* Minimum time between write and precharge
				 * to same bank. Unit: Clocks
				 * Specifications: WL + BL/2 + tWR = approximately
				 * 8 cycles +
				 * 15 ns = 14 clocks @400MHz and less for
				 * lower frequencies where:
				 * ¡ö       WL = write latency
				 * ¡ö       BL = burst length. This must
				 * match the value programmed in the BL
				 * bit of the mode register to the SDRAM.
				 * BST (burst terminate) is not supported
				 * at present.
				 * ¡ö       tWR = Write recovery time.
				 * This comes directly from the
				 * SDRAM specification.
				 * When the controller is operating in
				 * 1:2 frequency ratio mode,
				 * 1T mode, divide the above value by 2.
				 * No rounding up. When the controller
				 * is operating in 1:2 frequency ratio mode,
				 * 2T mode or LPDDR4 mode, divide the above
				 * value by 2 and
				 * round it up to the next integer value.
				 * Note that, depending on the PHY, if
				 * using LRDIMM, it may be necessary to
				 * adjust the value of this parameter to
				 * compensate for the extra cycle of latency
				 * through the LRDIMM.
				 * Value After Reset: 0xf
				 * Exists: Always
				 * Programming Mode: Quasi-dynamic Group
				 * 1 and Group 2 and Group 4 */
	u32 rsvd3:1;            /* - */
	} bits;
} reg_dramc_dramtmg0_t;

/* SDRAM Timing Register 1 */
typedef union {
	u32 val;
	struct {
	u32 t_rc:7;             /* tRC: Minimum time between activates
				 * to same bank.
				 * When the controller is operating in
				 * 1:2 frequency ratio mode, program this
				 * to (tRC/2) and round up to next integer
				 * value.
				 * Unit: Clocks.
				 * Value After Reset: 0x14
				 * Exists: Always
				 * Programming Mode: Quasi-dynamic Group
				 * 2 and Group 4 */
	u32 rsvd0:1;            /* - */
	u32 rd2pre:6;           /* tRTP: Minimum time from read to precharge
				 * of same bank.
				 * DDR2: tAL + BL/2 + max(tRTP, 2) - 2
				 * DDR3: tAL + max (tRTP, 4)
				 * When the controller is operating in
				 * 1:2 mode, 1T mode,
				 * divide the above value by 2. No rounding up.
				 * When the controller is operating in
				 * 1:2 mode, 2T mode or LPDDR4 mode, divide
				 * the above value by 2 and round it up
				 * to the next integer value.
				 * Unit: Clocks.
				 * Value After Reset: 0x4
				 * Exists: Always
				 * Programming Mode: Quasi-dynamic Group
				 * 1 and Group 2 and Group 4 */
	u32 rsvd1:2;            /* - */
	u32 t_xp:5;             /* tXP: Minimum time after power-down exit
				 * to any operation. For DDR3, this should
				 * be programmed to tXPDLL if slow powerdown
				 * exit is selected in MR0[12].
				 * When the controller is operating in
				 * 1:2 frequency ratio mode, program this
				 * to (tXP/2) and round it up to the next
				 * integer value.
				 * Units: Clocks
				 * Value After Reset: 0x8
				 * Exists: Always
				 * Programming Mode: Quasi-dynamic Group
				 * 2 and Group 4 */
	u32 rsvd2:11;           /* - */
	} bits;
} reg_dramc_dramtmg1_t;

/* SDRAM Timing Register 2 */
typedef union {
	u32 val;
	struct {
	u32 wr2rd:6;                    /* CWL + BL/2 + tWTR
					 * Includes time for bus turnaround,
					 * recovery times, and all per-bank,
					 * per-rank, and global constraints.
					 * Unit: Clocks. Where:
					 * ¡ö       CWL = CAS write latency
					 * ¡ö       BL = burst length.
					 * This must match the value programmed
					 * in the BL bit of the mode register
					 * to the SDRAM
					 * ¡ö       tWTR = internal write
					 * to read command delay. This
					 * comes directly from the SDRAM
					 * specification.
					 * When the controller is operating
					 * in 1:2 mode, divide the value
					 * calculated using the above equation
					 * by 2, and round it up to next
					 * integer.
					 * Value After Reset: 0xd
					 * Exists: Always
					 * Programming Mode: Quasi-dynamic
					 * Group 1 and Group 2 and Group 4 */
	u32 rsvd0:2;                    /* - */
	u32 rd2wr:6;                    /* DDR2/3/mDDR: RL + BL/2 + 2 - WL
					 * Minimum time from read command
					 * to write command. Include time
					 * for bus turnaround and all per-bank,
					 * per-rank, and global constraints.
					 * Please see the relevant PHY
					 * databook for details of what
					 * should be included here.
					 * Unit: Clocks. Where:
					 * ¡ö       WL = write latency
					 * ¡ö       BL = burst length.
					 * This must match the value programmed
					 * in the BL bit of the mode register
					 * to the SDRAM
					 * ¡ö       RL = read latency =
					 * CAS latency
					 * When the controller is operating
					 * in 1:2 frequency ratio mode,
					 * divide the value calculated
					 * using the above equation by
					 * 2, and round it up to next integer.
					 * Note that, depending on the
					 * PHY, if using LRDIMM, it may
					 * be necessary to adjust the value
					 * of this parameter to compensate
					 * for the extra cycle of latency
					 * through the LRDIMM.
					 * Value After Reset: 0x6
					 * Exists: Always
					 * Programming Mode: Quasi-dynamic
					 * Group 1 and Group 2 and Group 4 */
	u32 rsvd1:2;                    /* - */
	u32 read_latency:6;             /* Set to RL
					 * Time from read command to read
					 * data on SDRAM interface. This
					 * must be set to RL.
					 * Note that, depending on the
					 * PHY, if using RDIMM/LRDIMM,
					 * it may be necessary to adjust
					 * the value of RL to compensate
					 * for the extra cycle of latency
					 * through the RDIMM/LRDIMM.
					 * When the controller is operating
					 * in 1:2 frequency ratio mode,
					 * divide the value calculated
					 * using the above equation by
					 * 2, and round it up to next integer.
					 * This register field is not required
					 * for DDR2 and DDR3 (except if
					 * MEMC_TRAINING is set), as the
					 * DFI read and write latencies
					 * defined in DFITMG0 and DFITMG1
					 * are sufficient for those protocols
					 * Unit: clocks
					 * Value After Reset: 0x5
					 * Exists:
					 * MEMC_MOBILE_OR_LPDDR2_OR_DDR4_O
					 * R_TRAINING
					 * ==1
					 * Programming Mode: Quasi-dynamic
					 * Group 1 and Group 2 and Group 4 */
	u32 rsvd2:2;                    /* - */
	u32 write_latency:6;            /* Set to WL
					 * Time from write command to write
					 * data on SDRAM interface. This
					 * must be set to WL.
					 * Note that, depending on the
					 * PHY, if using RDIMM/LRDIMM,
					 * it may be necessary to adjust
					 * the value of WL to compensate
					 * for the extra cycle of latency
					 * through the RDIMM/LRDIMM.
					 * When the controller is operating
					 * in 1:2 frequency ratio mode,
					 * divide the value calculated
					 * using the above equation by
					 * 2, and round it up to next integer.
					 * This register field is not required
					 * for DDR2 and DDR3 (except if
					 * MEMC_TRAINING is set), as the
					 * DFI read and write latencies
					 * defined in DFITMG0 and DFITMG1
					 * are sufficient for those protocols
					 * Unit: clocks
					 * Value After Reset: 0x3
					 * Exists:
					 * MEMC_MOBILE_OR_LPDDR2_OR_DDR4_O
					 * R_TRAINING
					 * ==1
					 * Programming Mode: Quasi-dynamic
					 * Group 1 and Group 2 and Group 4 */
	u32 rsvd3:2;                    /* - */
	} bits;
} reg_dramc_dramtmg2_t;

/* SDRAM Timing Register 3 */
typedef union {
	u32 val;
	struct {
	u32 t_mod:10;           /* tMOD: Parameter used only in DDR3. Cycles
				 * between load mode command and following
				 * non-load mode command.
				 * Set to tMOD if controller is operating
				 * in 1:1 frequency ratio mode, or tMOD/2
				 * (rounded up to next integer) if controller
				 * is operating in 1:2 frequency ratio
				 * mode. Note that if using RDIMM/LRDIMM,
				 * depending on the PHY, it may be necessary
				 * to adjust the value of this parameter
				 * to compensate for the extra cycle of
				 * latency applied to mode register writes
				 * by the RDIMM/LRDIMM chip.
				 * Also note that if using LRDIMM, the
				 * minimum value of this register is tMRD_L2
				 * if controller is operating in 1:1 frequency
				 * ratio mode, or tMRD_L2/2 (rounded up
				 * to next integer) if controller is operating
				 * in 1:2 frequency ratio mode.
				 * Value After Reset: "(MEMC_DDR3_EN==1
				 * || MEMC_DDR4_EN==1 ) ? 0xc : 0x0"
				 * Exists: MEMC_DDR3_OR_4==1
				 * Programming Mode: Quasi-dynamic Group
				 * 2 and Group 4 */
	u32 rsvd0:2;            /* - */
	u32 t_mrd:6;            /* tMRD: Cycles to wait after a mode register
				 * write or read. Depending on the connected
				 * SDRAM, tMRD represents: DDR2/mDDR: Time
				 * from MRS to any command
				 * DDR3: Time from MRS to MRS command
				 * When the controller is operating in
				 * 1:2 frequency ratio mode, program this
				 * to (tMRD/2) and round it up to the next
				 * integer value.
				 * Value After Reset: 0x4
				 * Exists: Always
				 * Programming Mode: Quasi-dynamic Group
				 * 2 and Group 4 */
	u32 rsvd1:14;           /* - */
	} bits;
} reg_dramc_dramtmg3_t;

/* SDRAM Timing Register 4 */
typedef union {
	u32 val;
	struct {
	u32 t_rp:5;             /* tRP: Minimum time from precharge to
				 * activate of same bank.
				 * When the controller is operating in
				 * 1:1 frequency ratio mode, t_rp should
				 * be set to RoundUp(tRP/tCK).
				 * When the controller is operating in
				 * 1:2 frequency ratio mode, t_rp should
				 * be set to RoundDown(RoundUp(tRP/tCK)/2) + 1.
				 * When the controller is operating in
				 * 1:2 frequency ratio mode in LPDDR4,
				 * t_rp should be set to RoundUp(RoundUp(t
				 * RP/tCK)/2).
				 * Unit: Clocks.
				 * Value After Reset: 0x5
				 * Exists: Always
				 * Programming Mode: Quasi-dynamic Group
				 * 2 and Group 4 */
	u32 rsvd0:3;            /* - */
	u32 t_rrd:4;            /* tRRD: Minimum time between activates
				 * from bank
				 * "a" to bank "b"
				 * When the controller is operating in
				 * 1:2 frequency ratio mode, program this
				 * to (tRRD_L/2 or tRRD/2) and round it
				 * up to the next integer value.
				 * Unit: Clocks.
				 * Value After Reset: 0x4
				 * Exists: Always
				 * Programming Mode: Quasi-dynamic Group
				 * 2 and Group 4 */
	u32 rsvd1:4;            /* - */
	u32 t_ccd:4;            /* tCCD: This is the minimum time between
				 * two reads or two writes.
				 * When the controller is operating in
				 * 1:2 frequency ratio mode, program this
				 * to (tCCD_L/2 or tCCD/2) and round it
				 * up to the next integer value.
				 * Unit: clocks.
				 * Value After Reset: 0x4
				 * Exists: Always
				 * Programming Mode: Quasi-dynamic Group
				 * 2 and Group 4 */
	u32 rsvd2:4;            /* - */
	u32 t_rcd:5;            /* tRCD - tAL: Minimum time from activate
				 * to read or write command to same bank.
				 * When the controller is operating in
				 * 1:2 frequency ratio mode, program this
				 * to ((tRCD - tAL)/2) and round it up
				 * to the next integer value.
				 * Minimum value allowed for this register
				 * is 1, which implies minimum (tRCD -
				 * tAL) value to be 2 when the controller
				 * is operating in 1:2 frequency ratio mode.
				 * Unit: Clocks.
				 * Value After Reset: 0x5
				 * Exists: Always
				 * Programming Mode: Quasi-dynamic Group
				 * 1 and Group 2 and Group 4 */
	u32 rsvd3:3;            /* - */
	} bits;
} reg_dramc_dramtmg4_t;

/* SDRAM Timing Register 5 */
typedef union {
	u32 val;
	struct {
	u32 t_cke:5;            /* Minimum number of cycles of CKE HIGH/LOW
				 * during power-down and self refresh.
				 * Set this to tCKE value.
				 * When the controller is operating in
				 * 1:2 frequency ratio mode, program this
				 * to (value described above)/2 and round
				 * it up to the next integer value.
				 * Unit: Clocks.
				 * Value After Reset: 0x3
				 * Exists: Always
				 * Programming Mode: Quasi-dynamic Group
				 * 2 and Group 4 */
	u32 rsvd0:3;            /* - */
	u32 t_ckesr:6;          /* Minimum CKE low width for Self refresh
				 * or Self refresh power down entry to
				 * exit timing in memory clock cycles.
				 * Recommended settings:
				 * ¡ö       DDR2: tCKE
				 * ¡ö       DDR3: tCKE + 1
				 * When the controller is operating in
				 * 1:2 frequency ratio mode, program this
				 * to recommended value divided by two
				 * and round it up to next integer.
				 * Value After Reset: 0x4
				 * Exists: Always
				 * Programming Mode: Quasi-dynamic Group
				 * 2 and Group 4 */
	u32 rsvd1:2;            /* - */
	u32 t_cksre:4;          /* This is the time after Self Refresh
				 * Down Entry that CK is maintained as
				 * a valid clock. Specifies the clock disable
				 * delay after SRE.
				 * Recommended settings:
				 * ¡ö       DDR2: 1
				 * ¡ö       DDR3: max (10 ns, 5 tCK)
				 * When the controller is operating in
				 * 1:2 frequency ratio mode, program this
				 * to recommended value divided by two
				 * and round it up to next integer.
				 * Value After Reset: 0x5
				 * Exists: Always
				 * Programming Mode: Quasi-dynamic Group
				 * 2 and Group 4 */
	u32 rsvd2:4;            /* - */
	u32 t_cksrx:4;          /* This is the time before Self Refresh
				 * Exit that CK is maintained as a valid
				 * clock before issuing SRX. Specifies
				 * the clock stable time before SRX.
				 * Recommended settings:
				 * ¡ö       DDR2: 1
				 * ¡ö       DDR3: tCKSRX
				 * When the controller is operating in
				 * 1:2 frequency ratio mode, program this
				 * to recommended value divided by two
				 * and round it up to next integer.
				 * Value After Reset: 0x5
				 * Exists: Always
				 * Programming Mode: Quasi-dynamic Group
				 * 2 and Group 4 */
	u32 rsvd3:4;            /* - */
	} bits;
} reg_dramc_dramtmg5_t;

/* SDRAM Timing Register 8 */
typedef union {
	u32 val;
	struct {
	u32 t_xs_x32:7;                 /* tXS: Exit Self Refresh to commands
					 * not requiring a locked
					 * DLL.
					 * When the controller is operating
					 * in 1:2 frequency ratio mode,
					 * program this to the above value
					 * divided by 2 and round up to
					 * next integer value.
					 * Unit: Multiples of 32 clocks.
					 * Note: Used only for DDR2, DDR3
					 * SDRAMs.
					 * Value After Reset: 0x5
					 * Exists: Always
					 * Programming Mode: Quasi-dynamic
					 * Group 2 and Group 4 */
	u32 rsvd0:1;                    /* - */
	u32 t_xs_dll_x32:7;             /* tXSDLL: Exit Self Refresh to
					 * commands requiring a locked
					 * DLL.
					 * When the controller is operating
					 * in 1:2 frequency ratio mode,
					 * program this to the above value
					 * divided by 2 and round up to
					 * next integer value.
					 * Unit: Multiples of 32 clocks.
					 * Note: Used only for DDR2, DDR3
					 * SDRAMs.
					 * Value After Reset: 0x44
					 * Exists: Always
					 * Programming Mode: Quasi-dynamic
					 * Group 2 and Group 4 */
	u32 rsvd1:17;                   /* - */
	} bits;
} reg_dramc_dramtmg8_t;

/* SDRAM Timing Register 15 */
typedef union {
	u32 val;
	struct {
	u32 t_stab_x32:8;               /* tSTAB: Stabilization time.
					 * It is required in the following
					 * two cases for DDR3RDIMM :
					 * ¡ö       when exiting power
					 * saving mode, if the clock was
					 * stopped, after re-enabling it
					 * the clock must be stable for
					 * a time specified by tSTAB
					 * ¡ö       after issuing control
					 * words that refers to clock timing
					 * (Specification: 6us for DDR3)
					 * When the controller is operating
					 * in 1:2 frequency ratio mode,
					 * program this to recommended
					 * value divided by two and round
					 * it up to next integer.
					 * Unit: Multiples of 32 clock cycles.
					 * Value After Reset: 0x0
					 * Exists: MEMC_DDR3_OR_4==1
					 * Programming Mode: Quasi-dynamic
					 * Group 2 and Group 4 */
	u32 rsvd0:23;                   /* - */
	u32 en_dfi_lp_t_stab:1;         /* [0x0]: Disable using tSTAB when
					 * exiting DFI LP
					 * [0x1]:Enable using tSTAB when
					 * exiting DFI LP.
					 * Needs to be set when the PHY
					 * is stopping the clock during
					 * DFI LP to save maximum power.
					 * Value After Reset: 0x0
					 * Exists: MEMC_DDR3_OR_4==1
					 * Programming Mode: Quasi-dynamic
					 * Group 2 and Group 4 */
	} bits;
} reg_dramc_dramtmg15_t;

#define DRAMC_DRAMTMG15_EN_DFI_LP_T_STAB_0      0x0
#define DRAMC_DRAMTMG15_EN_DFI_LP_T_STAB_1      0x1

/* ZQ Control Register 0 */
typedef union {
	u32 val;
	struct {
	u32 t_zq_short_nop:10;          /* tZQCS for DDR3: Number of DFI
					 * clock cycles of NOP required
					 * after a ZQCS (ZQ calibration
					 * short)/MPC(ZQ Latch) command
					 * is issued to SDRAM.
					 * When the controller is operating
					 * in 1:2 frequency ratio mode,
					 * program this to tZQCS/2 and
					 * round it up to the next integer
					 * value.
					 * This is only present for designs
					 * supporting DDR3 devices.
					 * Value After Reset: 0x40
					 * Exists: MEMC_DDR3_OR_4_OR_LPDDR2==1
					 * Programming Mode: Static */
	u32 rsvd0:6;                    /* - */
	u32 t_zq_long_nop:11;           /* tZQoper for DDR3: Number of
					 * DFI clock cycles of NOP required
					 * after a ZQCL (ZQ calibration
					 * long)/MPC(ZQ Start) command
					 * is issued to SDRAM.
					 * When the controller is operating
					 * in 1:2 frequency ratio mode:
					 * DDR3: program this to tZQoper/2
					 * and round it up to the next
					 * integer value.
					 * This is only present for designs
					 * supporting DDR3 devices.
					 * Value After Reset: 0x200
					 * Exists: MEMC_DDR3_OR_4_OR_LPDDR2==1
					 * Programming Mode: Static */
	u32 rsvd1:2;                    /* - */
	u32 zq_resistor_shared:1;       /* [0x1]: Disable using tSTAB when
					 * exiting DFI LP
					 * Denotes that ZQ resistor is
					 * shared between ranks.
					 * Means ZQinit/ZQCL/ZQCS/MPC(ZQ
					 * calibration) commands are sent
					 * to one rank at a time with tZQinit/tZQCL/tZQCS/tZQCAL/tZQLAT
					 * timing met between commands
					 * so that commands to different
					 * ranks do not overlap.
					 * [0x0]: Disable using tSTAB when
					 * exiting DFI LP
					 * ZQ resistor is not shared.
					 * This is only present for designs
					 * supporting DDR3 devices.
					 * Value After Reset: 0x0
					 * Exists: MEMC_DDR3_OR_4_OR_LPDDR2==1
					 * Programming Mode: Static */
	u32 dis_srx_zqcl:1;             /* [0x1]: Disable using tSTAB when
					 * exiting DFI LP
					 * Disable issuing of ZQCL/MPC(ZQ
					 * calibration) command at Self-Refresh/SR-Powerdown
					 * exit. Only applicable when run
					 * in DDR3 mode.
					 * [0x0]: Disable using tSTAB when
					 * exiting DFI LP
					 * Enable issuing of ZQCL/MPC(ZQ
					 * calibration) command at Self-Refresh/SR-Powerdown
					 * exit. Only applicable when run
					 * in DDR3 mode.
					 * This is only present for designs
					 * supporting DDR3 devices.
					 * Value After Reset: 0x0
					 * Exists: MEMC_DDR3_OR_4_OR_LPDDR2==1
					 * Programming Mode: Quasi-dynamic
					 * Group 2 and Group 4 */
	u32 dis_auto_zq:1;              /* [0x1]: Disable using tSTAB when
					 * exiting DFI LP
					 *  Disable uMCTL2 generation of
					 * ZQCS/MPC(ZQ calibration) command.
					 * Register DBGCMD.zq_calib_short
					 * can be used instead to issue
					 * ZQ calibration request from
					 * APB module.
					 * [0x0]: Disable using tSTAB when
					 * exiting DFI LP
					 * Internally generate ZQCS/MPC(ZQ
					 * calibration) commands based
					 * on ZQCTL1.t_zq_short_interval_x1024.
					 * This is only present for designs
					 * supporting DDR3 devices.
					 * Value After Reset: 0x0
					 * Exists: MEMC_DDR3_OR_4_OR_LPDDR2==1
					 * Programming Mode: Dynamic */
	} bits;
} reg_dramc_zqctl0_t;

#define DRAMC_ZQCTL0_ZQ_RESISTOR_SHARED_1       0x1
#define DRAMC_ZQCTL0_ZQ_RESISTOR_SHARED_0       0x0
#define DRAMC_ZQCTL0_DIS_SRX_ZQCL_1             0x1
#define DRAMC_ZQCTL0_DIS_SRX_ZQCL_0             0x0
#define DRAMC_ZQCTL0_DIS_AUTO_ZQ_1              0x1
#define DRAMC_ZQCTL0_DIS_AUTO_ZQ_0              0x0

/* ZQ Control Register 1 */
typedef union {
	u32 val;
	struct {
	u32 t_zq_short_int_x1024:20;            /* Average interval to
						 * wait between automatically
						 * issuing ZQCS (ZQ calibration
						 * short)/MPC(ZQ calibration)
						 * commands to DDR3 devices.
						 * Meaningless, if ZQCTL0.dis_auto_zq=1.
						 * Unit: 1024 DFI clock cycles.
						 * This is only present
						 * for designs supporting
						 * DDR3 devices.
						 * Value After Reset: 0x100
						 * Exists: MEMC_DDR3_OR_4_
						 * OR_LPDDR2==1
						 * Programming Mode: Static */
	u32 rsvd0:12;                           /* - */
	} bits;
} reg_dramc_zqctl1_t;

/* DFI Timing Register 0 */
typedef union {
	u32 val;
	struct {
	u32 dfi_tphy_wrlat:6;                   /* Write latency
						 * Number of clocks from
						 * the write command to
						 * write data enable (dfi_wrdata_en).
						 * This corresponds to
						 * the DFI timing parameter
						 * tphy_wrlat.
						 * Refer to PHY specification
						 * for correct value.Note
						 * that, depending on the
						 * PHY, if using RDIMM/LRDIMM,
						 * it may be necessary
						 * to use the adjusted
						 * value of CL in the calculation
						 * of tphy_wrlat. This
						 * is to compensate for
						 * the extra cycle(s) of
						 * latency through the
						 * RDIMM/LRDIMM.
						 * For LPDDR4, dfi_tphy_wrlat>60
						 * is not supported.
						 * Unit: DFI clock cycles
						 * or DFI PHY clock cycles,
						 * depending on DFITMG0.df
						 * i_wrdata_use_dfi_phy_clk.
						 * Value After Reset: 0x2
						 * Exists: Always
						 * Programming Mode: Quasi-dynamic
						 * Group 1 and Group 4 */
	u32 rsvd0:2;                            /* - */
	u32 dfi_tphy_wd:6;                      /* Specifies the number
						 * of clock cycles between
						 * when dfi_wrdata_en is
						 * asserted to when the
						 * associated write data
						 * is driven on the dfi_wrdata
						 * signal. This corresponds
						 * to the DFI timing parameter
						 * tphy_wrdata. Refer to
						 * PHY specification for
						 * correct value. Note,
						 * max supported value is
						 * 8.
						 * Unit: DFI clock cycles
						 * or DFI PHY clock cycles,
						 * depending on DFITMG0.df
						 * i_wrdata_use_dfi_phy_clk.
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Quasi-dynamic
						 * Group 4 */
	u32 rsvd1:1;                            /* - */
	u32 dfi_wd_use_dfi_phy_clk:1;           /* Defines whether dfi_wrdata_en/dfi_wrdata/dfi_wrdata_mask
						 * is generated using HDR
						 * (DFI clock) or SDR (DFI
						 * PHY clock) values Selects
						 * whether value in DFITMG0.dfi_tphy_wrlat
						 * is in terms of HDR (DFI
						 * clock) or SDR (DFI PHY
						 * clock) cycles Selects
						 * whether value in DFITMG0.dfi_tphy_wrdata
						 * is in terms of HDR (DFI
						 * clock) or SDR (DFI PHY
						 * clock) cycles
						 * [0x0]: in terms of HDR
						 * (DFI clock) cycles
						 * [0x1]: in terms of SDR
						 * (DFI PHY clock) cycles
						 * Refer to PHY specification
						 * for correct value. If
						 * using a Synopsys DWC
						 * DDR3/2 PHY, DWC DDR2/3-Lite/mDDR
						 * PHY, DWC DDR multiPHY
						 * or DWC Gen2 DDR multiPHY,
						 * this field must be set
						 * to 0; otherwise:
						 * ¡ö       If MEMC_PROG_FREQ_RATIO=1
						 * and
						 * MSTR.frequency_ratio=1,
						 * this field should be set to 0
						 * ¡ö       Else, it must
						 * be set to 1
						 * Value After Reset: 0x0
						 * Exists: MEMC_FREQ_RATIO==2
						 * Programming Mode: Static */
	u32 dfi_t_rddata_en:7;                  /* Time from the assertion
						 * of a read command on
						 * the DFI interface to
						 * the assertion of the
						 * dfi_rddata_en signal.
						 * Refer to PHY specification
						 * for correct value.
						 * This corresponds to
						 * the DFI parameter trddata_en.
						 * Note that, depending
						 * on the PHY, if using
						 * RDIMM/LRDIMM, it may
						 * be necessary to use
						 * the adjusted value of
						 * CL in the calculation
						 * of trddata_en. This
						 * is to compensate for
						 * the extra cycle(s) of
						 * latency through the
						 * RDIMM/LRDIMM.
						 * Unit: DFI clock cycles
						 * or DFI PHY clock cycles,
						 * depending on DFITMG0.df
						 * i_rddata_use_dfi_phy_clk.
						 * Value After Reset: 0x2
						 * Exists: Always
						 * Programming Mode: Quasi-dynamic
						 * Group 1 and Group 4 */
	u32 dfi_rd_use_dfi_phy_clk:1;           /* Defines whether dfi_rddata_en/dfi_rddata/dfi_rddata_valid
						 * is generated using HDR
						 * (DFI clock) or SDR (DFI
						 * PHY clock) values.
						 * Selects whether value
						 * in DFITMG0.dfi_t_rddata_en
						 * is in terms of HDR (DFI
						 * clock) or SDR (DFI PHY
						 * clock) cycles:
						 * [0x0]: 0 in terms of
						 * HDR (DFI clock) cycles
						 * [0x1]: in terms of SDR
						 * (DFI PHY clock) cycles
						 * Refer to PHY specification
						 * for correct value. If
						 * using a Synopsys DWC
						 * DDR3/2 PHY, DWC DDR2/3-Lite/mDDR
						 * PHY, DWC DDR multiPHY
						 * or DWC Gen2 DDR multiPHY,
						 * this field must be set
						 * to 0; otherwise:
						 * ¡ö       If MEMC_PROG_FREQ_RATIO=1
						 * and
						 * MSTR.frequency_ratio=1,
						 * this field should be set to 0
						 * ¡ö       Else, it must
						 * be set to 1
						 * Value After Reset: 0x0
						 * Exists: MEMC_FREQ_RATIO==2
						 * Programming Mode: Static */
	u32 dfi_t_ctrl_delay:5;                 /* Specifies the number
						 * of DFI clock cycles
						 * after an assertion or
						 * de-assertion of the
						 * DFI control signals
						 * that the control signals
						 * at the PHY-DRAM interface
						 * reflect the assertion
						 * or de-assertion. If
						 * the DFI clock and the
						 * memory clock are not
						 * phase-aligned, this
						 * timing parameter should
						 * be rounded up to the
						 * next integer value.
						 * Note that if using RDIMM/LRDIMM,
						 * it is necessary to increment
						 * this parameter by RDIMM's/LRDIMM's
						 * extra cycle of latency
						 * in terms of DFI clock.
						 * Value After Reset: 0x7
						 * Exists: Always
						 * Programming Mode: Quasi-dynamic
						 * Group 4 */
	u32 rsvd2:3;                            /* - */
	} bits;
} reg_dramc_dfitmg0_t;

#define DRAMC_DFITMG0_DFI_WD_USE_DFI_PHY_CLK_0  0x0
#define DRAMC_DFITMG0_DFI_WD_USE_DFI_PHY_CLK_1  0x1
#define DRAMC_DFITMG0_DFI_RD_USE_DFI_PHY_CLK_0  0x0
#define DRAMC_DFITMG0_DFI_RD_USE_DFI_PHY_CLK_1  0x1

/* DFI Timing Register 1 */
typedef union {
	u32 val;
	struct {
	u32 dfi_t_dram_clk_enable:5;            /* Specifies the number
						 * of DFI clock cycles
						 * from the de- assertion
						 * of the dfi_dram_clk_disable
						 * signal on the DFI until
						 * the first valid rising
						 * edge of the clock to
						 * the DRAM memory devices,
						 * at the PHY-DRAM boundary.
						 * If the DFI clock and
						 * the memory clock are
						 * not phase aligned, this
						 * timing parameter should
						 * be rounded up to the
						 * next integer value.
						 * Value After Reset: 0x4
						 * Exists: Always
						 * Programming Mode: Quasi-dynamic
						 * Group 4 */
	u32 rsvd0:3;                            /* - */
	u32 dfi_t_dram_clk_disable:5;           /* Specifies the number
						 * of DFI clock cycles
						 * from the assertion of
						 * the dfi_dram_clk_disable
						 * signal on the DFI until
						 * the clock to the DRAM
						 * memory devices, at the
						 * PHY-DRAM boundary, maintains
						 * a low value. If the
						 * DFI clock and the memory
						 * clock are not phase
						 * aligned, this timing
						 * parameter should be
						 * rounded up to the next
						 * integer value.
						 * Value After Reset: 0x4
						 * Exists: Always
						 * Programming Mode: Quasi-dynamic
						 * Group 4 */
	u32 rsvd1:3;                            /* - */
	u32 dfi_t_wrdata_delay:5;               /* Specifies the number
						 * of DFI clock cycles
						 * between when the dfi_wrdata_en
						 * signal is asserted and
						 * when the corresponding
						 * write data transfer
						 * is completed on the
						 * DRAM bus. This corresponds
						 * to the DFI timing parameter
						 * twrdata_delay. Refer
						 * to PHY specification
						 * for correct value. For
						 * DFI 3.0 PHY, set to
						 * twrdata_delay, a new
						 * timing parameter introduced
						 * in DFI 3.0. For DFI
						 * 2.1 PHY, set to tphy_wrdata
						 * + (delay of DFI write
						 * data to the DRAM). Value
						 * to be programmed is
						 * in terms of DFI clocks,
						 * not PHY clocks. In FREQ_RATIO=2,
						 * divide PHY's value by
						 * 2 and round up to next
						 * integer. If using
						 * DFITMG0.dfi_wrdata_use_dfi_phy_clk=1,
						 * add 1 to the value.
						 * Unit: Clocks
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Quasi-dynamic
						 * Group 4 */
	u32 rsvd2:11;                           /* - */
	} bits;
} reg_dramc_dfitmg1_t;

/* DFI Low Power Configuration Register 0 */
typedef union {
	u32 val;
	struct {
	u32 dfi_lp_en_pd:1;             /* Enables DFI Low Power interface
					 * handshaking during Power
					 * Down Entry/Exit.
					 * [0x0]: Disabled
					 * [0x1]: Enabled
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd0:3;                    /* - */
	u32 dfi_lp_wakeup_pd:4;         /* Value in DFI clock cycles to
					 * drive on dfi_lp_wakeup signal
					 * when Power Down mode is entered.
					 * Determines the DFI's tlp_wakeup time:
					 * [0x0]:16 cycles
					 * [0x1]: 32 cycles
					 * [0x2]: 64 cycles
					 * [0x3]: 128 cycles
					 * [0x4]: 256 cycles
					 * [0x5]:512 cycles
					 * [0x6]: 1024 cycles
					 * [0x7]:2048 cycles
					 * [0x8]:4096 cycles
					 * [0x9]:8192 cycles
					 * [0xA]:16384 cycles
					 * [0xB]:32768 cycles
					 * [0xC]: 65536 cycles
					 * [0xD]:131072 cycles
					 * [0xE]:262144 cycles
					 * [0xF]:Unlimited
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 dfi_lp_en_sr:1;             /* Enables DFI Low Power interface
					 * handshaking during Self
					 * Refresh Entry/Exit.
					 * [0x0]: Disabled
					 * [0x1]:Enabled
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd1:3;                    /* - */
	u32 dfi_lp_wakeup_sr:4;         /* Value in DFI clpck cycles to
					 * drive on dfi_lp_wakeup signal
					 * when Self Refresh mode is entered.
					 * Determines the DFI's tlp_wakeup time:
					 * [0x0]:16 cycles
					 * [0x1]:32 cycles
					 * [0x2]: 64 cycles
					 * [0x3]:128 cycles
					 * [0x4]: 256 cycles
					 * [0x5]:512 cycles
					 * [0x6]:1024 cycles
					 * [0x7]:2048 cycles
					 * [0x8]:4096 cycles
					 * [0x9]:8192 cycles
					 * [0xA]:16384 cycles
					 * [0xB]:32768 cycles
					 * [0xC]:65536 cycles
					 * [0xD]:131072 cycles
					 * [0xE]:262144 cycles
					 * [0xF]:Unlimited
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd2:8;                    /* - */
	u32 dfi_tlp_resp:5;             /* Setting in DFI clock cycles
					 * for DFI's tlp_resp time.
					 * Same value is used for both
					 * Power Down, Self Refresh, Deep
					 * Power Down and Maximum Power
					 * Saving modes. DFI 2.1 specification
					 * onwards, recommends using a fixed
					 * value of 7 always.
					 * Value After Reset: 0x7
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd3:3;                    /* - */
	} bits;
} reg_dramc_dfilpcfg0_t;

#define DRAMC_DFILPCFG0_DFI_LP_EN_PD_0          0x0
#define DRAMC_DFILPCFG0_DFI_LP_EN_PD_1          0x1
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_PD_0      0x0
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_PD_1      0x1
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_PD_2      0x2
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_PD_3      0x3
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_PD_4      0x4
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_PD_5      0x5
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_PD_6      0x6
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_PD_7      0x7
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_PD_8      0x8
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_PD_9      0x9
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_PD_A      0xA
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_PD_B      0xB
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_PD_C      0xC
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_PD_D      0xD
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_PD_E      0xE
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_PD_F      0xF
#define DRAMC_DFILPCFG0_DFI_LP_EN_SR_0          0x0
#define DRAMC_DFILPCFG0_DFI_LP_EN_SR_1          0x1
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_SR_0      0x0
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_SR_1      0x1
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_SR_2      0x2
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_SR_3      0x3
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_SR_4      0x4
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_SR_5      0x5
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_SR_6      0x6
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_SR_7      0x7
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_SR_8      0x8
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_SR_9      0x9
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_SR_A      0xA
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_SR_B      0xB
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_SR_C      0xC
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_SR_D      0xD
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_SR_E      0xE
#define DRAMC_DFILPCFG0_DFI_LP_WAKEUP_SR_F      0xF

/* DFI Update Register 0 */
typedef union {
	u32 val;
	struct {
	u32 dfi_t_ctrlup_min:10;                /* Specifies the minimum
						 * number of DFI clock
						 * cycles that the dfi_ctrlupd_req
						 * signal must be asserted.
						 * The uMCTL2 expects the
						 * PHY to respond within
						 * this time. If the PHY
						 * does not respond, the
						 * uMCTL2 will de-assert
						 * dfi_ctrlupd_req after
						 * dfi_t_ctrlup_min + 2
						 * cycles. Lowest value
						 * to assign to this variable
						 * is 0x3.
						 * Value After Reset: 0x3
						 * Exists: Always
						 * Programming Mode: Static */
	u32 rsvd0:6;                            /* - */
	u32 dfi_t_ctrlup_max:10;                /* Specifies the maximum
						 * number of DFI clock
						 * cycles that the dfi_ctrlupd_req
						 * signal can assert. Lowest
						 * value to assign to this
						 * variable is 0x40.
						 * Value After Reset: 0x40
						 * Exists: Always
						 * Programming Mode: Static */
	u32 rsvd1:3;                            /* - */
	u32 ctrlupd_pre_srx:1;                  /* Selects dfi_ctrlupd_req
						 * requirements at SRX:
						 * [0x0]: send ctrlupd after SRX
						 * [0x1]: send ctrlupd
						 * before SRX  If DFIUPD0.dis_auto_ctrlupd_srx=1,
						 * this register has no
						 * impact, because no dfi_ctrlupd_req
						 * will be issued when SRX.
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Static */
	u32 dis_auto_ctrlupd_srx:1;             /* When '1', disable the
						 * automatic dfi_ctrlupd_req
						 * generation by the uMCTL2
						 * at self-refresh exit.
						 * When '0', uMCTL2 issues
						 * a dfi_ctrlupd_req before
						 * or after exiting self-refresh,
						 * depending on DFIUPD0.ct
						 * rlupd_pre_srx.
						 * [0x0]: Disabled
						 * [0x1]: Enabled
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Static */
	u32 dis_auto_ctrlupd:1;                 /* When '1', disable the
						 * automatic dfi_ctrlupd_req
						 * generation by the uMCTL2.
						 * The core must issue
						 * the dfi_ctrlupd_req
						 * signal using register
						 * DBGCMD.ctrlupd.
						 * When '0', uMCTL2 issues
						 * dfi_ctrlupd_req periodically.
						 * [0x0]: Disabled
						 * [0x1]: Enabled
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Quasi-dynamic
						 * Group 3 */
	} bits;
} reg_dramc_dfiupd0_t;

#define DRAMC_DFIUPD0_CTRLUPD_PRE_SRX_0         0x0
#define DRAMC_DFIUPD0_CTRLUPD_PRE_SRX_1         0x1
#define DRAMC_DFIUPD0_DIS_AUTO_CTRLUPD_SRX_0    0x0
#define DRAMC_DFIUPD0_DIS_AUTO_CTRLUPD_SRX_1    0x1
#define DRAMC_DFIUPD0_DIS_AUTO_CTRLUPD_0        0x0
#define DRAMC_DFIUPD0_DIS_AUTO_CTRLUPD_1        0x1

/* DFI Update Register 1 */
typedef union {
	u32 val;
	struct {
	u32 dfi_t_cu_int_max_x1024:8;           /* This is the maximum
						 * amount of time between
						 * uMCTL2 initiated DFI
						 * update requests. This
						 * timer resets with each
						 * update request; when
						 * the timer expires dfi_ctrlupd_req
						 * is sent and traffic
						 * is blocked until the
						 * dfi_ctrlupd_ackx is
						 * received. PHY can use
						 * this idle time to recalibrate
						 * the delay lines to the
						 * DLLs. The DFI controller
						 * update is also used
						 * to reset PHY FIFO pointers
						 * in case of data capture
						 * errors. Updates are
						 * required to maintain
						 * calibration over PVT,
						 * but frequent updates
						 * may impact performance.
						 * Minimum allowed value
						 * for this field is 1.
						 * Note: Value programmed
						 * for DFIUPD1.dfi_t_ctrlupd_interval_max_x1024
						 * must be greater than
						 * DFIUPD1.dfi_t_ctrlupd_i
						 * nterval_min_x1024.
						 * Unit: 1024 DFI clock cycles
						 * Value After Reset: 0x1
						 * Exists: Always
						 * Programming Mode: Static */
	u32 rsvd0:8;                            /* - */
	u32 dfi_t_cu_int_min_x1024:8;           /* This is the minimum
						 * amount of time between
						 * uMCTL2 initiated DFI
						 * update requests (which
						 * is executed whenever
						 * the uMCTL2 is idle).
						 * Set this number higher
						 * to reduce the frequency
						 * of update requests,
						 * which can have a small
						 * impact on the latency
						 * of the first read request
						 * when the uMCTL2 is idle.
						 * Minimum allowed value
						 * for this field is 1.
						 * Unit: 1024 DFI clock cycles
						 * Value After Reset: 0x1
						 * Exists: Always
						 * Programming Mode: Static */
	u32 rsvd1:8;                            /* - */
	} bits;
} reg_dramc_dfiupd1_t;

/* DFI Update Register 2 */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:31;                   /* - */
	u32 dfi_phyupd_en:1;            /* Enables the support for acknowledging
					 * PHY-initiated updates:
					 * [0x0]:Disabled
					 * [0x1]: Enabled
					 * Value After Reset: 0x1
					 * Exists: Always
					 * Programming Mode: Static */
	} bits;
} reg_dramc_dfiupd2_t;

#define DRAMC_DFIUPD2_DFI_PHYUPD_EN_0   0x0
#define DRAMC_DFIUPD2_DFI_PHYUPD_EN_1   0x1

/* DFI Miscellaneous Control Register */
typedef union {
	u32 val;
	struct {
	u32 dfi_init_complete_en:1;             /* PHY initialization complete
						 * enable signal.
						 * When asserted the dfi_init_complete
						 * signal can be used to
						 * trigger SDRAM initialization
						 * [0x0]: Disabled[0x1]:
						 * Enabled
						 * Value After Reset: 0x1
						 * Exists: Always
						 * Programming Mode: Quasi-dynamic
						 * Group 3 */
	u32 rsvd0:1;                            /* - */
	u32 dfi_data_cs_pol:1;                  /* Defines polarity of
						 * dfi_wrdata_cs and dfi_rddata_cs
						 * signals.
						 * [0x0]: Signals are active low
						 * [0x1]: Signals are active
						 * high
						 * Value After Reset: 0x0
						 * Exists: UMCTL2_DFI_DATA
						 * _CS_EN==1
						 * Programming Mode: Static */
	u32 share_dfi_dram_clk_dis:1;           /* Indicate dfi_dram_clk_disable
						 * is shared for two channels
						 * or not.
						 * [0x0]: Not share
						 * [0x1]:Share mode
						 * In Shared mode, Controller
						 * does not request PHY
						 * stop clock while any
						 * rank of any channel
						 * has not disable clock.
						 *  Note: when dfi_dram_clk_disable
						 * is shared by two channels,
						 * an additional DFF is
						 * inserted, that will
						 * cause dram clock enable
						 * is delay one cycle.
						 * Suggest set dfi_t_dram_clk_enable
						 * value to Tdram_clk_enable+1.
						 * Tdram_clk_enable value
						 * is from PHY, which indicate
						 * how many cycles from
						 * dfi_dram_clk_disable
						 * de-assert to dram clock
						 * output.
						 * Value After Reset: 0x0
						 * Exists: UMCTL2_SHARED_AC==1
						 * Programming Mode: Static */
	u32 ctl_idle_en:1;                      /* Enables support of ctl_idle
						 * signal, which is non-DFI
						 * related pin specific
						 * to certain Synopsys
						 * PHYs. See signal description
						 * of ctl_idle signal for
						 * further details of ctl_idle
						 * functionality.
						 * [0x0]: Disabled[0x1]:
						 * Enabled
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Static */
	u32 dfi_init_start:1;                   /* PHY init start request
						 * signal.When asserted
						 * it triggers the
						 * PHY init start request
						 * [0x0]: Disabled[0x1]:
						 * Enabled
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Quasi-dynamic
						 * Group 3 */
	u32 rsvd1:2;                            /* - */
	u32 dfi_frequency:5;                    /* Indicates the operating
						 * frequency of the system.
						 * The number of supported
						 * frequencies and the
						 * mapping of signal values
						 * to clock frequencies
						 * are defined by the PHY.
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Quasi-dynamic
						 * Group 1 */
	u32 rsvd2:19;                           /* - */
	} bits;
} reg_dramc_dfimisc_t;

#define DRAMC_DFIMISC_DFI_INIT_COMPLETE_EN_0    0x0
#define DRAMC_DFIMISC_DFI_DATA_CS_POL_0         0x0
#define DRAMC_DFIMISC_DFI_DATA_CS_POL_1         0x1
#define DRAMC_DFIMISC_SHARE_DFI_DRAM_CLK_DIS_0  0x0
#define DRAMC_DFIMISC_SHARE_DFI_DRAM_CLK_DIS_1  0x1
#define DRAMC_DFIMISC_CTL_IDLE_EN_0             0x0
#define DRAMC_DFIMISC_DFI_INIT_START_0          0x0

/* DFI Timing Register 2 */
typedef union {
	u32 val;
	struct {
	u32 dfi_tphy_wrcslat:6;         /* Number of DFI PHY clock cycles
					 * between when a write command
					 * is sent on the DFI control interface
					 * and when the associated dfi_wrdata_cs
					 * signal is asserted. This corresponds
					 * to the DFI timing parameter
					 * tphy_wrcslat.
					 * Refer to PHY specification for
					 * correct value.
					 * Value After Reset: 0x2
					 * Exists: Always
					 * Programming Mode: Quasi-dynamic
					 * Group 2 and Group 4 */
	u32 rsvd0:2;                    /* - */
	u32 dfi_tphy_rdcslat:7;         /* Number of DFI PHY clock cycles
					 * between when a read command
					 * is sent on the DFI control interface
					 * and when the associated dfi_rddata_cs
					 * signal is asserted. This corresponds
					 * to the DFI timing parameter
					 * tphy_rdcslat. Refer to PHY specification
					 * for correct value.
					 * Value After Reset: 0x2
					 * Exists: Always
					 * Programming Mode: Quasi-dynamic
					 * Group 2 and Group 4 */
	u32 rsvd1:17;                   /* - */
	} bits;
} reg_dramc_dfitmg2_t;

/* DFI Status Register */
typedef union {
	u32 val;
	struct {
	u32 dfi_init_complete:1;        /* The status flag register which
					 * announces when the DFI initialization
					 * has been completed. The DFI
					 * INIT triggered by dfi_init_start
					 * signal and then the dfi_init_complete
					 * flag is polled to know when
					 * the initialization is done.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Dynamic */
	u32 dfi_lp_ack:1;               /* Stores the value of the dfi_lp_ack
					 * input to the controller.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Dynamic */
	u32 rsvd0:30;                   /* - */
	} bits;
} reg_dramc_dfistat_t;

/* DFI PHY Master */
typedef union {
	u32 val;
	struct {
	u32 dfi_phymstr_en:1;           /* Enables the PHY Master Interface:
					 * [0x0]: Disabled[0x1]: Enabled
					 * Value After Reset: 0x1
					 * Exists: Always
					 * Programming Mode: Dynamic */
	u32 rsvd0:31;                   /* - */
	} bits;
} reg_dramc_dfiphymstr_t;

#define DRAMC_DFIPHYMSTR_DFI_PHYMSTR_EN_0       0x0

/* Address Map Register 0 */
typedef union {
	u32 val;
	struct {
	u32 addrmap_cs_bit0:5;          /* Selects the HIF address bit
					 * used as rank address bit 0.
					 * Valid Range: 0 to 29, and 31
					 * Internal Base: 6
					 * The selected HIF address bit
					 * is determined by adding the
					 * internal base to the value of
					 * this field.
					 * If unused, set to 31 and then
					 * rank address bit 0 is set to 0.
					 * Value After Reset: 0x0
					 * Exists: MEMC_NUM_RANKS>1
					 * Programming Mode: Static */
	u32 rsvd0:27;                   /* - */
	} bits;
} reg_dramc_addrmap0_t;

/* Address Map Register 1 */
typedef union {
	u32 val;
	struct {
	u32 addrmap_bank_b0:6;          /* Selects the HIF address bits
					 * used as bank address bit 0.
					 * Valid Range: 0 to 32, and 63
					 * Internal Base: 2
					 * The selected HIF address bit
					 * for each of the bank address
					 * bits is determined by adding
					 * the internal base to the value
					 * of this field.
					 * If unused, set to 63 and then
					 * bank address bit 0 is set to 0.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd0:2;                    /* - */
	u32 addrmap_bank_b1:6;          /* Selects the HIF address bits
					 * used as bank address bit 1.
					 * Valid Range: 0 to 32, and 63
					 * Internal Base: 3
					 * The selected HIF address bit
					 * for each of the bank address
					 * bits is determined by adding
					 * the internal base to the value
					 * of this field.
					 * If unused, set to 63 and then
					 * bank address bit 1 is set to 0.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd1:2;                    /* - */
	u32 addrmap_bank_b2:6;          /* Selects the HIF address bit
					 * used as bank address bit 2.
					 * Valid Range: 0 to 31, and 63
					 * Internal Base: 4
					 * The selected HIF address bit
					 * is determined by adding the
					 * internal base to the value of
					 * this field.
					 * If unused, set to 63 and then
					 * bank address bit 2 is set to 0.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd2:10;                   /* - */
	} bits;
} reg_dramc_addrmap1_t;

/* Address Map Register 2 */
typedef union {
	u32 val;
	struct {
	u32 addrmap_col_b2:4;           /* ¡ö       Full bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 2.
					 * ¡ö       Half bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 3.
					 * ¡ö       Quarter bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 4.
					 * Valid Range: 0 to 7
					 * Internal Base: 2
					 * The selected HIF address bit
					 * is determined by adding the
					 * internal base to the value of
					 * this field.
					 * Note, if UMCTL2_INCL_ARB=1 and
					 * MEMC_BURST_LENGTH=8, it is required
					 * to program this to
					 * 0 unless:
					 * ¡ö       in Half or Quarter
					 * bus width (MSTR.data_bus_width!=00)
					 * and
					 * ¡ö       PCCFG.bl_exp_mode==1
					 * and either
					 * ¡ö       In DDR4  and ADDRMAP8.addrmap_bg_b0==0
					 * or
					 * ¡ö       In LPDDR4 and ADDRMAP1
					 * .addrmap_bank_b0==0
					 * If UMCTL2_INCL_ARB=1 and MEMC_BURST_LENGTH=16,
					 * it is required to program this
					 * to 0 unless:
					 * ¡ö       in Half or Quarter
					 * bus width (MSTR.data_bus_width!=00)
					 * and
					 * ¡ö       PCCFG.bl_exp_mode==1 and
					 * ¡ö       In DDR4 and ADDRMAP8.a
					 * ddrmap_bg_b0==0
					 * Otherwise, if MEMC_BURST_LENGTH=8
					 * and Full Bus Width (MSTR.data_bus_width==00),
					 * it is recommended to program
					 * this to 0 so that HIF[2] maps
					 * to column address bit
					 * 2.
					 * If MEMC_BURST_LENGTH=16 and
					 * Full Bus Width (MSTR.data_bus_width==00),
					 * it is recommended to program
					 * this to 0 so that HIF[2] maps
					 * to column address bit 2.
					 * If MEMC_BURST_LENGTH=16 and
					 * Half Bus Width (MSTR.data_bus_width==01),
					 * it is recommended to program
					 * this to 0 so that HIF[2] maps
					 * to column address bit 3.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd0:4;                    /* - */
	u32 addrmap_col_b3:4;           /* ¡ö       Full bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 3.
					 * ¡ö       Half bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 4.
					 * ¡ö       Quarter bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 5.
					 * Valid Range: 0 to 7
					 * Internal Base: 3
					 * The selected HIF address bit
					 * is determined by adding the
					 * internal base to the value of
					 * this field.
					 * Note, if UMCTL2_INCL_ARB=1,
					 * MEMC_BURST_LENGTH=16, Full bus
					 * width (MSTR.data_bus_width=00)
					 * and BL16 (MSTR.burst_rdwr=1000),
					 * it is recommended to program
					 * this to 0.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd1:4;                    /* - */
	u32 addrmap_col_b4:4;           /* ¡ö       Full bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 4.
					 * ¡ö       Half bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 5.
					 * ¡ö       Quarter bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 6.
					 * Valid Range: 0 to 7, and 15
					 * Internal Base: 4
					 * The selected HIF address bit
					 * is determined by adding the
					 * internal base to the value of
					 * this field.
					 * If unused, set to 15 and then
					 * this column address bit is set to 0.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd2:4;                    /* - */
	u32 addrmap_col_b5:4;           /* ¡ö       Full bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 5.
					 * ¡ö       Half bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 6.
					 * ¡ö       Quarter bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 7 .
					 * Valid Range: 0 to 7, and 15
					 * Internal Base: 5
					 * The selected HIF address bit
					 * is determined by adding the
					 * internal base to the value of
					 * this field.
					 * If unused, set to 15 and then
					 * this column address bit is set to 0.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd3:4;                    /* - */
	} bits;
} reg_dramc_addrmap2_t;

/* Address Map Register 3 */
typedef union {
	u32 val;
	struct {
	u32 addrmap_col_b6:4;           /* ¡ö       Full bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 6.
					 * ¡ö       Half bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 7.
					 * ¡ö       Quarter bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 8.
					 * Valid Range: 0 to 7, and 15
					 * Internal Base: 6
					 * The selected HIF address bit
					 * is determined by adding the
					 * internal base to the value of
					 * this field.
					 * If unused, set to 15 and then
					 * this column address bit is set to 0.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd0:4;                    /* - */
	u32 addrmap_col_b7:5;           /* ¡ö       Full bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 7.
					 * ¡ö       Half bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 8.
					 * ¡ö       Quarter bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 9.
					 * Valid Range: 0 to 7, x, and
					 * 31. x indicate a valid value
					 * in inline ECC configuration.
					 * Internal Base: 7
					 * The selected HIF address bit
					 * is determined by adding the
					 * internal base to the value of
					 * this field.
					 * In Inline ECC configuration
					 * (MEMC_INLINE_ECC=1) and ECC
					 * is enabled (ECCCFG0.ecc_mode>0),
					 * the highest 3 column address
					 * bits must map to the highest
					 * 3 valid HIF address bits.
					 * If column bit 7 is the third
					 * highest column address bit,
					 * it must map to the third highest
					 * valid HIF address bit. (x =
					 * the highest valid HIF address
					 * bit - 2 - internal base)
					 * If unused, set to 31 and then
					 * this column address bit is set to 0.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd1:3;                    /* - */
	u32 addrmap_col_b8:5;           /* ¡ö       Full bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 8.
					 * ¡ö       Half bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 9.
					 * ¡ö       Quarter bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 11
					 * (10 in LPDDR2/LPDDR3 mode).
					 * Valid Range: 0 to 7, x, and
					 * 31. x indicate a valid value
					 * in inline ECC configuration.
					 * Internal Base: 8
					 * The selected HIF address bit
					 * is determined by adding the
					 * internal base to the value of
					 * this field.
					 * Note: Per JEDEC DDR2/3/mDDR
					 * specification, column address
					 * bit 10 is reserved for indicating
					 * auto-precharge, and hence no
					 * source address bit can be mapped
					 * to column address bit 10.
					 * In LPDDR2/LPDDR3, there is a
					 * dedicated bit for auto- precharge
					 * in the CA bus and hence column
					 * bit 10 is used.
					 * In Inline ECC configuration
					 * (MEMC_INLINE_ECC=1) and ECC
					 * is enabled (ECCCFG0.ecc_mode>0),
					 * the highest 3 column address
					 * bits must map to the highest
					 * 3 valid HIF address bits.
					 * If column bit 8 is the second
					 * highest column address bit,
					 * it must map to the second highest
					 * valid HIF address bit. (x =
					 * the highest valid HIF address
					 * bit - 1 - internal base)
					 * If column bit 8 is the third
					 * highest column address bit,
					 * it must map to the third highest
					 * valid HIF address bit. (x =
					 * the highest valid HIF address
					 * bit - 2 - internal base)
					 * If unused, set to 31 and then
					 * this column address bit is set to 0.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd2:3;                    /* - */
	u32 addrmap_col_b9:5;           /* ¡ö       Full bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 9.
					 * ¡ö       Half bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 11
					 * (10 in LPDDR2/LPDDR3 mode).
					 * ¡ö       Quarter bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 13
					 * (11 in LPDDR2/LPDDR3 mode).
					 * Valid Range: 0 to 7, x, and
					 * 31. x indicate a valid value
					 * in inline ECC configuration.
					 * Internal Base: 9
					 * The selected HIF address bit
					 * is determined by adding the
					 * internal base to the value of
					 * this field.
					 * Note: Per JEDEC DDR2/3/mDDR
					 * specification, column address
					 * bit 10 is reserved for indicating
					 * auto-precharge, and hence no
					 * source address bit can be mapped
					 * to column address bit 10.
					 * In LPDDR2/LPDDR3, there is a
					 * dedicated bit for auto- precharge
					 * in the CA bus and hence column
					 * bit 10 is used.
					 * In Inline ECC configuration
					 * (MEMC_INLINE_ECC=1) and ECC
					 * is enabled (ECCCFG0.ecc_mode>0),
					 * the highest 3 column address
					 * bits must map to the highest
					 * 3 valid HIF address bits.
					 * If column bit 9 is the highest
					 * column address bit, it must
					 * map to the highest valid HIF
					 * address bit. (x = the highest
					 * valid HIF address bit - internal
					 * base)
					 * If column bit 9 is the second
					 * highest column address bit,
					 * it must map to the second highest
					 * valid HIF address bit. (x =
					 * the highest valid HIF address
					 * bit - 1 - internal base)
					 * If column bit 9 is the third
					 * highest column address bit,
					 * it must map to the third highest
					 * valid HIF address bit. (x =
					 * the highest valid HIF address
					 * bit - 2 - internal base)
					 * If unused, set to 31 and then
					 * this column address bit is set to 0.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd3:3;                    /* - */
	} bits;
} reg_dramc_addrmap3_t;

/* Address Map Register 4 */
typedef union {
	u32 val;
	struct {
	u32 addrmap_col_b10:5;          /* ¡ö       Full bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 11
					 * (10 in LPDDR2/LPDDR3 mode).
					 * ¡ö       Half bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 13
					 * (11 in LPDDR2/LPDDR3 mode).
					 * ¡ö       Quarter bus width mode:
					 * UNUSED. See later in this description
					 * for value you need to set to
					 * make it unused.
					 * Valid Range: 0 to 7, x, and
					 * 31. x indicate a valid value
					 * in inline ECC configuration.
					 * Internal Base: 10
					 * The selected HIF address bit
					 * is determined by adding the
					 * internal base to the value of
					 * this field.
					 * Note: Per JEDEC DDR2/3/mDDR
					 * specification, column address
					 * bit 10 is reserved for indicating
					 * auto-precharge, and hence no
					 * source address bit can be mapped
					 * to column address bit 10.
					 * In LPDDR2/LPDDR3, there is a
					 * dedicated bit for auto- precharge
					 * in the CA bus and hence column
					 * bit 10 is used.
					 * In Inline ECC configuration
					 * (MEMC_INLINE_ECC=1) and ECC
					 * is enabled (ECCCFG0.ecc_mode>0),
					 * the highest 3 column address
					 * bits must map to the highest
					 * 3 valid HIF address bits.
					 * If column bit 10 is the highest
					 * column address bit, it must
					 * map to the highest valid HIF
					 * address bit. (x = the highest
					 * valid HIF address bit - internal
					 * base)
					 * If column bit 10 is the second
					 * highest column address bit,
					 * it must map to the second highest
					 * valid HIF address bit. (x =
					 * the highest valid HIF address
					 * bit - 1 - internal base)
					 * If column bit 10 is the third
					 * highest column address bit,
					 * it must map to the third highest
					 * valid HIF address bit. (x =
					 * the highest valid HIF address
					 * bit - 2 - internal base)
					 * If unused, set to 31 and then
					 * this column address bit is set to 0.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd0:3;                    /* - */
	u32 addrmap_col_b11:5;          /* ¡ö       Full bus width mode:
					 * Selects the HIF address bit
					 * used as column address bit 13
					 * (11 in LPDDR2/LPDDR3 mode).
					 * ¡ö       Half bus width mode:
					 * UNUSED. See later in this description
					 * for value you need to set to
					 * make it unused.
					 * ¡ö       Quarter bus width mode:
					 * UNUSED. See later in this description
					 * for value you need to set to
					 * make it unused.
					 * Valid Range: 0 to 7, x, and
					 * 31. x indicate a valid value
					 * in inline ECC configuration.
					 * Internal Base: 11
					 * The selected HIF address bit
					 * is determined by adding the
					 * internal base to the value of
					 * this field.
					 * Note: Per JEDEC DDR2/3/mDDR
					 * specification, column address
					 * bit 10 is reserved for indicating
					 * auto-precharge, and hence no
					 * source address bit can be mapped
					 * to column address bit 10.
					 * In LPDDR2/LPDDR3, there is a
					 * dedicated bit for auto- precharge
					 * in the CA bus and hence column
					 * bit 10 is used.
					 * In Inline ECC configuration
					 * (MEMC_INLINE_ECC=1) and ECC
					 * is enabled (ECCCFG0.ecc_mode>0),
					 * the highest 3 column address
					 * bits must map to the highest
					 * 3 valid HIF address bits.
					 * If column bit 11 is the highest
					 * column address bit, it must
					 * map to the highest valid HIF
					 * address bit. (x = the highest
					 * valid HIF address bit - internal
					 * base)
					 * If column bit 11 is the second
					 * highest column address bit,
					 * it must map to the second highest
					 * valid HIF address bit. (x =
					 * the highest valid HIF address
					 * bit - 1 - internal base)
					 * If column bit 11 is the third
					 * highest column address bit,
					 * it must map to the third highest
					 * valid HIF address bit. (x =
					 * the highest valid HIF address
					 * bit - 2 - internal base)
					 * If unused, set to 31 and then
					 * this column address bit is set to 0.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd1:19;                   /* - */
	} bits;
} reg_dramc_addrmap4_t;

/* Address Map Register 5 */
typedef union {
	u32 val;
	struct {
	u32 addrmap_row_b0:4;           /* Selects the HIF address bits
					 * used as row address bit 0. Valid
					 * Range: 0 to 11
					 * Internal Base: 6
					 * The selected HIF address bit
					 * for each of the row address
					 * bits is determined by adding
					 * the internal base to the value
					 * of this field.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd0:4;                    /* - */
	u32 addrmap_row_b1:4;           /* Selects the HIF address bits
					 * used as row address bit 1. Valid
					 * Range: 0 to 11
					 * Internal Base: 7
					 * The selected HIF address bit
					 * for each of the row address
					 * bits is determined by adding
					 * the internal base to the value
					 * of this field.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd1:4;                    /* - */
	u32 addrmap_row_b2_10:4;        /* Selects the HIF address bits
					 * used as row address bits 2 to
					 * 10.
					 * Valid Range: 0 to 11, and 15
					 * Internal Base: 8 (for row address
					 * bit 2), 9 (for row address bit
					 * 3), 10 (for row address bit
					 * 4) etc increasing to 16 (for
					 * row address bit 10)
					 * The selected HIF address bit
					 * for each of the row address
					 * bits is determined by adding
					 * the internal base to the value
					 * of this field.
					 * When set to 15, the values of
					 * row address bits 2 to 10 are
					 * defined by registers ADDRMAP9,
					 * ADDRMAP10, ADDRMAP11.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd2:4;                    /* - */
	u32 addrmap_row_b11:4;          /* Selects the HIF address bit
					 * used as row address bit 11.
					 * Valid Range: 0 to 11, and 15
					 * Internal Base: 17
					 * The selected HIF address bit
					 * is determined by adding the
					 * internal base to the value of
					 * this field.
					 * If unused, set to 15 and then
					 * row address bit 11 is set to 0.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd3:4;                    /* - */
	} bits;
} reg_dramc_addrmap5_t;

/* Address Map Register 6 */
typedef union {
	u32 val;
	struct {
	u32 addrmap_row_b12:4;          /* Selects the HIF address bit
					 * used as row address bit 12.
					 * Valid Range: 0 to 11, and 15
					 * Internal Base: 18
					 * The selected HIF address bit
					 * is determined by adding the
					 * internal base to the value of
					 * this field.
					 * If unused, set to 15 and then
					 * row address bit 12 is set to 0.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd0:4;                    /* - */
	u32 addrmap_row_b13:4;          /* Selects the HIF address bit
					 * used as row address bit 13.
					 * Valid Range: 0 to 11, and 15
					 * Internal Base: 19
					 * The selected HIF address bit
					 * is determined by adding the
					 * internal base to the value of
					 * this field.
					 * If unused, set to 15 and then
					 * row address bit 13 is set to 0.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd1:4;                    /* - */
	u32 addrmap_row_b14:4;          /* Selects the HIF address bit
					 * used as row address bit 14.
					 * Valid Range: 0 to 11, and 15
					 * Internal Base: 20
					 * The selected HIF address bit
					 * is determined by adding the
					 * internal base to the value of
					 * this field.
					 * If unused, set to 15 and then
					 * row address bit 14 is set to 0.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd2:4;                    /* - */
	u32 addrmap_row_b15:4;          /* Selects the HIF address bit
					 * used as row address bit 15.
					 * Valid Range: 0 to 11, and 15
					 * Internal Base: 21
					 * The selected HIF address bit
					 * is determined by adding the
					 * internal base to the value of
					 * this field.
					 * If unused, set to 15 and then
					 * row address bit 15 is set to 0.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd3:4;                    /* - */
	} bits;
} reg_dramc_addrmap6_t;

/* Address Map Register 9 */
typedef union {
	u32 val;
	struct {
	u32 addrmap_row_b2:4;           /* Selects the HIF address bits
					 * used as row address bit 2. Valid
					 * Range: 0 to 11
					 * Internal Base: 8
					 * The selected HIF address bit
					 * for each of the row address
					 * bits is determined by adding
					 * the internal base to the value
					 * of this field. This register
					 * field is used only when ADDRMAP5.addrmap_row_b2_10
					 * is set to value 15.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd0:4;                    /* - */
	u32 addrmap_row_b3:4;           /* Selects the HIF address bits
					 * used as row address bit 3. Valid
					 * Range: 0 to 11
					 * Internal Base: 9
					 * The selected HIF address bit
					 * for each of the row address
					 * bits is determined by adding
					 * the internal base to the value
					 * of this field. This register
					 * field is used only when ADDRMAP5.addrmap_row_b2_10
					 * is set to value 15.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd1:4;                    /* - */
	u32 addrmap_row_b4:4;           /* Selects the HIF address bits
					 * used as row address bit 4. Valid
					 * Range: 0 to 11
					 * Internal Base: 10
					 * The selected HIF address bit
					 * for each of the row address
					 * bits is determined by adding
					 * the internal base to the value
					 * of this field. This register
					 * field is used only when ADDRMAP5.addrmap_row_b2_10
					 * is set to value 15.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd2:4;                    /* - */
	u32 addrmap_row_b5:4;           /* Selects the HIF address bits
					 * used as row address bit 5. Valid
					 * Range: 0 to 11
					 * Internal Base: 11
					 * The selected HIF address bit
					 * for each of the row address
					 * bits is determined by adding
					 * the internal base to the value
					 * of this field. This register
					 * field is used only when ADDRMAP5.addrmap_row_b2_10
					 * is set to value 15.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd3:4;                    /* - */
	} bits;
} reg_dramc_addrmap9_t;

/* Address Map Register 10 */
typedef union {
	u32 val;
	struct {
	u32 addrmap_row_b6:4;           /* Selects the HIF address bits
					 * used as row address bit 6. Valid
					 * Range: 0 to 11
					 * Internal Base: 12
					 * The selected HIF address bit
					 * for each of the row address
					 * bits is determined by adding
					 * the internal base to the value
					 * of this field. This register
					 * field is used only when ADDRMAP5.addrmap_row_b2_10
					 * is set to value 15.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd0:4;                    /* - */
	u32 addrmap_row_b7:4;           /* Selects the HIF address bits
					 * used as row address bit 7. Valid
					 * Range: 0 to 11
					 * Internal Base: 13
					 * The selected HIF address bit
					 * for each of the row address
					 * bits is determined by adding
					 * the internal base to the value
					 * of this field. This register
					 * field is used only when ADDRMAP5.addrmap_row_b2_10
					 * is set to value 15.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd1:4;                    /* - */
	u32 addrmap_row_b8:4;           /* Selects the HIF address bits
					 * used as row address bit 8. Valid
					 * Range: 0 to 11
					 * Internal Base: 14
					 * The selected HIF address bit
					 * for each of the row address
					 * bits is determined by adding
					 * the internal base to the value
					 * of this field. This register
					 * field is used only when ADDRMAP5.addrmap_row_b2_10
					 * is set to value 15.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd2:4;                    /* - */
	u32 addrmap_row_b9:4;           /* Selects the HIF address bits
					 * used as row address bit 9. Valid
					 * Range: 0 to 11
					 * Internal Base: 15
					 * The selected HIF address bit
					 * for each of the row address
					 * bits is determined by adding
					 * the internal base to the value
					 * of this field. This register
					 * field is used only when ADDRMAP5.addrmap_row_b2_10
					 * is set to value 15.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd3:4;                    /* - */
	} bits;
} reg_dramc_addrmap10_t;

/* Address Map Register 11 */
typedef union {
	u32 val;
	struct {
	u32 addrmap_row_b10:4;          /* Selects the HIF address bits
					 * used as row address bit 10.
					 * Valid Range: 0 to 11
					 * Internal Base: 16
					 * The selected HIF address bit
					 * for each of the row address
					 * bits is determined by adding
					 * the internal base to the value
					 * of this field. This register
					 * field is used only when ADDRMAP5.addrmap_row_b2_10
					 * is set to value 15.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd0:28;                   /* - */
	} bits;
} reg_dramc_addrmap11_t;

/* ODT Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 rsvd0:2;                    /* - */
	u32 rd_odt_delay:5;             /* The delay, in DFI PHY clock
					 * cycles, from issuing a read
					 * command to setting ODT values
					 * associated with that command.
					 * ODT setting must remain constant
					 * for the entire time that DQS
					 * is driven by the uMCTL2.
					 * Recommended values:
					 * DDR2:
					 * ¡ö       CL + AL - 4 (not DDR2-1066),
					 * CL + AL - 5 (DDR2-1066)
					 * If (CL + AL - 4 < 0), uMCTL2
					 * does not support ODT for read
					 * operation.
					 * DDR3:
					 * ¡ö       CL - CWL
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Quasi-dynamic
					 * Group 1 and Group 4 */
	u32 rsvd1:1;                    /* - */
	u32 rd_odt_hold:4;              /* DFI PHY clock cycles to hold
					 * ODT for a read command. The
					 * minimum supported value is 2.
					 * Recommended values:
					 * DDR2:
					 * ¡ö       BL8: 0x6 (not DDR2-1066),
					 * 0x7 (DDR2-1066)
					 * ¡ö       BL4: 0x4 (not DDR2-1066),
					 * 0x5 (DDR2-1066)
					 * DDR3:
					 * ¡ö       BL8 - 0x6
					 * Value After Reset: 0x4
					 * Exists: Always
					 * Programming Mode: Quasi-dynamic
					 * Group 1 and Group 4 */
	u32 rsvd2:4;                    /* - */
	u32 wr_odt_delay:5;             /* The delay, in DFI PHY clock
					 * cycles, from issuing a write
					 * command to setting ODT values
					 * associated with that command.
					 * ODT setting must remain constant
					 * for the entire time that DQS
					 * is driven by the uMCTL2.
					 * Recommended values:
					 * DDR2:
					 * ¡ö       CWL + AL - 3 (DDR2-400/533/667),
					 * CWL + AL - 4 (DDR2-800), CWL
					 * + AL - 5 (DDR2-1066)
					 * If (CWL + AL - 3 < 0), uMCTL2
					 * does not support ODT for write
					 * operation.
					 * DDR3:
					 * ¡ö       0x0
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Quasi-dynamic
					 * Group 1 and Group 4 */
	u32 rsvd3:3;                    /* - */
	u32 wr_odt_hold:4;              /* DFI PHY clock cycles to hold
					 * ODT for a write command. The
					 * minimum supported value is 2.
					 * Recommended values:
					 * DDR2:
					 * ¡ö       BL8: 0x5 (DDR2-400/533/667),
					 * 0x6 (DDR2-800), 0x7 (DDR2-1066)
					 * ¡ö       BL4: 0x3 (DDR2-400/533/667),
					 * 0x4 (DDR2-800), 0x5 (DDR2-1066)
					 * DDR3:
					 * ¡ö       BL8: 0x6
					 * Value After Reset: 0x4
					 * Exists: Always
					 * Programming Mode: Quasi-dynamic
					 * Group 1 and Group 4 */
	u32 rsvd4:4;                    /* - */
	} bits;
} reg_dramc_odtcfg_t;

/* ODT/Rank Map Register */
typedef union {
	u32 val;
	struct {
	u32 rank0_wr_odt:2;             /* Indicates which remote ODTs
					 * must be turned on during a write
					 * to rank 0.
					 * Each rank has a remote ODT (in
					 * the SDRAM) which can be turned
					 * on by setting the appropriate
					 * bit here.
					 * Rank 0 is controlled by the
					 * LSB; rank 1 is controlled by
					 * bit next to the LSB, etc.
					 * For each rank, set its bit to
					 * 1 to enable its ODT.
					 * Value After Reset: 0x1
					 * Exists: Always
					 * Programming Mode: Static
					 * Range Variable[x]: "MEMC_NUM_RANKS"
					 * - 1 */
	u32 rsvd0:2;                    /* - */
	u32 rank0_rd_odt:2;             /* Indicates which remote ODTs
					 * must be turned on during a read
					 * from rank 0.
					 * Each rank has a remote ODT (in
					 * the SDRAM) which can be turned
					 * on by setting the appropriate
					 * bit here.
					 * Rank 0 is controlled by the
					 * LSB; rank 1 is controlled by
					 * bit next to the LSB, etc.
					 * For each rank, set its bit to
					 * 1 to enable its ODT.
					 * Value After Reset: 0x1
					 * Exists: Always
					 * Programming Mode: Static
					 * Range Variable[x]: "MEMC_NUM_RANKS"
					 * + 3 */
	u32 rsvd1:2;                    /* - */
	u32 rank1_wr_odt:2;             /* Indicates which remote ODTs
					 * must be turned on during a write
					 * to rank 1.
					 * Each rank has a remote ODT (in
					 * the SDRAM) which can be turned
					 * on by setting the appropriate
					 * bit here.
					 * Rank 0 is controlled by the
					 * LSB; rank 1 is controlled by
					 * bit next to the LSB, etc.
					 * For each rank, set its bit to
					 * 1 to enable its ODT.
					 * Present only in configurations
					 * that have 2 or more ranks Value
					 * After Reset: "(MEMC_NUM_RANKS>1)
					 * ? 0x2 : 0x0" Exists: MEMC_NUM_RANKS>1
					 * Programming Mode: Static */
	u32 rsvd2:2;                    /* - */
	u32 rank1_rd_odt:2;             /* Indicates which remote ODTs
					 * must be turned on during a read
					 * from rank 1.
					 * Each rank has a remote ODT (in
					 * the SDRAM) which can be turned
					 * on by setting the appropriate
					 * bit here.
					 * Rank 0 is controlled by the
					 * LSB; rank 1 is controlled by
					 * bit next to the LSB, etc.
					 * For each rank, set its bit to
					 * 1 to enable its ODT.
					 * Present only in configurations
					 * that have 2 or more ranks
					 * Value After Reset: "(MEMC_NUM_RANKS>1)
					 * ? 0x2 : 0x0" Exists: MEMC_NUM_RANKS>1
					 * Programming Mode: Static */
	u32 rsvd3:18;                   /* - */
	} bits;
} reg_dramc_odtmap_t;

/* Scheduler Control Register */
typedef union {
	u32 val;
	struct {
	u32 force_low_pri_n:1;                  /* Active low signal. When
						 * asserted ('0'), all
						 * incoming transactions
						 * are forced to low priority.
						 * This implies that all
						 * High Priority Read (HPR)
						 * and Variable Priority
						 * Read commands (VPR)
						 * will be treated as Low
						 * Priority Read (LPR)
						 * commands. On the write
						 * side, all Variable Priority
						 * Write (VPW) commands
						 * will be treated as Normal
						 * Priority Write (NPW)
						 * commands. Forcing the
						 * incoming transactions
						 * to low priority implicitly
						 * turns off Bypass path
						 * for read commands.
						 * FOR PERFORMANCE ONLY.
						 * Value After Reset: 0x1
						 * Exists: (UPCTL2_EN ==
						 * 1) ? 1 : 0==0
						 * Programming Mode: Static */
	u32 prefer_write:1;                     /* If set then the bank
						 * selector prefers writes
						 * over reads. FOR DEBUG ONLY.
						 * Value After Reset: 0x0
						 * Exists: (UPCTL2_EN ==
						 * 1) ? 1 : 0==0
						 * Programming Mode: Static */
	u32 pageclose:1;                        /* If true, bank is kept
						 * open only while there
						 * are page hit transactions
						 * available in the CAM
						 * to that bank. The last
						 * read or write command
						 * in the CAM with a bank
						 * and page hit will be
						 * executed with auto-precharge
						 * if SCHED1.pageclose_timer=0.
						 * Even if this register
						 * set to 1 and SCHED1.pageclose_timer
						 * is set to 0, explicit
						 * precharge (and not auto-precharge)
						 * may be issued in some
						 * cases where there is
						 * a mode switch between
						 * Write and Read or between
						 * LPR and HPR. The Read
						 * and Write commands that
						 * are executed as part
						 * of the ECC scrub requests
						 * are also executed without
						 * auto-precharge.
						 * If false, the bank remains
						 * open until there is
						 * a need to close it (to
						 * open a different page,
						 * or for page timeout
						 * or refresh timeout)
						 * - also known as open
						 * page policy. The open
						 * page policy can be overridden
						 * by setting the per-command-
						 * autopre bit on the HIF
						 * interface (hif_cmd_autopre).
						 * The pageclose feature
						 * provids a midway between
						 * Open and
						 * Close page policies.
						 * FOR PERFORMANCE ONLY.
						 * Value After Reset: 0x1
						 * Exists: Always
						 * Programming Mode: Quasi-dynamic
						 * Group 3 */
	u32 rsvd0:5;                            /* - */
	u32 lpr_num_entries:8;                  /* Number of entries in
						 * the low priority transaction
						 * store is this value + 1.
						 * (MEMC_NO_OF_ENTRY -
						 * (SCHED.lpr_num_entries
						 * + 1)) is the number
						 * of entries available
						 * for the high priority
						 * transaction store.
						 * Setting this to maximum
						 * value allocates all
						 * entries to low priority
						 * transaction store.
						 * Setting this to 0 allocates
						 * 1 entry to low priority
						 * transaction store and
						 * the rest to high priority
						 * transaction store.
						 * Note: In ECC configurations,
						 * the numbers of write
						 * and low priority read
						 * credits issued is one
						 * less than in the non-ECC
						 * case. One entry each
						 * is reserved in the write
						 * and low- priority read
						 * CAMs for storing the
						 * RMW requests arising
						 * out of single bit error
						 * correction RMW operation.
						 * Value After Reset: "MEM
						 * C_NO_OF_ENTRY/2"
						 * Exists: Always
						 * Programming Mode: Static
						 * Range Variable[x]: "MEMC_RDCMD_ENTRY_BITS"
						 * + 7 */
	u32 go2critical_hysteresis:8;           /* UNUSED
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Static */
	u32 rdwr_idle_gap:7;                    /* When the preferred transaction
						 * store is empty for these
						 * many clock cycles, switch
						 * to the alternate transaction
						 * store if it is non-empty.
						 * The read transaction
						 * store (both high and
						 * low priority) is the
						 * default preferred transaction
						 * store and the write
						 * transaction store is
						 * the alternative store.
						 * When prefer write over
						 * read is set this is reversed.
						 * 0x0 is a legal value
						 * for this register. When
						 * set to 0x0, the transaction
						 * store switching will
						 * happen immediately when
						 * the switching conditions
						 * become true.
						 * FOR PERFORMANCE ONLY
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Static */
	u32 rsvd1:1;                            /* - */
	} bits;
} reg_dramc_sched_t;

/* Scheduler Control Register 1 */
typedef union {
	u32 val;
	struct {
	u32 pageclose_timer:8;          /* This field works in conjunction
					 * with SCHED.pageclose. It only
					 * has meaning if SCHED.pageclose==1.
					 * If SCHED.pageclose==1 and pageclose_timer==0,
					 * then an auto-precharge may be
					 * scheduled for last read or write
					 * command in the CAM with a bank
					 * and page hit. Note, sometimes
					 * an explicit precharge is scheduled
					 * instead of the auto-precharge.
					 * See SCHED.pageclose for details
					 * of when this may happen.
					 * If SCHED.pageclose==1 and pageclose_timer>0,
					 * then an auto-precharge is not
					 * scheduled for last read or write
					 * command in the CAM with a bank
					 * and page hit. Instead, a timer
					 * is started, with pageclose_timer
					 * as the initial value. There
					 * is a timer on a per bank basis.
					 * The timer decrements unless
					 * the next read or write in the
					 * CAM to a bank is a page hit.
					 * It gets reset to pageclose_timer
					 * value if the next read or write
					 * in the CAM to a bank is a page
					 * hit. Once the timer has reached
					 * zero, an explcit precharge will
					 * be attempted to be scheduled.
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Static */
	u32 rsvd0:24;                   /* - */
	} bits;
} reg_dramc_sched1_t;

/* High Priority Read CAM Register 1 */
typedef union {
	u32 val;
	struct {
	u32 hpr_max_starve:16;          /* Number of DFI clocks that the
					 * HPR queue can be starved before
					 * it goes critical. The minimum
					 * valid functional value for this
					 * register is 0x1. Programming
					 * it to 0x0 will disable the starvation
					 * functionality; during normal
					 * operation, this function should
					 * not be disabled as it will cause
					 * excessive latencies.
					 * FOR PERFORMANCE ONLY.
					 * Value After Reset: 0x1
					 * Exists: Always
					 * Programming Mode: Quasi-dynamic
					 * Group 3 */
	u32 rsvd0:8;                    /* - */
	u32 hpr_xact_run_length:8;      /* Number of transactions that
					 * are serviced once the HPR
					 * queue goes critical is the smaller
					 * of:
					 * ¡ö       (a) This number
					 * ¡ö       (b) Number of transactions
					 * available.
					 * Unit: Transaction.
					 * FOR PERFORMANCE ONLY.
					 * Value After Reset: 0xf
					 * Exists: Always
					 * Programming Mode: Quasi-dynamic
					 * Group 3 */
	} bits;
} reg_dramc_perfhpr1_t;

/* Low Priority Read CAM Register 1 */
typedef union {
	u32 val;
	struct {
	u32 lpr_max_starve:16;          /* Number of DFI clocks that the
					 * LPR queue can be starved before
					 * it goes critical. The minimum
					 * valid functional value for this
					 * register is 0x1. Programming
					 * it to 0x0 will disable the starvation
					 * functionality; during normal
					 * operation, this function should
					 * not be disabled as it will cause
					 * excessive latencies.
					 * FOR PERFORMANCE ONLY.
					 * Value After Reset: 0x7f
					 * Exists: Always
					 * Programming Mode: Quasi-dynamic
					 * Group 3 */
	u32 rsvd0:8;                    /* - */
	u32 lpr_xact_run_length:8;      /* Number of transactions that
					 * are serviced once the LPR
					 * queue goes critical is the smaller
					 * of:
					 * ¡ö       (a) This number
					 * ¡ö       (b) Number of transactions
					 * available. Unit: Transaction.
					 * FOR PERFORMANCE ONLY.
					 * Value After Reset: 0xf
					 * Exists: Always
					 * Programming Mode: Quasi-dynamic
					 * Group 3 */
	} bits;
} reg_dramc_perflpr1_t;

/* Write CAM Register 1 */
typedef union {
	u32 val;
	struct {
	u32 w_max_starve:16;            /* Number of DFI clocks that the
					 * WR queue can be starved before
					 * it goes critical. The minimum
					 * valid functional value for this
					 * register is 0x1. Programming
					 * it to 0x0 will disable the starvation
					 * functionality; during normal
					 * operation, this function should
					 * not be disabled as it will cause
					 * excessive latencies.
					 * FOR PERFORMANCE ONLY. Value
					 * After Reset: 0x7f Exists: Always
					 * Programming Mode: Quasi-dynamic
					 * Group 3 */
	u32 rsvd0:8;                    /* - */
	u32 w_xact_run_length:8;        /* Number of transactions that
					 * are serviced once the WR
					 * queue goes critical is the smaller
					 * of:
					 * ¡ö       (a) This number
					 * ¡ö       (b) Number of transactions
					 * available. Unit: Transaction.
					 * FOR PERFORMANCE ONLY.
					 * Value After Reset: 0xf
					 * Exists: Always
					 * Programming Mode: Quasi-dynamic
					 * Group 3 */
	} bits;
} reg_dramc_perfwr1_t;

/* Debug Register 0 */
typedef union {
	u32 val;
	struct {
	u32 dis_wc:1;                           /* When 1, disable write
						 * combine. FOR DEBUG ONLY
						 * Value After Reset: "(UPCTL2_EN==1)
						 * ? 0x1 : 0x0"
						 * Exists: (UPCTL2_EN ==
						 * 1) ? 1 : 0==0
						 * Programming Mode: Static */
	u32 dis_rd_bypass:1;                    /* Only present in designs
						 * supporting read bypass.
						 * When 1, disable bypass
						 * path for high priority
						 * read page hits
						 * FOR DEBUG ONLY.
						 * Value After Reset: 0x0
						 * Exists: MEMC_BYPASS==1
						 * Programming Mode: Static */
	u32 dis_act_bypass:1;                   /* Only present in designs
						 * supporting activate bypass.
						 * When 1, disable bypass
						 * path for high priority
						 * read activates
						 * [0x0]: Disabled[0x1]:
						 * Enabled
						 * FOR DEBUG ONLY.
						 * Value After Reset: 0x0
						 * Exists: MEMC_BYPASS==1
						 * Programming Mode: Static */
	u32 rsvd0:1;                            /* - */
	u32 dis_collision_page_opt:1;           /* When this is set to
						 * '0', auto-precharge
						 * is disabled for the
						 * flushed command in a
						 * collision case. Collision
						 * cases are write followed
						 * by read to same address,
						 * read followed by write
						 * to same address, or
						 * write followed by write
						 * to same address with
						 * DBG0.dis_wc bit = 1
						 * (where same address
						 * comparisons exclude
						 * the two address bits
						 * representing critical word).
						 * [0x0]: Disabled[0x1]:
						 * Enabled
						 * FOR DEBUG ONLY.
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Static */
	u32 rsvd1:27;                           /* - */
	} bits;
} reg_dramc_dbg0_t;

#define DRAMC_DBG0_DIS_ACT_BYPASS_0             0x0
#define DRAMC_DBG0_DIS_COLLISION_PAGE_OPT_0     0x0

/* Debug Register 1 */
typedef union {
	u32 val;
	struct {
	u32 dis_dq:1;           /* When 1, uMCTL2 will not de-queue any
				 * transactions from the CAM. Bypass is
				 * also disabled. All transactions are
				 * queued in the CAM. No reads or writes
				 * are issued to SDRAM as long as this
				 * is asserted.
				 * This bit may be used to prevent reads
				 * or writes being issued by the uMCTL2,
				 * which makes it safe to modify certain
				 * register fields associated with reads
				 * and writes (see User Guide for details).
				 * After setting this bit, it is strongly
				 * recommended to poll DBGCAM.wr_data_pipeline_empty
				 * and DBGCAM.rd_data_pipeline_empty, before
				 * making changes to any registers which
				 * affect reads and writes. This will ensure
				 * that the relevant logic in the DDRC is idle.
				 * This bit is intended to be switched
				 * on-the-fly.
				 * Value After Reset: 0x0
				 * Exists: Always
				 * Programming Mode: Dynamic */
	u32 dis_hif:1;          /* When 1, uMCTL2 asserts the HIF command
				 * signal hif_cmd_stall. uMCTL2 will ignore
				 * the hif_cmd_valid and all other associated
				 * request signals.
				 * This bit is intended to be switched
				 * on-the-fly.
				 * Value After Reset: 0x0
				 * Exists: Always
				 * Programming Mode: Dynamic */
	u32 rsvd0:30;           /* - */
	} bits;
} reg_dramc_dbg1_t;

/* CAM Debug Register */
typedef union {
	u32 val;
	struct {
	u32 dbg_hpr_q_depth:8;                  /* High priority read queue
						 * depth
						 * FOR DEBUG ONLY
						 * Value After Reset: 0x0
						 * Exists: (UPCTL2_EN ==
						 * 1) ? 1 : 0==0
						 * Programming Mode: Dynamic
						 * Range Variable[x]: "MEMC_RDCMD_ENTRY_BITS
						 * + 1" - 1 */
	u32 dbg_lpr_q_depth:8;                  /* Low priority read queue depth
						 * The last entry of Lpr
						 * queue is reserved for
						 * ECC SCRUB operation.
						 * This entry is not included
						 * in the calculation of
						 * the queue depth.
						 * FOR DEBUG ONLY
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Dynamic
						 * Range Variable[x]: "MEMC_RDCMD_ENTRY_BITS
						 * + 1" +
						 * 7 */
	u32 dbg_w_q_depth:8;                    /* Write queue depth
						 * The last entry of WR
						 * queue is reserved for
						 * ECC SCRUB operation.
						 * This entry is not included
						 * in the calculation of
						 * the queue depth.
						 * FOR DEBUG ONLY
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Dynamic
						 * Range Variable[x]: "MEMC_WRCMD_ENTRY_BITS
						 * + 1" +
						 * 15 */
	u32 dbg_stall:1;                        /* Stall
						 * FOR DEBUG ONLY
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Dynamic */
	u32 dbg_rd_q_empty:1;                   /* When 1, all the Read
						 * command queues and Read
						 * data buffers inside
						 * DDRC are empty. This
						 * register is to be used
						 * for debug purpose.
						 * An example use-case
						 * scenario: When Controller
						 * enters Self- Refresh
						 * using the Low-Power
						 * entry sequence, Controller
						 * is expected to have
						 * executed all the commands
						 * in its queues and the
						 * write and read data
						 * drained. Hence this
						 * register should be 1
						 * at that time.
						 * FOR DEBUG ONLY
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Dynamic */
	u32 dbg_wr_q_empty:1;                   /* When 1, all the Write
						 * command queues and Write
						 * data buffers inside
						 * DDRC are empty. This
						 * register is to be used
						 * for debug purpose.
						 * An example use-case
						 * scenario: When Controller
						 * enters Self- Refresh
						 * using the Low-Power
						 * entry sequence, Controller
						 * is expected to have
						 * executed all the commands
						 * in its queues and the
						 * write and read data
						 * drained. Hence this
						 * register should be 1
						 * at that time.
						 * FOR DEBUG ONLY
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Dynamic */
	u32 rsvd0:1;                            /* - */
	u32 rd_data_pipeline_empty:1;           /* This bit indicates that
						 * the read data pipeline
						 * on the DFI interface
						 * is empty. This register
						 * is intended to be polled
						 * at least twice after
						 * setting DBG1.dis_dq,
						 * to ensure that all remaining
						 * commands/data have completed.
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Dynamic */
	u32 wr_data_pipeline_empty:1;           /* This bit indicates that
						 * the write data pipeline
						 * on the DFI interface
						 * is empty. This register
						 * is intended to be polled
						 * at least twice after
						 * setting DBG1.dis_dq,
						 * to ensure that all remaining
						 * commands/data have completed.
						 * Value After Reset: 0x0
						 * Exists: Always
						 * Programming Mode: Dynamic */
	u32 dbg_stall_wr:1;                     /* Stall for Write channel
						 * FOR DEBUG ONLY
						 * Value After Reset: 0x0
						 * Exists: UMCTL2_DUAL_PA_1==1
						 * Programming Mode: Dynamic */
	u32 dbg_stall_rd:1;                     /* Stall for Read channel
						 * FOR DEBUG ONLY
						 * Value After Reset: 0x0
						 * Exists: UMCTL2_DUAL_PA_1==1
						 * Programming Mode: Dynamic */
	} bits;
} reg_dramc_dbgcam_t;

/* Command Debug Register */
typedef union {
	u32 val;
	struct {
	u32 rank0_refresh:1;            /* Setting this register bit to
					 * 1 indicates to the uMCTL2 to
					 * issue a refresh to rank 0. Writing
					 * to this bit causes DBGSTAT.rank0_refresh_busy
					 * to be set. When DBGSTAT.rank0_refresh_busy
					 * is cleared, the command has
					 * been stored in uMCTL2.
					 * For 3DS configuration, refresh
					 * is sent to rank index 0. This
					 * operation can be performed only
					 * when RFSHCTL3.dis_auto_refresh=1.
					 * It is recommended NOT to
					 * set this register bit if in
					 * Init or Deep power-down operating
					 * modes or Maximum Power Saving Mode.
					 * [0x0]: Disabled[0x1]: Enabled
					 * Value After Reset: 0x0
					 * Exists: Always Testable: readOnly
					 * Programming Mode: Dynamic */
	u32 rank1_refresh:1;            /* Setting this register bit to
					 * 1 indicates to the uMCTL2 to
					 * issue a refresh to rank 1. Writing
					 * to this bit causes DBGSTAT.rank1_refresh_busy
					 * to be set. When DBGSTAT.rank1_refresh_busy
					 * is cleared, the command has
					 * been stored in uMCTL2.
					 * For 3DS configuration, refresh
					 * is sent to rank index 1. This
					 * operation can be performed only
					 * when RFSHCTL3.dis_auto_refresh=1.
					 * It is recommended NOT to
					 * set this register bit if in
					 * Init or Deep power-down operating
					 * modes or Maximum Power Saving Mode.
					 * [0x0]: Disabled[0x1]: Enabled
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_NUM_LRANKS_TOTAL>1
					 * Testable: readOnly
					 * Programming Mode: Dynamic */
	u32 rsvd0:2;                    /* - */
	u32 zq_calib_short:1;           /* Setting this register bit to
					 * 1 indicates to the uMCTL2 to
					 * issue a ZQCS (ZQ calibration
					 * short)/MPC(ZQ calibration) command
					 * to the SDRAM. When this request
					 * is stored in the uMCTL2, the
					 * bit is automatically cleared.
					 * This operation can be performed
					 * only when ZQCTL0.dis_auto_zq=1.
					 * It is recommended NOT to set
					 * this register bit if in Init,
					 * in Self- Refresh or Deep power-down
					 * operating modes or Maximum Power
					 * Saving Mode.
					 * For Self-Refresh, it will be
					 * scheduled after SR has been exited.
					 * For Deep power down and Maximum
					 * Power Saving Mode, it will not
					 * be scheduled, although DBGSTAT.zq_calib_short_busy
					 * will be de-asserted.
					 * [0x0]: Disabled[0x1]: Enabled
					 * Value After Reset: 0x0
					 * Exists: MEMC_DDR3_OR_4_OR_LPDDR2==1
					 * Testable: readOnly
					 * Programming Mode: Dynamic */
	u32 ctrlupd:1;                  /* Setting this register bit to
					 * 1 indicates to the uMCTL2 to
					 * issue a dfi_ctrlupd_req to the
					 * PHY. When this request is stored
					 * in the uMCTL2, the bit is automatically
					 * cleared. This operation must
					 * only be performed when DFIUPD0.
					 * dis_auto_ctrlupd=1.
					 * [0x0]: Disabled[0x1]: Enabled
					 * Value After Reset: 0x0
					 * Exists: Always Testable: readOnly
					 * Programming Mode: Dynamic */
	u32 rsvd1:26;                   /* - */
	} bits;
} reg_dramc_dbgcmd_t;

#define DRAMC_DBGCMD_RANK0_REFRESH_0    0x0
#define DRAMC_DBGCMD_RANK1_REFRESH_0    0x0
#define DRAMC_DBGCMD_ZQ_CALIB_SHORT_0   0x0
#define DRAMC_DBGCMD_CTRLUPD_0          0x0

/* Status Debug Register */
typedef union {
	u32 val;
	struct {
	u32 rank0_refresh_busy:1;       /* SoC core may initiate a rank0_refresh
					 * operation (refresh operation
					 * to rank 0) only if this signal
					 * is low. This signal goes high
					 * in the clock after DBGCMD.rank0_refresh
					 * is set to one. It goes low when
					 * the rank0_refresh operation
					 * is stored in the uMCTL2. It
					 * is recommended not to perform
					 * rank0_refresh operations when
					 * this signal is high.
					 * [0x0]: Indicates that the SoC
					 * core can initiate a rank0_refresh
					 * operation
					 * [0x1]: Indicates that rank0_refresh
					 * operation has not been stored
					 * yet in the uMCTL2
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Dynamic */
	u32 rank1_refresh_busy:1;       /* SoC core may initiate a rank1_refresh
					 * operation (refresh operation
					 * to rank 1) only if this signal
					 * is low. This signal goes high
					 * in the clock after DBGCMD.rank1_refresh
					 * is set to one. It goes low when
					 * the rank1_refresh operation
					 * is stored in the uMCTL2. It
					 * is recommended not to perform
					 * rank1_refresh operations when
					 * this signal is high.
					 * [0x0]: Indicates that the SoC
					 * core can initiate a rank1_refresh
					 * operation
					 * [0x1]: Indicates that rank1_refresh
					 * operation has not been stored
					 * yet in the uMCTL2
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_NUM_LRANKS_TOTAL>1
					 * Programming Mode: Dynamic */
	u32 rsvd0:2;                    /* - */
	u32 zq_calib_short_busy:1;      /* SoC core may initiate a ZQCS
					 * (ZQ calibration short) operation
					 * only if this signal is low.
					 * This signal goes high in the
					 * clock after the uMCTL2 accepts
					 * the ZQCS request. It goes low
					 * when the ZQCS operation is initiated
					 * in the uMCTL2. It is recommended
					 * not to perform ZQCS operations
					 * when this signal is high.
					 * [0x0]: Indicates that the SoC
					 * core can initiate a ZQCS
					 * operation
					 * [0x1]: Indicates that ZQCS operation
					 * has not been initiated yet in
					 * the uMCTL2
					 * Value After Reset: 0x0
					 * Exists: MEMC_DDR3_OR_4_OR_LPDDR2==1
					 * Programming Mode: Dynamic */
	u32 ctrlupd_busy:1;             /* SoC core may initiate a ctrlupd
					 * operation only if this signal
					 * is low. This signal goes high
					 * in the clock after the uMCTL2
					 * accepts the ctrlupd request.
					 * It goes low when the ctrlupd
					 * operation is initiated in the
					 * uMCTL2. It is recommended not
					 * to perform ctrlupd operations
					 * when this signal is high.
					 * [0x0]:Indicates that the SoC
					 * core can initiate a ctrlupd operation
					 * [0x1]:Indicates that ctrlupd
					 * operation has not been initiated
					 * yet in the uMCTL2
					 * Value After Reset: 0x0
					 * Exists: Always
					 * Programming Mode: Dynamic */
	u32 rsvd1:26;                   /* - */
	} bits;
} reg_dramc_dbgstat_t;

#define DRAMC_DBGSTAT_RANK0_REFRESH_BUSY_0      0x0
#define DRAMC_DBGSTAT_RANK0_REFRESH_BUSY_1      0x1
#define DRAMC_DBGSTAT_RANK1_REFRESH_BUSY_0      0x0
#define DRAMC_DBGSTAT_RANK1_REFRESH_BUSY_1      0x1
#define DRAMC_DBGSTAT_ZQ_CALIB_SHORT_BUSY_0     0x0
#define DRAMC_DBGSTAT_ZQ_CALIB_SHORT_BUSY_1     0x1
#define DRAMC_DBGSTAT_CTRLUPD_BUSY_0            0x0
#define DRAMC_DBGSTAT_CTRLUPD_BUSY_1            0x1

/* Software Register Programming Control Enable */
typedef union {
	u32 val;
	struct {
	u32 sw_done:1;          /* Enable quasi-dynamic register programming
				 * outside reset. Program register to 0
				 * to enable quasi-dynamic programming.
				 * Set back register to 1 once programming
				 * is done.
				 * [0x0]: Disabled[0x1]: Enabled
				 * Value After Reset: 0x1
				 * Exists: Always
				 * Programming Mode: Dynamic */
	u32 rsvd0:31;           /* - */
	} bits;
} reg_dramc_swctl_t;

#define DRAMC_SWCTL_SW_DONE_0   0x0

/* Software Register Programming Control Status */
typedef union {
	u32 val;
	struct {
	u32 sw_done_ack:1;      /* Register programming done. This register
				 * is the echo of SWCTL.sw_done. Wait for
				 * sw_done value 1 to propagate to sw_done_ack
				 * at the end of the programming sequence
				 * to ensure that the correct registers
				 * values are propagated to the destination
				 * clock domains.
				 * Value After Reset: 0x1
				 * Exists: Always Testable: untestable
				 * Programming Mode: Static */
	u32 rsvd0:31;           /* - */
	} bits;
} reg_dramc_swstat_t;

/* AXI Poison Configuration Register. Common for all AXI ports */
typedef union {
	u32 val;
	struct {
	u32 wr_poison_slverr_en:1;      /* If set to 1, enables SLVERR
					 * response for write transaction
					 * poisoning
					 * [0x0]: Disabled[0x1]: Enabled
					 * Value After Reset: 0x1
					 * Exists: Always
					 * Programming Mode: Dynamic */
	u32 rsvd0:3;                    /* - */
	u32 wr_poison_intr_en:1;        /* If set to 1, enables interrupts
					 * for write transaction poisoning
					 * [0x0]: Disabled[0x1]: Enabled
					 * Value After Reset: 0x1
					 * Exists: Always
					 * Programming Mode: Dynamic */
	u32 rsvd1:3;                    /* - */
	u32 wr_poison_intr_clr:1;       /* Interrupt clear for write transaction
					 * poisoning. Allow 2/3 clock cycles
					 * for correct value to propagate
					 * to core logic and clear the
					 * interrupts.
					 * Value After Reset: 0x0
					 * Exists: Always Testable: readOnly
					 * Programming Mode: Dynamic */
	u32 rsvd2:7;                    /* - */
	u32 rd_poison_slverr_en:1;      /* If set to 1, enables SLVERR
					 * response for read transaction
					 * poisoning
					 * [0x0]: Disabled[0x1]: Enabled
					 * Value After Reset: 0x1
					 * Exists: Always
					 * Programming Mode: Dynamic */
	u32 rsvd3:3;                    /* - */
	u32 rd_poison_intr_en:1;        /* If set to 1, enables interrupts
					 * for read transaction poisoning
					 * [0x0]: Disabled[0x1]: Enabled
					 * Value After Reset: 0x1
					 * Exists: Always
					 * Programming Mode: Dynamic */
	u32 rsvd4:3;                    /* - */
	u32 rd_poison_intr_clr:1;       /* Interrupt clear for read transaction
					 * poisoning. Allow 2/3 clock cycles
					 * for correct value to propagate
					 * to core logic and clear the
					 * interrupts.
					 * Value After Reset: 0x0
					 * Exists: Always Testable: readOnly
					 * Programming Mode: Dynamic */
	u32 rsvd5:7;                    /* - */
	} bits;
} reg_dramc_poisoncfg_t;

#define DRAMC_POISONCFG_WR_POISON_SLVERR_EN_0   0x0
#define DRAMC_POISONCFG_WR_POISON_INTR_EN_0     0x0
#define DRAMC_POISONCFG_RD_POISON_SLVERR_EN_0   0x0
#define DRAMC_POISONCFG_RD_POISON_INTR_EN_0     0x0

/* AXI Poison Status Register */
typedef union {
	u32 val;
	struct {
	u32 wr_poison_intr_0:1;         /* Write transaction poisoning
					 * error interrupt for port 0.
					 * This register is a APB clock
					 * copy (double register synchronizer)
					 * of the interrupt asserted when
					 * a transaction is poisoned on
					 * the corresponding AXI port's
					 * write address channel. Bit 0
					 * corresponds to Port 0, and so
					 * on. Interrupt is cleared by
					 * register wr_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_0==1
					 * Programming Mode: Dynamic */
	u32 wr_poison_intr_1:1;         /* Write transaction poisoning
					 * error interrupt for port 1.
					 * This register is a APB clock
					 * copy (double register synchronizer)
					 * of the interrupt asserted when
					 * a transaction is poisoned on
					 * the corresponding AXI port's
					 * write address channel. Bit 0
					 * corresponds to Port 0, and so
					 * on. Interrupt is cleared by
					 * register wr_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_1==1
					 * Programming Mode: Dynamic */
	u32 wr_poison_intr_2:1;         /* Write transaction poisoning
					 * error interrupt for port 2.
					 * This register is a APB clock
					 * copy (double register synchronizer)
					 * of the interrupt asserted when
					 * a transaction is poisoned on
					 * the corresponding AXI port's
					 * write address channel. Bit 0
					 * corresponds to Port 0, and so
					 * on. Interrupt is cleared by
					 * register wr_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_2==1
					 * Programming Mode: Dynamic */
	u32 wr_poison_intr_3:1;         /* Write transaction poisoning
					 * error interrupt for port 3.
					 * This register is a APB clock
					 * copy (double register synchronizer)
					 * of the interrupt asserted when
					 * a transaction is poisoned on
					 * the corresponding AXI port's
					 * write address channel. Bit 0
					 * corresponds to Port 0, and so
					 * on. Interrupt is cleared by
					 * register wr_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_3==1
					 * Programming Mode: Dynamic */
	u32 wr_poison_intr_4:1;         /* Write transaction poisoning
					 * error interrupt for port 4.
					 * This register is a APB clock
					 * copy (double register synchronizer)
					 * of the interrupt asserted when
					 * a transaction is poisoned on
					 * the corresponding AXI port's
					 * write address channel. Bit 0
					 * corresponds to Port 0, and so
					 * on. Interrupt is cleared by
					 * register wr_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_4==1
					 * Programming Mode: Dynamic */
	u32 wr_poison_intr_5:1;         /* Write transaction poisoning
					 * error interrupt for port 5.
					 * This register is a APB clock
					 * copy (double register synchronizer)
					 * of the interrupt asserted when
					 * a transaction is poisoned on
					 * the corresponding AXI port's
					 * write address channel. Bit 0
					 * corresponds to Port 0, and so
					 * on. Interrupt is cleared by
					 * register wr_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_5==1
					 * Programming Mode: Dynamic */
	u32 wr_poison_intr_6:1;         /* Write transaction poisoning
					 * error interrupt for port 6.
					 * This register is a APB clock
					 * copy (double register synchronizer)
					 * of the interrupt asserted when
					 * a transaction is poisoned on
					 * the corresponding AXI port's
					 * write address channel. Bit 0
					 * corresponds to Port 0, and so
					 * on. Interrupt is cleared by
					 * register wr_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_6==1
					 * Programming Mode: Dynamic */
	u32 wr_poison_intr_7:1;         /* Write transaction poisoning
					 * error interrupt for port 7.
					 * This register is a APB clock
					 * copy (double register synchronizer)
					 * of the interrupt asserted when
					 * a transaction is poisoned on
					 * the corresponding AXI port's
					 * write address channel. Bit 0
					 * corresponds to Port 0, and so
					 * on. Interrupt is cleared by
					 * register wr_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_7==1
					 * Programming Mode: Dynamic */
	u32 wr_poison_intr_8:1;         /* Write transaction poisoning
					 * error interrupt for port 8.
					 * This register is a APB clock
					 * copy (double register synchronizer)
					 * of the interrupt asserted when
					 * a transaction is poisoned on
					 * the corresponding AXI port's
					 * write address channel. Bit 0
					 * corresponds to Port 0, and so
					 * on. Interrupt is cleared by
					 * register wr_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_8==1
					 * Programming Mode: Dynamic */
	u32 wr_poison_intr_9:1;         /* Write transaction poisoning
					 * error interrupt for port 9.
					 * This register is a APB clock
					 * copy (double register synchronizer)
					 * of the interrupt asserted when
					 * a transaction is poisoned on
					 * the corresponding AXI port's
					 * write address channel. Bit 0
					 * corresponds to Port 0, and so
					 * on. Interrupt is cleared by
					 * register wr_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_9==1
					 * Programming Mode: Dynamic */
	u32 wr_poison_intr_10:1;        /* Write transaction poisoning
					 * error interrupt for port 10.
					 * This register is a APB clock
					 * copy (double register synchronizer)
					 * of the interrupt asserted when
					 * a transaction is poisoned on
					 * the corresponding AXI port's
					 * write address channel. Bit 0
					 * corresponds to Port 0, and so
					 * on. Interrupt is cleared by
					 * register wr_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_10==1
					 * Programming Mode: Dynamic */
	u32 wr_poison_intr_11:1;        /* Write transaction poisoning
					 * error interrupt for port 11.
					 * This register is a APB clock
					 * copy (double register synchronizer)
					 * of the interrupt asserted when
					 * a transaction is poisoned on
					 * the corresponding AXI port's
					 * write address channel. Bit 0
					 * corresponds to Port 0, and so
					 * on. Interrupt is cleared by
					 * register wr_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_11==1
					 * Programming Mode: Dynamic */
	u32 wr_poison_intr_12:1;        /* Write transaction poisoning
					 * error interrupt for port 12.
					 * This register is a APB clock
					 * copy (double register synchronizer)
					 * of the interrupt asserted when
					 * a transaction is poisoned on
					 * the corresponding AXI port's
					 * write address channel. Bit 0
					 * corresponds to Port 0, and so
					 * on. Interrupt is cleared by
					 * register wr_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_12==1
					 * Programming Mode: Dynamic */
	u32 wr_poison_intr_13:1;        /* Write transaction poisoning
					 * error interrupt for port 13.
					 * This register is a APB clock
					 * copy (double register synchronizer)
					 * of the interrupt asserted when
					 * a transaction is poisoned on
					 * the corresponding AXI port's
					 * write address channel. Bit 0
					 * corresponds to Port 0, and so
					 * on. Interrupt is cleared by
					 * register wr_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_13==1
					 * Programming Mode: Dynamic */
	u32 wr_poison_intr_14:1;        /* Write transaction poisoning
					 * error interrupt for port 14.
					 * This register is a APB clock
					 * copy (double register synchronizer)
					 * of the interrupt asserted when
					 * a transaction is poisoned on
					 * the corresponding AXI port's
					 * write address channel. Bit 0
					 * corresponds to Port 0, and so
					 * on. Interrupt is cleared by
					 * register wr_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_14==1
					 * Programming Mode: Dynamic */
	u32 wr_poison_intr_15:1;        /* Write transaction poisoning
					 * error interrupt for port 15.
					 * This register is a APB clock
					 * copy (double register synchronizer)
					 * of the interrupt asserted when
					 * a transaction is poisoned on
					 * the corresponding AXI port's
					 * write address channel. Bit 0
					 * corresponds to Port 0, and so
					 * on. Interrupt is cleared by
					 * register wr_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_15==1
					 * Programming Mode: Dynamic */
	u32 rd_poison_intr_0:1;         /* Read transaction poisoning error
					 * interrupt for port 0. This register
					 * is a APB clock copy (double
					 * register synchronizer) of the
					 * interrupt asserted when a transaction
					 * is poisoned on the corresponding
					 * AXI port's read address channel.
					 * Bit 0 corresponds to Port 0,
					 * and so on. Interrupt is cleared
					 * by register rd_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_0==1
					 * Programming Mode: Dynamic */
	u32 rd_poison_intr_1:1;         /* Read transaction poisoning error
					 * interrupt for port 1. This register
					 * is a APB clock copy (double
					 * register synchronizer) of the
					 * interrupt asserted when a transaction
					 * is poisoned on the corresponding
					 * AXI port's read address channel.
					 * Bit 0 corresponds to Port 0,
					 * and so on. Interrupt is cleared
					 * by register rd_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_1==1
					 * Programming Mode: Dynamic */
	u32 rd_poison_intr_2:1;         /* Read transaction poisoning error
					 * interrupt for port 2. This register
					 * is a APB clock copy (double
					 * register synchronizer) of the
					 * interrupt asserted when a transaction
					 * is poisoned on the corresponding
					 * AXI port's read address channel.
					 * Bit 0 corresponds to Port 0,
					 * and so on. Interrupt is cleared
					 * by register rd_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_2==1
					 * Programming Mode: Dynamic */
	u32 rd_poison_intr_3:1;         /* Read transaction poisoning error
					 * interrupt for port 3. This register
					 * is a APB clock copy (double
					 * register synchronizer) of the
					 * interrupt asserted when a transaction
					 * is poisoned on the corresponding
					 * AXI port's read address channel.
					 * Bit 0 corresponds to Port 0,
					 * and so on. Interrupt is cleared
					 * by register rd_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_3==1
					 * Programming Mode: Dynamic */
	u32 rd_poison_intr_4:1;         /* Read transaction poisoning error
					 * interrupt for port 4. This register
					 * is a APB clock copy (double
					 * register synchronizer) of the
					 * interrupt asserted when a transaction
					 * is poisoned on the corresponding
					 * AXI port's read address channel.
					 * Bit 0 corresponds to Port 0,
					 * and so on. Interrupt is cleared
					 * by register rd_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_4==1
					 * Programming Mode: Dynamic */
	u32 rd_poison_intr_5:1;         /* Read transaction poisoning error
					 * interrupt for port 5. This register
					 * is a APB clock copy (double
					 * register synchronizer) of the
					 * interrupt asserted when a transaction
					 * is poisoned on the corresponding
					 * AXI port's read address channel.
					 * Bit 0 corresponds to Port 0,
					 * and so on. Interrupt is cleared
					 * by register rd_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_5==1
					 * Programming Mode: Dynamic */
	u32 rd_poison_intr_6:1;         /* Read transaction poisoning error
					 * interrupt for port 6. This register
					 * is a APB clock copy (double
					 * register synchronizer) of the
					 * interrupt asserted when a transaction
					 * is poisoned on the corresponding
					 * AXI port's read address channel.
					 * Bit 0 corresponds to Port 0,
					 * and so on. Interrupt is cleared
					 * by register rd_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_6==1
					 * Programming Mode: Dynamic */
	u32 rd_poison_intr_7:1;         /* Read transaction poisoning error
					 * interrupt for port 7. This register
					 * is a APB clock copy (double
					 * register synchronizer) of the
					 * interrupt asserted when a transaction
					 * is poisoned on the corresponding
					 * AXI port's read address channel.
					 * Bit 0 corresponds to Port 0,
					 * and so on. Interrupt is cleared
					 * by register rd_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_7==1
					 * Programming Mode: Dynamic */
	u32 rd_poison_intr_8:1;         /* Read transaction poisoning error
					 * interrupt for port 8. This register
					 * is a APB clock copy (double
					 * register synchronizer) of the
					 * interrupt asserted when a transaction
					 * is poisoned on the corresponding
					 * AXI port's read address channel.
					 * Bit 0 corresponds to Port 0,
					 * and so on. Interrupt is cleared
					 * by register rd_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_8==1
					 * Programming Mode: Dynamic */
	u32 rd_poison_intr_9:1;         /* Read transaction poisoning error
					 * interrupt for port 9. This register
					 * is a APB clock copy (double
					 * register synchronizer) of the
					 * interrupt asserted when a transaction
					 * is poisoned on the corresponding
					 * AXI port's read address channel.
					 * Bit 0 corresponds to Port 0,
					 * and so on. Interrupt is cleared
					 * by register rd_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_9==1
					 * Programming Mode: Dynamic */
	u32 rd_poison_intr_10:1;        /* Read transaction poisoning error
					 * interrupt for port 10. This
					 * register is a APB clock copy
					 * (double register synchronizer)
					 * of the interrupt asserted when
					 * a transaction is poisoned on
					 * the corresponding AXI port's
					 * read address channel. Bit 0
					 * corresponds to Port 0, and so
					 * on. Interrupt is cleared by
					 * register rd_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_10==1
					 * Programming Mode: Dynamic */
	u32 rd_poison_intr_11:1;        /* Read transaction poisoning error
					 * interrupt for port 11. This
					 * register is a APB clock copy
					 * (double register synchronizer)
					 * of the interrupt asserted when
					 * a transaction is poisoned on
					 * the corresponding AXI port's
					 * read address channel. Bit 0
					 * corresponds to Port 0, and so
					 * on. Interrupt is cleared by
					 * register rd_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_11==1
					 * Programming Mode: Dynamic */
	u32 rd_poison_intr_12:1;        /* Read transaction poisoning error
					 * interrupt for port 12. This
					 * register is a APB clock copy
					 * (double register synchronizer)
					 * of the interrupt asserted when
					 * a transaction is poisoned on
					 * the corresponding AXI port's
					 * read address channel. Bit 0
					 * corresponds to Port 0, and so
					 * on. Interrupt is cleared by
					 * register rd_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_12==1
					 * Programming Mode: Dynamic */
	u32 rd_poison_intr_13:1;        /* Read transaction poisoning error
					 * interrupt for port 13. This
					 * register is a APB clock copy
					 * (double register synchronizer)
					 * of the interrupt asserted when
					 * a transaction is poisoned on
					 * the corresponding AXI port's
					 * read address channel. Bit 0
					 * corresponds to Port 0, and so
					 * on. Interrupt is cleared by
					 * register rd_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_13==1
					 * Programming Mode: Dynamic */
	u32 rd_poison_intr_14:1;        /* Read transaction poisoning error
					 * interrupt for port 14. This
					 * register is a APB clock copy
					 * (double register synchronizer)
					 * of the interrupt asserted when
					 * a transaction is poisoned on
					 * the corresponding AXI port's
					 * read address channel. Bit 0
					 * corresponds to Port 0, and so
					 * on. Interrupt is cleared by
					 * register rd_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_14==1
					 * Programming Mode: Dynamic */
	u32 rd_poison_intr_15:1;        /* Read transaction poisoning error
					 * interrupt for port 15. This
					 * register is a APB clock copy
					 * (double register synchronizer)
					 * of the interrupt asserted when
					 * a transaction is poisoned on
					 * the corresponding AXI port's
					 * read address channel. Bit 0
					 * corresponds to Port 0, and so
					 * on. Interrupt is cleared by
					 * register rd_poison_intr_clr,
					 * then value propagated to APB clock.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_A_AXI_15==1
					 * Programming Mode: Dynamic */
	} bits;
} reg_dramc_poisonstat_t;

/* Port Status Register */
typedef union {
	u32 val;
	struct {
	u32 rd_port_busy_0:1;           /* Indicates if there are outstanding
					 * reads for AXI port 0.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_PORT_0==1
					 * Programming Mode: Dynamic */
	u32 rsvd0:15;                   /* - */
	u32 wr_port_busy_0:1;           /* Indicates if there are outstanding
					 * writes for AXI port 0.
					 * Value After Reset: 0x0
					 * Exists: UMCTL2_PORT_0==1
					 * Programming Mode: Dynamic */
	u32 rsvd1:15;                   /* - */
	} bits;
} reg_dramc_pstat_t;

/* reg_dramc_t bank */
typedef struct tag_dramc {
	reg_dramc_mstr_t           mstr;       /* 0000 */
	reg_dramc_stat_t           stat;       /* 0004 */
	reg_dramc_mstr1_t          mstr1;      /* 0008 */
	u32                        res0[1];
	reg_dramc_mrctrl0_t        mrctrl0;    /* 0010 */
	reg_dramc_mrctrl1_t        mrctrl1;    /* 0014 */
	reg_dramc_mrstat_t         mrstat;     /* 0018 */
	u32                        res1[3];
	reg_dramc_mstr2_t          mstr2;      /* 0028 */
	u32                        res2[1];
	reg_dramc_pwrctl_t         pwrctl;     /* 0030 */
	reg_dramc_pwrtmg_t         pwrtmg;     /* 0034 */
	reg_dramc_hwlpctl_t        hwlpctl;    /* 0038 */
	u32                        res3[5];
	reg_dramc_rfshctl0_t       rfshctl0;   /* 0050 */
	reg_dramc_rfshctl1_t       rfshctl1;   /* 0054 */
	u32                        res4[2];
	reg_dramc_rfshctl3_t       rfshctl3;   /* 0060 */
	reg_dramc_rfshtmg_t        rfshtmg;    /* 0064 */
	u32                        res5[26];
	reg_dramc_init0_t          init0;      /* 00D0 */
	reg_dramc_init1_t          init1;      /* 00D4 */
	u32                        res6[1];
	reg_dramc_init3_t          init3;      /* 00DC */
	reg_dramc_init4_t          init4;      /* 00E0 */
	reg_dramc_init5_t          init5;      /* 00E4 */
	u32                        res7[2];
	reg_dramc_dimmctl_t        dimmctl;    /* 00F0 */
	reg_dramc_rankctl_t        rankctl;    /* 00F4 */
	u32                        res8[1];
	reg_dramc_chctl_t          chctl;      /* 00FC */
	reg_dramc_dramtmg0_t       dramtmg0;   /* 0100 */
	reg_dramc_dramtmg1_t       dramtmg1;   /* 0104 */
	reg_dramc_dramtmg2_t       dramtmg2;   /* 0108 */
	reg_dramc_dramtmg3_t       dramtmg3;   /* 010C */
	reg_dramc_dramtmg4_t       dramtmg4;   /* 0110 */
	reg_dramc_dramtmg5_t       dramtmg5;   /* 0114 */
	u32                        res9[2];
	reg_dramc_dramtmg8_t       dramtmg8;   /* 0120 */
	u32                        res10[6];
	reg_dramc_dramtmg15_t      dramtmg15;  /* 013C */
	u32                        res11[16];
	reg_dramc_zqctl0_t         zqctl0;     /* 0180 */
	reg_dramc_zqctl1_t         zqctl1;     /* 0184 */
	u32                        res12[2];
	reg_dramc_dfitmg0_t        dfitmg0;    /* 0190 */
	reg_dramc_dfitmg1_t        dfitmg1;    /* 0194 */
	reg_dramc_dfilpcfg0_t      dfilpcfg0;  /* 0198 */
	u32                        res13[1];
	reg_dramc_dfiupd0_t        dfiupd0;    /* 01A0 */
	reg_dramc_dfiupd1_t        dfiupd1;    /* 01A4 */
	reg_dramc_dfiupd2_t        dfiupd2;    /* 01A8 */
	u32                        res14[1];
	reg_dramc_dfimisc_t        dfimisc;    /* 01B0 */
	reg_dramc_dfitmg2_t        dfitmg2;    /* 01B4 */
	u32                        res15[1];
	reg_dramc_dfistat_t        dfistat;    /* 01BC */
	u32                        res16[1];
	reg_dramc_dfiphymstr_t     dfiphymstr; /* 01C4 */
	u32                        res17[14];
	reg_dramc_addrmap0_t       addrmap0;   /* 0200 */
	reg_dramc_addrmap1_t       addrmap1;   /* 0204 */
	reg_dramc_addrmap2_t       addrmap2;   /* 0208 */
	reg_dramc_addrmap3_t       addrmap3;   /* 020C */
	reg_dramc_addrmap4_t       addrmap4;   /* 0210 */
	reg_dramc_addrmap5_t       addrmap5;   /* 0214 */
	reg_dramc_addrmap6_t       addrmap6;   /* 0218 */
	u32                        res18[2];
	reg_dramc_addrmap9_t       addrmap9;   /* 0224 */
	reg_dramc_addrmap10_t      addrmap10;  /* 0228 */
	reg_dramc_addrmap11_t      addrmap11;  /* 022C */
	u32                        res19[4];
	reg_dramc_odtcfg_t         odtcfg;     /* 0240 */
	reg_dramc_odtmap_t         odtmap;     /* 0244 */
	u32                        res20[2];
	reg_dramc_sched_t          sched;      /* 0250 */
	reg_dramc_sched1_t         sched1;     /* 0254 */
	u32                        res21[1];
	reg_dramc_perfhpr1_t       perfhpr1;   /* 025C */
	u32                        res22[1];
	reg_dramc_perflpr1_t       perflpr1;   /* 0264 */
	u32                        res23[1];
	reg_dramc_perfwr1_t        perfwr1;    /* 026C */
	u32                        res24[36];
	reg_dramc_dbg0_t           dbg0;       /* 0300 */
	reg_dramc_dbg1_t           dbg1;       /* 0304 */
	reg_dramc_dbgcam_t         dbgcam;     /* 0308 */
	reg_dramc_dbgcmd_t         dbgcmd;     /* 030C */
	reg_dramc_dbgstat_t        dbgstat;    /* 0310 */
	u32                        res25[3];
	reg_dramc_swctl_t          swctl;      /* 0320 */
	reg_dramc_swstat_t         swstat;     /* 0324 */
	u32                        res26[17];
	reg_dramc_poisoncfg_t      poisoncfg;  /* 036C */
	reg_dramc_poisonstat_t     poisonstat; /* 0370 */
	u32                        res27[34];
	reg_dramc_pstat_t          pstat;      /* 03FC */
} reg_dramc_t;

#endif /* ___DRAMC___H___ */
