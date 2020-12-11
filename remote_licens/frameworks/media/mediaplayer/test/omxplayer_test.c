#define DBG_LEVEL         DBG_INFO
#define LOG_TAG           "omxplayer_test"

#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <oscl.h>
#include "display/oscl_display.h"

#include "omx_mediaplayer.h"
#include "omxplayer_test.h"

#define TEST_PAUSE_COUNT 10000000
#define TEST_CYCLE_COUNT 10000000
#define TEST_SEEK_COUNT  10000000

#define TEST_FAST_SEEK_STEP (5 * 1000)
#define TEST_DISPLAY_WIDTH 320
#define TEST_DISPLAY_HEIGHT 800

typedef enum OmxMptLoopMode {
	OMXMPT_LOOP_NONE = 0,
	OMXMPT_LOOP_FILE,
	OMXMPT_LOOP_DIR,
	OMXMPT_LOOP_USERDEF,
} OmxMptLoopMode_e;

typedef enum OmxMpTCmd {
	OMXMPT_CMD_INVALID = 0,
	OMXMPT_CMD_PREPARE,
	OMXMPT_CMD_START,
	OMXMPT_CMD_STOP,
	OMXMPT_CMD_SEEK,
	OMXMPT_CMD_PAUSE,
	OMXMPT_CMD_SW_AUDIO_TRACK,
	OMXMPT_CMD_ROTATE_EN,
	OMXMPT_CMD_ROTATE,
	OMXMPT_CMD_UPDATE_TIME,
	OMXMPT_CMD_EXIT,
} OmxMpTCmd_e;

typedef enum OmxMpTSeekCmd {
	CMD_SEEK_SET = 0,
	CMD_SEEK_CUR,
	CMD_SEEK_END,
} OmxMpTSeekCmd_e;

typedef struct OmxMpTest {
	void *player;
	char **files;
	int file_idx;
	int file_cnt;
	char *file_cur;
	OmxMptLoopMode_e loop;
	int rot_en;

	oscl_queue_t cmd_queue;
	sem_t cmd_sem;
	sem_t on_seek_completed_sem;
	sem_t on_completed_sem;
	sem_t on_prepared_sem;
	sem_t seek_completed_sem;
	sem_t prepared_sem;

	int cmd_thd_running;

	omxmp_audio_info_t ainfo;
	long duration_ms;
	int fast_seek_dir;
	int exit_flag;
} OmxMpTest_t;

static OmxMpTest_t g_omx_mp;


static int file_open(const char *path, int flag)
{
	return open(path, flag);
}

static long file_read(int fd, void *buf, unsigned long size)
{
	return read(fd, buf, size);
}

static int64_t file_seek(int fd, int64_t offset, int whence)
{
	return lseek(fd, offset, whence);
}

static int file_close(int fd)
{
	return close(fd);
}

int omxmpt_send_cmd(OmxMpTest_t *mp, OmxMpTCmd_e cmd, int param1, void *msg_data)
{
	oscl_message_t message;
	int ret;

	message.comp = mp;
	message.msg_type = cmd;
	message.para1 = param1;
	message.data = msg_data;
	ret = oscl_message_put(&mp->cmd_queue, &message);
	if (ret) {
		OSCL_LOGE("oscl_message_put error!");
		return -1;
	}

	sem_post(&mp->cmd_sem);

	return 0;
}

static void omxmpt_on_completion(void *handle)
{
	OmxMpTest_t *mp = &g_omx_mp;

	OSCL_LOGI("------on_completion--------");

	if (mp->loop == OMXMPT_LOOP_NONE) {
		omxmpt_send_cmd(mp, OMXMPT_CMD_STOP, 0, NULL);
	} else if (mp->loop == OMXMPT_LOOP_DIR) {
		omxmpt_send_cmd(mp, OMXMPT_CMD_STOP, 0, NULL);
		mp->file_idx++;
		mp->file_idx %= mp->file_cnt;
		mp->file_cur = mp->files[mp->file_idx];
		omxmpt_send_cmd(mp, OMXMPT_CMD_PREPARE, 0, mp->file_cur);
		omxmpt_send_cmd(mp, OMXMPT_CMD_START, 0, NULL);
	} else if (mp->loop == OMXMPT_LOOP_FILE) {
		omxmpt_send_cmd(mp, OMXMPT_CMD_STOP, 0, NULL);
		omxmpt_send_cmd(mp, OMXMPT_CMD_PREPARE, 0, mp->file_cur);
		omxmpt_send_cmd(mp, OMXMPT_CMD_START, 0, NULL);
	}

	sem_post(&mp->on_seek_completed_sem);
	sem_post(&mp->on_completed_sem);
}

static void omxmpt_on_error(void *handle, int omx_err)
{
	OmxMpTest_t *mp = &g_omx_mp;

	OSCL_LOGE("------on_error %x--------\n", omx_err);
	omxmpt_send_cmd(mp, OMXMPT_CMD_STOP, 0, NULL);

	sem_post(&mp->on_prepared_sem);
	sem_post(&mp->on_seek_completed_sem);
	sem_post(&mp->on_completed_sem);
	mp->exit_flag = 1;
}

static void omxmpt_on_prepared(void *handle)
{
	OmxMpTest_t *mp = &g_omx_mp;

	OSCL_LOGI("------on_prepared--------\n");
	sem_post(&mp->on_prepared_sem);
}

static void omxmpt_on_seek_complete(void *handle)
{
	OmxMpTest_t *mp = &g_omx_mp;

	OSCL_LOGI("------on_seek_complete--------\n");
	sem_post(&mp->on_seek_completed_sem);
}
static void omxmpt_on_video_size_changed(void *handle, int width, int height)
{
	OSCL_LOGI("on_video_size_changed width=%d, height=%d\n", width, height);
}
static void omxmpt_on_timed_text(void *handle, char *text)
{
	OSCL_LOGI("------on_timed_text %s--------\n", text);
}

static mp_callback_ops_t g_cb_ops = {
	.on_completion            = omxmpt_on_completion,
	.on_error                 = omxmpt_on_error,
	.on_prepared              = omxmpt_on_prepared,
	.on_seek_complete         = omxmpt_on_seek_complete,
	.on_video_size_changed    = omxmpt_on_video_size_changed,
	.on_timed_text            = omxmpt_on_timed_text,
};

static void *omxmpt_cmd_thread(void *param)
{
	OmxMpTest_t *mp = param;
	oscl_message_t msg;
	int ret;

	while (mp->cmd_thd_running) {
		sem_wait(&mp->cmd_sem);
		ret = oscl_message_get(&mp->cmd_queue, &msg);
		if (ret) {
			OSCL_LOGE("get cmd failed!");
			continue;
		}

		switch (msg.msg_type) {
		case OMXMPT_CMD_PREPARE: {
			char *filename = msg.data;
			OSCL_LOGI("start to play %s", filename);
			ret = omxmp_set_data_source(mp->player, filename);
			if (ret != 0) {
				OSCL_LOGE("set data url error!");
				break;
			}
#if 0
			omxmp_io_interface_t intf;
			intf.open = file_open;
			intf.read = file_read;
			intf.lseek = file_seek;
			intf.close = file_close;
			omxmp_set_io_interface(mp->player, &intf);
#endif
			ret = omxmp_prepare(mp->player);
			if (ret != 0) {
				OSCL_LOGE("omxmp_prepare error!");
				break;
			}
			sem_wait(&mp->on_prepared_sem);
			mp->duration_ms = omxmp_get_duration(mp->player);
			if (mp->duration_ms < 0)
				OSCL_LOGE("omxmp_get_duration error!");
			ret = omxmp_get_audio_info(mp->player, &mp->ainfo);
			if (ret != 0)
				OSCL_LOGE("omxmp_get_audio_info error!");
			sem_post(&mp->prepared_sem);
			break;
		}
		case OMXMPT_CMD_START: {
			ret = omxmp_start(mp->player);
			if (ret != 0)
				OSCL_LOGE("omxmp_start error!");
			break;
		}
		case OMXMPT_CMD_STOP: {
			omxmp_stop(mp->player);
			omxmp_reset(mp->player);
			break;
		}
		case OMXMPT_CMD_PAUSE: {
			omxmp_pause(mp->player);
			break;
		}
		case OMXMPT_CMD_SEEK: {
			if (msg.para1 == CMD_SEEK_CUR) {
				long oripos, curpos, tarpos, step;

				step = (long)msg.data;
				if (step == 0) {
					OSCL_LOGW("seek step 0");
					break;
				}
				tarpos = oripos = omxmp_get_current_position(mp->player);
				while (1) {
					tarpos += step;
					if ((step > 0 && tarpos >
						mp->duration_ms - 20 * 1000) ||
						(step < 0 && tarpos < 0)) {
						mp->fast_seek_dir *= -1;
						break;
					}
					ret = omxmp_seek_to(mp->player, tarpos);
					if (ret != 0) {
						OSCL_LOGE("seek error!");
						break;
					}
					sem_wait(&mp->on_seek_completed_sem);
					curpos = omxmp_get_current_position(mp->player);
					if (step > 0 && curpos > oripos)
						break;
					else if (curpos < oripos)
						break;
				}
			} else {
				ret = omxmp_seek_to(mp->player, (long)msg.data);
				if (ret != 0)
					OSCL_LOGE("seek error!");
				else
					sem_wait(&mp->on_seek_completed_sem);
			}
			sem_post(&mp->seek_completed_sem);
			break;
		}
		case OMXMPT_CMD_SW_AUDIO_TRACK: {
			ret = omxmp_set_audio_track(mp->player, msg.para1);
			if (ret != 0)
				OSCL_LOGE("omxmp_set_audio_track error!");
			break;
		}
		case OMXMPT_CMD_UPDATE_TIME: {
			long curpos;
			int sec, min, hour;
			curpos = omxmp_get_current_position(mp->player);
			if (curpos < 0) {
				OSCL_LOGE("omxmp_get_current_position error!");
				break;
			}
			curpos /= 1000;
			sec = curpos % 60;
			min = (curpos / 60) % 60;
			hour = curpos / 3600;
			printf("current time: %02d:%02d:%02d\n", hour, min, sec);
			break;
		}
		case OMXMPT_CMD_ROTATE: {
			ret = omxmp_set_rotation(mp->player, msg.para1);
			if (ret != 0)
				OSCL_LOGE("omxmp_set_rotation error!");
			break;
		}
		case OMXMPT_CMD_ROTATE_EN: {
			ret = omxmp_enable_rotation(mp->player, msg.para1);
			if (ret != 0)
				OSCL_LOGE("omxmp_enable_rotation error!");
			break;
		}
		default:
			OSCL_LOGW("unknown message type %d", msg.msg_type);
			break;
		}
	}

	OSCL_LOGI("exit");
	return NULL;
}

#include <iostream_plugin.h>
char *media_probe(file_stream_para_t *para);
static int avfilelist_init(const char *path, char ***filelist)
{
	DIR *dir = NULL;
	struct dirent *entry = NULL;
	char **flist = NULL;
	char *fname = NULL;
	int cnt = 0;
	file_stream_para_t para;

	fname = oscl_malloc(4096);
	if (fname == NULL) {
		OSCL_LOGE("alloc filename err");
		return -1;
	}
	dir = opendir(path);
	if (dir == NULL) {
		OSCL_LOGE("open dir err");
		goto ERR_EXIT;
	}

	para.io_intf.open = file_open;
	para.io_intf.read = file_read;
	para.io_intf.lseek = file_seek;
	para.io_intf.close = file_close;
	while ((entry = readdir(dir))) {
		OSCL_LOGD("%s d_type=%d", entry->d_name, entry->d_type);
		if (entry->d_type == DT_REG || entry->d_type == DT_LNK) {
			snprintf(fname, 4096, "%s/%s", path, entry->d_name);
			para.filename = fname;
			if (media_probe(&para) != NULL) {
				char **tmp;
				OSCL_LOGI("get %d av file %s", cnt, fname);
				tmp = realloc(flist, sizeof(char *) * (cnt + 1));
				if (tmp == NULL) {
					OSCL_LOGE("realloc err");
					break;
				}
				flist = tmp;
				flist[cnt] = oscl_strdup(fname);
				if (flist[cnt] == NULL) {
					OSCL_LOGE("strdup err");
					break;
				}
				cnt++;
			}
		}
	}
	closedir(dir);
	oscl_free(fname);
	*filelist = flist;
	OSCL_LOGI("get total %d av files", cnt);
	return cnt;

ERR_EXIT:
	if (dir != NULL)
		closedir(dir);
	if (fname != NULL)
		oscl_free(fname);
	return -1;
}

static void avfilelist_deinit(char **filelist, int num)
{
	int i;

	if (filelist == NULL)
		return;
	for (i = 0; i < num; i++)
		oscl_free(filelist[i]);
	oscl_free(filelist);
}

static void dir_auto_test(OmxMpTest_t *mp)
{
	char *fname;
	int i;
	int ret;

	fname = mp->files[mp->file_idx];
	while (mp->file_idx < mp->file_cnt) {
		OSCL_LOGI("%d/%d testing", mp->file_idx, mp->file_cnt);
		fname = mp->files[mp->file_idx];
		omxmpt_send_cmd(mp, OMXMPT_CMD_PREPARE, 0, fname);
		omxmpt_send_cmd(mp, OMXMPT_CMD_START, 0, NULL);
		for (i = 0; i < 2; i++) {
			ret = oscl_sem_timedwait_ms(&mp->on_completed_sem, 5000);
			if (ret == 0)
				goto FILE_END;
			if (mp->exit_flag)
				goto ERR_EXIT;

			omxmpt_send_cmd(mp, OMXMPT_CMD_PAUSE, 0, NULL);
			ret = oscl_sem_timedwait_ms(&mp->on_completed_sem, 2000);
			if (ret == 0)
				goto FILE_END;
			if (mp->exit_flag)
				goto ERR_EXIT;

			omxmpt_send_cmd(mp, OMXMPT_CMD_START, 0, NULL);
			ret = oscl_sem_timedwait_ms(&mp->on_completed_sem, 3000);
			if (ret == 0)
				goto FILE_END;
			if (mp->exit_flag)
				goto ERR_EXIT;
		}

		for (i = 0; i < 2; i++) {
			omxmpt_send_cmd(mp, OMXMPT_CMD_SEEK, CMD_SEEK_CUR, (void *)5000);
			sem_wait(&mp->seek_completed_sem);
			ret = oscl_sem_timedwait_ms(&mp->on_completed_sem, 3000);
			if (ret == 0)
				goto FILE_END;
			if (mp->exit_flag)
				goto ERR_EXIT;
		}

		for (i = 0; i < 2; i++) {
			omxmpt_send_cmd(mp, OMXMPT_CMD_SEEK, CMD_SEEK_CUR,
				(void *)(-5000));
			sem_wait(&mp->seek_completed_sem);
			ret = oscl_sem_timedwait_ms(&mp->on_completed_sem, 3000);
			if (ret == 0)
				goto FILE_END;
			if (mp->exit_flag)
				goto ERR_EXIT;
		}
FILE_END:
		omxmpt_send_cmd(mp, OMXMPT_CMD_STOP, 0, NULL);
		mp->file_idx++;
	}
	OSCL_LOGI("test finish");
	return;

ERR_EXIT:
	OSCL_LOGE("error exit");
	return;
}

static void show_help(void)
{
	const char *arg_format = "  %-4s%-24s %s%s\n";

	printf("usage: omxplayer [argument]\n");
	printf("arguments:\n");
	printf(arg_format, "-f", "--file filename", "set input filename", "");
	printf(arg_format, "-p", "--prepare", "prepare", "");
	printf(arg_format, "-b", "--start", "start player", "");
	printf(arg_format, "-e", "--stop", "stop player", "");
	printf(arg_format, "-a", "--pause", "pause player", "");
	printf(arg_format, "-q", "--quit", "quit player", "");
	printf(arg_format, "-k", "--seek", "seek to a certan position", "");
	printf(arg_format, "-P", "--test_pause", "test_pause for 10000000 times", "");
	printf(arg_format, "-B", "--test_dir_pause",
		"pause test loop for all file in the direcotry", "");
	printf(arg_format, "-S", "--test_seek", "test seek", "");
	printf(arg_format, "-C", "--test_cycle",
		"play & close the file for 10000000 times", "");
	printf(arg_format, "-L", "--test_loop", "test loop", "");
	printf(arg_format, "-D", "--test_dir", "play the file in the direcotry", "");
	printf(arg_format, "-Y", "--test_play", "play the file for one time", "");
	printf(arg_format, "-R", "--test_rotate", "test set rotate mode", "");
	printf(arg_format, "-A", "--test_audio_track", "test switch audio track", "");
	printf(arg_format, "-V", "--set_volume", "set_volume", "");
	printf(arg_format, "-J", "--test_jump", "test fast forward or backword", "");
	printf(arg_format, "-E", "--test_auto",
		"test play/pause/seek for all file in directory", "");
	printf(arg_format, "-h", "--help", "show this help info", "");
}

static struct option g_long_options[] = {
	{"file", 1, 0, 'f'},
	{"prepare", 0, 0, 'p'},
	{"start", 0, 0, 'b'},
	{"stop", 0, 0, 'e'},
	{"pause", 0, 0, 'a'},
	{"quit", 0, 0, 'q'},
	{"seek", 1, 0, 'k'},
	{"release", 0, 0, 'r'},
	{"rotate", 1, 0, 't'},
	{"test_pause", 1, 0, 'P'},
	{"test_dir_pause", 1, 0, 'B'},
	{"test_seek", 1, 0, 'S'},
	{"test_cycle", 1, 0, 'C'},
	{"test_loop", 1, 0, 'L'},
	{"test_dir", 1, 0, 'D'},
	{"test_play", 1, 0, 'Y'},
	{"set_volume", 1, 0, 'V'},
	{"test_rotate", 1, 0, 'R'},
	{"test_audio_track", 1, 0, 'A'},
	{"test_jump", 1, 0, 'J'},
	{"test_auto", 1, 0, 'E'},
	{"help", 0, 0, 'h'}
};

static const char *g_short_opts = "f:k:pbeaqhrt:P:C:L:S:D:Y:T:R:A:V:J:B:E:";
int omxplayer_test(int argc, char *argv[])
{
	int          ret = 0;
	int          c = 0;
	int          index;
	disp_size_t  scn_size;
	void         *disp_hdl;
	omxmp_win_t  win;

	OmxMpTest_t *mp = &g_omx_mp;
	pthread_attr_t cmd_thd_attr;
	pthread_t  cmd_thd_id;

	if (mp->player == NULL) {
		mp->player = omxmp_create(&g_cb_ops);
		if (mp->player == NULL) {
			OSCL_LOGE("create omxplayer error!\n");
			return -1;
		}
		/* get screen size */
		memset(&scn_size, 0, sizeof(scn_size));
		disp_hdl = oscl_open_disp_engine();
		oscl_param_check(disp_hdl != NULL, -1, "open disp engine err\n");
		ret = oscl_disp_get_screen_size(disp_hdl, &scn_size);
		oscl_param_check(ret == OMX_ErrorNone, -1, "get screen size err\n");
		oscl_close_disp_engine(disp_hdl);
		OSCL_LOGI("scrn w %d, h %d\n", scn_size.width, scn_size.height);

		/* set win */
#ifdef ARCH_LOMBO_N7V1_CDR
		/* for cdr */
		win.left = 0;
		win.top  = 240;
		win.width = TEST_DISPLAY_WIDTH;
		win.height = TEST_DISPLAY_HEIGHT;
#else
		win.left = 0;
		win.top  = 0;
		win.width = scn_size.width;
		win.height = scn_size.height;
#ifndef OMXPLAYER_TEST
		_get_win(&win, &scn_size);
#endif
#endif

		ret = omxmp_set_window(mp->player, &win);
		oscl_param_check(ret == 0, -1, "set win err\n");
		/* set scaling mode */
		ret = omxmp_set_scaling_mode(mp->player,
			VDISP_WINDOW_FULL_SCREEN_VIDEO_RATIO);
		oscl_param_check(ret == 0, -1, "set scale mode err\n");
		/* enable or disable rotation */
		ret = omxmp_enable_rotation(mp->player, 0);
		oscl_param_check(ret == 0, -1, "enable rotation err");

		oscl_queue_init(&mp->cmd_queue);
		sem_init(&mp->cmd_sem, 0, 0);
		sem_init(&mp->on_seek_completed_sem, 0, 0);
		sem_init(&mp->on_completed_sem, 0, 0);
		sem_init(&mp->on_prepared_sem, 0, 0);
		sem_init(&mp->seek_completed_sem, 0, 0);
		sem_init(&mp->prepared_sem, 0, 0);
		mp->cmd_thd_running = 1;
		/* init thread attr for test thread */
		pthread_attr_init(&cmd_thd_attr);
		pthread_attr_setstacksize(&cmd_thd_attr, 0x2000);
		ret = pthread_create(&cmd_thd_id, &cmd_thd_attr, omxmpt_cmd_thread, mp);
		if (ret != 0) {
			OSCL_LOGE("create cmd thread error(%s)!", strerror(errno));
			mp->cmd_thd_running = 0;
			goto EXIT;
		}

		mp->files = NULL;
		mp->file_cnt = 0;
		mp->file_idx = 0;
		mp->file_cur = NULL;
		mp->exit_flag = 0;
	}

	while (1) {
		c = getopt_long(argc, argv, g_short_opts, g_long_options, &index);
		if (c == -1) {
			OSCL_LOGI("get args end, exit!\n");
			break;
		}
		OSCL_LOGI("option:%c, optarg:%s", c, (optarg == NULL) ? "null" : optarg);
		switch (c) {
		case 'h':
			show_help();
			break;
		case 'f':
			ret = omxmp_set_data_source(mp->player, optarg);
			break;
		case 'p': {
			ret = omxmp_prepare(mp->player);
			break;
		}
		case 'b': {
			ret = omxmp_start(mp->player);
			break;
		}
		case 'e': {
			ret = omxmp_stop(mp->player);
			break;
		}
		case 'a': {
			ret = omxmp_pause(mp->player);
			break;
		}
		case 'q': {
			omxmp_reset(mp->player);
			break;
		}
		case 'k': {
			int position = atoi(optarg);
			OSCL_LOGI("seek to %d s\n", position);
			ret = omxmp_seek_to(mp->player, (long)(position*1000));
			break;
		}
		case 'r': {
			omxmp_release(mp->player);
			mp->player = NULL;
			break;
		}
		case 't': {
			int rotate = atoi(optarg);
			OSCL_LOGI("set rotate %d\n", rotate);
			omxmp_set_rotation(mp->player, rotate);
			break;
		}
		case 'v': {
			omxmp_set_playback_volume(mp->player, atoi(optarg));
			break;
		}
		case 'P': {
			int count = 0;
			mp->loop = OMXMPT_LOOP_FILE;
			mp->file_cur = optarg;
			omxmpt_send_cmd(mp, OMXMPT_CMD_PREPARE, 0, mp->file_cur);
			omxmpt_send_cmd(mp, OMXMPT_CMD_START, 0, NULL);
			while (count++ < TEST_PAUSE_COUNT && !mp->exit_flag) {
				oscl_mdelay(3000);
				OSCL_LOGI("%d pause loop", count);
				omxmpt_send_cmd(mp, OMXMPT_CMD_PAUSE, 0, NULL);
				oscl_mdelay(2000);
				omxmpt_send_cmd(mp, OMXMPT_CMD_START, 0, NULL);
			}
			omxmpt_send_cmd(mp, OMXMPT_CMD_STOP, 0, NULL);
			break;
		}
		case 'B': {
			int count = 0;
			mp->loop = OMXMPT_LOOP_DIR;
			mp->file_cnt = avfilelist_init(optarg, &mp->files);
			if (mp->file_cnt <= 0)
				break;
			mp->file_idx = 0;
			mp->file_cur = mp->files[0];
			omxmpt_send_cmd(mp, OMXMPT_CMD_PREPARE, 0, mp->file_cur);
			omxmpt_send_cmd(mp, OMXMPT_CMD_START, 0, NULL);
			while (count++ < TEST_PAUSE_COUNT && !mp->exit_flag) {
				OSCL_LOGI("%d pause loop", count);
				oscl_mdelay(3000);
				omxmpt_send_cmd(mp, OMXMPT_CMD_PAUSE, 0, NULL);
				oscl_mdelay(2000);
				omxmpt_send_cmd(mp, OMXMPT_CMD_START, 0, NULL);
			}
			omxmpt_send_cmd(mp, OMXMPT_CMD_STOP, 0, NULL);
			break;
		}
		case 'S': {
			int count = 0;
			mp->loop = OMXMPT_LOOP_FILE;
			mp->file_cur = optarg;
			omxmpt_send_cmd(mp, OMXMPT_CMD_PREPARE, 0, mp->file_cur);
			omxmpt_send_cmd(mp, OMXMPT_CMD_START, 0, NULL);
			sem_wait(&mp->prepared_sem);
			if (mp->duration_ms <= 0) {
				OSCL_LOGE("invalid duration %ld ms", mp->duration_ms);
				break;
			}
			while (count++ < TEST_SEEK_COUNT && !mp->exit_flag) {
				long pos = rand() % mp->duration_ms;
				OSCL_LOGI("%d times seek: seek to %ld ms", count, pos);
				omxmpt_send_cmd(mp, OMXMPT_CMD_SEEK,
					CMD_SEEK_SET, (void *)pos);
				sem_wait(&mp->seek_completed_sem);
				oscl_mdelay(3000);
			}
			omxmpt_send_cmd(mp, OMXMPT_CMD_STOP, 0, NULL);
			break;
		}
		case 'J': {
			int count = 0;
			mp->loop = OMXMPT_LOOP_FILE;
			mp->file_cur = optarg;
			mp->fast_seek_dir = 1;
			omxmpt_send_cmd(mp, OMXMPT_CMD_PREPARE, 0, mp->file_cur);
			omxmpt_send_cmd(mp, OMXMPT_CMD_START, 0, NULL);
			oscl_mdelay(1000);
			while (count++ < TEST_SEEK_COUNT && !mp->exit_flag) {
				omxmpt_send_cmd(mp, OMXMPT_CMD_SEEK, CMD_SEEK_CUR,
				    (void *)(TEST_FAST_SEEK_STEP * mp->fast_seek_dir));
				sem_wait(&mp->seek_completed_sem);
			}
			omxmpt_send_cmd(mp, OMXMPT_CMD_STOP, 0, NULL);
			break;
		}
		case 'Y': {
			mp->loop = OMXMPT_LOOP_NONE;
			mp->file_cur = optarg;
			omxmpt_send_cmd(mp, OMXMPT_CMD_PREPARE, 0, mp->file_cur);
			omxmpt_send_cmd(mp, OMXMPT_CMD_START, 0, NULL);
			while (!mp->exit_flag) {
				ret = oscl_sem_timedwait_ms(&mp->on_completed_sem, 2000);
				if (ret == 0) {
					OSCL_LOGI("play complete");
					break;
				}
				omxmpt_send_cmd(mp, OMXMPT_CMD_UPDATE_TIME, 0, NULL);
			}
			break;
		}
		case 'L': {
			int cnt = 0;
			mp->loop = OMXMPT_LOOP_FILE;
			mp->file_cur = optarg;
			omxmpt_send_cmd(mp, OMXMPT_CMD_PREPARE, 0, mp->file_cur);
			omxmpt_send_cmd(mp, OMXMPT_CMD_START, 0, NULL);
			while (!mp->exit_flag) {
				ret = oscl_sem_timedwait_ms(&mp->on_completed_sem, 2000);
				if (ret == 0) {
					cnt++;
					OSCL_LOGI("%d play loop complete", cnt);
				}
			}
			break;
		}
		case 'D': {
			int cnt = 0;
			mp->loop = OMXMPT_LOOP_DIR;
			mp->file_cnt = avfilelist_init(optarg, &mp->files);
			if (mp->file_cnt <= 0)
				break;
			mp->file_idx = 0;
			mp->file_cur = mp->files[0];
			omxmpt_send_cmd(mp, OMXMPT_CMD_PREPARE, 0, mp->file_cur);
			omxmpt_send_cmd(mp, OMXMPT_CMD_START, 0, NULL);
			while (!mp->exit_flag) {
				ret = oscl_sem_timedwait_ms(&mp->on_completed_sem, 2000);
				if (ret == 0) {
					cnt++;
					OSCL_LOGI("%d times play complete", cnt);
				}
			}
			break;
		}
		case 'C': {
			int count = 0;
			mp->loop = OMXMPT_LOOP_FILE;
			mp->file_cur = optarg;
			while (count++ < TEST_CYCLE_COUNT && !mp->exit_flag) {
				OSCL_LOGI("%d cycle loop", count);
				omxmpt_send_cmd(mp, OMXMPT_CMD_PREPARE, 0, mp->file_cur);
				omxmpt_send_cmd(mp, OMXMPT_CMD_START, 0, NULL);
				oscl_mdelay(5000);
				omxmpt_send_cmd(mp, OMXMPT_CMD_STOP, 0, NULL);
			}
			break;
		}
		case 'A': {
			int track = 1;
			mp->loop = OMXMPT_LOOP_FILE;
			mp->file_cur = optarg;
			omxmpt_send_cmd(mp, OMXMPT_CMD_PREPARE, 0, mp->file_cur);
			omxmpt_send_cmd(mp, OMXMPT_CMD_START, 0, NULL);
			sem_wait(&mp->prepared_sem);
			while (!mp->exit_flag) {
				oscl_mdelay(2000);
				track++;
				track %= mp->ainfo.audio_stream_num;
				OSCL_LOGI("switch to %d", track);
				omxmpt_send_cmd(mp, OMXMPT_CMD_SW_AUDIO_TRACK,
					track, NULL);
			}
			break;
		}
		case 'R': {
			int rot = 0;
			mp->loop = OMXMPT_LOOP_FILE;
			mp->file_cur = optarg;
			mp->rot_en = 1;
			omxmpt_send_cmd(mp, OMXMPT_CMD_ROTATE_EN, 1, NULL);
			omxmpt_send_cmd(mp, OMXMPT_CMD_PREPARE, 0, mp->file_cur);
			omxmpt_send_cmd(mp, OMXMPT_CMD_START, 0, NULL);
			while (!mp->exit_flag) {
				oscl_mdelay(2000);
				rot++;
				rot %= 4;
				omxmpt_send_cmd(mp, OMXMPT_CMD_ROTATE, rot, NULL);
			}
			break;
		}
		case 'T': {
			mp->loop = OMXMPT_LOOP_NONE;
			mp->file_cur = optarg;
			omxmpt_send_cmd(mp, OMXMPT_CMD_PREPARE, 0, mp->file_cur);
			sem_wait(&mp->prepared_sem);
			printf("duration %ld ms\n", mp->duration_ms);
			OSCL_LOGI("duration %ld ms", mp->duration_ms);
			omxmpt_send_cmd(mp, OMXMPT_CMD_STOP, 0, NULL);
			break;
		}
		case 'E': {
			mp->loop = OMXMPT_LOOP_USERDEF;
			mp->file_cnt = avfilelist_init(optarg, &mp->files);
			if (mp->file_cnt <= 0)
				break;
			mp->file_idx = 0;
			dir_auto_test(mp);
			break;
		}
		default:
			OSCL_LOGE("Unexcepted case, please let me know, option '%c'",
				optopt);
			break;
		}
	}

EXIT:
#ifdef OMXPLAYER_TEST
	if (mp->player != NULL) {
		omxmp_release(mp->player);
		mp->player = NULL;
	}
#endif
	if (mp->player == NULL) {
		if (mp->cmd_thd_running) {
			mp->cmd_thd_running = 0;
			omxmpt_send_cmd(mp, 0, 0, NULL);
			pthread_join(cmd_thd_id, NULL);
		}
		sem_destroy(&mp->cmd_sem);
		sem_destroy(&mp->on_seek_completed_sem);
		sem_destroy(&mp->on_completed_sem);
		sem_destroy(&mp->on_prepared_sem);
		sem_destroy(&mp->seek_completed_sem);
		sem_destroy(&mp->prepared_sem);
		oscl_queue_deinit(&mp->cmd_queue);
		avfilelist_deinit(mp->files, mp->file_cnt);
	}
	OSCL_LOGI("Test finish: ret %d", ret);
	optind = 0;
	return 0;
}

#ifdef OMXPLAYER_TEST
int main(int argc, char *argv[])
{
	return omxplayer_test(argc, argv);
}
#endif

#ifdef __EOS__
MSH_CMD_EXPORT(omxplayer_test, "omxplayer_test");
#endif

