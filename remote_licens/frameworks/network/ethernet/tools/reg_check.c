/* reg_check.c */

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#define PRT_DBG(fmt, ...)
#define PRT_INFO(fmt, ...)	printf(fmt, ##__VA_ARGS__)
#define PRT_WARN(fmt, ...)	printf(fmt, ##__VA_ARGS__)
#define PRT_ERR(fmt, ...)	printf(fmt, ##__VA_ARGS__)

static int devmem(unsigned int addr, unsigned int *val)
{
	FILE *stream;
	char recv_buf[16] = {0};
	char cmd_buf[32]  = {0};
	unsigned int temp;
	int i;

	memset(recv_buf, 0, sizeof(recv_buf));
	memset(cmd_buf,  0, sizeof(cmd_buf));

	sprintf(cmd_buf, "devmem 0x%x", addr);

	stream = popen(cmd_buf, "r");
	if (!stream) {
		PRT_ERR("failed to execute: %s\n", cmd_buf);
		return -EIO;
	}
	fread(recv_buf, sizeof(char), sizeof(recv_buf) - 1, stream);
	pclose(stream);

	i = sscanf(recv_buf, "0x%x", &temp);
	if (i != 1) {
		PRT_ERR("devmem: address[0x%08x] value[%s]\n", addr, recv_buf);
		return -EIO;
	}

	*val = temp;

	return 0;
}

static void usage(const char *argv0)
{
	printf("Usage: %s -p <file path>\n", argv0);
}

int main(int argc, char *argv[])
{
	FILE *fp;
	char *path = NULL;
	char *line = NULL;
	char *token = NULL;
	char *saveptr = NULL;
	size_t len = 0;
	unsigned int addr, mask, expect, value;
	int i, cnt, ret, opt, err = 0;

	while (-1 != (opt = getopt(argc, argv, "p:"))) {
		switch (opt) {
		case 'p':
			path = optarg;
			break;
		default:
			usage(argv[0]);
			return -EINVAL;
		}
	}

	if (!path) {
		usage(argv[0]);
		return -EINVAL;
	}

	fp = fopen(path, "r");
	if (!fp) {
		PRT_ERR("failed to open %s\n", path);
		return -ENOENT;
	}

	cnt = 0;
	while (getline(&line, &len, fp) != -1) {
		cnt++;

		if (line[0] == '#')
			continue;
		if (strstr(line, "#"))
			token = strtok_r(line, "#", &saveptr);
		else
			token = line;

		if (!token)
			continue;

		i = sscanf(token, "0x%x, 0x%x, 0x%x", &addr, &mask, &expect);
		if (i != 3) {
			if (i != 0)
				PRT_WARN("failed to parse line %d: %s",
					 cnt, line);
			continue;
		}

		ret = devmem(addr, &value);
		if (ret) {
			PRT_ERR("failed to devmem address 0x%08x\n", addr);
			goto error;
		}

		if ((mask & expect) == (mask & value)) {
			PRT_DBG("[ 0x%08x ] 0x%08x, 0x%08x, 0x%08x\n",
				addr, mask, expect, value);
		} else {
			PRT_WARN("[ 0x%08x ] 0x%08x, 0x%08x, 0x%08x failed\n",
				 addr, mask, expect, value);
			err = 1;
		}
	}

	free(line);
	fclose(fp);

	PRT_INFO("%s: %sall value meet expectation\n",
		 argv[0], err ? "not " : "");

	return 0;

error:
	free(line);
	fclose(fp);

	return ret;
}
