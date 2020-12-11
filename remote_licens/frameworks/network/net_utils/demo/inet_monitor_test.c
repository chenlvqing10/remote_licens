/* inet_monitor_test.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <net/if.h>

#define  LOG_TAG		"net_utils"
#include <log/log.h>

#include "net_utils.h"
#include "ip_config.h"
#include "inet_monitor.h"

static int g_to_exit;

static void sigint_handler(int signal)
{
	g_to_exit = 1;
}

/* This function will be called when internet connect status change */
static void connect_handler(inet_connect_sta_t status)
{
	ALOGI("internet connect status change: %d", status);
}

static void usage(const char *argv0)
{
	fprintf(stderr, "Usage: %s [options]\n", argv0);
	fprintf(stderr, "Available options are\n");
	fprintf(stderr, " -d device	net device's name, such as eth0\n");
	fprintf(stderr, " -h		print this help screen and exit\n");
}

int main(int argc, char **argv)
{
	char *device = NULL;
	int opt, ret;

	while ((opt = getopt(argc, argv, "d:h")) != -1) {
		switch (opt) {
		case 'd':
			device = optarg;
			break;

		case 'h':
			usage(argv[0]);
			return 0;

		default:
			usage(argv[0]);
			return -EINVAL;
		}
	}

	ret = inet_monitor_start(device, connect_handler);
	if (ret) {
		ALOGE("failed to start internet connect status monitor");
		return ret;
	}

	signal(SIGINT, sigint_handler);

	while (!g_to_exit) {
		ret = inet_monitor_get_status(device);
		if (ret < 0)
			ALOGI("failed to get internet connect status, ret: %d",
			      ret);
		else
			ALOGI("internet connect status is: %d", ret);

		sleep(1);
	}

	/* This function must be called to avoid memory leak */
	ret = inet_monitor_stop(device);
	if (ret) {
		ALOGE("failed to stop internet connect status monitor");
		return ret;
	}

	return 0;
}
