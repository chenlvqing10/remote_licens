/* inet_connect_test.c */

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
	struct timeval tv1, tv2;
	unsigned long long consume_us;
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

	signal(SIGINT, sigint_handler);

	while (!g_to_exit) {
		gettimeofday(&tv1, NULL);

		ret = inet_is_connect(device);
		if (ret) {
			if (device)
				ALOGI("internet is reachable by %s", device);
			else
				ALOGI("internet is reachable");
		} else {
			if (device)
				ALOGI("internet is unreachable by %s", device);
			else
				ALOGI("internet is unreachable");
		}

		gettimeofday(&tv2, NULL);

		consume_us = tv2.tv_sec * 1000000 + tv2.tv_usec -
			     (tv1.tv_sec * 1000000 + tv1.tv_usec);

		if (consume_us < 1200000)
			usleep(1200000 - consume_us);
	}

	return 0;
}
