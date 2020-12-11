#ifndef __POT_WDG_H__
#define __POT_WDG_H__

/* GPIO OFFSET CONFIG */
#define _GPIO_BASE_		0x4015000//GPIO Base addr
#define _GPIO_MODE_		0xA8//GPIOC Function register
#define _GPIO_DATA_		0xD0//GPIOC data register
/* GPIO MODE CONFIG */
#define MODE_OFFSET		0x4//GPIOC 17
#define MODE_BITS		0x4//4 bits
#define MODE_VALUE		0b0010//OUTPUT
/* GPIO GPIO DATA CONFIG */
#define PULL_UP			0b1//up
#define PULL_DOWN		0b0//down
#define DATA_OFFSET		(17)//GPIOC 17
#define DATA_BITS		0x1//1 bit

/* other DATA */
#define POT_RET_PARA_ADDR	0x40300000
#define POT_RET_PARA_MAGIC	0x50415241
#define A_RUN_POT_NONE		0x00

#define s32 int
#define u32 unsigned int
#define u16 unsigned short
#define READREG32(reg) *(volatile unsigned int *)(reg)
#define READREG16(reg) *(volatile unsigned short *)(reg)
#define WRITEREG32(reg, val) *(volatile unsigned int *)(reg) = (val)
#define read_reg_bits(reg, pos, width) (READREG32(reg) >> pos) & ((1U << width) -1)
#define write_reg_bits(reg, pos, width, val) WRITEREG32(reg, ((READREG32(reg) & (u32)(~(((1U << width) -1) << pos))) | (u32)((val & ((1U << width) - 1)) << pos)))

typedef struct tag_run_flag
{
	u32 magic;
	u16 pot_type;
	u16 flag;
	u32 ret_val1;
	u32 ret_val2;
	u32 res0;
	u32 res1;
	u32 res2;
	u32 res3;
} run_flag_t;

#endif //  ifndef __POT_WDG_H__
