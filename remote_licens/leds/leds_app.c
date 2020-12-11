#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

//#define RUN_LIGHT
#define _5G_LIGHT

#define RUNLIGHTPATH "/sys/class/leds/GPIO7/brightness"
#define _5GLIGHTPATH "/sys/class/leds/5G_USBDET/brightness"

#define ON  1	//led on
#define OFF 0	//led off

//delay time
void delay1s(int time){
	unsigned char h,i,j,k;
	for(;time>0;time--)
		for(h=5;h>0;h--)
			for(i=4;i>0;i--)
				for(j=116;j>0;j--)
					for(k=214;k>0;k--);
}

//show the current status of run light
void show_run_light_status()
{
	//open the leds of run by sys/class/leads node
#ifdef RUN_LIGHT
	FILE* fp = fopen(RUNLIGHTPATH,"w+");
#else
	FILE* fp = fopen(_5GLIGHTPATH,"w+");
#endif

	if(NULL == fp)
	{
		perror("open /sys/class/leds");
		exit(-1);
	}

	//read the run led's status
	char rBuf[2] = {0};
	int  rCount  = fread(rBuf,1,1,fp);
	if(rCount <= 0)
	{
		perror("read /sys/class/leds");
		exit(-1);
	}

	//show
	printf("the led's current status :%s\n",rBuf);

	fclose(fp);
}

void led_on()
{
	//open the leds of run by sys/class/leads node
#ifdef RUN_LIGHT
	FILE* fp = fopen(RUNLIGHTPATH,"w+");
#else
	FILE* fp = fopen(_5GLIGHTPATH,"w+");
#endif
	if(NULL == fp)
	{
		perror("open /sys/class/leds");
		exit(-1);
	}

	//make run light on
	fprintf(fp,"%d",ON);

	fclose(fp);
}

void led_off()
{
	//open the leds of run by sys/class/leads node
#ifdef RUN_LIGHT
	FILE* fp = fopen(RUNLIGHTPATH,"w+");
#else
	FILE* fp = fopen(_5GLIGHTPATH,"w+");
#endif
	if(NULL == fp)
	{
		perror("open /sys/class/leds");
		exit(-1);
	}

	//make run light off
	fprintf(fp,"%d",OFF);

	fclose(fp);
}
int main()
{
	//write 0/1 to make run leds on/off
	while(1)
	{
		led_on();
		show_run_light_status();
		delay1s(20);
		led_off();
		show_run_light_status();
		delay1s(20);
	}	
}
