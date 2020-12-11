/*
 * sound_svc_test.c - Get the private data from private partition
 *
 * Copyright (C) 2016-2019, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <iostream>
#include <fstream>
#include <string>

extern "C" {

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define LOG_TAG		"tts"
#include <log/log.h>

#include "sound_svc.h"

}

#define CLEAN_ERASE_H "stty erase ^H"

using namespace std;

#define TTS_STRING_LEN 1024

typedef struct sound_test_s{
	char status;
	char *test_file;
	int line_no;
	int line_count;
	int replay_count;
	FILE *soucces_fp;
	FILE *fail_fp;
	char tts_string[TTS_STRING_LEN];
} sound_test_t;

sound_test_t *test;
static sig_atomic_t s_signal_received = 0;

static void signal_handler(int sig_num)
{
	signal(sig_num, signal_handler);
	s_signal_received = sig_num;
}

int is_file_exist(char *file_path)
{
	if (!file_path)
		return -1;

	if (access(file_path, F_OK) != -1)
		return 0;

	return -1;
}

int read_count_line(char* filename)
{
	ifstream in;
	in.open(filename);
	int line = 0;
	while (in.getline(test->tts_string, TTS_STRING_LEN)) {
		line++;
	}
	in.close();
	return line;
}

void read_line_data(char* filename, int linenum, char* data)
{
	ifstream in;
	in.open(filename);

	int line = 0;
	while (in.getline(data, TTS_STRING_LEN)) {
		if (linenum == line){
			break;
		}
		line ++ ;
	}
	in.close();
}

int tts_play(char *tts_string, int len)
{
	tts_data_t tts;

	memset(&tts, 0 ,sizeof(tts_data_t));

	printf("tts_string: %s len: %d\n", tts_string, len);

	for (unsigned int i = 0; i < strlen(tts_string); i++) {
		printf("0x%02x ", tts_string[i]);
	}
	printf("\n\n");

	tts.format = TTS_FORMAT_UTF8;
	tts.string = tts_string;
	tts.len = len;
	tts.greet_param.pos = GREETING_END;
	tts.greet_id = GREET_WORK;

	/* for temp test */
	tts.greet_param.language_type = 0;
	tts.greet_param.greeting_num = 0;
	tts.greet_param.pos = GREETING_CLOSE;

	return sound_play_tts(&tts);
}

int main(int argc, char *argv[])
{
	system(CLEAN_ERASE_H);

	int ret = -1;
	int is_exits = -1;
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);

	test = (sound_test_t*)malloc(sizeof(sound_test_t));
	if (!test) {
		printf("malloc 失败\n");
		return -1;
	}
	memset(test, 0, sizeof(sound_test_t));

	if(sound_svc_init()){
		printf("tts 初始化失败\n");
		return -1;
	}

	printf("注意 TTS 字符串目前只支持UTF-8\n");

	//sound_play_file("/usr/share/aio/res/sound/bind.wav");
	if (argc == 1) {
		while (s_signal_received == 0) {
			memset(&test->tts_string, 0, TTS_STRING_LEN);
			printf("请输入测试的字符串\n");
			printf("text:");
			fgets(test->tts_string, TTS_STRING_LEN, stdin);
			tts_play(test->tts_string, strlen(test->tts_string));
		}
	} else if (argc == 2 || argc == 3) {
		test->test_file = argv[1];
		is_exits = is_file_exist(test->test_file);
		if (is_exits) {
			printf("请检查输入文件路径: %s\n", test->test_file);
			goto exit;
		}

		test->line_count = read_count_line(test->test_file);
		printf("文件行数: %d\n", test->line_count);

		if (argc == 3) {
			printf("程序将重复循环播放: %s 次数: %d\n", test->test_file, atoi(argv[2]));
		}

		if (argc == 2) {
			printf("需要重复这次播放输入 'j'(下一个)\n");
			printf("需要继续播放输入 'k'(上一个)\n");
		}

		test->soucces_fp = fopen("/data/tts_success.txt", "ab+");
		if (!test->soucces_fp) {
			printf("创建tts_success.txt文件失败\n");
			goto exit;
		}

		test->fail_fp = fopen("/data/tts_fail.txt", "ab+");
		if (!test->fail_fp) {
			printf("创建tts_success.txt文件失败\n");
			goto exit;
		}

		while (s_signal_received == 0) {
			memset(&test->tts_string, 0, TTS_STRING_LEN);
			printf("line_no = %d\n", test->line_no);
			read_line_data(test->test_file, test->line_no, test->tts_string);
			if (strlen(test->tts_string) > 0) {
				ret = tts_play(test->tts_string, strlen(test->tts_string));
				if (ret == 0) {
					fwrite(test->tts_string, 1, strlen(test->tts_string), test->soucces_fp);
					fputc('\n', test->soucces_fp);
				} else if (ret == -1){
					/* has err */
					fwrite(test->tts_string, 1, strlen(test->tts_string), test->fail_fp);
					fputc('\n', test->fail_fp);
				} else if (ret == -2) {
					sleep(5);
					continue;
				} else {
					printf("unknow err\n");
				}
			}

			if (argc == 2) {
				char str[32] = {0};
				printf("请输入:");
				fgets(str, 32, stdin);

				if (str[0] == 'j') {
					test->status = 'j';
					if (test->line_no < test->line_count)
						test->line_no++;
					else
						test->line_no = 0;
				} else if (str[0] == 'k') {
					test->status = 'k';
					if (test->line_no > 0)
						test->line_no--;
					else
						test->line_no = test->line_count;
				}
			} else {
				if (test->line_no < test->line_count)
					test->line_no++;
				else {
					test->line_no = 0;
					test->replay_count++;
					printf("重复播放的次数: %d\n", test->replay_count);
					if (test->replay_count >= atoi(argv[2])) {
						printf("退出程序:'Ctrl + c' + Enter \n");
						goto exit;
					}
					sleep(2);
				}
			}
		}
	}

exit:

	printf("正在退出\n");
	sound_svc_deinit();
	printf("已退出\n");

	if (test->soucces_fp) {
		fputc('\n', test->soucces_fp);
		fclose(test->soucces_fp);
	}
	if (test->fail_fp) {
		fputc('\n', test->fail_fp);
		fclose(test->fail_fp);
	}
	return 0;
}



