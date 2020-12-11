/*
 * oscl.h - common lib api used by lombo media player.
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

#ifndef __LB_OMX_TEST_H__
#define __LB_OMX_TEST_H__

#define TEST_IN_FILE "/1.txt"
#define TEST_OUT_FILE "/2.txt"

int compare_in_out(char *in_name, char *out_name);
void creat_input_file(char *name);
int get_port_index(OMX_COMPONENTTYPE *comp, OMX_DIRTYPE dir,
	OMX_PORTDOMAINTYPE domain, int start);

int openmax_test_untunel(void);
int openmax_test_tunnel(void);
int openmax_test_vrec_vrender_tunnel(void);
int openmax_test_vrec_vrender_untunnel(void);
int openmax_test_rot_untunnel(void);
int openmax_test_video_stream(void);
OMX_ERRORTYPE file_reader_component_init(lb_omx_component_t *cmp_handle);
OMX_ERRORTYPE file_writer_component_init(lb_omx_component_t *cmp_handle);
OMX_ERRORTYPE filter_component_init(lb_omx_component_t *cmp_handle);
OMX_ERRORTYPE vrec_component_init(lb_omx_component_t *cmp_handle);
OMX_ERRORTYPE vrender_component_init(lb_omx_component_t *cmp_handle);
OMX_ERRORTYPE splitter_component_init(lb_omx_component_t *lb_cmp_hdl);
OMX_ERRORTYPE rotate_component_init(lb_omx_component_t *cmp_handle);

void vc_core_test();

#endif /* __LB_OSCL_H__ */
