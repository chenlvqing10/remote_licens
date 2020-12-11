#define DBG_LEVEL         DBG_INFO
#define LOG_TAG           "nn_self_test"

#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include <oscl.h>
#include <time.h>

#include "display/oscl_display.h"

#include "face_svc_dec.h"
#include "face_svc.h"
#include "omx_api.h"


#include "face_svc_event.h"
#include "media_probe.h"


#include "face_svc_json.h"
#include "face_svc_log.h"
#include "face_svc_sample.h"




#define MAX_FILE_CNT	64
#define TEST_PAUSE_COUNT 10000000
#define TEST_CYCLE_COUNT 10000000
#define TEST_SEEK_COUNT  10000000
#define TEST_DISPLAY_WIDTH 320
#define TEST_DISPLAY_HEIGHT 800

typedef enum OmxMptLoopMode {
	LOOP_NONE = 0,
	LOOP_FILE,
	LOOP_DIR,
} OmxMptLoopMode_e;

static OmxMptLoopMode_e loop;
static int file_cnt;
static int cur_file_index;
static char files[MAX_FILE_CNT][512];
static char *cur_file;
static sem_t sem_end;
void *omxplayer_rgb;
void *omxplayer_nir;

static int g_exit_flag;
static long long g_duration_ms;
static long long g_start_ms;
static int g_test_count;

#define TEST_FASTFB_STEP	(5 * 1000)
static int g_fastfb_test;
static long g_fastfb_tarpos;

static int g_auto_test;
static sem_t g_completion_sem;
static sem_t g_seek_completed_sem;



extern int face_svc_nn_self_test_start(void *handle);

extern int face_svc_rgb_frame_cb(void *p, face_frame_t *frame);
extern void dump_stack(void);
//extern char *media_probe(char *file_name);
extern int face_svc_deinit(void *handle);

#if 0
static void signal_handler(int sig)
{
	OSCL_LOGI("mediaplayer test exit");
	face_svc_dec_stop(omxplayer);
	face_svc_dec_reset(omxplayer);
	g_exit_flag = 1;
	sem_post(&sem_end);
}
#endif
void *svc_hdl = NULL;
void *face_svc_hdl = NULL;


static void save_event_db(const char *path)
{
	char savedb[128] =	{0};
	time_t t = time(0);

	char *dirc = NULL;
	char *basec = NULL;
	char *base = NULL;
	char *dir = NULL;
	basec = strdup(path);
	dirc = strdup(path);

	base = basename(basec);
	dir = dirname(dirc);
	if (NULL != base)
	{
		printf("save_event_db : %s,%s,%s\n", path, dir, base);

		snprintf(savedb, 128, "%s/%s_%d.json", 
			dir, 
			base,
			g_test_count/(file_cnt*2));
		printf("save_event_db new %s\n", savedb);
		face_log_set_log_full_path(savedb);
		#if 0
		snprintf(savedb, 128, 
			"logcat | grep \"face_nst\" | cut -d \":\" -f 2- > %s/%s.json",
			dir,
			base);
		
		printf("execute %s\n", savedb);
		system(savedb);
		free(basec);
		free(dirc);
		#endif
	}
	else
	{
		printf("strdup err: %s\n", strerror(errno));
	}

}

static void play_next(void)
{
	int ret = 0;
	int next = 0;
	int last = cur_file_index;
	const char *next_rgb = NULL;
	const char *next_nir = NULL;

	next = (++cur_file_index) % file_cnt;
	OSCL_LOGI("setup to play %d->%d!!!\n", last, next);

	next_rgb = face_svc_get_rgb(next);
	next_nir = face_svc_get_nir(next);
	OSCL_LOGI("setup to play %s", next_rgb);




	face_svc_setup();
	face_player_setup(omxplayer_rgb, next_rgb, face_svc_dec_rgb_frame_cb);
	

	
	OSCL_LOGI("setup to play %s", next_nir);
	//face_player_setup(omxplayer_nir, files[cur_file_index], face_svc_dec_nir_frame_cb);
	face_player_setup(omxplayer_nir, next_nir, face_svc_dec_nir_frame_cb);

	

	
	save_event_db(next_rgb);

	sleep(1);
	ret = face_svc_dec_start(omxplayer_rgb);
	oscl_param_check(ret == 0, 1, "player1 start err!\n");
	
	ret = face_svc_dec_start(omxplayer_nir);
	oscl_param_check(ret == 0, 2, "player2 start err!\n");
}

static void on_completion(void *handle)
{
	g_test_count++;
	printf("on_completion !!!!!!!!!!!!!!!\n");
	OSCL_LOGI("%d times play test: ------on_completion--------%p", g_test_count, handle);

	int ret = 0;

	if (g_auto_test) {
		sem_post(&g_completion_sem);
		save_event_db("/root/cams");
		return;
	}

	if (g_start_ms > 0) {
		long long play_time_ms = oscl_get_msec() - g_start_ms;
		long long diff = play_time_ms - g_duration_ms;
		OSCL_LOGI("play_time %lld ms, duration %lld ms",
			play_time_ms, g_duration_ms);
		if (diff > 5000 || diff < -5000)
			OSCL_LOGW("play time diff %lld ms, may something wrong!", diff);
	}
	if (loop == LOOP_NONE) {
		face_svc_dec_stop(handle);
		face_svc_dec_reset(handle);
		g_exit_flag = 1;
		sem_post(&sem_end);
	}
	if (loop == LOOP_FILE) {
		
	}
	if (loop == LOOP_DIR) {
		face_svc_dec_stop(handle);
		face_svc_dec_reset(handle);
	
		OSCL_LOGI("player=%p reset done!!! count=%d\n", handle, g_test_count);
		/**/
		if (face_svc_dec_player_is_ready_for_next(omxplayer_rgb) && face_svc_dec_player_is_ready_for_next(omxplayer_nir) )
		{
			sem_post(&g_completion_sem);
			OSCL_LOGI("two player is completed!!! count=%d\n", g_test_count);
		}
	}
	if (g_start_ms > 0)
		g_start_ms = oscl_get_msec();
}
static void on_error(void *handle, int omx_err)
{
	OSCL_LOGE("------on_error %d--------\n", omx_err);
	face_svc_dec_stop(handle);
	g_exit_flag = 1;
}

static void on_prepared(void *handle)
{
	OSCL_LOGI("------on_prepared--------%p\n", handle);
}

static void on_seek_complete(void *handle)
{
	OSCL_LOGI("------on_seek_complete--------\n");

	if (g_auto_test) {
		sem_post(&g_seek_completed_sem);
		return;
	}
	if (!g_fastfb_test)
		return;

	long curpos = face_svc_dec_get_current_position(handle);
	int ret;
	OSCL_LOGI("seek complete pos %ld", curpos);
	if (g_fastfb_test == 1) {
		if (curpos < g_fastfb_tarpos)
			g_fastfb_tarpos += TEST_FASTFB_STEP;
		else
			g_fastfb_tarpos = curpos + TEST_FASTFB_STEP;

		if (g_fastfb_tarpos >= g_duration_ms - 2*TEST_FASTFB_STEP) {
			g_fastfb_test = 2;
			g_fastfb_tarpos = g_duration_ms - 2*TEST_FASTFB_STEP;
		}
	} else {
		if (curpos > g_fastfb_tarpos)
			g_fastfb_tarpos -= TEST_FASTFB_STEP;
		else
			g_fastfb_tarpos = curpos - TEST_FASTFB_STEP;

		if (g_fastfb_tarpos <= 0) {
			g_fastfb_test = 1;
			g_fastfb_tarpos = 0;
		}
	}

	OSCL_LOGI("seek target %ld", g_fastfb_tarpos);
	ret = face_svc_dec_seek_to(handle, g_fastfb_tarpos);
	if (ret)
		OSCL_LOGE("seek error");
}
static void on_video_size_changed(void *handle, int width, int height)
{
	OSCL_LOGI("on_video_size_changed width=%d, height=%d\n", width, height);
}
static void on_timed_text(void *handle, char *text)
{
	OSCL_LOGI("------on_timed_text %s--------\n", text);
}

static mp_callback_ops_t cb_ops = {
	.on_completion            = on_completion,
	.on_error                 = on_error,
	.on_prepared              = on_prepared,
	.on_seek_complete         = on_seek_complete,
	.on_video_size_changed    = on_video_size_changed,
	.on_timed_text            = on_timed_text,
};



struct option play_long_options[] = {
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


const char *short_opts = "f:k:pbeaqhrt:P:C:L:S:D:Y:T:R:A:V:J:E:";




static int get_avfiles(char *file_path)
{
	int i = 0;
	const char *nir = 0;
	const char *rgb = 0;
	face_svc_set_path(file_path);
	printf("-----all files---------\n");
	do {
		nir = face_svc_get_nir(i);
		rgb = face_svc_get_rgb(i);
		if ((NULL != nir) && (NULL != rgb))
		{
			file_cnt++;
		}
		i++;
		printf("rgb=%s nir=%s\n", rgb, nir);
	}while((NULL != nir) && (NULL != rgb));
	printf("---------end all files %d---------\n", file_cnt);
	return 0;
}

void *det_cb(face_frame_t *frame, face_rects_t *rects)
{
	int i;

//	ALOGD("det:ft=%llu %d_%d", 
//		frame->time_stamp,
//		frame->width,
//		frame->height);
	for (i = 0; i < rects->cnt; i++) {
//		ALOGD("rect[%d] x:%d, y:%d, x1:%d, y1:%d",
//				i,
//				rects->p_rect[i].rect[0],
//				rects->p_rect[i].rect[1],
//				rects->p_rect[i].rect[2],
//				rects->p_rect[i].rect[3]);

		frame_t *ptr = (frame_t *)frame->priv;
		face_event_face_det_send(ptr->info.video.time_stamp,
								rects->p_rect[i].head_pose,
								3,
								rects->p_rect[i].score);
		
	}

	if (0 == rects->cnt)
	{
		ALOGD("no face");
	}
	return NULL;
}

void *recg_cb(face_frame_t *frame, face_info_t *face_info)
{
//	ALOGD("recg:ft=%llu", frame->time_stamp);
//
//	ALOGD("id: %d max_id: %d living: %d score: %f feat_version: %d",
//			face_info->rect.id,
//			face_info->feat.max_id,
//			face_info->feat.living_body,
//			face_info->feat.score,
//			face_info->feat.feat_version);
	return NULL;
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

void *face_create_player(void)
{
	int          ret = 0;

	disp_size_t  scn_size;
	void         *disp_hdl;
	face_svc_dec_win_t  win;

	void *player = NULL;
	player = face_svc_dec_create(&cb_ops);
	oscl_param_check(player != NULL, -1, "create omxplayer error!\n");
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
#endif
	
	ret = face_svc_dec_set_window(player, &win);
	oscl_param_check(ret == 0, -1, "set win err\n");
	/* set scaling mode */
	ret = face_svc_dec_set_scaling_mode(player,
		VDISP_WINDOW_FULL_SCREEN_VIDEO_RATIO);
	oscl_param_check(ret == 0, -1, "set scale mode err\n");
	/* enable or disable rotation */
	face_svc_dec_enable_rotation(player, 0);
	oscl_param_check(ret == 0, -1, "enable rotation err");
	return player;
}


void face_player_setup(void *player, const char *filename, p_face_frame_cb f_cb)
{
	int ret = 0;


	ret = face_svc_dec_set_data_source(player, filename);
	oscl_param_check(ret == 0, -1, "set data url err!\n");


	ret = face_svc_dec_prepare(player);
	oscl_param_check(ret == 0, -1, "omxmp_prepare err!\n");

	face_svc_dec_set_cb(player, 0, face_svc_hdl, f_cb);
}

void face_svc_setup(void)
{
	if ((NULL == face_svc_hdl) /*|| 
		(0 == face_svc_deinit(face_svc_hdl))*/
		)
	{
		face_svc_hdl = face_svc_init();
		
		oscl_param_check(face_svc_hdl != 0, -1, "face_svc_init failed!\n");
		/* app set cb */
		face_register_det_cb(face_svc_hdl, det_cb);
		face_register_recg_cb(face_svc_hdl, recg_cb);
	}
}

int face_nn_test(int argc, char *argv[])
{
	int          ret = 0;
	int          c;
	int          index;
	int	     wait_flag = 0;
	disp_size_t  scn_size;
	void         *disp_hdl;
	face_svc_dec_win_t  win;
	pthread_attr_t t_attr;
	pthread_t      thread_id;
	
	(void)t_attr;
	(void)thread_id;
	if (omxplayer_rgb == NULL) {
		omxplayer_rgb = face_create_player();
	}

	/* signal(SIGINT, signal_handler); */

	loop = LOOP_NONE;
	sem_init(&sem_end, 0, 0);
	sem_init(&g_completion_sem, 0, 0);
	sem_init(&g_seek_completed_sem, 0, 0);

	g_exit_flag = 0;
	g_start_ms = 0;
	g_test_count = 0;
	g_fastfb_test = 0;
	g_auto_test = 0;
	face_log_init();

	while (1) {
		c = getopt_long(argc, argv, short_opts, play_long_options, &index);
		if (c == -1) {
			OSCL_LOGI("get args end, exit!\n");
			break;
		}
		OSCL_LOGI("option:%c", c);
		OSCL_LOGI("option:%c, optarg:%s", c, (optarg == NULL) ? "null" : optarg);
		switch (c) {
		case 'h':
			show_help();
			break;
		case 'D': {
			loop = LOOP_DIR;


			if (get_avfiles(optarg))
				break;


			cur_file_index = 0;
			OSCL_LOGI("start to play %s", face_svc_get_rgb(cur_file_index));

			save_event_db(face_svc_get_rgb(cur_file_index));
			face_svc_setup();

			//face_player_setup(omxplayer_rgb, files[cur_file_index], face_svc_dec_rgb_frame_cb);
			face_player_setup(omxplayer_rgb, face_svc_get_rgb(cur_file_index), face_svc_dec_rgb_frame_cb);
			//face_player_setup(omxplayer_rgb, files[cur_file_index],  face_svc_dec_nir_frame_cb);
			
			g_duration_ms  = face_svc_dec_get_duration(omxplayer_rgb);


			omxplayer_nir = face_create_player();
			OSCL_LOGI("start to play %s", face_svc_get_nir(cur_file_index));
			//face_player_setup(omxplayer_nir, files[cur_file_index], face_svc_dec_nir_frame_cb);
			face_player_setup(omxplayer_nir, face_svc_get_nir(cur_file_index), face_svc_dec_nir_frame_cb);
			
			
			oscl_mdelay(2000);
			ret = face_svc_dec_start(omxplayer_rgb);
			oscl_param_check_exit(ret == 0, -1, "omxmp_start err!\n");

			ret = face_svc_dec_start(omxplayer_nir);
			oscl_param_check_exit(ret == 0, -1, "omxmp_start err!\n");
			g_start_ms = oscl_get_msec();

			

			wait_flag = 1;
			break;
		}		
		case 'Y': {
			
			break;
		}

		default:
			OSCL_LOGE("Unexcepted case, please let me know, option '%c'",
				optopt);
			break;
		}
	}
	
	while(1){
		printf("wait for next play!!!\n");
		sem_wait(&g_completion_sem);
		play_next();
	}
EXIT:
	if (wait_flag)
		sem_wait(&sem_end);

	if (cur_file != NULL) {
		oscl_free(cur_file);
		cur_file = NULL;
	}
	OSCL_LOGI("option:%c, ret %d\n", c, ret);
	optind = 0;
	return 0;
}



static void exithook(void)
{
	dump_stack();
}




int main(int argc, char *argv[])
{

	atexit(exithook);
	face_log_init();

#if 0
	face_svc_set_path("/data/sample");
	int i = 0;
	const char *nir;
	const char *rgb;
	do {

		nir = face_svc_get_nir(i);
		rgb = face_svc_get_rgb(i);
		if ((NULL != nir) && (NULL != rgb))
		{
			//file_cnt++;
			printf("rgb=%s nir=%s\n", rgb, nir);
		}
		i++;
	}while((NULL != nir) && (NULL != rgb));
#endif
	//save_event_db("/data/sample/sample.mp4");


	face_nn_test(argc, argv);

	return 0;
}



