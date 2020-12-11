/* drampub.h */

#ifndef ___DRAMPUB___H___
#define ___DRAMPUB___H___

#define BASE_DRAMPUB                0x01218000

#define VA_DRAMPUB_RIDR              (0x00000000 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_PIR               (0x00000004 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_PGCR0             (0x00000008 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_PGCR1             (0x0000000C + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_PGSR0             (0x00000010 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_PGSR1             (0x00000014 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_PLLCR             (0x00000018 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_PTR0              (0x0000001C + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_PTR1              (0x00000020 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_PTR2              (0x00000024 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_PTR3              (0x00000028 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_PTR4              (0x0000002C + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_ACMDLR            (0x00000030 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_ACBDLR            (0x00000034 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_ACIOCR            (0x00000038 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DXCCR             (0x0000003C + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DSGCR             (0x00000040 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DCR               (0x00000044 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DTPR0             (0x00000048 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DTPR1             (0x0000004C + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DTPR2             (0x00000050 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_MR0               (0x00000054 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_MR1               (0x00000058 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_MR2               (0x0000005C + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_MR3               (0x00000060 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_ODTCR             (0x00000064 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DTCR              (0x00000068 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DTAR0             (0x0000006C + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DTAR1             (0x00000070 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DTAR2             (0x00000074 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DTAR3             (0x00000078 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DTDR0             (0x0000007C + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DTDR1             (0x00000080 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DTEDR0            (0x00000084 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DTEDR1            (0x00000088 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_VREF_AC           (0x00000090 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_VREF_DX01         (0x00000094 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_VREF_DX23         (0x00000098 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_FPGA_DBG          (0x000000BC + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DCUAR             (0x000000C0 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DCUDR             (0x000000C4 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DCURR             (0x000000C8 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DCULR             (0x000000CC + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DCUGCR            (0x000000D0 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DCUTPR            (0x000000D4 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DCUSR0            (0x000000D8 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DCUSR1            (0x000000DC + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_BISTRR            (0x00000100 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_BISTWCR           (0x00000104 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_BISTMSKR0         (0x00000108 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_BISTMSKR1         (0x0000010C + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_BISTMSKR2         (0x00000110 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_BISTLSR           (0x00000114 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_BISTAR0           (0x00000118 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_BISTAR1           (0x0000011C + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_BISTAR2           (0x00000120 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_BISTUDPR          (0x00000124 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_BISTGSR           (0x00000128 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_BISTWER           (0x0000012C + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_BISTBER0          (0x00000130 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_BISTBER1          (0x00000134 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_BISTBER2          (0x00000138 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_BISTBER3          (0x0000013C + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_BISTWCSR          (0x00000140 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_BISTFWR0          (0x00000144 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_BISTFWR1          (0x00000148 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_BISTFWR2          (0x0000014C + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_GPR0              (0x00000178 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_GPR1              (0x0000017C + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_ZQ0CR0       (0x00000180 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_ZQ1CR0       (0x00000190 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_ZQ2CR0       (0x000001A0 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_ZQ3CR0       (0x000001B0 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_ZQ0CR1       (0x00000184 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_ZQ1CR1       (0x00000194 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_ZQ2CR1       (0x000001A4 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_ZQ3CR1       (0x000001B4 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_ZQ0SR0       (0x00000188 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_ZQ1SR0       (0x00000198 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_ZQ2SR0       (0x000001A8 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_ZQ3SR0       (0x000001B8 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_ZQ0SR1       (0x0000018C + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_ZQ1SR1       (0x0000019C + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_ZQ2SR1       (0x000001AC + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_ZQ3SR1       (0x000001BC + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX0GCR       (0x000001C0 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX1GCR       (0x00000200 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX2GCR       (0x00000240 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX3GCR       (0x00000280 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX0GSR0      (0x000001C4 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX1GSR0      (0x00000204 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX2GSR0      (0x00000244 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX3GSR0      (0x00000284 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX0GSR1      (0x000001C8 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX1GSR1      (0x00000208 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX2GSR1      (0x00000248 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX3GSR1      (0x00000288 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX0BDLR0     (0x000001CC + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX1BDLR0     (0x0000020C + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX2BDLR0     (0x0000024C + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX3BDLR0     (0x0000028C + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX0BDLR1     (0x000001D0 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX1BDLR1     (0x00000210 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX2BDLR1     (0x00000250 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX3BDLR1     (0x00000290 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX0BDLR2     (0x000001D4 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX1BDLR2     (0x00000214 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX2BDLR2     (0x00000254 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX3BDLR2     (0x00000294 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX0BDLR3     (0x000001D8 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX1BDLR3     (0x00000218 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX2BDLR3     (0x00000258 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX3BDLR3     (0x00000298 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX0BDLR4     (0x000001DC + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX1BDLR4     (0x0000021C + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX2BDLR4     (0x0000025C + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX3BDLR4     (0x0000029C + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX0LCDLR0    (0x000001E0 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX1LCDLR0    (0x00000220 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX2LCDLR0    (0x00000260 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX3LCDLR0    (0x000002A0 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX0LCDLR1    (0x000001E4 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX1LCDLR1    (0x00000224 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX2LCDLR1    (0x00000264 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX3LCDLR1    (0x000002A4 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX0LCDLR2    (0x000001E8 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX1LCDLR2    (0x00000228 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX2LCDLR2    (0x00000268 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX3LCDLR2    (0x000002A8 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX0MDLR      (0x000001EC + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX1MDLR      (0x0000022C + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX2MDLR      (0x0000026C + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX3MDLR      (0x000002AC + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX0GTR       (0x000001F0 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX1GTR       (0x00000230 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX2GTR       (0x00000270 + BASE_DRAMPUB + VA_DRAMPUB)
#define VA_DRAMPUB_DX3GTR       (0x000002B0 + BASE_DRAMPUB + VA_DRAMPUB)

#define DATA_DRAMPUB_RIDR                0x00100210
#define DATA_DRAMPUB_PIR                 0x00000000
#define DATA_DRAMPUB_PGCR0               0xA8003E3F
#define DATA_DRAMPUB_PGCR1               0x0300C433
#define DATA_DRAMPUB_PGSR0               0x00000000
#define DATA_DRAMPUB_PGSR1               0x00000000
#define DATA_DRAMPUB_PLLCR               0x0001C000
#define DATA_DRAMPUB_PTR0                0x320FA010
#define DATA_DRAMPUB_PTR1                0x46500578
#define DATA_DRAMPUB_PTR2                0x00083DEF
#define DATA_DRAMPUB_PTR3                0x18082356
#define DATA_DRAMPUB_PTR4                0x0AAF4156
#define DATA_DRAMPUB_ACMDLR              0x00000000
#define DATA_DRAMPUB_ACBDLR              0x00000000
#define DATA_DRAMPUB_ACIOCR              0x33C03812
#define DATA_DRAMPUB_DXCCR               0x00001884
#define DATA_DRAMPUB_DSGCR               0xF000001F
#define DATA_DRAMPUB_DCR                 0x0000040B
#define DATA_DRAMPUB_DTPR0               0x61126633
#define DATA_DRAMPUB_DTPR1               0x1A841A42
#define DATA_DRAMPUB_DTPR2               0x1001A0C8
#define DATA_DRAMPUB_MR0                 0x00000A26
#define DATA_DRAMPUB_MR1                 0x00000000
#define DATA_DRAMPUB_MR2                 0x00000000
#define DATA_DRAMPUB_MR3                 0x00000000
#define DATA_DRAMPUB_ODTCR               0x84210000
#define DATA_DRAMPUB_DTCR                0x0F001007
#define DATA_DRAMPUB_DTAR0               0x10001000
#define DATA_DRAMPUB_DTAR1               0x10001008
#define DATA_DRAMPUB_DTAR2               0x10001010
#define DATA_DRAMPUB_DTAR3               0x10001018
#define DATA_DRAMPUB_DTDR0               0xDD22EE11
#define DATA_DRAMPUB_DTDR1               0x7788BB44
#define DATA_DRAMPUB_DTEDR0              0x00000000
#define DATA_DRAMPUB_DTEDR1              0x00000000
#define DATA_DRAMPUB_VREF_AC             0x00600060
#define DATA_DRAMPUB_VREF_DX01           0x00200060
#define DATA_DRAMPUB_VREF_DX23           0x00200060
#define DATA_DRAMPUB_FPGA_DBG            0x00000001
#define DATA_DRAMPUB_DCUAR               0x00000000
#define DATA_DRAMPUB_DCUDR               0x00000000
#define DATA_DRAMPUB_DCURR               0x00000000
#define DATA_DRAMPUB_DCULR               0x00000000
#define DATA_DRAMPUB_DCUGCR              0x00000000
#define DATA_DRAMPUB_DCUTPR              0x00000000
#define DATA_DRAMPUB_DCUSR0              0x00000000
#define DATA_DRAMPUB_DCUSR1              0x00000000
#define DATA_DRAMPUB_BISTRR              0x00000000
#define DATA_DRAMPUB_BISTWCR             0x00000020
#define DATA_DRAMPUB_BISTMSKR0           0x00000000
#define DATA_DRAMPUB_BISTMSKR1           0x00000000
#define DATA_DRAMPUB_BISTMSKR2           0x00000000
#define DATA_DRAMPUB_BISTLSR             0x1234ABCD
#define DATA_DRAMPUB_BISTAR0             0x00000000
#define DATA_DRAMPUB_BISTAR1             0x0000000C
#define DATA_DRAMPUB_BISTAR2             0x7FFFFFFF
#define DATA_DRAMPUB_BISTUDPR            0xFFFF0000
#define DATA_DRAMPUB_BISTGSR             0x00000000
#define DATA_DRAMPUB_BISTWER             0x00000000
#define DATA_DRAMPUB_BISTBER0            0x00000000
#define DATA_DRAMPUB_BISTBER1            0x00000000
#define DATA_DRAMPUB_BISTBER2            0x00000000
#define DATA_DRAMPUB_BISTBER3            0x00000000
#define DATA_DRAMPUB_BISTWCSR            0x00000000
#define DATA_DRAMPUB_BISTFWR0            0x00000000
#define DATA_DRAMPUB_BISTFWR1            0x00000000
#define DATA_DRAMPUB_BISTFWR2            0x00000000
#define DATA_DRAMPUB_GPR0                0x00000000
#define DATA_DRAMPUB_GPR1                0x00000000
#define DATA_DRAMPUB_ZQ0CR0         0x40000000
#define DATA_DRAMPUB_ZQ1CR0         0x40000000
#define DATA_DRAMPUB_ZQ2CR0         0x40000000
#define DATA_DRAMPUB_ZQ3CR0         0x40000000
#define DATA_DRAMPUB_ZQ0CR1         0x0000007B
#define DATA_DRAMPUB_ZQ1CR1         0x0000007B
#define DATA_DRAMPUB_ZQ2CR1         0x0000007B
#define DATA_DRAMPUB_ZQ3CR1         0x0000007B
#define DATA_DRAMPUB_ZQ0SR0         0x00000000
#define DATA_DRAMPUB_ZQ1SR0         0x00000000
#define DATA_DRAMPUB_ZQ2SR0         0x00000000
#define DATA_DRAMPUB_ZQ3SR0         0x00000000
#define DATA_DRAMPUB_ZQ0SR1         0x00000000
#define DATA_DRAMPUB_ZQ1SR1         0x00000000
#define DATA_DRAMPUB_ZQ2SR1         0x00000000
#define DATA_DRAMPUB_ZQ3SR1         0x00000000
#define DATA_DRAMPUB_DX0GCR         0x7C000E81
#define DATA_DRAMPUB_DX1GCR         0x7C000E81
#define DATA_DRAMPUB_DX2GCR         0x7C000E81
#define DATA_DRAMPUB_DX3GCR         0x7C000E81
#define DATA_DRAMPUB_DX0GSR0        0x00000000
#define DATA_DRAMPUB_DX1GSR0        0x00000000
#define DATA_DRAMPUB_DX2GSR0        0x00000000
#define DATA_DRAMPUB_DX3GSR0        0x00000000
#define DATA_DRAMPUB_DX0GSR1        0x00000000
#define DATA_DRAMPUB_DX1GSR1        0x00000000
#define DATA_DRAMPUB_DX2GSR1        0x00000000
#define DATA_DRAMPUB_DX3GSR1        0x00000000
#define DATA_DRAMPUB_DX0BDLR0       0x00000000
#define DATA_DRAMPUB_DX1BDLR0       0x00000000
#define DATA_DRAMPUB_DX2BDLR0       0x00000000
#define DATA_DRAMPUB_DX3BDLR0       0x00000000
#define DATA_DRAMPUB_DX0BDLR1       0x00000000
#define DATA_DRAMPUB_DX1BDLR1       0x00000000
#define DATA_DRAMPUB_DX2BDLR1       0x00000000
#define DATA_DRAMPUB_DX3BDLR1       0x00000000
#define DATA_DRAMPUB_DX0BDLR2       0x00000000
#define DATA_DRAMPUB_DX1BDLR2       0x00000000
#define DATA_DRAMPUB_DX2BDLR2       0x00000000
#define DATA_DRAMPUB_DX3BDLR2       0x00000000
#define DATA_DRAMPUB_DX0BDLR3       0x00000000
#define DATA_DRAMPUB_DX1BDLR3       0x00000000
#define DATA_DRAMPUB_DX2BDLR3       0x00000000
#define DATA_DRAMPUB_DX3BDLR3       0x00000000
#define DATA_DRAMPUB_DX0BDLR4       0x00000000
#define DATA_DRAMPUB_DX1BDLR4       0x00000000
#define DATA_DRAMPUB_DX2BDLR4       0x00000000
#define DATA_DRAMPUB_DX3BDLR4       0x00000000
#define DATA_DRAMPUB_DX0LCDLR0      0x00000000
#define DATA_DRAMPUB_DX1LCDLR0      0x00000000
#define DATA_DRAMPUB_DX2LCDLR0      0x00000000
#define DATA_DRAMPUB_DX3LCDLR0      0x00000000
#define DATA_DRAMPUB_DX0LCDLR1      0x00000000
#define DATA_DRAMPUB_DX1LCDLR1      0x00000000
#define DATA_DRAMPUB_DX2LCDLR1      0x00000000
#define DATA_DRAMPUB_DX3LCDLR1      0x00000000
#define DATA_DRAMPUB_DX0LCDLR2      0x00000000
#define DATA_DRAMPUB_DX1LCDLR2      0x00000000
#define DATA_DRAMPUB_DX2LCDLR2      0x00000000
#define DATA_DRAMPUB_DX3LCDLR2      0x00000000
#define DATA_DRAMPUB_DX0MDLR        0x00000000
#define DATA_DRAMPUB_DX1MDLR        0x00000000
#define DATA_DRAMPUB_DX2MDLR        0x00000000
#define DATA_DRAMPUB_DX3MDLR        0x00000000
#define DATA_DRAMPUB_DX0GTR         0x00055000
#define DATA_DRAMPUB_DX1GTR         0x00055000
#define DATA_DRAMPUB_DX2GTR         0x00055000
#define DATA_DRAMPUB_DX3GTR         0x00055000

/* Revision Identification Register */
typedef union {
	u32 val;
	struct {
	u32 pubmnr:4;           /* PUB Minor Revision: Indicates minor
				 * update of the PUB such as bug fixes.
				 * Normally no new features are included. */
	u32 pubmdr:4;           /* PUB Moderate Revision: Indicates moderate
				 * revision of the PUB such as addition
				 * of new features. Normally the new version
				 * is still compatible with previous versions. */
	u32 pubmjr:4;           /* PUB Major Revision: Indicates major
				 * revision of the PUB such addition of
				 * the features that make the new version
				 * not compatible with previous versions. */
	u32 phymnr:4;           /* PHY Minor Revision: Indicates minor
				 * update of the PHY such as bug fixes.
				 * Normally no new features are included. */
	u32 phymdr:4;           /* PHY Moderate Revision: Indicates moderate
				 * revision of the PHY such as addition
				 * of new features. Normally the new version
				 * is still compatible with previous versions */
	u32 phymjr:4;           /* PHY Major Revision: Indicates major
				 * revision of the PHY such addition of
				 * the features that make the new version
				 * not compatible with previous versions. */
	u32 udrid:8;            /* User-Defined Revision ID: General purpose
				 * revision identification set by the user. */
	} bits;
} reg_drampub_ridr_t;

/* PHY Initialization Register */
typedef union {
	u32 val;
	struct {
	u32 init:1;             /* Initialization Trigger: A write of '1'
				 * to this bit triggers the DDR system
				 * initialization, including PHY initialization,
				 * DRAM initialization, and PHY training.
				 * The exact initialization steps to be
				 * executed are specified in bits 1 to
				 * 15 of this register. A bit setting of
				 * 1 means the step will be executed as
				 * part of the initialization sequence,
				 * while a setting of ¡®0¡¯ means the step
				 * will be bypassed. The initialization
				 * trigger bit is self-clearing.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 zcal:1;             /* Impedance Calibration: Performs PHY
				 * impedance calibration. When set the
				 * impedance calibration will be performed
				 * in parallel with PHY initialization
				 * (PLL initialization + DDL calibration
				 * + PHY reset).
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 rsvd0:2;            /* - */
	u32 pllinit:1;          /* PLL Initialization: Executes the PLL
				 * initialization sequence which includes
				 * correct driving of PLL power-down, reset
				 * and gear shift pins, and then waiting
				 * for the PHY PLLs to lock.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dcal:1;             /* Digital Delay Line (DDL) Calibration:
				 * Performs PHY delay line calibration.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 phyrst:1;           /* PHY Reset: Resets the AC and DATX8 modules
				 * by asserting the AC/DATX8 reset pin.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dramrst:1;          /* DRAM Reset (DDR3 Only): Issues a reset
				 * to the DRAM (by driving the DRAM reset
				 * pin low) and wait 200us. This can be
				 * triggered in isolation or with the full
				 * DRAM initialization (DRAMINIT). For
				 * the later case, the reset is issued
				 * and 200us is waited before starting
				 * the full initialization sequence.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 draminit:1;         /* DRAM Initialization: Executes the DRAM
				 * initialization sequence.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 wl:1;               /* Write Leveling (DDR3 Only): Executes
				 * a PUB write leveling routine.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 qsgate:1;           /* Read DQS Gate Training: Executes a PUB
				 * training routine to determine the optimum
				 * position of the read data DQS strobe
				 * for maximum system timing margins.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 wladj:1;            /* Write Leveling Adjust (DDR3 Only): Executes
				 * a PUB training routine that re- adjusts
				 * the write latency used during write
				 * in case the write leveling routine changed
				 * the expected latency.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 rddskw:1;           /* Read Data Bit Deskew: Executes a PUB
				 * training routine to deskew the DQ bits
				 * during read.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 wrdskw:1;           /* Write Data Bit Deskew: Executes a PUB
				 * training routine to deskew the DQ bits
				 * during write.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 rdeye:1;            /* Read Data Eye Training: Executes a PUB
				 * training routine to maximize the read
				 * data eye.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 wreye:1;            /* Write Data Eye Training: Executes a
				 * PUB training routine to maximize the
				 * write data eye.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 icpc:1;             /* Initialization Complete Pin Configuration:
				 * Specifies how the DFI initialization
				 * complete output pin (dfi_init_complete)
				 * should be used to indicate the status
				 * of initialization. Valid value are:
				 * 0 = Asserted after PHY initialization
				 * (DLL locking and impedance calibration)
				 * is complete.
				 * 1 = Asserted after PHY initialization
				 * is complete and the triggered the PUB
				 * initialization (DRAM initialization,
				 * data training, or initialization trigger
				 * with no selected initialization) is complete.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 pllbyp:1;           /* PLL Bypass: A setting of 1 on this bit
				 * will put all PHY PLLs in bypass mode
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 ctldinit:1;         /* Controller DRAM Initialization: Indicates
				 * if set that DRAM initialization will
				 * be performed by the controller. Otherwise
				 * if not set it indicates that DRAM initialization
				 * will be performed using the built-in
				 * initialization sequence or using software
				 * through the configuration port.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 rsvd1:8;            /* - */
	u32 clrsr:1;            /* Clear Status Registers: A write of '1'
				 * to this bit will clear (reset to ¡®0¡¯)
				 * all status registers, including PGSR
				 * and DXnGSR. The clear status register
				 * bit is self- clearing.
				 * This bit is primarily for debug purposes
				 * and is typically not needed during normal
				 * functional operation. It can be used
				 * when PGSR.IDONE=1, to manually clear
				 * the PGSR status bits, although starting
				 * a new init process will automatically
				 * clear the PGSR status bits. Or it can
				 * be used to manually clear the DXnGSR
				 * status bits, although starting a new
				 * data training process will automatically
				 * clear the DXnGSR status bits.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 lockbyp:1;          /* PLL Lock Bypass: Bypasses or stops,
				 * if set, the waiting of PLLs to lock.
				 * PLL lock wait is automatically triggered
				 * after reset. PLL lock wait may be triggered
				 * manually using INIT and PLLLOCK bits
				 * of the PIR register. This bit is self-
				 * clearing.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dcalbyp:1;          /* Digital Delay Line (DDL) Calibration
				 * Bypass: Bypasses or stops, if set, DDL
				 * calibration that automatically triggers
				 * after reset. DDL calibration may be
				 * triggered manually using INIT and DCAL
				 * bits of the PIR register. This bit is
				 * self- clearing.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 zcalbyp:1;          /* Impedance Calibration Bypass: Bypasses
				 * or stops, if set, impedance calibration
				 * of all ZQ control blocks that automatically
				 * triggers after reset. Impedance calibration
				 * may be triggered manually using INIT
				 * and ZCAL bits of the PIR register. This
				 * bit is self-clearing.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 initbyp:1;          /* Initialization Bypass: Bypasses or stops,
				 * if set, all initialization routines
				 * currently running, including PHY initialization,
				 * DRAM initialization, and PHY training.
				 * Initialization may be triggered manually
				 * using INIT and the other relevant bits
				 * of the PIR register. This bit is self-c
				 * learing.
				 * [0x0]: Disabled[0x1]: Enabled */
	} bits;
} reg_drampub_pir_t;

#define DRAMPUB_PIR_INIT_0      0x0
#define DRAMPUB_PIR_ZCAL_0      0x0
#define DRAMPUB_PIR_PLLINIT_0   0x0
#define DRAMPUB_PIR_DCAL_0      0x0
#define DRAMPUB_PIR_PHYRST_0    0x0
#define DRAMPUB_PIR_DRAMRST_0   0x0
#define DRAMPUB_PIR_DRAMINIT_0  0x0
#define DRAMPUB_PIR_WL_0        0x0
#define DRAMPUB_PIR_QSGATE_0    0x0
#define DRAMPUB_PIR_WLADJ_0     0x0
#define DRAMPUB_PIR_RDDSKW_0    0x0
#define DRAMPUB_PIR_WRDSKW_0    0x0
#define DRAMPUB_PIR_RDEYE_0     0x0
#define DRAMPUB_PIR_WREYE_0     0x0
#define DRAMPUB_PIR_ICPC_0      0x0
#define DRAMPUB_PIR_PLLBYP_0    0x0
#define DRAMPUB_PIR_CTLDINIT_0  0x0
#define DRAMPUB_PIR_CLRSR_0     0x0
#define DRAMPUB_PIR_LOCKBYP_0   0x0
#define DRAMPUB_PIR_DCALBYP_0   0x0
#define DRAMPUB_PIR_ZCALBYP_0   0x0
#define DRAMPUB_PIR_INITBYP_0   0x0

/* PHY General Configuration Registers 0 */
typedef union {
	u32 val;
	struct {
	u32 wllvt:1;            /* Write Leveling LCDL Delay VT Compensation:
				 * Enables, if set, the VT drift compensation
				 * of the write leveling LCDL.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 wdlvt:1;            /* Write DQ LCDL Delay VT Compensation:
				 * Enables, if set the VT drift compensation
				 * of the write DQ LCDL.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 rdlvt:1;            /* Read DQS LCDL Delay VT Compensation:
				 * Enables, if set the VT drift compensation
				 * of the read DQS LCDL.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 rglvt:1;            /* Read DQS Gating LCDL Delay VT Compensation:
				 * Enables, if set the VT drift compensation
				 * of the read DQS gating LCDL.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 wdbvt:1;            /* Write Data BDL VT Compensation: Enables,
				 * if set the VT drift compensation of
				 * the write data bit delay lines.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 rdbvt:1;            /* Read Data BDL VT Compensation: Enables,
				 * if set the VT drift compensation of
				 * the read data bit delay lines.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dltmode:1;          /* Delay Line Test Mode: Selects, if set,
				 * the delay line oscillator test mode.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dltst:1;            /* Delay Line Test Start: A write of '1'
				 * to this bit will trigger delay line
				 * oscillator mode period measurement.
				 * This bit is not self clearing and needs
				 * to be reset to '0' before the measurement
				 * can be re-triggered.
				 * [0x0]: Stop[0x1]: Enabled */
	u32 oscen:1;            /* Oscillator Enable: Enables, if set,
				 * the delay line oscillation.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 oscdiv:3;           /* Oscillator Mode Division: Specifies
				 * the factor by which the delay line oscillator
				 * mode output is divided down before it
				 * is output on the delay line digital
				 * test output pin dl_dto. Valid values are:
				 * [0x0]: Divide by 1
				 * [0x1]: Divide by 256
				 * [0x2]: Divide by 512
				 * [0x3]: Divide by 1024
				 * [0x4]: Divide by 2048
				 * [0x5]: Divide by 4096
				 * [0x6]: Divide by 8192
				 * [0x7]: Divide by 65536 */
	u32 oscwdl:2;           /* Oscillator Mode Write-Leveling Delay
				 * Line Select: Selects which of the two
				 * write leveling LCDLs is active. The
				 * delay select value of the inactive LCDL
				 * is set to zero while the delay select
				 * value of the active LCDL can be varied
				 * by the input write leveling delay select
				 * pin. Valid values are:
				 * [0x0]: No WL LCDL is active
				 * [0x1]: DDR WL LCDL is active
				 * [0x2]: SDR WL LCDL is active
				 * [0x3]: Both LCDLs are active */
	u32 zcalbyp:5;          /* Digital Test Output Select: Selects
				 * the PHY digital test output that should
				 * be driven onto PHY digital test output
				 * (phy_dto) pin: Valid values are:
				 * [0x00]: DATX8 0 PLL digital test output
				 * [0x01]: DATX8 1 PLL digital test output
				 * [0x02]: DATX8 2 PLL digital test output
				 * [0x03]: DATX8 3 PLL digital test output
				 * [0x04]: DATX8 4 PLL digital test output
				 * [0x05]: DATX8 5 PLL digital test output
				 * [0x06]: DATX8 6 PLL digital test output
				 * [0x07]: DATX8 7 PLL digital test output
				 * [0x08]: DATX8 8 PLL digital test output
				 * [0x09]: AC PLL digital test output
				 * [0x0A]: Reserved
				 * [0x0B]: Reserved
				 * [0x0C]: Reserved
				 * [0x0D]: Reserved
				 * [0x0E]: Reserved
				 * [0x0F]: Reserved
				 * [0x10]: DATX8 0 delay line digital test
				 * output
				 * [0x11]: DATX8 1 delay line digital test
				 * output
				 * [0x12]: DATX8 2 delay line digital test
				 * output
				 * [0x13]: DATX8 3 delay line digital test
				 * output
				 * [0x14]: DATX8 4 delay line digital test
				 * output
				 * [0x15]: DATX8 5 delay line digital test
				 * output
				 * [0x16]: DATX8 6 delay line digital test
				 * output
				 * [0x17]: DATX8 7 delay line digital test
				 * output
				 * [0x18]: DATX8 8 delay line digital test
				 * output
				 * [0x19]: AC delay line digital test output
				 * [0x1A]: Reserved
				 * [0x1B]: Reserved
				 * [0x1C]: Reserved
				 * [0x1D]: Reserved
				 * [0x1E]: Reserved
				 * [0x1F]: Reserved */
	u32 rsvd0:7;            /* - */
	u32 ck0en:2;            /* CK0 Enable: Controls whether the CK0
				 * going to the SDRAM is enabled (toggling)
				 * or disabled (static value) and whether
				 * the CK0 is inverted. Valid values for
				 * the two bits are:
				 * [0x0]: CK disabled (Driven to constant 0)
				 * [0x1]: CK toggling with inverted polarity
				 * [0x2]: CK toggling with normal polarity
				 * (This should be the default setting)
				 * [0x3]: CK disabled (Driven to constant 1) */
	u32 ck1en:2;            /* CK1 Enable: Controls whether the CK1
				 * going to the SDRAM is enabled (toggling)
				 * or disabled (static value) and whether
				 * the CK1 is inverted. Valid values for
				 * the two bits are:
				 * [0x0]: CK disabled (Driven to constant 0)
				 * [0x1]: CK toggling with inverted polarity
				 * [0x2]: CK toggling with normal polarity
				 * (This should be the default setting)
				 * [0x3]: CK disabled (Driven to constant 1) */
	u32 ck2en:2;            /* CK2 Enable: Controls whether the CK2
				 * going to the SDRAM is enabled (toggling)
				 * or disabled (static value) and whether
				 * the CK2 is inverted. Valid values for
				 * the two bits are:
				 * [0x0]: CK disabled (Driven to constant 0)
				 * [0x1]: CK toggling with inverted polarity
				 * [0x2]: CK toggling with normal polarity
				 * (This should be the default setting)
				 * [0x3]: CK disabled (Driven to constant 1) */
	} bits;
} reg_drampub_pgcr0_t;

#define DRAMPUB_PGCR0_WLLVT_0   0x0
#define DRAMPUB_PGCR0_WDLVT_0   0x0
#define DRAMPUB_PGCR0_RDLVT_0   0x0
#define DRAMPUB_PGCR0_RGLVT_0   0x0
#define DRAMPUB_PGCR0_WDBVT_0   0x0
#define DRAMPUB_PGCR0_RDBVT_0   0x0
#define DRAMPUB_PGCR0_DLTMODE_0 0x0
#define DRAMPUB_PGCR0_DLTST_0   0x0
#define DRAMPUB_PGCR0_OSCEN_0   0x0
#define DRAMPUB_PGCR0_OSCDIV_0  0x0
#define DRAMPUB_PGCR0_OSCDIV_1  0x1
#define DRAMPUB_PGCR0_OSCDIV_2  0x2
#define DRAMPUB_PGCR0_OSCDIV_3  0x3
#define DRAMPUB_PGCR0_OSCDIV_4  0x4
#define DRAMPUB_PGCR0_OSCDIV_5  0x5
#define DRAMPUB_PGCR0_OSCDIV_6  0x6
#define DRAMPUB_PGCR0_OSCDIV_7  0x7
#define DRAMPUB_PGCR0_OSCWDL_0  0x0
#define DRAMPUB_PGCR0_OSCWDL_1  0x1
#define DRAMPUB_PGCR0_OSCWDL_2  0x2
#define DRAMPUB_PGCR0_OSCWDL_3  0x3
#define DRAMPUB_PGCR0_ZCALBYP_0         0x0
#define DRAMPUB_PGCR0_ZCALBYP_1         0x1
#define DRAMPUB_PGCR0_ZCALBYP_2         0x2
#define DRAMPUB_PGCR0_ZCALBYP_3         0x3
#define DRAMPUB_PGCR0_ZCALBYP_4         0x4
#define DRAMPUB_PGCR0_ZCALBYP_5         0x5
#define DRAMPUB_PGCR0_ZCALBYP_6         0x6
#define DRAMPUB_PGCR0_ZCALBYP_7         0x7
#define DRAMPUB_PGCR0_ZCALBYP_8         0x8
#define DRAMPUB_PGCR0_ZCALBYP_9         0x9
#define DRAMPUB_PGCR0_ZCALBYP_A         0xA
#define DRAMPUB_PGCR0_ZCALBYP_B         0xB
#define DRAMPUB_PGCR0_ZCALBYP_C         0xC
#define DRAMPUB_PGCR0_ZCALBYP_D         0xD
#define DRAMPUB_PGCR0_ZCALBYP_E         0xE
#define DRAMPUB_PGCR0_ZCALBYP_F         0xF
#define DRAMPUB_PGCR0_ZCALBYP_10        0x10
#define DRAMPUB_PGCR0_ZCALBYP_11        0x11
#define DRAMPUB_PGCR0_ZCALBYP_12        0x12
#define DRAMPUB_PGCR0_ZCALBYP_13        0x13
#define DRAMPUB_PGCR0_ZCALBYP_14        0x14
#define DRAMPUB_PGCR0_ZCALBYP_15        0x15
#define DRAMPUB_PGCR0_ZCALBYP_16        0x16
#define DRAMPUB_PGCR0_ZCALBYP_17        0x17
#define DRAMPUB_PGCR0_ZCALBYP_18        0x18
#define DRAMPUB_PGCR0_ZCALBYP_19        0x19
#define DRAMPUB_PGCR0_ZCALBYP_1A        0x1A
#define DRAMPUB_PGCR0_ZCALBYP_1B        0x1B
#define DRAMPUB_PGCR0_ZCALBYP_1C        0x1C
#define DRAMPUB_PGCR0_ZCALBYP_1D        0x1D
#define DRAMPUB_PGCR0_ZCALBYP_1E        0x1E
#define DRAMPUB_PGCR0_ZCALBYP_1F        0x1F
#define DRAMPUB_PGCR0_CK0EN_0   0x0
#define DRAMPUB_PGCR0_CK0EN_1   0x1
#define DRAMPUB_PGCR0_CK0EN_2   0x2
#define DRAMPUB_PGCR0_CK0EN_3   0x3
#define DRAMPUB_PGCR0_CK1EN_0   0x0
#define DRAMPUB_PGCR0_CK1EN_1   0x1
#define DRAMPUB_PGCR0_CK1EN_2   0x2
#define DRAMPUB_PGCR0_CK1EN_3   0x3
#define DRAMPUB_PGCR0_CK2EN_0   0x0
#define DRAMPUB_PGCR0_CK2EN_1   0x1
#define DRAMPUB_PGCR0_CK2EN_2   0x2
#define DRAMPUB_PGCR0_CK2EN_3   0x3

/* PHY General Configuration Registers 1 */
typedef union {
	u32 val;
	struct {
	u32 pddisdx:1;          /* Power Down Disabled Byte: Indicates
				 * if set that the DLL and I/Os of a disabled
				 * byte should be powered down.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 wlfull:1;           /* Write Level Full: Indicates, if set,
				 * that a full write leveling algorithm
				 * should be executed. Full write leveling
				 * includes setting up the SDRAM for writing
				 * leveling as well as iterating through
				 * all the ranks. Refer to ¡°Write Leveling¡±
				 * on page 118 for details.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 wlstep:1;           /* Write Leveling Step: Specifies the number
				 * of delay step-size increments during
				 * each step of write leveling. Valid values
				 * are:
				 * [0x0]: 32 step sizes
				 * [0x1]: 1 step size */
	u32 wluncrt:1;          /* Write Level Uncertainty Region: Indicates,
				 * if set, that an extended algorithm that
				 * write levels through the setup/hold
				 * uncertainty region should be executed.
				 * NOTE: This algorithm is not implemented
				 * in the current version of the PUB.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 wlrank:2;           /* Write Leveling Rank: Selects the number
				 * of ranks to be write level. valid values are:
				 * [0x0]: Write level 1 rank (Rank 0)
				 * [0x1]: Write level 2 ranks (Ranks 0, 1)
				 * [0x2]: Write level 3 ranks (Ranks 0, 1, 2)
				 * [0x3]: Write level 4 ranks (Ranks 0,1,2,3) */
	u32 wlselt:1;           /* Write Leveling Select Type: Selects
				 * the encoding type for the write leveling
				 * select signal depending on the desired
				 * setup/hold margins for the internal
				 * pipelines. Refer to the DDR PHY Databook
				 * for details of how the select type is
				 * used. Valid values are:
				 * [0x0]: Type 1: Setup margin of 90 degrees
				 * and hold margin of 90 degrees
				 * [0x1]: Type 2: Setup margin of 135 degrees
				 * and hold margin of 45 degrees */
	u32 ioddrm:2;           /* I/O DDR Mode (D3F I/O Only): Selects
				 * the DDR mode for the I/Os. These bits
				 * connect to bits [2:1] of the IOM pin
				 * of the SSTL I/O. For more information,
				 * refer to the SSTL I/O chapter in the
				 * DWC DDR PHY Databook.
				 * (Note: there is used D3R ) */
	u32 mdlen:1;            /* Master Delay Line Enable: Enables, if
				 * set, the AC master delay line calibration
				 * to perform subsequent period measurements
				 * following the initial period measurements
				 * that are performed after reset or on
				 * when calibration is manually triggered.
				 * These additional measurements are accumulated
				 * and filtered as long as this bit remains
				 * high.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 lpfen:1;            /* Low-Pass Filter Enable: Enables, if
				 * set, the low pass filtering of MDL period
				 * measurements.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 lpfdepth:2;         /* Low-Pass Filter Depth: Specifies the
				 * number of measurements over which MDL
				 * period measurements are filtered. This
				 * determines the time constant of the
				 * low pass filter. Valid values are:
				 * [0x0]: 2
				 * [0x1]: 4
				 * [0x2]: 8
				 * [0x3]: 16 */
	u32 fdepth:2;           /* Filter Depth: Specifies the number of
				 * measurements over which all AC and DATX8
				 * initial period measurements, that happen
				 * after reset or when calibration is manually
				 * triggered, are averaged. Valid values are:
				 * [0x0]: 2
				 * [0x1]: 4
				 * [0x2]: 8
				 * [0x3]: 16 */
	u32 dldlmt:8;           /* Delay Line VT Drift Limit: Specifies
				 * the minimum change in the delay line
				 * VT drift in one direction which should
				 * result in the assertion of the delay
				 * line VT drift status signal (vt_drift).
				 * The limit is specified in terms of delay
				 * select values. A value of 0 disables
				 * the assertion of delay line VT drift
				 * status signal. */
	u32 zcksel:2;           /* Impedance Clock Divider Select: Selects
				 * the divide ratio for the clock used
				 * by the impedance control logic relative
				 * to the clock used by the memory controller
				 * and SDRAM. Valid values are:
				 * [0x0]: Divide by 2
				 * [0x1]: Divide by 8
				 * [0x2]: Divide by 32
				 * [0x3]: Divide by 64 */
	u32 phyhrst:1;          /* PHY High-Speed Reset: A write of ¡®0¡¯
				 * to this bit resets the AC and DATX8
				 * macros without resetting PUB RTL logic.
				 * This bit is not self-clearing and a
				 * ¡®1¡¯ must be written to de-assert the reset.
				 * [0x0]: Reset[0x1]: de-assert */
	u32 inhvt:1;            /* VT Calculation Inhibit: Inhibits calculation
				 * of the next VT compensated delay line
				 * values. A value of 1 will inhibit the
				 * VT calculation. This bit should be set
				 * to 1 during writes to the delay line
				 * registers.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 oscwdl:1;           /* I/O Loop-Back Select: Selects where
				 * inside the I/O the loop-back of signals
				 * happens. Valid values are:
				 * [0x0]: Loopback is after output buffer;
				 * output enable must be asserted
				 * [0x1]: Loopback is before output buffer;
				 * output enable is don¡¯t care */
	u32 lbdqss:1;           /* Loopback DQS Shift: Selects how the
				 * read DQS is shifted during loopback
				 * to ensure that the read DQS is centered
				 * into the read data eye. Valid values are:
				 * [0x0]: PUB sets the read DQS LCDL to
				 * 0; DQS is already shifted 90 degrees
				 * by write path
				 * [0x1]: The read DQS shift is set manually
				 * through software */
	u32 lbgdqs:2;           /* Loopback DQS Gating: Selects the DQS
				 * gating mode that should be used when the
				 * PHY is in loopback mode, including BIST
				 * loopback mode. Valid values are:
				 * [0x0]:DQS gate is always on
				 * [0x1]:DQS gate training will be triggered
				 * on the PUB
				 * [0x2]:DQS gate is set manually using software
				 * [0x3]: Reserved */
	u32 lbmode:1;           /* Loopback Mode: Indicates, if set, that
				 * the PHY/PUB is in loopback mode.
				 * [0x0]: Disabled[0x1]: Enabled */
	} bits;
} reg_drampub_pgcr1_t;

#define DRAMPUB_PGCR1_PDDISDX_0         0x0
#define DRAMPUB_PGCR1_WLFULL_0          0x0
#define DRAMPUB_PGCR1_WLSTEP_0          0x0
#define DRAMPUB_PGCR1_WLSTEP_1          0x1
#define DRAMPUB_PGCR1_WLUNCRT_0         0x0
#define DRAMPUB_PGCR1_WLRANK_0          0x0
#define DRAMPUB_PGCR1_WLRANK_1          0x1
#define DRAMPUB_PGCR1_WLRANK_2          0x2
#define DRAMPUB_PGCR1_WLRANK_3          0x3
#define DRAMPUB_PGCR1_WLSELT_0          0x0
#define DRAMPUB_PGCR1_WLSELT_1          0x1
#define DRAMPUB_PGCR1_MDLEN_0           0x0
#define DRAMPUB_PGCR1_LPFEN_0           0x0
#define DRAMPUB_PGCR1_LPFDEPTH_0        0x0
#define DRAMPUB_PGCR1_LPFDEPTH_1        0x1
#define DRAMPUB_PGCR1_LPFDEPTH_2        0x2
#define DRAMPUB_PGCR1_LPFDEPTH_3        0x3
#define DRAMPUB_PGCR1_FDEPTH_0          0x0
#define DRAMPUB_PGCR1_FDEPTH_1          0x1
#define DRAMPUB_PGCR1_FDEPTH_2          0x2
#define DRAMPUB_PGCR1_FDEPTH_3          0x3
#define DRAMPUB_PGCR1_ZCKSEL_0          0x0
#define DRAMPUB_PGCR1_ZCKSEL_1          0x1
#define DRAMPUB_PGCR1_ZCKSEL_2          0x2
#define DRAMPUB_PGCR1_ZCKSEL_3          0x3
#define DRAMPUB_PGCR1_PHYHRST_0         0x0
#define DRAMPUB_PGCR1_INHVT_0           0x0
#define DRAMPUB_PGCR1_OSCWDL_0          0x0
#define DRAMPUB_PGCR1_OSCWDL_1          0x1
#define DRAMPUB_PGCR1_LBDQSS_0          0x0
#define DRAMPUB_PGCR1_LBDQSS_1          0x1
#define DRAMPUB_PGCR1_LBGDQS_0          0x0
#define DRAMPUB_PGCR1_LBGDQS_1          0x1
#define DRAMPUB_PGCR1_LBGDQS_2          0x2
#define DRAMPUB_PGCR1_LBGDQS_3          0x3
#define DRAMPUB_PGCR1_LBMODE_0          0x0

/* PHY General Status Registers 0 */
typedef union {
	u32 val;
	struct {
	u32 idone:1;            /* Initialization Done: Indicates if set
				 * that the DDR system initialization has
				 * completed. This bit is set after all
				 * the selected initialization routines
				 * in PIR register have completed. */
	u32 pldone:1;           /* PLL Lock Done: Indicates if set that
				 * PLL locking has completed. */
	u32 dcdone:1;           /* Digital Delay Line (DDL) Calibration
				 * Done: Indicates if set that DDL calibration
				 * has completed. */
	u32 zcdone:1;           /* Impedance Calibration Done: Indicates
				 * if set that impedance calibration has
				 * completed. */
	u32 didone:1;           /* DRAM Initialization Done: Indicates
				 * if set that DRAM initialization has
				 * completed. */
	u32 wldone:1;           /* Write Leveling Done: Indicates if set
				 * that write leveling has completed. */
	u32 qsgdone:1;          /* DQS Gate Training Done: Indicates if
				 * set that DQS gate training has completed. */
	u32 wladone:1;          /* Write Leveling Adjustment Done: Indicates
				 * if set that write leveling adjustment
				 * has completed. */
	u32 rddone:1;           /* Read Bit Deskew Done: Indicates if set
				 * that read bit deskew has completed. */
	u32 wddone:1;           /* Write Bit Deskew Done: Indicates if
				 * set that write bit deskew has completed. */
	u32 redone:1;           /* Read Eye Training Done: Indicates if
				 * set that read eye training has completed. */
	u32 wedone:1;           /* Write Eye Training Done: Indicates if
				 * set that write eye training has completed. */
	u32 rsvd0:4;            /* - */
	u32 ierr:1;             /* Initialization Error: Indicates if set
				 * that there is an error in the DDR system
				 * initialization. This bit is set if any
				 * of the selected initialization routines
				 * in PIR register has an error. */
	u32 plerr:1;            /* PLL Lock Error: Indicates if set that
				 * there is an error in PLL locking. */
	u32 dcerr:1;            /* Digital Delay Line (DDL) Calibration
				 * Error: Indicates if set that there is
				 * an error in
				 * DDL calibration. */
	u32 zcerr:1;            /* Impedance Calibration Error: Indicates
				 * if set that there is an error in impedance
				 * calibration. */
	u32 dierr:1;            /* DRAM Initialization Error: Indicates
				 * if set that there is an error in DRAM
				 * initialization. */
	u32 wlerr:1;            /* Write Leveling Error: Indicates if set
				 * that there is an error in write leveling. */
	u32 qsgerr:1;           /* DQS Gate Training Error: Indicates if
				 * set that there is an error in DQS gate
				 * training. */
	u32 wlaerr:1;           /* Write Leveling Adjustment Error: Indicates
				 * if set that there is an error in write
				 * leveling adjustment. */
	u32 rderr:1;            /* Read Bit Deskew Error: Indicates if
				 * set that there is an error in read bit
				 * deskew. */
	u32 wderr:1;            /* Write Bit Deskew Error: Indicates if
				 * set that there is an error in write
				 * bit deskew. */
	u32 reerr:1;            /* Read Eye Training Error: Indicates if
				 * set that there is an error in read eye
				 * training. */
	u32 weerr:1;            /* Write Eye Training Error: Indicates
				 * if set that there is an error in write
				 * eye training. */
	u32 dterr:3;            /* Data Training Error Status: Specifies
				 * the error status code for data training. */
	u32 aplock:1;           /* AC PLL Lock: Indicates, if set, that
				 * AC PLL has locked. This is a direct
				 * status of the
				 * AC PLL lock pin. */
	} bits;
} reg_drampub_pgsr0_t;

/* PHY General Status Registers 1 */
typedef union {
	u32 val;
	struct {
	u32 dltdone:1;          /* Delay Line Test Done: Indicates, if
				 * set, that the PHY control block has
				 * finished doing period measurement of
				 * the AC delay line digital test output. */
	u32 dltcode:24;         /* Delay Line Test Code: Returns the code
				 * measured by the PHY control block that
				 * corresponds to the period of the AC
				 * delay line digital test output. */
	u32 rsvd0:7;            /* - */
	} bits;
} reg_drampub_pgsr1_t;

/* PLL Control Register */
typedef union {
	u32 val;
	struct {
	u32 dtc:2;              /* Digital Test Control: Selects various
				 * PLL digital test signals and other test
				 * mode signals to be brought out via bit
				 * [1] of the PLL digital test output (pll_dto[1]).
				 * Valid values are:
				 * [0x0]: ¡®0¡¯ (Test output is disabled)
				 * [0x1]: PLL x1 clock (X1)
				 * [0x2]: PLL reference (input) clock (REF_CLK)
				 * [0x3]: PLL feedback clock (FB_X1) */
	u32 atc:4;              /* Analog Test Control: Selects various
				 * PLL analog test signals to be brought out via
				 * PLL analog test output pin (pll_ato).
				 * Valid values are:
				 * [0x0]: Reserved
				 * [0x1]: vdd_ckin
				 * [0x2]: vrfbf
				 * [0x3]: vdd_cko
				 * [0x4]: vp_cp
				 * [0x5]: vpfil(vp)
				 * [0x6]: Reserved
				 * [0x7]: gd
				 * [0x8]: vcntrl_atb
				 * [0x9]: vref_atb
				 * [0xA]: vpsf_atb
				 * [0xB]: Reserved
				 * [0xC]: Reserved
				 * [0xD]: Reserved
				 * [0xE]: Reserved
				 * [0xF]: Reserved */
	u32 atoen:4;            /* Analog Test Enable (ATOEN): Selects
				 * the analog test signal that should be
				 * driven on the analog test output pin.
				 * Otherwise the analog test output is
				 * tri-stated. This allows analog test
				 * output pins from multiple PLLs to be
				 * connected together. Valid values are:
				 * [0x0]: All PLL analog test signals are
				 * tri-stated
				 * [0x1]: AC PLL analog test signal is
				 * driven out
				 * [0x2]: DATX8 0 PLL analog test signal
				 * is driven out
				 * [0x3]: DATX8 1 PLL analog test signal
				 * is driven out
				 * [0x4]: DATX8 2 PLL analog test signal
				 * is driven out
				 * [0x5]: DATX8 3 PLL analog test signal
				 * is driven out
				 * [0x6]: DATX8 4 PLL analog test signal
				 * is driven out
				 * [0x7]: DATX8 5 PLL analog test signal
				 * is driven out
				 * [0x8]: DATX8 6 PLL analog test signal
				 * is driven out
				 * [0x9]: DATX8 7 PLL analog test signal
				 * is driven out
				 * [0xA]: DATX8 8 PLL analog test signal
				 * is driven out
				 * [0xB]: Reserved
				 * [0xC]: Reserved
				 * [0xD]: Reserved
				 * [0xE]: Reserved
				 * [0xF]: Reserved */
	u32 gshift:1;           /* Gear Shift: Enables, if set, rapid locking
				 * mode.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 cpic:2;             /* Charge Pump Integrating Current Control */
	u32 cppc:4;             /* Charge Pump Proportional Current Control */
	u32 qpmode:1;           /* PLL Quadrature Phase Mode: Enables,
				 * if set, the quadrature phase clock outputs.
				 * This mode is not used in this version
				 * of the PHY.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 frqsel:2;           /* PLL Frequency Select: Selects the operating
				 * range of the PLL.
				 * Valid values for PHYs that don¡¯t go
				 * up to 2133 Mbps are:
				 * [0x0]: PLL reference clock (ctl_clk/REF_CLK)
				 * ranges from 250MHz to 400MHz
				 * [0x1]: PLL reference clock (ctl_clk/REF_CLK)
				 * ranges from 166MHz to 300MHz
				 * [0x2]: Reserved
				 * [0x3]: Reserved */
	u32 rsvd0:9;            /* - */
	u32 pllpd:1;            /* PLL Power Down: Puts the PLLs in power
				 * down mode by driving the PLL power down
				 * pin. This bit is not self-clearing and
				 * a ¡®0¡¯ must be written to de-assert
				 * the power-down.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 pllrst:1;           /* PLL Rest: Resets the PLLs by driving
				 * the PLL reset pin. This bit is not self-clearing
				 * and a ¡®0¡¯ must be written to de-assert
				 * the reset.
				 * [0x0]: de-assert[0x1]: reset */
	u32 byp:1;              /* PLL Bypass: Bypasses the PLL if set to 1.
				 * [0x0]: Disabled[0x1]: Enabled */
	} bits;
} reg_drampub_pllcr_t;

#define DRAMPUB_PLLCR_DTC_0     0x0
#define DRAMPUB_PLLCR_DTC_1     0x1
#define DRAMPUB_PLLCR_DTC_2     0x2
#define DRAMPUB_PLLCR_DTC_3     0x3
#define DRAMPUB_PLLCR_ATC_0     0x0
#define DRAMPUB_PLLCR_ATC_1     0x1
#define DRAMPUB_PLLCR_ATC_2     0x2
#define DRAMPUB_PLLCR_ATC_3     0x3
#define DRAMPUB_PLLCR_ATC_4     0x4
#define DRAMPUB_PLLCR_ATC_5     0x5
#define DRAMPUB_PLLCR_ATC_6     0x6
#define DRAMPUB_PLLCR_ATC_7     0x7
#define DRAMPUB_PLLCR_ATC_8     0x8
#define DRAMPUB_PLLCR_ATC_9     0x9
#define DRAMPUB_PLLCR_ATC_A     0xA
#define DRAMPUB_PLLCR_ATC_B     0xB
#define DRAMPUB_PLLCR_ATC_C     0xC
#define DRAMPUB_PLLCR_ATC_D     0xD
#define DRAMPUB_PLLCR_ATC_E     0xE
#define DRAMPUB_PLLCR_ATC_F     0xF
#define DRAMPUB_PLLCR_ATOEN_0   0x0
#define DRAMPUB_PLLCR_ATOEN_1   0x1
#define DRAMPUB_PLLCR_ATOEN_2   0x2
#define DRAMPUB_PLLCR_ATOEN_3   0x3
#define DRAMPUB_PLLCR_ATOEN_4   0x4
#define DRAMPUB_PLLCR_ATOEN_5   0x5
#define DRAMPUB_PLLCR_ATOEN_6   0x6
#define DRAMPUB_PLLCR_ATOEN_7   0x7
#define DRAMPUB_PLLCR_ATOEN_8   0x8
#define DRAMPUB_PLLCR_ATOEN_9   0x9
#define DRAMPUB_PLLCR_ATOEN_A   0xA
#define DRAMPUB_PLLCR_ATOEN_B   0xB
#define DRAMPUB_PLLCR_ATOEN_C   0xC
#define DRAMPUB_PLLCR_ATOEN_D   0xD
#define DRAMPUB_PLLCR_ATOEN_E   0xE
#define DRAMPUB_PLLCR_ATOEN_F   0xF
#define DRAMPUB_PLLCR_GSHIFT_0  0x0
#define DRAMPUB_PLLCR_QPMODE_0  0x0
#define DRAMPUB_PLLCR_FRQSEL_0  0x0
#define DRAMPUB_PLLCR_FRQSEL_1  0x1
#define DRAMPUB_PLLCR_FRQSEL_2  0x2
#define DRAMPUB_PLLCR_FRQSEL_3  0x3
#define DRAMPUB_PLLCR_PLLPD_0   0x0
#define DRAMPUB_PLLCR_PLLRST_0  0x0
#define DRAMPUB_PLLCR_BYP_0     0x0

/* PHY Timing Registers 0 */
typedef union {
	u32 val;
	struct {
	u32 tphyrst:6;          /* PHY Reset Time: Number of clock cycles
				 * that the PHY reset must remain asserted
				 * after PHY calibration is done before
				 * the reset to the PHY is de-asserted.
				 * This is used to extend the reset to
				 * the PHY so that the reset is asserted
				 * for some clock cycles after the clocks
				 * are stable. */
	u32 tpllgs:15;          /* PLL Gear Shift Time: Number of clock
				 * cycles from when the PLL reset pin is
				 * de- asserted to when the PLL gear shift
				 * pin is de-asserted. This must correspond
				 * to a value that is equal to or more
				 * than 40us. Default value corresponds
				 * to 40us at
				 * 400MHz. */
	u32 tpllpd:11;          /* PLL Power-Down Time: Number of clock
				 * cycles that the PLL must remain in power-
				 * down mode, i.e. number of clock cycles
				 * from when PLL power-down pin is asserted
				 * to when PLL power-down pin is de-asserted.
				 * This must correspond to a value that
				 * is equal to or more than 1us. Default
				 * value corresponds to 1us at 400MHz. */
	} bits;
} reg_drampub_ptr0_t;

/* PHY Timing Registers 1 */
typedef union {
	u32 val;
	struct {
	u32 tpllrst:13;         /* PLL Reset Time: Number of clock cycles
				 * that the PLL must remain in reset mode,
				 * i.e. number of clock cycles from when
				 * PLL power-down pin is de-asserted and
				 * PLL reset pin is asserted to when PLL
				 * reset pin is de-asserted. This must
				 * correspond to a value that is equal
				 * to or more than 3us but less than 10us.
				 * Default value corresponds to 3us at 400MHz. */
	u32 rsvd0:3;            /* - */
	u32 tplllock:16;        /* PLL Lock Time: Number of clock cycles
				 * for the PLL to stabilize and lock, i.e.
				 * number of clock cycles from when the
				 * PLL reset pin is de-asserted to when
				 * the PLL has lock and is ready for use.
				 * This must correspond to a value that
				 * is equal to or more than
				 * 100us. Default value corresponds to
				 * 100us at 400MH. */
	} bits;
} reg_drampub_ptr1_t;

/* PHY Timing Registers 2 */
typedef union {
	u32 val;
	struct {
	u32 tcalon:5;           /* Calibration On Time: Number of clock
				 * cycles that the calibration clock is enabled
				 * (cal_clk_en asserted). */
	u32 tcals:5;            /* Calibration Setup Time: Number of clock
				 * cycles from when calibration is enabled
				 * (cal_en asserted) to when the calibration
				 * clock is asserted again (cal_clk_en
				 * asserted). */
	u32 tcalh:5;            /* Calibration Hold Time: Number of clock
				 * cycles from when the clock was disabled
				 * (cal_clk_en deasserted) to when calibration
				 * is enable (cal_en asserted). */
	u32 twldlys:5;          /* Write Leveling Delay Settling Time:
				 * Number of clock cycles from when a new
				 * value of the write leveling delay is
				 * applies to the LCDL to when to DQS high
				 * is driven high. This allows the delay
				 * to settle. */
	u32 rsvd0:12;           /* - */
	} bits;
} reg_drampub_ptr2_t;

/* PHY Timing Registers 3 */
typedef union {
	u32 val;
	struct {
	u32 tdinit0:20;         /* DRAM Initialization Time 0: DRAM initialization
				 * time corresponding to the following:
				 * DDR3 = CKE low time with power and clock
				 * stable (500 us)
				 * DDR2 = CKE low time with power and clock
				 * stable (200 us) Default value corresponds
				 * to DDR3 500 us at 1066 MHz. */
	u32 tdinit1:9;          /* DRAM Initialization Time 1: DRAM initialization
				 * time corresponding to the following:
				 * DDR3 = CKE high time to first command
				 * (tRFC + 10 ns or 5 tCK, whichever is
				 * bigger) DDR2 = CKE high time to first
				 * command (400 ns)
				 * Default value corresponds to DDR3 tRFC
				 * of 360ns at 1066 MHz. */
	u32 rsvd0:3;            /* - */
	} bits;
} reg_drampub_ptr3_t;

/* PHY Timing Registers 4 */
typedef union {
	u32 val;
	struct {
	u32 tdinit2:18;         /* DRAM Initialization Time 2: DRAM initialization
				 * time corresponding to the following:
				 * DDR3 = Reset low time (200 us on power-up
				 * or 100 ns after power-up)
				 * Default value corresponds to DDR3 200
				 * us at 1066 MHz. */
	u32 tdinit3:10;         /* DRAM Initialization Time 3: DRAM initialization
				 * time corresponding to the following:
				 * DDR3 = Time from ZQ initialization command
				 * to first command (1 us)
				 * Default value corresponds to the DDR3
				 * 640ns at 1066 MHz. */
	u32 rsvd0:4;            /* - */
	} bits;
} reg_drampub_ptr4_t;

/* AC Master Delay Line Register */
typedef union {
	u32 val;
	struct {
	u32 iprd:8;             /* Initial Period: Initial period measured
				 * by the master delay line calibration
				 * for VT drift compensation. This value
				 * is used as the denominator when calculating
				 * the ratios of updates during VT compensation. */
	u32 tprd:8;             /* Target Period: Target period measured
				 * by the master delay line calibration
				 * for VT drift compensation. This is the
				 * current measured value of the period
				 * and is continuously updated if the MDL
				 * is enabled to do so. */
	u32 mdld:8;             /* MDL Delay: Delay select for the LCDL
				 * for the Master Delay Line. */
	u32 rsvd0:8;            /* - */
	} bits;
} reg_drampub_acmdlr_t;

/* AC Bit Delay Line Register */
typedef union {
	u32 val;
	struct {
	u32 ck0bd:6;            /* CK0 Bit Delay: Delay select for the
				 * BDL on CK0. */
	u32 ck1bd:6;            /* CK1 Bit Delay: Delay select for the
				 * BDL on CK1. */
	u32 ck2bd:6;            /* CK2 Bit Delay: Delay select for the
				 * BDL on CK2. */
	u32 acbd:6;             /* Address/Command Bit Delay: Delay select
				 * for the BDLs on address and command signals. */
	u32 rsvd0:8;            /* - */
	} bits;
} reg_drampub_acbdlr_t;

/* AC I/O Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 aciom:1;            /* Address/Command I/O Mode: Selects SSTL
				 * mode (when set to 0) or CMOS mode (when
				 * set to 1) of the I/O for all address
				 * and command pins. This bit connects
				 * to bit [0] of the IOM pin on the D3F
				 * I/Os, and for other I/O libraries, it
				 * connects to the IOM pin of the I/O.
				 * [0x0]: SSTL mode[0x1]: CMOS mode */
	u32 acoe:1;             /* Address/Command Output Enable: Enables,
				 * when set, the output driver on the I/O
				 * for all address and command pins.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 acodt:1;            /* Address/Command On-Die Termination:
				 * Enables, when set, the on-die termination
				 * on the I/O for RAS#, CAS#, WE#, BA[2:0],
				 * and A[15:0] pins.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 acpdd:1;            /* AC Power Down Driver: Powers down, when
				 * set, the output driver on the I/O for
				 * RAS#, CAS#, WE#, BA[2:0], and A[15:0] pins.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 acpdr:1;            /* AC Power Down Receiver: Powers down,
				 * when set, the input receiver on the I/O for
				 * RAS#, CAS#, WE#, BA[2:0], and A[15:0] pins.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 ckodt:3;            /* CK On-Die Termination: Enables, when
				 * set, the on-die termination on the I/O for
				 * CK[0], CK[1], and CK[2] pins, respectively. */
	u32 ckpdd:3;            /* CK Power Down Driver: Powers down, when
				 * set, the output driver on the I/O for
				 * CK[0], CK[1], and CK[2] pins, respectively. */
	u32 ckpdr:3;            /* CK Power Down Receiver: Powers down,
				 * when set, the input receiver on the I/O for
				 * CK[0], CK[1], and CK[2] pins, respectively. */
	u32 rankodt:4;          /* Rank On-Die Termination: Enables, when
				 * set, the on-die termination on the I/O
				 * for CKE[3:0], ODT[3:0], and CS#[3:0]
				 * pins. RANKODT[0] controls the on-die
				 * termination for CKE[0], ODT[0], and
				 * CS#[0], RANKODT[1] controls the on-die
				 * termination for CKE[1], ODT[1], and
				 * CS#[1], and so on. */
	u32 cspdd:4;            /* CS# Power Down Driver: Powers down,
				 * when set, the output driver on the I/O
				 * for CS#[3:0] pins. CSPDD[0] controls
				 * the power down for CS#[0], CSPDD[1]
				 * controls the power down for CS#[1],
				 * and so on. CKE and ODT driver power
				 * down is controlled by DSGCR register. */
	u32 rankpdr:4;          /* Rank Power Down Receiver: Powers down,
				 * when set, the input receiver on the
				 * I/O CKE[3:0], ODT[3:0], and CS#[3:0]
				 * pins. RANKPDR[0] controls the power
				 * down for CKE[0], ODT[0], and CS#[0],
				 * RANKPDR[1] controls the power down for
				 * CKE[1], ODT[1], and CS#[1], and so on. */
	u32 rstodt:1;           /* SDRAM Reset On-Die Termination: Enables,
				 * when set, the on-die termination on
				 * the I/O for SDRAM RST# pin.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 rstpdd:1;           /* SDRAM Reset Power Down Driver: Powers
				 * down, when set, the output driver on the
				 * I/O for SDRAM RST# pin.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 rstpdr:1;           /* SDRAM Reset Power Down Receiver: Powers
				 * down, when set, the input receiver on
				 * the I/O for SDRAM RST# pin.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 rstiom:1;           /* SDRAM Reset I/O Mode: Selects SSTL mode
				 * (when set to 0) or CMOS mode
				 * (when set to 1) of the I/O for SDRAM Reset.
				 * [0x0]: SSTL mode[0x1]: CMOS mode */
	u32 acsr:2;             /* Address/Command Slew Rate (D3F I/O Only):
				 * Selects slew rate of the I/O for all
				 * address and command pins. */
	} bits;
} reg_drampub_aciocr_t;

#define DRAMPUB_ACIOCR_ACIOM_0          0x0
#define DRAMPUB_ACIOCR_ACOE_0           0x0
#define DRAMPUB_ACIOCR_ACODT_0          0x0
#define DRAMPUB_ACIOCR_ACPDD_0          0x0
#define DRAMPUB_ACIOCR_ACPDR_0          0x0
#define DRAMPUB_ACIOCR_RSTODT_0         0x0
#define DRAMPUB_ACIOCR_RSTPDD_0         0x0
#define DRAMPUB_ACIOCR_RSTPDR_0         0x0
#define DRAMPUB_ACIOCR_RSTIOM_0         0x0

/* DATX8 Common Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 dxodt:1;            /* Data On-Die Termination: Enables, when
				 * set, the on-die termination on the I/O
				 * for DQ, DM, and DQS/DQS# pins of all
				 * DATX8 macros. This bit is ORed with
				 * the ODT configuration bit of the individual
				 * DATX8 (¡°DATX8 Common Configuration
				 * Register (DXCCR)¡± on page 62)
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dxiom:1;            /* Data I/O Mode: Selects SSTL mode (when
				 * set to 0) or CMOS mode (when set to
				 * 1) of the I/O for DQ, DM, and DQS/DQS#
				 * pins of all DATX8 macros. This bit is
				 * ORed with the IOM configuration bit
				 * of the individual DATX8.
				 * [0x0]: SSTL mode[0x1]: CMOS mode */
	u32 mdlen:1;            /* Master Delay Line Enable: Enables, if
				 * set, all DATX8 master delay line calibration
				 * to perform subsequent period measurements
				 * following the initial period measurements
				 * that are performed after reset or on
				 * when calibration is manually triggered.
				 * These additional measurements are accumulated
				 * and filtered as long as this bit remains
				 * high. This bit it combined with the
				 * MDLEN bit in the individual DATX8.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dxpdd:1;            /* Data Power Down Driver: Powers down,
				 * when set, the output driver on I/O for
				 * DQ, DM, and DQS/DQS# pins of all DATX8
				 * macros. This bit is ORed with the PDD
				 * configuration bit of the individual DATX8.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dxpdr:1;            /* Data Power Down Receiver: Powers down,
				 * when set, the input receiver on I/O
				 * for DQ, DM, and DQS/DQS# pins of all
				 * DATX8 macros. This bit is ORed with
				 * the PDR configuration bit of the individual
				 * DATX8.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dqsres:4;           /* DQS Resistor: Selects the on-die pull-down/pull-up
				 * resistor for DQS pins. DQSRES[3] selects
				 * pull-down (when set to 0) or pull-up
				 * (when set to 1). DQSRES[2:0] selects
				 * the resistor value as follows:
				 * [0x0]: Open: On-die resistor disconnected
				 * (use external resistor)
				 * [0x1]: 688 ohms
				 * [0x2]: 611 ohms
				 * [0x3]: 550 ohms
				 * [0x4]: 500 ohms
				 * [0x5]: 458 ohms
				 * [0x6]: 393 ohms
				 * [0x7]: 344 ohms */
	u32 dqsnres:4;          /* DQS# Resistor: Selects the on-die pull-up/pull-down
				 * resistor for DQS# pins. Same encoding
				 * as DQSRES.
				 * [0x0]: Open: On-die resistor disconnected
				 * (use external resistor)
				 * [0x1]: 688 ohms
				 * [0x2]: 611 ohms
				 * [0x3]: 550 ohms
				 * [0x4]: 500 ohms
				 * [0x5]: 458 ohms
				 * [0x6]: 393 ohms
				 * [0x7]: 344 ohms */
	u32 dxsr:2;             /* Data Slew Rate (D3F I/O Only): Selects
				 * slew rate of the I/O for DQ, DM, and
				 * DQS/DQS# pins of all DATX8 macros. */
	u32 rsvd0:17;           /* - */
	} bits;
} reg_drampub_dxccr_t;

#define DRAMPUB_DXCCR_DXODT_0   0x0
#define DRAMPUB_DXCCR_DXIOM_0   0x0
#define DRAMPUB_DXCCR_MDLEN_0   0x0
#define DRAMPUB_DXCCR_DXPDD_0   0x0
#define DRAMPUB_DXCCR_DXPDR_0   0x0
#define DRAMPUB_DXCCR_DQSRES_0  0x0
#define DRAMPUB_DXCCR_DQSRES_1  0x1
#define DRAMPUB_DXCCR_DQSRES_2  0x2
#define DRAMPUB_DXCCR_DQSRES_3  0x3
#define DRAMPUB_DXCCR_DQSRES_4  0x4
#define DRAMPUB_DXCCR_DQSRES_5  0x5
#define DRAMPUB_DXCCR_DQSRES_6  0x6
#define DRAMPUB_DXCCR_DQSRES_7  0x7
#define DRAMPUB_DXCCR_DQSNRES_0 0x0
#define DRAMPUB_DXCCR_DQSNRES_1 0x1
#define DRAMPUB_DXCCR_DQSNRES_2 0x2
#define DRAMPUB_DXCCR_DQSNRES_3 0x3
#define DRAMPUB_DXCCR_DQSNRES_4 0x4
#define DRAMPUB_DXCCR_DQSNRES_5 0x5
#define DRAMPUB_DXCCR_DQSNRES_6 0x6
#define DRAMPUB_DXCCR_DQSNRES_7 0x7

/* DDR System General Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 puren:1;            /* PHY Update Request Enable: Specifies
				 * if set, that the PHY should issue PHY-
				 * initiated update request when there
				 * is DDL VT drift. By default the PHY
				 * does not issue PHY-initiated update requests.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 bdisen:1;           /* Byte Disable Enable: Specifies if set
				 * that the PHY should respond to DFI byte
				 * disable request. Otherwise the byte
				 * disable from the DFI is ignored in which
				 * case bytes can only be disabled using
				 * the DXnGCR register.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 zuen:1;             /* Impedance Update Enable: Specifies,
				 * if set, that in addition to DDL VT update,
				 * the PHY should also perform impedance
				 * calibration (update) whenever there
				 * is a controller initiated update request.
				 * By default the PHY does not perform
				 * impedance calibration when the update
				 * is triggered through the DFI.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 lpiopd:1;           /* Low Power I/O Power Down: Specifies
				 * if set that the PHY should respond to the DFI
				 * low power opportunity request and power
				 * down the I/Os of the byte.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 lppllpd:1;          /* Low Power PLL Power Down: Specifies
				 * if set that the PHY should respond to
				 * the DFI low power opportunity request
				 * and power down the PLL of the byte if
				 * the wakeup time request satisfies the
				 * PLL lock time.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 cuaen:1;            /* Controller Update Acknowledge Enable:
				 * Specifies, if set, that the PHY should
				 * issue controller update acknowledge
				 * when the DFI controller update request
				 * is asserted. By default the PHY does
				 * not acknowledge controller initiated
				 * update requests but simply does an update
				 * whenever there is a controller update
				 * request. This speeds up the update.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dqsgx:1;            /* DQS Gate Extension: Specifies if set
				 * that the DQS gating must be extended
				 * by two DRAM clock cycles and then re-centered,
				 * i.e. one clock cycle extension on either
				 * side.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 rsvd0:11;           /* - */
	u32 rrmode:1;           /* Rise-to-Rise Mode: Indicates if set
				 * that the PHY is configured to run in
				 * rise-to-rise mode. Otherwise if not
				 * set the PHY is running in rise-to-fall mode.
				 * [0x0]: rise-to-fall mode[0x1]: rise-to-rise
				 * mode */
	u32 sdrmode:1;          /* Single Data Rate Mode: Indicates if
				 * set that the external controller is
				 * configured to run in single data rate
				 * (SDR) mode. Otherwise if not set the
				 * controller is running in half data rate
				 * (HDR) mode. This bit not supported in
				 * the current version of the PUB. */
	u32 ckepdd:4;           /* CKE Power Down Driver: Powers down,
				 * when set, the output driver on the I/O
				 * for CKE[3:0] pins. CKEPDD[0] controls
				 * the power down for CKE[0], CKEPDD[1]
				 * controls the power down for CKE[1],
				 * and so on. */
	u32 odtpdd:4;           /* ODT Power Down Driver: Powers down,
				 * when set, the output driver on the I/O
				 * for ODT[3:0] pins. ODTPDD[0] controls
				 * the power down for ODT[0], ODTPDD[1]
				 * controls the power down for ODT[1],
				 * and so on. */
	u32 ckoe:1;             /* SDRAM CK Output Enable: Enables, when
				 * set, the output driver on the I/O for
				 * SDRAM CK/CK# pins.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 odtoe:1;            /* SDRAM ODT Output Enable: Enables, when
				 * set, the output driver on the I/O for
				 * SDRAM ODT pins.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 rstoe:1;            /* SDRAM Reset Output Enable: Enables,
				 * when set, the output driver on the I/O for
				 * SDRAM RST# pin.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 ckeoe:1;            /* SDRAM CKE Output Enable: Enables, when
				 * set, the output driver on the I/O for
				 * SDRAM CKE pins.
				 * [0x0]: Disabled[0x1]: Enabled */
	} bits;
} reg_drampub_dsgcr_t;

#define DRAMPUB_DSGCR_PUREN_0   0x0
#define DRAMPUB_DSGCR_BDISEN_0  0x0
#define DRAMPUB_DSGCR_ZUEN_0    0x0
#define DRAMPUB_DSGCR_LPIOPD_0  0x0
#define DRAMPUB_DSGCR_LPPLLPD_0 0x0
#define DRAMPUB_DSGCR_CUAEN_0   0x0
#define DRAMPUB_DSGCR_DQSGX_0   0x0
#define DRAMPUB_DSGCR_RRMODE_0  0x0
#define DRAMPUB_DSGCR_CKOE_0    0x0
#define DRAMPUB_DSGCR_ODTOE_0   0x0
#define DRAMPUB_DSGCR_RSTOE_0   0x0
#define DRAMPUB_DSGCR_CKEOE_0   0x0

/* DRAM Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 ddrmd:3;            /* DDR Mode: SDRAM DDR mode. Valid values are:
				 * [0x0]: LPDDR (Mobile DDR)
				 * [0x1]: DDR
				 * [0x2]: DDR2
				 * [0x3]: DDR3
				 * [0x4]: LPDDR2 (Mobile DDR2)
				 * [0x5]: Reserved
				 * [0x6]: Reserved
				 * [0x7]: Reserved */
	u32 ddr8bnk:1;          /* DDR 8-Bank: Indicates if set that the
				 * SDRAM used has 8 banks. tRPA = tRP+1
				 * and tFAW are used for 8-bank DRAMs,
				 * other tRPA = tRP and no tFAW is used.
				 * Note that a setting of 1 for DRAMs that
				 * have fewer than 8 banks still results
				 * in correct functionality but less tighter
				 * DRAM command spacing for the parameters
				 * described here.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 pdq:3;              /* Primary DQ (DDR3 Only): Specifies the
				 * DQ pin in a byte that is designated
				 * as a primary pin for Multi-Purpose Register
				 * (MPR) reads. Valid values are 0 to 7
				 * for DQ[0] to DQ[7], respectively. */
	u32 mprdq:1;            /* Multi-Purpose Register (MPR) DQ (DDR3
				 * Only): Specifies the value that is driven
				 * on non-primary DQ pins during MPR reads.
				 * Valid values are:
				 * [0x0]: Primary DQ drives out the data
				 * from MPR (0-1-0-1); non-primary DQs
				 * drive ¡®0¡¯
				 * [0x1]: Primary DQ and non-primary DQs
				 * all drive the same data from MPR (0-1-0-1) */
	u32 rsvd0:2;            /* - */
	u32 bytemask:8;         /* Byte Mask: Mask applied to all beats
				 * of read data on all bytes lanes during
				 * read DQS gate training. This allows
				 * training to be conducted based on selected
				 * bit(s) from the byte lanes.
				 * Note that this mask applies in DDR3
				 * MPR operation mode as well and must
				 * be in keeping with the PDQ field setting. */
	u32 rsvd1:9;            /* - */
	u32 nosra:1;            /* No Simultaneous Rank Access: Specifies
				 * if set that simultaneous rank access
				 * on the same clock cycle is not allowed.
				 * This means that multiple chip select
				 * signals should not be asserted at the
				 * same time. This may be required on some
				 * DIMM systems.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 ddr2t:1;            /* DDR 2T Timing: Indicates if set that
				 * 2T timing should be used by PUB internally
				 * generated SDRAM transactions.
				 * [0x0]: 1T Timing[0x1]: 2T Timing */
	u32 udimm:1;            /* Un-buffered DIMM Address Mirroring:
				 * Indicates if set that there is address
				 * mirroring on the second rank of an un-buffered
				 * DIMM (the rank connected to CS#[1]).
				 * In this case, the PUB re-scrambles the
				 * bank and address when sending mode register
				 * commands to the second rank. This only
				 * applies to PUB internal SDRAM transactions.
				 * Transactions generated by the controller
				 * must make its own adjustments when using
				 * an un-buffered DIMM. DCR[NOSRA] must
				 * be set if address mirroring is enabled.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 rdimm:1;            /* Registered DIMM: Indicates if set that
				 * a registered DIMM is used. In this case,
				 * the PUB increases the SDRAM write and
				 * read latencies (WL/RL) by 1. This only
				 * applies to PUB internal SDRAM transactions.
				 * Transactions generated by the controller
				 * must make its own adjustments to WL/RL
				 * when using a registered DIMM.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 rsvd2:1;            /* - */
	} bits;
} reg_drampub_dcr_t;

#define DRAMPUB_DCR_DDRMD_0     0x0
#define DRAMPUB_DCR_DDRMD_1     0x1
#define DRAMPUB_DCR_DDRMD_2     0x2
#define DRAMPUB_DCR_DDRMD_3     0x3
#define DRAMPUB_DCR_DDRMD_4     0x4
#define DRAMPUB_DCR_DDRMD_5     0x5
#define DRAMPUB_DCR_DDRMD_6     0x6
#define DRAMPUB_DCR_DDRMD_7     0x7
#define DRAMPUB_DCR_DDR8BNK_0   0x0
#define DRAMPUB_DCR_MPRDQ_0     0x0
#define DRAMPUB_DCR_MPRDQ_1     0x1
#define DRAMPUB_DCR_NOSRA_0     0x0
#define DRAMPUB_DCR_DDR2T_0     0x0
#define DRAMPUB_DCR_UDIMM_0     0x0
#define DRAMPUB_DCR_RDIMM_0     0x0

/* DRAM Timing Parameters Register 0 */
typedef union {
	u32 val;
	struct {
	u32 trtp:4;             /* Internal read to precharge command delay.
				 * Valid values are 2 to 6. */
	u32 twtr:4;             /* Internal write to read command delay.
				 * Valid values are 1 to 6. */
	u32 trp:4;              /* Precharge command period: The minimum
				 * time between a precharge command and
				 * any other command. Note that the Controller
				 * automatically derives tRPA for 8- bank
				 * DDR2 devices by adding 1 to tRP. Valid
				 * values are 2 to 11. */
	u32 trcd:4;             /* Activate to read or write delay. Minimum
				 * time from when an activate command is
				 * issued to when a read or write to the
				 * activated row can be issued. Valid values
				 * are 2 to 11. */
	u32 tras:6;             /* Activate to precharge command delay.
				 * Valid values are 2 to 31. */
	u32 trrd:4;             /* Activate to activate command delay (different
				 * banks). Valid values are 1 to 8. */
	u32 trc:6;              /* Activate to activate command delay (same
				 * bank). Valid values are 2 to 42. */
	} bits;
} reg_drampub_dtpr0_t;

/* DRAM Timing Parameters Register 1 */
typedef union {
	u32 val;
	struct {
	u32 tmrd:2;             /* Load mode cycle time: The minimum time
				 * between a load mode register command
				 * and any other command. For DDR3 this
				 * is the minimum time between two load
				 * mode register commands. Valid values
				 * for DDR2 are 2 to 3. For DDR3, the value
				 * used for tMRD is 4 plus the value programmed
				 * in these bits, i.e. tMRD value for DDR3
				 * ranges from 4 to 7. */
	u32 tmod:3;             /* Load mode update delay (DDR3 only).
				 * The minimum time between a load mode
				 * register command and a non-load mode
				 * register command. Valid values are:
				 * [0x0]: 12
				 * [0x1]: 13
				 * [0x2]: 14
				 * [0x3]: 15
				 * [0x4]: 16
				 * [0x5]: 17
				 * [0x6]: Reserved
				 * [0x7]: Reserved */
	u32 tfaw:6;             /* 4-bank activate period. No more than
				 * 4-bank activate commands may be issued
				 * in a given tFAW period. Only applies
				 * to 8-bank devices. Valid values are 2 to 31. */
	u32 trfc:9;             /* Refresh-to-Refresh: Indicates the minimum
				 * time, in clock cycles, between two refresh
				 * commands or between a refresh and an
				 * active command. This is derived from
				 * the minimum refresh interval from the
				 * datasheet, tRFC(min), divided by the
				 * clock cycle time. The default number
				 * of clock cycles is for the largest JEDEC
				 * tRFC(min parameter value supported. */
	u32 twlmrd:6;           /* Minimum delay from when write leveling
				 * mode is programmed to the first
				 * DQS/DQS# rising edge. */
	u32 twlo:4;             /* Write leveling output delay: Number
				 * of clock cycles from when write leveling
				 * DQS is driven high by the control block
				 * to when the results from the SDRAM on
				 * DQ is sampled by the control block.
				 * This must include the SDRAM tWLO timing
				 * parameter plus the round trip delay
				 * from control block to SDRAM back to
				 * control block. */
	u32 taond_taofd:2;      /* ODT turn-on/turn-off delays (DDR2 only).
				 * The delays are in clock cycles. Valid
				 * values are:
				 * [0x0]: 2/2.5
				 * [0x1]: 3/3.5
				 * [0x2]: 4/4.5
				 * [0x3]: 5/5.5
				 * Most DDR2 devices utilize a fixed value
				 * of 2/2.5. For non-standard SDRAMs, the
				 * user must ensure that the operational
				 * Write Latency is always greater than
				 * or equal to the ODT turn-on delay. For
				 * example, a DDR2 SDRAM with CAS latency
				 * set to 3 and CAS additive latency set
				 * to 0 has a Write Latency of 2. Thus
				 * 2/2.5 can be used, but not 3/3.5 or higher. */
	} bits;
} reg_drampub_dtpr1_t;

#define DRAMPUB_DTPR1_TMOD_0            0x0
#define DRAMPUB_DTPR1_TMOD_1            0x1
#define DRAMPUB_DTPR1_TMOD_2            0x2
#define DRAMPUB_DTPR1_TMOD_3            0x3
#define DRAMPUB_DTPR1_TMOD_4            0x4
#define DRAMPUB_DTPR1_TMOD_5            0x5
#define DRAMPUB_DTPR1_TMOD_6            0x6
#define DRAMPUB_DTPR1_TMOD_7            0x7
#define DRAMPUB_DTPR1_TAOND_TAOFD_0     0x0
#define DRAMPUB_DTPR1_TAOND_TAOFD_1     0x1
#define DRAMPUB_DTPR1_TAOND_TAOFD_2     0x2
#define DRAMPUB_DTPR1_TAOND_TAOFD_3     0x3

/* DRAM Timing Parameters Register 2 */
typedef union {
	u32 val;
	struct {
	u32 txs:10;             /* Self refresh exit delay. The minimum
				 * time between a self refresh exit command
				 * and any other command. This parameter
				 * must be set to the maximum of the various
				 * minimum self refresh exit delay parameters
				 * specified in the SDRAM datasheet, i.e.
				 * max(tXSNR, tXSRD) for DDR2 and max(tXS,
				 * tXSDLL) for DDR3. Valid values are 2 to
				 * 1023. */
	u32 txp:5;              /* Power down exit delay. The minimum time
				 * between a power down exit command and
				 * any other command. This parameter must
				 * be set to the maximum of the various
				 * minimum power down exit delay parameters
				 * specified in the SDRAM datasheet, i.e.
				 * max(tXP, tXARD, tXARDS) for DDR2 and
				 * max(tXP, tXPDLL) for DDR3. Valid values
				 * are 2 to 31. */
	u32 tcke:4;             /* CKE minimum pulse width. Also specifies
				 * the minimum time that the SDRAM must
				 * remain in power down or self refresh
				 * mode. For DDR3 this parameter must be
				 * set to the value of tCKESR which is
				 * usually bigger than the value of tCKE.
				 * Valid values are 2 to 15. */
	u32 tdllk:10;           /* DLL locking time. Valid values are 2 to 1023. */
	u32 trtodt:1;           /* Read to ODT delay (DDR3 only). Specifies
				 * whether ODT can be enabled immediately
				 * after the read post-amble or one clock
				 * delay has to be added. Valid values are:
				 * [0x0]: ODT may be turned on immediately
				 * after read post-amble
				 * [0x1]: ODT may not be turned on until
				 * one clock after the read post-amble
				 * If tRTODT is set to 1, then the read-to-write
				 * latency is increased by 1 if ODT is enabled. */
	u32 trtw:1;             /* Read to Write command delay. Valid values
				 * are:
				 * [0x0]: standard bus turn around delay
				 * [0x1]: add 1 clock to standard bus turn
				 * around delay
				 * This parameter allows the user to increase
				 * the delay between issuing Write commands
				 * to the SDRAM when preceded by Read commands.
				 * This provides an option to increase
				 * bus turn-around margin for high frequency
				 * systems. */
	u32 tccd:1;             /* Read to read and write to write command
				 * delay. Valid values are:
				 * [0x0]: BL/2 for DDR2 and 4 for DDR3
				 * [0x1]: BL/2 + 1 for DDR2 and 5 for DDR3 */
	} bits;
} reg_drampub_dtpr2_t;

#define DRAMPUB_DTPR2_TRTODT_0  0x0
#define DRAMPUB_DTPR2_TRTODT_1  0x1
#define DRAMPUB_DTPR2_TRTW_0    0x0
#define DRAMPUB_DTPR2_TRTW_1    0x1
#define DRAMPUB_DTPR2_TCCD_0    0x0
#define DRAMPUB_DTPR2_TCCD_1    0x1

/* DDR3 Mode Register 0 */
typedef union {
	u32 val;
	struct {
	u32 bl:2;               /* Burst Length: Determines the maximum
				 * number of column locations that can
				 * be accessed during a given read or write
				 * command. Valid values are:
				 * Valid values for DDR3 are:
				 * [0x0]: 8 (Fixed)
				 * [0x1]: 4 or 8 (On the fly)
				 * [0x2]: 4 (Fixed)
				 * [0x3]: Reserved */
	u32 cl_bit0:1;          /* CAS Latency */
	u32 bt:1;               /* Burst Type: Indicates whether a burst
				 * is sequential (0) or interleaved (1).
				 * [0x0]: sequential [0x1]: interleaved */
	u32 cl_bit1_2_3:3;      /* CAS Latency: The delay, in clock cycles,
				 * between when the SDRAM registers a read
				 * command to when data is available. Valid
				 * values are:
				 * 001 = 5
				 * 010 = 6
				 * 011 = 7
				 * 100 = 8
				 * 101 = 10
				 * 110 = 12
				 * All other settings are reserved and
				 * should not be used. */
	u32 tm:1;               /* Operating Mode: Selects either normal
				 * operating mode (0) or test mode (1).
				 * Test mode is reserved for the manufacturer
				 * and should not be used.
				 * [0x0]: operating mode[0x1]: test mode */
	u32 dr:1;               /* DLL Reset: Writing a ¡®1¡¯ to this bit
				 * will reset the SDRAM DLL. This bit is
				 * self- clearing, i.e. it returns back
				 * to ¡®0¡¯ after the DLL reset has been issued.
				 * [0x0]: de-assert[0x1]: reset */
	u32 wr:3;               /* Write Recovery: This is the value of
				 * the write recovery in clock cycles.
				 * It is calculated by dividing the datasheet
				 * write recovery time, tWR (ns) by the
				 * datasheet clock cycle time, tCK (ns)
				 * and rounding up a non-integer value
				 * to the next integer. Valid values are:
				 * [0x0]: Reserved
				 * [0x1]: 5
				 * [0x2]: 6
				 * [0x3]: 7
				 * [0x4]: 8
				 * [0x5]: 10
				 * [0x6]: 12
				 * [0x7]: Reserved
				 * All other settings are reserved and
				 * should not be used.
				 * NOTE: tWR (ns) is the time from the
				 * first SDRAM positive clock edge after
				 * the last data-in pair of a write command,
				 * to when a precharge of the same bank
				 * can be issued. */
	u32 pd:1;               /* Power-Down Control: Controls the exit
				 * time for power-down modes. Refer to the
				 * SDRAM datasheet for details on power-down
				 * modes. Valid values are:
				 * [0x0]: Slow exit (DLL off)
				 * [0x1]: Fast exit (DLL on) */
	u32 rsvd0:3;            /* Reserved. These are JEDEC reserved bits
				 * and are recommended by JEDEC to be programmed
				 * to ¡®0¡¯. */
	u32 rsvd1:16;           /* - */
	} bits;
} reg_drampub_mr0_t;

#define DRAMPUB_MR0_BL_0                0x0
#define DRAMPUB_MR0_BL_1                0x1
#define DRAMPUB_MR0_BL_2                0x2
#define DRAMPUB_MR0_BL_3                0x3
#define DRAMPUB_MR0_BT_0                0x0
#define DRAMPUB_MR0_TM_0                0x0
#define DRAMPUB_MR0_DR_0                0x0
#define DRAMPUB_MR0_WR_0                0x0
#define DRAMPUB_MR0_WR_1                0x1
#define DRAMPUB_MR0_WR_2                0x2
#define DRAMPUB_MR0_WR_3                0x3
#define DRAMPUB_MR0_WR_4                0x4
#define DRAMPUB_MR0_WR_5                0x5
#define DRAMPUB_MR0_WR_6                0x6
#define DRAMPUB_MR0_WR_7                0x7
#define DRAMPUB_MR0_PD_0                0x0
#define DRAMPUB_MR0_PD_1                0x1

/* DDR3 Mode Register 1 */
typedef union {
	u32 val;
	struct {
	u32 de:1;               /* DLL Enable/Disable: Enable (0) or disable
				 * (1) the DLL. DLL must be enabled for
				 * normal operation.
				 * [0x0]: Enabled [0x1]: Disabled */
	u32 dic_bit0:1;         /* Output Driver Impedance Control */
	u32 rtt_bit0:1;         /* On Die Termination */
	u32 al:2;               /* Posted CAS Additive Latency: Setting
				 * additive latency that allows read and
				 * write commands to be issued to the SDRAM
				 * earlier than normal (refer to the SDRAM
				 * datasheet for details). Valid values are:
				 * [0x0]: 0 (AL disabled)
				 * [0x1]: CL - 1
				 * [0x2]: CL - 2
				 * [0x3]: Reserved */
	u32 dic_bit1:1;         /* Output Driver Impedance Control: Controls
				 * the output drive strength. Valid values are:
				 * 00 = Reserved for RZQ/6
				 * 01 = RZQ7
				 * 10 = Reserved
				 * 11 = Reserved */
	u32 rtt_bit1:1;         /* On Die Termination */
	u32 level:1;            /* Write Leveling Enable: Enables write-leveling
				 * when set.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 rsvd0:1;            /* Reserved. This is a JEDEC reserved bit
				 * for DDR3 and is recommended by JEDEC
				 * to be programmed to ¡®0¡¯. */
	u32 rtt_bit2:1;         /* On Die Termination: Selects the effective
				 * resistance for SDRAM on die termination.
				 * Valid values are:
				 * 000 = ODT disabled
				 * 001 = RZQ/4
				 * 010 = RZQ/2
				 * 011 = RZQ/6
				 * 100 = RZQ/12
				 * 101 = RZQ/8
				 * All other settings are reserved and
				 * should not be used. */
	u32 rsvd1:1;            /* Reserved. This is a JEDEC reserved bit
				 * for DDR3 and is recommended by JEDEC
				 * to be programmed to ¡®0¡¯. */
	u32 tdqs:1;             /* Termination Data Strobe: When enabled
				 * (¡®1¡¯) TDQS provides additional termination
				 * resistance outputs that may be useful
				 * in some system configurations. Refer
				 * to the SDRAM datasheet for details.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 qoff:1;             /* Output Enable/Disable: When ¡®0¡¯, all
				 * outputs function normal; when ¡®1¡¯
				 * all SDRAM outputs are disabled removing
				 * output buffer current. This feature
				 * is intended to be used for IDD characterization
				 * of read current and should not be used
				 * in normal operation.
				 * [0x0]: outputs function normal[0x1]:
				 * outputs function disabled */
	u32 rsvd2:3;            /* Reserved. These are JEDEC reserved bits
				 * and are recommended by JEDEC to be programmed
				 * to ¡®0¡¯. */
	u32 rsvd3:16;           /* - */
	} bits;
} reg_drampub_mr1_t;

#define DRAMPUB_MR1_DE_0        0x0
#define DRAMPUB_MR1_AL_0        0x0
#define DRAMPUB_MR1_AL_1        0x1
#define DRAMPUB_MR1_AL_2        0x2
#define DRAMPUB_MR1_AL_3        0x3
#define DRAMPUB_MR1_LEVEL_0     0x0
#define DRAMPUB_MR1_TDQS_0      0x0
#define DRAMPUB_MR1_QOFF_0      0x0

/* DDR3 Mode Register 2 */
typedef union {
	u32 val;
	struct {
	u32 pasr:3;             /* Partial Array Self Refresh: Specifies
				 * that data located in areas of the array
				 * beyond the specified location will be
				 * lost if self refresh is entered.
				 * Valid settings for 4 banks are:
				 * 000 = Full Array
				 * 001 = Half Array (BA[1:0] = 00 & 01)
				 * 010 = Quarter Array (BA[1:0] = 00)
				 * 011 = Not defined
				 * 100 = 3/4 Array (BA[1:0] = 01, 10, & 11)
				 * 101 = Half Array (BA[1:0] = 10 & 11)
				 * 110 = Quarter Array (BA[1:0] = 11)
				 * 111 = Not defined
				 * Valid settings for 8 banks are:
				 * 000 = Full Array
				 * 001 = Half Array (BA[2:0] = 000, 001,
				 * 010 & 011)
				 * 010 = Quarter Array (BA[2:0] = 000, 001)
				 * 011 = 1/8 Array (BA[2:0] = 000)
				 * 100 = 3/4 Array (BA[2:0] = 010, 011,
				 * 100, 101, 110 & 111)
				 * 101 = Half Array (BA[2:0] = 100, 101,
				 * 110 & 111)
				 * 110 = Quarter Array (BA[2:0] = 110 & 111)
				 * 111 = 1/8 Array (BA[2:0] 111) */
	u32 cwl:3;              /* CAS Write Latency: The delay, in clock
				 * cycles, between when the SDRAM registers
				 * a write command to when write data is
				 * available. Valid values are:
				 * [0x0]: 5 (tCK > 2.5ns)
				 * [0x1]: 6 (2.5ns > tCK > 1.875ns)
				 * [0x2]: 7 (1.875ns > tCK> 1.5ns)
				 * [0x3]: 8 (1.5ns > tCK > 1.25ns)
				 * [0x4]: 9 (1.25ns > tCK > 1.07ns)
				 * [0x5]:10 (1.07ns > tCK > 0.935ns)
				 * [0x6]: 11 (0.935ns > tCK > 0.833ns)
				 * [0x7]: 12 (0.833ns > tCK > 0.75ns)
				 * All other settings are reserved and
				 * should not be used. */
	u32 asr:1;              /* Auto Self-Refresh: When enabled (¡®1¡¯),
				 * SDRAM automatically provides self-refresh
				 * power management functions for all supported
				 * operating temperature values. Otherwise
				 * the SRT bit must be programmed to indicate
				 * the temperature range.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 srt:1;              /* Self-Refresh Temperature Range: Selects
				 * either normal (¡®0¡¯) or extended (¡®1¡¯)
				 * operating temperature range during self
				 * -refresh.
				 * [0x0]: normal[0x1]: extended */
	u32 rsvd0:1;            /* Reserved. This bit is JEDEC reserved
				 * and is recommended by JEDEC to be programmed
				 * to ¡®0¡¯. */
	u32 rttwr:2;            /* Dynamic ODT: Selects RTT for dynamic
				 * ODT. Valid values are:
				 * [0x0]: Dynamic ODT off
				 * [0x1]: RZQ/4
				 * [0x2]: RZQ/2
				 * [0x3]: Reserved */
	u32 rsvd1:5;            /* Reserved. These are JEDEC reserved bits
				 * and are recommended by JEDEC to be programmed
				 * to ¡®0¡¯. */
	u32 rsvd2:16;           /* - */
	} bits;
} reg_drampub_mr2_t;

#define DRAMPUB_MR2_CWL_0       0x0
#define DRAMPUB_MR2_CWL_1       0x1
#define DRAMPUB_MR2_CWL_2       0x2
#define DRAMPUB_MR2_CWL_3       0x3
#define DRAMPUB_MR2_CWL_4       0x4
#define DRAMPUB_MR2_CWL_5       0x5
#define DRAMPUB_MR2_CWL_6       0x6
#define DRAMPUB_MR2_CWL_7       0x7
#define DRAMPUB_MR2_ASR_0       0x0
#define DRAMPUB_MR2_SRT_0       0x0
#define DRAMPUB_MR2_RTTWR_0     0x0
#define DRAMPUB_MR2_RTTWR_1     0x1
#define DRAMPUB_MR2_RTTWR_2     0x2
#define DRAMPUB_MR2_RTTWR_3     0x3

/* Mode Register 3 */
typedef union {
	u32 val;
	struct {
	u32 mprloc:2;           /* Multi-Purpose Register (MPR) Location:
				 * Selects MPR data location: Valid value are:
				 * 00 = Predefined pattern for system cali
				 * bration
				 * All other settings are reserved and
				 * should not be used. */
	u32 mpr:1;              /* Multi-Purpose Register Enable: Enables,
				 * if set, that read data should come from the
				 * Multi-Purpose Register. Otherwise read
				 * data come from the DRAM array.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 rsvd0:13;           /* Reserved. These are JEDEC reserved bits
				 * and are recommended by JEDEC to be programmed
				 * to ¡®0¡¯. */
	u32 rsvd1:16;           /* - */
	} bits;
} reg_drampub_mr3_t;

#define DRAMPUB_MR3_MPR_0       0x0

/* ODT Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 rdodt0:4;           /* Read ODT: Specifies whether ODT should
				 * be enabled (¡®1¡¯) or disabled (¡®0¡¯)
				 * on each of the up to four ranks when
				 * a read command is sent to rank n. RDODT0,
				 * RDODT1, RDODT2, and RDODT3 specify ODT
				 * settings when a read is to rank 0, rank
				 * 1, rank
				 * 2, and rank 3, respectively. The four
				 * bits of each field each represent a rank, the
				 * LSB being rank 0 and the MSB being rank
				 * 3. Default is to disable ODT during reads. */
	u32 rdodt1:4;           /* Read ODT: Specifies whether ODT should
				 * be enabled (¡®1¡¯) or disabled (¡®0¡¯)
				 * on each of the up to four ranks when
				 * a read command is sent to rank n. RDODT0,
				 * RDODT1, RDODT2, and RDODT3 specify ODT
				 * settings when a read is to rank 0, rank
				 * 1, rank
				 * 2, and rank 3, respectively. The four
				 * bits of each field each represent a rank, the
				 * LSB being rank 0 and the MSB being rank
				 * 3. Default is to disable ODT during reads. */
	u32 rdodt2:4;           /* Read ODT: Specifies whether ODT should
				 * be enabled (¡®1¡¯) or disabled (¡®0¡¯)
				 * on each of the up to four ranks when
				 * a read command is sent to rank n. RDODT0,
				 * RDODT1, RDODT2, and RDODT3 specify ODT
				 * settings when a read is to rank 0, rank
				 * 1, rank
				 * 2, and rank 3, respectively. The four
				 * bits of each field each represent a rank, the
				 * LSB being rank 0 and the MSB being rank
				 * 3. Default is to disable ODT during reads. */
	u32 rdodt3:4;           /* Read ODT: Specifies whether ODT should
				 * be enabled (¡®1¡¯) or disabled (¡®0¡¯)
				 * on each of the up to four ranks when
				 * a read command is sent to rank n. RDODT0,
				 * RDODT1, RDODT2, and RDODT3 specify ODT
				 * settings when a read is to rank 0, rank
				 * 1, rank
				 * 2, and rank 3, respectively. The four
				 * bits of each field each represent a rank, the
				 * LSB being rank 0 and the MSB being rank
				 * 3. Default is to disable ODT during reads. */
	u32 wrodt0:4;           /* Write ODT: Specifies whether ODT should
				 * be enabled (¡®1¡¯) or disabled (¡®0¡¯)
				 * on each of the up to four ranks when
				 * a write command is sent to rank n. WRODT0,
				 * WRODT1, WRODT2, and WRODT3 specify ODT
				 * settings when a write is to rank 0,
				 * rank 1, rank 2, and rank 3, respectively.
				 * The four bits of each field each represent
				 * a rank, the LSB being rank 0 and the
				 * MSB being rank 3. Default is to enable
				 * ODT only on rank being written to. */
	u32 wrodt1:4;           /* Write ODT: Specifies whether ODT should
				 * be enabled (¡®1¡¯) or disabled (¡®0¡¯)
				 * on each of the up to four ranks when
				 * a write command is sent to rank n. WRODT0,
				 * WRODT1, WRODT2, and WRODT3 specify ODT
				 * settings when a write is to rank 0,
				 * rank 1, rank 2, and rank 3, respectively.
				 * The four bits of each field each represent
				 * a rank, the LSB being rank 0 and the
				 * MSB being rank 3. Default is to enable
				 * ODT only on rank being written to. */
	u32 wrodt2:4;           /* Write ODT: Specifies whether ODT should
				 * be enabled (¡®1¡¯) or disabled (¡®0¡¯)
				 * on each of the up to four ranks when
				 * a write command is sent to rank n. WRODT0,
				 * WRODT1, WRODT2, and WRODT3 specify ODT
				 * settings when a write is to rank 0,
				 * rank 1, rank 2, and rank 3, respectively.
				 * The four bits of each field each represent
				 * a rank, the LSB being rank 0 and the
				 * MSB being rank 3. Default is to enable
				 * ODT only on rank being written to. */
	u32 wrodt3:4;           /* Write ODT: Specifies whether ODT should
				 * be enabled (¡®1¡¯) or disabled (¡®0¡¯)
				 * on each of the up to four ranks when
				 * a write command is sent to rank n. WRODT0,
				 * WRODT1, WRODT2, and WRODT3 specify ODT
				 * settings when a write is to rank 0,
				 * rank 1, rank 2, and rank 3, respectively.
				 * The four bits of each field each represent
				 * a rank, the LSB being rank 0 and the
				 * MSB being rank 3. Default is to enable
				 * ODT only on rank being written to. */
	} bits;
} reg_drampub_odtcr_t;

/* Data Training Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 dtrptn:4;           /* Data Training Repeat Number: Repeat
				 * number used to confirm stability of DDR
				 * write or read */
	u32 dtrank:2;           /* Data Training Rank: Selects the SDRAM
				 * rank to be used during data bit deskew
				 * and eye centering. */
	u32 dtmpr:1;            /* Data Training Using MPR (DDR3 Only):
				 * Specifies, if set, that DQS gate training
				 * should use the SDRAM Multi-Purpose Register
				 * (MPR) register. Otherwise data- training
				 * is performed by first writing to some
				 * locations in the SDRAM and then reading
				 * them back.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dtcmpd:1;           /* Data Training Compare Data: Specifies,
				 * if set, that DQS gate training should
				 * also check if the returning read data
				 * is correct. Otherwise data-training
				 * only checks if the correct number of
				 * DQS edges were returned.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dtwdqm:4;           /* Training WDQ Margin: Defines how close
				 * to 0 or how close to 2*(wdq calibration_value)
				 * the WDQ lcdl can be moved during training.
				 * Basically defines how much timing margin. */
	u32 dtwbddm:1;          /* Data Training Write Bit Deskew Data
				 * Mask. If set it enables write bit deskew
				 * of the data mask
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 rsvd0:3;            /* - */
	u32 dtdbs:4;            /* Data Training Debug Byte Select: Selects
				 * the byte during data training debug mode. */
	u32 dtden:1;            /* Data Training Debug Enable: Enables,
				 * if set, the data training debug mode.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dtdstp:1;           /* Data Training Debug Step: A write of
				 * 1 to this bit steps the data training
				 * algorithm through a single step. This
				 * bit is self-clearing.
				 * [0x0]: normal training[0x1]: single step */
	u32 rsvd1:1;            /* - */
	u32 dtexg:1;            /* Data Training with Early/Extended Gate:
				 * Specifies if set that the DQS gate training
				 * should be performed with an early/extended
				 * gate as specified in DSGCR[DQSGX].
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 ranken:4;           /* Rank Enable: Specifies the ranks that
				 * are enabled for data-training. Bit 0
				 * controls rank 0, bit 1 controls rank
				 * 1, bit 2 controls rank 2, and bit 3
				 * controls rank 3. Setting the bit to
				 * ¡®1¡¯ enables the rank, and setting
				 * it to ¡®0¡¯ disables the rank. */
	u32 rfshdt:4;           /* Refresh During Training: A non-zero
				 * value specifies that a burst of refreshes
				 * equal to the number specified in this
				 * field should be sent to the SDRAM after
				 * training each rank except the last rank. */
	} bits;
} reg_drampub_dtcr_t;

#define DRAMPUB_DTCR_DTMPR_0    0x0
#define DRAMPUB_DTCR_DTCMPD_0   0x0
#define DRAMPUB_DTCR_DTWBDDM_0  0x0
#define DRAMPUB_DTCR_DTDEN_0    0x0
#define DRAMPUB_DTCR_DTDSTP_0   0x0
#define DRAMPUB_DTCR_DTEXG_0    0x0

/* Data Training Address Register 0 */
typedef union {
	u32 val;
	struct {
	u32 dtcol:12;           /* Data Training Column Address: Selects
				 * the SDRAM column address to be used
				 * during data training. The lower four
				 * bits of this address must always be ¡°000¡±. */
	u32 dtrow:16;           /* Data Training Row Address: Selects the
				 * SDRAM row address to be used during
				 * data training. */
	u32 dtbank:3;           /* Data Training Bank Address: Selects
				 * the SDRAM bank address to be used during
				 * data training. */
	u32 rsvd0:1;            /* - */
	} bits;
} reg_drampub_dtar0_t;

/* Data Training Address Register 1 */
typedef union {
	u32 val;
	struct {
	u32 dtcol:12;           /* Data Training Column Address: Selects
				 * the SDRAM column address to be used
				 * during data training. The lower four
				 * bits of this address must always be ¡°000¡±. */
	u32 dtrow:16;           /* Data Training Row Address: Selects the
				 * SDRAM row address to be used during
				 * data training. */
	u32 dtbank:3;           /* Data Training Bank Address: Selects
				 * the SDRAM bank address to be used during
				 * data training. */
	u32 rsvd0:1;            /* - */
	} bits;
} reg_drampub_dtar1_t;

/* Data Training Address Register 2 */
typedef union {
	u32 val;
	struct {
	u32 dtcol:12;           /* Data Training Column Address: Selects
				 * the SDRAM column address to be used
				 * during data training. The lower four
				 * bits of this address must always be ¡°000¡±. */
	u32 dtrow:16;           /* Data Training Row Address: Selects the
				 * SDRAM row address to be used during
				 * data training. */
	u32 dtbank:3;           /* Data Training Bank Address: Selects
				 * the SDRAM bank address to be used during
				 * data training. */
	u32 rsvd0:1;            /* - */
	} bits;
} reg_drampub_dtar2_t;

/* Data Training Address Register 3 */
typedef union {
	u32 val;
	struct {
	u32 dtcol:12;           /* Data Training Column Address: Selects
				 * the SDRAM column address to be used
				 * during data training. The lower four
				 * bits of this address must always be ¡°000¡±. */
	u32 dtrow:16;           /* Data Training Row Address: Selects the
				 * SDRAM row address to be used during
				 * data training. */
	u32 dtbank:3;           /* Data Training Bank Address: Selects
				 * the SDRAM bank address to be used during
				 * data training. */
	u32 rsvd0:1;            /* - */
	} bits;
} reg_drampub_dtar3_t;

/* Data Training Data Register 0 */
typedef union {
	u32 val;
	struct {
	u32 dtbyte0:8;          /* Data Training Data: The byte0¡¯s data
				 * used during data training. This same
				 * data byte is used for each Byte Lane.
				 * Default sequence is a walking 1 while
				 * toggling data every data cycle. */
	u32 dtbyte1:8;          /* Data Training Data: The byte1¡¯s data
				 * used during data training. This same
				 * data byte is used for each Byte Lane.
				 * Default sequence is a walking 1 while
				 * toggling data every data cycle. */
	u32 dtbyte2:8;          /* Data Training Data: The byte2¡¯s data
				 * used during data training. This same
				 * data byte is used for each Byte Lane.
				 * Default sequence is a walking 1 while
				 * toggling data every data cycle. */
	u32 dtbyte3:8;          /* Data Training Data: The byte3¡¯s data
				 * used during data training. This same
				 * data byte is used for each Byte Lane.
				 * Default sequence is a walking 1 while
				 * toggling data every data cycle. */
	} bits;
} reg_drampub_dtdr0_t;

/* Data Training Data Register 1 */
typedef union {
	u32 val;
	struct {
	u32 dtbyte4:8;          /* Data Training Data: The byte4¡¯s data
				 * used during data training. This same
				 * data byte is used for each Byte Lane.
				 * Default sequence is a walking 1 while
				 * toggling data every data cycle. */
	u32 dtbyte5:8;          /* Data Training Data: The byte5¡¯s data
				 * used during data training. This same
				 * data byte is used for each Byte Lane.
				 * Default sequence is a walking 1 while
				 * toggling data every data cycle. */
	u32 dtbyte6:8;          /* Data Training Data: The byte6¡¯s data
				 * used during data training. This same
				 * data byte is used for each Byte Lane.
				 * Default sequence is a walking 1 while
				 * toggling data every data cycle. */
	u32 dtbyte7:8;          /* Data Training Data: The byte7¡¯s data
				 * used during data training. This same
				 * data byte is used for each Byte Lane.
				 * Default sequence is a walking 1 while
				 * toggling data every data cycle. */
	} bits;
} reg_drampub_dtdr1_t;

/* Data Training Eye Data Register 0 */
typedef union {
	u32 val;
	struct {
	u32 dtwlmn:8;           /* Data Training WDQ LCDL Minimum. */
	u32 dtwlmx:8;           /* Data Training WDQ LCDL Maximum. */
	u32 dtwbmn:8;           /* Data Training Write BDL Shift Minimum. */
	u32 dtwbmx:8;           /* Data Training Write BDL Shift Maximum. */
	} bits;
} reg_drampub_dtedr0_t;

/* Data Training Eye Data Register 1 */
typedef union {
	u32 val;
	struct {
	u32 dtrlmn:8;           /* Data Training RDQS LCDL Minimum. */
	u32 dtrlmx:8;           /* Data Training RDQS LCDL Maximum. */
	u32 dtrbmn:8;           /* Data Training Read BDL Shift Minimum. */
	u32 dtrbmx:8;           /* Data Training Read BDL Shift Maximum. */
	} bits;
} reg_drampub_dtedr1_t;

/* Vref Voltage Control Register for AC */
typedef union {
	u32 val;
	struct {
	u32 inner_cfg:6;        /* Inner Vref Configration */
	u32 inner_en:1;         /* Inner Vref Enable */
	u32 rsvd0:9;            /* - */
	u32 outer_cfg:6;        /* Outer Vref Configration */
	u32 outer_en:1;         /* Outer Vref Enable */
	u32 rsvd1:9;            /* - */
	} bits;
} reg_drampub_vref_ac_t;

/* Vref Voltage Control Register for DX01 */
typedef union {
	u32 val;
	struct {
	u32 inner_cfg:6;        /* Inner Vref Configration */
	u32 inner_en:1;         /* Inner Vref Enable */
	u32 rsvd0:9;            /* - */
	u32 outer_cfg:6;        /* Outer Vref Configration */
	u32 outer_en:1;         /* Outer Vref Enable */
	u32 rsvd1:9;            /* - */
	} bits;
} reg_drampub_vref_dx01_t;

/* Vref Voltage Control Register for DX23 */
typedef union {
	u32 val;
	struct {
	u32 inner_cfg:6;        /* Inner Vref Configration */
	u32 inner_en:1;         /* Inner Vref Enable */
	u32 rsvd0:9;            /* - */
	u32 outer_cfg:6;        /* Outer Vref Configration */
	u32 outer_en:1;         /* Outer Vref Enable */
	u32 rsvd1:9;            /* - */
	} bits;
} reg_drampub_vref_dx23_t;

/* FPGA Debug Register */
typedef union {
	u32 val;
	struct {
	u32 other:1;            /* OTHER */
	u32 rsvd0:1;            /* - */
	u32 smple:1;            /* Sample Phase Control
				 * [1¡¯b0]: Rise edge
				 * [1¡¯b1]: Pre-fall edge */
	u32 dqsgs:2;            /* DQS Gate Shift Step Control */
	u32 rsvd1:27;           /* - */
	} bits;
} reg_drampub_fpga_dbg_t;

#define DRAMPUB_FPGA_DBG_SMPLE_0        0x0
#define DRAMPUB_FPGA_DBG_SMPLE_1        0x1

/* DCU Address Register */
typedef union {
	u32 val;
	struct {
	u32 cwaddr:4;           /* Cache Word Address: Address of the cache
				 * word to be accessed. */
	u32 csaddr:4;           /* Cache Slice Address: Address of the
				 * cache slice to be accessed. */
	u32 csel:2;             /* Cache Select: Selects the cache to be
				 * accessed. Valid values are:
				 * [0x0]: Command cache
				 * [0x1]: Expected data cache
				 * [0x2]: Read data cache
				 * [0x3]: Reserved */
	u32 inca:1;             /* Increment Address: Specifies, if set,
				 * that the cache address specified in
				 * WADDR and SADDR should be automatically
				 * incremented after each access of the
				 * cache. The increment happens in such
				 * a way that all the slices of a selected
				 * word are first accessed before going
				 * to the next word.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 atype:1;            /* Access Type: Specifies the type of access
				 * to be performed using this address.
				 * Valid values are:
				 * [0x0]: Write access
				 * [0x1]: Read access */
	u32 rsvd0:20;           /* - */
	} bits;
} reg_drampub_dcuar_t;

#define DRAMPUB_DCUAR_CSEL_0    0x0
#define DRAMPUB_DCUAR_CSEL_1    0x1
#define DRAMPUB_DCUAR_CSEL_2    0x2
#define DRAMPUB_DCUAR_CSEL_3    0x3
#define DRAMPUB_DCUAR_INCA_0    0x0
#define DRAMPUB_DCUAR_ATYPE_0   0x0
#define DRAMPUB_DCUAR_ATYPE_1   0x1

/* DCU Data Register */
typedef union {
	u32 val;
	struct {
	u32 cdata:32;           /* Cache Data: Data to be written to or
				 * read from a cache. This data corresponds
				 * to the cache word slice specified by
				 * the DCU Address Register. */
	} bits;
} reg_drampub_dcudr_t;

/* DCU Run Register */
typedef union {
	u32 val;
	struct {
	u32 dinst:4;            /* DCU Instruction: Selects the DCU command
				 * to be executed: Valid values are:
				 * [0x0]: NOP: No operation
				 * [0x1]: Run: Triggers the execution of
				 * commands in the command cache.
				 * [0x2]: Stop: Stops the execution of
				 * commands in the command cache.
				 * [0x3]: Stop Loop: Stops the execution
				 * of an infinite loop in the command cache.
				 * [0x4]: Reset: Resets all DCU run time
				 * registers. See ¡°DCU Status¡± on page
				 * 150 for details.
				 * [0x5]: Reserved
				 * [0x6]: Reserved
				 * [0x7]: Reserved */
	u32 saddr:4;            /* Start Address: Cache word address where
				 * the execution of commands should begin. */
	u32 eaddr:4;            /* End Address: Cache word address where
				 * the execution of command should end. */
	u32 nfail:8;            /* Number of Failures: Specifies the number
				 * of failures after which the execution
				 * of commands and the capture of read
				 * data should stop if SONF bit of this
				 * register is set. Execution of commands
				 * and the capture of read data will stop
				 * after (NFAIL+1) failures if SONF is set. */
	u32 sonf:1;             /* Stop On Nth Fail: Specifies if set that
				 * the execution of commands and the capture
				 * of read data should stop when there
				 * are N read data failures. The number
				 * of failures is specified by NFAIL. Otherwise
				 * commands execute until the end of the
				 * program or until manually stopped using
				 * a STOP command.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 scof:1;             /* Stop Capture On Full: Specifies if set
				 * that the capture of read data should
				 * stop when the capture cache is full.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 rcen:1;             /* Read Capture Enable: Indicates if set
				 * that read data coming back from the SDRAM
				 * should be captured into the read data cache.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 xcen:1;             /* Expected Compare Enable: Indicates if
				 * set that read data coming back from the
				 * SDRAM should be should be compared with
				 * the expected data.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 rsvd0:8;            /* - */
	} bits;
} reg_drampub_dcurr_t;

#define DRAMPUB_DCURR_DINST_0   0x0
#define DRAMPUB_DCURR_DINST_1   0x1
#define DRAMPUB_DCURR_DINST_2   0x2
#define DRAMPUB_DCURR_DINST_3   0x3
#define DRAMPUB_DCURR_DINST_4   0x4
#define DRAMPUB_DCURR_DINST_5   0x5
#define DRAMPUB_DCURR_DINST_6   0x6
#define DRAMPUB_DCURR_DINST_7   0x7
#define DRAMPUB_DCURR_SONF_0    0x0
#define DRAMPUB_DCURR_SCOF_0    0x0
#define DRAMPUB_DCURR_RCEN_0    0x0
#define DRAMPUB_DCURR_XCEN_0    0x0

/* DCU Loop Register */
typedef union {
	u32 val;
	struct {
	u32 lsaddr:4;           /* Loop Start Address: Command cache word
				 * address where the loop should start. */
	u32 leaddr:4;           /* Loop End Address: Command cache word
				 * address where the loop should end. */
	u32 lcnt:8;             /* Loop Count: The number of times that
				 * the loop should be executed if LINF
				 * is not set. */
	u32 linf:1;             /* Loop Infinite: Indicates if set that
				 * the loop should be executed indefinitely
				 * until stopped by the STOP command. Otherwise
				 * the loop is execute LCNT times.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 ida:1;              /* Increment DRAM Address: Indicates if
				 * set that DRAM addresses should be incremented
				 * every time a DRAM read/write command
				 * inside the loop is executed.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 rsvd0:10;           /* - */
	u32 xleaddr:4;          /* Expected Data Loop End Address: The
				 * last expected data cache word address
				 * that contains valid expected data. Expected
				 * data should looped between 0 and this
				 * address. */
	} bits;
} reg_drampub_dculr_t;

#define DRAMPUB_DCULR_LINF_0    0x0
#define DRAMPUB_DCULR_IDA_0     0x0

/* DCU General Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 rcsw:16;            /* Read Capture Start Word: The capture
				 * and compare of read data should start after
				 * Nth word. For example setting this value
				 * to 12 will skip the first 12 read data. */
	u32 rsvd0:16;           /* - */
	} bits;
} reg_drampub_dcugcr_t;

/* DCU Timing Parameters Registers */
typedef union {
	u32 val;
	struct {
	u32 tdcut0:8;           /* DCU Generic Timing Parameter 0. */
	u32 tdcut1:8;           /* DCU Generic Timing Parameter 1. */
	u32 tdcut2:8;           /* DCU Generic Timing Parameter 2. */
	u32 tdcut3:8;           /* DCU Generic Timing Parameter 3. */
	} bits;
} reg_drampub_dcutpr_t;

/* DCU Status Register 0 */
typedef union {
	u32 val;
	struct {
	u32 rdone:1;            /* Run Done: Indicates if set that the
				 * DCU has finished executing the commands
				 * in the command cache. This bit is also
				 * set to indicate that a STOP command
				 * has successfully been executed and command
				 * execution has stopped. */
	u32 cfail:1;            /* Capture Fail: Indicates if set that
				 * at least one read data word has failed. */
	u32 cfull:1;            /* Capture Full: Indicates if set that
				 * the capture cache is full. */
	u32 rsvd0:29;           /* - */
	} bits;
} reg_drampub_dcusr0_t;

/* DCU Status Register 1 */
typedef union {
	u32 val;
	struct {
	u32 rdcnt:16;           /* Read Count: Number of read words returned
				 * from the SDRAM. */
	u32 flcnt:8;            /* Fail Count: Number of read words that
				 * have failed. */
	u32 lpcnt:8;            /* Loop Count: Indicates the value of the
				 * loop count. This is useful when the
				 * program has stopped because of failures
				 * to assess how many reads were executed
				 * before first fail. */
	} bits;
} reg_drampub_dcusr1_t;

/* BIST Run Register */
typedef union {
	u32 val;
	struct {
	u32 binst:3;            /* BIST Instruction: Selects the BIST instruction
				 * to be executed: Valid values are:
				 * [0x0]: NOP: No operation
				 * [0x1]: Run: Triggers the running of the BIST.
				 * [0x2]: Stop: Stops the running of the BIST.
				 * [0x3]: Reset: Resets all BIST run-time
				 * registers, such as error counters.
				 * [0x4]:  Reserved
				 * [0x5]:  Reserved
				 * [0x6]:  Reserved
				 * [0x7]:  Reserved */
	u32 bmode:1;            /* BIST Mode: Selects the mode in which
				 * BIST is run. Valid values are:
				 * [0x0]:Loopback mode: Address, commands
				 * and data loop back at the PHY I/Os.
				 * [0x1]: DRAM mode: Address, commands
				 * and data go to DRAM for normal memory
				 * accesses. */
	u32 binf:1;             /* BIST Infinite Run: Specifies if set
				 * that the BIST should be run indefinitely
				 * until when it is either stopped or a
				 * failure has been encountered. Otherwise
				 * BIST is run until number of BIST words
				 * specified in the BISTWCR register has
				 * been generated.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 nfail:8;            /* Number of Failures: Specifies the number
				 * of failures after which the execution
				 * of commands and the capture of read
				 * data should stop if BSONF bit of this
				 * register is set. Execution of commands
				 * and the capture of read data will stop
				 * after (NFAIL+1) failures if BSONF is set. */
	u32 bsonf:1;            /* BIST Stop On Nth Fail: Specifies if
				 * set that the BIST should stop when an
				 * nth data word or address/command comparison
				 * error has been encountered. */
	u32 bdxen:1;            /* BIST DATX8 Enable: Enables the running
				 * of BIST on the data byte lane PHYs.
				 * This bit is exclusive with BACEN, i.e.
				 * both cannot be set to ¡®1¡¯ at the same time.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 bacen:1;            /* BIST AC Enable: Enables the running
				 * of BIST on the address/command lane
				 * PHY. This bit is exclusive with BDXEN,
				 * i.e. both cannot be set to ¡®1¡¯ at
				 * the same time.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 bdmen:1;            /* BIST Data Mask Enable: Enables if set
				 * that the data mask BIST should be included
				 * in the BIST run, i.e. data pattern generated
				 * and loopback data compared. This is
				 * valid only for loopback mode.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 bdpat:2;            /* BIST Data Pattern: Selects the data
				 * pattern used during BIST. Valid values are:
				 * 00 = Walking 0
				 * 01 = Walking 1
				 * 10 = LFSR-based pseudo-random
				 * 11 = User programmable */
	u32 bdxsel:4;           /* BIST DATX8 Select: Select the byte lane
				 * for comparison of loopback/read data.
				 * Valid values are 0 to 8. */
	u32 bcksel:2;           /* BIST CK Select: Selects the CK that
				 * should be used to register the AC loopback
				 * signals from the I/Os. Valid values are:
				 * 00 = CK[0]
				 * 01 = CK[1]
				 * 10 = CK[2]
				 * 11 = Reserved */
	u32 bccsel:2;           /* BIST Clock Cycle Select: Selects the
				 * clock numbers on which the AC loopback
				 * data is written into the FIFO. Data
				 * is written into the loopback FIFO once
				 * every four clock cycles. Valid values are:
				 * 00 = Clock cycle 0, 4, 8, 12, etc.
				 * 01 = Clock cycle 1, 5, 9, 13, etc.
				 * 10 = Clock cycle 2, 6, 10, 14, etc.
				 * 11 = Clock cycle 3, 7, 11, 15, etc. */
	u32 rsvd0:5;            /* - */
	} bits;
} reg_drampub_bistrr_t;

#define DRAMPUB_BISTRR_BINST_0  0x0
#define DRAMPUB_BISTRR_BINST_1  0x1
#define DRAMPUB_BISTRR_BINST_2  0x2
#define DRAMPUB_BISTRR_BINST_3  0x3
#define DRAMPUB_BISTRR_BINST_4  0x4
#define DRAMPUB_BISTRR_BINST_5  0x5
#define DRAMPUB_BISTRR_BINST_6  0x6
#define DRAMPUB_BISTRR_BINST_7  0x7
#define DRAMPUB_BISTRR_BMODE_0  0x0
#define DRAMPUB_BISTRR_BMODE_1  0x1
#define DRAMPUB_BISTRR_BINF_0   0x0
#define DRAMPUB_BISTRR_BDXEN_0  0x0
#define DRAMPUB_BISTRR_BACEN_0  0x0
#define DRAMPUB_BISTRR_BDMEN_0  0x0

/* BIST Word Count Register */
typedef union {
	u32 val;
	struct {
	u32 bwcnt:16;           /* BIST Word Count: Indicates the number
				 * of words to generate during BIST. This
				 * must be a multiple of DRAM burst length
				 * (BL) divided by 2, e.g. for BL=8, valid
				 * values are 4, 8, 12, 16, and so on. */
	u32 rsvd0:16;           /* - */
	} bits;
} reg_drampub_bistwcr_t;

/* BIST Mask Register 0 */
typedef union {
	u32 val;
	struct {
	u32 amsk:16;            /* Mask bit for each of the up to 16 address
				 * bits. */
	u32 bamsk:3;            /* Mask bit for each of the up to 3 bank
				 * address bits. */
	u32 wemsk:1;            /* Mask bit for the WE#. */
	u32 ckemsk:4;           /* Mask bit for each of the up to 4 CKE bits. */
	u32 csmsk:4;            /* Mask bit for each of the up to 4 CS# bits. */
	u32 odtmsk:4;           /* Mask bit for each of the up to 4 ODT bits. */
	} bits;
} reg_drampub_bistmskr0_t;

/* BIST Mask Register 1 */
typedef union {
	u32 val;
	struct {
	u32 rasmsk:1;           /* Mask bit for the RAS. */
	u32 casmsk:1;           /* Mask bit for the CAS. */
	u32 rsvd0:26;           /* - */
	u32 dmmsk:4;            /* Mask bit for the data mask (DM) bit. */
	} bits;
} reg_drampub_bistmskr1_t;

/* BIST Mask Register 2 */
typedef union {
	u32 val;
	struct {
	u32 dqmsk:32;           /* Mask bit for each of the 8 data (DQ) bits. */
	} bits;
} reg_drampub_bistmskr2_t;

/* BIST LFSR Seed Register */
typedef union {
	u32 val;
	struct {
	u32 seed:32;            /* LFSR seed for pseudo-random BIST patterns. */
	} bits;
} reg_drampub_bistlsr_t;

/* BIST Address Register 0 */
typedef union {
	u32 val;
	struct {
	u32 bcol:12;            /* BIST Column Address: Selects the SDRAM
				 * column address to be used during BIST.
				 * The lower bits of this address must
				 * be ¡°0000¡± for BL16, ¡°000¡± for BL8,
				 * ¡°00¡± for BL4 and ¡°0¡± for BL2. */
	u32 brow:16;            /* BIST Row Address: Selects the SDRAM
				 * row address to be used during BIST. */
	u32 bbank:3;            /* BIST Bank Address: Selects the SDRAM
				 * bank address to be used during BIST. */
	u32 rsvd0:1;            /* - */
	} bits;
} reg_drampub_bistar0_t;

/* BIST Address Register 1 */
typedef union {
	u32 val;
	struct {
	u32 brank:2;            /* BIST Rank: Selects the SDRAM rank to
				 * be used during BIST. Valid values range
				 * from 0 to maximum ranks minus 1. */
	u32 bmrank:2;           /* BIST Maximum Rank: Specifies the maximum
				 * SDRAM rank to be used during BIST. The
				 * default value is set to maximum ranks
				 * minus 1. Example default shown here
				 * is for a 4-rank system */
	u32 bainc:12;           /* BIST Address Increment: Selects the
				 * value by which the SDRAM address is
				 * incremented for each write/read access.
				 * This value must be at the beginning
				 * of a burst boundary, i.e. the lower
				 * bits must be ¡°0000¡± for BL16, ¡°000¡±
				 * for BL8, ¡°00¡± for BL4 and ¡°0¡± for BL2. */
	u32 rsvd0:16;           /* - */
	} bits;
} reg_drampub_bistar1_t;

/* BIST Address Register 2 */
typedef union {
	u32 val;
	struct {
	u32 bmcol:12;           /* BIST Maximum Column Address: Specifies
				 * the maximum SDRAM column address to
				 * be used during BIST before the address
				 * increments to the next row. */
	u32 bmrow:16;           /* BIST Maximum Row Address: Specifies
				 * the maximum SDRAM row address to be
				 * used during BIST before the address
				 * increments to the next bank. */
	u32 bmbank:3;           /* BIST Maximum Bank Address: Specifies
				 * the maximum SDRAM bank address to be
				 * used during BIST before the address
				 * increments to the next rank. */
	u32 rsvd0:1;            /* - */
	} bits;
} reg_drampub_bistar2_t;

/* BIST User Data Pattern Register */
typedef union {
	u32 val;
	struct {
	u32 budp0:16;           /* BIST User Data Pattern 0: Data to be
				 * applied on even DQ pins during BIST. */
	u32 budp1:16;           /* BIST User Data Pattern 1: Data to be
				 * applied on odd DQ pins during BIST. */
	} bits;
} reg_drampub_bistudpr_t;

/* BIST General Status Register */
typedef union {
	u32 val;
	struct {
	u32 bdone:1;            /* BIST Done: Indicates if set that the
				 * BIST has finished executing. This bit
				 * is reset to zero when BIST is triggered. */
	u32 bacerr:1;           /* BIST Address/Command Error: indicates
				 * if set that there is a data comparison
				 * error in the address/command lane. */
	u32 bdxerr:1;           /* BIST Data Error: indicates if set that
				 * there is a data comparison error in
				 * the byte lane. */
	u32 rsvd0:17;           /* - */
	u32 dmber:8;            /* DM Bit Error: Indicates the number of
				 * bit errors on data mask (DM) bit. DMBER[1:0]
				 * are for even DQS cycles first DM beat,
				 * and DMBER[3:2] are for even DQS cycles
				 * second DM beat. Similarly, DMBER[5:4]
				 * are for odd DQS cycles first DM beat,
				 * and DMBER[7:6] are for odd DQS cycles
				 * second DM beat. */
	u32 rasber:2;           /* RAS Bit Error: Indicates the number
				 * of bit errors on RAS. */
	u32 casber:2;           /* CAS Bit Error: Indicates the number
				 * of bit errors on CAS. */
	} bits;
} reg_drampub_bistgsr_t;

/* BIST Word Error Register */
typedef union {
	u32 val;
	struct {
	u32 acwer:16;           /* Address/Command Word Error: Indicates
				 * the number of word errors on the address/command
				 * lane. An error on any bit of the address/command
				 * bus increments the error count. */
	u32 dxwer:16;           /* Byte Word Error: Indicates the number
				 * of word errors on the byte lane. An
				 * error on any bit of the data bus including
				 * the data mask bit increments the error count. */
	} bits;
} reg_drampub_bistwer_t;

/* BIST Bit Error Register 0 */
typedef union {
	u32 val;
	struct {
	u32 aber:32;            /* Address Bit Error: Each group of two
				 * bits indicate the bit error count on
				 * each of the up to 16 address bits. [1:0]
				 * is the error count for A[0], [3:2] for
				 * A[1], and so on. */
	} bits;
} reg_drampub_bistber0_t;

/* BIST Bit Error Register 1 */
typedef union {
	u32 val;
	struct {
	u32 baber:6;            /* Bank Address Bit Error: Each group of
				 * two bits indicate the bit error count
				 * on each of the up to 3 bank address
				 * bits. [1:0] is the error count for BA[0],
				 * [3:2] for BA[1], and so on. */
	u32 weber:2;            /* WE# Bit Error: Indicates the number
				 * of bit errors on WE#. */
	u32 ckeber:8;           /* CKE Bit Error: Each group of two bits
				 * indicate the bit error count on each
				 * of the up to 4 CKE bits. [1:0] is the
				 * error count for CKE[0], [3:2] for CKE[1],
				 * and so on. */
	u32 csber:8;            /* CS# Bit Error: Each group of two bits
				 * indicate the bit error count on each
				 * of the up to 4 CS# bits. [1:0] is the
				 * error count for CS#[0], [3:2] for CS#[1],
				 * and so on. */
	u32 odtber:8;           /* ODT Bit Error: Each group of two bits
				 * indicates the bit error count on each
				 * of the up to 4 ODT bits. [1:0] is the
				 * error count for ODT[0], [3:2] for ODT[1],
				 * and so on. */
	} bits;
} reg_drampub_bistber1_t;

/* BIST Bit Error Register 2 */
typedef union {
	u32 val;
	struct {
	u32 dqber0:32;          /* Data Bit Error: The error count for
				 * even DQS cycles. The first 16 bits indicate
				 * the error count for the first data beat
				 * (i.e. the data driven out on DQ[7:0]
				 * on the rising edge of DQS). The second
				 * 16 bits indicate the error on the second
				 * data beat (i.e. the error count of the
				 * data driven out on DQ[7:0] on the falling
				 * edge of DQS). For each of the 16-bit
				 * group, the first 2 bits are for DQ[0],
				 * the second for DQ[1], and so on. */
	} bits;
} reg_drampub_bistber2_t;

/* BIST Bit Error Register 3 */
typedef union {
	u32 val;
	struct {
	u32 dqber1:32;          /* Data Bit Error: The error count for
				 * odd DQS cycles. The first 16 bits indicate
				 * the error count for the first data beat
				 * (i.e. the data driven out on DQ[7:0]
				 * on the rising edge of DQS). The second
				 * 16 bits indicate the error on the second
				 * data beat (i.e. the error count of the
				 * data driven out on DQ[7:0] on the falling
				 * edge of DQS). For each of the 16-bit
				 * group, the first 2 bits are for DQ[0],
				 * the second for DQ[1], and so on. */
	} bits;
} reg_drampub_bistber3_t;

/* BIST Word Count Status Register */
typedef union {
	u32 val;
	struct {
	u32 acwcnt:16;          /* Address/Command Word Count: Indicates
				 * the number of words received from the
				 * address/command lane. */
	u32 dxwcnt:16;          /* Byte Word Count: Indicates the number
				 * of words received from the byte lane. */
	} bits;
} reg_drampub_bistwcsr_t;

/* BIST Fail Word Register 0 */
typedef union {
	u32 val;
	struct {
	u32 awebs:16;           /* Bit status during a word error for each
				 * of the up to 16 address bits. */
	u32 bawebs:3;           /* Bit status during a word error for each
				 * of the up to 3 bank address bits. */
	u32 wewebs:1;           /* Bit status during a word error for the WE#. */
	u32 ckewebs:4;          /* Bit status during a word error for each
				 * of the up to 4 CKE bits. */
	u32 cswebs:4;           /* Bit status during a word error for each
				 * of the up to 4 CS# bits. */
	u32 odtwebs:4;          /* Bit status during a word error for each
				 * of the up to 4 ODT bits. */
	} bits;
} reg_drampub_bistfwr0_t;

/* BIST Fail Word Register 1 */
typedef union {
	u32 val;
	struct {
	u32 raswebs:1;          /* Bit status during a word error for the RAS. */
	u32 caswebs:1;          /* Bit status during a word error for the CAS. */
	u32 rsvd0:26;           /* - */
	u32 dmwebs:4;           /* Bit status during a word error for the
				 * data mask (DM) bit. DMWEBS [0] is for
				 * the first DM beat, DMWEBS [1] is for
				 * the second DM beat, and so on. */
	} bits;
} reg_drampub_bistfwr1_t;

/* BIST Fail Word Register 2 */
typedef union {
	u32 val;
	struct {
	u32 dqwebs:32;          /* Bit status during a word error for each
				 * of the 8 data (DQ) bits. The first 8
				 * bits indicate the status of the first
				 * data beat (i.e. the status of the data
				 * driven out on DQ[7:0] on the rising
				 * edge of DQS). The second 8 bits indicate
				 * the status of the second data beat (i.e.
				 * the status of the data driven out on
				 * DQ[7:0] on the falling edge of DQS),
				 * and so on. For each of the 8-bit group,
				 * the first bit is for DQ[0], the second
				 * bit is for DQ[1], and so on. */
	} bits;
} reg_drampub_bistfwr2_t;

/* General Purpose Register 0 */
typedef union {
	u32 val;
	struct {
	u32 gpr0:32;            /* General Purpose Register 0: General
				 * purpose register bits. */
	} bits;
} reg_drampub_gpr0_t;

/* General Purpose Register 1 */
typedef union {
	u32 val;
	struct {
	u32 gpr1:32;            /* General Purpose Register 1: General
				 * purpose register bits. */
	} bits;
} reg_drampub_gpr1_t;

/* ZQ [CH] Impedance Control Register 0 */
typedef union {
	u32 val;
	struct {
	u32 zdata:28;           /* Impedance Over-Ride Data: Data used
				 * to directly drive the impedance control.
				 * ZDATA field mapping for D3F I/Os is
				 * as follows:
				 * ZDATA[27:21] is used to select the pull-up
				 * on-die termination impedance ZDATA[20:14]
				 * is used to select the pull-down on-die
				 * termination impedance ZDATA[13:7] is
				 * used to select the pull-up output impedance
				 * ZDATA[6:0] is used to select the pull-down
				 * output impedance
				 * ZDATA field mapping for D3A/B/R I/Os
				 * is as follows: ZDATA[27:20] is reserved
				 * and returns zeros on reads
				 * ZDATA[19:15] is used to select the pull-up
				 * on-die termination impedance ZDATA[14:10]
				 * is used to select the pull-down on-die
				 * termination impedance ZDATA[9:5] is
				 * used to select the pull-up output impedance
				 * ZDATA[4:0] is used to select the pull-down
				 * output impedance */
	u32 zden:1;             /* Impedance Over-ride Enable: When this
				 * bit is set, it allows users to directly
				 * drive the impedance control using the
				 * data programmed in the ZQDATA field.
				 * Otherwise, the control is generated
				 * automatically by the impedance control logic
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 zcalbyp:1;          /* Impedance Calibration Bypass: Bypasses,
				 * if set, impedance calibration of this
				 * ZQ control block when impedance calibration
				 * is already in progress. Impedance calibration
				 * can be disabled prior to trigger by
				 * using the ZCALEN bit.
				 * [0x0]: not bypass[0x1]: bypass */
	u32 zcalen:1;           /* Impedance Calibration Enable: Enables
				 * if the set the impedance calibration
				 * of this ZQ control block when impedance
				 * calibration is triggered using either
				 * the ZCAL bit of PIR register or the
				 * DFI update interface.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 zqpd:1;             /* ZQ Power Down: Powers down, if set,
				 * the PZQ cell.
				 * [0x0]: Disabled[0x1]: Enabled */
	} bits;
} reg_drampub_zq0cr0_t;

#define DRAMPUB_ZQ0CR0_ZDEN_0           0x0
#define DRAMPUB_ZQ0CR0_ZCALBYP_0        0x0
#define DRAMPUB_ZQ0CR0_ZCALEN_0         0x0
#define DRAMPUB_ZQ0CR0_ZQPD_0           0x0

/* ZQ [CH] Impedance Control Register 0 */
typedef union {
	u32 val;
	struct {
	u32 zdata:28;           /* Impedance Over-Ride Data: Data used
				 * to directly drive the impedance control.
				 * ZDATA field mapping for D3F I/Os is
				 * as follows:
				 * ZDATA[27:21] is used to select the pull-up
				 * on-die termination impedance ZDATA[20:14]
				 * is used to select the pull-down on-die
				 * termination impedance ZDATA[13:7] is
				 * used to select the pull-up output impedance
				 * ZDATA[6:0] is used to select the pull-down
				 * output impedance
				 * ZDATA field mapping for D3A/B/R I/Os
				 * is as follows: ZDATA[27:20] is reserved
				 * and returns zeros on reads
				 * ZDATA[19:15] is used to select the pull-up
				 * on-die termination impedance ZDATA[14:10]
				 * is used to select the pull-down on-die
				 * termination impedance ZDATA[9:5] is
				 * used to select the pull-up output impedance
				 * ZDATA[4:0] is used to select the pull-down
				 * output impedance */
	u32 zden:1;             /* Impedance Over-ride Enable: When this
				 * bit is set, it allows users to directly
				 * drive the impedance control using the
				 * data programmed in the ZQDATA field.
				 * Otherwise, the control is generated
				 * automatically by the impedance control logic
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 zcalbyp:1;          /* Impedance Calibration Bypass: Bypasses,
				 * if set, impedance calibration of this
				 * ZQ control block when impedance calibration
				 * is already in progress. Impedance calibration
				 * can be disabled prior to trigger by
				 * using the ZCALEN bit.
				 * [0x0]: not bypass[0x1]: bypass */
	u32 zcalen:1;           /* Impedance Calibration Enable: Enables
				 * if the set the impedance calibration
				 * of this ZQ control block when impedance
				 * calibration is triggered using either
				 * the ZCAL bit of PIR register or the
				 * DFI update interface.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 zqpd:1;             /* ZQ Power Down: Powers down, if set,
				 * the PZQ cell.
				 * [0x0]: Disabled[0x1]: Enabled */
	} bits;
} reg_drampub_zq1cr0_t;

#define DRAMPUB_ZQ1CR0_ZDEN_0           0x0
#define DRAMPUB_ZQ1CR0_ZCALBYP_0        0x0
#define DRAMPUB_ZQ1CR0_ZCALEN_0         0x0
#define DRAMPUB_ZQ1CR0_ZQPD_0           0x0

/* ZQ [CH] Impedance Control Register 0 */
typedef union {
	u32 val;
	struct {
	u32 zdata:28;           /* Impedance Over-Ride Data: Data used
				 * to directly drive the impedance control.
				 * ZDATA field mapping for D3F I/Os is
				 * as follows:
				 * ZDATA[27:21] is used to select the pull-up
				 * on-die termination impedance ZDATA[20:14]
				 * is used to select the pull-down on-die
				 * termination impedance ZDATA[13:7] is
				 * used to select the pull-up output impedance
				 * ZDATA[6:0] is used to select the pull-down
				 * output impedance
				 * ZDATA field mapping for D3A/B/R I/Os
				 * is as follows: ZDATA[27:20] is reserved
				 * and returns zeros on reads
				 * ZDATA[19:15] is used to select the pull-up
				 * on-die termination impedance ZDATA[14:10]
				 * is used to select the pull-down on-die
				 * termination impedance ZDATA[9:5] is
				 * used to select the pull-up output impedance
				 * ZDATA[4:0] is used to select the pull-down
				 * output impedance */
	u32 zden:1;             /* Impedance Over-ride Enable: When this
				 * bit is set, it allows users to directly
				 * drive the impedance control using the
				 * data programmed in the ZQDATA field.
				 * Otherwise, the control is generated
				 * automatically by the impedance control logic
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 zcalbyp:1;          /* Impedance Calibration Bypass: Bypasses,
				 * if set, impedance calibration of this
				 * ZQ control block when impedance calibration
				 * is already in progress. Impedance calibration
				 * can be disabled prior to trigger by
				 * using the ZCALEN bit.
				 * [0x0]: not bypass[0x1]: bypass */
	u32 zcalen:1;           /* Impedance Calibration Enable: Enables
				 * if the set the impedance calibration
				 * of this ZQ control block when impedance
				 * calibration is triggered using either
				 * the ZCAL bit of PIR register or the
				 * DFI update interface.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 zqpd:1;             /* ZQ Power Down: Powers down, if set,
				 * the PZQ cell.
				 * [0x0]: Disabled[0x1]: Enabled */
	} bits;
} reg_drampub_zq2cr0_t;

#define DRAMPUB_ZQ2CR0_ZDEN_0           0x0
#define DRAMPUB_ZQ2CR0_ZCALBYP_0        0x0
#define DRAMPUB_ZQ2CR0_ZCALEN_0         0x0
#define DRAMPUB_ZQ2CR0_ZQPD_0           0x0

/* ZQ [CH] Impedance Control Register 0 */
typedef union {
	u32 val;
	struct {
	u32 zdata:28;           /* Impedance Over-Ride Data: Data used
				 * to directly drive the impedance control.
				 * ZDATA field mapping for D3F I/Os is
				 * as follows:
				 * ZDATA[27:21] is used to select the pull-up
				 * on-die termination impedance ZDATA[20:14]
				 * is used to select the pull-down on-die
				 * termination impedance ZDATA[13:7] is
				 * used to select the pull-up output impedance
				 * ZDATA[6:0] is used to select the pull-down
				 * output impedance
				 * ZDATA field mapping for D3A/B/R I/Os
				 * is as follows: ZDATA[27:20] is reserved
				 * and returns zeros on reads
				 * ZDATA[19:15] is used to select the pull-up
				 * on-die termination impedance ZDATA[14:10]
				 * is used to select the pull-down on-die
				 * termination impedance ZDATA[9:5] is
				 * used to select the pull-up output impedance
				 * ZDATA[4:0] is used to select the pull-down
				 * output impedance */
	u32 zden:1;             /* Impedance Over-ride Enable: When this
				 * bit is set, it allows users to directly
				 * drive the impedance control using the
				 * data programmed in the ZQDATA field.
				 * Otherwise, the control is generated
				 * automatically by the impedance control logic
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 zcalbyp:1;          /* Impedance Calibration Bypass: Bypasses,
				 * if set, impedance calibration of this
				 * ZQ control block when impedance calibration
				 * is already in progress. Impedance calibration
				 * can be disabled prior to trigger by
				 * using the ZCALEN bit.
				 * [0x0]: not bypass[0x1]: bypass */
	u32 zcalen:1;           /* Impedance Calibration Enable: Enables
				 * if the set the impedance calibration
				 * of this ZQ control block when impedance
				 * calibration is triggered using either
				 * the ZCAL bit of PIR register or the
				 * DFI update interface.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 zqpd:1;             /* ZQ Power Down: Powers down, if set,
				 * the PZQ cell.
				 * [0x0]: Disabled[0x1]: Enabled */
	} bits;
} reg_drampub_zq3cr0_t;

#define DRAMPUB_ZQ3CR0_ZDEN_0           0x0
#define DRAMPUB_ZQ3CR0_ZCALBYP_0        0x0
#define DRAMPUB_ZQ3CR0_ZCALEN_0         0x0
#define DRAMPUB_ZQ3CR0_ZQPD_0           0x0

/* ZQ [CH] Impedance Control Register 1 */
typedef union {
	u32 val;
	struct {
	u32 zprog:8;            /* Impedance Divide Ratio: Selects the
				 * external resistor divide ratio to be
				 * used to set the output impedance and
				 * the on-die termination as follows:
				 * ZPROG[7:4] = On-die termination divide select
				 * ZPROG[3:0] = Output impedance divide select */
	u32 rsvd0:24;           /* - */
	} bits;
} reg_drampub_zq0cr1_t;

/* ZQ [CH] Impedance Control Register 1 */
typedef union {
	u32 val;
	struct {
	u32 zprog:8;            /* Impedance Divide Ratio: Selects the
				 * external resistor divide ratio to be
				 * used to set the output impedance and
				 * the on-die termination as follows:
				 * ZPROG[7:4] = On-die termination divide select
				 * ZPROG[3:0] = Output impedance divide select */
	u32 rsvd0:24;           /* - */
	} bits;
} reg_drampub_zq1cr1_t;

/* ZQ [CH] Impedance Control Register 1 */
typedef union {
	u32 val;
	struct {
	u32 zprog:8;            /* Impedance Divide Ratio: Selects the
				 * external resistor divide ratio to be
				 * used to set the output impedance and
				 * the on-die termination as follows:
				 * ZPROG[7:4] = On-die termination divide select
				 * ZPROG[3:0] = Output impedance divide select */
	u32 rsvd0:24;           /* - */
	} bits;
} reg_drampub_zq2cr1_t;

/* ZQ [CH] Impedance Control Register 1 */
typedef union {
	u32 val;
	struct {
	u32 zprog:8;            /* Impedance Divide Ratio: Selects the
				 * external resistor divide ratio to be
				 * used to set the output impedance and
				 * the on-die termination as follows:
				 * ZPROG[7:4] = On-die termination divide select
				 * ZPROG[3:0] = Output impedance divide select */
	u32 rsvd0:24;           /* - */
	} bits;
} reg_drampub_zq3cr1_t;

/* ZQ [CH] Impedance Status Register 0 */
typedef union {
	u32 val;
	struct {
	u32 zctrl:28;           /* Impedance Control: Current value of
				 * impedance control.
				 * ZCTRL field mapping for D3F I/Os is
				 * as follows:
				 * ZCTRL[27:21] is used to select the pull-up
				 * on-die termination impedance ZCTRL[20:14]
				 * is used to select the pull-down on-die
				 * termination impedance ZCTRL[13:7] is
				 * used to select the pull-up output impedance
				 * ZCTRL[6:0] is used to select the pull-down
				 * output impedance
				 * ZCTRL field mapping for D3A/B/R I/Os
				 * is as follows: ZCTRL[27:20] is reserved
				 * and returns zeros on reads
				 * ZCTRL[19:15] is used to select the pull-up
				 * on-die termination impedance ZCTRL[14:10]
				 * is used to select the pull-down on-die
				 * termination impedance ZCTRL[9:5] is
				 * used to select the pull-up output impedance
				 * ZCTRL[4:0] is used to select the pull-down
				 * output impedance */
	u32 rsvd0:2;            /* - */
	u32 zerr:1;             /* Impedance Calibration Error: If set,
				 * indicates that there was an error during
				 * impedance calibration. */
	u32 zdone:1;            /* Impedance Calibration Done: Indicates
				 * that impedance calibration has completed. */
	} bits;
} reg_drampub_zq0sr0_t;

/* ZQ [CH] Impedance Status Register 0 */
typedef union {
	u32 val;
	struct {
	u32 zctrl:28;           /* Impedance Control: Current value of
				 * impedance control.
				 * ZCTRL field mapping for D3F I/Os is
				 * as follows:
				 * ZCTRL[27:21] is used to select the pull-up
				 * on-die termination impedance ZCTRL[20:14]
				 * is used to select the pull-down on-die
				 * termination impedance ZCTRL[13:7] is
				 * used to select the pull-up output impedance
				 * ZCTRL[6:0] is used to select the pull-down
				 * output impedance
				 * ZCTRL field mapping for D3A/B/R I/Os
				 * is as follows: ZCTRL[27:20] is reserved
				 * and returns zeros on reads
				 * ZCTRL[19:15] is used to select the pull-up
				 * on-die termination impedance ZCTRL[14:10]
				 * is used to select the pull-down on-die
				 * termination impedance ZCTRL[9:5] is
				 * used to select the pull-up output impedance
				 * ZCTRL[4:0] is used to select the pull-down
				 * output impedance */
	u32 rsvd0:2;            /* - */
	u32 zerr:1;             /* Impedance Calibration Error: If set,
				 * indicates that there was an error during
				 * impedance calibration. */
	u32 zdone:1;            /* Impedance Calibration Done: Indicates
				 * that impedance calibration has completed. */
	} bits;
} reg_drampub_zq1sr0_t;

/* ZQ [CH] Impedance Status Register 0 */
typedef union {
	u32 val;
	struct {
	u32 zctrl:28;           /* Impedance Control: Current value of
				 * impedance control.
				 * ZCTRL field mapping for D3F I/Os is
				 * as follows:
				 * ZCTRL[27:21] is used to select the pull-up
				 * on-die termination impedance ZCTRL[20:14]
				 * is used to select the pull-down on-die
				 * termination impedance ZCTRL[13:7] is
				 * used to select the pull-up output impedance
				 * ZCTRL[6:0] is used to select the pull-down
				 * output impedance
				 * ZCTRL field mapping for D3A/B/R I/Os
				 * is as follows: ZCTRL[27:20] is reserved
				 * and returns zeros on reads
				 * ZCTRL[19:15] is used to select the pull-up
				 * on-die termination impedance ZCTRL[14:10]
				 * is used to select the pull-down on-die
				 * termination impedance ZCTRL[9:5] is
				 * used to select the pull-up output impedance
				 * ZCTRL[4:0] is used to select the pull-down
				 * output impedance */
	u32 rsvd0:2;            /* - */
	u32 zerr:1;             /* Impedance Calibration Error: If set,
				 * indicates that there was an error during
				 * impedance calibration. */
	u32 zdone:1;            /* Impedance Calibration Done: Indicates
				 * that impedance calibration has completed. */
	} bits;
} reg_drampub_zq2sr0_t;

/* ZQ [CH] Impedance Status Register 0 */
typedef union {
	u32 val;
	struct {
	u32 zctrl:28;           /* Impedance Control: Current value of
				 * impedance control.
				 * ZCTRL field mapping for D3F I/Os is
				 * as follows:
				 * ZCTRL[27:21] is used to select the pull-up
				 * on-die termination impedance ZCTRL[20:14]
				 * is used to select the pull-down on-die
				 * termination impedance ZCTRL[13:7] is
				 * used to select the pull-up output impedance
				 * ZCTRL[6:0] is used to select the pull-down
				 * output impedance
				 * ZCTRL field mapping for D3A/B/R I/Os
				 * is as follows: ZCTRL[27:20] is reserved
				 * and returns zeros on reads
				 * ZCTRL[19:15] is used to select the pull-up
				 * on-die termination impedance ZCTRL[14:10]
				 * is used to select the pull-down on-die
				 * termination impedance ZCTRL[9:5] is
				 * used to select the pull-up output impedance
				 * ZCTRL[4:0] is used to select the pull-down
				 * output impedance */
	u32 rsvd0:2;            /* - */
	u32 zerr:1;             /* Impedance Calibration Error: If set,
				 * indicates that there was an error during
				 * impedance calibration. */
	u32 zdone:1;            /* Impedance Calibration Done: Indicates
				 * that impedance calibration has completed. */
	} bits;
} reg_drampub_zq3sr0_t;

/* ZQ [CH] Impedance Status Register 1 */
typedef union {
	u32 val;
	struct {
	u32 zpd:2;              /* Output impedance pull-down calibration
				 * status. Valid status encodings are:
				 * 00 = Completed with no errors
				 * 01 = Overflow error
				 * 10 = Underflow error
				 * 11 = Calibration in progress */
	u32 zpu:2;              /* Output impedance pull-up calibration
				 * status. Similar status encodings as ZPD. */
	u32 opd:2;              /* On-die termination (ODT) pull-down calibration
				 * status. Similar status encodings as ZPD. */
	u32 opu:2;              /* On-die termination (ODT) pull-up calibration
				 * status. Similar status encodings as ZPD. */
	u32 rsvd0:24;           /* - */
	} bits;
} reg_drampub_zq0sr1_t;

/* ZQ [CH] Impedance Status Register 1 */
typedef union {
	u32 val;
	struct {
	u32 zpd:2;              /* Output impedance pull-down calibration
				 * status. Valid status encodings are:
				 * 00 = Completed with no errors
				 * 01 = Overflow error
				 * 10 = Underflow error
				 * 11 = Calibration in progress */
	u32 zpu:2;              /* Output impedance pull-up calibration
				 * status. Similar status encodings as ZPD. */
	u32 opd:2;              /* On-die termination (ODT) pull-down calibration
				 * status. Similar status encodings as ZPD. */
	u32 opu:2;              /* On-die termination (ODT) pull-up calibration
				 * status. Similar status encodings as ZPD. */
	u32 rsvd0:24;           /* - */
	} bits;
} reg_drampub_zq1sr1_t;

/* ZQ [CH] Impedance Status Register 1 */
typedef union {
	u32 val;
	struct {
	u32 zpd:2;              /* Output impedance pull-down calibration
				 * status. Valid status encodings are:
				 * 00 = Completed with no errors
				 * 01 = Overflow error
				 * 10 = Underflow error
				 * 11 = Calibration in progress */
	u32 zpu:2;              /* Output impedance pull-up calibration
				 * status. Similar status encodings as ZPD. */
	u32 opd:2;              /* On-die termination (ODT) pull-down calibration
				 * status. Similar status encodings as ZPD. */
	u32 opu:2;              /* On-die termination (ODT) pull-up calibration
				 * status. Similar status encodings as ZPD. */
	u32 rsvd0:24;           /* - */
	} bits;
} reg_drampub_zq2sr1_t;

/* ZQ [CH] Impedance Status Register 1 */
typedef union {
	u32 val;
	struct {
	u32 zpd:2;              /* Output impedance pull-down calibration
				 * status. Valid status encodings are:
				 * 00 = Completed with no errors
				 * 01 = Overflow error
				 * 10 = Underflow error
				 * 11 = Calibration in progress */
	u32 zpu:2;              /* Output impedance pull-up calibration
				 * status. Similar status encodings as ZPD. */
	u32 opd:2;              /* On-die termination (ODT) pull-down calibration
				 * status. Similar status encodings as ZPD. */
	u32 opu:2;              /* On-die termination (ODT) pull-up calibration
				 * status. Similar status encodings as ZPD. */
	u32 rsvd0:24;           /* - */
	} bits;
} reg_drampub_zq3sr1_t;

/* DATX8 [CH] General Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 dxen:1;             /* Data Byte Enable: Enables if set the
				 * data byte. Setting this bit to '0' disables
				 * the byte, i.e. the byte is not used
				 * in PHY initialization or training and
				 * is ignored during SDRAM read/write oper
				 * ations.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dqsodt:1;           /* DQS On-Die Termination: Enables, when
				 * set, the on-die termination on the I/O
				 * for DQS/DQS# pin of the byte. This bit
				 * is ORed with the common DATX8 ODT configuration
				 * bit
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dqodt:1;            /* Data On-Die Termination: Enables, when
				 * set, the on-die termination on the I/O
				 * for DQ and DM pins of the byte. This
				 * bit is ORed with the common DATX8 ODT
				 * configuration bit
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dxiom:1;            /* Data I/O Mode: Selects SSTL mode (when
				 * set to 0) or CMOS mode (when set to
				 * 1) of the I/O for DQ, DM, and DQS/DQS#
				 * pins of the byte. This bit is ORed with
				 * the IOM configuration bit of the individual
				 * DATX8
				 * [0x0]: SSTL mode[0x1]: CMOS mode */
	u32 dxpdd:1;            /* Data Power Down Driver: Powers down,
				 * when set, the output driver on I/O for
				 * DQ, DM, and DQS/DQS# pins of the byte.
				 * This bit is ORed with the common PDD
				 * configuration bit
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dxpdr:1;            /* Data Power Down Receiver: Powers down,
				 * when set, the input receiver on I/O
				 * for DQ, DM, and DQS/DQS# pins of the
				 * byte. This bit is ORed with the common
				 * PDR configuration bit
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dqsrpd:1;           /* DQSR Power Down: Powers down, if set,
				 * the PDQSR cell. This bit is ORed with
				 * the common PDR configuration bit
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dsen:2;             /* Write DQS Enable: Controls whether the
				 * write DQS going to the SDRAM is enabled
				 * (toggling) or disabled (static value)
				 * and whether the DQS is inverted. DQS#
				 * is always the inversion of DQS. These
				 * values are valid only when DQS/DQS#
				 * output enable is on, otherwise the DQS/DQS#
				 * is tristated. Valid settings are:
				 * [0x0]: DQS disabled (Driven to constant 0)
				 * [0x1]: DQS toggling with inverted polarity
				 * [0x2]: DQS toggling with normal polarity
				 * (This should be the default setting)
				 * [0x3]: DQS disabled (Driven to constant 1) */
	u32 dqsrtt:1;           /* DQS Dynamic RTT Control: Indicates,
				 * if set, that the ODT control of DQS
				 * SSTL I/Os be dynamically controlled
				 * by setting it to the value in DQSODT
				 * during reads and disabling it (setting
				 * it to ¡®0¡¯) during any other cycle.
				 * If this bit is not set, then the ODT
				 * control of DQS SSTL I/Os is always set
				 * to the value in DQSODT field.
				 * [0x0]: always on
				 * [0x1]: dynamic ODT */
	u32 dqrtt:1;            /* DQ Dynamic RTT Control: Indicates, if
				 * set, that the ODT control of DQ/DM SSTL
				 * I/Os be dynamically controlled by setting
				 * it to the value in DQODT during reads
				 * and disabling it (setting it to ¡®0¡¯)
				 * during any other cycle. If this bit
				 * is not set, then the ODT control of
				 * DQ SSTL I/Os is always set to the value
				 * in DQODT.
				 * [0x0]: always on
				 * [0x1]: dynamic ODT */
	u32 rttoh:2;            /* RTT Output Hold: Indicates the number
				 * of clock cycles (from 0 to 3) after
				 * the read data postamble for which ODT
				 * control should remain set to DQSODT
				 * for DQS or DQODT for DQ/DM before disabling
				 * it (setting it to ¡®0¡¯) when using
				 * dynamic ODT control. ODT is disabled
				 * almost RTTOH clock cycles after the
				 * read postamble. */
	u32 rttoal:1;           /* RTT On Additive Latency: Indicates when
				 * the ODT control of DQ/DQS SSTL I/Os
				 * is set to the value in DQODT/DQSODT
				 * during read cycles. Valid values are:
				 * [0x0]:  ODT control is set to DQSODT/DQODT
				 * almost two cycles before read data preamble
				 * [0x1]:  ODT control is set to DQSODT/DQODT
				 * almost one cycle before read data preamble */
	u32 rsvd0:2;            /* - */
	u32 pllrst:1;           /* PLL Rest: Resets the byte PLL by driving
				 * the PLL reset pin. This bit is not self-
				 * clearing and a '0' must be written to
				 * de-assert the reset. This bit is ORed
				 * with the global PLLRST configuration bit.
				 * [0x0]: de-assert[0x1]: reset */
	u32 pllpd:1;            /* PLL Power Down: Puts the byte PLL in
				 * power down mode by driving the PLL power
				 * down pin. This bit is not self-clearing
				 * and a '0' must be written to de-assert
				 * the power-down. This bit is ORed with
				 * the global PLLPD configuration bit.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 gshift:1;           /* Gear Shift: Enables, if set, rapid locking
				 * mode on the byte PLL. This bit is ORed
				 * with the global GSHIFT configuration bit.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 rsvd1:7;            /* - */
	u32 wlrken:4;           /* Write Level Rank Enable: Specifies the
				 * ranks that should be write leveled for
				 * this byte. Write leveling responses
				 * from ranks that are not enabled for
				 * write leveling for a particular byte
				 * are ignored and write leveling is flagged
				 * as done for these ranks. WLRKEN[0] enables
				 * rank 0, [1] enables rank 1, [2] enables
				 * rank 2, and [3] enables rank 3. */
	u32 mdlen:1;            /* Master Delay Line Enable: Enables, if
				 * set, the DATX8 master delay line calibration
				 * to perform subsequent period measurements
				 * following the initial period measurements
				 * that are performed after reset or on
				 * when calibration is manually triggered.
				 * These additional measurements are accumulated
				 * and filtered as long as this bit remains
				 * high. This bit is combined with the
				 * common DATX8 MDL enable bit.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 calbyp:1;           /* Calibration Bypass: Prevents, if set,
				 * period measurement calibration from
				 * automatically triggering after PHY init
				 * ialization.
				 * [0x0]: Disabled[0x1]: Enabled */
	} bits;
} reg_drampub_dx0gcr_t;

#define DRAMPUB_DX0GCR_DXEN_0   0x0
#define DRAMPUB_DX0GCR_DQSODT_0 0x0
#define DRAMPUB_DX0GCR_DQODT_0  0x0
#define DRAMPUB_DX0GCR_DXIOM_0  0x0
#define DRAMPUB_DX0GCR_DXPDD_0  0x0
#define DRAMPUB_DX0GCR_DXPDR_0  0x0
#define DRAMPUB_DX0GCR_DQSRPD_0 0x0
#define DRAMPUB_DX0GCR_DSEN_0   0x0
#define DRAMPUB_DX0GCR_DSEN_1   0x1
#define DRAMPUB_DX0GCR_DSEN_2   0x2
#define DRAMPUB_DX0GCR_DSEN_3   0x3
#define DRAMPUB_DX0GCR_DQSRTT_0 0x0
#define DRAMPUB_DX0GCR_DQSRTT_1 0x1
#define DRAMPUB_DX0GCR_DQRTT_0  0x0
#define DRAMPUB_DX0GCR_DQRTT_1  0x1
#define DRAMPUB_DX0GCR_RTTOAL_0 0x0
#define DRAMPUB_DX0GCR_RTTOAL_1 0x1
#define DRAMPUB_DX0GCR_PLLRST_0 0x0
#define DRAMPUB_DX0GCR_PLLPD_0  0x0
#define DRAMPUB_DX0GCR_GSHIFT_0 0x0
#define DRAMPUB_DX0GCR_MDLEN_0  0x0
#define DRAMPUB_DX0GCR_CALBYP_0 0x0

/* DATX8 [CH] General Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 dxen:1;             /* Data Byte Enable: Enables if set the
				 * data byte. Setting this bit to '0' disables
				 * the byte, i.e. the byte is not used
				 * in PHY initialization or training and
				 * is ignored during SDRAM read/write oper
				 * ations.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dqsodt:1;           /* DQS On-Die Termination: Enables, when
				 * set, the on-die termination on the I/O
				 * for DQS/DQS# pin of the byte. This bit
				 * is ORed with the common DATX8 ODT configuration
				 * bit
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dqodt:1;            /* Data On-Die Termination: Enables, when
				 * set, the on-die termination on the I/O
				 * for DQ and DM pins of the byte. This
				 * bit is ORed with the common DATX8 ODT
				 * configuration bit
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dxiom:1;            /* Data I/O Mode: Selects SSTL mode (when
				 * set to 0) or CMOS mode (when set to
				 * 1) of the I/O for DQ, DM, and DQS/DQS#
				 * pins of the byte. This bit is ORed with
				 * the IOM configuration bit of the individual
				 * DATX8
				 * [0x0]: SSTL mode[0x1]: CMOS mode */
	u32 dxpdd:1;            /* Data Power Down Driver: Powers down,
				 * when set, the output driver on I/O for
				 * DQ, DM, and DQS/DQS# pins of the byte.
				 * This bit is ORed with the common PDD
				 * configuration bit
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dxpdr:1;            /* Data Power Down Receiver: Powers down,
				 * when set, the input receiver on I/O
				 * for DQ, DM, and DQS/DQS# pins of the
				 * byte. This bit is ORed with the common
				 * PDR configuration bit
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dqsrpd:1;           /* DQSR Power Down: Powers down, if set,
				 * the PDQSR cell. This bit is ORed with
				 * the common PDR configuration bit
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dsen:2;             /* Write DQS Enable: Controls whether the
				 * write DQS going to the SDRAM is enabled
				 * (toggling) or disabled (static value)
				 * and whether the DQS is inverted. DQS#
				 * is always the inversion of DQS. These
				 * values are valid only when DQS/DQS#
				 * output enable is on, otherwise the DQS/DQS#
				 * is tristated. Valid settings are:
				 * [0x0]: DQS disabled (Driven to constant 0)
				 * [0x1]: DQS toggling with inverted polarity
				 * [0x2]: DQS toggling with normal polarity
				 * (This should be the default setting)
				 * [0x3]: DQS disabled (Driven to constant 1) */
	u32 dqsrtt:1;           /* DQS Dynamic RTT Control: Indicates,
				 * if set, that the ODT control of DQS
				 * SSTL I/Os be dynamically controlled
				 * by setting it to the value in DQSODT
				 * during reads and disabling it (setting
				 * it to ¡®0¡¯) during any other cycle.
				 * If this bit is not set, then the ODT
				 * control of DQS SSTL I/Os is always set
				 * to the value in DQSODT field.
				 * [0x0]: always on
				 * [0x1]: dynamic ODT */
	u32 dqrtt:1;            /* DQ Dynamic RTT Control: Indicates, if
				 * set, that the ODT control of DQ/DM SSTL
				 * I/Os be dynamically controlled by setting
				 * it to the value in DQODT during reads
				 * and disabling it (setting it to ¡®0¡¯)
				 * during any other cycle. If this bit
				 * is not set, then the ODT control of
				 * DQ SSTL I/Os is always set to the value
				 * in DQODT.
				 * [0x0]: always on
				 * [0x1]: dynamic ODT */
	u32 rttoh:2;            /* RTT Output Hold: Indicates the number
				 * of clock cycles (from 0 to 3) after
				 * the read data postamble for which ODT
				 * control should remain set to DQSODT
				 * for DQS or DQODT for DQ/DM before disabling
				 * it (setting it to ¡®0¡¯) when using
				 * dynamic ODT control. ODT is disabled
				 * almost RTTOH clock cycles after the
				 * read postamble. */
	u32 rttoal:1;           /* RTT On Additive Latency: Indicates when
				 * the ODT control of DQ/DQS SSTL I/Os
				 * is set to the value in DQODT/DQSODT
				 * during read cycles. Valid values are:
				 * [0x0]:  ODT control is set to DQSODT/DQODT
				 * almost two cycles before read data preamble
				 * [0x1]:  ODT control is set to DQSODT/DQODT
				 * almost one cycle before read data preamble */
	u32 rsvd0:2;            /* - */
	u32 pllrst:1;           /* PLL Rest: Resets the byte PLL by driving
				 * the PLL reset pin. This bit is not self-
				 * clearing and a '0' must be written to
				 * de-assert the reset. This bit is ORed
				 * with the global PLLRST configuration bit.
				 * [0x0]: de-assert[0x1]: reset */
	u32 pllpd:1;            /* PLL Power Down: Puts the byte PLL in
				 * power down mode by driving the PLL power
				 * down pin. This bit is not self-clearing
				 * and a '0' must be written to de-assert
				 * the power-down. This bit is ORed with
				 * the global PLLPD configuration bit.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 gshift:1;           /* Gear Shift: Enables, if set, rapid locking
				 * mode on the byte PLL. This bit is ORed
				 * with the global GSHIFT configuration bit.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 rsvd1:7;            /* - */
	u32 wlrken:4;           /* Write Level Rank Enable: Specifies the
				 * ranks that should be write leveled for
				 * this byte. Write leveling responses
				 * from ranks that are not enabled for
				 * write leveling for a particular byte
				 * are ignored and write leveling is flagged
				 * as done for these ranks. WLRKEN[0] enables
				 * rank 0, [1] enables rank 1, [2] enables
				 * rank 2, and [3] enables rank 3. */
	u32 mdlen:1;            /* Master Delay Line Enable: Enables, if
				 * set, the DATX8 master delay line calibration
				 * to perform subsequent period measurements
				 * following the initial period measurements
				 * that are performed after reset or on
				 * when calibration is manually triggered.
				 * These additional measurements are accumulated
				 * and filtered as long as this bit remains
				 * high. This bit is combined with the
				 * common DATX8 MDL enable bit.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 calbyp:1;           /* Calibration Bypass: Prevents, if set,
				 * period measurement calibration from
				 * automatically triggering after PHY init
				 * ialization.
				 * [0x0]: Disabled[0x1]: Enabled */
	} bits;
} reg_drampub_dx1gcr_t;

#define DRAMPUB_DX1GCR_DXEN_0   0x0
#define DRAMPUB_DX1GCR_DQSODT_0 0x0
#define DRAMPUB_DX1GCR_DQODT_0  0x0
#define DRAMPUB_DX1GCR_DXIOM_0  0x0
#define DRAMPUB_DX1GCR_DXPDD_0  0x0
#define DRAMPUB_DX1GCR_DXPDR_0  0x0
#define DRAMPUB_DX1GCR_DQSRPD_0 0x0
#define DRAMPUB_DX1GCR_DSEN_0   0x0
#define DRAMPUB_DX1GCR_DSEN_1   0x1
#define DRAMPUB_DX1GCR_DSEN_2   0x2
#define DRAMPUB_DX1GCR_DSEN_3   0x3
#define DRAMPUB_DX1GCR_DQSRTT_0 0x0
#define DRAMPUB_DX1GCR_DQSRTT_1 0x1
#define DRAMPUB_DX1GCR_DQRTT_0  0x0
#define DRAMPUB_DX1GCR_DQRTT_1  0x1
#define DRAMPUB_DX1GCR_RTTOAL_0 0x0
#define DRAMPUB_DX1GCR_RTTOAL_1 0x1
#define DRAMPUB_DX1GCR_PLLRST_0 0x0
#define DRAMPUB_DX1GCR_PLLPD_0  0x0
#define DRAMPUB_DX1GCR_GSHIFT_0 0x0
#define DRAMPUB_DX1GCR_MDLEN_0  0x0
#define DRAMPUB_DX1GCR_CALBYP_0 0x0

/* DATX8 [CH] General Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 dxen:1;             /* Data Byte Enable: Enables if set the
				 * data byte. Setting this bit to '0' disables
				 * the byte, i.e. the byte is not used
				 * in PHY initialization or training and
				 * is ignored during SDRAM read/write oper
				 * ations.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dqsodt:1;           /* DQS On-Die Termination: Enables, when
				 * set, the on-die termination on the I/O
				 * for DQS/DQS# pin of the byte. This bit
				 * is ORed with the common DATX8 ODT configuration
				 * bit
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dqodt:1;            /* Data On-Die Termination: Enables, when
				 * set, the on-die termination on the I/O
				 * for DQ and DM pins of the byte. This
				 * bit is ORed with the common DATX8 ODT
				 * configuration bit
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dxiom:1;            /* Data I/O Mode: Selects SSTL mode (when
				 * set to 0) or CMOS mode (when set to
				 * 1) of the I/O for DQ, DM, and DQS/DQS#
				 * pins of the byte. This bit is ORed with
				 * the IOM configuration bit of the individual
				 * DATX8
				 * [0x0]: SSTL mode[0x1]: CMOS mode */
	u32 dxpdd:1;            /* Data Power Down Driver: Powers down,
				 * when set, the output driver on I/O for
				 * DQ, DM, and DQS/DQS# pins of the byte.
				 * This bit is ORed with the common PDD
				 * configuration bit
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dxpdr:1;            /* Data Power Down Receiver: Powers down,
				 * when set, the input receiver on I/O
				 * for DQ, DM, and DQS/DQS# pins of the
				 * byte. This bit is ORed with the common
				 * PDR configuration bit
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dqsrpd:1;           /* DQSR Power Down: Powers down, if set,
				 * the PDQSR cell. This bit is ORed with
				 * the common PDR configuration bit
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dsen:2;             /* Write DQS Enable: Controls whether the
				 * write DQS going to the SDRAM is enabled
				 * (toggling) or disabled (static value)
				 * and whether the DQS is inverted. DQS#
				 * is always the inversion of DQS. These
				 * values are valid only when DQS/DQS#
				 * output enable is on, otherwise the DQS/DQS#
				 * is tristated. Valid settings are:
				 * [0x0]: DQS disabled (Driven to constant 0)
				 * [0x1]: DQS toggling with inverted polarity
				 * [0x2]: DQS toggling with normal polarity
				 * (This should be the default setting)
				 * [0x3]: DQS disabled (Driven to constant 1) */
	u32 dqsrtt:1;           /* DQS Dynamic RTT Control: Indicates,
				 * if set, that the ODT control of DQS
				 * SSTL I/Os be dynamically controlled
				 * by setting it to the value in DQSODT
				 * during reads and disabling it (setting
				 * it to ¡®0¡¯) during any other cycle.
				 * If this bit is not set, then the ODT
				 * control of DQS SSTL I/Os is always set
				 * to the value in DQSODT field.
				 * [0x0]: always on
				 * [0x1]: dynamic ODT */
	u32 dqrtt:1;            /* DQ Dynamic RTT Control: Indicates, if
				 * set, that the ODT control of DQ/DM SSTL
				 * I/Os be dynamically controlled by setting
				 * it to the value in DQODT during reads
				 * and disabling it (setting it to ¡®0¡¯)
				 * during any other cycle. If this bit
				 * is not set, then the ODT control of
				 * DQ SSTL I/Os is always set to the value
				 * in DQODT.
				 * [0x0]: always on
				 * [0x1]: dynamic ODT */
	u32 rttoh:2;            /* RTT Output Hold: Indicates the number
				 * of clock cycles (from 0 to 3) after
				 * the read data postamble for which ODT
				 * control should remain set to DQSODT
				 * for DQS or DQODT for DQ/DM before disabling
				 * it (setting it to ¡®0¡¯) when using
				 * dynamic ODT control. ODT is disabled
				 * almost RTTOH clock cycles after the
				 * read postamble. */
	u32 rttoal:1;           /* RTT On Additive Latency: Indicates when
				 * the ODT control of DQ/DQS SSTL I/Os
				 * is set to the value in DQODT/DQSODT
				 * during read cycles. Valid values are:
				 * [0x0]:  ODT control is set to DQSODT/DQODT
				 * almost two cycles before read data preamble
				 * [0x1]:  ODT control is set to DQSODT/DQODT
				 * almost one cycle before read data preamble */
	u32 rsvd0:2;            /* - */
	u32 pllrst:1;           /* PLL Rest: Resets the byte PLL by driving
				 * the PLL reset pin. This bit is not self-
				 * clearing and a '0' must be written to
				 * de-assert the reset. This bit is ORed
				 * with the global PLLRST configuration bit.
				 * [0x0]: de-assert[0x1]: reset */
	u32 pllpd:1;            /* PLL Power Down: Puts the byte PLL in
				 * power down mode by driving the PLL power
				 * down pin. This bit is not self-clearing
				 * and a '0' must be written to de-assert
				 * the power-down. This bit is ORed with
				 * the global PLLPD configuration bit.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 gshift:1;           /* Gear Shift: Enables, if set, rapid locking
				 * mode on the byte PLL. This bit is ORed
				 * with the global GSHIFT configuration bit.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 rsvd1:7;            /* - */
	u32 wlrken:4;           /* Write Level Rank Enable: Specifies the
				 * ranks that should be write leveled for
				 * this byte. Write leveling responses
				 * from ranks that are not enabled for
				 * write leveling for a particular byte
				 * are ignored and write leveling is flagged
				 * as done for these ranks. WLRKEN[0] enables
				 * rank 0, [1] enables rank 1, [2] enables
				 * rank 2, and [3] enables rank 3. */
	u32 mdlen:1;            /* Master Delay Line Enable: Enables, if
				 * set, the DATX8 master delay line calibration
				 * to perform subsequent period measurements
				 * following the initial period measurements
				 * that are performed after reset or on
				 * when calibration is manually triggered.
				 * These additional measurements are accumulated
				 * and filtered as long as this bit remains
				 * high. This bit is combined with the
				 * common DATX8 MDL enable bit.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 calbyp:1;           /* Calibration Bypass: Prevents, if set,
				 * period measurement calibration from
				 * automatically triggering after PHY init
				 * ialization.
				 * [0x0]: Disabled[0x1]: Enabled */
	} bits;
} reg_drampub_dx2gcr_t;

#define DRAMPUB_DX2GCR_DXEN_0   0x0
#define DRAMPUB_DX2GCR_DQSODT_0 0x0
#define DRAMPUB_DX2GCR_DQODT_0  0x0
#define DRAMPUB_DX2GCR_DXIOM_0  0x0
#define DRAMPUB_DX2GCR_DXPDD_0  0x0
#define DRAMPUB_DX2GCR_DXPDR_0  0x0
#define DRAMPUB_DX2GCR_DQSRPD_0 0x0
#define DRAMPUB_DX2GCR_DSEN_0   0x0
#define DRAMPUB_DX2GCR_DSEN_1   0x1
#define DRAMPUB_DX2GCR_DSEN_2   0x2
#define DRAMPUB_DX2GCR_DSEN_3   0x3
#define DRAMPUB_DX2GCR_DQSRTT_0 0x0
#define DRAMPUB_DX2GCR_DQSRTT_1 0x1
#define DRAMPUB_DX2GCR_DQRTT_0  0x0
#define DRAMPUB_DX2GCR_DQRTT_1  0x1
#define DRAMPUB_DX2GCR_RTTOAL_0 0x0
#define DRAMPUB_DX2GCR_RTTOAL_1 0x1
#define DRAMPUB_DX2GCR_PLLRST_0 0x0
#define DRAMPUB_DX2GCR_PLLPD_0  0x0
#define DRAMPUB_DX2GCR_GSHIFT_0 0x0
#define DRAMPUB_DX2GCR_MDLEN_0  0x0
#define DRAMPUB_DX2GCR_CALBYP_0 0x0

/* DATX8 [CH] General Configuration Register */
typedef union {
	u32 val;
	struct {
	u32 dxen:1;             /* Data Byte Enable: Enables if set the
				 * data byte. Setting this bit to '0' disables
				 * the byte, i.e. the byte is not used
				 * in PHY initialization or training and
				 * is ignored during SDRAM read/write oper
				 * ations.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dqsodt:1;           /* DQS On-Die Termination: Enables, when
				 * set, the on-die termination on the I/O
				 * for DQS/DQS# pin of the byte. This bit
				 * is ORed with the common DATX8 ODT configuration
				 * bit
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dqodt:1;            /* Data On-Die Termination: Enables, when
				 * set, the on-die termination on the I/O
				 * for DQ and DM pins of the byte. This
				 * bit is ORed with the common DATX8 ODT
				 * configuration bit
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dxiom:1;            /* Data I/O Mode: Selects SSTL mode (when
				 * set to 0) or CMOS mode (when set to
				 * 1) of the I/O for DQ, DM, and DQS/DQS#
				 * pins of the byte. This bit is ORed with
				 * the IOM configuration bit of the individual
				 * DATX8
				 * [0x0]: SSTL mode[0x1]: CMOS mode */
	u32 dxpdd:1;            /* Data Power Down Driver: Powers down,
				 * when set, the output driver on I/O for
				 * DQ, DM, and DQS/DQS# pins of the byte.
				 * This bit is ORed with the common PDD
				 * configuration bit
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dxpdr:1;            /* Data Power Down Receiver: Powers down,
				 * when set, the input receiver on I/O
				 * for DQ, DM, and DQS/DQS# pins of the
				 * byte. This bit is ORed with the common
				 * PDR configuration bit
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dqsrpd:1;           /* DQSR Power Down: Powers down, if set,
				 * the PDQSR cell. This bit is ORed with
				 * the common PDR configuration bit
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 dsen:2;             /* Write DQS Enable: Controls whether the
				 * write DQS going to the SDRAM is enabled
				 * (toggling) or disabled (static value)
				 * and whether the DQS is inverted. DQS#
				 * is always the inversion of DQS. These
				 * values are valid only when DQS/DQS#
				 * output enable is on, otherwise the DQS/DQS#
				 * is tristated. Valid settings are:
				 * [0x0]: DQS disabled (Driven to constant 0)
				 * [0x1]: DQS toggling with inverted polarity
				 * [0x2]: DQS toggling with normal polarity
				 * (This should be the default setting)
				 * [0x3]: DQS disabled (Driven to constant 1) */
	u32 dqsrtt:1;           /* DQS Dynamic RTT Control: Indicates,
				 * if set, that the ODT control of DQS
				 * SSTL I/Os be dynamically controlled
				 * by setting it to the value in DQSODT
				 * during reads and disabling it (setting
				 * it to ¡®0¡¯) during any other cycle.
				 * If this bit is not set, then the ODT
				 * control of DQS SSTL I/Os is always set
				 * to the value in DQSODT field.
				 * [0x0]: always on
				 * [0x1]: dynamic ODT */
	u32 dqrtt:1;            /* DQ Dynamic RTT Control: Indicates, if
				 * set, that the ODT control of DQ/DM SSTL
				 * I/Os be dynamically controlled by setting
				 * it to the value in DQODT during reads
				 * and disabling it (setting it to ¡®0¡¯)
				 * during any other cycle. If this bit
				 * is not set, then the ODT control of
				 * DQ SSTL I/Os is always set to the value
				 * in DQODT.
				 * [0x0]: always on
				 * [0x1]: dynamic ODT */
	u32 rttoh:2;            /* RTT Output Hold: Indicates the number
				 * of clock cycles (from 0 to 3) after
				 * the read data postamble for which ODT
				 * control should remain set to DQSODT
				 * for DQS or DQODT for DQ/DM before disabling
				 * it (setting it to ¡®0¡¯) when using
				 * dynamic ODT control. ODT is disabled
				 * almost RTTOH clock cycles after the
				 * read postamble. */
	u32 rttoal:1;           /* RTT On Additive Latency: Indicates when
				 * the ODT control of DQ/DQS SSTL I/Os
				 * is set to the value in DQODT/DQSODT
				 * during read cycles. Valid values are:
				 * [0x0]:  ODT control is set to DQSODT/DQODT
				 * almost two cycles before read data preamble
				 * [0x1]:  ODT control is set to DQSODT/DQODT
				 * almost one cycle before read data preamble */
	u32 rsvd0:2;            /* - */
	u32 pllrst:1;           /* PLL Rest: Resets the byte PLL by driving
				 * the PLL reset pin. This bit is not self-
				 * clearing and a '0' must be written to
				 * de-assert the reset. This bit is ORed
				 * with the global PLLRST configuration bit.
				 * [0x0]: de-assert[0x1]: reset */
	u32 pllpd:1;            /* PLL Power Down: Puts the byte PLL in
				 * power down mode by driving the PLL power
				 * down pin. This bit is not self-clearing
				 * and a '0' must be written to de-assert
				 * the power-down. This bit is ORed with
				 * the global PLLPD configuration bit.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 gshift:1;           /* Gear Shift: Enables, if set, rapid locking
				 * mode on the byte PLL. This bit is ORed
				 * with the global GSHIFT configuration bit.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 rsvd1:7;            /* - */
	u32 wlrken:4;           /* Write Level Rank Enable: Specifies the
				 * ranks that should be write leveled for
				 * this byte. Write leveling responses
				 * from ranks that are not enabled for
				 * write leveling for a particular byte
				 * are ignored and write leveling is flagged
				 * as done for these ranks. WLRKEN[0] enables
				 * rank 0, [1] enables rank 1, [2] enables
				 * rank 2, and [3] enables rank 3. */
	u32 mdlen:1;            /* Master Delay Line Enable: Enables, if
				 * set, the DATX8 master delay line calibration
				 * to perform subsequent period measurements
				 * following the initial period measurements
				 * that are performed after reset or on
				 * when calibration is manually triggered.
				 * These additional measurements are accumulated
				 * and filtered as long as this bit remains
				 * high. This bit is combined with the
				 * common DATX8 MDL enable bit.
				 * [0x0]: Disabled[0x1]: Enabled */
	u32 calbyp:1;           /* Calibration Bypass: Prevents, if set,
				 * period measurement calibration from
				 * automatically triggering after PHY init
				 * ialization.
				 * [0x0]: Disabled[0x1]: Enabled */
	} bits;
} reg_drampub_dx3gcr_t;

#define DRAMPUB_DX3GCR_DXEN_0   0x0
#define DRAMPUB_DX3GCR_DQSODT_0 0x0
#define DRAMPUB_DX3GCR_DQODT_0  0x0
#define DRAMPUB_DX3GCR_DXIOM_0  0x0
#define DRAMPUB_DX3GCR_DXPDD_0  0x0
#define DRAMPUB_DX3GCR_DXPDR_0  0x0
#define DRAMPUB_DX3GCR_DQSRPD_0 0x0
#define DRAMPUB_DX3GCR_DSEN_0   0x0
#define DRAMPUB_DX3GCR_DSEN_1   0x1
#define DRAMPUB_DX3GCR_DSEN_2   0x2
#define DRAMPUB_DX3GCR_DSEN_3   0x3
#define DRAMPUB_DX3GCR_DQSRTT_0 0x0
#define DRAMPUB_DX3GCR_DQSRTT_1 0x1
#define DRAMPUB_DX3GCR_DQRTT_0  0x0
#define DRAMPUB_DX3GCR_DQRTT_1  0x1
#define DRAMPUB_DX3GCR_RTTOAL_0 0x0
#define DRAMPUB_DX3GCR_RTTOAL_1 0x1
#define DRAMPUB_DX3GCR_PLLRST_0 0x0
#define DRAMPUB_DX3GCR_PLLPD_0  0x0
#define DRAMPUB_DX3GCR_GSHIFT_0 0x0
#define DRAMPUB_DX3GCR_MDLEN_0  0x0
#define DRAMPUB_DX3GCR_CALBYP_0 0x0

/* DATX8 [CH] General Status Registers 0 */
typedef union {
	u32 val;
	struct {
	u32 wdqcal:1;           /* Write DQ Calibration: Indicates, if
				 * set, that the DATX8 has finished doing
				 * period measurement calibration for the
				 * write DQ LCDL. */
	u32 rdqscal:1;          /* Read DQS Calibration: Indicates, if
				 * set, that the DATX8 has finished doing
				 * period measurement calibration for the
				 * read DQS LCDL. */
	u32 rdqsncal:1;         /* Read DQS# Calibration (Type B/B1 PHY
				 * Only): Indicates, if set, that the DATX8
				 * has finished doing period measurement
				 * calibration for the read DQS# LCDL. */
	u32 gdqscal:1;          /* Read DQS gating Calibration: Indicates,
				 * if set, that the DATX8 has finished
				 * doing period measurement calibration
				 * for the read DQS gating LCDL. */
	u32 wlcal:1;            /* Write Leveling Calibration: Indicates,
				 * if set, that the DATX8 has finished
				 * doing period measurement calibration
				 * for the write leveling slave delay line. */
	u32 wldone:1;           /* Write Leveling Done: Indicates, if set,
				 * that the DATX8 has completed write leveling. */
	u32 wlerr:1;            /* Write Leveling Error: Indicates, if
				 * set, that there is a write leveling
				 * error in the
				 * DATX8. */
	u32 wlprd:8;            /* Write Leveling Period: Returns the DDR
				 * clock period measured by the write leveling
				 * LCDL during calibration. The measured
				 * period is used to generate the control
				 * of the write leveling pipeline which
				 * is a function of the write-leveling
				 * delay and the clock period. This value
				 * is PVT compensated. */
	u32 dplock:1;           /* DATX8 PLL Lock: Indicates, if set, that
				 * the DATX8 PLL has locked. This is a
				 * direct status of the DATX8 PLL lock pin. */
	u32 gdqsprd:8;          /* Read DQS gating Period: Returns the
				 * DDR clock period measured by the read
				 * DQS gating LCDL during calibration.
				 * This value is PVT compensated. */
	u32 qsgerr:4;           /* DQS Gate Training Error: Indicates if
				 * set that there is an error in DQS gate
				 * training. One bit for each of the up
				 * to 4 ranks. */
	u32 weyewn:1;           /* Write Eye Warning: Indicates if set
				 * that the Write Eye algorithm is unable
				 * to find the right or left edge of the
				 * data eye. The result is that the centering
				 * of the dqs in the data eye may not be
				 * in the optimal position. */
	u32 wbdwn:1;            /* Indicates if set that the Write Bit
				 * Deskew algorithm is unable to add enough
				 * delay using the WDQ LCDL and DQ bdls
				 * to find the left edge of the data eye.
				 * The result is that the write bit deskewing
				 * of the data bits will not be fully de-skewed. */
	u32 dterr:1;            /* Data Training Error: Indicates if set
				 * that the byte encountered a data training
				 * error */
	u32 rsvd0:1;            /* - */
	} bits;
} reg_drampub_dx0gsr0_t;

/* DATX8 [CH] General Status Registers 0 */
typedef union {
	u32 val;
	struct {
	u32 wdqcal:1;           /* Write DQ Calibration: Indicates, if
				 * set, that the DATX8 has finished doing
				 * period measurement calibration for the
				 * write DQ LCDL. */
	u32 rdqscal:1;          /* Read DQS Calibration: Indicates, if
				 * set, that the DATX8 has finished doing
				 * period measurement calibration for the
				 * read DQS LCDL. */
	u32 rdqsncal:1;         /* Read DQS# Calibration (Type B/B1 PHY
				 * Only): Indicates, if set, that the DATX8
				 * has finished doing period measurement
				 * calibration for the read DQS# LCDL. */
	u32 gdqscal:1;          /* Read DQS gating Calibration: Indicates,
				 * if set, that the DATX8 has finished
				 * doing period measurement calibration
				 * for the read DQS gating LCDL. */
	u32 wlcal:1;            /* Write Leveling Calibration: Indicates,
				 * if set, that the DATX8 has finished
				 * doing period measurement calibration
				 * for the write leveling slave delay line. */
	u32 wldone:1;           /* Write Leveling Done: Indicates, if set,
				 * that the DATX8 has completed write leveling. */
	u32 wlerr:1;            /* Write Leveling Error: Indicates, if
				 * set, that there is a write leveling
				 * error in the
				 * DATX8. */
	u32 wlprd:8;            /* Write Leveling Period: Returns the DDR
				 * clock period measured by the write leveling
				 * LCDL during calibration. The measured
				 * period is used to generate the control
				 * of the write leveling pipeline which
				 * is a function of the write-leveling
				 * delay and the clock period. This value
				 * is PVT compensated. */
	u32 dplock:1;           /* DATX8 PLL Lock: Indicates, if set, that
				 * the DATX8 PLL has locked. This is a
				 * direct status of the DATX8 PLL lock pin. */
	u32 gdqsprd:8;          /* Read DQS gating Period: Returns the
				 * DDR clock period measured by the read
				 * DQS gating LCDL during calibration.
				 * This value is PVT compensated. */
	u32 qsgerr:4;           /* DQS Gate Training Error: Indicates if
				 * set that there is an error in DQS gate
				 * training. One bit for each of the up
				 * to 4 ranks. */
	u32 weyewn:1;           /* Write Eye Warning: Indicates if set
				 * that the Write Eye algorithm is unable
				 * to find the right or left edge of the
				 * data eye. The result is that the centering
				 * of the dqs in the data eye may not be
				 * in the optimal position. */
	u32 wbdwn:1;            /* Indicates if set that the Write Bit
				 * Deskew algorithm is unable to add enough
				 * delay using the WDQ LCDL and DQ bdls
				 * to find the left edge of the data eye.
				 * The result is that the write bit deskewing
				 * of the data bits will not be fully de-skewed. */
	u32 dterr:1;            /* Data Training Error: Indicates if set
				 * that the byte encountered a data training
				 * error */
	u32 rsvd0:1;            /* - */
	} bits;
} reg_drampub_dx1gsr0_t;

/* DATX8 [CH] General Status Registers 0 */
typedef union {
	u32 val;
	struct {
	u32 wdqcal:1;           /* Write DQ Calibration: Indicates, if
				 * set, that the DATX8 has finished doing
				 * period measurement calibration for the
				 * write DQ LCDL. */
	u32 rdqscal:1;          /* Read DQS Calibration: Indicates, if
				 * set, that the DATX8 has finished doing
				 * period measurement calibration for the
				 * read DQS LCDL. */
	u32 rdqsncal:1;         /* Read DQS# Calibration (Type B/B1 PHY
				 * Only): Indicates, if set, that the DATX8
				 * has finished doing period measurement
				 * calibration for the read DQS# LCDL. */
	u32 gdqscal:1;          /* Read DQS gating Calibration: Indicates,
				 * if set, that the DATX8 has finished
				 * doing period measurement calibration
				 * for the read DQS gating LCDL. */
	u32 wlcal:1;            /* Write Leveling Calibration: Indicates,
				 * if set, that the DATX8 has finished
				 * doing period measurement calibration
				 * for the write leveling slave delay line. */
	u32 wldone:1;           /* Write Leveling Done: Indicates, if set,
				 * that the DATX8 has completed write leveling. */
	u32 wlerr:1;            /* Write Leveling Error: Indicates, if
				 * set, that there is a write leveling
				 * error in the
				 * DATX8. */
	u32 wlprd:8;            /* Write Leveling Period: Returns the DDR
				 * clock period measured by the write leveling
				 * LCDL during calibration. The measured
				 * period is used to generate the control
				 * of the write leveling pipeline which
				 * is a function of the write-leveling
				 * delay and the clock period. This value
				 * is PVT compensated. */
	u32 dplock:1;           /* DATX8 PLL Lock: Indicates, if set, that
				 * the DATX8 PLL has locked. This is a
				 * direct status of the DATX8 PLL lock pin. */
	u32 gdqsprd:8;          /* Read DQS gating Period: Returns the
				 * DDR clock period measured by the read
				 * DQS gating LCDL during calibration.
				 * This value is PVT compensated. */
	u32 qsgerr:4;           /* DQS Gate Training Error: Indicates if
				 * set that there is an error in DQS gate
				 * training. One bit for each of the up
				 * to 4 ranks. */
	u32 weyewn:1;           /* Write Eye Warning: Indicates if set
				 * that the Write Eye algorithm is unable
				 * to find the right or left edge of the
				 * data eye. The result is that the centering
				 * of the dqs in the data eye may not be
				 * in the optimal position. */
	u32 wbdwn:1;            /* Indicates if set that the Write Bit
				 * Deskew algorithm is unable to add enough
				 * delay using the WDQ LCDL and DQ bdls
				 * to find the left edge of the data eye.
				 * The result is that the write bit deskewing
				 * of the data bits will not be fully de-skewed. */
	u32 dterr:1;            /* Data Training Error: Indicates if set
				 * that the byte encountered a data training
				 * error */
	u32 rsvd0:1;            /* - */
	} bits;
} reg_drampub_dx2gsr0_t;

/* DATX8 [CH] General Status Registers 0 */
typedef union {
	u32 val;
	struct {
	u32 wdqcal:1;           /* Write DQ Calibration: Indicates, if
				 * set, that the DATX8 has finished doing
				 * period measurement calibration for the
				 * write DQ LCDL. */
	u32 rdqscal:1;          /* Read DQS Calibration: Indicates, if
				 * set, that the DATX8 has finished doing
				 * period measurement calibration for the
				 * read DQS LCDL. */
	u32 rdqsncal:1;         /* Read DQS# Calibration (Type B/B1 PHY
				 * Only): Indicates, if set, that the DATX8
				 * has finished doing period measurement
				 * calibration for the read DQS# LCDL. */
	u32 gdqscal:1;          /* Read DQS gating Calibration: Indicates,
				 * if set, that the DATX8 has finished
				 * doing period measurement calibration
				 * for the read DQS gating LCDL. */
	u32 wlcal:1;            /* Write Leveling Calibration: Indicates,
				 * if set, that the DATX8 has finished
				 * doing period measurement calibration
				 * for the write leveling slave delay line. */
	u32 wldone:1;           /* Write Leveling Done: Indicates, if set,
				 * that the DATX8 has completed write leveling. */
	u32 wlerr:1;            /* Write Leveling Error: Indicates, if
				 * set, that there is a write leveling
				 * error in the
				 * DATX8. */
	u32 wlprd:8;            /* Write Leveling Period: Returns the DDR
				 * clock period measured by the write leveling
				 * LCDL during calibration. The measured
				 * period is used to generate the control
				 * of the write leveling pipeline which
				 * is a function of the write-leveling
				 * delay and the clock period. This value
				 * is PVT compensated. */
	u32 dplock:1;           /* DATX8 PLL Lock: Indicates, if set, that
				 * the DATX8 PLL has locked. This is a
				 * direct status of the DATX8 PLL lock pin. */
	u32 gdqsprd:8;          /* Read DQS gating Period: Returns the
				 * DDR clock period measured by the read
				 * DQS gating LCDL during calibration.
				 * This value is PVT compensated. */
	u32 qsgerr:4;           /* DQS Gate Training Error: Indicates if
				 * set that there is an error in DQS gate
				 * training. One bit for each of the up
				 * to 4 ranks. */
	u32 weyewn:1;           /* Write Eye Warning: Indicates if set
				 * that the Write Eye algorithm is unable
				 * to find the right or left edge of the
				 * data eye. The result is that the centering
				 * of the dqs in the data eye may not be
				 * in the optimal position. */
	u32 wbdwn:1;            /* Indicates if set that the Write Bit
				 * Deskew algorithm is unable to add enough
				 * delay using the WDQ LCDL and DQ bdls
				 * to find the left edge of the data eye.
				 * The result is that the write bit deskewing
				 * of the data bits will not be fully de-skewed. */
	u32 dterr:1;            /* Data Training Error: Indicates if set
				 * that the byte encountered a data training
				 * error */
	u32 rsvd0:1;            /* - */
	} bits;
} reg_drampub_dx3gsr0_t;

/* DATX8 [CH] General Status Registers 1 */
typedef union {
	u32 val;
	struct {
	u32 dltdone:1;          /* Delay Line Test Done: Indicates, if
				 * set, that the PHY control block has
				 * finished doing period measurement of
				 * the DATX8 delay line digital test output. */
	u32 dltcode:24;         /* Delay Line Test Code: Returns the code
				 * measured by the PHY control block that
				 * corresponds to the period of the DATX8
				 * delay line digital test output. */
	u32 rsvd0:7;            /* - */
	} bits;
} reg_drampub_dx0gsr1_t;

/* DATX8 [CH] General Status Registers 1 */
typedef union {
	u32 val;
	struct {
	u32 dltdone:1;          /* Delay Line Test Done: Indicates, if
				 * set, that the PHY control block has
				 * finished doing period measurement of
				 * the DATX8 delay line digital test output. */
	u32 dltcode:24;         /* Delay Line Test Code: Returns the code
				 * measured by the PHY control block that
				 * corresponds to the period of the DATX8
				 * delay line digital test output. */
	u32 rsvd0:7;            /* - */
	} bits;
} reg_drampub_dx1gsr1_t;

/* DATX8 [CH] General Status Registers 1 */
typedef union {
	u32 val;
	struct {
	u32 dltdone:1;          /* Delay Line Test Done: Indicates, if
				 * set, that the PHY control block has
				 * finished doing period measurement of
				 * the DATX8 delay line digital test output. */
	u32 dltcode:24;         /* Delay Line Test Code: Returns the code
				 * measured by the PHY control block that
				 * corresponds to the period of the DATX8
				 * delay line digital test output. */
	u32 rsvd0:7;            /* - */
	} bits;
} reg_drampub_dx2gsr1_t;

/* DATX8 [CH] General Status Registers 1 */
typedef union {
	u32 val;
	struct {
	u32 dltdone:1;          /* Delay Line Test Done: Indicates, if
				 * set, that the PHY control block has
				 * finished doing period measurement of
				 * the DATX8 delay line digital test output. */
	u32 dltcode:24;         /* Delay Line Test Code: Returns the code
				 * measured by the PHY control block that
				 * corresponds to the period of the DATX8
				 * delay line digital test output. */
	u32 rsvd0:7;            /* - */
	} bits;
} reg_drampub_dx3gsr1_t;

/* DATX8 [CH] Bit Delay Line Registers 0 */
typedef union {
	u32 val;
	struct {
	u32 dq0wbd:6;           /* DQ0 Write Bit Delay: Delay select for
				 * the BDL on DQ0 write path. */
	u32 dq1wbd:6;           /* DQ1 Write Bit Delay: Delay select for
				 * the BDL on DQ1 write path. */
	u32 dq2wbd:6;           /* DQ2 Write Bit Delay: Delay select for
				 * the BDL on DQ2 write path. */
	u32 dq3wbd:6;           /* DQ3 Write Bit Delay: Delay select for
				 * the BDL on DQ3 write path */
	u32 dq4wbd:6;           /* DQ4 Write Bit Delay: Delay select for
				 * the BDL on DQ4 write path. */
	u32 rsvd0:2;            /* - */
	} bits;
} reg_drampub_dx0bdlr0_t;

/* DATX8 [CH] Bit Delay Line Registers 0 */
typedef union {
	u32 val;
	struct {
	u32 dq0wbd:6;           /* DQ0 Write Bit Delay: Delay select for
				 * the BDL on DQ0 write path. */
	u32 dq1wbd:6;           /* DQ1 Write Bit Delay: Delay select for
				 * the BDL on DQ1 write path. */
	u32 dq2wbd:6;           /* DQ2 Write Bit Delay: Delay select for
				 * the BDL on DQ2 write path. */
	u32 dq3wbd:6;           /* DQ3 Write Bit Delay: Delay select for
				 * the BDL on DQ3 write path */
	u32 dq4wbd:6;           /* DQ4 Write Bit Delay: Delay select for
				 * the BDL on DQ4 write path. */
	u32 rsvd0:2;            /* - */
	} bits;
} reg_drampub_dx1bdlr0_t;

/* DATX8 [CH] Bit Delay Line Registers 0 */
typedef union {
	u32 val;
	struct {
	u32 dq0wbd:6;           /* DQ0 Write Bit Delay: Delay select for
				 * the BDL on DQ0 write path. */
	u32 dq1wbd:6;           /* DQ1 Write Bit Delay: Delay select for
				 * the BDL on DQ1 write path. */
	u32 dq2wbd:6;           /* DQ2 Write Bit Delay: Delay select for
				 * the BDL on DQ2 write path. */
	u32 dq3wbd:6;           /* DQ3 Write Bit Delay: Delay select for
				 * the BDL on DQ3 write path */
	u32 dq4wbd:6;           /* DQ4 Write Bit Delay: Delay select for
				 * the BDL on DQ4 write path. */
	u32 rsvd0:2;            /* - */
	} bits;
} reg_drampub_dx2bdlr0_t;

/* DATX8 [CH] Bit Delay Line Registers 0 */
typedef union {
	u32 val;
	struct {
	u32 dq0wbd:6;           /* DQ0 Write Bit Delay: Delay select for
				 * the BDL on DQ0 write path. */
	u32 dq1wbd:6;           /* DQ1 Write Bit Delay: Delay select for
				 * the BDL on DQ1 write path. */
	u32 dq2wbd:6;           /* DQ2 Write Bit Delay: Delay select for
				 * the BDL on DQ2 write path. */
	u32 dq3wbd:6;           /* DQ3 Write Bit Delay: Delay select for
				 * the BDL on DQ3 write path */
	u32 dq4wbd:6;           /* DQ4 Write Bit Delay: Delay select for
				 * the BDL on DQ4 write path. */
	u32 rsvd0:2;            /* - */
	} bits;
} reg_drampub_dx3bdlr0_t;

/* DATX8 [CH] Bit Delay Line Registers 1 */
typedef union {
	u32 val;
	struct {
	u32 dq5wbd:6;           /* DQ5 Write Bit Delay: Delay select for
				 * the BDL on DQ5 write path. */
	u32 dq6wbd:6;           /* DQ6 Write Bit Delay: Delay select for
				 * the BDL on DQ6 write path. */
	u32 dq7wbd:6;           /* DQ7 Write Bit Delay: Delay select for
				 * the BDL on DQ7 write path. */
	u32 dmwbd:6;            /* DM Write Bit Delay: Delay select for
				 * the BDL on DM write path. */
	u32 dswbd:6;            /* DQS Write Bit Delay: Delay select for
				 * the BDL on DQS write path */
	u32 rsvd0:2;            /* - */
	} bits;
} reg_drampub_dx0bdlr1_t;

/* DATX8 [CH] Bit Delay Line Registers 1 */
typedef union {
	u32 val;
	struct {
	u32 dq5wbd:6;           /* DQ5 Write Bit Delay: Delay select for
				 * the BDL on DQ5 write path. */
	u32 dq6wbd:6;           /* DQ6 Write Bit Delay: Delay select for
				 * the BDL on DQ6 write path. */
	u32 dq7wbd:6;           /* DQ7 Write Bit Delay: Delay select for
				 * the BDL on DQ7 write path. */
	u32 dmwbd:6;            /* DM Write Bit Delay: Delay select for
				 * the BDL on DM write path. */
	u32 dswbd:6;            /* DQS Write Bit Delay: Delay select for
				 * the BDL on DQS write path */
	u32 rsvd0:2;            /* - */
	} bits;
} reg_drampub_dx1bdlr1_t;

/* DATX8 [CH] Bit Delay Line Registers 1 */
typedef union {
	u32 val;
	struct {
	u32 dq5wbd:6;           /* DQ5 Write Bit Delay: Delay select for
				 * the BDL on DQ5 write path. */
	u32 dq6wbd:6;           /* DQ6 Write Bit Delay: Delay select for
				 * the BDL on DQ6 write path. */
	u32 dq7wbd:6;           /* DQ7 Write Bit Delay: Delay select for
				 * the BDL on DQ7 write path. */
	u32 dmwbd:6;            /* DM Write Bit Delay: Delay select for
				 * the BDL on DM write path. */
	u32 dswbd:6;            /* DQS Write Bit Delay: Delay select for
				 * the BDL on DQS write path */
	u32 rsvd0:2;            /* - */
	} bits;
} reg_drampub_dx2bdlr1_t;

/* DATX8 [CH] Bit Delay Line Registers 1 */
typedef union {
	u32 val;
	struct {
	u32 dq5wbd:6;           /* DQ5 Write Bit Delay: Delay select for
				 * the BDL on DQ5 write path. */
	u32 dq6wbd:6;           /* DQ6 Write Bit Delay: Delay select for
				 * the BDL on DQ6 write path. */
	u32 dq7wbd:6;           /* DQ7 Write Bit Delay: Delay select for
				 * the BDL on DQ7 write path. */
	u32 dmwbd:6;            /* DM Write Bit Delay: Delay select for
				 * the BDL on DM write path. */
	u32 dswbd:6;            /* DQS Write Bit Delay: Delay select for
				 * the BDL on DQS write path */
	u32 rsvd0:2;            /* - */
	} bits;
} reg_drampub_dx3bdlr1_t;

/* DATX8 [CH] Bit Delay Line Registers 2 */
typedef union {
	u32 val;
	struct {
	u32 dsoebd:6;           /* DQS Output Enable Bit Delay: Delay select
				 * for the BDL on DQS output enable path */
	u32 dqoebd:6;           /* DQ Output Enable Bit Delay: Delay select
				 * for the BDL on DQ/DM output enable path. */
	u32 dsrbd:6;            /* DQS Read Bit Delay: Delay select for
				 * the BDL on DQS read path */
	u32 dsnrbd:6;           /* DQSN Read Bit Delay (Type B/B1 PHY Only):
				 * Delay select for the BDL on DQSN
				 * read path */
	u32 rsvd0:8;            /* - */
	} bits;
} reg_drampub_dx0bdlr2_t;

/* DATX8 [CH] Bit Delay Line Registers 2 */
typedef union {
	u32 val;
	struct {
	u32 dsoebd:6;           /* DQS Output Enable Bit Delay: Delay select
				 * for the BDL on DQS output enable path */
	u32 dqoebd:6;           /* DQ Output Enable Bit Delay: Delay select
				 * for the BDL on DQ/DM output enable path. */
	u32 dsrbd:6;            /* DQS Read Bit Delay: Delay select for
				 * the BDL on DQS read path */
	u32 dsnrbd:6;           /* DQSN Read Bit Delay (Type B/B1 PHY Only):
				 * Delay select for the BDL on DQSN
				 * read path */
	u32 rsvd0:8;            /* - */
	} bits;
} reg_drampub_dx1bdlr2_t;

/* DATX8 [CH] Bit Delay Line Registers 2 */
typedef union {
	u32 val;
	struct {
	u32 dsoebd:6;           /* DQS Output Enable Bit Delay: Delay select
				 * for the BDL on DQS output enable path */
	u32 dqoebd:6;           /* DQ Output Enable Bit Delay: Delay select
				 * for the BDL on DQ/DM output enable path. */
	u32 dsrbd:6;            /* DQS Read Bit Delay: Delay select for
				 * the BDL on DQS read path */
	u32 dsnrbd:6;           /* DQSN Read Bit Delay (Type B/B1 PHY Only):
				 * Delay select for the BDL on DQSN
				 * read path */
	u32 rsvd0:8;            /* - */
	} bits;
} reg_drampub_dx2bdlr2_t;

/* DATX8 [CH] Bit Delay Line Registers 2 */
typedef union {
	u32 val;
	struct {
	u32 dsoebd:6;           /* DQS Output Enable Bit Delay: Delay select
				 * for the BDL on DQS output enable path */
	u32 dqoebd:6;           /* DQ Output Enable Bit Delay: Delay select
				 * for the BDL on DQ/DM output enable path. */
	u32 dsrbd:6;            /* DQS Read Bit Delay: Delay select for
				 * the BDL on DQS read path */
	u32 dsnrbd:6;           /* DQSN Read Bit Delay (Type B/B1 PHY Only):
				 * Delay select for the BDL on DQSN
				 * read path */
	u32 rsvd0:8;            /* - */
	} bits;
} reg_drampub_dx3bdlr2_t;

/* DATX8 [CH] Bit Delay Line Registers 3 */
typedef union {
	u32 val;
	struct {
	u32 dq0rbd:6;           /* DQ0 Read Bit Delay: Delay select for
				 * the BDL on DQ0 read path. */
	u32 dq1rbd:6;           /* DQ1 Read Bit Delay: Delay select for
				 * the BDL on DQ1 read path. */
	u32 dq2rbd:6;           /* DQ2 Read Bit Delay: Delay select for
				 * the BDL on DQ2 read path. */
	u32 dq3rbd:6;           /* DQ3 Read Bit Delay: Delay select for
				 * the BDL on DQ3 read path */
	u32 dq4rbd:6;           /* DQ4 Read Bit Delay: Delay select for
				 * the BDL on DQ4 read path. */
	u32 rsvd0:2;            /* - */
	} bits;
} reg_drampub_dx0bdlr3_t;

/* DATX8 [CH] Bit Delay Line Registers 3 */
typedef union {
	u32 val;
	struct {
	u32 dq0rbd:6;           /* DQ0 Read Bit Delay: Delay select for
				 * the BDL on DQ0 read path. */
	u32 dq1rbd:6;           /* DQ1 Read Bit Delay: Delay select for
				 * the BDL on DQ1 read path. */
	u32 dq2rbd:6;           /* DQ2 Read Bit Delay: Delay select for
				 * the BDL on DQ2 read path. */
	u32 dq3rbd:6;           /* DQ3 Read Bit Delay: Delay select for
				 * the BDL on DQ3 read path */
	u32 dq4rbd:6;           /* DQ4 Read Bit Delay: Delay select for
				 * the BDL on DQ4 read path. */
	u32 rsvd0:2;            /* - */
	} bits;
} reg_drampub_dx1bdlr3_t;

/* DATX8 [CH] Bit Delay Line Registers 3 */
typedef union {
	u32 val;
	struct {
	u32 dq0rbd:6;           /* DQ0 Read Bit Delay: Delay select for
				 * the BDL on DQ0 read path. */
	u32 dq1rbd:6;           /* DQ1 Read Bit Delay: Delay select for
				 * the BDL on DQ1 read path. */
	u32 dq2rbd:6;           /* DQ2 Read Bit Delay: Delay select for
				 * the BDL on DQ2 read path. */
	u32 dq3rbd:6;           /* DQ3 Read Bit Delay: Delay select for
				 * the BDL on DQ3 read path */
	u32 dq4rbd:6;           /* DQ4 Read Bit Delay: Delay select for
				 * the BDL on DQ4 read path. */
	u32 rsvd0:2;            /* - */
	} bits;
} reg_drampub_dx2bdlr3_t;

/* DATX8 [CH] Bit Delay Line Registers 3 */
typedef union {
	u32 val;
	struct {
	u32 dq0rbd:6;           /* DQ0 Read Bit Delay: Delay select for
				 * the BDL on DQ0 read path. */
	u32 dq1rbd:6;           /* DQ1 Read Bit Delay: Delay select for
				 * the BDL on DQ1 read path. */
	u32 dq2rbd:6;           /* DQ2 Read Bit Delay: Delay select for
				 * the BDL on DQ2 read path. */
	u32 dq3rbd:6;           /* DQ3 Read Bit Delay: Delay select for
				 * the BDL on DQ3 read path */
	u32 dq4rbd:6;           /* DQ4 Read Bit Delay: Delay select for
				 * the BDL on DQ4 read path. */
	u32 rsvd0:2;            /* - */
	} bits;
} reg_drampub_dx3bdlr3_t;

/* DATX8 [CH] Bit Delay Line Registers 4 */
typedef union {
	u32 val;
	struct {
	u32 dq5rbd:6;           /* DQ5 Read Bit Delay: Delay select for
				 * the BDL on DQ5 read path. */
	u32 dq6rbd:6;           /* DQ6 Read Bit Delay: Delay select for
				 * the BDL on DQ6 read path. */
	u32 dq7rbd:6;           /* DQ7 Read Bit Delay: Delay select for
				 * the BDL on DQ7 read path. */
	u32 dmrbd:6;            /* DM Read Bit Delay: Delay select for
				 * the BDL on DM read path. */
	u32 rsvd0:8;            /* - */
	} bits;
} reg_drampub_dx0bdlr4_t;

/* DATX8 [CH] Bit Delay Line Registers 4 */
typedef union {
	u32 val;
	struct {
	u32 dq5rbd:6;           /* DQ5 Read Bit Delay: Delay select for
				 * the BDL on DQ5 read path. */
	u32 dq6rbd:6;           /* DQ6 Read Bit Delay: Delay select for
				 * the BDL on DQ6 read path. */
	u32 dq7rbd:6;           /* DQ7 Read Bit Delay: Delay select for
				 * the BDL on DQ7 read path. */
	u32 dmrbd:6;            /* DM Read Bit Delay: Delay select for
				 * the BDL on DM read path. */
	u32 rsvd0:8;            /* - */
	} bits;
} reg_drampub_dx1bdlr4_t;

/* DATX8 [CH] Bit Delay Line Registers 4 */
typedef union {
	u32 val;
	struct {
	u32 dq5rbd:6;           /* DQ5 Read Bit Delay: Delay select for
				 * the BDL on DQ5 read path. */
	u32 dq6rbd:6;           /* DQ6 Read Bit Delay: Delay select for
				 * the BDL on DQ6 read path. */
	u32 dq7rbd:6;           /* DQ7 Read Bit Delay: Delay select for
				 * the BDL on DQ7 read path. */
	u32 dmrbd:6;            /* DM Read Bit Delay: Delay select for
				 * the BDL on DM read path. */
	u32 rsvd0:8;            /* - */
	} bits;
} reg_drampub_dx2bdlr4_t;

/* DATX8 [CH] Bit Delay Line Registers 4 */
typedef union {
	u32 val;
	struct {
	u32 dq5rbd:6;           /* DQ5 Read Bit Delay: Delay select for
				 * the BDL on DQ5 read path. */
	u32 dq6rbd:6;           /* DQ6 Read Bit Delay: Delay select for
				 * the BDL on DQ6 read path. */
	u32 dq7rbd:6;           /* DQ7 Read Bit Delay: Delay select for
				 * the BDL on DQ7 read path. */
	u32 dmrbd:6;            /* DM Read Bit Delay: Delay select for
				 * the BDL on DM read path. */
	u32 rsvd0:8;            /* - */
	} bits;
} reg_drampub_dx3bdlr4_t;

/* DATX8 [CH] Local Calibrated Delay Line Registers 0 */
typedef union {
	u32 val;
	struct {
	u32 r0wld:8;            /* Rank 0 Write Leveling Delay: Rank 0
				 * delay select for the write leveling
				 * (WL) LCDL. */
	u32 r1wld:8;            /* Rank 1 Write Leveling Delay: Rank 1
				 * delay select for the write leveling
				 * (WL) LCDL. */
	u32 r2wld:8;            /* Rank 2 Write Leveling Delay: Rank 2
				 * delay select for the write leveling
				 * (WL) LCDL. */
	u32 r3wld:8;            /* Rank 3 Write Leveling Delay: Rank 3
				 * delay select for the write leveling
				 * (WL) LCDL. */
	} bits;
} reg_drampub_dx0lcdlr0_t;

/* DATX8 [CH] Local Calibrated Delay Line Registers 0 */
typedef union {
	u32 val;
	struct {
	u32 r0wld:8;            /* Rank 0 Write Leveling Delay: Rank 0
				 * delay select for the write leveling
				 * (WL) LCDL. */
	u32 r1wld:8;            /* Rank 1 Write Leveling Delay: Rank 1
				 * delay select for the write leveling
				 * (WL) LCDL. */
	u32 r2wld:8;            /* Rank 2 Write Leveling Delay: Rank 2
				 * delay select for the write leveling
				 * (WL) LCDL. */
	u32 r3wld:8;            /* Rank 3 Write Leveling Delay: Rank 3
				 * delay select for the write leveling
				 * (WL) LCDL. */
	} bits;
} reg_drampub_dx1lcdlr0_t;

/* DATX8 [CH] Local Calibrated Delay Line Registers 0 */
typedef union {
	u32 val;
	struct {
	u32 r0wld:8;            /* Rank 0 Write Leveling Delay: Rank 0
				 * delay select for the write leveling
				 * (WL) LCDL. */
	u32 r1wld:8;            /* Rank 1 Write Leveling Delay: Rank 1
				 * delay select for the write leveling
				 * (WL) LCDL. */
	u32 r2wld:8;            /* Rank 2 Write Leveling Delay: Rank 2
				 * delay select for the write leveling
				 * (WL) LCDL. */
	u32 r3wld:8;            /* Rank 3 Write Leveling Delay: Rank 3
				 * delay select for the write leveling
				 * (WL) LCDL. */
	} bits;
} reg_drampub_dx2lcdlr0_t;

/* DATX8 [CH] Local Calibrated Delay Line Registers 0 */
typedef union {
	u32 val;
	struct {
	u32 r0wld:8;            /* Rank 0 Write Leveling Delay: Rank 0
				 * delay select for the write leveling
				 * (WL) LCDL. */
	u32 r1wld:8;            /* Rank 1 Write Leveling Delay: Rank 1
				 * delay select for the write leveling
				 * (WL) LCDL. */
	u32 r2wld:8;            /* Rank 2 Write Leveling Delay: Rank 2
				 * delay select for the write leveling
				 * (WL) LCDL. */
	u32 r3wld:8;            /* Rank 3 Write Leveling Delay: Rank 3
				 * delay select for the write leveling
				 * (WL) LCDL. */
	} bits;
} reg_drampub_dx3lcdlr0_t;

/* DATX8 [CH] Local Calibrated Delay Line Registers 1 */
typedef union {
	u32 val;
	struct {
	u32 wdqd:8;             /* Write Data Delay: Delay select for the
				 * write data (WDQ) LCDL. */
	u32 rdqsd:8;            /* Read DQS Delay: Delay select for the
				 * read DQS (RDQS) LCDL. */
	u32 rdqsnd:8;           /* Read DQSN Delay: Delay select for the
				 * read DQSN (RDQS) LCDL. */
	u32 rsvd0:8;            /* - */
	} bits;
} reg_drampub_dx0lcdlr1_t;

/* DATX8 [CH] Local Calibrated Delay Line Registers 1 */
typedef union {
	u32 val;
	struct {
	u32 wdqd:8;             /* Write Data Delay: Delay select for the
				 * write data (WDQ) LCDL. */
	u32 rdqsd:8;            /* Read DQS Delay: Delay select for the
				 * read DQS (RDQS) LCDL. */
	u32 rdqsnd:8;           /* Read DQSN Delay: Delay select for the
				 * read DQSN (RDQS) LCDL. */
	u32 rsvd0:8;            /* - */
	} bits;
} reg_drampub_dx1lcdlr1_t;

/* DATX8 [CH] Local Calibrated Delay Line Registers 1 */
typedef union {
	u32 val;
	struct {
	u32 wdqd:8;             /* Write Data Delay: Delay select for the
				 * write data (WDQ) LCDL. */
	u32 rdqsd:8;            /* Read DQS Delay: Delay select for the
				 * read DQS (RDQS) LCDL. */
	u32 rdqsnd:8;           /* Read DQSN Delay: Delay select for the
				 * read DQSN (RDQS) LCDL. */
	u32 rsvd0:8;            /* - */
	} bits;
} reg_drampub_dx2lcdlr1_t;

/* DATX8 [CH] Local Calibrated Delay Line Registers 1 */
typedef union {
	u32 val;
	struct {
	u32 wdqd:8;             /* Write Data Delay: Delay select for the
				 * write data (WDQ) LCDL. */
	u32 rdqsd:8;            /* Read DQS Delay: Delay select for the
				 * read DQS (RDQS) LCDL. */
	u32 rdqsnd:8;           /* Read DQSN Delay: Delay select for the
				 * read DQSN (RDQS) LCDL. */
	u32 rsvd0:8;            /* - */
	} bits;
} reg_drampub_dx3lcdlr1_t;

/* DATX8 [CH] Local Calibrated Delay Line Registers 2 */
typedef union {
	u32 val;
	struct {
	u32 r0dqsgd:8;          /* Rank 0 Read DQS Gating Delay: Rank 0
				 * delay select for the read DQS gating
				 * (DQSG) LCDL. */
	u32 r1dqsgd:8;          /* Rank 1 Read DQS Gating Delay: Rank 1
				 * delay select for the read DQS gating
				 * (DQSG) LCDL. */
	u32 r2dqsgd:8;          /* Rank 2 Read DQS Gating Delay: Rank 2
				 * delay select for the read DQS gating
				 * (DQSG) LCDL. */
	u32 r3dqsgd:8;          /* Rank 3 Read DQS Gating Delay: Rank 3
				 * delay select for the read DQS gating
				 * (DQSG) LCDL. */
	} bits;
} reg_drampub_dx0lcdlr2_t;

/* DATX8 [CH] Local Calibrated Delay Line Registers 2 */
typedef union {
	u32 val;
	struct {
	u32 r0dqsgd:8;          /* Rank 0 Read DQS Gating Delay: Rank 0
				 * delay select for the read DQS gating
				 * (DQSG) LCDL. */
	u32 r1dqsgd:8;          /* Rank 1 Read DQS Gating Delay: Rank 1
				 * delay select for the read DQS gating
				 * (DQSG) LCDL. */
	u32 r2dqsgd:8;          /* Rank 2 Read DQS Gating Delay: Rank 2
				 * delay select for the read DQS gating
				 * (DQSG) LCDL. */
	u32 r3dqsgd:8;          /* Rank 3 Read DQS Gating Delay: Rank 3
				 * delay select for the read DQS gating
				 * (DQSG) LCDL. */
	} bits;
} reg_drampub_dx1lcdlr2_t;

/* DATX8 [CH] Local Calibrated Delay Line Registers 2 */
typedef union {
	u32 val;
	struct {
	u32 r0dqsgd:8;          /* Rank 0 Read DQS Gating Delay: Rank 0
				 * delay select for the read DQS gating
				 * (DQSG) LCDL. */
	u32 r1dqsgd:8;          /* Rank 1 Read DQS Gating Delay: Rank 1
				 * delay select for the read DQS gating
				 * (DQSG) LCDL. */
	u32 r2dqsgd:8;          /* Rank 2 Read DQS Gating Delay: Rank 2
				 * delay select for the read DQS gating
				 * (DQSG) LCDL. */
	u32 r3dqsgd:8;          /* Rank 3 Read DQS Gating Delay: Rank 3
				 * delay select for the read DQS gating
				 * (DQSG) LCDL. */
	} bits;
} reg_drampub_dx2lcdlr2_t;

/* DATX8 [CH] Local Calibrated Delay Line Registers 2 */
typedef union {
	u32 val;
	struct {
	u32 r0dqsgd:8;          /* Rank 0 Read DQS Gating Delay: Rank 0
				 * delay select for the read DQS gating
				 * (DQSG) LCDL. */
	u32 r1dqsgd:8;          /* Rank 1 Read DQS Gating Delay: Rank 1
				 * delay select for the read DQS gating
				 * (DQSG) LCDL. */
	u32 r2dqsgd:8;          /* Rank 2 Read DQS Gating Delay: Rank 2
				 * delay select for the read DQS gating
				 * (DQSG) LCDL. */
	u32 r3dqsgd:8;          /* Rank 3 Read DQS Gating Delay: Rank 3
				 * delay select for the read DQS gating
				 * (DQSG) LCDL. */
	} bits;
} reg_drampub_dx3lcdlr2_t;

/* DATX8 [CH] Master Delay Line Register */
typedef union {
	u32 val;
	struct {
	u32 iprd:8;             /* Initial Period: Initial period measured
				 * by the master delay line calibration
				 * for VT drift compensation. This value
				 * is used as the denominator when calculating
				 * the ratios of updates during VT compensation. */
	u32 tprd:8;             /* Target Period: Target period measured
				 * by the master delay line calibration
				 * for VT drift compensation. This is the
				 * current measured value of the period
				 * and is continuously updated if the MDL
				 * is enabled to do so. */
	u32 mdld:8;             /* MDL Delay: Delay select for the LCDL
				 * for the Master Delay Line. */
	u32 rsvd0:8;            /* - */
	} bits;
} reg_drampub_dx0mdlr_t;

/* DATX8 [CH] Master Delay Line Register */
typedef union {
	u32 val;
	struct {
	u32 iprd:8;             /* Initial Period: Initial period measured
				 * by the master delay line calibration
				 * for VT drift compensation. This value
				 * is used as the denominator when calculating
				 * the ratios of updates during VT compensation. */
	u32 tprd:8;             /* Target Period: Target period measured
				 * by the master delay line calibration
				 * for VT drift compensation. This is the
				 * current measured value of the period
				 * and is continuously updated if the MDL
				 * is enabled to do so. */
	u32 mdld:8;             /* MDL Delay: Delay select for the LCDL
				 * for the Master Delay Line. */
	u32 rsvd0:8;            /* - */
	} bits;
} reg_drampub_dx1mdlr_t;

/* DATX8 [CH] Master Delay Line Register */
typedef union {
	u32 val;
	struct {
	u32 iprd:8;             /* Initial Period: Initial period measured
				 * by the master delay line calibration
				 * for VT drift compensation. This value
				 * is used as the denominator when calculating
				 * the ratios of updates during VT compensation. */
	u32 tprd:8;             /* Target Period: Target period measured
				 * by the master delay line calibration
				 * for VT drift compensation. This is the
				 * current measured value of the period
				 * and is continuously updated if the MDL
				 * is enabled to do so. */
	u32 mdld:8;             /* MDL Delay: Delay select for the LCDL
				 * for the Master Delay Line. */
	u32 rsvd0:8;            /* - */
	} bits;
} reg_drampub_dx2mdlr_t;

/* DATX8 [CH] Master Delay Line Register */
typedef union {
	u32 val;
	struct {
	u32 iprd:8;             /* Initial Period: Initial period measured
				 * by the master delay line calibration
				 * for VT drift compensation. This value
				 * is used as the denominator when calculating
				 * the ratios of updates during VT compensation. */
	u32 tprd:8;             /* Target Period: Target period measured
				 * by the master delay line calibration
				 * for VT drift compensation. This is the
				 * current measured value of the period
				 * and is continuously updated if the MDL
				 * is enabled to do so. */
	u32 mdld:8;             /* MDL Delay: Delay select for the LCDL
				 * for the Master Delay Line. */
	u32 rsvd0:8;            /* - */
	} bits;
} reg_drampub_dx3mdlr_t;

/* DATX8 [CH] General Timing Register */
typedef union {
	u32 val;
	struct {
	u32 r0dgsl:3;           /* Rank 0 DQS Gating System Latency: This
				 * is used to increase the number of clock
				 * cycles needed to expect valid DDR read
				 * data by up to seven extra clock cycles.
				 * This is used to compensate for board
				 * delays and other system delays. Power-up
				 * default is 000 (i.e. no extra clock
				 * cycles required). The SL fields are
				 * initially set by the PUB during automatic
				 * DQS data training but these values can
				 * be overwritten by a direct write to
				 * this register. Every three bits of this
				 * register control the latency of each
				 * of the (up to) four ranks. R0DGSL controls
				 * the latency of rank 0, R1DGSL controls
				 * rank 1, and so on. Valid values are 0 to 7: */
	u32 r1dgsl:3;           /* Rank 1 DQS Gating System Latency: This
				 * is used to increase the number of clock
				 * cycles needed to expect valid DDR read
				 * data by up to seven extra clock cycles.
				 * This is used to compensate for board
				 * delays and other system delays. Power-up
				 * default is 000 (i.e. no extra clock
				 * cycles required). The SL fields are
				 * initially set by the PUB during automatic
				 * DQS data training but these values can
				 * be overwritten by a direct write to
				 * this register. Every three bits of this
				 * register control the latency of each
				 * of the (up to) four ranks. R0DGSL controls
				 * the latency of rank 0, R1DGSL controls
				 * rank 1, and so on. Valid values are 0 to 7: */
	u32 r2dgsl:3;           /* Rank 2 DQS Gating System Latency: This
				 * is used to increase the number of clock
				 * cycles needed to expect valid DDR read
				 * data by up to seven extra clock cycles.
				 * This is used to compensate for board
				 * delays and other system delays. Power-up
				 * default is 000 (i.e. no extra clock
				 * cycles required). The SL fields are
				 * initially set by the PUB during automatic
				 * DQS data training but these values can
				 * be overwritten by a direct write to
				 * this register. Every three bits of this
				 * register control the latency of each
				 * of the (up to) four ranks. R0DGSL controls
				 * the latency of rank 0, R1DGSL controls
				 * rank 1, and so on. Valid values are 0 to 7: */
	u32 r3dgsl:3;           /* Rank 3 DQS Gating System Latency: This
				 * is used to increase the number of clock
				 * cycles needed to expect valid DDR read
				 * data by up to seven extra clock cycles.
				 * This is used to compensate for board
				 * delays and other system delays. Power-up
				 * default is 000 (i.e. no extra clock
				 * cycles required). The SL fields are
				 * initially set by the PUB during automatic
				 * DQS data training but these values can
				 * be overwritten by a direct write to
				 * this register. Every three bits of this
				 * register control the latency of each
				 * of the (up to) four ranks. R0DGSL controls
				 * the latency of rank 0, R1DGSL controls
				 * rank 1, and so on. Valid values are 0 to 7: */
	u32 r0wlsl:2;           /* Rank 0 Write Leveling System Latency:
				 * This is used to adjust the write latency
				 * after write leveling. Power-up default
				 * is 01 (i.e. no extra clock cycles required).
				 * The SL fields are initially set by the
				 * PUB during automatic write leveling
				 * but these values can be overwritten
				 * by a direct write to this register.
				 * Every two bits of this register control
				 * the latency of each of the (up to) four
				 * ranks. R0WLSL controls the latency of
				 * rank 0, R1WLSL controls rank 1, and
				 * so on. Valid values:
				 * 00 = Write latency = WL - 1
				 * 01 = Write latency = WL
				 * 10 = Write latency = WL + 1
				 * 11 = Reserved */
	u32 r1wlsl:2;           /* Rank 1 Write Leveling System Latency:
				 * This is used to adjust the write latency
				 * after write leveling. Power-up default
				 * is 01 (i.e. no extra clock cycles required).
				 * The SL fields are initially set by the
				 * PUB during automatic write leveling
				 * but these values can be overwritten
				 * by a direct write to this register.
				 * Every two bits of this register control
				 * the latency of each of the (up to) four
				 * ranks. R0WLSL controls the latency of
				 * rank 0, R1WLSL controls rank 1, and
				 * so on. Valid values:
				 * 00 = Write latency = WL - 1
				 * 01 = Write latency = WL
				 * 10 = Write latency = WL + 1
				 * 11 = Reserved */
	u32 r2wlsl:2;           /* Rank 2 Write Leveling System Latency:
				 * This is used to adjust the write latency
				 * after write leveling. Power-up default
				 * is 01 (i.e. no extra clock cycles required).
				 * The SL fields are initially set by the
				 * PUB during automatic write leveling
				 * but these values can be overwritten
				 * by a direct write to this register.
				 * Every two bits of this register control
				 * the latency of each of the (up to) four
				 * ranks. R0WLSL controls the latency of
				 * rank 0, R1WLSL controls rank 1, and
				 * so on. Valid values:
				 * 00 = Write latency = WL - 1
				 * 01 = Write latency = WL
				 * 10 = Write latency = WL + 1
				 * 11 = Reserved */
	u32 r3wlsl:2;           /* Rank 3 Write Leveling System Latency:
				 * This is used to adjust the write latency
				 * after write leveling. Power-up default
				 * is 01 (i.e. no extra clock cycles required).
				 * The SL fields are initially set by the
				 * PUB during automatic write leveling
				 * but these values can be overwritten
				 * by a direct write to this register.
				 * Every two bits of this register control
				 * the latency of each of the (up to) four
				 * ranks. R0WLSL controls the latency of
				 * rank 0, R1WLSL controls rank 1, and
				 * so on. Valid values:
				 * 00 = Write latency = WL - 1
				 * 01 = Write latency = WL
				 * 10 = Write latency = WL + 1
				 * 11 = Reserved */
	u32 rsvd0:12;           /* - */
	} bits;
} reg_drampub_dx0gtr_t;

/* DATX8 [CH] General Timing Register */
typedef union {
	u32 val;
	struct {
	u32 r0dgsl:3;           /* Rank 0 DQS Gating System Latency: This
				 * is used to increase the number of clock
				 * cycles needed to expect valid DDR read
				 * data by up to seven extra clock cycles.
				 * This is used to compensate for board
				 * delays and other system delays. Power-up
				 * default is 000 (i.e. no extra clock
				 * cycles required). The SL fields are
				 * initially set by the PUB during automatic
				 * DQS data training but these values can
				 * be overwritten by a direct write to
				 * this register. Every three bits of this
				 * register control the latency of each
				 * of the (up to) four ranks. R0DGSL controls
				 * the latency of rank 0, R1DGSL controls
				 * rank 1, and so on. Valid values are 0 to 7: */
	u32 r1dgsl:3;           /* Rank 1 DQS Gating System Latency: This
				 * is used to increase the number of clock
				 * cycles needed to expect valid DDR read
				 * data by up to seven extra clock cycles.
				 * This is used to compensate for board
				 * delays and other system delays. Power-up
				 * default is 000 (i.e. no extra clock
				 * cycles required). The SL fields are
				 * initially set by the PUB during automatic
				 * DQS data training but these values can
				 * be overwritten by a direct write to
				 * this register. Every three bits of this
				 * register control the latency of each
				 * of the (up to) four ranks. R0DGSL controls
				 * the latency of rank 0, R1DGSL controls
				 * rank 1, and so on. Valid values are 0 to 7: */
	u32 r2dgsl:3;           /* Rank 2 DQS Gating System Latency: This
				 * is used to increase the number of clock
				 * cycles needed to expect valid DDR read
				 * data by up to seven extra clock cycles.
				 * This is used to compensate for board
				 * delays and other system delays. Power-up
				 * default is 000 (i.e. no extra clock
				 * cycles required). The SL fields are
				 * initially set by the PUB during automatic
				 * DQS data training but these values can
				 * be overwritten by a direct write to
				 * this register. Every three bits of this
				 * register control the latency of each
				 * of the (up to) four ranks. R0DGSL controls
				 * the latency of rank 0, R1DGSL controls
				 * rank 1, and so on. Valid values are 0 to 7: */
	u32 r3dgsl:3;           /* Rank 3 DQS Gating System Latency: This
				 * is used to increase the number of clock
				 * cycles needed to expect valid DDR read
				 * data by up to seven extra clock cycles.
				 * This is used to compensate for board
				 * delays and other system delays. Power-up
				 * default is 000 (i.e. no extra clock
				 * cycles required). The SL fields are
				 * initially set by the PUB during automatic
				 * DQS data training but these values can
				 * be overwritten by a direct write to
				 * this register. Every three bits of this
				 * register control the latency of each
				 * of the (up to) four ranks. R0DGSL controls
				 * the latency of rank 0, R1DGSL controls
				 * rank 1, and so on. Valid values are 0 to 7: */
	u32 r0wlsl:2;           /* Rank 0 Write Leveling System Latency:
				 * This is used to adjust the write latency
				 * after write leveling. Power-up default
				 * is 01 (i.e. no extra clock cycles required).
				 * The SL fields are initially set by the
				 * PUB during automatic write leveling
				 * but these values can be overwritten
				 * by a direct write to this register.
				 * Every two bits of this register control
				 * the latency of each of the (up to) four
				 * ranks. R0WLSL controls the latency of
				 * rank 0, R1WLSL controls rank 1, and
				 * so on. Valid values:
				 * 00 = Write latency = WL - 1
				 * 01 = Write latency = WL
				 * 10 = Write latency = WL + 1
				 * 11 = Reserved */
	u32 r1wlsl:2;           /* Rank 1 Write Leveling System Latency:
				 * This is used to adjust the write latency
				 * after write leveling. Power-up default
				 * is 01 (i.e. no extra clock cycles required).
				 * The SL fields are initially set by the
				 * PUB during automatic write leveling
				 * but these values can be overwritten
				 * by a direct write to this register.
				 * Every two bits of this register control
				 * the latency of each of the (up to) four
				 * ranks. R0WLSL controls the latency of
				 * rank 0, R1WLSL controls rank 1, and
				 * so on. Valid values:
				 * 00 = Write latency = WL - 1
				 * 01 = Write latency = WL
				 * 10 = Write latency = WL + 1
				 * 11 = Reserved */
	u32 r2wlsl:2;           /* Rank 2 Write Leveling System Latency:
				 * This is used to adjust the write latency
				 * after write leveling. Power-up default
				 * is 01 (i.e. no extra clock cycles required).
				 * The SL fields are initially set by the
				 * PUB during automatic write leveling
				 * but these values can be overwritten
				 * by a direct write to this register.
				 * Every two bits of this register control
				 * the latency of each of the (up to) four
				 * ranks. R0WLSL controls the latency of
				 * rank 0, R1WLSL controls rank 1, and
				 * so on. Valid values:
				 * 00 = Write latency = WL - 1
				 * 01 = Write latency = WL
				 * 10 = Write latency = WL + 1
				 * 11 = Reserved */
	u32 r3wlsl:2;           /* Rank 3 Write Leveling System Latency:
				 * This is used to adjust the write latency
				 * after write leveling. Power-up default
				 * is 01 (i.e. no extra clock cycles required).
				 * The SL fields are initially set by the
				 * PUB during automatic write leveling
				 * but these values can be overwritten
				 * by a direct write to this register.
				 * Every two bits of this register control
				 * the latency of each of the (up to) four
				 * ranks. R0WLSL controls the latency of
				 * rank 0, R1WLSL controls rank 1, and
				 * so on. Valid values:
				 * 00 = Write latency = WL - 1
				 * 01 = Write latency = WL
				 * 10 = Write latency = WL + 1
				 * 11 = Reserved */
	u32 rsvd0:12;           /* - */
	} bits;
} reg_drampub_dx1gtr_t;

/* DATX8 [CH] General Timing Register */
typedef union {
	u32 val;
	struct {
	u32 r0dgsl:3;           /* Rank 0 DQS Gating System Latency: This
				 * is used to increase the number of clock
				 * cycles needed to expect valid DDR read
				 * data by up to seven extra clock cycles.
				 * This is used to compensate for board
				 * delays and other system delays. Power-up
				 * default is 000 (i.e. no extra clock
				 * cycles required). The SL fields are
				 * initially set by the PUB during automatic
				 * DQS data training but these values can
				 * be overwritten by a direct write to
				 * this register. Every three bits of this
				 * register control the latency of each
				 * of the (up to) four ranks. R0DGSL controls
				 * the latency of rank 0, R1DGSL controls
				 * rank 1, and so on. Valid values are 0 to 7: */
	u32 r1dgsl:3;           /* Rank 1 DQS Gating System Latency: This
				 * is used to increase the number of clock
				 * cycles needed to expect valid DDR read
				 * data by up to seven extra clock cycles.
				 * This is used to compensate for board
				 * delays and other system delays. Power-up
				 * default is 000 (i.e. no extra clock
				 * cycles required). The SL fields are
				 * initially set by the PUB during automatic
				 * DQS data training but these values can
				 * be overwritten by a direct write to
				 * this register. Every three bits of this
				 * register control the latency of each
				 * of the (up to) four ranks. R0DGSL controls
				 * the latency of rank 0, R1DGSL controls
				 * rank 1, and so on. Valid values are 0 to 7: */
	u32 r2dgsl:3;           /* Rank 2 DQS Gating System Latency: This
				 * is used to increase the number of clock
				 * cycles needed to expect valid DDR read
				 * data by up to seven extra clock cycles.
				 * This is used to compensate for board
				 * delays and other system delays. Power-up
				 * default is 000 (i.e. no extra clock
				 * cycles required). The SL fields are
				 * initially set by the PUB during automatic
				 * DQS data training but these values can
				 * be overwritten by a direct write to
				 * this register. Every three bits of this
				 * register control the latency of each
				 * of the (up to) four ranks. R0DGSL controls
				 * the latency of rank 0, R1DGSL controls
				 * rank 1, and so on. Valid values are 0 to 7: */
	u32 r3dgsl:3;           /* Rank 3 DQS Gating System Latency: This
				 * is used to increase the number of clock
				 * cycles needed to expect valid DDR read
				 * data by up to seven extra clock cycles.
				 * This is used to compensate for board
				 * delays and other system delays. Power-up
				 * default is 000 (i.e. no extra clock
				 * cycles required). The SL fields are
				 * initially set by the PUB during automatic
				 * DQS data training but these values can
				 * be overwritten by a direct write to
				 * this register. Every three bits of this
				 * register control the latency of each
				 * of the (up to) four ranks. R0DGSL controls
				 * the latency of rank 0, R1DGSL controls
				 * rank 1, and so on. Valid values are 0 to 7: */
	u32 r0wlsl:2;           /* Rank 0 Write Leveling System Latency:
				 * This is used to adjust the write latency
				 * after write leveling. Power-up default
				 * is 01 (i.e. no extra clock cycles required).
				 * The SL fields are initially set by the
				 * PUB during automatic write leveling
				 * but these values can be overwritten
				 * by a direct write to this register.
				 * Every two bits of this register control
				 * the latency of each of the (up to) four
				 * ranks. R0WLSL controls the latency of
				 * rank 0, R1WLSL controls rank 1, and
				 * so on. Valid values:
				 * 00 = Write latency = WL - 1
				 * 01 = Write latency = WL
				 * 10 = Write latency = WL + 1
				 * 11 = Reserved */
	u32 r1wlsl:2;           /* Rank 1 Write Leveling System Latency:
				 * This is used to adjust the write latency
				 * after write leveling. Power-up default
				 * is 01 (i.e. no extra clock cycles required).
				 * The SL fields are initially set by the
				 * PUB during automatic write leveling
				 * but these values can be overwritten
				 * by a direct write to this register.
				 * Every two bits of this register control
				 * the latency of each of the (up to) four
				 * ranks. R0WLSL controls the latency of
				 * rank 0, R1WLSL controls rank 1, and
				 * so on. Valid values:
				 * 00 = Write latency = WL - 1
				 * 01 = Write latency = WL
				 * 10 = Write latency = WL + 1
				 * 11 = Reserved */
	u32 r2wlsl:2;           /* Rank 2 Write Leveling System Latency:
				 * This is used to adjust the write latency
				 * after write leveling. Power-up default
				 * is 01 (i.e. no extra clock cycles required).
				 * The SL fields are initially set by the
				 * PUB during automatic write leveling
				 * but these values can be overwritten
				 * by a direct write to this register.
				 * Every two bits of this register control
				 * the latency of each of the (up to) four
				 * ranks. R0WLSL controls the latency of
				 * rank 0, R1WLSL controls rank 1, and
				 * so on. Valid values:
				 * 00 = Write latency = WL - 1
				 * 01 = Write latency = WL
				 * 10 = Write latency = WL + 1
				 * 11 = Reserved */
	u32 r3wlsl:2;           /* Rank 3 Write Leveling System Latency:
				 * This is used to adjust the write latency
				 * after write leveling. Power-up default
				 * is 01 (i.e. no extra clock cycles required).
				 * The SL fields are initially set by the
				 * PUB during automatic write leveling
				 * but these values can be overwritten
				 * by a direct write to this register.
				 * Every two bits of this register control
				 * the latency of each of the (up to) four
				 * ranks. R0WLSL controls the latency of
				 * rank 0, R1WLSL controls rank 1, and
				 * so on. Valid values:
				 * 00 = Write latency = WL - 1
				 * 01 = Write latency = WL
				 * 10 = Write latency = WL + 1
				 * 11 = Reserved */
	u32 rsvd0:12;           /* - */
	} bits;
} reg_drampub_dx2gtr_t;

/* DATX8 [CH] General Timing Register */
typedef union {
	u32 val;
	struct {
	u32 r0dgsl:3;           /* Rank 0 DQS Gating System Latency: This
				 * is used to increase the number of clock
				 * cycles needed to expect valid DDR read
				 * data by up to seven extra clock cycles.
				 * This is used to compensate for board
				 * delays and other system delays. Power-up
				 * default is 000 (i.e. no extra clock
				 * cycles required). The SL fields are
				 * initially set by the PUB during automatic
				 * DQS data training but these values can
				 * be overwritten by a direct write to
				 * this register. Every three bits of this
				 * register control the latency of each
				 * of the (up to) four ranks. R0DGSL controls
				 * the latency of rank 0, R1DGSL controls
				 * rank 1, and so on. Valid values are 0 to 7: */
	u32 r1dgsl:3;           /* Rank 1 DQS Gating System Latency: This
				 * is used to increase the number of clock
				 * cycles needed to expect valid DDR read
				 * data by up to seven extra clock cycles.
				 * This is used to compensate for board
				 * delays and other system delays. Power-up
				 * default is 000 (i.e. no extra clock
				 * cycles required). The SL fields are
				 * initially set by the PUB during automatic
				 * DQS data training but these values can
				 * be overwritten by a direct write to
				 * this register. Every three bits of this
				 * register control the latency of each
				 * of the (up to) four ranks. R0DGSL controls
				 * the latency of rank 0, R1DGSL controls
				 * rank 1, and so on. Valid values are 0 to 7: */
	u32 r2dgsl:3;           /* Rank 2 DQS Gating System Latency: This
				 * is used to increase the number of clock
				 * cycles needed to expect valid DDR read
				 * data by up to seven extra clock cycles.
				 * This is used to compensate for board
				 * delays and other system delays. Power-up
				 * default is 000 (i.e. no extra clock
				 * cycles required). The SL fields are
				 * initially set by the PUB during automatic
				 * DQS data training but these values can
				 * be overwritten by a direct write to
				 * this register. Every three bits of this
				 * register control the latency of each
				 * of the (up to) four ranks. R0DGSL controls
				 * the latency of rank 0, R1DGSL controls
				 * rank 1, and so on. Valid values are 0 to 7: */
	u32 r3dgsl:3;           /* Rank 3 DQS Gating System Latency: This
				 * is used to increase the number of clock
				 * cycles needed to expect valid DDR read
				 * data by up to seven extra clock cycles.
				 * This is used to compensate for board
				 * delays and other system delays. Power-up
				 * default is 000 (i.e. no extra clock
				 * cycles required). The SL fields are
				 * initially set by the PUB during automatic
				 * DQS data training but these values can
				 * be overwritten by a direct write to
				 * this register. Every three bits of this
				 * register control the latency of each
				 * of the (up to) four ranks. R0DGSL controls
				 * the latency of rank 0, R1DGSL controls
				 * rank 1, and so on. Valid values are 0 to 7: */
	u32 r0wlsl:2;           /* Rank 0 Write Leveling System Latency:
				 * This is used to adjust the write latency
				 * after write leveling. Power-up default
				 * is 01 (i.e. no extra clock cycles required).
				 * The SL fields are initially set by the
				 * PUB during automatic write leveling
				 * but these values can be overwritten
				 * by a direct write to this register.
				 * Every two bits of this register control
				 * the latency of each of the (up to) four
				 * ranks. R0WLSL controls the latency of
				 * rank 0, R1WLSL controls rank 1, and
				 * so on. Valid values:
				 * 00 = Write latency = WL - 1
				 * 01 = Write latency = WL
				 * 10 = Write latency = WL + 1
				 * 11 = Reserved */
	u32 r1wlsl:2;           /* Rank 1 Write Leveling System Latency:
				 * This is used to adjust the write latency
				 * after write leveling. Power-up default
				 * is 01 (i.e. no extra clock cycles required).
				 * The SL fields are initially set by the
				 * PUB during automatic write leveling
				 * but these values can be overwritten
				 * by a direct write to this register.
				 * Every two bits of this register control
				 * the latency of each of the (up to) four
				 * ranks. R0WLSL controls the latency of
				 * rank 0, R1WLSL controls rank 1, and
				 * so on. Valid values:
				 * 00 = Write latency = WL - 1
				 * 01 = Write latency = WL
				 * 10 = Write latency = WL + 1
				 * 11 = Reserved */
	u32 r2wlsl:2;           /* Rank 2 Write Leveling System Latency:
				 * This is used to adjust the write latency
				 * after write leveling. Power-up default
				 * is 01 (i.e. no extra clock cycles required).
				 * The SL fields are initially set by the
				 * PUB during automatic write leveling
				 * but these values can be overwritten
				 * by a direct write to this register.
				 * Every two bits of this register control
				 * the latency of each of the (up to) four
				 * ranks. R0WLSL controls the latency of
				 * rank 0, R1WLSL controls rank 1, and
				 * so on. Valid values:
				 * 00 = Write latency = WL - 1
				 * 01 = Write latency = WL
				 * 10 = Write latency = WL + 1
				 * 11 = Reserved */
	u32 r3wlsl:2;           /* Rank 3 Write Leveling System Latency:
				 * This is used to adjust the write latency
				 * after write leveling. Power-up default
				 * is 01 (i.e. no extra clock cycles required).
				 * The SL fields are initially set by the
				 * PUB during automatic write leveling
				 * but these values can be overwritten
				 * by a direct write to this register.
				 * Every two bits of this register control
				 * the latency of each of the (up to) four
				 * ranks. R0WLSL controls the latency of
				 * rank 0, R1WLSL controls rank 1, and
				 * so on. Valid values:
				 * 00 = Write latency = WL - 1
				 * 01 = Write latency = WL
				 * 10 = Write latency = WL + 1
				 * 11 = Reserved */
	u32 rsvd0:12;           /* - */
	} bits;
} reg_drampub_dx3gtr_t;

/* reg_drampub_t bank */
typedef struct tag_drampub {
	reg_drampub_ridr_t         ridr;       /* 0000 */
	reg_drampub_pir_t          pir;        /* 0004 */
	reg_drampub_pgcr0_t        pgcr0;      /* 0008 */
	reg_drampub_pgcr1_t        pgcr1;      /* 000C */
	reg_drampub_pgsr0_t        pgsr0;      /* 0010 */
	reg_drampub_pgsr1_t        pgsr1;      /* 0014 */
	reg_drampub_pllcr_t        pllcr;      /* 0018 */
	reg_drampub_ptr0_t         ptr0;       /* 001C */
	reg_drampub_ptr1_t         ptr1;       /* 0020 */
	reg_drampub_ptr2_t         ptr2;       /* 0024 */
	reg_drampub_ptr3_t         ptr3;       /* 0028 */
	reg_drampub_ptr4_t         ptr4;       /* 002C */
	reg_drampub_acmdlr_t       acmdlr;     /* 0030 */
	reg_drampub_acbdlr_t       acbdlr;     /* 0034 */
	reg_drampub_aciocr_t       aciocr;     /* 0038 */
	reg_drampub_dxccr_t        dxccr;      /* 003C */
	reg_drampub_dsgcr_t        dsgcr;      /* 0040 */
	reg_drampub_dcr_t          dcr;        /* 0044 */
	reg_drampub_dtpr0_t        dtpr0;      /* 0048 */
	reg_drampub_dtpr1_t        dtpr1;      /* 004C */
	reg_drampub_dtpr2_t        dtpr2;      /* 0050 */
	reg_drampub_mr0_t          mr0;        /* 0054 */
	reg_drampub_mr1_t          mr1;        /* 0058 */
	reg_drampub_mr2_t          mr2;        /* 005C */
	reg_drampub_mr3_t          mr3;        /* 0060 */
	reg_drampub_odtcr_t        odtcr;      /* 0064 */
	reg_drampub_dtcr_t         dtcr;       /* 0068 */
	reg_drampub_dtar0_t        dtar0;      /* 006C */
	reg_drampub_dtar1_t        dtar1;      /* 0070 */
	reg_drampub_dtar2_t        dtar2;      /* 0074 */
	reg_drampub_dtar3_t        dtar3;      /* 0078 */
	reg_drampub_dtdr0_t        dtdr0;      /* 007C */
	reg_drampub_dtdr1_t        dtdr1;      /* 0080 */
	reg_drampub_dtedr0_t       dtedr0;     /* 0084 */
	reg_drampub_dtedr1_t       dtedr1;     /* 0088 */
	u32                        res0[1];
	reg_drampub_vref_ac_t      vref_ac;    /* 0090 */
	reg_drampub_vref_dx01_t    vref_dx01;  /* 0094 */
	reg_drampub_vref_dx23_t    vref_dx23;  /* 0098 */
	u32                        res1[8];
	reg_drampub_fpga_dbg_t     fpga_dbg;   /* 00BC */
	reg_drampub_dcuar_t        dcuar;      /* 00C0 */
	reg_drampub_dcudr_t        dcudr;      /* 00C4 */
	reg_drampub_dcurr_t        dcurr;      /* 00C8 */
	reg_drampub_dculr_t        dculr;      /* 00CC */
	reg_drampub_dcugcr_t       dcugcr;     /* 00D0 */
	reg_drampub_dcutpr_t       dcutpr;     /* 00D4 */
	reg_drampub_dcusr0_t       dcusr0;     /* 00D8 */
	reg_drampub_dcusr1_t       dcusr1;     /* 00DC */
	u32                        res2[8];
	reg_drampub_bistrr_t       bistrr;     /* 0100 */
	reg_drampub_bistwcr_t      bistwcr;    /* 0104 */
	reg_drampub_bistmskr0_t    bistmskr0;  /* 0108 */
	reg_drampub_bistmskr1_t    bistmskr1;  /* 010C */
	reg_drampub_bistmskr2_t    bistmskr2;  /* 0110 */
	reg_drampub_bistlsr_t      bistlsr;    /* 0114 */
	reg_drampub_bistar0_t      bistar0;    /* 0118 */
	reg_drampub_bistar1_t      bistar1;    /* 011C */
	reg_drampub_bistar2_t      bistar2;    /* 0120 */
	reg_drampub_bistudpr_t     bistudpr;   /* 0124 */
	reg_drampub_bistgsr_t      bistgsr;    /* 0128 */
	reg_drampub_bistwer_t      bistwer;    /* 012C */
	reg_drampub_bistber0_t     bistber0;   /* 0130 */
	reg_drampub_bistber1_t     bistber1;   /* 0134 */
	reg_drampub_bistber2_t     bistber2;   /* 0138 */
	reg_drampub_bistber3_t     bistber3;   /* 013C */
	reg_drampub_bistwcsr_t     bistwcsr;   /* 0140 */
	reg_drampub_bistfwr0_t     bistfwr0;   /* 0144 */
	reg_drampub_bistfwr1_t     bistfwr1;   /* 0148 */
	reg_drampub_bistfwr2_t     bistfwr2;   /* 014C */
	u32                        res3[10];
	reg_drampub_gpr0_t         gpr0;       /* 0178 */
	reg_drampub_gpr1_t         gpr1;       /* 017C */
	reg_drampub_zq0cr0_t       zq0cr0;     /* 0180 */
	reg_drampub_zq0cr1_t       zq0cr1;     /* 0184 */
	reg_drampub_zq0sr0_t       zq0sr0;     /* 0188 */
	reg_drampub_zq0sr1_t       zq0sr1;     /* 018C */
	reg_drampub_zq1cr0_t       zq1cr0;     /* 0190 */
	reg_drampub_zq1cr1_t       zq1cr1;     /* 0194 */
	reg_drampub_zq1sr0_t       zq1sr0;     /* 0198 */
	reg_drampub_zq1sr1_t       zq1sr1;     /* 019C */
	reg_drampub_zq2cr0_t       zq2cr0;     /* 01A0 */
	reg_drampub_zq2cr1_t       zq2cr1;     /* 01A4 */
	reg_drampub_zq2sr0_t       zq2sr0;     /* 01A8 */
	reg_drampub_zq2sr1_t       zq2sr1;     /* 01AC */
	reg_drampub_zq3cr0_t       zq3cr0;     /* 01B0 */
	reg_drampub_zq3cr1_t       zq3cr1;     /* 01B4 */
	reg_drampub_zq3sr0_t       zq3sr0;     /* 01B8 */
	reg_drampub_zq3sr1_t       zq3sr1;     /* 01BC */
	reg_drampub_dx0gcr_t       dx0gcr;     /* 01C0 */
	reg_drampub_dx0gsr0_t      dx0gsr0;    /* 01C4 */
	reg_drampub_dx0gsr1_t      dx0gsr1;    /* 01C8 */
	reg_drampub_dx0bdlr0_t     dx0bdlr0;   /* 01CC */
	reg_drampub_dx0bdlr1_t     dx0bdlr1;   /* 01D0 */
	reg_drampub_dx0bdlr2_t     dx0bdlr2;   /* 01D4 */
	reg_drampub_dx0bdlr3_t     dx0bdlr3;   /* 01D8 */
	reg_drampub_dx0bdlr4_t     dx0bdlr4;   /* 01DC */
	reg_drampub_dx0lcdlr0_t    dx0lcdlr0;  /* 01E0 */
	reg_drampub_dx0lcdlr1_t    dx0lcdlr1;  /* 01E4 */
	reg_drampub_dx0lcdlr2_t    dx0lcdlr2;  /* 01E8 */
	reg_drampub_dx0mdlr_t      dx0mdlr;    /* 01EC */
	reg_drampub_dx0gtr_t       dx0gtr;     /* 01F0 */
	u32                        res4[3];
	reg_drampub_dx1gcr_t       dx1gcr;     /* 0200 */
	reg_drampub_dx1gsr0_t      dx1gsr0;    /* 0204 */
	reg_drampub_dx1gsr1_t      dx1gsr1;    /* 0208 */
	reg_drampub_dx1bdlr0_t     dx1bdlr0;   /* 020C */
	reg_drampub_dx1bdlr1_t     dx1bdlr1;   /* 0210 */
	reg_drampub_dx1bdlr2_t     dx1bdlr2;   /* 0214 */
	reg_drampub_dx1bdlr3_t     dx1bdlr3;   /* 0218 */
	reg_drampub_dx1bdlr4_t     dx1bdlr4;   /* 021C */
	reg_drampub_dx1lcdlr0_t    dx1lcdlr0;  /* 0220 */
	reg_drampub_dx1lcdlr1_t    dx1lcdlr1;  /* 0224 */
	reg_drampub_dx1lcdlr2_t    dx1lcdlr2;  /* 0228 */
	reg_drampub_dx1mdlr_t      dx1mdlr;    /* 022C */
	reg_drampub_dx1gtr_t       dx1gtr;     /* 0230 */
	u32                        res5[3];
	reg_drampub_dx2gcr_t       dx2gcr;     /* 0240 */
	reg_drampub_dx2gsr0_t      dx2gsr0;    /* 0244 */
	reg_drampub_dx2gsr1_t      dx2gsr1;    /* 0248 */
	reg_drampub_dx2bdlr0_t     dx2bdlr0;   /* 024C */
	reg_drampub_dx2bdlr1_t     dx2bdlr1;   /* 0250 */
	reg_drampub_dx2bdlr2_t     dx2bdlr2;   /* 0254 */
	reg_drampub_dx2bdlr3_t     dx2bdlr3;   /* 0258 */
	reg_drampub_dx2bdlr4_t     dx2bdlr4;   /* 025C */
	reg_drampub_dx2lcdlr0_t    dx2lcdlr0;  /* 0260 */
	reg_drampub_dx2lcdlr1_t    dx2lcdlr1;  /* 0264 */
	reg_drampub_dx2lcdlr2_t    dx2lcdlr2;  /* 0268 */
	reg_drampub_dx2mdlr_t      dx2mdlr;    /* 026C */
	reg_drampub_dx2gtr_t       dx2gtr;     /* 0270 */
	u32                        res6[3];
	reg_drampub_dx3gcr_t       dx3gcr;     /* 0280 */
	reg_drampub_dx3gsr0_t      dx3gsr0;    /* 0284 */
	reg_drampub_dx3gsr1_t      dx3gsr1;    /* 0288 */
	reg_drampub_dx3bdlr0_t     dx3bdlr0;   /* 028C */
	reg_drampub_dx3bdlr1_t     dx3bdlr1;   /* 0290 */
	reg_drampub_dx3bdlr2_t     dx3bdlr2;   /* 0294 */
	reg_drampub_dx3bdlr3_t     dx3bdlr3;   /* 0298 */
	reg_drampub_dx3bdlr4_t     dx3bdlr4;   /* 029C */
	reg_drampub_dx3lcdlr0_t    dx3lcdlr0;  /* 02A0 */
	reg_drampub_dx3lcdlr1_t    dx3lcdlr1;  /* 02A4 */
	reg_drampub_dx3lcdlr2_t    dx3lcdlr2;  /* 02A8 */
	reg_drampub_dx3mdlr_t      dx3mdlr;    /* 02AC */
	reg_drampub_dx3gtr_t       dx3gtr;     /* 02B0 */
} reg_drampub_t;

#endif /* ___DRAMPUB___H___ */
