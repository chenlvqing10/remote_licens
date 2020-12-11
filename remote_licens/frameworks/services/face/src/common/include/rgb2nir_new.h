/*
 * rgb2nir.h - code for rotate component.
 *
 * Copyright (C) 2019-2020, LomboTech Co.Ltd.
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

#ifndef __RGB2NIR_NEW_H__
#define __RGB2NIR_NEW_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

	struct point_t {
		double x;
		double y;
	};

	struct img_point_t {
		struct img_point_t *next;
		int count;
		struct point_t *point; /* need free by myself */
	};

	struct cal_img_t {
		char *path;
		int row;
		int col;
		double ratio; /* vec_grids_per_face */
		int x;
		int y;
		int w;
		int h;
		struct img_point_t *img_point; /* need free by myself */
	};

	struct map_t {
		double x_map[5];
		double y_map[5];
	};

	struct cal_iface_t {
		struct img_point_t *rgb_point; /* list head */
		struct img_point_t *nir_point; /* list head */

		int(*get_point_from)(struct cal_img_t *cal_img);
		int(*add_rgb_point)(struct img_point_t *img_point);
		int(*add_nir_point)(struct img_point_t *img_point);

		int(*take_ord_pic)(void);
		int(*take_mod_pic)(int x, int y, int w, int h);

		int(*cal_ord_map)(struct map_t *map);
		int(*cal_mod_map)(struct map_t *map);
		int(*cal_reset_point)(void);
	};

	extern struct cal_iface_t *cal_iface;

	int r2n_test_pic(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __RGB2NIR_NEW_H__ */
