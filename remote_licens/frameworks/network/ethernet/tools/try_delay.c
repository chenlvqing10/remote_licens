/* try_delay.c */

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define TX_DELAY_MAX		63
#define RX_DELAY_MAX		63

static const unsigned int total_cnt[] = {5, 20, 100, 200};
static const unsigned int pass_cnt[]  = {1, 15,  90, 200};

static void set_delay(unsigned int tx_delay, unsigned int rx_delay)
{
	char command[64] = {0};
	unsigned int delay = rx_delay * (1 << 24) + tx_delay;

	memset(command, 0, sizeof(command));
	sprintf(command, "devmem 0x01244014 32 0x%08x", delay);
	system(command);
}

static int ping(const char *dst, unsigned int cnt)
{
	FILE *stream;
	char recv_buf[32] = {0};
	char cmd_buf[128] = {0};

	if (!dst || !cnt)
		return -EINVAL;

	memset(recv_buf, 0, sizeof(recv_buf));
	memset(cmd_buf,  0, sizeof(cmd_buf));

	sprintf(cmd_buf, "ping %s -I %s -c %d -W 1 | grep time= | wc -l",
		dst, "eth0", cnt);
	stream = popen(cmd_buf, "r");
	fread(recv_buf, sizeof(char), sizeof(recv_buf) - 1, stream);
	pclose(stream);

	return atoi(recv_buf);
}

static void usage(const char *argv0)
{
	const char *format = "  %-20s %s\n";

	printf("Usage: %s [options]\n", argv0);
	printf(format, "-d <destination ip>", "destination ip address");
	printf(format, "-t <min max>", "tx delay parameter range");
	printf(format, "-r <min max>", "rx delay parameter range");
	printf(format, "-h, -H", "print this help screen and exit");
	printf("Note:\n"
	       "  1. tx delay parameter range should be a subset of [0, 63]\n"
	       "  2. rx delay parameter range should be a subset of [0, 63]\n"
	       "  3. tx delay parameter range should be [0, 0] if your type of "
	       "phy interface is rmii\n");
}

int main(int argc, char *argv[])
{
	char *dest = NULL;
	int ret, opt, i;
	unsigned int tx_delay, rx_delay;
	unsigned int tx_min = 0, rx_min = 0;
	unsigned int tx_max = TX_DELAY_MAX, rx_max = RX_DELAY_MAX;

	while (-1 != (opt = getopt(argc, argv, "d:r:t:hH"))) {
		switch (opt) {
		case 'd':
			dest = optarg;
			break;
		case 'r':
			rx_min = (unsigned int)strtol(argv[optind - 1],
						      NULL, 10);
			rx_max = (unsigned int)strtol(argv[optind], NULL, 10);
			break;
		case 't':
			tx_min = (unsigned int)strtol(argv[optind - 1],
						      NULL, 10);
			tx_max = (unsigned int)strtol(argv[optind], NULL, 10);
			break;
		case 'h':
		case 'H':
			usage(argv[0]);
			return 0;
		default:
			usage(argv[0]);
			return -EINVAL;
		}
	}

	if (!dest) {
		usage(argv[0]);
		return -EINVAL;
	}

	if ((tx_max > TX_DELAY_MAX) || (tx_min > tx_max) ||
		(rx_max > RX_DELAY_MAX) || (rx_min > rx_max)) {
		usage(argv[0]);
		return -EINVAL;
	}

	for (tx_delay = tx_min; tx_delay <= tx_max; tx_delay++) {
		for (rx_delay = rx_min; rx_delay <= rx_max; rx_delay++) {
			printf("\n[ tx_delay:%2u rx_delay:%2u ]  ",
			       tx_delay, rx_delay);

			set_delay(tx_delay, rx_delay);
			usleep(500 * 1000);

			for (i = 0;
			     i < (sizeof(total_cnt)/sizeof(total_cnt[0]));
			     i++) {
				ret = ping(dest, total_cnt[i]);

				printf("step%d: %d/%d, ", i, ret, total_cnt[i]);

				if (ret < pass_cnt[i])
					break;
			}
		}
	}

	printf("\n");

	return 0;
}
