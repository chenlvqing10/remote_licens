/*
* lombo_sound_mixer.c
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

#include <mixer.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#include <lombo_sound_mixer.h>

#define LOG_TAG "SOUND_MIXER"
#include <log/log.h>

#if 0
static void tinymix_list_controls(struct mixer *mixer, int print_all);
#endif

static void tinymix_detail_control(struct mixer *mixer, const char *control);

static void tinymix_set_value(struct mixer *mixer, const char *control,
                              char **values, unsigned int num_values);

static void tinymix_print_enum(struct mixer_ctl *ctl);

#if 0
void usage(void)
{
	ALOGD("usage: tinymix [options] <command>\n");
	ALOGD("options:\n");
	ALOGD("\t-h, --help        : prints this help message and exists\n");
	ALOGD("\t-v, --version     : prints this version of tinymix and exists\n");
	ALOGD("\t-D, --card NUMBER : specifies the card number of the mixer\n");
	ALOGD("commands:\n");
	ALOGD("\tget NAME|ID       : prints the values of a control\n");
	ALOGD("\tset NAME|ID VALUE : sets the value of a control\n");
	ALOGD("\tcontrols          : lists controls of the mixer\n");
	ALOGD("\tcontents          : lists controls of the mixer and their contents\n");
}

void version(void)
{
	ALOGD("tinymix version 2.0 (tinyalsa version %s)\n", TINYALSA_VERSION_STRING);
}
#endif

int lombo_sound_mixer(sound_mixer_status_t smt, int *value)
{
	struct mixer *mixer;
	int card = 0;
	const char *volume = "6";
	char data[10];
	char *val = data;

	mixer = mixer_open(card);
	if (!mixer) {
		ALOGE("Failed to open mixer\n");
		return -1;
	}

	if (smt == LOMBO_SOUND_GET_VOLUME) {
		tinymix_detail_control(mixer, volume);
		ALOGD("\n");
	} else if (smt == LOMBO_SOUND_SET_VOLUME) {
		sprintf(val, "%d", *value);
		/* ALOGD("val:%s\n", val); */
		tinymix_set_value(mixer, volume, &val, 1);
	} else
		return -2;

	mixer_close(mixer);

	return 0;
}

static void tinymix_print_enum(struct mixer_ctl *ctl)
{
	unsigned int num_enums;
	unsigned int i;
	const char *string;

	num_enums = mixer_ctl_get_num_enums(ctl);

	for (i = 0; i < num_enums; i++) {
		string = mixer_ctl_get_enum_string(ctl, i);
		ALOGD("%s%s", mixer_ctl_get_value(ctl, 0) == (int)i ? ", " : "",
		string);
	}
}

static void tinymix_detail_control(struct mixer *mixer, const char *control)
{
	struct mixer_ctl *ctl;
	enum mixer_ctl_type type;
	unsigned int num_values;
	unsigned int i;
	int min, max;
	int ret;
	char *buf = NULL;
	unsigned int tlv_header_size = 0;

	if (isdigit(control[0]))
		ctl = mixer_get_ctl(mixer, atoi(control));
	else
		ctl = mixer_get_ctl_by_name(mixer, control);

	if (!ctl) {
		ALOGE("Invalid mixer control\n");
		return;
	}

	type = mixer_ctl_get_type(ctl);
	num_values = mixer_ctl_get_num_values(ctl);

	if ((type == MIXER_CTL_TYPE_BYTE) && (num_values > 0)) {
		if (mixer_ctl_is_access_tlv_rw(ctl) != 0) {
			tlv_header_size = TLV_HEADER_SIZE;
		}
		buf = calloc(1, num_values + tlv_header_size);
		if (buf == NULL) {
			ALOGE("Failed to alloc mem for bytes %u\n", num_values);
			return;
		}

		ret = mixer_ctl_get_array(ctl, buf, num_values + tlv_header_size);
		if (ret < 0) {
			ALOGE("Failed to mixer_ctl_get_array\n");
			free(buf);
			return;
		}
	}

	ALOGD("Mixer control type[%d] [%d]\n", type, num_values);

	for (i = 0; i < num_values; i++) {
		switch (type)
		{
		case MIXER_CTL_TYPE_INT:
			ALOGD("%d", mixer_ctl_get_value(ctl, i));
			break;
		case MIXER_CTL_TYPE_BOOL:
			ALOGD("%s", mixer_ctl_get_value(ctl, i) ? "On" : "Off");
			break;
		case MIXER_CTL_TYPE_ENUM:
			tinymix_print_enum(ctl);
			break;
		case MIXER_CTL_TYPE_BYTE:
			/* skip printing TLV header if exists */
			ALOGD(" %02x", buf[i + tlv_header_size]);
		break;
			default:
			ALOGD("unknown");
		break;
		};
		if ((i + 1) < num_values) {
			ALOGD(", ");
		}
	}

	if (type == MIXER_CTL_TYPE_INT) {
		min = mixer_ctl_get_range_min(ctl);
		max = mixer_ctl_get_range_max(ctl);
		ALOGD(" (range %d->%d)", min, max);
	}

	free(buf);
}

static void tinymix_set_byte_ctl(struct mixer_ctl *ctl,
    char **values, unsigned int num_values)
{
	int ret;
	char *buf;
	char *end;
	unsigned int i;
	long n;
	unsigned int *tlv, tlv_size;
	unsigned int tlv_header_size = 0;

	if (mixer_ctl_is_access_tlv_rw(ctl) != 0) {
		tlv_header_size = TLV_HEADER_SIZE;
	}

	tlv_size = num_values + tlv_header_size;

	buf = calloc(1, tlv_size);
	if (buf == NULL) {
		ALOGE("set_byte_ctl: Failed to alloc mem for bytes %u\n", num_values);
		exit(EXIT_FAILURE);
	}

	tlv = (unsigned int *)buf;
	tlv[0] = 0;
	tlv[1] = num_values;

	for (i = 0; i < num_values; i++) {
		errno = 0;
		n = strtol(values[i], &end, 0);
		if (*end) {
			ALOGE("%s not an integer\n", values[i]);
			goto fail;
		}
		if (errno) {
			ALOGE("strtol: %s: %s\n", values[i],
			strerror(errno));
			goto fail;
		}
		if (n < 0 || n > 0xff) {
			ALOGE("%s should be between [0, 0xff]\n",
			values[i]);
			goto fail;
		}
		/* start filling after tlv header */
		buf[i + tlv_header_size] = n;
	}

	ret = mixer_ctl_set_array(ctl, buf, tlv_size);
	if (ret < 0) {
		ALOGE("Failed to set binary control\n");
		goto fail;
	}

	free(buf);
	return;

	fail:
	free(buf);
	exit(EXIT_FAILURE);
}

static int is_int(char *value)
{
	char* end;
	long int result;

	errno = 0;
	result = strtol(value, &end, 10);

	if (result == LONG_MIN || result == LONG_MAX)
		return 0;

	return errno == 0 && *end == '\0';
}

static void tinymix_set_value(struct mixer *mixer, const char *control,
                              char **values, unsigned int num_values)
{
	struct mixer_ctl *ctl;
	enum mixer_ctl_type type;
	unsigned int num_ctl_values;
	unsigned int i;

	if (isdigit(control[0]))
		ctl = mixer_get_ctl(mixer, atoi(control));
	else
		ctl = mixer_get_ctl_by_name(mixer, control);

	if (!ctl) {
		ALOGE("Invalid mixer control\n");
		return;
	}

	type = mixer_ctl_get_type(ctl);
	num_ctl_values = mixer_ctl_get_num_values(ctl);

	if (type == MIXER_CTL_TYPE_BYTE) {
		tinymix_set_byte_ctl(ctl, values, num_values);
		return;
	}

	if (is_int(values[0])) {
		if (num_values == 1) {
		/* Set all values the same */
		int value = atoi(values[0]);

			for (i = 0; i < num_ctl_values; i++) {
				if (mixer_ctl_set_value(ctl, i, value)) {
					ALOGE("Error: invalid value\n");
					return;
				}
			}
		} else {
			/* Set multiple values */
			if (num_values > num_ctl_values) {
				fprintf(stderr,
				"Error: %u values given, but control only takes %u\n",
				num_values, num_ctl_values);
				return;
			}
			for (i = 0; i < num_values; i++) {
				if (mixer_ctl_set_value(ctl, i, atoi(values[i]))) {
					ALOGE("Error: invalid value for index %u\n", i);
					return;
				}
			}
		}
	} else {
		if (type == MIXER_CTL_TYPE_ENUM) {
			if (num_values != 1) {
			ALOGE("Enclose strings in quotes and try again\n");
			return;
		}
		if (mixer_ctl_set_enum_by_string(ctl, values[0]))
			ALOGE("Error: invalid enum value\n");
		} else {
			ALOGE("Error: only enum types can be set with strings\n");
		}
	}
}

