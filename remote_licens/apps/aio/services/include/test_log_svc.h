/*
 * test_log_svc.h
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

#ifndef __TEST_LOG_SVC__
#define __TEST_LOG_SVC__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void test_log_record_default(web_socket_checkin_info_t *p_info, char *user_name,
	face_record_t *record);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TEST_LOG_SVC__ */