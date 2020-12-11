#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define  LOG_TAG		"at_config"
#include <log/log.h>

#define MODEM_CFG_PATH		"/etc/ppp/modem_cfg"
#define PRE_TTY			"tty:"

/**
 * at_config_file_exist - check modem at config file whether exist
 *
 * if file no exist return -1.
 */
static int at_config_file_exist(void)
{
	int ret;
	ret = access(MODEM_CFG_PATH, F_OK);
	if (ret == -1)
		ALOGE("%s no exist\n", MODEM_CFG_PATH);

	return ret;
}

/**
 * at_get_tty_path - get modem at config path
 *
 *@path: tty path buffer start address
 *@len: tty path length
 *
 * config path exist return 0.
 */
int at_get_tty_path(char *path, int *len)
{
	int ret;
	char data_buf[128] = {0};
	int tmp_len;
	FILE *fp;

	ret = at_config_file_exist();
	if (ret == -1)
		return ret;

	fp = fopen(MODEM_CFG_PATH, "r");
	if (fp == NULL) {
		ALOGE("%s open fail\n", MODEM_CFG_PATH);
		return -1;
	}

	while (!feof(fp)) {
		fgets(data_buf, 128, fp);
		data_buf[strlen(data_buf)-1] = 0;
		ret = strncmp(data_buf, PRE_TTY, strlen(PRE_TTY));
		if (ret == 0) {
			tmp_len = strlen(data_buf) - strlen(PRE_TTY);
			memcpy(path, data_buf + strlen(PRE_TTY), tmp_len);
			/* ALOGD("path: %s, len: %d\n", path, tmp_len); */
			*len = tmp_len;
			fclose(fp);
			return 0;
		}
		memset(data_buf, 0, 128);
	}

	fclose(fp);
	return -1;
}
