/*
* lombo_sound_mixer.h
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

#ifndef LOMBO_SOUND_MIXER_H
#define LOMBO_SOUND_MIXER_H

#include <stdlib.h>

typedef enum {
	LOMBO_SOUND_GET_VOLUME,
	LOMBO_SOUND_SET_VOLUME,
} sound_mixer_status_t;

int lombo_sound_mixer(sound_mixer_status_t smt, int *value);

#endif /* LOMBO_SOUND_MIXER_H */

