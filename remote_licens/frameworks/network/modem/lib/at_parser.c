#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>

#define  LOG_TAG	"at_parser"
#include <log/log.h>

#include "at_parser.h"
#include "config.h"

#define TTY_PATH_LEN	128
#define READ_TRY_COUNT	20
#define RESP_BUF_SIZE	512

#define CHARISNUM(x)	((x) >= '0' && (x) <= '9')
#define CHARISHEXNUM(x)	(((x) >= '0' && (x) <= '9') || \
			((x) >= 'a' && (x) <= 'f') || \
			((x) >= 'A' && (x) <= 'F'))
#define CHARTONUM(x)	((x) - '0')

/**
 * open_tty_dev - open at tty device
 *
 * return file handle if success; -1 mean fail.
 */
static int open_tty_dev(void)
{
	int ret;
	int len;
	int fd;
	char path[128] = {0};

	ret = at_get_tty_path(path, &len);
	if (ret != 0) {
		ALOGE("no find at tty device path %d\n", ret);
		return -1;
	}

	/* open uart device */
	fd = open(path, O_RDWR|O_NOCTTY);
	if (fd < 0) {
		ALOGE("open %s failed\n", path);
		return -1;
	}

	/* clear input and output buffer */
	tcflush(fd, TCIFLUSH);

	return fd;
}

/**
 * parser_number - transfer string to number
 *
 *@ptr: string buffer start address
 *@cnt: offset count
 *
 * return the transfer result.
 */
static int parser_number(const char *ptr, int *cnt)
{
	int minus = 0, sum = 0, i = 0;

	if (*ptr == '-') {
		minus = 1;
		ptr++;
		i++;
	}

	while (CHARISNUM(*ptr)) {
		sum = 10 * sum + CHARTONUM(*ptr);
		ptr++;
		i++;
	}

	if (cnt != NULL)
		*cnt = i;

	if (minus)
		return 0 - sum;

	return sum;
}

/**
 * at_cmd_send_recv - send AT command and receive response data.
 *
 *@cmd: AT command
 *@cmd_len: command length
 *@data: AT command response data
 *@data_len: response data length
 *
 * return 0 if success, -1 if failed.
 */
int at_cmd_send_recv(char *cmd, int cmd_len, char *data, int *data_len)
{
	int ret = 0;
	int fd;
	int i;
	int nread;
	int send_len = 0;
	int resp_len = 0;
	char tmp_buf[RESP_BUF_SIZE] = {0};
	char buf[RESP_BUF_SIZE] = {0};

	if (cmd == NULL || data == NULL) {
		ALOGE("cmd or data buff is null\n");
		return -1;
	}

	/* open tty device */
	fd = open_tty_dev();
	if (fd < 0)
		return -1;

	/* send at command */
	send_len = write(fd, cmd, cmd_len);
	if (send_len != cmd_len) {
		tcflush(fd, TCOFLUSH);
		ALOGE("send cmd: %s fail\n", cmd);
		ret = -1;
		goto err;
	}

	/* read tty data */
	for (i = 0; i < READ_TRY_COUNT; i++) {
		nread = read(fd, tmp_buf, RESP_BUF_SIZE);
		/* printf("[%d] len: %d, %s\n", i, nread, tmp_buf); */
		resp_len += nread;
		if (resp_len > RESP_BUF_SIZE) {
			ALOGW("cmd %s resp large %d\n", cmd, RESP_BUF_SIZE);
			ret = -1;
			goto err;
		}

		strncat(buf, tmp_buf, nread);
		memset(tmp_buf, 0, RESP_BUF_SIZE);

		/* break */
		if ((strstr(buf, "OK") != NULL) ||
		    (strstr(buf, "ERR") != NULL)) {
			break;
		}
	}

	memcpy(data, buf, resp_len);
	*data_len = resp_len;

err:
	close(fd);
	return ret;
}

/**
 * at_cmd_check_at - send at command AT and receive response data.
 *
 *@resp: AT command response data
 *
 * return 0 if success, -1 if failed.
 */
int at_cmd_check_at(struct common_resp *resp)
{
	int ret;
	int resp_len = 0;
	char *str = NULL;
	char buf[RESP_BUF_SIZE] = {0};
	char cmd_buf[] = "AT\r";

	if (resp == NULL) {
		ALOGE("common_resp is null\n");
		return -1;
	}

	ret = at_cmd_send_recv(cmd_buf, sizeof(cmd_buf), buf, &resp_len);
	if (ret != AT_OK)
		return -1;

	/* parser data OK */
	str = strstr(buf, "OK");
	if (str == NULL)
		resp->status = AT_ERR;
	else
		resp->status = AT_OK;

	return 0;
}

/**
 * at_cmd_get_gmr - send at command AT+GMR and receive response data.
 *
 *@resp: AT command response data
 *
 * return 0 if success, -1 if failed.
 */
int at_cmd_get_gmr(struct common_resp *resp)
{
	int ret = 0;
	int i = 0;
	int resp_len = 0;
	char *str = NULL;
	char buf[RESP_BUF_SIZE] = {0};
	char cmd_buf[] = "AT+GMR\r";

	if (resp == NULL) {
		ALOGE("common_resp is null\n");
		return -1;
	}

	ret = at_cmd_send_recv(cmd_buf, sizeof(cmd_buf), buf, &resp_len);
	if (ret != AT_OK)
		return -1;

	/* parser data OK */
	str = strstr(buf, "OK");
	if (str == NULL)
		resp->status = AT_ERR;
	else {
		resp->status = AT_OK;
		while (*(buf + 8 + i) != '\n')
			i++;

		memcpy(resp->data, buf + 8, i);
		resp->len = i;
	}

	return 0;
}

/**
 * at_cmd_get_ccid - send at command AT+CCID and receive response data.
 *
 *@resp: CCID command response data
 *
 * return 0 if success, -1 if failed.
 */
int at_cmd_get_ccid(struct ccid_resp *resp)
{
	int ret;
	int resp_len = 0;
	char *str = NULL;
	char buf[RESP_BUF_SIZE] = {0};
	char cmd_buf[] = "AT+CCID\r";

	if (resp == NULL) {
		ALOGE("ccid_resp is null\n");
		return -1;
	}

	ret = at_cmd_send_recv(cmd_buf, sizeof(cmd_buf), buf, &resp_len);
	if (ret != AT_OK) {
		ALOGE("ccid at_cmd_send_recv fail\n");
		return -1;
	}

	/* parser data: +CCID:89860002190810001367 */
	str = strstr(buf, "OK");
	if (str == NULL) {
		ALOGE("ccid response err %s\n", buf);
		resp->status = AT_ERR;
		return 0;
	}

	resp->status = AT_OK;
	str = strstr(buf, "+CCID:");
	if (str == NULL) {
		memset(resp->id, 0, 20);
		resp->oper = OPER_UNDEFINE;
	} else {
		memcpy(resp->id, buf + 9, 20);

		/* china mobile */
		if (!strncmp(buf + 9, CHINA_MOBILE_1, 6) ||
		    !strncmp(buf + 9, CHINA_MOBILE_2, 6) ||
		    !strncmp(buf + 9, CHINA_MOBILE_3, 6) ||
		    !strncmp(buf + 9, CHINA_MOBILE_4, 6))
			resp->oper = OPER_MOBILE;
		/* china unicom */
		else if (!strncmp(buf + 9, CHINE_UNICOM_1, 6) ||
			 !strncmp(buf + 9, CHINE_UNICOM_2, 6) ||
			 !strncmp(buf + 9, CHINE_UNICOM_3, 6))
			resp->oper = OPER_UNICOM;
		/* china telecom */
		else if (!strncmp(buf + 9, CHINE_TELECOM_1, 6) ||
			 !strncmp(buf + 9, CHINE_TELECOM_1, 6))
			resp->oper = OPER_TELECOM;
		else
			resp->oper = OPER_UNDEFINE;
	}

	return ret;
}

/**
 * at_cmd_get_cpin - send at command AT+CPIN and receive response data.
 *
 *@resp: CCPIN command response data
 *
 * return 0 if success, -1 if failed.
 */
int at_cmd_get_cpin(struct cpin_resp *resp)
{
	int ret = 0;
	int i = 0;
	int resp_len = 0;
	char *str = NULL;
	char buf[RESP_BUF_SIZE] = {0};
	char cmd_buf[] = "AT+CPIN?\r";

	if (resp == NULL) {
		ALOGE("cpin_resp is null\n");
		return -1;
	}

	ret = at_cmd_send_recv(cmd_buf, sizeof(cmd_buf), buf, &resp_len);
	if (ret != AT_OK)
		return -1;

	/* parser data OK */
	str = strstr(buf, "OK");
	if (str == NULL)
		resp->status = AT_ERR;
	else {
		resp->status = AT_OK;
		if (strncmp(buf + 9, "READY", 5) == 0)
			resp->is_ready = 1;

		while (*(buf + 9 + i) != '\n')
			i++;

		memcpy(resp->pin, buf + 9, i);
	}

	return ret;
}

/**
 * at_cmd_get_creg - send at command AT+CREG and receive response data.
 *
 *@resp: CREG command response data
 *
 * return 0 if success, -1 if failed.
 */
int at_cmd_get_creg(struct creg_resp *resp)
{
	int ret = 0;
	int resp_len = 0;
	int off_len = 0;
	char *str = NULL;
	char buf[RESP_BUF_SIZE] = {0};
	char cmd_buf[] = "AT+CREG?\r";

	if (resp == NULL) {
		ALOGE("creg_resp is null\n");
		return -1;
	}

	ret = at_cmd_send_recv(cmd_buf, sizeof(cmd_buf), buf, &resp_len);
	if (ret != AT_OK)
		return -1;

	/* parser data OK */
	str = strstr(buf, "OK");
	if (str == NULL)
		resp->status = AT_ERR;
	else {
		resp->status = AT_OK;
		resp->net_reg = parser_number((buf + 9), &off_len);
		resp->stat = parser_number((buf + 10 + off_len), 0);
	}

	return ret;
}

/**
 * at_cmd_get_csq - send at command AT+CSQ and receive response data.
 *
 *@resp: CSQ command response data
 *
 * return 0 if success, -1 if failed.
 */
int at_cmd_get_csq(struct csq_resp *resp)
{
	int ret;
	int resp_len = 0;
	int off_len = 0;
	char *str = NULL;
	char buf[RESP_BUF_SIZE] = {0};
	char cmd_buf[] = "AT+CSQ\r";

	if (resp == NULL) {
		ALOGE("csq_resp is null\n");
		return -1;
	}

	ret = at_cmd_send_recv(cmd_buf, sizeof(cmd_buf), buf, &resp_len);
	if (ret != AT_OK)
		return -1;

	/* parser data +CSQ: 28,99 */
	str = strstr(buf, "OK");
	if (str == NULL) {
		resp->status = AT_ERR;
		resp->signal = 0;
		resp->ber = 0;
		return 0;
	}

	resp->status = AT_OK;
	str = strstr(buf, "+CSQ:");
	if (str == NULL) {
		resp->signal = 0;
		resp->ber = 0;
	} else {
		resp->signal = parser_number((buf + 8), &off_len);
		resp->ber = parser_number((buf + 9 + off_len), 0);
	}

	return 0;
}

/**
 * at_cmd_get_csq - send at command AT+CGATT and receive response data.
 *
 *@resp: CGATT command response data
 *
 * return 0 if success, -1 if failed.
 */
int at_cmd_get_cgatt(struct cgatt_resp *resp)
{
	int ret = 0;
	int resp_len = 0;
	char *str = NULL;
	char buf[RESP_BUF_SIZE] = {0};
	char cmd_buf[] = "AT+CGATT?\r";

	if (resp == NULL) {
		ALOGE("creg_resp is null\n");
		return -1;
	}

	ret = at_cmd_send_recv(cmd_buf, sizeof(cmd_buf), buf, &resp_len);
	if (ret != AT_OK)
		return -1;

	/* parser data OK */
	str = strstr(buf, "OK");
	if (str == NULL)
		resp->status = AT_ERR;
	else {
		resp->status = AT_OK;
		resp->result = parser_number((buf + 10), 0);
	}

	return ret;
}

/**
 * at_cmd_get_csq - send at command AT+CGATT= and receive response data.
 *
 *@resp: CGATT command response data
 *@connect: 1 - connect, 0 - disconnect
 *
 * return 0 if success, -1 if failed.
 */
int at_cmd_set_cgatt(struct cgatt_resp *resp, int connect)
{
	int ret = 0;
	int resp_len = 0;
	char *str = NULL;
	char buf[RESP_BUF_SIZE] = {0};
	char cmd_buf_c[] = "AT+CGATT=1\r";
	char cmd_buf_d[] = "AT+CGATT=0\r";

	if (resp == NULL) {
		ALOGE("creg_resp is null\n");
		return -1;
	}

	if (connect)
		ret = at_cmd_send_recv(cmd_buf_c, sizeof(cmd_buf_c),
					buf, &resp_len);
	else
		ret = at_cmd_send_recv(cmd_buf_d, sizeof(cmd_buf_d),
					buf, &resp_len);
	if (ret != AT_OK)
		return -1;

	/* parser data OK */
	str = strstr(buf, "OK");
	if (str == NULL)
		resp->status = AT_ERR;
	else
		resp->status = AT_OK;

	return ret;
}

/**
 * at_cmd_get_csq - send at command AT$MYSYSINFO and receive response data.
 *
 *@resp: MYSYSINFO command response data
 *
 * return 0 if success, -1 if failed.
 */
int at_cmd_get_sysinfo(struct sysinfo_resp *resp)
{
	int ret = 0;
	int resp_len = 0;
	int off_len = 0;
	char *str = NULL;
	char buf[RESP_BUF_SIZE] = {0};
	char cmd_buf[] = "AT$MYSYSINFO\r";

	if (resp == NULL) {
		ALOGE("sysinfo_resp is null\n");
		return -1;
	}

	ret = at_cmd_send_recv(cmd_buf, sizeof(cmd_buf), buf, &resp_len);
	if (ret != AT_OK)
		return -1;

	/* parser data OK */
	str = strstr(buf, "OK");
	if (str == NULL) {
		resp->status = AT_ERR;
		resp->mode = 0;
		resp->mnc = 0;
	} else {
		resp->status = AT_OK;
		resp->mode = parser_number((buf + 14), &off_len);
		resp->mnc = parser_number((buf + 15 + off_len), 0);
	}

	return ret;
}

/**
 * at_cmd_get_netmsg - send at command AT+NETMSG and receive response data.
 *
 *@resp: NETMSG command response data
 *
 * return 0 if success, -1 if failed.
 */
int at_cmd_get_netmsg(struct netmsg_resp *resp)
{
	int ret = 0;
	int resp_len = 0;
	int off_len = 0;
	int off_len_1 = 0;
	int off_len_2 = 0;
	int mode_s = 0;
	int mode_e = 0;
	char *str = NULL;
	char buf[RESP_BUF_SIZE] = {0};
	char cmd_buf[] = "AT+NETMSG\r";

	if (resp == NULL) {
		ALOGE("netmsg_resp is null\n");
		return -1;
	}

	ret = at_cmd_send_recv(cmd_buf, sizeof(cmd_buf), buf, &resp_len);
	if (ret != AT_OK)
		return -1;

	/* parser data OK */
	str = strstr(buf, "OK");
	if (str == NULL) {
		resp->status = AT_ERR;
		resp->mcc = 0;
		resp->mnc = 0;
		memset(resp->net_mode, 0, 30);
	} else {
		resp->status = AT_OK;
		resp->mcc = parser_number((buf + 12), &off_len_1);
		if ((resp->mcc == 0) && (off_len_1 == 1)) {
			resp->mnc = 0;
			memcpy(resp->net_mode, "NONE", 4);
		} else {
			resp->mnc = parser_number((buf + 13 + off_len_1),
						  &off_len_2);
			off_len = 14 + off_len_1 + off_len_2;
			while (*(buf + off_len + mode_s) != '"')
				mode_s++;

			off_len = 15 + off_len_1 + off_len_2 + mode_s;
			while (*(buf + off_len + mode_e) != '"')
				mode_e++;

			off_len = 15 + off_len_1 + off_len_2 + mode_s;
			memcpy(resp->net_mode, buf + off_len, mode_e);
		}
	}

	return ret;
}

/**
 * at_cmd_get_gmr - send at command AT+CGSN and receive response data.
 *
 *@resp: AT command response data
 *
 * return 0 if success, -1 if failed.
 */
int at_cmd_get_cgsn(struct common_resp *resp)
{
	int ret = 0;
	int resp_len = 0;
	char *str = NULL;
	char buf[RESP_BUF_SIZE] = {0};
	char cmd_buf[] = "AT+CGSN\r";

	if (resp == NULL) {
		ALOGE("common_resp is null\n");
		return -1;
	}

	ret = at_cmd_send_recv(cmd_buf, sizeof(cmd_buf), buf, &resp_len);
	if (ret != AT_OK)
		return -1;

	/* parser data OK */
	str = strstr(buf, "OK");
	if (str == NULL)
		resp->status = AT_ERR;
	else {
		/* IMEI is 15B length number */
		resp->status = AT_OK;
		memcpy(resp->data, buf + 10, 15);
		resp->len = 15;
	}

	return 0;
}
