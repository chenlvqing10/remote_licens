#ifndef __MD5_H
#define __MD5_H

/**
 * @brief  to get the data
 * @param  *filename: file name
 * @param  *md5_str: data
 * @param  str_len: data size
 * @retval 0 if success
 */
int check_file_md5(char *file_name, char *md5_src);

/**
 * @brief  check file
 * @param  *file_name: file name
 * @param  *md5_src: data
 * @retval -1 if faile
 */
int get_md5_str(const char *filename, char *md5_str, int str_len);

#endif
