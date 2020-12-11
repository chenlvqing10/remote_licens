#include <pthread.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <dirent.h>

#include "update_svc.h"
#include "hotplug.h"
#include "md5.h"
#include "system_update.h"
#include "file_ops.h"

#define LOG_TAG  "UPDATE_SVC"
#include <log/log.h>

#define MAX_UPDATE_FILE_NAME_LENGTH 64
#define MD5_LENGTH 32

static int update_device_in_operation = 0;
static char update_file_name[MAX_UPDATE_FILE_NAME_LENGTH];
static char md5[MAX_UPDATE_FILE_NAME_LENGTH];
const char *update_file_head = "EUPGRADE-";
const char *udisk_dir = "/mnt/usb/";
const char *dst_file_name = "/cache/package.efw";

/**
 * find_pkg - find valid firmware for upgrade.
 *
 * Returns 0  if find, -1 if failure, 1 not find
 */
int find_pkg(void)
{
	const char *base_path = udisk_dir;
	DIR *dir;
	struct dirent *ptr;

	dir = opendir(base_path);
	if (dir == NULL) {
		ALOGE("%s:Open dir error!\n", __func__);
		return -1;
	}

	while ((ptr = readdir(dir)) != NULL) {
		if (strcmp(ptr->d_name, ".") == 0
			|| strcmp(ptr->d_name, "..") == 0)
			continue;
		else if (ptr->d_type == DT_REG) {
/*           ALOGD("%s:DT_REG: d_name:%s/%s\n", __func__, basePath,ptr->d_name);
*			filter file ,name format like EUPGRADE-$(MD5).pkg
*			example : EUPGRADE-(79054025255fb1a26e4bc422aef54eb4).pkg */
			size_t cmp_len = strlen(update_file_head);
			if (strncmp(ptr->d_name, update_file_head, cmp_len) == 0) {
				ALOGD("%s:DT_REG: %s\n", __func__, ptr->d_name);
				ALOGD("%s: head length: %d\n", __func__, cmp_len);
				memset(update_file_name, 0x00, MAX_UPDATE_FILE_NAME_LENGTH);
				memset(md5, 0x00, MAX_UPDATE_FILE_NAME_LENGTH);
				sprintf(update_file_name, "%s", udisk_dir);
				sprintf(update_file_name + strlen(udisk_dir),
						"%s", ptr->d_name);
				snprintf(md5, MD5_LENGTH + 1, "%s", (ptr->d_name) + cmp_len);
				ALOGD("%s:filename: %s\n", __func__, update_file_name);
				ALOGD("%s:MD5: %s\n", __func__, md5);

				int rc = check_file_md5(update_file_name, md5);
				if (rc == 0) {
					ALOGD("%s[%d] find upgrade pkg is OK\n",
							__func__, __LINE__);
					#if 0
					ALOGD("%s: copy file to /cache!\n", __func__);
					int ret = file_copy(update_file_name,
							(char *)dst_file_name);
					if (ret != 0) {
						ALOGE("%s: copy file error!\n", __func__);
						return -1;
					}
					#endif
					closedir(dir);
					return 0;
				}
			}
		}
	}
	closedir(dir);
	return 1;
}

/**
 * copy_pkg - copy firmware pkg of upgrade to cache of directory.
 *
 * Returns 0  if find, -1 if failure, 1 not find
 */
int copy_pkg(const char *base_path)
{
	DIR *dir;
	struct dirent *ptr;

	dir = opendir(base_path);
	if (dir == NULL) {
		ALOGE("%s:Open dir error!\n", __func__);
		return -1;
	}

	while ((ptr = readdir(dir)) != NULL) {
		if (strcmp(ptr->d_name, ".") == 0
			|| strcmp(ptr->d_name, "..") == 0)
			continue;
		else if (ptr->d_type == DT_REG) {
			size_t cmp_len = strlen(update_file_head);
			if (strncmp(ptr->d_name, update_file_head, cmp_len) == 0) {
				ALOGD("%s:DT_REG: %s\n", __func__, ptr->d_name);
				ALOGD("%s: head length: %d\n", __func__, cmp_len);
				memset(update_file_name,
						0x00, MAX_UPDATE_FILE_NAME_LENGTH);
				memset(md5, 0x00, MAX_UPDATE_FILE_NAME_LENGTH);
				sprintf(update_file_name, "%s", base_path);
				sprintf(
					update_file_name + strlen(base_path),
					"%s", ptr->d_name);
				snprintf(md5, MD5_LENGTH + 1, "%s",
						(ptr->d_name) + cmp_len);
				ALOGD("%s:filename: %s\n", __func__, update_file_name);
				ALOGD("%s:MD5: %s\n", __func__, md5);

				int rc = check_file_md5(update_file_name, md5);
				if (rc == 0) {
					ALOGD("%s: copy file to /cache!\n", __func__);
					int ret = file_copy(update_file_name,
							(char *)dst_file_name);
					if (ret != 0) {
						ALOGE("%s: copy file error!\n", __func__);
						closedir(dir);
						return -1;
					}

					ret = check_file_md5((char *)dst_file_name, md5);
					closedir(dir);
					return ret;
				}
			}
		}
	}
	closedir(dir);

	return 1;
}

/**
 * update_device_start - upgrade device firmware.
 *
 * Returns 0  if success, other if failure.
 */
int update_device_start(void)
{
	return ota_start();
}

/**
 * update_device_by_path - upgrade device firmware.
 *
 * Returns 0  if find, -1 if failure, 1 not find, 2 in_operation
 */
int update_device_copy_pkg(const char *base_path)
{
	int ret = 0;

	if (update_device_in_operation == 1)
		return 2;

	update_device_in_operation = 1;

	ret = copy_pkg(base_path);
	if (ret != 0)
		update_device_in_operation = 0;

	return ret;
}

int update_device_cancel(void)
{
	update_device_in_operation = 0;
	return 0;
}

