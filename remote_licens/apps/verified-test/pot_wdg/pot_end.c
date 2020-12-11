#include "pot_wdg.h"

void main(void)
{
	u32 status = 0;
	u32 flag = 0;
	run_flag_t *run_flag = (run_flag_t *)POT_RET_PARA_ADDR;

	/* SET GPIOC17 MODE : OUTPUT */
	write_reg_bits((_GPIO_BASE_ + _GPIO_MODE_), MODE_OFFSET, MODE_BITS, MODE_VALUE);
	/* PULL GPIOC17 DOWN */
	write_reg_bits((_GPIO_BASE_ + _GPIO_DATA_), DATA_OFFSET, DATA_BITS, PULL_DOWN);
	/* check GPIOC17 value */
	if(0 == read_reg_bits((_GPIO_BASE_ + _GPIO_DATA_), DATA_OFFSET, DATA_BITS))
		flag = 1;
	/* set finish flag */
	run_flag->magic = POT_RET_PARA_MAGIC;
	run_flag->pot_type = A_RUN_POT_NONE;
	run_flag->flag = flag;

	return;
}
