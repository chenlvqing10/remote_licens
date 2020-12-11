#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <pthread.h>
#include "watchdog.h"

int watchdogfd = -1;

void *feeddogthread()
{
	int count;
	int feeddogvalue;

	feeddogvalue = 65535;
	count = 0;

	while (1) {
		count++;
		printf("feeddogthread count: %d s.\n", count);
		if (count%10 == 0 && watchdogfd > 0) {
			write(watchdogfd, &feeddogvalue, sizeof(int));
			count = 0;
		}
		sleep(1);
	}
}

int main()
{
	pthread_t wdog_pt;
	int returnval;
	char ch;
	struct watchdog_info wi;
	int tmp;

	watchdogfd = open("/dev/watchdog", O_RDWR | O_NONBLOCK);
	if (watchdogfd < 0) {
		printf("cannot open the watchdog device.\n");
		exit(0);
	}

	ioctl(watchdogfd, WDIOC_GETSUPPORT, &wi);
	printf("watchdog info:%d, %s\n", wi.options, wi.identity);

	returnval = pthread_create(&wdog_pt, NULL, feeddogthread, NULL);
	if (returnval < 0)
		printf("cannot create feeddog thread.\n");

	while (1) {
		printf("Press 'd' to disable watchdog, or 'e' to enable!\n");
		printf("Press 's' to set watchdog timeout in 5s!\n");
		printf("Press 'q' to quit watchdog test!\n");
		ch = getchar();

		if (ch == 'd') {
			printf("Disable watchdog!\n");
			tmp = WDIOS_DISABLECARD;
			ioctl(watchdogfd, WDIOC_SETOPTIONS, &tmp);
		}

		if (ch == 'e') {
			printf("Enable watchdog!\n");
			tmp = WDIOS_ENABLECARD;
			ioctl(watchdogfd, WDIOC_SETOPTIONS, &tmp);
		}

		if (ch == 's') {
			printf("Set watchdog timeout in 5s!\n");
			tmp = 5;
			ioctl(watchdogfd, WDIOC_SETTIMEOUT, &tmp);

			tmp = 0;
			ioctl(watchdogfd, WDIOC_GETTIMEOUT, &tmp);
			printf("watchdog get timeout:%d\n", tmp);
		}

		if (ch == 'q') {
			printf("Quit watchdog test!\n");
			break;
		}
	}

	printf("Close watchdog an exit safety!\n");
	close(watchdogfd);
	watchdogfd = -1;

	return 0;
}
