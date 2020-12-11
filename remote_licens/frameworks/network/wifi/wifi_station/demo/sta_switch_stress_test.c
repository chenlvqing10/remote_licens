#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define  LOG_TAG		"sta_switch_stress_test"
#include <log/log.h>

#include "wifi_station.h"

struct os_time {
	long sec;
	long usec;
};

static int os_get_time(struct os_time *t)
{
	int res;
	struct timeval tv;
	res = gettimeofday(&tv, NULL);
	t->sec = tv.tv_sec;
	t->usec = tv.tv_usec;
	return res;
}

static void wifi_debug_print_timestamp(void)
{
	struct os_time tv;

	os_get_time(&tv);
	ALOGD("timestamp: %ld.%06u: ", (long) tv.sec, (unsigned int) tv.usec);
}

static void sta_event_callback(STA_EVENT event)
{
	switch (event) {
	case STA_DISCONNECTED:
		ALOGI("wifi disconnected!\n");
		break;

	case STA_SCAN_RESULTS:
		ALOGI("go to scan_result!!\n");
		break;

	case STA_CONNECTED:
		ALOGI("start dhcp!\n");
		break;

	case STA_NETWORK_NOT_EXIST:
		ALOGW("network is not exist!\n");
		break;

	case STA_PASSWD_ERROR:
		ALOGW("password is wrong!\n");
		break;

	case STA_CONNECT_REJECT:
		ALOGW("connection is rejected!\n");
		break;

	case STA_UNKNOWN_EVENT:
		ALOGW("unknown wifi event!\n");
		break;
	}
}

static void use_help(void)
{
	printf("usage:\n");
	printf("	sta_switch_stress_test [options]\n");
	printf("option = instruction:\n");
	printf("	-n = number of AP loop closed and started.\n");
	printf("	-h = show help\n");
	printf("example:\n");
	printf("	sta_switch_stress_test -n 1000\n");
}

int main(int argc, char *argv[])
{
	int ret = 0, count = 0, test_count = 0;
	int opt = 0;
	int enable_successed_count = 0, disable_successed_count = 0;
	char *number_ptr = NULL;

	if (argc == 1) {
		use_help();
		ret = -1;
		goto exit;
	}

	for (;;) {
		opt = getopt(argc, argv, "n:h");
		if (opt < 0)
			break;
		switch (opt) {
		case 'n':
			number_ptr = optarg;
			break;
		case 'h':
			use_help();
			ret = 0;
			goto exit;
		default:
			use_help();
			ret = -1;
			goto exit;
		}
	}

	test_count = atoi(number_ptr);
	ALOGI("total test time: %d!\n", test_count);
	for (count = 0; count < test_count; count++) {
		ALOGI("test times: %d!\n", count);

		wifi_debug_print_timestamp();
		ret = sta_enable(sta_event_callback);
		wifi_debug_print_timestamp();
		if (ret == -1) {
			ALOGE("enable sta failed!\n");
			goto exit;
		} else {
			enable_successed_count++;
			ALOGI("enable sta successed!\n");
		}

		wifi_debug_print_timestamp();
		ret = sta_disable();
		wifi_debug_print_timestamp();
		if (ret == -1) {
			ALOGE("disable sta failed!\n");
			goto exit;
		} else {
			disable_successed_count++;
			ALOGI("disable sta successed!\n");
		}

	}
exit:
	ALOGI("enable_successed_count: %d, disable_successed_count: %d",
		enable_successed_count, disable_successed_count);
	if ((enable_successed_count == test_count) &&
		(disable_successed_count == test_count))
		ALOGI("test is passed!\n");
	else
		ALOGI("test is not passed!\n");
	return ret;
}
