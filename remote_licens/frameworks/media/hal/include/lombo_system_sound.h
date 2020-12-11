/*
* lombo_system_sound.h
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
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef LOMBO_SYSTEM_SOUND_H
#define LOMBO_SYSTEM_SOUND_H

#include <stdlib.h>

int lombo_system_sound_close();

int lombo_system_sound_play(const char *file_name);

int lombo_system_play_wav(char *wav_data, int size);

/* volume : from 0 to 100, this will only affect system sound play */
void lombo_system_set_volume(int volume);

int lombo_system_get_volume(void);

/* volume : from 0 to 100, this will affect all */
void lombo_set_master_volume(int volume);

int lombo_get_master_volume(void);

/* volume : from 0 to 100, this will affect all */
void lombo_set_master_record_volume(int volume);

int lombo_get_master_record_volume(void);

#endif /* LOMBO_SYSTEM_SOUND_H */

