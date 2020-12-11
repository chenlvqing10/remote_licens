/*
* mixer.h
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

/** @file */

/** @defgroup libtinyalsa-mixer Mixer Interface
 * @brief All macros, structures and functions that make up the mixer interface.
 */

#ifndef TINYALSA_MIXER_H
#define TINYALSA_MIXER_H

#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* TLV header size*/
#define TLV_HEADER_SIZE (2 * sizeof(unsigned int))

struct mixer;

struct mixer_ctl;

/** Mixer control type.
 * @ingroup libtinyalsa-mixer
 */
enum mixer_ctl_type {
    /** boolean control type */
    MIXER_CTL_TYPE_BOOL,
    /** integer control type */
    MIXER_CTL_TYPE_INT,
    /** an enumerated control type */
    MIXER_CTL_TYPE_ENUM,
    MIXER_CTL_TYPE_BYTE,
    MIXER_CTL_TYPE_IEC958,
    /** a 64 bit integer control type */
    MIXER_CTL_TYPE_INT64,
    /** unknown control type */
    MIXER_CTL_TYPE_UNKNOWN,
    /** end of the enumeration (not a control type) */
    MIXER_CTL_TYPE_MAX,
};

struct mixer *mixer_open(unsigned int card);

void mixer_close(struct mixer *mixer);

int mixer_add_new_ctls(struct mixer *mixer);

const char *mixer_get_name(const struct mixer *mixer);

unsigned int mixer_get_num_ctls(const struct mixer *mixer);

unsigned int mixer_get_num_ctls_by_name(const struct mixer *mixer, const char *name);

const struct mixer_ctl *mixer_get_ctl_const(const struct mixer *mixer, unsigned int id);

struct mixer_ctl *mixer_get_ctl(struct mixer *mixer, unsigned int id);

struct mixer_ctl *mixer_get_ctl_by_name(struct mixer *mixer, const char *name);

struct mixer_ctl *mixer_get_ctl_by_name_and_index(struct mixer *mixer,
                                                  const char *name,
                                                  unsigned int index);

int mixer_subscribe_events(struct mixer *mixer, int subscribe);

int mixer_wait_event(struct mixer *mixer, int timeout);

unsigned int mixer_ctl_get_id(const struct mixer_ctl *ctl);

const char *mixer_ctl_get_name(const struct mixer_ctl *ctl);

enum mixer_ctl_type mixer_ctl_get_type(const struct mixer_ctl *ctl);

const char *mixer_ctl_get_type_string(const struct mixer_ctl *ctl);

unsigned int mixer_ctl_get_num_values(const struct mixer_ctl *ctl);

unsigned int mixer_ctl_get_num_enums(const struct mixer_ctl *ctl);

const char *mixer_ctl_get_enum_string(struct mixer_ctl *ctl, unsigned int enum_id);

/* Some sound cards update their controls due to external events,
 * such as HDMI EDID byte data changing when an HDMI cable is
 * connected. This API allows the count of elements to be updated.
 */
void mixer_ctl_update(struct mixer_ctl *ctl);

int mixer_ctl_is_access_tlv_rw(const struct mixer_ctl *ctl);

/* Set and get mixer controls */
int mixer_ctl_get_percent(const struct mixer_ctl *ctl, unsigned int id);

int mixer_ctl_set_percent(struct mixer_ctl *ctl, unsigned int id, int percent);

int mixer_ctl_get_value(const struct mixer_ctl *ctl, unsigned int id);

int mixer_ctl_get_array(const struct mixer_ctl *ctl, void *array, size_t count);

int mixer_ctl_set_value(struct mixer_ctl *ctl, unsigned int id, int value);

int mixer_ctl_set_array(struct mixer_ctl *ctl, const void *array, size_t count);

int mixer_ctl_set_enum_by_string(struct mixer_ctl *ctl, const char *string);

/* Determe range of integer mixer controls */
int mixer_ctl_get_range_min(const struct mixer_ctl *ctl);

int mixer_ctl_get_range_max(const struct mixer_ctl *ctl);

#if defined(__cplusplus)
}  /* extern "C" */
#endif

#endif

