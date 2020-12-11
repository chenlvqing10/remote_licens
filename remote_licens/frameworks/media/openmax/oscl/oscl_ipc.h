/*
 * oscl_debug.h - debug api used by lombo media framework.
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

#ifndef __OSCL_IPC_H__
#define __OSCL_IPC_H__

#ifdef __linux__
int ipc_sem_init(key_t sem_key);
void ipc_sem_wait(int semid);
void ipc_sem_post(int semid);
void ipc_sem_destroy(int semid);
int send_cmd_to_pipe(char *pipe, int argc, char **argv);
#endif

#endif /* __OSCL_IPC_H__ */
