/*
 * lb_recorder.c - Standard functionality for recorder.
 *
 * Copyright (C) 2016-2018, LomboTech Co.Ltd.
 * Author: lomboswer <lomboswer@lombotech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#define DBG_LEVEL		DBG_INFO

#include <oscl.h>
#include <base_component.h>
#include "omx_test.h"
#include "vrender_component.h"
#include "vrec_component.h"
#include "omx_api.h"
#include "lb_recorder.h"
#include <getopt.h>
#include "recorder_private.h"
#include "dirent.h"
#include "lb_omx_core.h"

static void omx_show_active()
{
	OMX_HANDLETYPE *active_comp;
	int i;

	OSCL_LOGE("====");
	active_comp = omx_active_handle_get();
	for (i = 0; i < OMX_COMP_MAX_INST; i++) {
		if (active_comp[i] != NULL) {
			OSCL_LOGE("==active_comp[%d]:%p==", i, active_comp[i]);
			base_show_component(active_comp[i]);
		}
	}
	omx_active_handle_put();
}

int recorder_show_components(int argc, char **argv)
{
	OSCL_LOGE("====");
	omx_show_active();
	return 0;
}

