/*
* copyright (c) 2018 All Rights Reserved
*
* This file is part of media.
*
* File   : media_probe.h
* Version: V1.0
* Date   : 2019/8/7 10:21:02
* Other  : ffmpeg-3.2.7
*/
#ifndef MEDIA_PROBE_H
#define MEDIA_PROBE_H

#include <iostream_plugin.h>

#define MAX_FNAME			256
char *media_probe(file_stream_para_t *para);

OMX_STRING sub_probe(OMX_STRING media_name, OMX_S32 *codec_id);

#endif /* MEDIA_PROBE_H */

