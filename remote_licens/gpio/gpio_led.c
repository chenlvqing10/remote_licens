#include <stdio.h>
#include <stdlib.h>

//define GPIOx Module base address
typedef unsigned int uint32;

typedef struct{
	uint32 GPDIR;//0x0230 0000
	uint32 GPODR;//0x0230 0004
	uint32 GPDAT;//0x0230 0008
	uint32 GPIER;//0x0230 000C
	uint32 GPIMR;//0x0230 0010
	uint32 GPICR;//0x0230 0014
}gpio1;

#define  GPIO1     (* (volatile gpio1 *)0x02300000)

int main()
{
	printf("1111\n");
	//GPIO1_31  set to gpio module 0:The corresponding signal is actively driven as an output.  high is active
	printf("GPDIR = %p\n",&GPIO1.GPDIR);//get gpdir register's value
	printf("GPODR = %p\n",&GPIO1.GPODR);//get gpdir register's value
	printf("GPDAT = %p\n",&GPIO1.GPDAT);//get gpdir register's value
	//GPIO1.GPODR &= ~(0x1 << 31 );

	//GPIO_31   set  out direction
	printf("1111\n");
	GPIO1.GPDIR  |= (0x1 << 31);

	//GPIO_31   set 0/1 values
	printf("1111\n");
	GPIO1.GPDAT |= (0x1 << 31);

	return 0;
}
