#include <stdio.h>
#include <stdlib.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

/* test data, test abort time boundary */
static struct rtc_time test[] = {
	{59, 0, 0, 1, 1, 2000, 0, 0, 0}, {60, 0, 0, 1, 1, 2000, 0, 0, 0}, {0, 0, 0, 1, 1, 2000, 0, 0, 0},
	{-1, 0, 0, 1, 1, 2000, 0, 0, 0}, {1, 0, 0, 1, 1, 2000, 0, 0, 0}, {0, 59, 0, 1, 1, 2000, 0, 0, 0},
	{0, 60, 0, 1, 1, 2000, 0, 0, 0}, {0, -1, 0, 1, 1, 2000, 0, 0, 0}, {0, 1, 0, 1, 1, 2000, 0, 0, 0},
	{0, 0, 23, 1, 1, 2000, 0, 0, 0}, {0, 0, 24, 1, 1, 2000, 0, 0, 0}, {0, 0, -1, 1, 1, 2000, 0, 0, 0},
	{0, 0, 1, 1, 1, 2000, 0, 0, 0}, {0, 0, 0, 0, 1, 2000, 0, 0, 0}, {0, 0, 0, 31, 1, 2000, 0, 0, 0},
	{0, 0, 0, 31, 1, 2000, 0, 0, 0}, {0, 0, 0, 32, 1, 2000, 0, 0, 0}, {0, 0, 0, 30, 4, 2000, 0, 0, 0},
	{0, 0, 0, 31, 4, 2000, 0, 0, 0}, {0, 0, 0, 30, 2, 2000, 0, 0, 0}, {0, 0, 0, 29, 2, 2000, 0, 0, 0},
	{0, 0, 0, 28, 2, 1999, 0, 0, 0}, {0, 0, 0, 29, 2, 1999, 0, 0, 0}, {0, 0, 0, 1, 12, 2000, 0, 0, 0},
	{0, 0, 0, 1, 13, 2000, 0, 0, 0}, {0, 0, 0, 1, 1, 2000, 0, 0, 0}, {0, 0, 0, 1, 0, 2000, 0, 0, 0},
	{0, 0, 0, 1, 2, 2000, 0, 0, 0}, {0, 0, 0, 1, 1, 2100, 0, 0, 0}, {0, 0, 0, 1, 1, 2101, 0, 0, 0},
	{0, 0, 0, 1, 1, 1970, 0, 0, 0}, {0, 0, 0, 1, 1, 1969, 0, 0, 0},
};

/* default rtc time, time is 2020/1/1 01:01:01 */
static struct rtc_time rtc_tm = {
	.tm_sec = 1,
	.tm_min = 1,
	.tm_hour = 1,
	.tm_mday = 1,
	.tm_mon = 1,
	.tm_year = 2020,
	.tm_wday = 0,
	.tm_yday = 0,
	.tm_isdst = 0,
};

/* default rtc alarm */
struct rtc_wkalrm alrm = {
	.time.tm_sec = 1,
	.time.tm_min = 1,
	.time.tm_hour = 1,
	.time.tm_mday = 1,
	.time.tm_mon = 2,
	.time.tm_year = 2020,
	.enabled = 1,
};

/* raw time transfer normal time */
struct rtc_time time_transfer(struct rtc_time rtc_tm)
{
	rtc_tm.tm_year -= 1900;
	rtc_tm.tm_mon -= 1;
	return rtc_tm;
}

/* print rtc time */
void print_time(struct rtc_time rtc_tm)
{
	printf("RTC time: %d/%d/%d %02d:%02d:%02d \n",
		rtc_tm.tm_year + 1900, rtc_tm.tm_mon + 1, rtc_tm.tm_mday,
		rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);
}

/* print rtc alarm time */
void print_alarm(struct rtc_wkalrm alarm)
{
	printf("alarm is %d/%d/%d %02d:%02d:%02d\n", alarm.time.tm_year + 1900,
		alarm.time.tm_mon + 1, alarm.time.tm_mday, alarm.time.tm_hour,
		alarm.time.tm_min, alarm.time.tm_sec);
}

/* get rtc time */
int rtc_get_time_test()
{
	int fd, ret;
	struct rtc_time rtc_tm;

	printf("start rtc_get_time_test\n");
	fd = open("/dev/rtc0", O_RDONLY);
	if (fd < 0) {
		printf("open rtc failed\n\n");
		return -1;
	}

	ret = ioctl(fd, RTC_RD_TIME, &rtc_tm);
	if (ret < 0) {
		close(fd);
		printf("read rtc time failed\n\n");
		return -1;
	}

	close(fd);
	print_time(rtc_tm);

	printf("end rtc_get_time_test\n\n");
	return 0;
}

/* set rtc time */
int rtc_set_time_test(struct rtc_time rtc_tm)
{
	int fd, ret;
	struct rtc_time tm;

	printf("start rtc_set_time_test\n");
	fd = open("/dev/rtc0", O_RDWR);
	printf("fd is %d\n", fd);
	if (fd < 0) {
		printf("open rtc failed\n\n");
		return -1;
	}

	tm = rtc_tm;
	tm = time_transfer(tm);
	ret = ioctl(fd, RTC_SET_TIME, &tm);

	if (ret < 0) {
		close(fd);
		printf("set rtc time failed, case is \n");
		print_time(tm);
		printf("\n");
		return -1;
	}

	close(fd);

	printf("end rtc_set_time_test\n\n");
	return 0;
}

/* get alarm time */
int rtc_alarm_get_test()
{

	int fd, ret;
	struct rtc_wkalrm alarm;

	printf("start rtc_alarm_get_test\n");
	fd = open("/dev/rtc0", O_RDONLY);
	if (fd < 0) {
		printf("open rtc failed\n\n");
		return -1;
	}

	ret = ioctl(fd, RTC_ALM_READ, &alarm.time);
	if (ret < 0) {
		close(fd);
		printf("read rtc time failed\n\n");
		return -1;
	}

	close(fd);
	print_alarm(alarm);

	printf("end rtc_alarm_get_test\n\n");
	return 0;
}

/* set rtc alarm */
int rtc_alarm_set_test(struct rtc_time rtc_tm)
{
	int fd, ret;
	struct rtc_wkalrm alarm;

	printf("start rtc_alarm_set_test\n");
	fd = open("/dev/rtc0", O_RDWR);
	if (fd < 0) {
		printf("open rtc failed\n\n");
		return -1;
	}

	alarm.time = rtc_tm;
	alarm.time = time_transfer(alarm.time);
	alarm.time.tm_min += 1;

	ret = ioctl(fd, RTC_ALM_SET, &alarm.time);
	if (ret < 0) {
		close(fd);
		printf("set rtc alarm failed\n\n");
		return -1;
	}

	ret = ioctl(fd, RTC_AIE_ON, 0);
	if (ret < 0) {
		close(fd);
		printf("set rtc alarm aie failed\n\n");
		return -1;
	}

	close(fd);

	printf("end rtc_alarm_set_test\n\n");
	return 0;
}

/* alarm callback test */
void rtc_alarm_callback_test()
{
	int n;
	int fd, ret;
	struct rtc_time tm;
	struct rtc_wkalrm alarm;
	unsigned char flag;

	printf("rtc alarm callback test start\n");

	fd = open("/dev/rtc0", O_RDWR);
	if (fd < 0) {
		printf("open rtc failed\n\n");
		return;
	}

	n = 5;
	tm = rtc_tm;
	tm = time_transfer(tm);
	ret = ioctl(fd, RTC_SET_TIME, &tm);

	while (n--) {
		flag = 0;
		ioctl(fd, RTC_RD_TIME, &tm);
		rtc_tm.tm_sec += 10;
		ioctl(fd, RTC_ALM_SET, &tm);
		ioctl(fd, RTC_AIE_ON, 0);
		ret = ioctl(fd, RTC_WKALM_RD, &alarm);
		printf("after set alarm, alarm irq is %s\n",
			(alarm.enabled) ? "yes" : "no");
		if (alarm.enabled == 1)
			flag |= 1;
		sleep(10);
		ret = ioctl(fd, RTC_WKALM_RD, &alarm);
		printf("alarm irq is %s\n", (alarm.enabled) ? "yes" : "no");
		if (alarm.enabled == 0)
			flag |= (1 << 1);
		if (flag == 3)
			printf("alarm rang!\n");
	}
	close(fd);

	printf("end\n\n");
}

int main()
{
	int n = sizeof(test) / sizeof(test[0]);

	rtc_get_time_test();
	while(n--)
		rtc_set_time_test(test[n]);
	rtc_set_time_test(rtc_tm);
	rtc_alarm_set_test(rtc_tm);
	rtc_alarm_get_test();
	rtc_alarm_callback_test();

	return 0;
}
