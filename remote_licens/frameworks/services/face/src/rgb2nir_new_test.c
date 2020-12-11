/*
 * test.c
 *
 * Copyright (C) 2019, LomboTech Co.Ltd.
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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <stdlib.h>
#include <math.h>
#include <execinfo.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <dirent.h>

#define LOG_TAG "gpio"
#include <log/log.h>

#include "face_svc.h"
#include "face_svc_conf.h"
#include "jpeg/jpeg_enc.h"
#include "cut.h"
#include "flip.h"
#include "recg_img.h"
#include "time_conv.h"
#include "file_ops.h"
#include "zbar_svc.h"
#include "rgb2nir.h"
#include "rgb2nir_new.h"


int main(void)
{
	face_svc_load_conf();
	r2n_test_pic();
	return 0;
}
