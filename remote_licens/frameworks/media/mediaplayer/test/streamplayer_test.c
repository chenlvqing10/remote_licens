#define DBG_LEVEL         DBG_INFO
#define LOG_TAG           "streamplayer_test"

#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <dirent.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <dlfcn.h>
#include <oscl.h>
#include "display/oscl_display.h"
#include <media_common.h>

#include "omx_mediaplayer.h"


typedef struct raw_h264_mgr {
	#define STREAM_BUFFER_SIZE (5 * 1024 * 1024)
	#define NAL_ARRAY_SIZE     (8 * 1024)
	char data[STREAM_BUFFER_SIZE];
	char *nal[NAL_ARRAY_SIZE];
	int nal_size[NAL_ARRAY_SIZE];
	int nal_num;
} raw_h264_mgr_t;

typedef struct strmp_audio_param {
	char *filename;
	int codecid;
	int channels;
	int bits;
	int samplerate;
} strmp_audio_param_t;

typedef struct strmp_video_param {
	char *filename;
	int codecid;
	int width;
	int height;
} strmp_video_param_t;

typedef struct streamplayer_test {
	void *player;
	strmp_video_param_t video;
	strmp_audio_param_t audio;
	int has_audio;
	int has_video;
	int muxer_type;
	int thd_running;
} streamplayer_test_t;

static streamplayer_test_t g_streamplayer;

static int init_h264_file(raw_h264_mgr_t *raw, const char *filename)
{
	int fd;
	int data_len;
	int inal;
	char *p, *end;

	if (raw == NULL) {
		OSCL_LOGE("raw_stream==NULL!!");
		return -1;
	}

	fd = open(filename, O_RDONLY);
	if (fd < 0) {
		OSCL_LOGE("open %s error(%s)!", filename, strerror(errno));
		return -1;
	}
	data_len = read(fd, raw->data, sizeof(raw->data));
	if (data_len < 0) {
		OSCL_LOGE("read %s error(%s)!", filename, strerror(errno));
		goto ERR_EXIT;
	}
	close(fd);

	inal = 0;
	p = raw->data;
	end = raw->data + data_len;
	while (p < end - 4) {
		if (p[0] == 0 && p[1] == 0 && p[2] == 0 && p[3] == 1) {
			if (inal > 0)
				raw->nal_size[inal - 1] = p - raw->nal[inal - 1];
			raw->nal[inal] = p;
			inal++;
			p += 4;
			continue;
		}
		if (p[0] == 0 && p[1] == 0 && p[2] == 1) {
			if (inal > 0)
				raw->nal_size[inal - 1] = p - raw->nal[inal - 1];
			raw->nal[inal] = p;
			inal++;
			p += 3;
			continue;
		}
		p++;
	}
	if (inal > 0)
		raw->nal_size[inal - 1] = end - raw->nal[inal - 1];
	raw->nal_num = inal;
	OSCL_LOGI("total %d nal", raw->nal_num);

	return 0;

ERR_EXIT:
	close(fd);
	return -1;
}

static int init_stream_param(streamplayer_test_t *sp)
{
	avstream_info_t si;
	char *suffix = NULL;
	int len;
	int ret;

	OSCL_LOGI("start");

	memset(&si, 0, sizeof(avstream_info_t));
	sp->muxer_type = MUXER_TYPE_UNKNOWN;
	sp->has_video = 0;
	sp->has_audio = 0;
	sp->audio.codecid = AUDIO_CODEC_UNKNOWN;

	while (1) {
		if (sp->video.filename == NULL)
			break;
		suffix = strrchr(sp->video.filename, '.');
		if (suffix == NULL) {
			OSCL_LOGE("file %s suffix err!", sp->video.filename);
			break;
		}
		suffix++;
		len = strlen(suffix);
		if (!strncasecmp(suffix, "h264", len)) {
			OSCL_LOGI("suffix=%s", suffix);
			si.muxer_type = MUXER_TYPE_RAW;
			si.vstream_num = 1;
			si.vstream_info.codec = VIDEO_CODEC_H264;
			si.vstream_info.width = sp->video.width;
			si.vstream_info.height = sp->video.height;

			sp->video.codecid = VIDEO_CODEC_H264;
			sp->has_video = 1;
			sp->muxer_type = MUXER_TYPE_RAW;
		}  else if (!strncasecmp(suffix, "ts", len) ||
			    !strncasecmp(suffix, "m2ts", len)) {
			OSCL_LOGI("suffix=%s", suffix);
			si.muxer_type = MUXER_TYPE_TS;
			si.vstream_info.codec = VIDEO_CODEC_H264;
			si.vstream_info.width = sp->video.width;
			si.vstream_info.height = sp->video.height;

			sp->muxer_type = MUXER_TYPE_TS;
		}
		break;
	}

	while (si.muxer_type == MUXER_TYPE_RAW || si.muxer_type == MUXER_TYPE_UNKNOWN) {
		if (sp->audio.filename == NULL) {
			OSCL_LOGW("no audio stream");
			break;
		}
		suffix = strrchr(sp->audio.filename, '.');
		if (suffix == NULL) {
			OSCL_LOGE("file %s suffix err!", sp->audio.filename);
			break;
		}
		suffix++;
		len = strlen(suffix);
		OSCL_LOGI("suffix=%s", suffix);
		if (!strncasecmp(suffix, "wav", len)) {
			si.muxer_type = MUXER_TYPE_RAW;
			si.astream_num = 1;
			si.astream_info.codec = AUDIO_CODEC_PCM;
			si.astream_info.samplerate = sp->audio.samplerate;
			si.astream_info.channels = sp->audio.channels;
			si.astream_info.bits_per_sample = sp->audio.bits;

			sp->audio.codecid = AUDIO_CODEC_PCM;
			sp->has_audio = 1;
			sp->muxer_type = MUXER_TYPE_RAW;
		}
		break;
	}
	if (sp->muxer_type == MUXER_TYPE_UNKNOWN) {
		OSCL_LOGE("no stream file!");
		return -1;
	}
	OSCL_LOGI("muxer type %d", si.muxer_type);
	ret = omxmp_set_stream(sp->player, &si);

	OSCL_LOGI("end");

	return ret;
}

static int fill_stream_data(streamplayer_test_t *sp, int type,
	uint8_t *data, int size, int64_t ts)
{
	avstream_t s;
	OSCL_LOGD("filling data: size %d", size);
	s.type = type;
	s.data = data;
	s.data_size = size;
	s.timestamp = ts;
	int ret = omxmp_fill_stream(sp->player, &s);
	if (ret < 0) {
		OSCL_LOGE("fill data error!");
		return -1;
	}
	return 0;
}

#include "demuxer_plugin.h"
#include "iostream_plugin.h"
static void *stream_thread(void *param)
{
	#define STRMP_VIDEO_BUFFER_SIZE		(4*1024*1024)
	#define STRMP_VIDEO_BUFFER_ALIGN		(4*1024)
	#define STRMP_AUDIO_BUFFER_SIZE		(512*1024)
	#define STRMP_AUDIO_BUFFER_ALIGN		(1)
	#define STRMP_SUBTITLE_BUFFER_SIZE	(256*1024)
	#define STRMP_SUBTITLE_BUFFER_ALIGN	(1)

	streamplayer_test_t *sp = param;
#if 0
	void *parser = NULL;
	void *dlhandle = NULL;
	char libname[64] = "libiostream.so";
	dmx_plugin_t *plugin = NULL;
	iostream_plugin_t *io_plugin = NULL;
	void *io_dlhandle = NULL;
	void *io_ctx = NULL;
	int vidx = -1;
#else
	raw_h264_mgr_t h264_stm;
	int cnt = 0;
#endif
	int fd = -1;
	uint8_t *pbuf = NULL;
	int64_t video_ts, audio_ts;
	int audio_frame_len1 = 0, audio_frame_len2 = 0;
	int audio_len_per_sec = 0;
	int frame_rate = 25;

	if (sp->muxer_type == MUXER_TYPE_RAW) {
		if (sp->has_video) {
#if 0
			typedef void *(*get_plugin)(void);
			dmx_buffer_info_t buf_info;
			dmx_mediainfo_t mediainfo;
			get_plugin plugin_func;

			OSCL_LOGI("opening lib: %s", libname);
			io_dlhandle = dlopen(libname, RTLD_LAZY);
			if (io_dlhandle == NULL) {
				OSCL_LOGE("Failed to open lib: %s(%s)", libname,
					dlerror());
				goto EXIT;
			}
			plugin_func = (get_plugin)dlsym(io_dlhandle, "get_plugin_info");
			if (plugin_func == NULL) {
				OSCL_LOGE("get plugin info error!");
				goto EXIT;
			}
			io_plugin = plugin_func();
			if (io_plugin == NULL) {
				OSCL_LOGE("get plugin method error!");
				goto EXIT;
			}

			io_ctx = io_plugin->create(sp->video.filename, STREAM_TYPE_FILE);
			if (io_ctx == NULL) {
				OSCL_LOGE("create iostream error!");
				goto EXIT;
			}

			libname[0] = 0;
			strcat(libname, "lib");
			strcat(libname, "h264");
			strcat(libname, "demuxer.so");
			OSCL_LOGI("opening lib: %s", libname);
			dlhandle = dlopen(libname, RTLD_LAZY);
			if (dlhandle == NULL) {
				OSCL_LOGE("Failed to open lib: %s(%s)", libname,
					dlerror());
				goto EXIT;
			}
			plugin_func = (get_plugin)dlsym(dlhandle, "get_plugin_info");
			if (plugin_func == NULL) {
				OSCL_LOGE("get plugin info error!");
				goto EXIT;
			}
			plugin = plugin_func();
			if (plugin == NULL) {
				OSCL_LOGE("get plugin method error!");
				goto EXIT;
			}

			memset(&buf_info, 0, sizeof(dmx_buffer_info_t));
			memset(&mediainfo, 0, sizeof(dmx_mediainfo_t));
			buf_info.video_buf_size = STRMP_VIDEO_BUFFER_SIZE;
			buf_info.video_buf_align = STRMP_VIDEO_BUFFER_ALIGN;
			buf_info.audio_buf_size = STRMP_AUDIO_BUFFER_SIZE;
			buf_info.audio_buf_align = STRMP_AUDIO_BUFFER_ALIGN;
			buf_info.sub_buf_size = STRMP_SUBTITLE_BUFFER_SIZE;
			buf_info.sub_buf_align = STRMP_SUBTITLE_BUFFER_ALIGN;
			parser = plugin->read_header(io_ctx, &buf_info, &mediainfo);
			if (parser == NULL) {
				OSCL_LOGE("read header error!");
				goto EXIT;
			}

			dmx_vinfo_t *vi =
				&mediainfo.vinfo[mediainfo.default_vstream_idx];
			if (vi->extradata != NULL) {
				fill_stream_data(sp, STREAM_TYPE_VIDEO,
					vi->extradata, vi->extradata_size, 0);
			}
			vidx =
			    mediainfo.vinfo[mediainfo.default_vstream_idx].stream_index;
#else
			memset(&h264_stm, 0, sizeof(h264_stm));
			init_h264_file(&h264_stm, sp->video.filename);
#endif
		}

		if (sp->has_audio) {
			int len_sample = sp->audio.channels * sp->audio.bits / 8;
			audio_len_per_sec = sp->audio.samplerate * len_sample;
			int frame_len = audio_len_per_sec / frame_rate;
			audio_frame_len1 = frame_len;
			audio_frame_len1 /= len_sample;
			audio_frame_len1 /= 2;
			audio_frame_len1 *= len_sample;
			audio_frame_len2 = frame_len - audio_frame_len1;
			pbuf = oscl_malloc(audio_len_per_sec);
			if (pbuf == NULL)
				OSCL_LOGE("alloc abuf error!");
			fd = open(sp->audio.filename, O_RDONLY);
			if (fd < 0)
				OSCL_LOGE("open %s error!", sp->audio.filename);
		}
	} else {
		fd = open(sp->video.filename, O_RDONLY);
		if (fd < 0) {
			OSCL_LOGE("open %s error!", sp->audio.filename);
			goto EXIT;
		}
		pbuf = oscl_malloc(64 * 1024);
		if (pbuf == NULL) {
			OSCL_LOGE("alloc buf error!");
			goto EXIT;
		}
	}

	video_ts = audio_ts = 0;
	while (sp->thd_running) {
		if (sp->muxer_type == MUXER_TYPE_RAW) {
			if (sp->has_video) {
#if 0
				dmx_packet_t pkt;
				int ret = 0;

				ret = plugin->read_packet(parser, &pkt);
				if (ret != 0) {
					OSCL_LOGE("read pkt error(%d)!", ret);
					break;
				}
				if (pkt.stream_index == vidx)
					fill_stream_data(sp, STREAM_TYPE_VIDEO,
						pkt.vir_addr, pkt.data_size, video_ts);
				plugin->return_packet(parser, &pkt);
#else
				if (cnt < h264_stm.nal_num) {
					fill_stream_data(sp, STREAM_TYPE_VIDEO,
						(uint8_t *)h264_stm.nal[cnt],
						h264_stm.nal_size[cnt],
						video_ts);
				}
				cnt++;
#endif
				video_ts += 1000000 / frame_rate;
			}

			if (sp->has_audio) {
				int len = read(fd, pbuf, audio_frame_len1);
				if (len > 0) {
					fill_stream_data(sp, STREAM_TYPE_AUDIO,
						pbuf, len, audio_ts);
				}
				audio_ts += (int64_t)len * 1000000 / audio_len_per_sec;

				len = read(fd, pbuf, audio_frame_len2);
				if (len > 0) {
					fill_stream_data(sp, STREAM_TYPE_AUDIO,
						pbuf, len, audio_ts);
				}
				audio_ts += (int64_t)len * 1000000 / audio_len_per_sec;
			}
		} else {
			int len = read(fd, pbuf, 64 * 1024);
			if (len < 0) {
				OSCL_LOGE("read data error!");
				break;
			} else if (len == 0) {
				OSCL_LOGI("file end");
				break;
			} else {
				OSCL_LOGD("fill %d bytes", len);
				fill_stream_data(sp, STREAM_TYPE_VIDEO, pbuf, len, 0);
			}
		}
		oscl_mdelay(2);
	}

EXIT:
	if (fd >= 0)
		close(fd);
	if (pbuf != NULL)
		free(pbuf);
#if 0
	if (parser != NULL)
		plugin->read_close(parser);
	if (dlhandle != NULL)
		dlclose(dlhandle);
	if (io_ctx != NULL)
		io_plugin->destroy(io_ctx);
	if (io_dlhandle != NULL)
		dlclose(io_dlhandle);
#endif
	OSCL_LOGI("exit");
	return NULL;
}

static void strmp_on_completion(void *handle)
{
	OSCL_LOGI("------on_completion--------\n");
}

static void strmp_on_error(void *handle, int omx_err)
{
	OSCL_LOGE("------on_error %d--------\n", omx_err);
}

static void strmp_on_prepared(void *handle)
{
	OSCL_LOGI("------on_prepared--------\n");
}

static void strmp_on_seek_complete(void *handle)
{
	OSCL_LOGI("------on_seek_complete--------\n");
}
static void strmp_on_video_size_changed(void *handle, int width, int height)
{
	OSCL_LOGI("on_video_size_changed width=%d, height=%d\n", width, height);
}
static void strmp_on_timed_text(void *handle, char *text)
{
	OSCL_LOGI("------on_timed_text %s--------\n", text);
}

static mp_callback_ops_t g_cbops = {
	.on_completion            = strmp_on_completion,
	.on_error                 = strmp_on_error,
	.on_prepared              = strmp_on_prepared,
	.on_seek_complete         = strmp_on_seek_complete,
	.on_video_size_changed    = strmp_on_video_size_changed,
	.on_timed_text            = strmp_on_timed_text,
};

static void strmp_show_help(void)
{
	const char *arg_format = "  %-4s%-24s %s%s\n";

	printf("usage: streamplayer_test [argument]\n");
	printf("arguments:\n");
	printf(arg_format, "-v", "--video_file", "set video stream file", "");
	printf(arg_format, "-w", "--width", "set video stream width", "");
	printf(arg_format, "-d", "--height", "set video stream height", "");
	printf(arg_format, "-a", "--audio_file", "set audio stream file", "");
	printf(arg_format, "-b", "--audio_bits", "set audio bits per samble", "");
	printf(arg_format, "-c", "--audio_channels", "set audio channel", "");
	printf(arg_format, "-r", "--audio_samplerate", "set audio sample rate", "");
	printf(arg_format, "-h", "--help", "show this help info", "");
}

static struct option g_long_opts[] = {
	{"video_file", 1, 0, 'v'},
	{"width", 1, 0, 'w'},
	{"height", 1, 0, 'd'},
	{"audio_file", 1, 0, 'a'},
	{"audio_bits", 1, 0, 'b'},
	{"audio_channels", 1, 0, 'c'},
	{"audio_samplerate", 1, 0, 'r'},
	{"help", 0, 0, 'h'}
};

static const char *g_short_opts = "v:a:b:c:r:w:d:h";

static int stream_player_test(int argc, char *argv[])
{
	streamplayer_test_t *sp = &g_streamplayer;
	pthread_attr_t thd_attr;
	pthread_t  thd_id = 0;
	int c = 0;
	int index;
	int ret = 0;

	while (1) {
		c = getopt_long(argc, argv, g_short_opts, g_long_opts, &index);
		if (c == -1) {
			OSCL_LOGI("get args end, exit!\n");
			break;
		}
		OSCL_LOGI("option:%c, optarg:%s", c, (optarg == NULL) ? "null" : optarg);
		switch (c) {
		case 'v': {
			if (sp->video.filename != NULL) {
				OSCL_LOGW("free vfname first!");
				oscl_free(sp->video.filename);
			}
			sp->video.filename = oscl_strdup(optarg);
			oscl_param_check_exit(sp->video.filename != NULL, -1,
				"alloc vfname err!");
			break;
		}
		case 'a': {
			if (sp->audio.filename != NULL) {
				OSCL_LOGW("free afname first!");
				oscl_free(sp->audio.filename);
			}
			sp->audio.filename = oscl_strdup(optarg);
			oscl_param_check_exit(sp->audio.filename != NULL, -1,
				"alloc afname err!");
			break;
		}
		case 'b': {
			sp->audio.bits = atoi(optarg);
			break;
		}
		case 'c': {
			sp->audio.channels = atoi(optarg);
			break;
		}
		case 'r': {
			sp->audio.samplerate = atoi(optarg);
			break;
		}
		case 'w': {
			sp->video.width = atoi(optarg);
			break;
		}
		case 'd': {
			sp->video.height = atoi(optarg);
			break;
		}
		case 'h': {
			strmp_show_help();
			break;
		}
		default:
			OSCL_LOGE("Unexcepted case, please let me know, option '%c'", c);
			break;
		}
	}

	sp->player = omxmp_create(&g_cbops);
	oscl_param_check_exit(sp->player != NULL, -1, "create vplayer error!");

	if (sp->video.filename) {
		disp_size_t scn_size;
		void *disp_hdl;
		omxmp_win_t win;

		memset(&scn_size, 0, sizeof(scn_size));
		disp_hdl = oscl_open_disp_engine();
		oscl_param_check_exit(disp_hdl != NULL, -1, "open disp engine err");
		ret = oscl_disp_get_screen_size(disp_hdl, &scn_size);
		oscl_param_check(ret == 0, -1, "get screen size err");
		oscl_close_disp_engine(disp_hdl);
		OSCL_LOGI("scrn w %d, h %d\n", scn_size.width, scn_size.height);

		win.left = 0;
		win.top  = 0;
		win.width = scn_size.width;
		win.height = scn_size.height;
		ret = omxmp_set_window(sp->player, &win);
		oscl_param_check_exit(ret == 0, -1, "set win err");
		/* set scaling mode */
		ret = omxmp_set_scaling_mode(sp->player,
			VDISP_WINDOW_FULL_SCREEN_VIDEO_RATIO);
		oscl_param_check_exit(ret == 0, -1, "set scale mode err");
		/* enable or disable rotation */
		ret = omxmp_enable_rotation(sp->player, 0);
		oscl_param_check_exit(ret == 0, -1, "enable rotation err");
	}

	oscl_param_check_exit(ret == 0, -1, "get external stream handle err!");
	ret = init_stream_param(sp);
	oscl_param_check_exit(ret == 0, -1, "init stream param err!");

	sp->thd_running = 1;
	pthread_attr_init(&thd_attr);
	pthread_attr_setstacksize(&thd_attr, 0x2000);
	ret = pthread_create(&thd_id, &thd_attr, stream_thread, sp);
	if (ret != 0) {
		OSCL_LOGE("create vstream_thread error(%s)!", strerror(errno));
		sp->thd_running = 0;
		goto EXIT;
	}

	ret = omxmp_set_data_source(sp->player, "external_stream");
	oscl_param_check_exit(ret == 0, -1, "omxmp_set_data_source error");
	ret = omxmp_prepare(sp->player);
	oscl_param_check_exit(ret == 0, -1, "omxmp_prepare error");
	ret = omxmp_start(sp->player);
	oscl_param_check_exit(ret == 0, -1, "omxmp_start error");

	while (1)
		oscl_mdelay(1000);

EXIT:
	if (sp->thd_running) {
		sp->thd_running = 0;
		pthread_join(thd_id, NULL);
	}

	if (sp->player != NULL) {
		omxmp_stop(sp->player);
		omxmp_reset(sp->player);
		omxmp_release(sp->player);
	}

	if (sp->video.filename)
		oscl_free(sp->video.filename);
	if (sp->audio.filename)
		oscl_free(sp->audio.filename);

	return 0;
}

int main(int argc, char *argv[])
{
	return stream_player_test(argc, argv);
}
