#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#define LOG_TAG "MD5"
#include <log/log.h>
#define DD_UTIL_DEBUG 1
#include "dd_md5_i.h"
#include "dd_md5.h"

/**
 * @brief  to get the data
 * @param  *filename: file name
 * @param  *md5_str: data
 * @param  str_len: data size
 * @retval 0 if success
 */
int get_md5_str(const char *filename, char *md5_str, int str_len)
{
	FILE *fp;
	struct MD5Context md5;
	unsigned char buffer[1024];
	int pos, length, len, i;
	unsigned char decrypt[16];
	if (!filename || !md5_str || (str_len <= 32))
		return -1;
	fp = fopen(filename, "rb");
	errno = 0;
	if (!fp) {
		ALOGE("Can not open(%s), errno %d!\n", filename, errno);
		return -1;
	}
	fseek(fp, 0, SEEK_END);
	length = ftell(fp);
	pos = 0;
	fseek(fp, 0, SEEK_SET);
	MD5Init(&md5);
	while (pos < length) {
		len = fread(buffer, 1, sizeof(buffer), fp);
		if (len > 0) {
			MD5Update(&md5, buffer, len);
			pos += len;
		} else {
			fclose(fp);
			ALOGD("%s[%d] fread() ret %d", __func__, __LINE__, len);
			return -1;
		}
	}
	MD5Final(decrypt, &md5);
	fclose(fp);
	for (i = 0; i < sizeof(decrypt); i++) {
		sprintf(md5_str + i * 2, "%02x", decrypt[i]);
	}
	return 0;
}

/**
 * @brief  check file
 * @param  *file_name: file name
 * @param  *md5_src: data
 * @retval -1 if faile
 */
int check_file_md5(char *file_name, char *md5_src)
{
	int ret = -1;
	char md5_str[40];
	if (file_name == NULL)
		return -1;
	memset(md5_str, 0, sizeof(md5_str));
	ret = get_md5_str(file_name, md5_str, sizeof(md5_str));
	if (ret == 0) {
		ALOGD_IF(DD_UTIL_DEBUG,
			"%s md5 str: %s\n", file_name, md5_str);
		if (strcasecmp(md5_src, md5_str) == 0)
			return 0;
	}
	return -1;
}

